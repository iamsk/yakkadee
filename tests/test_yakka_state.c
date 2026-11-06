// tests/test_yakka_state.c
// 单元测试: Yakka Dee 状态机流转、超时兜底、防沉迷休眠、家长双键手势及防误触
#include "yakka_state.h"
#include <assert.h>
#include <stdio.h>

static void test_zero_button_flow(void) {
    yakka_machine_t m;
    uint32_t now = 1000;
    yakka_machine_init(&m, now);

    assert(m.state == YAKKA_STATE_BOOT_INTRO);
    assert(m.current_vocab == YAKKA_VOCAB_DOG);

    // 1. 开机引导 4.5 秒后自动进入 VOCAB_INTRO
    uint32_t act = yakka_machine_tick(&m, now + YAKKA_BOOT_DURATION_MS - 1);
    assert(m.state == YAKKA_STATE_BOOT_INTRO);
    act = yakka_machine_tick(&m, now + YAKKA_BOOT_DURATION_MS);
    assert(m.state == YAKKA_STATE_VOCAB_INTRO);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO));

    // 2. 引出词汇 3.0 秒后进入对比 1 (Big dog)
    now += YAKKA_BOOT_DURATION_MS;
    act = yakka_machine_tick(&m, now + YAKKA_VOCAB_INTRO_DURATION_MS);
    assert(m.state == YAKKA_STATE_CONTRAST_1);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1));

    // 3. 对比 1 -> 对比 2 (Tiny dog)
    now += YAKKA_VOCAB_INTRO_DURATION_MS;
    act = yakka_machine_tick(&m, now + YAKKA_CONTRAST_DURATION_MS);
    assert(m.state == YAKKA_STATE_CONTRAST_2);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2));

    // 4. 对比 2 -> 对比 3 (Silly dog)
    now += YAKKA_CONTRAST_DURATION_MS;
    act = yakka_machine_tick(&m, now + YAKKA_CONTRAST_DURATION_MS);
    assert(m.state == YAKKA_STATE_CONTRAST_3);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3));

    // 5. 对比 3 -> 提问 (You say it! 启动麦克风侦听)
    now += YAKKA_CONTRAST_DURATION_MS;
    act = yakka_machine_tick(&m, now + YAKKA_CONTRAST_DURATION_MS);
    assert(m.state == YAKKA_STATE_QUESTION);
    assert(act & (1u << YAKKA_ACTION_START_MIC));
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_QUESTION));

    // 6. 宝宝大声发音: 触发 SUCCESS (彩带 + 欢呼)
    now += YAKKA_CONTRAST_DURATION_MS;
    act = yakka_machine_on_voice_detected(&m, now + 500);
    assert(m.state == YAKKA_STATE_SUCCESS);
    assert(act & (1u << YAKKA_ACTION_STOP_MIC));
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_SUCCESS));

    // 7. 庆祝 3.5 秒后自动流转至下一词 (BANANA)
    now += 500;
    act = yakka_machine_tick(&m, now + YAKKA_SUCCESS_DURATION_MS);
    assert(m.state == YAKKA_STATE_VOCAB_INTRO);
    assert(m.current_vocab == YAKKA_VOCAB_BANANA);
    assert(m.completed_vocab_count == 1);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO));

    printf("test_zero_button_flow: PASS\n");
}

static void test_silence_fallback(void) {
    yakka_machine_t m;
    uint32_t now = 5000;
    yakka_machine_init(&m, now);

    // 快速推至 QUESTION 状态
    m.state = YAKKA_STATE_QUESTION;
    m.state_enter_time_ms = now;

    // 静默 10 秒超时 -> 自动进入温和鼓励重试 (不报错)
    uint32_t act = yakka_machine_tick(&m, now + YAKKA_QUESTION_TIMEOUT_MS - 1);
    assert(m.state == YAKKA_STATE_QUESTION);
    act = yakka_machine_tick(&m, now + YAKKA_QUESTION_TIMEOUT_MS);
    assert(m.state == YAKKA_STATE_ENCOURAGE_RETRY);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_RETRY));

    // 重试阶段静默 8 秒超时 -> 播放轻快转场口哨，平滑切至下一词
    now += YAKKA_QUESTION_TIMEOUT_MS;
    act = yakka_machine_tick(&m, now + YAKKA_RETRY_TIMEOUT_MS);
    assert(m.state == YAKKA_STATE_VOCAB_INTRO);
    assert(m.current_vocab == YAKKA_VOCAB_BANANA);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_SKIP));
    assert(act & (1u << YAKKA_ACTION_STOP_MIC));

    printf("test_silence_fallback: PASS\n");
}

