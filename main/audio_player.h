// main/audio_player.h
// Yakka Dee 口袋开口机 —— 异步音频播放与麦克风侦听任务
#pragma once

#include "audio_energy.h"
#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*yakka_voice_detected_cb_t)(void *user);

// 初始化音频播放器与麦克风任务
esp_err_t audio_player_init(uint8_t default_volume);

// 销毁音频任务
void audio_player_deinit(void);

// 异步播放一段音频 (支持 16-bit PCM 或 4-bit IMA-ADPCM, 非阻塞)
// is_adpcm: true 表示数据为 IMA-ADPCM 压缩音频，false 表示 raw PCM
// interrupt: 是否打断当前正在播放的内容
void audio_player_play(const void *audio, size_t bytes, bool is_adpcm, bool interrupt);

// 播放萌系轻柔按键啵啵声 (高优先级独立短音，10% 音量触觉反馈)
void audio_player_play_pop(const void *audio, size_t bytes, bool is_adpcm);

// 停止当前所有播放
void audio_player_stop(void);

// 开启麦克风侦听
void audio_player_start_mic(yakka_voice_detected_cb_t cb, void *user, audio_sens_t sens);

// 停止麦克风侦听
void audio_player_stop_mic(void);

// 动态调整灵敏度
void audio_player_set_sens(audio_sens_t sens);

// 动态设置音量 (0..100)
void audio_player_set_volume(uint8_t percent);

// 进入低功耗前停止所有 I2S 传输并让 codec 休眠
void audio_player_sleep(void);

// 从休眠恢复
void audio_player_wake(void);

#ifdef __cplusplus
}
#endif
