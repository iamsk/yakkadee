// main/yakka_assets.c
// Yakka Dee 口袋开口机 —— 第一季 20 词轻量压缩素材表
#include "yakka_assets.h"

// 宏: 声明内嵌二进制素材
#define DECLARE_BIN(name) \
    extern const uint8_t _binary_##name##_start[]; \
    extern const uint8_t _binary_##name##_end[]

typedef struct {
    const uint8_t *start;
    const uint8_t *end;
} bin_ref_t;

#define MAKE_BIN_REF(name) { \
    .start = _binary_##name##_start, \
    .end   = _binary_##name##_end, \
}

// --- 声明图片资源 ---
DECLARE_BIN(apple_big_jpg);
DECLARE_BIN(apple_crunch_jpg);
DECLARE_BIN(apple_intro_jpg);
DECLARE_BIN(apple_yummy_jpg);
DECLARE_BIN(ball_bounce_jpg);
DECLARE_BIN(ball_intro_jpg);
DECLARE_BIN(ball_roll_jpg);
DECLARE_BIN(ball_round_jpg);
DECLARE_BIN(banana_big_jpg);
DECLARE_BIN(banana_intro_jpg);
DECLARE_BIN(banana_long_jpg);
DECLARE_BIN(banana_yellow_jpg);
DECLARE_BIN(beans_green_jpg);
DECLARE_BIN(beans_intro_jpg);
DECLARE_BIN(beans_jumping_jpg);
DECLARE_BIN(beans_magic_jpg);
DECLARE_BIN(bed_cozy_jpg);
DECLARE_BIN(bed_intro_jpg);
DECLARE_BIN(bed_sleepy_jpg);
DECLARE_BIN(bed_soft_jpg);
DECLARE_BIN(bike_fast_jpg);
DECLARE_BIN(bike_intro_jpg);
DECLARE_BIN(bike_little_jpg);
DECLARE_BIN(bike_ring_jpg);
DECLARE_BIN(boat_float_jpg);
DECLARE_BIN(boat_intro_jpg);
DECLARE_BIN(boat_sailing_jpg);
DECLARE_BIN(boat_speed_jpg);
DECLARE_BIN(book_big_jpg);
DECLARE_BIN(book_intro_jpg);
DECLARE_BIN(book_open_jpg);
DECLARE_BIN(book_story_jpg);
DECLARE_BIN(boots_intro_jpg);
DECLARE_BIN(boots_muddy_jpg);
DECLARE_BIN(boots_red_jpg);
DECLARE_BIN(boots_shiny_jpg);
DECLARE_BIN(bus_beep_jpg);
DECLARE_BIN(bus_big_jpg);
DECLARE_BIN(bus_intro_jpg);
DECLARE_BIN(bus_red_jpg);
DECLARE_BIN(car_beep_jpg);
DECLARE_BIN(car_fast_jpg);
DECLARE_BIN(car_intro_jpg);
DECLARE_BIN(car_red_jpg);
DECLARE_BIN(cup_big_jpg);
DECLARE_BIN(cup_blue_jpg);
DECLARE_BIN(cup_intro_jpg);
DECLARE_BIN(cup_sippy_jpg);
DECLARE_BIN(dee_listen_jpg);
DECLARE_BIN(dee_sleep_jpg);
DECLARE_BIN(dee_welcome_jpg);
DECLARE_BIN(dog_big_jpg);
DECLARE_BIN(dog_intro_jpg);
DECLARE_BIN(dog_silly_jpg);
DECLARE_BIN(dog_tiny_jpg);
DECLARE_BIN(duck_fly_jpg);
DECLARE_BIN(duck_intro_jpg);
DECLARE_BIN(duck_quack_jpg);
DECLARE_BIN(duck_swim_jpg);
DECLARE_BIN(hat_intro_jpg);
DECLARE_BIN(hat_silly_jpg);
DECLARE_BIN(hat_sun_jpg);
DECLARE_BIN(hat_warm_jpg);
DECLARE_BIN(house_brick_jpg);
DECLARE_BIN(house_cozy_jpg);
DECLARE_BIN(house_intro_jpg);
DECLARE_BIN(house_tall_jpg);
DECLARE_BIN(lion_big_jpg);
DECLARE_BIN(lion_golden_jpg);
DECLARE_BIN(lion_intro_jpg);
DECLARE_BIN(lion_roar_jpg);
DECLARE_BIN(mouse_intro_jpg);
DECLARE_BIN(mouse_little_jpg);
DECLARE_BIN(mouse_quiet_jpg);
DECLARE_BIN(mouse_squeak_jpg);
DECLARE_BIN(peas_green_jpg);
DECLARE_BIN(peas_intro_jpg);
DECLARE_BIN(peas_sweet_jpg);
DECLARE_BIN(peas_tiny_jpg);
DECLARE_BIN(worm_earth_jpg);
DECLARE_BIN(worm_intro_jpg);
DECLARE_BIN(worm_long_jpg);
DECLARE_BIN(worm_wiggly_jpg);

