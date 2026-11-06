// tests/test_audio_energy.c
// 单元测试: RMS 能量计算、底噪自适应跟踪、持续 120ms 判定门限及抗脉冲误触
#include "audio_energy.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_rms_calculation(void) {
    // 1. 全零采样 -> RMS = 0
    int16_t zeros[512] = {0};
    float rms = audio_energy_calc_rms(zeros, 512);
    assert(fabsf(rms - 0.0f) < 0.001f);

    // 2. 直流幅度 1000 -> RMS = 1000
    int16_t dc[512];
    for (int i = 0; i < 512; i++) dc[i] = 1000;
    rms = audio_energy_calc_rms(dc, 512);
    assert(fabsf(rms - 1000.0f) < 0.1f);

    // 3. 正弦波峰值 A = 10000 -> RMS 约为 10000 / sqrt(2) ≈ 7071
    int16_t sine[1000];
    for (int i = 0; i < 1000; i++) {
        sine[i] = (int16_t)(10000.0 * sin(2.0 * M_PI * (double)i / 100.0));
    }
    rms = audio_energy_calc_rms(sine, 1000);
    assert(fabsf(rms - 7071.0f) < 50.0f);

    printf("test_rms_calculation: PASS\n");
}

static void test_energy_trigger_duration_gate(void) {
    audio_energy_detector_t det;
    audio_energy_init(&det, 16000, AUDIO_SENS_MED);

    // 每次处理 512 采样 @ 16kHz = 32ms
    int16_t quiet_buf[512];
    for (int i = 0; i < 512; i++) quiet_buf[i] = (int16_t)(i % 50 - 25); // 低底噪约 15

    int16_t loud_buf[512];
    for (int i = 0; i < 512; i++) loud_buf[i] = (int16_t)(3000 * sin(i * 0.1)); // 强能量 RMS 约 2120

    float cur_rms = 0.0f;

    // 1. 安静环境喂入 10 块 (320ms): 绝不触发
    for (int i = 0; i < 10; i++) {
        bool triggered = audio_energy_feed(&det, quiet_buf, 512, &cur_rms);
        assert(!triggered);
    }

    // 2. 短脉冲噪声 (仅 1 块 = 32ms): 不满足持续 >= 120ms 门限，绝不触发
    bool triggered = audio_energy_feed(&det, loud_buf, 512, &cur_rms);
    assert(!triggered);

    // 恢复安静 1 块
    triggered = audio_energy_feed(&det, quiet_buf, 512, &cur_rms);
    assert(!triggered);

    // 3. 持续发声 (连续喂入 7 块 = 224ms >= 200ms): 必须在持续达到门限后准确触发成功!
    for (int i = 0; i < 6; i++) {
        triggered = audio_energy_feed(&det, loud_buf, 512, &cur_rms);
        assert(!triggered);
    }
    triggered = audio_energy_feed(&det, loud_buf, 512, &cur_rms); // 第 7 块 (224ms) -> 触发!
    assert(triggered);

    printf("test_energy_trigger_duration_gate: PASS\n");
}

static void test_sensitivity_levels(void) {
    audio_energy_detector_t det;
    audio_energy_init(&det, 16000, AUDIO_SENS_HIGH);

    // 高灵敏度: 较轻发音 (RMS 约 850) 即可触发
    int16_t soft_buf[512];
    for (int i = 0; i < 512; i++) soft_buf[i] = (int16_t)(1200 * sin(i * 0.1)); // RMS 约 850
    float cur_rms;

    bool triggered = false;
    for (int i = 0; i < 7; i++) {
        triggered = audio_energy_feed(&det, soft_buf, 512, &cur_rms);
    }
    assert(triggered); // 高灵敏度应触发

    // 低灵敏度: 同样的发音不应触发
    audio_energy_init(&det, 16000, AUDIO_SENS_LOW);
    for (int i = 0; i < 7; i++) {
        triggered = audio_energy_feed(&det, soft_buf, 512, &cur_rms);
    }
    assert(!triggered); // 低灵敏度不触发

    printf("test_sensitivity_levels: PASS\n");
}

int main(void) {
    test_rms_calculation();
    test_energy_trigger_duration_gate();
    test_sensitivity_levels();
    printf("ALL AUDIO ENERGY TESTS PASSED!\n");
    return 0;
}
