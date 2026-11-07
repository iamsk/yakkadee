// main/ui_yakka.c
// Yakka Dee 口袋开口机 —— 护眼专用应用交互界面实现
#include "ui_yakka.h"
#include "bsp_display.h"
#include "confetti.h"
#include "lv_font_cn_16.h"
#include "yakka_assets.h"
#include <stdio.h>
#include <math.h>

#define COLOR_WARM_PAPER   0xF6F2E5  // 护眼暖纸米黄主底色
#define COLOR_INK          0x2C2C2C  // 粗轮廓黑墨
#define COLOR_SOFT_TEAL    0x56CCF2  // 呼吸光圈柔和青
#define COLOR_SOFT_GREEN   0x6FCF97  // 柔和果绿
#define COLOR_SOFT_YELLOW  0xF2C94C  // 柔和金黄
#define COLOR_CARD_BG      0xEFE8D3  // 词汇卡片浅暖底色
#define COLOR_PARENT_BG    0xFFFDF8  // 家长菜单底色
#define COLOR_SELECT_BAR   0xE0D6B8  // 菜单高亮条底色

static lv_obj_t *s_scr = NULL;
static lv_obj_t *s_header_panel = NULL;
static lv_obj_t *s_badge_label = NULL;
static lv_obj_t *s_battery_label = NULL;

static lv_obj_t *s_pulse_ring = NULL;
static lv_obj_t *s_main_img = NULL;

static lv_obj_t *s_card_panel = NULL;
static lv_obj_t *s_title_label = NULL;
static lv_obj_t *s_subtitle_label = NULL;

// 10 颗彩带粒子控件
static confetti_engine_t s_confetti_engine;
static lv_obj_t *s_confetti_objs[CONFETTI_PARTICLE_COUNT];

// 家长菜单控件
static lv_obj_t *s_parent_panel = NULL;
static lv_obj_t *s_parent_title = NULL;
static lv_obj_t *s_parent_items[5];
static lv_obj_t *s_parent_hint = NULL;

static void create_confetti_layer(lv_obj_t *parent) {
    confetti_init(&s_confetti_engine, 240, 320);

    for (int i = 0; i < CONFETTI_PARTICLE_COUNT; i++) {
        const confetti_particle_t *p = &s_confetti_engine.particles[i];
        lv_obj_t *obj = lv_obj_create(parent);
        lv_obj_set_size(obj, p->size_w, p->size_h);
        lv_obj_set_style_bg_color(obj, lv_color_hex(p->color_hex), 0);
        lv_obj_set_style_border_width(obj, 2, 0);
        lv_obj_set_style_border_color(obj, lv_color_hex(COLOR_INK), 0);

        if (p->shape == CONFETTI_SHAPE_CIRCLE) {
            lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
        } else {
            lv_obj_set_style_radius(obj, 4, 0);
        }

        lv_obj_set_pos(obj, (int32_t)p->x, (int32_t)p->y);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        s_confetti_objs[i] = obj;
    }
}

static void create_parent_menu(lv_obj_t *parent) {
    s_parent_panel = lv_obj_create(parent);
    lv_obj_set_size(s_parent_panel, 216, 276);
    lv_obj_center(s_parent_panel);
    lv_obj_set_style_bg_color(s_parent_panel, lv_color_hex(COLOR_PARENT_BG), 0);
    lv_obj_set_style_border_color(s_parent_panel, lv_color_hex(COLOR_INK), 0);
    lv_obj_set_style_border_width(s_parent_panel, 3, 0);
    lv_obj_set_style_radius(s_parent_panel, 14, 0);
    lv_obj_set_style_pad_all(s_parent_panel, 8, 0);

    // 标题
    s_parent_title = lv_label_create(s_parent_panel);
    lv_obj_set_style_text_font(s_parent_title, &lv_font_cn_16, 0);
    lv_obj_set_style_text_color(s_parent_title, lv_color_hex(COLOR_INK), 0);
    lv_label_set_text(s_parent_title, "家长模式设置");
    lv_obj_align(s_parent_title, LV_ALIGN_TOP_MID, 0, 4);

    // 5 项菜单
    for (int i = 0; i < 5; i++) {
        lv_obj_t *row = lv_obj_create(s_parent_panel);
        lv_obj_set_size(row, 196, 36);
        lv_obj_set_pos(row, 2, 34 + i * 40);
        lv_obj_set_style_border_width(row, 2, 0);
        lv_obj_set_style_border_color(row, lv_color_hex(COLOR_INK), 0);
        lv_obj_set_style_radius(row, 8, 0);
        lv_obj_set_style_pad_all(row, 4, 0);

        lv_obj_t *lbl = lv_label_create(row);
        lv_obj_set_style_text_font(lbl, &lv_font_cn_16, 0);
        lv_obj_set_style_text_color(lbl, lv_color_hex(COLOR_INK), 0);
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 6, 0);

        s_parent_items[i] = row;
    }

    // 底部提示
    s_parent_hint = lv_label_create(s_parent_panel);
    lv_obj_set_style_text_font(s_parent_hint, &lv_font_cn_16, 0);
    lv_obj_set_style_text_color(s_parent_hint, lv_color_hex(0x7A7A7A), 0);
    lv_label_set_text(s_parent_hint, "按确定切换 / 8秒自动锁定");
    lv_obj_align(s_parent_hint, LV_ALIGN_BOTTOM_MID, 0, -2);

    lv_obj_add_flag(s_parent_panel, LV_OBJ_FLAG_HIDDEN);
}