// --- 声明系统音频 ---
DECLARE_BIN(audio_boot_adpcm);
DECLARE_BIN(audio_cheer_adpcm);
DECLARE_BIN(audio_nap_adpcm);
DECLARE_BIN(audio_parent_on_adpcm);
DECLARE_BIN(audio_pop_adpcm);
DECLARE_BIN(audio_skip_adpcm);

// --- 声明词汇音频 ---
DECLARE_BIN(audio_apple_c1_adpcm);
DECLARE_BIN(audio_apple_c2_adpcm);
DECLARE_BIN(audio_apple_c3_adpcm);
DECLARE_BIN(audio_apple_intro_adpcm);
DECLARE_BIN(audio_apple_q_adpcm);
DECLARE_BIN(audio_apple_retry_adpcm);
DECLARE_BIN(audio_ball_c1_adpcm);
DECLARE_BIN(audio_ball_c2_adpcm);
DECLARE_BIN(audio_ball_c3_adpcm);
DECLARE_BIN(audio_ball_intro_adpcm);
DECLARE_BIN(audio_ball_q_adpcm);
DECLARE_BIN(audio_ball_retry_adpcm);
DECLARE_BIN(audio_banana_c1_adpcm);
DECLARE_BIN(audio_banana_c2_adpcm);
DECLARE_BIN(audio_banana_c3_adpcm);
DECLARE_BIN(audio_banana_intro_adpcm);
DECLARE_BIN(audio_banana_q_adpcm);
DECLARE_BIN(audio_banana_retry_adpcm);
DECLARE_BIN(audio_beans_c1_adpcm);
DECLARE_BIN(audio_beans_c2_adpcm);
DECLARE_BIN(audio_beans_c3_adpcm);
DECLARE_BIN(audio_beans_intro_adpcm);
DECLARE_BIN(audio_beans_q_adpcm);
DECLARE_BIN(audio_beans_retry_adpcm);
DECLARE_BIN(audio_bed_c1_adpcm);
DECLARE_BIN(audio_bed_c2_adpcm);
DECLARE_BIN(audio_bed_c3_adpcm);
DECLARE_BIN(audio_bed_intro_adpcm);
DECLARE_BIN(audio_bed_q_adpcm);
DECLARE_BIN(audio_bed_retry_adpcm);
DECLARE_BIN(audio_bike_c1_adpcm);
DECLARE_BIN(audio_bike_c2_adpcm);
DECLARE_BIN(audio_bike_c3_adpcm);
DECLARE_BIN(audio_bike_intro_adpcm);
DECLARE_BIN(audio_bike_q_adpcm);
DECLARE_BIN(audio_bike_retry_adpcm);
DECLARE_BIN(audio_boat_c1_adpcm);
DECLARE_BIN(audio_boat_c2_adpcm);
DECLARE_BIN(audio_boat_c3_adpcm);
DECLARE_BIN(audio_boat_intro_adpcm);
DECLARE_BIN(audio_boat_q_adpcm);
DECLARE_BIN(audio_boat_retry_adpcm);
DECLARE_BIN(audio_book_c1_adpcm);
DECLARE_BIN(audio_book_c2_adpcm);
DECLARE_BIN(audio_book_c3_adpcm);
DECLARE_BIN(audio_book_intro_adpcm);
DECLARE_BIN(audio_book_q_adpcm);
DECLARE_BIN(audio_book_retry_adpcm);
DECLARE_BIN(audio_boots_c1_adpcm);
DECLARE_BIN(audio_boots_c2_adpcm);
DECLARE_BIN(audio_boots_c3_adpcm);
DECLARE_BIN(audio_boots_intro_adpcm);
DECLARE_BIN(audio_boots_q_adpcm);
DECLARE_BIN(audio_boots_retry_adpcm);
DECLARE_BIN(audio_bus_c1_adpcm);
DECLARE_BIN(audio_bus_c2_adpcm);
DECLARE_BIN(audio_bus_c3_adpcm);
DECLARE_BIN(audio_bus_intro_adpcm);
DECLARE_BIN(audio_bus_q_adpcm);
DECLARE_BIN(audio_bus_retry_adpcm);
DECLARE_BIN(audio_car_c1_adpcm);
DECLARE_BIN(audio_car_c2_adpcm);
DECLARE_BIN(audio_car_c3_adpcm);
DECLARE_BIN(audio_car_intro_adpcm);
DECLARE_BIN(audio_car_q_adpcm);
DECLARE_BIN(audio_car_retry_adpcm);
DECLARE_BIN(audio_cup_c1_adpcm);
DECLARE_BIN(audio_cup_c2_adpcm);
DECLARE_BIN(audio_cup_c3_adpcm);
DECLARE_BIN(audio_cup_intro_adpcm);
DECLARE_BIN(audio_cup_q_adpcm);
DECLARE_BIN(audio_cup_retry_adpcm);
DECLARE_BIN(audio_dog_c1_adpcm);
DECLARE_BIN(audio_dog_c2_adpcm);
DECLARE_BIN(audio_dog_c3_adpcm);
DECLARE_BIN(audio_dog_intro_adpcm);
DECLARE_BIN(audio_dog_q_adpcm);
DECLARE_BIN(audio_dog_retry_adpcm);
DECLARE_BIN(audio_duck_c1_adpcm);
DECLARE_BIN(audio_duck_c2_adpcm);
DECLARE_BIN(audio_duck_c3_adpcm);
DECLARE_BIN(audio_duck_intro_adpcm);
DECLARE_BIN(audio_duck_q_adpcm);
DECLARE_BIN(audio_duck_retry_adpcm);
DECLARE_BIN(audio_hat_c1_adpcm);
DECLARE_BIN(audio_hat_c2_adpcm);
DECLARE_BIN(audio_hat_c3_adpcm);
DECLARE_BIN(audio_hat_intro_adpcm);
DECLARE_BIN(audio_hat_q_adpcm);
DECLARE_BIN(audio_hat_retry_adpcm);
DECLARE_BIN(audio_house_c1_adpcm);
DECLARE_BIN(audio_house_c2_adpcm);
DECLARE_BIN(audio_house_c3_adpcm);
DECLARE_BIN(audio_house_intro_adpcm);
DECLARE_BIN(audio_house_q_adpcm);
DECLARE_BIN(audio_house_retry_adpcm);
DECLARE_BIN(audio_lion_c1_adpcm);
DECLARE_BIN(audio_lion_c2_adpcm);
DECLARE_BIN(audio_lion_c3_adpcm);
DECLARE_BIN(audio_lion_intro_adpcm);
DECLARE_BIN(audio_lion_q_adpcm);
DECLARE_BIN(audio_lion_retry_adpcm);
DECLARE_BIN(audio_mouse_c1_adpcm);
DECLARE_BIN(audio_mouse_c2_adpcm);
DECLARE_BIN(audio_mouse_c3_adpcm);
DECLARE_BIN(audio_mouse_intro_adpcm);
DECLARE_BIN(audio_mouse_q_adpcm);
DECLARE_BIN(audio_mouse_retry_adpcm);
DECLARE_BIN(audio_peas_c1_adpcm);
DECLARE_BIN(audio_peas_c2_adpcm);
DECLARE_BIN(audio_peas_c3_adpcm);
DECLARE_BIN(audio_peas_intro_adpcm);
DECLARE_BIN(audio_peas_q_adpcm);
DECLARE_BIN(audio_peas_retry_adpcm);
DECLARE_BIN(audio_worm_c1_adpcm);
DECLARE_BIN(audio_worm_c2_adpcm);
DECLARE_BIN(audio_worm_c3_adpcm);
DECLARE_BIN(audio_worm_intro_adpcm);
DECLARE_BIN(audio_worm_q_adpcm);
DECLARE_BIN(audio_worm_retry_adpcm);

