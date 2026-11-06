// main/confetti.c
// Yakka Dee 口袋开口机 —— 护眼慢速彩带粒子引擎实现
#include "confetti.h"
#include <math.h>
#include <string.h>

// 严格遵守护眼饱和度限制 (70% ~ 80%) 的柔和马卡龙大色块
static const uint32_t EYE_CARE_COLORS[] = {
    0xFFB4A2, // 暖柔珊瑚
    0xB5E2FA, // 柔和天空蓝
    0xFCE7A4, // 暖乳黄
    0xDBCDF0, // 柔和熏衣草紫
    0xFFDAC1, // 柔和蜜桃
    0xB8E0D2, // 柔和薄荷绿
};
#define COLOR_COUNT (sizeof(EYE_CARE_COLORS) / sizeof(EYE_CARE_COLORS[0]))

void confetti_init(confetti_engine_t *ce, int screen_w, int screen_h) {
    if (!ce) return;
    ce->screen_w = screen_w > 0 ? screen_w : 240;
    ce->screen_h = screen_h > 0 ? screen_h : 320;
    ce->active = false;
    confetti_reset(ce);
}

void confetti_reset(confetti_engine_t *ce) {
    if (!ce) return;
    ce->active = true;

    // 预设确定的分布，保证跨平台、无硬随机数的确定性与平滑性
    static const int x_slots[CONFETTI_PARTICLE_COUNT] = {
        24, 46, 68, 90, 112, 134, 156, 178, 200, 216
    };
    static const int y_offsets[CONFETTI_PARTICLE_COUNT] = {
        -15, -45, -25, -65, -10, -50, -30, -70, -20, -55
    };
    static const float speeds[CONFETTI_PARTICLE_COUNT] = {
        2.4f, 2.8f, 2.2f, 3.0f, 2.6f, 2.3f, 2.9f, 2.5f, 2.7f, 3.1f
    };
    static const float sways[CONFETTI_PARTICLE_COUNT] = {
        5.0f, 6.0f, 4.5f, 7.0f, 5.5f, 6.5f, 4.0f, 7.5f, 5.0f, 6.0f
    };

    for (int i = 0; i < CONFETTI_PARTICLE_COUNT; i++) {
        confetti_particle_t *p = &ce->particles[i];
        p->base_x = (float)x_slots[i];
        p->x = p->base_x;
        p->y = (float)y_offsets[i];
        p->vy = speeds[i];
        p->sway_amp = sways[i];
        p->sway_speed = 0.08f + (float)i * 0.01f;
        p->phase = (float)i * 0.6f;
        p->size_w = 14 + (i % 3) * 2; // 14 ~ 18 px
        p->size_h = 10 + (i % 2) * 4; // 10 ~ 14 px
        p->shape = (confetti_shape_t)(i % CONFETTI_SHAPE_COUNT);
        p->color_hex = EYE_CARE_COLORS[i % COLOR_COUNT];
    }
}

void confetti_step(confetti_engine_t *ce) {
    if (!ce || !ce->active) return;

    for (int i = 0; i < CONFETTI_PARTICLE_COUNT; i++) {
        confetti_particle_t *p = &ce->particles[i];
        p->y += p->vy;
        p->phase += p->sway_speed;
        p->x = p->base_x + sinf(p->phase) * p->sway_amp;

        // 若落出屏幕底端，循环从顶部重新落下，形成持续温和雨落感
        if (p->y > (float)(ce->screen_h + 20)) {
            p->y = -20.0f;
        }
    }
}
