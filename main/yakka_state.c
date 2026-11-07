// main/yakka_state.c
// Yakka Dee 口袋开口机 —— 纯逻辑状态机实现
#include "yakka_state.h"
#include <string.h>

#define BOOT_DURATION_MS        YAKKA_BOOT_DURATION_MS
#define VOCAB_INTRO_DURATION_MS YAKKA_VOCAB_INTRO_DURATION_MS
#define CONTRAST_DURATION_MS    YAKKA_CONTRAST_DURATION_MS
#define QUESTION_TIMEOUT_MS     YAKKA_QUESTION_TIMEOUT_MS
#define RETRY_TIMEOUT_MS        YAKKA_RETRY_TIMEOUT_MS
#define SUCCESS_DURATION_MS     YAKKA_SUCCESS_DURATION_MS
#define NAP_TRIGGER_TIME_MS     YAKKA_NAP_TRIGGER_TIME_MS
#define NAP_TRIGGER_WORDS       YAKKA_NAP_TRIGGER_WORDS
#define NAP_ANIM_DURATION_MS    YAKKA_NAP_ANIM_DURATION_MS
#define NAP_TOTAL_LOCK_MS       YAKKA_NAP_TOTAL_LOCK_MS
#define PARENT_AUTO_LOCK_MS     YAKKA_PARENT_AUTO_LOCK_MS

static const char *s_vocab_names[YAKKA_VOCAB_COUNT] = {
    "BANANA",
    "DOG",
    "BOOK",
    "BOOTS",
    "BIKE",
    "DUCK",
    "CUP",
    "BUS",
    "PEAS",
    "HAT",
    "WORM",
    "BOAT",
    "HOUSE",
    "LION",
    "APPLE",
    "BALL",
    "MOUSE",
    "BEANS",
    "CAR",
    "BED",
};

static const char *s_contrast_labels[YAKKA_VOCAB_COUNT][3] = {
    { "Big banana!", "Long banana!", "Yellow banana!" },
    { "Big dog!", "Tiny dog!", "Silly dog!" },
    { "Big book!", "Story book!", "Open book!" },
    { "Red boots!", "Muddy boots!", "Shiny boots!" },
    { "Fast bike!", "Little bike!", "Ring-ring bike!" },
    { "Swimming duck!", "Quacking duck!", "Yellow duck!" },
    { "Blue cup!", "Big cup!", "Sippy cup!" },
    { "Big bus!", "Red bus!", "Beep-beep bus!" },
    { "Green peas!", "Tiny peas!", "Sweet peas!" },
    { "Sun hat!", "Silly hat!", "Warm hat!" },
    { "Wiggly worm!", "Long worm!", "Earth worm!" },
    { "Sailing boat!", "Speed boat!", "Floating boat!" },
    { "Cozy house!", "Tall house!", "Brick house!" },
    { "Roaring lion!", "Big lion!", "Golden lion!" },
    { "Red apple!", "Sweet apple!", "Crunchy apple!" },
    { "Bouncing ball!", "Round ball!", "Rolling ball!" },
    { "Little mouse!", "Squeaky mouse!", "Quiet mouse!" },
    { "Magic beans!", "Green beans!", "Jumping beans!" },
    { "Fast car!", "Red car!", "Beep-beep car!" },
    { "Soft bed!", "Sleepy bed!", "Cozy bed!" },
};

const char *yakka_vocab_name(yakka_vocab_t vocab) {
    if (vocab < 0 || vocab >= YAKKA_VOCAB_COUNT) return "";
    return s_vocab_names[vocab];
}

const char *yakka_contrast_label(yakka_vocab_t vocab, int contrast_idx) {
    if (vocab < 0 || vocab >= YAKKA_VOCAB_COUNT) return "";
    if (contrast_idx < 0 || contrast_idx >= 3) return "";
    return s_contrast_labels[vocab][contrast_idx];
}

