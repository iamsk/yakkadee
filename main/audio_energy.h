// main/audio_energy.h
// Yakka Dee 口袋开口机 —— 麦克风 I2S RMS 能量峰值检测算法
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_SENS_LOW = 0,
    AUDIO_SENS_MED = 1,
    AUDIO_SENS_HIGH = 2,
} audio_sens_t;

typedef struct {
    uint32_t sample_rate;         // 默认 16000 Hz
    float noise_floor_rms;        // 自适应环境底噪估计 (RMS)
    uint32_t consecutive_over_ms; // 能量持续超过阈值的毫秒数
    uint32_t min_trigger_ms;      // 判定有效发声的最小持续时间 (默认 120ms)
    audio_sens_t sensitivity;     // 灵敏度档位
} audio_energy_detector_t;

// 初始化检测器
void audio_energy_init(audio_energy_detector_t *det, uint32_t sample_rate, audio_sens_t sens);

// 设置灵敏度
void audio_energy_set_sens(audio_energy_detector_t *det, audio_sens_t sens);

// 计算 16-bit PCM 单声道缓冲区的 RMS 能量
float audio_energy_calc_rms(const int16_t *samples, size_t count);

// 处理一段音频样本块并进行自适应检测
// 返回 true 表示成功检测到幼儿有效发声 (持续 >= 120ms)
bool audio_energy_feed(audio_energy_detector_t *det, const int16_t *samples, size_t count, float *out_rms);

// 重置侦听状态 (在开启麦克风侦听时调用)
void audio_energy_reset_trigger(audio_energy_detector_t *det);

#ifdef __cplusplus
}
#endif