void ui_yakka_init(void) {
    if (s_scr) return;

    s_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(s_scr, lv_color_hex(COLOR_WARM_PAPER), 0);

    // 1. 顶部栏 (高度 32)
    s_header_panel = lv_obj_create(s_scr);
    lv_obj_set_size(s_header_panel, 216, 32);
    lv_obj_align(s_header_panel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(s_header_panel, lv_color_hex(COLOR_CARD_BG), 0);
    lv_obj_set_style_border_color(s_header_panel, lv_color_hex(COLOR_INK), 0);
    lv_obj_set_style_border_width(s_header_panel, 2, 0);
    lv_obj_set_style_radius(s_header_panel, 10, 0);
    lv_obj_set_style_pad_hor(s_header_panel, 10, 0);

    s_badge_label = lv_label_create(s_header_panel);
    lv_obj_set_style_text_font(s_badge_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_badge_label, lv_color_hex(COLOR_INK), 0);
    lv_label_set_text(s_badge_label, "YAKKA DEE");
    lv_obj_align(s_badge_label, LV_ALIGN_LEFT_MID, 0, 0);

    s_battery_label = lv_label_create(s_header_panel);
    lv_obj_set_style_text_font(s_battery_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_battery_label, lv_color_hex(COLOR_INK), 0);
    lv_label_set_text(s_battery_label, "100%");
    lv_obj_align(s_battery_label, LV_ALIGN_RIGHT_MID, 0, 0);

    // 2. 提问阶段呼吸光圈 (212x212)
    s_pulse_ring = lv_obj_create(s_scr);
    lv_obj_set_size(s_pulse_ring, 214, 214);
    lv_obj_align(s_pulse_ring, LV_ALIGN_TOP_MID, 0, 41);
    lv_obj_set_style_bg_opa(s_pulse_ring, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(s_pulse_ring, lv_color_hex(COLOR_SOFT_TEAL), 0);
    lv_obj_set_style_border_width(s_pulse_ring, 4, 0);
    lv_obj_set_style_radius(s_pulse_ring, 18, 0);
    lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);

    // 3. 中央插画 (200x200)
    s_main_img = lv_image_create(s_scr);
    lv_obj_set_size(s_main_img, YAKKA_IMG_W, YAKKA_IMG_H);
    lv_obj_align(s_main_img, LV_ALIGN_TOP_MID, 0, 48);
    lv_image_set_src(s_main_img, yakka_asset_get_dee_image(YAKKA_STATE_BOOT_INTRO));

    // 4. 底部词汇与提示卡片
    s_card_panel = lv_obj_create(s_scr);
    lv_obj_set_size(s_card_panel, 216, 56);
    lv_obj_align(s_card_panel, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(s_card_panel, lv_color_hex(COLOR_CARD_BG), 0);
    lv_obj_set_style_border_color(s_card_panel, lv_color_hex(COLOR_INK), 0);
    lv_obj_set_style_border_width(s_card_panel, 3, 0);
    lv_obj_set_style_radius(s_card_panel, 12, 0);
    lv_obj_set_style_pad_all(s_card_panel, 4, 0);

    s_title_label = lv_label_create(s_card_panel);
    lv_obj_set_style_text_font(s_title_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(s_title_label, lv_color_hex(COLOR_INK), 0);
    lv_label_set_text(s_title_label, "Yakka Dee!");
    lv_obj_align(s_title_label, LV_ALIGN_TOP_MID, 0, 2);

    s_subtitle_label = lv_label_create(s_card_panel);
    lv_obj_set_style_text_font(s_subtitle_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_subtitle_label, lv_color_hex(COLOR_INK), 0);
    lv_label_set_text(s_subtitle_label, "Talk! Talk! Talk!");
    lv_obj_align(s_subtitle_label, LV_ALIGN_BOTTOM_MID, 0, -2);

    // 5. 彩带粒子层
    create_confetti_layer(s_scr);

    // 6. 家长菜单层
    create_parent_menu(s_scr);

    lv_screen_load(s_scr);
}

void ui_yakka_show_confetti(bool show) {
    s_confetti_engine.active = show;
    if (show) confetti_reset(&s_confetti_engine);
    for (int i = 0; i < CONFETTI_PARTICLE_COUNT; i++) {
        if (s_confetti_objs[i]) {
            if (show) lv_obj_remove_flag(s_confetti_objs[i], LV_OBJ_FLAG_HIDDEN);
            else lv_obj_add_flag(s_confetti_objs[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void ui_yakka_confetti_step(void) {
    if (!s_confetti_engine.active) return;
    confetti_step(&s_confetti_engine);
    for (int i = 0; i < CONFETTI_PARTICLE_COUNT; i++) {
        if (s_confetti_objs[i]) {
            const confetti_particle_t *p = &s_confetti_engine.particles[i];
            lv_obj_set_pos(s_confetti_objs[i], (int32_t)p->x, (int32_t)p->y);
        }
    }
}

void ui_yakka_pulse_step(uint32_t now_ms) {
    if (!s_pulse_ring || lv_obj_has_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN)) return;
    // 呼吸灯光圈粗细变化 (3px ~ 7px)
    float phase = (float)(now_ms % 1000) / 1000.0f * 2.0f * (float)M_PI;
    int width = 3 + (int)(2.0f * (sinf(phase) + 1.0f));
    lv_obj_set_style_border_width(s_pulse_ring, width, 0);
}

void ui_yakka_update(const yakka_machine_t *m, int battery_soc, int battery_mv) {
    if (!m || !s_scr) return;

    // 1. 刷新电池状态
    if (s_battery_label) {
        if (battery_soc >= 0) {
            lv_label_set_text_fmt(s_battery_label, "%d%%", battery_soc);
        } else {
            lv_label_set_text(s_battery_label, "--%");
        }
    }

    // 2. 家长菜单展示与更新
    if (m->state == YAKKA_STATE_PARENT_MENU) {
        lv_obj_remove_flag(s_parent_panel, LV_OBJ_FLAG_HIDDEN);
        ui_yakka_show_confetti(false);
        lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);

        // 更新 5 行内容与光标
        const char *vol_str = m->volume == YAKKA_VOL_LOW ? "低 60%" : (m->volume == YAKKA_VOL_MED ? "中 80%" : "高 100%");
        const char *bl_str = m->backlight_percent == 25 ? "护眼 25%" : (m->backlight_percent == 50 ? "标准 50%" : "明亮 75%");
        const char *sens_str = m->mic_sensitivity == YAKKA_MIC_SENS_LOW ? "低" : (m->mic_sensitivity == YAKKA_MIC_SENS_MED ? "中" : "高");
        const char *loop_str = m->selected_vocab_loop == YAKKA_LOOP_ALL ? "全部 20 词" : yakka_vocab_name((yakka_vocab_t)m->selected_vocab_loop);

        char item_buf[64];
        for (int i = 0; i < 5; i++) {
            lv_obj_t *row = s_parent_items[i];
            lv_obj_t *lbl = lv_obj_get_child(row, 0);
            bool is_selected = (i == m->parent_menu_cursor);

            lv_obj_set_style_bg_color(row, lv_color_hex(is_selected ? COLOR_SELECT_BAR : COLOR_PARENT_BG), 0);

            if (i == 0) snprintf(item_buf, sizeof(item_buf), "主音量: %s", vol_str);
            else if (i == 1) snprintf(item_buf, sizeof(item_buf), "背光亮度: %s", bl_str);
            else if (i == 2) snprintf(item_buf, sizeof(item_buf), "麦克风灵敏度: %s", sens_str);
            else if (i == 3) snprintf(item_buf, sizeof(item_buf), "词汇循环: %s", loop_str);
            else if (i == 4) snprintf(item_buf, sizeof(item_buf), "保存并退出");

            if (lbl) lv_label_set_text(lbl, item_buf);
        }
        return;
    } else {
        lv_obj_add_flag(s_parent_panel, LV_OBJ_FLAG_HIDDEN);
    }

    // 3. 正常主界面更新
    const char *vname = yakka_vocab_name(m->current_vocab);

    switch (m->state) {
        case YAKKA_STATE_BOOT_INTRO:
            lv_image_set_src(s_main_img, yakka_asset_get_dee_image(YAKKA_STATE_BOOT_INTRO));
            lv_label_set_text(s_title_label, "Yakka Dee!");
            lv_label_set_text(s_subtitle_label, "Talk, talk, talk!");
            lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        case YAKKA_STATE_VOCAB_INTRO:
            lv_image_set_src(s_main_img, yakka_asset_get_image(m->current_vocab, YAKKA_STATE_VOCAB_INTRO));
            lv_label_set_text(s_title_label, vname);
            lv_label_set_text(s_subtitle_label, yakka_vocab_phrase(m->current_vocab));
            lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        case YAKKA_STATE_CONTRAST_1:
            lv_image_set_src(s_main_img, yakka_asset_get_image(m->current_vocab, YAKKA_STATE_CONTRAST_1));
            lv_label_set_text(s_title_label, vname);
            lv_label_set_text(s_subtitle_label, yakka_contrast_label(m->current_vocab, 0));
            lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        case YAKKA_STATE_CONTRAST_2:
            lv_image_set_src(s_main_img, yakka_asset_get_image(m->current_vocab, YAKKA_STATE_CONTRAST_2));
            lv_label_set_text(s_title_label, vname);
            lv_label_set_text(s_subtitle_label, yakka_contrast_label(m->current_vocab, 1));
            lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        case YAKKA_STATE_CONTRAST_3:
            lv_image_set_src(s_main_img, yakka_asset_get_image(m->current_vocab, YAKKA_STATE_CONTRAST_3));
            lv_label_set_text(s_title_label, vname);
            lv_label_set_text(s_subtitle_label, yakka_contrast_label(m->current_vocab, 2));
            lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        case YAKKA_STATE_QUESTION:
            lv_image_set_src(s_main_img, yakka_asset_get_dee_image(YAKKA_STATE_QUESTION));
            lv_label_set_text(s_title_label, vname);
            lv_label_set_text(s_subtitle_label, "Your turn! You say it!");
            lv_obj_remove_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        case YAKKA_STATE_ENCOURAGE_RETRY:
            lv_image_set_src(s_main_img, yakka_asset_get_image(m->current_vocab, YAKKA_STATE_VOCAB_INTRO));
            lv_label_set_text(s_title_label, vname);
            lv_label_set_text(s_subtitle_label, "Say it together!");
            lv_obj_remove_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        case YAKKA_STATE_SUCCESS:
            lv_image_set_src(s_main_img, yakka_asset_get_image(m->current_vocab, YAKKA_STATE_CONTRAST_1));
            lv_label_set_text(s_title_label, "You Did It!");
            lv_label_set_text(s_subtitle_label, "Hooray! Great job!");
            lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(true);
            break;

        case YAKKA_STATE_NAP_TIME:
            lv_image_set_src(s_main_img, yakka_asset_get_dee_image(YAKKA_STATE_NAP_TIME));
            lv_label_set_text(s_title_label, "Nap Time...");
            lv_label_set_text(s_subtitle_label, "Dee is sleeping! Zzz...");
            lv_obj_add_flag(s_pulse_ring, LV_OBJ_FLAG_HIDDEN);
            ui_yakka_show_confetti(false);
            break;

        default:
            break;
    }
}