void yakka_machine_init(yakka_machine_t *m, uint32_t now_ms) {
    if (!m) return;
    memset(m, 0, sizeof(*m));
    m->state = YAKKA_STATE_BOOT_INTRO;
    m->current_vocab = YAKKA_VOCAB_BANANA;
    m->state_enter_time_ms = now_ms;
    m->play_start_time_ms = now_ms;
    m->completed_vocab_count = 0;
    m->nap_screen_dimmed = false;

    // 出厂默认设置
    m->volume = YAKKA_VOL_HIGH;            // 默认 100% 饱满音量
    m->backlight_percent = 50;             // 默认 50% 清晰护眼亮度
    m->mic_sensitivity = YAKKA_MIC_SENS_MED;
    m->selected_vocab_loop = YAKKA_LOOP_ALL;
    m->parent_menu_cursor = 0;
    m->parent_unlocked = false;

    // 发声响应延迟初始化
    m->voice_pending = false;
    m->voice_first_heard_ms = 0;
    m->voice_last_heard_ms = 0;
}

static yakka_vocab_t get_next_vocab(const yakka_machine_t *m) {
    if (m->selected_vocab_loop >= 0 && m->selected_vocab_loop < YAKKA_VOCAB_COUNT) {
        return (yakka_vocab_t)m->selected_vocab_loop;
    }
    return (yakka_vocab_t)((m->current_vocab + 1) % YAKKA_VOCAB_COUNT);
}

static bool check_nap_time_trigger(const yakka_machine_t *m, uint32_t now_ms) {
    if (now_ms - m->play_start_time_ms >= NAP_TRIGGER_TIME_MS) {
        return true;
    }
    if (m->completed_vocab_count >= NAP_TRIGGER_WORDS) {
        return true;
    }
    return false;
}

uint32_t yakka_machine_tick(yakka_machine_t *m, uint32_t now_ms) {
    if (!m) return 0;
    uint32_t actions = 0;
    uint32_t elapsed = now_ms - m->state_enter_time_ms;

    // 1. 家长菜单状态: 8秒无操作自动重锁退出
    if (m->state == YAKKA_STATE_PARENT_MENU) {
        if (now_ms - m->last_parent_action_ms >= PARENT_AUTO_LOCK_MS) {
            // 自动重锁并回到游戏主线
            m->parent_unlocked = false;
            m->state = YAKKA_STATE_VOCAB_INTRO;
            m->state_enter_time_ms = now_ms;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT);
        }
        return actions;
    }

    // 2. 睡午觉状态
    if (m->state == YAKKA_STATE_NAP_TIME) {
        if (!m->nap_screen_dimmed && elapsed >= NAP_ANIM_DURATION_MS) {
            // 动画播放完毕，调暗背光进入微光安抚睡眠
            m->nap_screen_dimmed = true;
            actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT);
        }
        if (elapsed >= NAP_TOTAL_LOCK_MS) {
            // 睡满 3 分钟，睡饱饱醒来继续玩
            m->state = YAKKA_STATE_BOOT_INTRO;
            m->state_enter_time_ms = now_ms;
            m->play_start_time_ms = now_ms;
            m->completed_vocab_count = 0;
            m->nap_screen_dimmed = false;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_BOOT);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT);
        }
        return actions;
    }

    // 3. 正常幼童零按键流转状态机
    switch (m->state) {
        case YAKKA_STATE_BOOT_INTRO:
            if (elapsed >= BOOT_DURATION_MS) {
                m->state = YAKKA_STATE_VOCAB_INTRO;
                m->state_enter_time_ms = now_ms;
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_VOCAB_INTRO:
            if (elapsed >= VOCAB_INTRO_DURATION_MS) {
                m->state = YAKKA_STATE_CONTRAST_1;
                m->state_enter_time_ms = now_ms;
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_CONTRAST_1:
            if (elapsed >= CONTRAST_DURATION_MS) {
                m->state = YAKKA_STATE_CONTRAST_2;
                m->state_enter_time_ms = now_ms;
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_CONTRAST_2:
            if (elapsed >= CONTRAST_DURATION_MS) {
                m->state = YAKKA_STATE_CONTRAST_3;
                m->state_enter_time_ms = now_ms;
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_CONTRAST_3:
            if (elapsed >= CONTRAST_DURATION_MS) {
                m->state = YAKKA_STATE_QUESTION;
                m->state_enter_time_ms = now_ms;
                m->voice_pending = false;
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_QUESTION);
                actions |= (1u << YAKKA_ACTION_START_MIC);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_QUESTION:
            if (m->voice_pending) {
                // 侦测到宝宝发音，等待念完并留足自然停顿 (防打断/防抢话)
                uint32_t speech_dur = now_ms - m->voice_first_heard_ms;
                uint32_t silence_gap = now_ms - m->voice_last_heard_ms;
                if ((speech_dur >= YAKKA_VOICE_MIN_DELAY_MS && silence_gap >= YAKKA_VOICE_SILENCE_GAP_MS) ||
                    (speech_dur >= YAKKA_VOICE_MAX_WAIT_MS)) {
                    m->voice_pending = false;
                    m->state = YAKKA_STATE_SUCCESS;
                    m->state_enter_time_ms = now_ms;
                    actions |= (1u << YAKKA_ACTION_STOP_MIC);
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_SUCCESS);
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                }
            } else if (elapsed >= QUESTION_TIMEOUT_MS) {
                // 静默超时: 进入温和鼓励
                m->state = YAKKA_STATE_ENCOURAGE_RETRY;
                m->state_enter_time_ms = now_ms;
                m->voice_pending = false;
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_RETRY);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_ENCOURAGE_RETRY:
            if (m->voice_pending) {
                // 侦测到宝宝发音，等待念完并留足自然停顿
                uint32_t speech_dur = now_ms - m->voice_first_heard_ms;
                uint32_t silence_gap = now_ms - m->voice_last_heard_ms;
                if ((speech_dur >= YAKKA_VOICE_MIN_DELAY_MS && silence_gap >= YAKKA_VOICE_SILENCE_GAP_MS) ||
                    (speech_dur >= YAKKA_VOICE_MAX_WAIT_MS)) {
                    m->voice_pending = false;
                    m->state = YAKKA_STATE_SUCCESS;
                    m->state_enter_time_ms = now_ms;
                    actions |= (1u << YAKKA_ACTION_STOP_MIC);
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_SUCCESS);
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                }
            }
            // 鼓励重试等待超时后: 暂停在此处!
            // 绝不自动跳词，麦克风保持侦听，耐心地等待宝宝发声，或由家长按键控制 (OK 键重听提示，上下键手动换词)
            break;

        case YAKKA_STATE_SUCCESS:
            if (elapsed >= SUCCESS_DURATION_MS) {
                m->voice_pending = false;
                m->completed_vocab_count++;
                if (check_nap_time_trigger(m, now_ms)) {
                    m->state = YAKKA_STATE_NAP_TIME;
                    m->state_enter_time_ms = now_ms;
                    m->nap_screen_dimmed = false;
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_NAP);
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                } else {
                    m->current_vocab = get_next_vocab(m);
                    m->state = YAKKA_STATE_VOCAB_INTRO;
                    m->state_enter_time_ms = now_ms;
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO);
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                }
            }
            break;

        default:
            break;
    }

    return actions;
}

