// main/adpcm.h
// Yakka Dee 口袋开口机 —— IMA-ADPCM 4-bit 音频编解码模块
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADPCM 解码器/编码器状态结构体
 */
typedef struct {
    int16_t prev_sample; // 上一个样本值 [-32768, 32767]
    int8_t step_index;   // 当前步长表索引 [0, 88]
} adpcm_state_t;

/**
 * @brief 重置 ADPCM 状态
 */
static inline void adpcm_state_init(adpcm_state_t *state) {
    if (state) {
        state->prev_sample = 0;
        state->step_index = 0;
    }
}

/**
 * @brief 解码单个 4-bit nibble 为 16-bit PCM 采样
 */
int16_t adpcm_decode_nibble(uint8_t nibble, adpcm_state_t *state);

/**
 * @brief 解码一整块紧凑打包的 ADPCM 字节流为 16-bit PCM 采样
 *
 * 每个字节包含 2 个采样点：
 *   采样点 0: 低 4 位 (byte & 0x0F)
 *   采样点 1: 高 4 位 ((byte >> 4) & 0x0F)
 *
 * @param in 打包的 ADPCM 输入字节流
 * @param in_bytes 输入字节长度
 * @param out 输出 PCM 16-bit 缓冲区 (大小至少为 in_bytes * 2 个 int16_t)
 * @param state 跨块保存的状态指针
 * @return 成功解码并写入 out 的采样点总数 (in_bytes * 2)
 */
size_t adpcm_decode_block(const uint8_t *in, size_t in_bytes, int16_t *out, adpcm_state_t *state);

/**
 * @brief 编码单个 16-bit PCM 采样为 4-bit nibble
 */
uint8_t adpcm_encode_sample(int16_t sample, adpcm_state_t *state);

/**
 * @brief 编码一整块 16-bit PCM 采样为紧凑打包的 ADPCM 字节流
 *
 * @param in 输入 16-bit PCM 采样数组
 * @param num_samples 采样点总数
 * @param out 输出 ADPCM 字节缓冲区 (大小至少为 (num_samples + 1) / 2 字节)
 * @param state 编码状态指针
 * @return 写入 out 的字节总数
 */
size_t adpcm_encode_block(const int16_t *in, size_t num_samples, uint8_t *out, adpcm_state_t *state);

#ifdef __cplusplus
}
#endif
