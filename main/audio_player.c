// main/audio_player.c
// Yakka Dee 口袋开口机 —— 异步音频播放与麦克风侦听任务实现
#include "audio_player.h"
#include "bsp_audio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "audio_player";

#define AUDIO_CHUNK_SAMPLES 512
#define AUDIO_SAMPLE_RATE   16000

typedef struct {
    const uint8_t *data;
    size_t size;
    bool is_pop;
} audio_play_req_t;

static TaskHandle_t s_play_task = NULL;
static TaskHandle_t s_mic_task = NULL;
static QueueHandle_t s_play_queue = NULL;

static volatile bool s_cancel_play = false;
static volatile bool s_is_playing = false;

static volatile bool s_mic_active = false;
static yakka_voice_detected_cb_t s_voice_cb = NULL;
static void *s_voice_user = NULL;
static audio_energy_detector_t s_detector;

static uint8_t s_current_volume = 60;

static void audio_play_task(void *arg) {
    (void)arg;
    int16_t chunk[AUDIO_CHUNK_SAMPLES];

    for (;;) {
        audio_play_req_t req;
        if (xQueueReceive(s_play_queue, &req, portMAX_DELAY) != pdTRUE) continue;

        if (!req.data || req.size == 0) continue;

        s_cancel_play = false;
        s_is_playing = true;

        if (req.is_pop) {
            // 水滴轻柔音量 10%
            bsp_audio_set_volume(10);
        } else {
            bsp_audio_set_volume(s_current_volume);
        }

        size_t offset = 0;
        size_t total = req.size;
        const int16_t *src = (const int16_t *)req.data;
        size_t total_samples = total / sizeof(int16_t);

        while (offset < total_samples && !s_cancel_play) {
            size_t n = (total_samples - offset) < AUDIO_CHUNK_SAMPLES ? (total_samples - offset) : AUDIO_CHUNK_SAMPLES;
            memcpy(chunk, src + offset, n * sizeof(int16_t));
            esp_err_t err = bsp_audio_write(chunk, n * sizeof(int16_t));
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "bsp_audio_write 失败: %s", esp_err_to_name(err));
                break;
            }
            offset += n;
        }

        s_is_playing = false;
        if (req.is_pop) {
            // 恢复正常音量
            bsp_audio_set_volume(s_current_volume);
        }
    }
}

static void audio_mic_task(void *arg) {
    (void)arg;
    int16_t rec_buf[AUDIO_CHUNK_SAMPLES];

    for (;;) {
        if (!s_mic_active) {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        // 侦听读取 512 采样 (32ms)
        esp_err_t err = bsp_audio_read(rec_buf, sizeof(rec_buf));
        if (err != ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        if (!s_mic_active) continue;

        float rms = 0.0f;
        if (audio_energy_feed(&s_detector, rec_buf, AUDIO_CHUNK_SAMPLES, &rms)) {
            ESP_LOGI(TAG, "检测到宝宝发声! RMS=%.1f", rms);
            if (s_voice_cb) {
                s_voice_cb(s_voice_user);
            }
        }
    }
}

esp_err_t audio_player_init(uint8_t default_volume) {
    s_current_volume = default_volume > 80 ? 80 : (default_volume < 20 ? 20 : default_volume);

    esp_err_t err = bsp_audio_set_format(AUDIO_SAMPLE_RATE, 16, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "设置音频格式 16k 16bit 单声道失败: %s", esp_err_to_name(err));
        return err;
    }
    bsp_audio_set_volume(s_current_volume);

    audio_energy_init(&s_detector, AUDIO_SAMPLE_RATE, AUDIO_SENS_MED);

    if (!s_play_queue) {
        s_play_queue = xQueueCreate(4, sizeof(audio_play_req_t));
        if (!s_play_queue) return ESP_ERR_NO_MEM;
    }

    if (!s_play_task) {
        if (xTaskCreate(audio_play_task, "yakka_audio_play", 3072, NULL, 5, &s_play_task) != pdPASS) {
            return ESP_ERR_NO_MEM;
        }
    }

    if (!s_mic_task) {
        if (xTaskCreate(audio_mic_task, "yakka_audio_mic", 3072, NULL, 5, &s_mic_task) != pdPASS) {
            return ESP_ERR_NO_MEM;
        }
    }

    return ESP_OK;
}

void audio_player_play(const void *pcm, size_t bytes, bool interrupt) {
    if (!pcm || bytes == 0 || !s_play_queue) return;

    if (interrupt && s_is_playing) {
        s_cancel_play = true;
        xQueueReset(s_play_queue);
        vTaskDelay(pdMS_TO_TICKS(15));
    }

    audio_play_req_t req = {
        .data = (const uint8_t *)pcm,
        .size = bytes,
        .is_pop = false,
    };
    xQueueSend(s_play_queue, &req, 0);
}

void audio_player_play_pop(const void *pcm, size_t bytes) {
    if (!pcm || bytes == 0 || !s_play_queue) return;
    // 只有在空闲或允许插队时快速发出轻柔啵啵声
    if (xQueueReceive(s_play_queue, &(audio_play_req_t){0}, 0) == pdTRUE) {
        // dropped stale item
    }
    audio_play_req_t req = {
        .data = (const uint8_t *)pcm,
        .size = bytes,
        .is_pop = true,
    };
    xQueueSend(s_play_queue, &req, 0);
}

void audio_player_stop(void) {
    s_cancel_play = true;
    if (s_play_queue) xQueueReset(s_play_queue);
}

void audio_player_start_mic(yakka_voice_detected_cb_t cb, void *user, audio_sens_t sens) {
    s_voice_cb = cb;
    s_voice_user = user;
    audio_energy_set_sens(&s_detector, sens);
    audio_energy_reset_trigger(&s_detector);
    s_mic_active = true;
}

void audio_player_stop_mic(void) {
    s_mic_active = false;
    audio_energy_reset_trigger(&s_detector);
}

void audio_player_set_sens(audio_sens_t sens) {
    audio_energy_set_sens(&s_detector, sens);
}

void audio_player_set_volume(uint8_t percent) {
    s_current_volume = percent > 80 ? 80 : (percent < 20 ? 20 : percent);
    bsp_audio_set_volume(s_current_volume);
}

void audio_player_sleep(void) {
    audio_player_stop();
    audio_player_stop_mic();
    vTaskDelay(pdMS_TO_TICKS(20)); // 等待当前 PCM 读写退出
    bsp_audio_sleep();
}

void audio_player_wake(void) {
    bsp_audio_wake();
    bsp_audio_set_format(AUDIO_SAMPLE_RATE, 16, 1);
    bsp_audio_set_volume(s_current_volume);
}

void audio_player_deinit(void) {
    audio_player_sleep();
    if (s_play_task) {
        vTaskDelete(s_play_task);
        s_play_task = NULL;
    }
    if (s_mic_task) {
        vTaskDelete(s_mic_task);
        s_mic_task = NULL;
    }
    if (s_play_queue) {
        vQueueDelete(s_play_queue);
        s_play_queue = NULL;
    }
}
