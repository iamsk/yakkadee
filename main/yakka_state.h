// main/yakka_state.h
// Yakka Dee 口袋开口机 —— 纯逻辑状态机模型
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 5 组基础趣味词汇
typedef enum {
    YAKKA_VOCAB_DOG = 0,
    YAKKA_VOCAB_BANANA,
    YAKKA_VOCAB_DUCK,
    YAKKA_VOCAB_CAR,
    YAKKA_VOCAB_APPLE,
    YAKKA_VOCAB_COUNT,
} yakka_vocab_t;

// 主交互流程状态
typedef enum {
    YAKKA_STATE_BOOT_INTRO = 0,   // 开机引导: "Yakka Dee, talk talk talk!" (2.0s)
    YAKKA_STATE_VOCAB_INTRO,      // 阶段一: 引出词汇 "Let's talk about DOG!" (1.5s)
    YAKKA_STATE_CONTRAST_1,       // 阶段二.1: Big dog (1.5s)
    YAKKA_STATE_CONTRAST_2,       // 阶段二.2: Tiny dog (1.5s)
    YAKKA_STATE_CONTRAST_3,       // 阶段二.3: Silly dog (1.5s)
    YAKKA_STATE_QUESTION,         // 阶段三: You say it! 开启麦克风侦听 (最长 6.0s)
    YAKKA_STATE_SUCCESS,          // 阶段四: 正向大爆发! 彩带 + 欢呼 (2.5s)
    YAKKA_STATE_ENCOURAGE_RETRY,  // 阶段四(静默): 温和鼓励重试 "Say DOG!" (最长 4.0s)
    YAKKA_STATE_NAP_TIME,         // 生理防沉迷: Dee 睡午觉啦 (15分钟/25词触发, 温和微光睡眠)
    YAKKA_STATE_PARENT_MENU,      // 家长设置菜单 (长按 OK 键 2 秒进入)
} yakka_state_t;

// 音量档位 (60% / 80% / 100%)
typedef enum {
    YAKKA_VOL_LOW = 60,
    YAKKA_VOL_MED = 80,
    YAKKA_VOL_HIGH = 100,
} yakka_volume_t;

// 麦克风灵敏度档位
typedef enum {
    YAKKA_MIC_SENS_LOW = 0,
    YAKKA_MIC_SENS_MED,
    YAKKA_MIC_SENS_HIGH,
} yakka_mic_sens_t;

// 词汇循环模式
typedef enum {
    YAKKA_LOOP_ALL = -1,  // 顺序循环全部 5 词
    // >= 0 为锁定循环特定单一词汇 (yakka_vocab_t)
} yakka_loop_mode_t;

// 状态机配置与运行时状态
typedef struct {
    // 当前状态与词汇
    yakka_state_t state;
    yakka_vocab_t current_vocab;
    uint32_t state_enter_time_ms;     // 当前状态进入时间戳

    // 防沉迷计数
    uint32_t play_start_time_ms;      // 连续游戏开始时间戳
    uint32_t completed_vocab_count;   // 本轮已完成的词汇数
    bool nap_screen_dimmed;           // 睡午觉是否已进入微光睡眠

    // 家长设置
    yakka_volume_t volume;
    uint8_t backlight_percent;        // 背光亮度 (25% / 50% / 75%)
    yakka_mic_sens_t mic_sensitivity;
    int selected_vocab_loop;          // YAKKA_LOOP_ALL 或特定词汇
    uint32_t last_parent_action_ms;   // 家长最后操作时间戳 (8秒无操作自动重锁)
    int parent_menu_cursor;           // 家长菜单当前高亮项 (0..4)

    bool parent_unlocked;
} yakka_machine_t;

// 状态机触发动作指令
typedef enum {
    YAKKA_ACTION_NONE = 0,
    YAKKA_ACTION_PLAY_AUDIO_BOOT,
    YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO,
    YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1,
    YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2,
    YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3,
    YAKKA_ACTION_PLAY_AUDIO_QUESTION,
    YAKKA_ACTION_PLAY_AUDIO_SUCCESS,
    YAKKA_ACTION_PLAY_AUDIO_RETRY,
    YAKKA_ACTION_PLAY_AUDIO_SKIP,
    YAKKA_ACTION_PLAY_AUDIO_NAP,
    YAKKA_ACTION_PLAY_AUDIO_POP,        // 按钮轻微触觉水滴音
    YAKKA_ACTION_PLAY_AUDIO_PARENT_ON,  // 家长解锁升调音
    YAKKA_ACTION_UPDATE_UI,
    YAKKA_ACTION_START_MIC,
    YAKKA_ACTION_STOP_MIC,
    YAKKA_ACTION_SET_BACKLIGHT,         // 调整背光 (正常 25% / 休眠 0%)
} yakka_action_t;

// 初始化状态机
void yakka_machine_init(yakka_machine_t *m, uint32_t now_ms);

// 时间步进 Tick，由主循环定期调用 (如每 20ms ~ 50ms)
// 返回本次步进需执行的动作标志掩码 (按位或)
uint32_t yakka_machine_tick(yakka_machine_t *m, uint32_t now_ms);

// 麦克风检测到宝宝声音能量峰值
uint32_t yakka_machine_on_voice_detected(yakka_machine_t *m, uint32_t now_ms);

// 物理按键事件输入
// btn: 0=UP, 1=DOWN, 2=OK
// is_press: true=按下, false=松开
// is_click: true=单击
// is_long: true=长按 (>= 500ms)
uint32_t yakka_machine_on_button(yakka_machine_t *m, int btn, bool is_press, bool is_click, bool is_long, uint32_t now_ms);

// 获取当前词汇名称 (如 "DOG")
const char *yakka_vocab_name(yakka_vocab_t vocab);

// 获取当前词汇对比变体名称 (如 "Big dog", "Tiny dog", "Silly dog")
const char *yakka_contrast_label(yakka_vocab_t vocab, int contrast_idx);

#ifdef __cplusplus
}
#endif
