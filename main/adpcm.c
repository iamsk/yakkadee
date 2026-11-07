// main/adpcm.c
// Yakka Dee 口袋开口机 —— IMA-ADPCM 4-bit 音频编解码实现
#include "adpcm.h"

// 标准 IMA-ADPCM 步长表 (89项)
static const int16_t s_step_table[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

// 步长索引调整表
static const int8_t s_index_table[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

int16_t adpcm_decode_nibble(uint8_t nibble, adpcm_state_t *state) {
    if (!state) return 0;

    int8_t idx = state->step_index;
    if (idx < 0) idx = 0;
    else if (idx > 88) idx = 88;

    int32_t step = s_step_table[idx];
    int32_t diff = step >> 3;
    if (nibble & 4) diff += step;
    if (nibble & 2) diff += step >> 1;
    if (nibble & 1) diff += step >> 2;
    if (nibble & 8) diff = -diff;

    int32_t sample = state->prev_sample + diff;
    if (sample > 32767) sample = 32767;
    else if (sample < -32768) sample = -32768;

    state->prev_sample = (int16_t)sample;

    idx += s_index_table[nibble & 0x0F];
    if (idx < 0) idx = 0;
    else if (idx > 88) idx = 88;
    state->step_index = idx;

    return (int16_t)sample;
}

size_t adpcm_decode_block(const uint8_t *in, size_t in_bytes, int16_t *out, adpcm_state_t *state) {
    if (!in || !out || !state || in_bytes == 0) return 0;

    size_t out_idx = 0;
    for (size_t i = 0; i < in_bytes; i++) {
        uint8_t byte = in[i];
        out[out_idx++] = adpcm_decode_nibble(byte & 0x0F, state);
        out[out_idx++] = adpcm_decode_nibble((byte >> 4) & 0x0F, state);
    }
    return out_idx;
}

uint8_t adpcm_encode_sample(int16_t sample, adpcm_state_t *state) {
    if (!state) return 0;

    int8_t idx = state->step_index;
    if (idx < 0) idx = 0;
    else if (idx > 88) idx = 88;

    int32_t step = s_step_table[idx];
    int32_t diff = sample - state->prev_sample;
    uint8_t nibble = 0;

    if (diff < 0) {
        nibble |= 8;
        diff = -diff;
    }
    if (diff >= step) {
        nibble |= 4;
        diff -= step;
    }
    if (diff >= (step >> 1)) {
        nibble |= 2;
        diff -= (step >> 1);
    }
    if (diff >= (step >> 2)) {
        nibble |= 1;
    }

    // 调用解码更新内部状态，保证编解码端状态完全同步
    adpcm_decode_nibble(nibble, state);
    return nibble;
}

size_t adpcm_encode_block(const int16_t *in, size_t num_samples, uint8_t *out, adpcm_state_t *state) {
    if (!in || !out || !state || num_samples == 0) return 0;

    size_t out_bytes = 0;
    for (size_t i = 0; i < num_samples; i += 2) {
        uint8_t nib0 = adpcm_encode_sample(in[i], state);
        uint8_t nib1 = 0;
        if (i + 1 < num_samples) {
            nib1 = adpcm_encode_sample(in[i + 1], state);
        }
        out[out_bytes++] = (uint8_t)((nib1 << 4) | (nib0 & 0x0F));
    }
    return out_bytes;
}
