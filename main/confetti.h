// main/confetti.h
// Yakka Dee 口袋开口机 —— 护眼慢速彩带粒子引擎
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFETTI_PARTICLE_COUNT 10

typedef enum {
    CONFETTI_SHAPE_RECT = 0,
    CONFETTI_SHAPE_CIRCLE,
    CONFETTI_SHAPE_DIAMOND,
    CONFETTI_SHAPE_COUNT,
} confetti_shape_t;

typedef struct {
    float x;
    float y;
    float base_x;
    float vy;          // 下落匀速 2.0 ~ 3.5 px/帧
    float sway_amp;    // 左右摇摆幅度 (4 ~ 8 px)
    float sway_speed;  // 摇摆角速度
    float phase;       // 当前相位
    int16_t size_w;    // 宽度 (12 ~ 18 px 大几何块)
    int16_t size_h;    // 高度
    confetti_shape_t shape;
    uint32_t color_hex;
} confetti_particle_t;

typedef struct {
    int screen_w;
    int screen_h;
    confetti_particle_t particles[CONFETTI_PARTICLE_COUNT];
    bool active;
} confetti_engine_t;

// 初始化彩带引擎
void confetti_init(confetti_engine_t *ce, int screen_w, int screen_h);

// 重新激活彩带爆发 (重置所有粒子至顶部)
void confetti_reset(confetti_engine_t *ce);

// 步进一帧 (25 fps 推荐步长 40ms)
void confetti_step(confetti_engine_t *ce);

#ifdef __cplusplus
}
#endif
