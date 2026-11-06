// main/yakka_assets.c
// Yakka Dee 口袋开口机 —— 内嵌素材资源表实现
#include "yakka_assets.h"

// 宏: 声明内嵌二进制素材
#define DECLARE_BIN(name) \
    extern const uint8_t _binary_##name##_start[]; \
    extern const uint8_t _binary_##name##_end[]

// 宏: 构造 lv_image_dsc_t
#define MAKE_IMG_DSC(name) { \
    .header.magic = LV_IMAGE_HEADER_MAGIC, \
    .header.cf = LV_COLOR_FORMAT_RGB565, \
    .header.flags = 0, \
    .header.w = YAKKA_IMG_W, \
    .header.h = YAKKA_IMG_H, \
    .header.stride = YAKKA_IMG_STRIDE, \
    .data_size = YAKKA_IMG_BYTES, \
    .data = _binary_##name##_start, \
}

// 宏: 构造 yakka_pcm_asset_t
#define MAKE_PCM_ASSET(name) { \
    .data = _binary_##name##_start, \
    .size = (size_t)(_binary_##name##_end - _binary_##name##_start), \
}

// 声明图片
DECLARE_BIN(dog_intro_rgb565);
DECLARE_BIN(dog_big_rgb565);
DECLARE_BIN(dog_tiny_rgb565);
DECLARE_BIN(dog_silly_rgb565);

DECLARE_BIN(banana_intro_rgb565);
DECLARE_BIN(banana_big_rgb565);
DECLARE_BIN(banana_long_rgb565);
DECLARE_BIN(banana_yellow_rgb565);

DECLARE_BIN(duck_intro_rgb565);
DECLARE_BIN(duck_swim_rgb565);
DECLARE_BIN(duck_quack_rgb565);
DECLARE_BIN(duck_fly_rgb565);

DECLARE_BIN(car_intro_rgb565);
DECLARE_BIN(car_fast_rgb565);
DECLARE_BIN(car_red_rgb565);
DECLARE_BIN(car_beep_rgb565);

DECLARE_BIN(apple_intro_rgb565);
DECLARE_BIN(apple_big_rgb565);
DECLARE_BIN(apple_yummy_rgb565);

DECLARE_BIN(dee_welcome_rgb565);
DECLARE_BIN(dee_listen_rgb565);
DECLARE_BIN(dee_sleep_rgb565);

// 声明系统音频
DECLARE_BIN(audio_boot_pcm);
DECLARE_BIN(audio_cheer_pcm);
DECLARE_BIN(audio_skip_pcm);
DECLARE_BIN(audio_nap_pcm);
DECLARE_BIN(audio_pop_pcm);
DECLARE_BIN(audio_parent_on_pcm);

// 声明词汇音频
DECLARE_BIN(audio_dog_intro_pcm);
DECLARE_BIN(audio_dog_c1_pcm);
DECLARE_BIN(audio_dog_c2_pcm);
DECLARE_BIN(audio_dog_c3_pcm);
DECLARE_BIN(audio_dog_q_pcm);
DECLARE_BIN(audio_dog_retry_pcm);

DECLARE_BIN(audio_banana_intro_pcm);
DECLARE_BIN(audio_banana_c1_pcm);
DECLARE_BIN(audio_banana_c2_pcm);
DECLARE_BIN(audio_banana_c3_pcm);
DECLARE_BIN(audio_banana_q_pcm);
DECLARE_BIN(audio_banana_retry_pcm);

DECLARE_BIN(audio_duck_intro_pcm);
DECLARE_BIN(audio_duck_c1_pcm);
DECLARE_BIN(audio_duck_c2_pcm);
DECLARE_BIN(audio_duck_c3_pcm);
DECLARE_BIN(audio_duck_q_pcm);
DECLARE_BIN(audio_duck_retry_pcm);

DECLARE_BIN(audio_car_intro_pcm);
DECLARE_BIN(audio_car_c1_pcm);
DECLARE_BIN(audio_car_c2_pcm);
DECLARE_BIN(audio_car_c3_pcm);
DECLARE_BIN(audio_car_q_pcm);
DECLARE_BIN(audio_car_retry_pcm);

DECLARE_BIN(audio_apple_intro_pcm);
DECLARE_BIN(audio_apple_c1_pcm);
DECLARE_BIN(audio_apple_c2_pcm);
DECLARE_BIN(audio_apple_c3_pcm);
DECLARE_BIN(audio_apple_q_pcm);
DECLARE_BIN(audio_apple_retry_pcm);