#define TOTAL_JPG_COUNT 83
static const bin_ref_t s_jpg_refs[TOTAL_JPG_COUNT] = {
    MAKE_BIN_REF(apple_big_jpg),
    MAKE_BIN_REF(apple_crunch_jpg),
    MAKE_BIN_REF(apple_intro_jpg),
    MAKE_BIN_REF(apple_yummy_jpg),
    MAKE_BIN_REF(ball_bounce_jpg),
    MAKE_BIN_REF(ball_intro_jpg),
    MAKE_BIN_REF(ball_roll_jpg),
    MAKE_BIN_REF(ball_round_jpg),
    MAKE_BIN_REF(banana_big_jpg),
    MAKE_BIN_REF(banana_intro_jpg),
    MAKE_BIN_REF(banana_long_jpg),
    MAKE_BIN_REF(banana_yellow_jpg),
    MAKE_BIN_REF(beans_green_jpg),
    MAKE_BIN_REF(beans_intro_jpg),
    MAKE_BIN_REF(beans_jumping_jpg),
    MAKE_BIN_REF(beans_magic_jpg),
    MAKE_BIN_REF(bed_cozy_jpg),
    MAKE_BIN_REF(bed_intro_jpg),
    MAKE_BIN_REF(bed_sleepy_jpg),
    MAKE_BIN_REF(bed_soft_jpg),
    MAKE_BIN_REF(bike_fast_jpg),
    MAKE_BIN_REF(bike_intro_jpg),
    MAKE_BIN_REF(bike_little_jpg),
    MAKE_BIN_REF(bike_ring_jpg),
    MAKE_BIN_REF(boat_float_jpg),
    MAKE_BIN_REF(boat_intro_jpg),
    MAKE_BIN_REF(boat_sailing_jpg),
    MAKE_BIN_REF(boat_speed_jpg),
    MAKE_BIN_REF(book_big_jpg),
    MAKE_BIN_REF(book_intro_jpg),
    MAKE_BIN_REF(book_open_jpg),
    MAKE_BIN_REF(book_story_jpg),
    MAKE_BIN_REF(boots_intro_jpg),
    MAKE_BIN_REF(boots_muddy_jpg),
    MAKE_BIN_REF(boots_red_jpg),
    MAKE_BIN_REF(boots_shiny_jpg),
    MAKE_BIN_REF(bus_beep_jpg),
    MAKE_BIN_REF(bus_big_jpg),
    MAKE_BIN_REF(bus_intro_jpg),
    MAKE_BIN_REF(bus_red_jpg),
    MAKE_BIN_REF(car_beep_jpg),
    MAKE_BIN_REF(car_fast_jpg),
    MAKE_BIN_REF(car_intro_jpg),
    MAKE_BIN_REF(car_red_jpg),
    MAKE_BIN_REF(cup_big_jpg),
    MAKE_BIN_REF(cup_blue_jpg),
    MAKE_BIN_REF(cup_intro_jpg),
    MAKE_BIN_REF(cup_sippy_jpg),
    MAKE_BIN_REF(dee_listen_jpg),
    MAKE_BIN_REF(dee_sleep_jpg),
    MAKE_BIN_REF(dee_welcome_jpg),
    MAKE_BIN_REF(dog_big_jpg),
    MAKE_BIN_REF(dog_intro_jpg),
    MAKE_BIN_REF(dog_silly_jpg),
    MAKE_BIN_REF(dog_tiny_jpg),
    MAKE_BIN_REF(duck_fly_jpg),
    MAKE_BIN_REF(duck_intro_jpg),
    MAKE_BIN_REF(duck_quack_jpg),
    MAKE_BIN_REF(duck_swim_jpg),
    MAKE_BIN_REF(hat_intro_jpg),
    MAKE_BIN_REF(hat_silly_jpg),
    MAKE_BIN_REF(hat_sun_jpg),
    MAKE_BIN_REF(hat_warm_jpg),
    MAKE_BIN_REF(house_brick_jpg),
    MAKE_BIN_REF(house_cozy_jpg),
    MAKE_BIN_REF(house_intro_jpg),
    MAKE_BIN_REF(house_tall_jpg),
    MAKE_BIN_REF(lion_big_jpg),
    MAKE_BIN_REF(lion_golden_jpg),
    MAKE_BIN_REF(lion_intro_jpg),
    MAKE_BIN_REF(lion_roar_jpg),
    MAKE_BIN_REF(mouse_intro_jpg),
    MAKE_BIN_REF(mouse_little_jpg),
    MAKE_BIN_REF(mouse_quiet_jpg),
    MAKE_BIN_REF(mouse_squeak_jpg),
    MAKE_BIN_REF(peas_green_jpg),
    MAKE_BIN_REF(peas_intro_jpg),
    MAKE_BIN_REF(peas_sweet_jpg),
    MAKE_BIN_REF(peas_tiny_jpg),
    MAKE_BIN_REF(worm_earth_jpg),
    MAKE_BIN_REF(worm_intro_jpg),
    MAKE_BIN_REF(worm_long_jpg),
    MAKE_BIN_REF(worm_wiggly_jpg),
};

