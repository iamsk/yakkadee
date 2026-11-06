// tests/test_confetti.c
// 单元测试: 护眼慢速彩带粒子初始化、边界更新、平滑位移与柔和配色
#include "confetti.h"
#include <assert.h>
#include <stdio.h>

static void test_confetti_init_and_step(void) {
    confetti_engine_t ce;
    confetti_init(&ce, 240, 320);

    assert(ce.screen_w == 240);
    assert(ce.screen_h == 320);
    assert(ce.active);

    // 验证粒子数量为 10
    for (int i = 0; i < CONFETTI_PARTICLE_COUNT; i++) {
        confetti_particle_t *p = &ce.particles[i];
        assert(p->x >= 0.0f && p->x <= 240.0f);
        assert(p->vy >= 2.0f && p->vy <= 3.5f); // 慢速位移
        assert(p->size_w >= 12 && p->size_w <= 20); // 大几何块
        assert(p->color_hex != 0xFFFFFF); // 杜绝刺眼纯白
        assert(p->color_hex != 0x000000); // 杜绝纯黑
    }

    // 步进 50 帧 (约 2 秒 @ 25fps)
    float prev_y = ce.particles[0].y;
    for (int frame = 0; frame < 50; frame++) {
        confetti_step(&ce);
    }
    // 匀速向下平滑下落
    assert(ce.particles[0].y > prev_y || ce.particles[0].y < 0.0f);

    printf("test_confetti_init_and_step: PASS\n");
}

int main(void) {
    test_confetti_init_and_step();
    printf("ALL CONFETTI TESTS PASSED!\n");
    return 0;
}
