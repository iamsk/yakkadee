// main/main.c
// Yakka Dee 口袋开口机 —— 主程序入口与事件循环调度
#include "audio_player.h"
#include "bsp_audio.h"
#include "bsp_battery.h"
#include "bsp_button.h"
#include "bsp_display.h"
#include "bsp_i2c.h"
#include "bsp_pins.h"
#include "key_guard.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "ui_yakka.h"
#include "yakka_assets.h"
#include "yakka_state.h"

static const char *TAG = "yakka_dee";

#define EVENT_QUEUE_DEPTH 16
#define EYE_CARE_BACKLIGHT_PERCENT 25  // 严格锁死 25% 护眼柔和背光 (PRD 6.4)
#define KEY_GUARD_MAX_US (10LL * 1000 * 1000)

typedef enum {
    EV_TICK,
    EV_BUTTON,
    EV_VOICE,
} event_type_t;

typedef struct {
    event_type_t type;
    int btn;
    bool is_press;
    bool is_click;
    bool is_long;
} app_event_t;

static yakka_machine_t s_machine;
static QueueHandle_t s_event_queue = NULL;
static key_guard_t s_key_guard;
static volatile bool s_input_ready = false;

static void on_key(bsp_btn_t btn, bsp_btn_ev_t ev, void *user) {
    (void)user;
    const bool was_armed = s_key_guard.armed;
    if (key_guard_consume(&s_key_guard, ev == BSP_BTN_RELEASE, esp_timer_get_time())) {
        if (was_armed && !s_key_guard.armed) {
            ESP_LOGI(TAG, "唤醒键已松手,按键恢复响应");
        }
        return;
    }
    if (!s_input_ready || !s_event_queue) return;

    app_event_t event;
    event.type = EV_BUTTON;
    event.btn = (int)btn;
    event.is_press = (ev == BSP_BTN_PRESS);
    event.is_click = (ev == BSP_BTN_CLICK);
    event.is_long = (ev == BSP_BTN_LONG);

    // 捕获 PRESS、CLICK、LONG 以及 RELEASE
    if (ev == BSP_BTN_PRESS || ev == BSP_BTN_CLICK || ev == BSP_BTN_LONG || ev == BSP_BTN_RELEASE) {
        if (ev == BSP_BTN_RELEASE) {
            event.is_press = false;
            event.is_click = false;
            event.is_long = false;
        }
        (void)xQueueSend(s_event_queue, &event, 0);
    }
}

static void on_voice_detected_cb(void *user) {
    (void)user;
    if (!s_event_queue) return;
    app_event_t event = { .type = EV_VOICE };
    (void)xQueueSend(s_event_queue, &event, 0);
}

static const char *state_to_str(yakka_state_t st) {
    switch (st) {
        case YAKKA_STATE_BOOT_INTRO: return "BOOT_INTRO";
        case YAKKA_STATE_VOCAB_INTRO: return "VOCAB_INTRO";
        case YAKKA_STATE_CONTRAST_1: return "CONTRAST_1";
        case YAKKA_STATE_CONTRAST_2: return "CONTRAST_2";
        case YAKKA_STATE_CONTRAST_3: return "CONTRAST_3";
        case YAKKA_STATE_QUESTION: return "QUESTION";
        case YAKKA_STATE_SUCCESS: return "SUCCESS";
        case YAKKA_STATE_ENCOURAGE_RETRY: return "ENCOURAGE_RETRY";
        case YAKKA_STATE_NAP_TIME: return "NAP_TIME";
        case YAKKA_STATE_PARENT_MENU: return "PARENT_MENU";
        default: return "UNKNOWN";
    }
}