static lv_image_dsc_t s_jpg_dscs[TOTAL_JPG_COUNT];
static bool s_dscs_inited = false;

static void ensure_dscs_inited(void) {
    if (s_dscs_inited) return;
    for (int i = 0; i < TOTAL_JPG_COUNT; i++) {
        s_jpg_dscs[i].header.magic = LV_IMAGE_HEADER_MAGIC;
        s_jpg_dscs[i].header.cf = LV_COLOR_FORMAT_RAW;
        s_jpg_dscs[i].header.flags = 0;
        s_jpg_dscs[i].header.w = YAKKA_IMG_W;
        s_jpg_dscs[i].header.h = YAKKA_IMG_H;
        s_jpg_dscs[i].header.stride = 0;
        s_jpg_dscs[i].data = s_jpg_refs[i].start;
        s_jpg_dscs[i].data_size = (size_t)(s_jpg_refs[i].end - s_jpg_refs[i].start);
    }
    s_dscs_inited = true;
}

typedef struct {
    int intro_idx;
    int c1_idx;
    int c2_idx;
    int c3_idx;
} vocab_img_map_t;

static const vocab_img_map_t s_vocab_img_map[YAKKA_VOCAB_COUNT] = {
    [YAKKA_VOCAB_BANANA] = { 9, 8, 10, 11 },
    [YAKKA_VOCAB_DOG] = { 52, 51, 54, 53 },
    [YAKKA_VOCAB_BOOK] = { 29, 28, 30, 31 },
    [YAKKA_VOCAB_BOOTS] = { 32, 34, 33, 35 },
    [YAKKA_VOCAB_BIKE] = { 21, 20, 22, 23 },
    [YAKKA_VOCAB_DUCK] = { 56, 58, 57, 55 },
    [YAKKA_VOCAB_CUP] = { 46, 45, 44, 47 },
    [YAKKA_VOCAB_BUS] = { 38, 37, 39, 36 },
    [YAKKA_VOCAB_PEAS] = { 76, 75, 78, 77 },
    [YAKKA_VOCAB_HAT] = { 59, 61, 60, 62 },
    [YAKKA_VOCAB_WORM] = { 80, 82, 81, 79 },
    [YAKKA_VOCAB_BOAT] = { 25, 26, 27, 24 },
    [YAKKA_VOCAB_HOUSE] = { 65, 64, 66, 63 },
    [YAKKA_VOCAB_LION] = { 69, 70, 67, 68 },
    [YAKKA_VOCAB_APPLE] = { 2, 0, 3, 1 },
    [YAKKA_VOCAB_BALL] = { 5, 4, 7, 6 },
    [YAKKA_VOCAB_MOUSE] = { 71, 72, 74, 73 },
    [YAKKA_VOCAB_BEANS] = { 13, 15, 12, 14 },
    [YAKKA_VOCAB_CAR] = { 42, 41, 43, 40 },
    [YAKKA_VOCAB_BED] = { 17, 19, 18, 16 },
};