uint32_t yakka_machine_on_voice_detected(yakka_machine_t *m, uint32_t now_ms) {
    if (!m) return 0;
    // 仅在提问阶段或重试鼓励阶段响应声音
    if (m->state == YAKKA_STATE_QUESTION || m->state == YAKKA_STATE_ENCOURAGE_RETRY) {
        if (!m->voice_pending) {
            m->voice_pending = true;
            m->voice_first_heard_ms = now_ms;
        }
        m->voice_last_heard_ms = now_ms;
        // 不立刻打断宝宝说话，由 tick 函数在宝宝念完并自然停顿后平滑进入 SUCCESS
    }
    return 0;
}

uint32_t yakka_machine_on_button(yakka_machine_t *m, int btn, bool is_press, bool is_click, bool is_long, uint32_t now_ms) {
    if (!m) return 0;
    uint32_t actions = 0;

    // 1. 睡午觉状态下:
    if (m->state == YAKKA_STATE_NAP_TIME) {
        // 长按 OK 键 (btn == 2) 随时唤醒 Dee
        if (is_long && btn == 2) {
            m->state = YAKKA_STATE_BOOT_INTRO;
            m->state_enter_time_ms = now_ms;
            m->play_start_time_ms = now_ms;
            m->completed_vocab_count = 0;
            m->nap_screen_dimmed = false;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_BOOT);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT);
            return actions;
        }
        if (is_press) {
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_POP);
        }
        return actions;
    }

    // 2. 非家长模式 (幼儿交互流)
    if (m->state != YAKKA_STATE_PARENT_MENU) {
        // 长按中间 OK 键 (btn == 2) 解锁进入家长菜单!
        if (is_long && btn == 2) {
            m->voice_pending = false;
            m->state = YAKKA_STATE_PARENT_MENU;
            m->state_enter_time_ms = now_ms;
            m->last_parent_action_ms = now_ms;
            m->parent_unlocked = true;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_PARENT_ON);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            actions |= (1u << YAKKA_ACTION_STOP_MIC);
            actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT);
            return actions;
        }

        // 提问或重试暂停阶段的按键响应:
        if (m->state == YAKKA_STATE_QUESTION || m->state == YAKKA_STATE_ENCOURAGE_RETRY) {
            if (is_click) {
                if (btn == 2) {
                    // OK 键单击: 重新播放当前提示语音 (再次鼓励宝宝读)
                    m->voice_pending = false;
                    m->state_enter_time_ms = now_ms;
                    if (m->state == YAKKA_STATE_ENCOURAGE_RETRY) {
                        actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_RETRY);
                    } else {
                        actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_QUESTION);
                    }
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                    return actions;
                } else if (btn == 1 || btn == 0) {
                    // 下键或上键单击: 手动跳到下一词汇 (方便家长切词)
                    m->voice_pending = false;
                    actions |= (1u << YAKKA_ACTION_STOP_MIC);
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_SKIP);
                    m->completed_vocab_count++;
                    if (check_nap_time_trigger(m, now_ms)) {
                        m->state = YAKKA_STATE_NAP_TIME;
                        m->state_enter_time_ms = now_ms;
                        m->nap_screen_dimmed = false;
                        actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_NAP);
                        actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                    } else {
                        m->current_vocab = get_next_vocab(m);
                        m->state = YAKKA_STATE_VOCAB_INTRO;
                        m->state_enter_time_ms = now_ms;
                        actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO);
                        actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                    }
                    return actions;
                }
            }
        }

        // 幼童拍按任何单键仅发出轻柔水滴啵啵声，绝不打断业务流程
        if (is_press) {
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_POP);
        }
        return actions;
    }

    // 3. 家长模式 (YAKKA_STATE_PARENT_MENU) 按键控制
    m->last_parent_action_ms = now_ms;

    if (is_click) {
        // 菜单项:
        // 0: 音量调节 (60% -> 80% -> 100% -> 60%)
        // 1: 背光亮度 (25% -> 50% -> 75% -> 25%)
        // 2: 麦克风灵敏度 (低 -> 中 -> 高 -> 低)
        // 3: 词汇循环 (全部 -> DOG -> BANANA -> DUCK -> CAR -> APPLE -> 全部)
        // 4: 保存并退出
        if (btn == 0) { // UP
            m->parent_menu_cursor = (m->parent_menu_cursor + 4) % 5;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_POP);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
        } else if (btn == 1) { // DOWN
            m->parent_menu_cursor = (m->parent_menu_cursor + 1) % 5;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_POP);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
        } else if (btn == 2) { // OK (切换选中项数值)
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_POP);
            switch (m->parent_menu_cursor) {
                case 0: // 音量
                    if (m->volume == YAKKA_VOL_LOW) m->volume = YAKKA_VOL_MED;
                    else if (m->volume == YAKKA_VOL_MED) m->volume = YAKKA_VOL_HIGH;
                    else m->volume = YAKKA_VOL_LOW;
                    break;
                case 1: // 背光亮度
                    if (m->backlight_percent == 25) m->backlight_percent = 50;
                    else if (m->backlight_percent == 50) m->backlight_percent = 75;
                    else m->backlight_percent = 25;
                    actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT);
                    break;
                case 2: // 麦克风灵敏度
                    m->mic_sensitivity = (yakka_mic_sens_t)((m->mic_sensitivity + 1) % 3);
                    break;
                case 3: // 词汇循环
                    m->selected_vocab_loop++;
                    if (m->selected_vocab_loop >= YAKKA_VOCAB_COUNT) {
                        m->selected_vocab_loop = YAKKA_LOOP_ALL;
                    }
                    if (m->selected_vocab_loop >= 0) {
                        m->current_vocab = (yakka_vocab_t)m->selected_vocab_loop;
                    }
                    break;
                case 4: // 保存并退出
                    m->state = YAKKA_STATE_VOCAB_INTRO;
                    m->state_enter_time_ms = now_ms;
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO);
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                    actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT);
                    return actions;
            }
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
        }
    }

    return actions;
}
