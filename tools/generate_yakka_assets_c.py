#!/usr/bin/env python3
# tools/generate_yakka_assets_c.py
# 自动生成 main/yakka_assets.c 与 main/CMakeLists.txt 的 EMBED_FILES 资源清单

from pathlib import Path

CURRICULUM = [
    # id, intro_img, c1_img, c2_img, c3_img
    ("banana", "banana_intro", "banana_big", "banana_long", "banana_yellow"),
    ("dog",    "dog_intro",    "dog_big",    "dog_tiny",    "dog_silly"),
    ("book",   "book_intro",   "book_big",   "book_open",   "book_story"),
    ("boots",  "boots_intro",  "boots_red",  "boots_muddy", "boots_shiny"),
    ("bike",   "bike_intro",   "bike_fast",  "bike_little", "bike_ring"),
    ("duck",   "duck_intro",   "duck_swim",  "duck_quack",  "duck_fly"),
    ("cup",    "cup_intro",    "cup_blue",   "cup_big",     "cup_sippy"),
    ("bus",    "bus_intro",    "bus_big",    "bus_red",     "bus_beep"),
    ("peas",   "peas_intro",   "peas_green", "peas_tiny",   "peas_sweet"),
    ("hat",    "hat_intro",    "hat_sun",    "hat_silly",   "hat_warm"),
    ("worm",   "worm_intro",   "worm_wiggly","worm_long",   "worm_earth"),
    ("boat",   "boat_intro",   "boat_sailing","boat_speed", "boat_float"),
    ("house",  "house_intro",  "house_cozy", "house_tall",  "house_brick"),
    ("lion",   "lion_intro",   "lion_roar",  "lion_big",    "lion_golden"),
    ("apple",  "apple_intro",  "apple_big",  "apple_yummy", "apple_crunch"),
    ("ball",   "ball_intro",   "ball_bounce","ball_round",  "ball_roll"),
    ("mouse",  "mouse_intro",  "mouse_little","mouse_squeak","mouse_quiet"),
    ("beans",  "beans_intro",  "beans_magic","beans_green", "beans_jumping"),
    ("car",    "car_intro",    "car_fast",   "car_red",     "car_beep"),
    ("bed",    "bed_intro",    "bed_soft",   "bed_sleepy",  "bed_cozy"),
]

SYSTEM_AUDIO = [
    "audio_boot",
    "audio_cheer",
    "audio_skip",
    "audio_nap",
    "audio_pop",
    "audio_parent_on",
]

DEE_IMAGES = [
    "dee_welcome",
    "dee_listen",
    "dee_sleep",
]