static void test_anti_touch_pop_sound(void) {
    yakka_machine_t m;
    uint32_t now = 1000;
    yakka_machine_init(&m, now);

    // 幼童拍按或短按: 状态绝不被打断，仅发出触觉水滴啵啵声
    uint32_t act = yakka_machine_on_button(&m, 2 /* OK */, true /* press */, false, false, now);
    assert(m.state == YAKKA_STATE_BOOT_INTRO);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_POP));

    act = yakka_machine_on_button(&m, 0 /* UP */, true /* press */, false, false, now + 100);
    assert(m.state == YAKKA_STATE_BOOT_INTRO);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_POP));

    printf("test_anti_touch_pop_sound: PASS\n");
}

static void test_parent_unlock_gesture(void) {
    yakka_machine_t m;
    uint32_t now = 1000;
    yakka_machine_init(&m, now);

    // 单按/短按 OK: 不解锁
    yakka_machine_on_button(&m, 2 /* OK */, true, true, false /* not long */, now);
    assert(m.state != YAKKA_STATE_PARENT_MENU);

    // 长按 OK 键 (is_long = true) -> 解锁进入家长菜单!
    uint32_t act = yakka_machine_on_button(&m, 2 /* OK */, true, false, true /* is_long */, now + 1000);
    assert(m.state == YAKKA_STATE_PARENT_MENU);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_PARENT_ON));
    assert(m.parent_unlocked);

    // 在家长菜单中调节音量: 游标 0 处按 OK
    m.parent_menu_cursor = 0;
    assert(m.volume == YAKKA_VOL_HIGH);
    yakka_machine_on_button(&m, 2 /* OK */, false, true /* click */, false, now + 1100);
    assert(m.volume == YAKKA_VOL_LOW);

    // 8 秒无操作 -> 自动退出家长菜单并锁死
    act = yakka_machine_tick(&m, now + 1100 + 8000);
    assert(m.state == YAKKA_STATE_VOCAB_INTRO);
    assert(!m.parent_unlocked);

    printf("test_parent_unlock_gesture: PASS\n");
}

static void test_nap_time_lock(void) {
    yakka_machine_t m;
    uint32_t now = 1000;
    yakka_machine_init(&m, now);

    // 模拟连续完成 25 组词汇触发午睡
    m.completed_vocab_count = 24;
    m.state = YAKKA_STATE_SUCCESS;
    m.state_enter_time_ms = now;

    // 庆祝结束 -> 触发“Dee 要睡午觉啦”
    uint32_t act = yakka_machine_tick(&m, now + YAKKA_SUCCESS_DURATION_MS);
    assert(m.state == YAKKA_STATE_NAP_TIME);
    assert(act & (1u << YAKKA_ACTION_PLAY_AUDIO_NAP));
    assert(!m.nap_screen_dimmed);

    // 3 秒哈欠动画后 -> 进入微光安抚夜灯状态
    now += YAKKA_SUCCESS_DURATION_MS;
    act = yakka_machine_tick(&m, now + 3000);
    assert(m.nap_screen_dimmed);
    assert(act & (1u << YAKKA_ACTION_SET_BACKLIGHT));

    // 锁屏期间短按仅发出轻微提示
    act = yakka_machine_on_button(&m, 2, true, false, false, now + 5000);
    assert(m.state == YAKKA_STATE_NAP_TIME);

    // 家长长按 OK 键随时唤醒!
    act = yakka_machine_on_button(&m, 2, true, false, true /* is_long */, now + 6000);
    assert(m.state == YAKKA_STATE_BOOT_INTRO);
    assert(m.completed_vocab_count == 0);
    assert(!m.nap_screen_dimmed);

    printf("test_nap_time_lock: PASS\n");
}

int main(void) {
    test_zero_button_flow();
    test_silence_fallback();
    test_anti_touch_pop_sound();
    test_parent_unlock_gesture();
    test_nap_time_lock();
    printf("ALL YAKKA STATE TESTS PASSED!\n");
    return 0;
}