typedef struct {
    bin_ref_t intro;
    bin_ref_t c1;
    bin_ref_t c2;
    bin_ref_t c3;
    bin_ref_t q;
    bin_ref_t retry;
} vocab_audio_bundle_t;

static const vocab_audio_bundle_t s_vocab_audios[YAKKA_VOCAB_COUNT] = {
    [YAKKA_VOCAB_BANANA] = {
        .intro = MAKE_BIN_REF(audio_banana_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_banana_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_banana_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_banana_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_banana_q_adpcm),
        .retry = MAKE_BIN_REF(audio_banana_retry_adpcm),
    },
    [YAKKA_VOCAB_DOG] = {
        .intro = MAKE_BIN_REF(audio_dog_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_dog_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_dog_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_dog_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_dog_q_adpcm),
        .retry = MAKE_BIN_REF(audio_dog_retry_adpcm),
    },
    [YAKKA_VOCAB_BOOK] = {
        .intro = MAKE_BIN_REF(audio_book_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_book_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_book_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_book_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_book_q_adpcm),
        .retry = MAKE_BIN_REF(audio_book_retry_adpcm),
    },
    [YAKKA_VOCAB_BOOTS] = {
        .intro = MAKE_BIN_REF(audio_boots_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_boots_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_boots_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_boots_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_boots_q_adpcm),
        .retry = MAKE_BIN_REF(audio_boots_retry_adpcm),
    },
    [YAKKA_VOCAB_BIKE] = {
        .intro = MAKE_BIN_REF(audio_bike_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_bike_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_bike_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_bike_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_bike_q_adpcm),
        .retry = MAKE_BIN_REF(audio_bike_retry_adpcm),
    },
    [YAKKA_VOCAB_DUCK] = {
        .intro = MAKE_BIN_REF(audio_duck_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_duck_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_duck_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_duck_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_duck_q_adpcm),
        .retry = MAKE_BIN_REF(audio_duck_retry_adpcm),
    },
    [YAKKA_VOCAB_CUP] = {
        .intro = MAKE_BIN_REF(audio_cup_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_cup_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_cup_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_cup_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_cup_q_adpcm),
        .retry = MAKE_BIN_REF(audio_cup_retry_adpcm),
    },
    [YAKKA_VOCAB_BUS] = {
        .intro = MAKE_BIN_REF(audio_bus_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_bus_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_bus_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_bus_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_bus_q_adpcm),
        .retry = MAKE_BIN_REF(audio_bus_retry_adpcm),
    },
    [YAKKA_VOCAB_PEAS] = {
        .intro = MAKE_BIN_REF(audio_peas_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_peas_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_peas_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_peas_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_peas_q_adpcm),
        .retry = MAKE_BIN_REF(audio_peas_retry_adpcm),
    },
    [YAKKA_VOCAB_HAT] = {
        .intro = MAKE_BIN_REF(audio_hat_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_hat_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_hat_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_hat_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_hat_q_adpcm),
        .retry = MAKE_BIN_REF(audio_hat_retry_adpcm),
    },
    [YAKKA_VOCAB_WORM] = {
        .intro = MAKE_BIN_REF(audio_worm_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_worm_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_worm_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_worm_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_worm_q_adpcm),
        .retry = MAKE_BIN_REF(audio_worm_retry_adpcm),
    },
    [YAKKA_VOCAB_BOAT] = {
        .intro = MAKE_BIN_REF(audio_boat_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_boat_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_boat_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_boat_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_boat_q_adpcm),
        .retry = MAKE_BIN_REF(audio_boat_retry_adpcm),
    },
    [YAKKA_VOCAB_HOUSE] = {
        .intro = MAKE_BIN_REF(audio_house_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_house_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_house_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_house_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_house_q_adpcm),
        .retry = MAKE_BIN_REF(audio_house_retry_adpcm),
    },
    [YAKKA_VOCAB_LION] = {
        .intro = MAKE_BIN_REF(audio_lion_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_lion_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_lion_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_lion_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_lion_q_adpcm),
        .retry = MAKE_BIN_REF(audio_lion_retry_adpcm),
    },
    [YAKKA_VOCAB_APPLE] = {
        .intro = MAKE_BIN_REF(audio_apple_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_apple_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_apple_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_apple_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_apple_q_adpcm),
        .retry = MAKE_BIN_REF(audio_apple_retry_adpcm),
    },
    [YAKKA_VOCAB_BALL] = {
        .intro = MAKE_BIN_REF(audio_ball_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_ball_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_ball_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_ball_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_ball_q_adpcm),
        .retry = MAKE_BIN_REF(audio_ball_retry_adpcm),
    },
    [YAKKA_VOCAB_MOUSE] = {
        .intro = MAKE_BIN_REF(audio_mouse_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_mouse_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_mouse_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_mouse_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_mouse_q_adpcm),
        .retry = MAKE_BIN_REF(audio_mouse_retry_adpcm),
    },
    [YAKKA_VOCAB_BEANS] = {
        .intro = MAKE_BIN_REF(audio_beans_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_beans_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_beans_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_beans_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_beans_q_adpcm),
        .retry = MAKE_BIN_REF(audio_beans_retry_adpcm),
    },
    [YAKKA_VOCAB_CAR] = {
        .intro = MAKE_BIN_REF(audio_car_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_car_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_car_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_car_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_car_q_adpcm),
        .retry = MAKE_BIN_REF(audio_car_retry_adpcm),
    },
    [YAKKA_VOCAB_BED] = {
        .intro = MAKE_BIN_REF(audio_bed_intro_adpcm),
        .c1    = MAKE_BIN_REF(audio_bed_c1_adpcm),
        .c2    = MAKE_BIN_REF(audio_bed_c2_adpcm),
        .c3    = MAKE_BIN_REF(audio_bed_c3_adpcm),
        .q     = MAKE_BIN_REF(audio_bed_q_adpcm),
        .retry = MAKE_BIN_REF(audio_bed_retry_adpcm),
    },
};