def main():
    assets_dir = Path("main/assets")
    all_jpgs = set()
    all_adpcms = set()

    for item in CURRICULUM:
        w_id = item[0]
        for img in item[1:]:
            all_jpgs.add(img)
        all_adpcms.add(f"audio_{w_id}_intro")
        all_adpcms.add(f"audio_{w_id}_c1")
        all_adpcms.add(f"audio_{w_id}_c2")
        all_adpcms.add(f"audio_{w_id}_c3")
        all_adpcms.add(f"audio_{w_id}_q")
        all_adpcms.add(f"audio_{w_id}_retry")

    for img in DEE_IMAGES:
        all_jpgs.add(img)

    for aud in SYSTEM_AUDIO:
        all_adpcms.add(aud)

    # 检查所有文件是否存在
    missing = []
    for j in all_jpgs:
        p = assets_dir / f"{j}.jpg"
        if not p.exists():
            missing.append(str(p))
    for a in all_adpcms:
        p = assets_dir / f"{a}.adpcm"
        if not p.exists():
            missing.append(str(p))

    if missing:
        print("Missing files:", missing)
        return

    print(f"Verified {len(all_jpgs)} JPG files and {len(all_adpcms)} ADPCM files!")

    # 1. 生成 main/CMakeLists.txt
    cmake_path = Path("main/CMakeLists.txt")
    embed_lines = []
    for j in sorted(all_jpgs):
        embed_lines.append(f'        "assets/{j}.jpg"')
    for a in sorted(all_adpcms):
        embed_lines.append(f'        "assets/{a}.adpcm"')

    cmake_content = f"""idf_component_register(
    SRCS "main.c"
         "adpcm.c"
         "yakka_state.c"
         "audio_energy.c"
         "confetti.c"
         "audio_player.c"
         "ui_yakka.c"
         "yakka_assets.c"
         "lv_font_cn_16.c"
    INCLUDE_DIRS "."
    REQUIRES bsp esp_timer
    EMBED_FILES
{chr(10).join(embed_lines)}
)
"""
    cmake_path.write_text(cmake_content)
    print("Updated main/CMakeLists.txt")

    # 2. 生成 main/yakka_assets.c
    c_lines = []
    c_lines.append('// main/yakka_assets.c')
    c_lines.append('// Yakka Dee 口袋开口机 —— 第一季 20 词轻量压缩素材表')
    c_lines.append('#include "yakka_assets.h"')
    c_lines.append('')
    c_lines.append('// 宏: 声明内嵌二进制素材')
    c_lines.append('#define DECLARE_BIN(name) \\')
    c_lines.append('    extern const uint8_t _binary_##name##_start[]; \\')
    c_lines.append('    extern const uint8_t _binary_##name##_end[]')
    c_lines.append('')
    c_lines.append('typedef struct {')
    c_lines.append('    const uint8_t *start;')
    c_lines.append('    const uint8_t *end;')
    c_lines.append('} bin_ref_t;')
    c_lines.append('')
    c_lines.append('#define MAKE_BIN_REF(name) { \\')
    c_lines.append('    .start = _binary_##name##_start, \\')
    c_lines.append('    .end   = _binary_##name##_end, \\')
    c_lines.append('}')
    c_lines.append('')

    c_lines.append('// --- 声明图片资源 ---')
    for j in sorted(all_jpgs):
        c_lines.append(f'DECLARE_BIN({j}_jpg);')
    c_lines.append('')

    c_lines.append('// --- 声明系统音频 ---')
    for a in sorted(SYSTEM_AUDIO):
        c_lines.append(f'DECLARE_BIN({a}_adpcm);')
    c_lines.append('')

    c_lines.append('// --- 声明词汇音频 ---')
    vocab_audios = sorted([a for a in all_adpcms if a not in SYSTEM_AUDIO])
    for a in vocab_audios:
        c_lines.append(f'DECLARE_BIN({a}_adpcm);')
    c_lines.append('')

    # 图片映射索引
    sorted_jpgs = sorted(all_jpgs)
    jpg_to_idx = {j: i for i, j in enumerate(sorted_jpgs)}

    c_lines.append(f'#define TOTAL_JPG_COUNT {len(sorted_jpgs)}')
    c_lines.append('static const bin_ref_t s_jpg_refs[TOTAL_JPG_COUNT] = {')
    for j in sorted_jpgs:
        c_lines.append(f'    MAKE_BIN_REF({j}_jpg),')
    c_lines.append('};')
    c_lines.append('')
    c_lines.append('static lv_image_dsc_t s_jpg_dscs[TOTAL_JPG_COUNT];')
    c_lines.append('static bool s_dscs_inited = false;')
    c_lines.append('')
    c_lines.append('static void ensure_dscs_inited(void) {')
    c_lines.append('    if (s_dscs_inited) return;')
    c_lines.append('    for (int i = 0; i < TOTAL_JPG_COUNT; i++) {')
    c_lines.append('        s_jpg_dscs[i].header.magic = LV_IMAGE_HEADER_MAGIC;')
    c_lines.append('        s_jpg_dscs[i].header.cf = LV_COLOR_FORMAT_RAW;')
    c_lines.append('        s_jpg_dscs[i].header.flags = 0;')
    c_lines.append('        s_jpg_dscs[i].header.w = YAKKA_IMG_W;')
    c_lines.append('        s_jpg_dscs[i].header.h = YAKKA_IMG_H;')
    c_lines.append('        s_jpg_dscs[i].header.stride = 0;')
    c_lines.append('        s_jpg_dscs[i].data = s_jpg_refs[i].start;')
    c_lines.append('        s_jpg_dscs[i].data_size = (size_t)(s_jpg_refs[i].end - s_jpg_refs[i].start);')
    c_lines.append('    }')
    c_lines.append('    s_dscs_inited = true;')
    c_lines.append('}')
    c_lines.append('')

    c_lines.append('typedef struct {')
    c_lines.append('    int intro_idx;')
    c_lines.append('    int c1_idx;')
    c_lines.append('    int c2_idx;')
    c_lines.append('    int c3_idx;')
    c_lines.append('} vocab_img_map_t;')
    c_lines.append('')

    c_lines.append('static const vocab_img_map_t s_vocab_img_map[YAKKA_VOCAB_COUNT] = {')
    for item in CURRICULUM:
        w_id, intro, c1, c2, c3 = item
        c_lines.append(f'    [YAKKA_VOCAB_{w_id.upper()}] = {{ {jpg_to_idx[intro]}, {jpg_to_idx[c1]}, {jpg_to_idx[c2]}, {jpg_to_idx[c3]} }},')
    c_lines.append('};')
    c_lines.append('')

    c_lines.append('typedef struct {')
    c_lines.append('    bin_ref_t intro;')
    c_lines.append('    bin_ref_t c1;')
    c_lines.append('    bin_ref_t c2;')
    c_lines.append('    bin_ref_t c3;')
    c_lines.append('    bin_ref_t q;')
    c_lines.append('    bin_ref_t retry;')
    c_lines.append('} vocab_audio_bundle_t;')
    c_lines.append('')

    c_lines.append('static const vocab_audio_bundle_t s_vocab_audios[YAKKA_VOCAB_COUNT] = {')
    for item in CURRICULUM:
        w_id = item[0]
        c_lines.append(f'    [YAKKA_VOCAB_{w_id.upper()}] = {{')
        c_lines.append(f'        .intro = MAKE_BIN_REF(audio_{w_id}_intro_adpcm),')
        c_lines.append(f'        .c1    = MAKE_BIN_REF(audio_{w_id}_c1_adpcm),')
        c_lines.append(f'        .c2    = MAKE_BIN_REF(audio_{w_id}_c2_adpcm),')
        c_lines.append(f'        .c3    = MAKE_BIN_REF(audio_{w_id}_c3_adpcm),')
        c_lines.append(f'        .q     = MAKE_BIN_REF(audio_{w_id}_q_adpcm),')
        c_lines.append(f'        .retry = MAKE_BIN_REF(audio_{w_id}_retry_adpcm),')
        c_lines.append('    },')
    c_lines.append('};')
    c_lines.append('')

    c_lines.append('static inline yakka_pcm_asset_t to_pcm_asset(bin_ref_t ref) {')
    c_lines.append('    if (!ref.start || !ref.end || ref.end <= ref.start) {')
    c_lines.append('        return (yakka_pcm_asset_t){ .data = NULL, .size = 0, .is_adpcm = false };')
    c_lines.append('    }')
    c_lines.append('    return (yakka_pcm_asset_t){')
    c_lines.append('        .data = ref.start,')
    c_lines.append('        .size = (size_t)(ref.end - ref.start),')
    c_lines.append('        .is_adpcm = true,')
    c_lines.append('    };')
    c_lines.append('}')
    c_lines.append('')

    c_lines.append('const lv_image_dsc_t *yakka_asset_get_dee_image(yakka_state_t state) {')
    c_lines.append('    ensure_dscs_inited();')
    c_lines.append(f'    if (state == YAKKA_STATE_NAP_TIME) return &s_jpg_dscs[{jpg_to_idx["dee_sleep"]}];')
    c_lines.append(f'    if (state == YAKKA_STATE_QUESTION || state == YAKKA_STATE_ENCOURAGE_RETRY) return &s_jpg_dscs[{jpg_to_idx["dee_listen"]}];')
    c_lines.append(f'    return &s_jpg_dscs[{jpg_to_idx["dee_welcome"]}];')
    c_lines.append('}')
    c_lines.append('')

    c_lines.append('const lv_image_dsc_t *yakka_asset_get_image(yakka_vocab_t vocab, yakka_state_t state) {')
    c_lines.append('    ensure_dscs_inited();')
    c_lines.append(f'    if (vocab < 0 || vocab >= YAKKA_VOCAB_COUNT) return &s_jpg_dscs[{jpg_to_idx["dee_welcome"]}];')
    c_lines.append('    const vocab_img_map_t *m = &s_vocab_img_map[vocab];')
    c_lines.append('    if (state == YAKKA_STATE_CONTRAST_1) return &s_jpg_dscs[m->c1_idx];')
    c_lines.append('    if (state == YAKKA_STATE_CONTRAST_2) return &s_jpg_dscs[m->c2_idx];')
    c_lines.append('    if (state == YAKKA_STATE_CONTRAST_3) return &s_jpg_dscs[m->c3_idx];')
    c_lines.append('    return &s_jpg_dscs[m->intro_idx];')
    c_lines.append('}')
    c_lines.append('')

    c_lines.append('yakka_pcm_asset_t yakka_asset_audio_boot(void)      { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_boot_adpcm)); }')
    c_lines.append('yakka_pcm_asset_t yakka_asset_audio_cheer(void)     { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_cheer_adpcm)); }')
    c_lines.append('yakka_pcm_asset_t yakka_asset_audio_skip(void)      { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_skip_adpcm)); }')
    c_lines.append('yakka_pcm_asset_t yakka_asset_audio_nap(void)       { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_nap_adpcm)); }')
    c_lines.append('yakka_pcm_asset_t yakka_asset_audio_pop(void)       { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_pop_adpcm)); }')
    c_lines.append('yakka_pcm_asset_t yakka_asset_audio_parent_on(void) { return to_pcm_asset((bin_ref_t)MAKE_BIN_REF(audio_parent_on_adpcm)); }')
    c_lines.append('')

    c_lines.append('yakka_pcm_asset_t yakka_asset_get_audio(yakka_vocab_t vocab, yakka_action_t action) {')
    c_lines.append('    if (vocab < 0 || vocab >= YAKKA_VOCAB_COUNT) return (yakka_pcm_asset_t){ .data = NULL, .size = 0, .is_adpcm = false };')
    c_lines.append('    const vocab_audio_bundle_t *b = &s_vocab_audios[vocab];')
    c_lines.append('    if (action == YAKKA_ACTION_PLAY_AUDIO_VOCAB_INTRO) return to_pcm_asset(b->intro);')
    c_lines.append('    if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_1)  return to_pcm_asset(b->c1);')
    c_lines.append('    if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_2)  return to_pcm_asset(b->c2);')
    c_lines.append('    if (action == YAKKA_ACTION_PLAY_AUDIO_CONTRAST_3)  return to_pcm_asset(b->c3);')
    c_lines.append('    if (action == YAKKA_ACTION_PLAY_AUDIO_QUESTION)    return to_pcm_asset(b->q);')
    c_lines.append('    if (action == YAKKA_ACTION_PLAY_AUDIO_RETRY)       return to_pcm_asset(b->retry);')
    c_lines.append('    return (yakka_pcm_asset_t){ .data = NULL, .size = 0, .is_adpcm = false };')
    c_lines.append('}')
    c_lines.append('')

    Path("main/yakka_assets.c").write_text("\n".join(c_lines))
    print("Updated main/yakka_assets.c successfully!")

if __name__ == "__main__":
    main()