static void execute_actions(uint32_t actions, int battery_soc, int battery_mv) {
    if (actions == 0) return;

    static yakka_state_t s_prev_state = YAKKA_STATE_BOOT_INTRO;
    if (s_machine.state != s_prev_state) {
        ESP_LOGI(TAG, "状态流转: %s -> %s (词汇: %d)", state_to_str(s_prev_state), state_to_str(s_machine.state), (int)s_machine.current_vocab);
        s_prev_state = s_machine.state;
    }

    // 1. 背光控制 (睡午觉微光夜灯 vs 正常护眼亮度)
    if (actions & (1u << YAKKA_ACTION_SET_BACKLIGHT)) {
        if (s_machine.state == YAKKA_STATE_NAP_TIME && s_machine.nap_screen_dimmed) {
            ESP_LOGI(TAG, "Dee 睡午觉，开启 10%% 微光安抚夜灯");
            bsp_display_backlight(10);
        } else {
            bsp_display_backlight(s_machine.backlight_percent);
        }
    }

    // 2. 麦克风控制
    if (actions & (1u << YAKKA_ACTION_START_MIC)) {
        audio_sens_t sens = (audio_sens_t)s_machine.mic_sensitivity;
        audio_player_start_mic(on_voice_detected_cb, NULL, sens);
    }
    if (actions & (1u << YAKKA_ACTION_STOP_MIC)) {
        audio_player_stop_mic();
    }

    // 3. 音效播放
    if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_POP)) {
        yakka_pcm_asset_t a = yakka_asset_audio_pop();
        audio_player_play_pop(a.data, a.size);
    } else {
        yakka_action_t sound_act = YAKKA_ACTION_NONE;
        if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_BOOT)) sound_act = YAKKA_ACTION_PLAY_AUDIO_BOOT;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO)) sound_act = YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)) sound_act = YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)) sound_act = YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)) sound_act = YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_QUESTION)) sound_act = YAKKA_ACTION_PLAY_AUDIO_QUESTION;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_SUCCESS)) sound_act = YAKKA_ACTION_PLAY_AUDIO_SUCCESS;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_RETRY)) sound_act = YAKKA_ACTION_PLAY_AUDIO_RETRY;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_SKIP)) sound_act = YAKKA_ACTION_PLAY_AUDIO_SKIP;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_NAP)) sound_act = YAKKA_ACTION_PLAY_AUDIO_NAP;
        else if (actions & (1u << YAKKA_ACTION_PLAY_AUDIO_PARENT_ON)) sound_act = YAKKA_ACTION_PLAY_AUDIO_PARENT_ON;

        if (sound_act != YAKKA_ACTION_NONE) {
            yakka_pcm_asset_t a = yakka_asset_get_audio(s_machine.current_vocab, sound_act);
            if (a.data && a.size > 0) {
                audio_player_play(a.data, a.size, true);
            }
        }
    }

    // 4. UI 界面刷新 (持 LVGL 锁)
    if (actions & (1u << YAKKA_ACTION_UPDATE_UI)) {
        if (bsp_lvgl_lock(500)) {
            ui_yakka_update(&s_machine, battery_soc, battery_mv);
            bsp_lvgl_unlock();
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "===========================================");
    ESP_LOGI(TAG, "  Yakka Dee 口袋开口机 (Pocket Talker) v1.0 ");
    ESP_LOGI(TAG, "  100%% 纯离线 / 零按键开机即玩 / 护眼暖色 UI  ");
    ESP_LOGI(TAG, "===========================================");

    // 1. 初始化硬件总线与屏幕
    const uint32_t wake_causes = esp_sleep_get_wakeup_causes();
    if (wake_causes & (1u << ESP_SLEEP_WAKEUP_GPIO)) {
        key_guard_arm(&s_key_guard, esp_timer_get_time(), KEY_GUARD_MAX_US);
        ESP_LOGI(TAG, "按键唤醒:先屏蔽按键动作,直到唤醒键松手");
    }

    bsp_i2c_init();

    if (bsp_display_init() != ESP_OK || !bsp_lvgl_init()) {
        ESP_LOGE(TAG, "显示屏/LVGL 初始化失败，无法启动");
        return;
    }

    // 2. 初始化音频与电池
    bsp_audio_init();
    bsp_battery_init();

    // 3. 初始化状态机
    uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);
    yakka_machine_init(&s_machine, now_ms);

    // 护眼底层光学控制: 初始舒适背光
    bsp_display_backlight(s_machine.backlight_percent);

    // 4. 初始化音频引擎
    audio_player_init(s_machine.volume);

    // 5. 初始化 Yakka 护眼图形界面
    if (bsp_lvgl_lock(1000)) {
        ui_yakka_init();
        ui_yakka_update(&s_machine, 100, 4100);
        bsp_lvgl_unlock();
    }

    // 6. 注册按键与唤醒守卫同步
    s_event_queue = xQueueCreate(EVENT_QUEUE_DEPTH, sizeof(app_event_t));
    if (bsp_button_init(on_key, NULL) != ESP_OK) {
        ESP_LOGW(TAG, "按键初始化失败或重叠");
    } else if (s_key_guard.armed) {
        const bool released = bsp_button_read_mv() >= BSP_BTN_MV_RELEASED_MIN;
        key_guard_sync_released(&s_key_guard, released);
    }
    s_input_ready = true;

    // 播报开机语音
    execute_actions(1u << YAKKA_ACTION_PLAY_AUDIO_BOOT, 100, 4100);

    // 7. 主事件与动画调度循环 (步长 40ms = 25 FPS)
    uint32_t last_tick_ms = now_ms;
    uint32_t last_bat_ms = now_ms;
    int bat_soc = 100;
    int bat_mv = 4100;

    for (;;) {
        app_event_t event;
        // 等待事件，最长 40ms 超时保证 25fps 动效平滑刷新
        if (xQueueReceive(s_event_queue, &event, pdMS_TO_TICKS(40)) == pdTRUE) {
            now_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);
            uint32_t actions = 0;

            if (event.type == EV_BUTTON) {
                actions = yakka_machine_on_button(&s_machine, event.btn, event.is_press, event.is_click, event.is_long, now_ms);
                // 同步音量设置
                audio_player_set_volume(s_machine.volume);
                audio_player_set_sens((audio_sens_t)s_machine.mic_sensitivity);
            } else if (event.type == EV_VOICE) {
                actions = yakka_machine_on_voice_detected(&s_machine, now_ms);
            }

            execute_actions(actions, bat_soc, bat_mv);
        }

        now_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);

        // 定期状态机 Tick (每 50ms)
        if (now_ms - last_tick_ms >= 50) {
            last_tick_ms = now_ms;
            uint32_t actions = yakka_machine_tick(&s_machine, now_ms);
            execute_actions(actions, bat_soc, bat_mv);
        }

        // 定期电量读取 (每 10 秒)
        if (now_ms - last_bat_ms >= 10000) {
            last_bat_ms = now_ms;
            bat_soc = bsp_battery_soc();
            bat_mv = bsp_battery_mv();
        }

        // 动效刷新 (25 FPS)
        if (s_machine.state == YAKKA_STATE_SUCCESS) {
            if (bsp_lvgl_lock(30)) {
                ui_yakka_confetti_step();
                bsp_lvgl_unlock();
            }
        } else if (s_machine.state == YAKKA_STATE_QUESTION || s_machine.state == YAKKA_STATE_ENCOURAGE_RETRY) {
            if (bsp_lvgl_lock(30)) {
                ui_yakka_pulse_step(now_ms);
                bsp_lvgl_unlock();
            }
        }
    }
}