static inline yakka_pcm_asset_t to_pcm_asset(bin_ref_t ref) {
    if (!ref.start || !ref.end || ref.end <= ref.start) {
        return (yakka_pcm_asset_t){ .data = NULL, .size = 0, .is_adpcm = false };
    }
    return (yakka_pcm_asset_t){
        .data = ref.start,
        .size = (size_t)(ref.end - ref.start),
        .is_adpcm = true,
    };
}

const lv_image_dsc_t *yakka_asset_get_dee_image(yakka_state_t state) {
    ensure_dscs_inited();
    if (state == YAKKA_STATE_NAP_TIME) return &s_jpg_dscs[49];
    if (state == YAKKA_STATE_QUESTION || state == YAKKA_STATE_ENCOURAGE_RETRY) return &s_jpg_dscs[48];
    return &s_jpg_dscs[50];
}

const lv_image_dsc_t *yakka_asset_get_image(yakka_vocab_t vocab, yakka_state_t state) {
    ensure_dscs_inited();
    if (vocab < 0 || vocab >= YAKKA_VOCAB_COUNT) return &s_jpg_dscs[50];
    const vocab_img_map_t *m = &s_vocab_img_map[vocab];
    if (state == YAKKA_STATE_CONTRAST_1) return &s_jpg_dscs[m->c1_idx];
    if (state == YAKKA_STATE_CONTRAST_2) return &s_jpg_dscs[m->c2_idx];
    if (state == YAKKA_STATE_CONTRAST_3) return &s_jpg_dscs[m->c3_idx];
    return &s_jpg_dscs[m->intro_idx];
}

