// main/yakka_assets.h
// Yakka Dee 口袋开口机 —— 内嵌素材资源表
#pragma once

#include "lvgl.h"
#include "yakka_state.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define YAKKA_IMG_W 200
#define YAKKA_IMG_H 200
#define YAKKA_IMG_STRIDE (YAKKA_IMG_W * 2)
#define YAKKA_IMG_BYTES (YAKKA_IMG_STRIDE * YAKKA_IMG_H)

typedef struct {
    const uint8_t *data;
    size_t size;
} yakka_pcm_asset_t;

// 获取词汇对应的图形
const lv_image_dsc_t *yakka_asset_get_image(yakka_vocab_t vocab, yakka_state_t state);

// 获取 Dee 吉祥物表情图 (welcome / listen / sleep)
const lv_image_dsc_t *yakka_asset_get_dee_image(yakka_state_t state);

// 获取对应状态的 PCM 音频片段
yakka_pcm_asset_t yakka_asset_get_audio(yakka_vocab_t vocab, yakka_action_t action);

// 音频资源便捷获取
yakka_pcm_asset_t yakka_asset_audio_boot(void);
yakka_pcm_asset_t yakka_asset_audio_cheer(void);
yakka_pcm_asset_t yakka_asset_audio_skip(void);
yakka_pcm_asset_t yakka_asset_audio_nap(void);
yakka_pcm_asset_t yakka_asset_audio_pop(void);
yakka_pcm_asset_t yakka_asset_audio_parent_on(void);

#ifdef __cplusplus
}
#endif
