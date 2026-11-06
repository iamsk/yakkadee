// main/yakka_state.c
// Yakka Dee 口袋开口机 —— 纯逻辑状态机实现
#include "yakka_state.h"
#include <string.h>

#define BOOT_DURATION_MS        2000
#define VOCAB_INTRO_DURATION_MS 1500
#define CONTRAST_DURATION_MS    1500
#define QUESTION_TIMEOUT_MS     6000
#define RETRY_TIMEOUT_MS        4000
#define SUCCESS_DURATION_MS     2500
#define NAP_TRIGGER_TIME_MS     300000  // 5 分钟
#define NAP_TRIGGER_WORDS       6       // 6 组词汇
#define NAP_ANIM_DURATION_MS    3000    // 睡前打哈欠动画
#define NAP_TOTAL_LOCK_MS       180000  // 锁屏休眠 3 分钟
#define PARENT_UNLOCK_HOLD_MS   3000    // 上+下 按住 3 秒解锁
#define PARENT_AUTO_LOCK_MS     8000    // 8 秒无操作自动重锁

static const char *s_vocab_names[YAKKA_VOCAB_COUNT] = {
    "DOG",
    "BANANA",
    "DUCK",
    "CAR",
    "APPLE",
};

static const char *s_contrast_labels[YAKKA_VOCAB_COUNT][3] = {
    { "Big dog!", "Tiny dog!", "Silly dog!" },
    { "Big banana!", "Long banana!", "Yellow banana!" },
    { "Swimming duck!", "Quacking duck!", "Flying duck!" },
    { "Fast car!", "Red car!", "Beep-beep car!" },
    { "Red apple!", "Big apple!", "Yummy apple!" },
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
    m->current_vocab = YAKKA_VOCAB_DOG;
    m->state_enter_time_ms = now_ms;
    m->play_start_time_ms = now_ms;
    m->completed_vocab_count = 0;
    m->nap_screen_dimmed = false;

    // 出厂默认设置
    m->volume = YAKKA_VOL_MED;             // 60% 安全音量
    m->mic_sensitivity = YAKKA_MIC_SENS_MED;
    m->selected_vocab_loop = YAKKA_LOOP_ALL;
    m->parent_menu_cursor = 0;
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

    // 1. 检查家长双键按住解锁
    if (m->up_pressed && m->down_pressed && m->state != YAKKA_STATE_PARENT_MENU) {
        if (m->combo_start_time_ms == 0) {
            m->combo_start_time_ms = now_ms;
        } else if (now_ms - m->combo_start_time_ms >= PARENT_UNLOCK_HOLD_MS) {
            // 解锁成功! 进入家长菜单
            m->state = YAKKA_STATE_PARENT_MENU;
            m->state_enter_time_ms = now_ms;
            m->last_parent_action_ms = now_ms;
            m->parent_unlocked = true;
            m->combo_start_time_ms = 0;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_PARENT_ON);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            actions |= (1u << YAKKA_ACTION_STOP_MIC);
            actions |= (1u << YAKKA_ACTION_SET_BACKLIGHT); // 确保背光亮起
            return actions;
        }
    } else {
        m->combo_start_time_ms = 0;
    }

    // 2. 家长菜单状态: 8秒无操作自动重锁退出
    if (m->state == YAKKA_STATE_PARENT_MENU) {
        if (now_ms - m->last_parent_action_ms >= PARENT_AUTO_LOCK_MS) {
            // 自动重锁并回到游戏主线
            m->parent_unlocked = false;
            m->state = YAKKA_STATE_VOCAB_INTRO;
            m->state_enter_time_ms = now_ms;
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO);
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
        }
        return actions;
    }

    // 3. 睡午觉状态
    if (m->state == YAKKA_STATE_NAP_TIME) {
        if (!m->nap_screen_dimmed && elapsed >= NAP_ANIM_DURATION_MS) {
            // 动画播放完毕，关闭背光黑屏断电
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

    // 4. 正常幼童零按键流转状态机
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
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_QUESTION);
                actions |= (1u << YAKKA_ACTION_START_MIC);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_QUESTION:
            // 等待出声或 6 秒超时
            if (elapsed >= QUESTION_TIMEOUT_MS) {
                // 静默超时: 进入温和鼓励
                m->state = YAKKA_STATE_ENCOURAGE_RETRY;
                m->state_enter_time_ms = now_ms;
                actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_RETRY);
                actions |= (1u << YAKKA_ACTION_UPDATE_UI);
            }
            break;

        case YAKKA_STATE_ENCOURAGE_RETRY:
            // 再次等待 4 秒，若仍未出声则无痛平滑跳到下一词
            if (elapsed >= RETRY_TIMEOUT_MS) {
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
            }
            break;

        case YAKKA_STATE_SUCCESS:
            if (elapsed >= SUCCESS_DURATION_MS) {
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
        m->state = YAKKA_STATE_SUCCESS;
        m->state_enter_time_ms = now_ms;
        uint32_t actions = 0;
        actions |= (1u << YAKKA_ACTION_STOP_MIC);
        actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_SUCCESS);
        actions |= (1u << YAKKA_ACTION_UPDATE_UI);
        return actions;
    }
    return 0;
}