yakka_pcm_asset_t yakka_asset_audio_boot(void)      { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_boot_adpcm)); }
yakka_pcm_asset_t yakka_asset_audio_cheer(void)     { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_cheer_adpcm)); }
yakka_pcm_asset_t yakka_asset_audio_skip(void)      { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_skip_adpcm)); }
yakka_pcm_asset_t yakka_asset_audio_nap(void)       { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_nap_adpcm)); }
yakka_pcm_asset_t yakka_asset_audio_pop(void)       { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_pop_adpcm)); }
yakka_pcm_asset_t yakka_asset_audio_parent_on(void) { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_parent_on_adpcm)); }

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

    if (vocab < 0 || vocab >= YAKKA_VOCAB_COUNT) return (yakka_pcm_asset_t){ .data = NULL, .size = 0, .is_adpcm = false };
    const vocab_audio_bundle_t *b = &s_vocab_audios[vocab];
    if (action == YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO) return to_pcm_asset(b->intro);
    if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)  return to_pcm_asset(b->c1);
    if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)  return to_pcm_asset(b->c2);
    if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)  return to_pcm_asset(b->c3);
    if (action == YAKKA_ACTION_PLAY_AUDIO_QUESTION)    return to_pcm_asset(b->q);
    if (action == YAKKA_ACTION_PLAY_AUDIO_RETRY)       return to_pcm_asset(b->retry);
    return (yakka_pcm_asset_t){ .data = NULL, .size = 0, .is_adpcm = false };
}