// 图片结构体表
static const lv_image_dsc_t IMG_DOG_INTRO   = MAKE_IMG_DSC(dog_intro_rgb565);
static const lv_image_dsc_t IMG_DOG_BIG     = MAKE_IMG_DSC(dog_big_rgb565);
static const lv_image_dsc_t IMG_DOG_TINY    = MAKE_IMG_DSC(dog_tiny_rgb565);
static const lv_image_dsc_t IMG_DOG_SILLY   = MAKE_IMG_DSC(dog_silly_rgb565);

static const lv_image_dsc_t IMG_BANANA_INTRO  = MAKE_IMG_DSC(banana_intro_rgb565);
static const lv_image_dsc_t IMG_BANANA_BIG    = MAKE_IMG_DSC(banana_big_rgb565);
static const lv_image_dsc_t IMG_BANANA_LONG   = MAKE_IMG_DSC(banana_long_rgb565);
static const lv_image_dsc_t IMG_BANANA_YELLOW = MAKE_IMG_DSC(banana_yellow_rgb565);

static const lv_image_dsc_t IMG_DUCK_INTRO = MAKE_IMG_DSC(duck_intro_rgb565);
static const lv_image_dsc_t IMG_DUCK_SWIM  = MAKE_IMG_DSC(duck_swim_rgb565);
static const lv_image_dsc_t IMG_DUCK_QUACK = MAKE_IMG_DSC(duck_quack_rgb565);
static const lv_image_dsc_t IMG_DUCK_FLY   = MAKE_IMG_DSC(duck_fly_rgb565);

static const lv_image_dsc_t IMG_CAR_INTRO = MAKE_IMG_DSC(car_intro_rgb565);
static const lv_image_dsc_t IMG_CAR_FAST  = MAKE_IMG_DSC(car_fast_rgb565);
static const lv_image_dsc_t IMG_CAR_RED   = MAKE_IMG_DSC(car_red_rgb565);
static const lv_image_dsc_t IMG_CAR_BEEP  = MAKE_IMG_DSC(car_beep_rgb565);

static const lv_image_dsc_t IMG_APPLE_INTRO = MAKE_IMG_DSC(apple_intro_rgb565);
static const lv_image_dsc_t IMG_APPLE_BIG   = MAKE_IMG_DSC(apple_big_rgb565);
static const lv_image_dsc_t IMG_APPLE_YUMMY = MAKE_IMG_DSC(apple_yummy_rgb565);

static const lv_image_dsc_t IMG_DEE_WELCOME = MAKE_IMG_DSC(dee_welcome_rgb565);
static const lv_image_dsc_t IMG_DEE_LISTEN  = MAKE_IMG_DSC(dee_listen_rgb565);
static const lv_image_dsc_t IMG_DEE_SLEEP   = MAKE_IMG_DSC(dee_sleep_rgb565);

const lv_image_dsc_t *yakka_asset_get_dee_image(yakka_state_t state) {
    if (state == YAKKA_STATE_NAP_TIME) return &IMG_DEE_SLEEP;
    if (state == YAKKA_STATE_QUESTION || state == YAKKA_STATE_ENCOURAGE_RETRY) return &IMG_DEE_LISTEN;
    return &IMG_DEE_WELCOME;
}

