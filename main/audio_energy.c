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
            base_thresh = 280.0f;
            multiplier = 1.3f;
            break;
        case AUDIO_SENS_LOW:
            base_thresh = 1000.0f;
            multiplier = 2.0f;
            break;
        case AUDIO_SENS_MED:
        default:
            base_thresh = 450.0f;
            multiplier = 1.5f;
            break;
    }
    float dynamic_thresh = det->noise_floor_rms * multiplier + (base_thresh * 0.4f);
    return dynamic_thresh > base_thresh ? dynamic_thresh : base_thresh;
}

// 检验声学多维特征是否符合人类/幼儿真实发音 (Voice Activity Detection)
static bool is_human_voice_features(const int16_t *samples, size_t count, float rms) {
    if (!samples || count < 128 || rms < 100.0f) return false;

    // 1. 峰均比 (Crest Factor) 判定: 剔除尖锐瞬态冲击音 (拍桌、敲击、咳嗽、拍手)
    int16_t peak = 0;
    for (size_t i = 0; i < count; i++) {
        int16_t abs_v = samples[i] < 0 ? (int16_t)(-samples[i]) : samples[i];
        if (abs_v > peak) peak = abs_v;
    }
    float crest_factor = (float)peak / (rms + 1.0f);
    // 人声元音平滑波形通常为 1.2 ~ 4.2；拍桌子/敲击刺脉冲通常 > 4.5
    if (crest_factor < 1.05f || crest_factor > 5.5f) {
        return false;
    }

    // 2. 过零率 (Zero-Crossing Rate) 判定:
    // 512 采样 @ 16kHz (32ms):
    // 人声频段集中在 250Hz ~ 3.5kHz，过零率稳定在 6 ~ 125 之间
    // 低频空调/机器轰鸣与桌面震动 ZCR < 6；高频摩擦沙沙声与白噪声 ZCR > 130
    uint32_t zcr = 0;
    for (size_t i = 1; i < count; i++) {
        if ((samples[i] >= 0 && samples[i - 1] < 0) || (samples[i] < 0 && samples[i - 1] >= 0)) {
            zcr++;
        }
    }
    if (zcr < 4 || zcr > 135) {
        return false;
    }

    // 3. 幼儿声带基频周期性 (Harmonic Pitch Autocorrelation) 判定:
    // 幼儿发音声带基频在 200Hz ~ 600Hz 之间 (16kHz 下对应 lag = 26 .. 80 个采样点)
    size_t max_calc = count > 160 ? (count - 160) : 0;
    if (max_calc == 0) return false;

    float r0_slice = 0.0f;
    for (size_t i = 0; i < max_calc; i += 2) {
        float v = (float)samples[i];
        r0_slice += v * v;
    }
    if (r0_slice < 500.0f) return false;

    float max_autocorr = 0.0f;
    for (size_t lag = 20; lag <= 160; lag += 2) {
        float r_lag = 0.0f;
        for (size_t i = 0; i < max_calc; i += 2) {
            r_lag += (float)samples[i] * (float)samples[i + lag];
        }
        float norm_r = r_lag / r0_slice;
        if (norm_r > max_autocorr) {
            max_autocorr = norm_r;
        }
    }

    // 人声在基频处具有强烈的周期相关性 (通常 > 0.40)；摩擦、敲击、环境杂音无相关性 (通常 < 0.20)
    return (max_autocorr >= 0.28f);
}

bool audio_energy_feed(audio_energy_detector_t *det, const int16_t *samples, size_t count, float *out_rms) {
    if (!det || !samples || count == 0) return false;

    float rms = audio_energy_calc_rms(samples, count);
    if (out_rms) *out_rms = rms;

    uint32_t chunk_ms = (uint32_t)((count * 1000ULL) / det->sample_rate);
    if (chunk_ms == 0) chunk_ms = 1;

    float threshold = get_threshold(det);

    if (rms >= threshold && is_human_voice_features(samples, count, rms)) {
        det->consecutive_over_ms += chunk_ms;
        if (det->consecutive_over_ms >= det->min_trigger_ms) {
            det->consecutive_over_ms = 0; // 触发后重置
            return true;
        }
    } else {
        // 未超阈值或非人声环境音: 自适应底噪平滑跟踪 (仅在较安静时缓慢更新)
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