uint32_t yakka_machine_on_button(yakka_machine_t *m, int btn, bool is_press, bool is_click, uint32_t now_ms) {
    if (!m) return 0;
    uint32_t actions = 0;

    // 记录按键按压状态 (btn: 0=UP, 1=DOWN, 2=OK)
    if (btn == 0) m->up_pressed = is_press;
    if (btn == 1) m->down_pressed = is_press;

    // 维护双键同时按下的起始时间戳
    if (m->up_pressed && m->down_pressed) {
        if (m->combo_start_time_ms == 0) {
            m->combo_start_time_ms = now_ms;
        }
    } else {
        m->combo_start_time_ms = 0;
    }

    // 幼儿玩耍模式下: 幼童拍按任何单键仅发出轻柔水滴啵啵声，绝不打断业务流程
    if (m->state != YAKKA_STATE_PARENT_MENU) {
        if (is_press) {
            actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_POP);
        }
        return actions;
    }

    // --- 以下为家长模式 (YAKKA_STATE_PARENT_MENU) 按键控制 ---
    m->last_parent_action_ms = now_ms;

    if (is_click) {
        // 菜单项:
        // 0: 音量调节 (40% -> 60% -> 80% -> 40%)
        // 1: 麦克风灵敏度 (低 -> 中 -> 高 -> 低)
        // 2: 词汇循环 (全部 -> DOG -> BANANA -> DUCK -> CAR -> APPLE -> 全部)
        // 3: 立即睡午觉 / 休眠
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
                case 1: // 麦克风灵敏度
                    m->mic_sensitivity = (yakka_mic_sens_t)((m->mic_sensitivity + 1) % 3);
                    break;
                case 2: // 词汇循环
                    m->selected_vocab_loop++;
                    if (m->selected_vocab_loop >= YAKKA_VOCAB_COUNT) {
                        m->selected_vocab_loop = YAKKA_LOOP_ALL;
                    }
                    if (m->selected_vocab_loop >= 0) {
                        m->current_vocab = (yakka_vocab_t)m->selected_vocab_loop;
                    }
                    break;
                case 3: // 立即睡午觉
                    m->state = YAKKA_STATE_NAP_TIME;
                    m->state_enter_time_ms = now_ms;
                    m->nap_screen_dimmed = false;
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_NAP);
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                    return actions;
                case 4: // 保存并退出
                    m->state = YAKKA_STATE_VOCAB_INTRO;
                    m->state_enter_time_ms = now_ms;
                    actions |= (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO);
                    actions |= (1u << YAKKA_ACTION_UPDATE_UI);
                    return actions;
            }
            actions |= (1u << YAKKA_ACTION_UPDATE_UI);
        }
    }

    return actions;
}
