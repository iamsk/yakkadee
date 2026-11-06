// main/audio_energy.c
// Yakka Dee 口袋开口机 —— 麦克风 I2S RMS 能量峰值检测算法实现
#include "audio_energy.h"
#include <math.h>
#include <string.h>

#define DEFAULT_MIN_TRIGGER_MS 200
#define INITIAL_NOISE_FLOOR    200.0f

void audio_energy_init(audio_energy_detector_t *det, uint32_t sample_rate, audio_sens_t sens) {
    if (!det) return;
    det->sample_rate = sample_rate ? sample_rate : 16000;
    det->noise_floor_rms = INITIAL_NOISE_FLOOR;
    det->consecutive_over_ms = 0;
    det->min_trigger_ms = DEFAULT_MIN_TRIGGER_MS;
    det->sensitivity = sens;
}

void audio_energy_set_sens(audio_energy_detector_t *det, audio_sens_t sens) {
    if (!det) return;
    det->sensitivity = sens;
}

void audio_energy_reset_trigger(audio_energy_detector_t *det) {
    if (!det) return;
    det->consecutive_over_ms = 0;
}

float audio_energy_calc_rms(const int16_t *samples, size_t count) {
    if (!samples || count == 0) return 0.0f;
    uint64_t sum_sq = 0;
    for (size_t i = 0; i < count; i++) {
        int32_t v = (int32_t)samples[i];
        sum_sq += (uint64_t)(v * v);
    }
    double mean_sq = (double)sum_sq / (double)count;
    return (float)sqrt(mean_sq);
}

static float get_threshold(const audio_energy_detector_t *det) {
    float base_thresh;
    float multiplier;
    switch (det->sensitivity) {
        case AUDIO_SENS_HIGH:
            base_thresh = 600.0f;
            multiplier = 1.4f;
            break;
        case AUDIO_SENS_LOW:
            base_thresh = 1800.0f;
            multiplier = 2.4f;
            break;
        case AUDIO_SENS_MED:
        default:
            base_thresh = 1100.0f;
            multiplier = 1.8f;
            break;
    }
    float dynamic_thresh = det->noise_floor_rms * multiplier + (base_thresh * 0.4f);
    return dynamic_thresh > base_thresh ? dynamic_thresh : base_thresh;
}

bool audio_energy_feed(audio_energy_detector_t *det, const int16_t *samples, size_t count, float *out_rms) {
    if (!det || !samples || count == 0) return false;

    float rms = audio_energy_calc_rms(samples, count);
    if (out_rms) *out_rms = rms;

    uint32_t chunk_ms = (uint32_t)((count * 1000ULL) / det->sample_rate);
    if (chunk_ms == 0) chunk_ms = 1;

    float threshold = get_threshold(det);

    if (rms >= threshold) {
        det->consecutive_over_ms += chunk_ms;
        if (det->consecutive_over_ms >= det->min_trigger_ms) {
            det->consecutive_over_ms = 0; // 触发后重置
            return true;
        }
    } else {
        // 未超阈值: 自适应底噪平滑跟踪 (仅在较安静时缓慢更新)
        if (rms < threshold * 0.9f) {
            det->noise_floor_rms = det->noise_floor_rms * 0.96f + rms * 0.04f;
            if (det->noise_floor_rms < 50.0f) det->noise_floor_rms = 50.0f;
        }
        // 允许单个微弱掉帧容差 (衰减而非瞬间归零)
        if (det->consecutive_over_ms > chunk_ms) {
            det->consecutive_over_ms -= chunk_ms;
        } else {
            det->consecutive_over_ms = 0;
        }
    }

    return false;
}
