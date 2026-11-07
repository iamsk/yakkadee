// tests/test_adpcm.c
// Yakka Dee IMA-ADPCM 编解码器单元测试
#include "adpcm.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_silence(void) {
    int16_t orig[100] = {0};
    uint8_t enc[50] = {0};
    int16_t dec[100] = {0};

    adpcm_state_t enc_st, dec_st;
    adpcm_state_init(&enc_st);
    adpcm_state_init(&dec_st);

    size_t enc_bytes = adpcm_encode_block(orig, 100, enc, &enc_st);
    assert(enc_bytes == 50);

    size_t dec_samples = adpcm_decode_block(enc, 50, dec, &dec_st);
    assert(dec_samples == 100);

    for (int i = 0; i < 100; i++) {
        assert(abs(dec[i]) <= 8); // 静音编码解码误差不超过步长基底
    }
}

static void test_sine_wave_snr(void) {
    const size_t N = 16000; // 1秒 16kHz
    int16_t *orig = malloc(N * sizeof(int16_t));
    uint8_t *enc = malloc((N / 2) * sizeof(uint8_t));
    int16_t *dec = malloc(N * sizeof(int16_t));
    assert(orig && enc && dec);

    for (size_t i = 0; i < N; i++) {
        double t = (double)i / 16000.0;
        orig[i] = (int16_t)(sin(2.0 * M_PI * 440.0 * t) * 20000.0);
    }

    adpcm_state_t enc_st, dec_st;
    adpcm_state_init(&enc_st);
    adpcm_state_init(&dec_st);

    size_t enc_bytes = adpcm_encode_block(orig, N, enc, &enc_st);
    assert(enc_bytes == N / 2);

    size_t dec_samples = adpcm_decode_block(enc, enc_bytes, dec, &dec_st);
    assert(dec_samples == N);

    double sig_power = 0.0;
    double err_power = 0.0;
    for (size_t i = 0; i < N; i++) {
        double s = orig[i];
        double e = orig[i] - dec[i];
        sig_power += s * s;
        err_power += e * e;
    }

    double snr = 10.0 * log10(sig_power / err_power);
    // ADPCM 4-bit 对 440Hz 纯音的信噪比应大于 30 dB (实测 ~43 dB)
    assert(snr > 30.0);

    free(orig);
    free(enc);
    free(dec);
}

static void test_streaming_chunk_continuity(void) {
    const size_t N = 2048;
    int16_t orig[2048];
    uint8_t enc[1024];
    int16_t dec_continuous[2048];
    int16_t dec_chunked[2048];

    for (size_t i = 0; i < N; i++) {
        orig[i] = (int16_t)((i * 37) % 30000 - 15000);
    }

    adpcm_state_t enc_st;
    adpcm_state_init(&enc_st);
    adpcm_encode_block(orig, N, enc, &enc_st);

    // 一次性整体解码
    adpcm_state_t dec_st1;
    adpcm_state_init(&dec_st1);
    adpcm_decode_block(enc, 1024, dec_continuous, &dec_st1);

    // 分块流式解码 (每次 128 字节 = 256 采样点)
    adpcm_state_t dec_st2;
    adpcm_state_init(&dec_st2);
    for (size_t offset = 0; offset < 1024; offset += 128) {
        adpcm_decode_block(enc + offset, 128, dec_chunked + (offset * 2), &dec_st2);
    }

    // 两者解码结果必须 100% 逐样点完全一致
    for (size_t i = 0; i < N; i++) {
        assert(dec_continuous[i] == dec_chunked[i]);
    }
}

int main(void) {
    test_silence();
    test_sine_wave_snr();
    test_streaming_chunk_continuity();
    printf("test_adpcm: ALL PASS\n");
    return 0;
}
