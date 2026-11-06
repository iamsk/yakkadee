// main/ui_yakka.h
// Yakka Dee 口袋开口机 —— 护眼专用应用交互界面
#pragma once

#include "lvgl.h"
#include "yakka_state.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 初始化 Yakka Dee 专用护眼界面
void ui_yakka_init(void);

// 根据状态机状态刷新 UI 显示 (在 LVGL 线程中执行或持有 bsp_lvgl_lock)
void ui_yakka_update(const yakka_machine_t *m, int battery_soc, int battery_mv);

// 彩带动画帧步进 (约 25~30 fps)
void ui_yakka_confetti_step(void);

// 显示或隐藏彩带层
void ui_yakka_show_confetti(bool show);

// 呼吸灯光圈动画步进 (提问阶段)
void ui_yakka_pulse_step(uint32_t now_ms);

#ifdef __cplusplus
}
#endif