const lv_image_dsc_t *yakka_asset_get_image(yakka_vocab_t vocab, yakka_state_t state) {
    switch (vocab) {
        case YAKKA_VOCAB_DOG:
            if (state == YAKKA_STATE_CONTRAST_1) return &IMG_DOG_BIG;
            if (state == YAKKA_STATE_CONTRAST_2) return &IMG_DOG_TINY;
            if (state == YAKKA_STATE_CONTRAST_3) return &IMG_DOG_SILLY;
            return &IMG_DOG_INTRO;

        case YAKKA_VOCAB_BANANA:
            if (state == YAKKA_STATE_CONTRAST_1) return &IMG_BANANA_BIG;
            if (state == YAKKA_STATE_CONTRAST_2) return &IMG_BANANA_LONG;
            if (state == YAKKA_STATE_CONTRAST_3) return &IMG_BANANA_YELLOW;
            return &IMG_BANANA_INTRO;

        case YAKKA_VOCAB_DUCK:
            if (state == YAKKA_STATE_CONTRAST_1) return &IMG_DUCK_SWIM;
            if (state == YAKKA_STATE_CONTRAST_2) return &IMG_DUCK_QUACK;
            if (state == YAKKA_STATE_CONTRAST_3) return &IMG_DUCK_FLY;
            return &IMG_DUCK_INTRO;

        case YAKKA_VOCAB_CAR:
            if (state == YAKKA_STATE_CONTRAST_1) return &IMG_CAR_FAST;
            if (state == YAKKA_STATE_CONTRAST_2) return &IMG_CAR_RED;
            if (state == YAKKA_STATE_CONTRAST_3) return &IMG_CAR_BEEP;
            return &IMG_CAR_INTRO;

        case YAKKA_VOCAB_APPLE:
            if (state == YAKKA_STATE_CONTRAST_1) return &IMG_APPLE_BIG;
            if (state == YAKKA_STATE_CONTRAST_2) return &IMG_APPLE_YUMMY;
            if (state == YAKKA_STATE_CONTRAST_3) return &IMG_APPLE_INTRO;
            return &IMG_APPLE_INTRO;

        default:
            return &IMG_DEE_WELCOME;
    }
}

yakka_pcm_asset_t yakka_asset_audio_boot(void)      { return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_boot_pcm); }
yakka_pcm_asset_t yakka_asset_audio_cheer(void)     { return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_cheer_pcm); }
yakka_pcm_asset_t yakka_asset_audio_skip(void)      { return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_skip_pcm); }
yakka_pcm_asset_t yakka_asset_audio_nap(void)       { return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_nap_pcm); }
yakka_pcm_asset_t yakka_asset_audio_pop(void)       { return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_pop_pcm); }
yakka_pcm_asset_t yakka_asset_audio_parent_on(void) { return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_parent_on_pcm); }

yakka_pcm_asset_t yakka_asset_get_audio(yakka_vocab_t vocab, yakka_action_t action) {
    switch (action) {
        case YAKKA_ACTION_PLAY_AUDIO_BOOT:      return yakka_asset_audio_boot();
        case YAKKA_ACTION_PLAY_AUDIO_SUCCESS:   return yakka_asset_audio_cheer();
        case YAKKA_ACTION_PLAY_AUDIO_SKIP:      return yakka_asset_audio_skip();
        case YAKKA_ACTION_PLAY_AUDIO_NAP:       return yakka_asset_audio_nap();
        case YAKKA_ACTION_PLAY_AUDIO_POP:       return yakka_asset_audio_pop();
        case YAKKA_ACTION_PLAY_AUDIO_PARENT_ON: return yakka_asset_audio_parent_on();
        default: break;
    }

    if (vocab == YAKKA_VOCAB_DOG) {
        if (action == YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO) return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_dog_intro_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_dog_c1_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_dog_c2_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_dog_c3_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_QUESTION)    return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_dog_q_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_RETRY)       return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_dog_retry_pcm);
    } else if (vocab == YAKKA_VOCAB_BANANA) {
        if (action == YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO) return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_banana_intro_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_banana_c1_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_banana_c2_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_banana_c3_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_QUESTION)    return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_banana_q_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_RETRY)       return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_banana_retry_pcm);
    } else if (vocab == YAKKA_VOCAB_DUCK) {
        if (action == YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO) return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_duck_intro_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_duck_c1_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_duck_c2_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_duck_c3_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_QUESTION)    return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_duck_q_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_RETRY)       return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_duck_retry_pcm);
    } else if (vocab == YAKKA_VOCAB_CAR) {
        if (action == YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO) return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_car_intro_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_car_c1_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_car_c2_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_car_c3_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_QUESTION)    return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_car_q_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_RETRY)       return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_car_retry_pcm);
    } else if (vocab == YAKKA_VOCAB_APPLE) {
        if (action == YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO) return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_apple_intro_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_apple_c1_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_apple_c2_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)  return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_apple_c3_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_QUESTION)    return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_apple_q_pcm);
        if (action == YAKKA_ACTION_PLAY_AUDIO_RETRY)       return (yakka_pcm_asset_t)MAKE_PCM_ASSET(audio_apple_retry_pcm);
    }

    return (yakka_pcm_asset_t){ .data = NULL, .size = 0 };
}
