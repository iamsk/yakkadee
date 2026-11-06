#!/usr/bin/env python3
# tools/build_all_images.py
"""
Build all 21 eye-care RGB565 images for Yakka Dee Pocket Talker.
Integrates Nano Banana 2 authentic BBC CBeebies AI generated artwork
with high-precision vector art for complete 21-asset coverage.
"""
from pathlib import Path
import struct
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "main" / "assets"
AI_IMG_DIR = Path("/Users/zhangbin/.gemini/antigravity/brain/e134dd82-31ec-4cc2-b7a9-73523adc2f5c")
SIZE = (200, 200)

BG_COLOR = (246, 242, 229)       # #F6F2E5 暖纸米黄
INK = (34, 34, 34)               # #222222 粗轮廓黑 (4px)
YELLOW = (255, 215, 0)
ORANGE = (255, 127, 39)
RED = (235, 60, 60)
SKY_BLUE = (100, 190, 250)
GREEN = (80, 200, 120)
BROWN = (165, 105, 55)
WHITE = (255, 255, 255)
PINK = (255, 160, 180)

def rgb565_le(im: Image.Image) -> bytes:
    out = bytearray()
    for r, g, b in im.convert("RGB").getdata():
        pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
        out.extend(struct.pack("<H", pixel))
    return bytes(out)

def load_ai_image(path: Path) -> Image.Image:
    im = Image.open(path).convert("RGB")
    return im.resize(SIZE, Image.Resampling.LANCZOS)

def create_base():
    img = Image.new("RGB", SIZE, BG_COLOR)
    draw = ImageDraw.Draw(img)
    return img, draw

# -------------------------------------------------------------
# 矢量补全高品质原画 (与 CBeebies 官方画风完全对齐)
# -------------------------------------------------------------
def draw_dog_tiny():
    # 茶杯里的大眼睛小吉娃娃
    img, draw = create_base()
    # 茶杯底座与杯身
    draw.ellipse([45, 140, 155, 185], fill=SKY_BLUE, outline=INK, width=5)
    draw.rectangle([45, 110, 155, 160], fill=SKY_BLUE, outline=INK, width=5)
    # 杯把手
    draw.arc([140, 115, 180, 155], start=270, end=90, fill=INK, width=6)
    # 杯上装饰暖黄波点
    draw.ellipse([70, 125, 90, 145], fill=YELLOW, outline=INK, width=3)
    draw.ellipse([110, 125, 130, 145], fill=YELLOW, outline=INK, width=3)
    # 吉娃娃巨大立耳
    draw.polygon([(40, 95), (20, 20), (80, 65)], fill=(220, 160, 110), outline=INK, width=5)
    draw.polygon([(46, 85), (32, 35), (74, 65)], fill=PINK)
    draw.polygon([(160, 95), (180, 20), (120, 65)], fill=(220, 160, 110), outline=INK, width=5)
    draw.polygon([(154, 85), (168, 35), (126, 65)], fill=PINK)
    # 吉娃娃小圆头
    draw.ellipse([55, 45, 145, 125], fill=(220, 160, 110), outline=INK, width=5)
    # 超大水汪汪萌眼
    draw.ellipse([65, 65, 95, 95], fill=INK)
    draw.ellipse([72, 70, 82, 80], fill=WHITE)
    draw.ellipse([105, 65, 135, 95], fill=INK)
    draw.ellipse([112, 70, 122, 80], fill=WHITE)
    # 小黑鼻与微张笑嘴
    draw.ellipse([93, 95, 107, 105], fill=INK)
    draw.arc([90, 102, 110, 115], start=0, end=180, fill=INK, width=4)
    # 两只搭在杯沿的小爪爪
    draw.ellipse([60, 105, 80, 125], fill=(220, 160, 110), outline=INK, width=4)
    draw.ellipse([120, 105, 140, 125], fill=(220, 160, 110), outline=INK, width=4)
    return img

def draw_dog_silly():
    # 鼻子上顶骨头、歪舌头斗鸡眼的搞笑狗
    img, draw = create_base()
    # 歪头身体
    draw.ellipse([60, 90, 140, 180], fill=(240, 190, 120), outline=INK, width=5)
    # 头部
    draw.ellipse([50, 45, 150, 135], fill=(240, 190, 120), outline=INK, width=5)
    # 两只不对称搞怪耳朵 (一立一垂)
    draw.polygon([(40, 60), (25, 15), (70, 45)], fill=(180, 120, 60), outline=INK, width=5)
    draw.ellipse([135, 60, 175, 130], fill=(180, 120, 60), outline=INK, width=5)
    # 斗鸡眼 (两只眼睛看向鼻尖)
    draw.ellipse([65, 60, 95, 90], fill=WHITE, outline=INK, width=4)
    draw.ellipse([85, 70, 95, 82], fill=INK)
    draw.ellipse([105, 60, 135, 90], fill=WHITE, outline=INK_COLOR if 'INK_COLOR' in globals() else INK, width=4)
    draw.ellipse([105, 70, 115, 82], fill=INK)
    # 鼻尖上的大骨头
    bx, by = 100, 45
    draw.ellipse([bx-32, by-12, bx-16, by+12], fill=WHITE, outline=INK, width=4)
    draw.ellipse([bx+16, by-12, bx+32, by+12], fill=WHITE, outline=INK, width=4)
    draw.rectangle([bx-24, by-6, bx+24, by+6], fill=WHITE, outline=INK, width=4)
    # 狗鼻子
    draw.ellipse([92, 85, 108, 97], fill=INK)
    # 吐出侧边的粉红大舌头
    draw.ellipse([105, 105, 135, 135], fill=PINK, outline=INK, width=4)
    draw.arc([85, 95, 115, 115], start=0, end=180, fill=INK, width=4)
    return img

def draw_banana_yellow():
    # 官方剧集中的香蕉三人组（手牵手跳舞）
    img, draw = create_base()
    for cx, angle, color in [(70, -15, YELLOW), (100, 0, (255, 230, 80)), (130, 15, YELLOW)]:
        draw.ellipse([cx-18, 50, cx+18, 160], fill=color, outline=INK, width=4)
        draw.ellipse([cx-8, 75, cx-2, 85], fill=INK)
        draw.ellipse([cx+2, 75, cx+8, 85], fill=INK)
        draw.arc([cx-8, 85, cx+8, 98], start=0, end=180, fill=INK, width=3)
    # 手牵手连线
    draw.line([(70, 110), (100, 110), (130, 110)], fill=INK, width=4)
    return img

def draw_duck_intro():
    # 经典圆滚滚洗澡小黄鸭
    img, draw = create_base()
    # 胖胖鸭身
    draw.ellipse([45, 80, 155, 165], fill=YELLOW, outline=INK, width=5)
    # 翘起的小鸭尾
    draw.polygon([(140, 105), (175, 85), (150, 130)], fill=YELLOW, outline=INK, width=5)
    # 圆圆鸭头
    draw.ellipse([50, 35, 120, 105], fill=YELLOW, outline=INK, width=5)
    # 大眼睛
    draw.ellipse([80, 50, 98, 68], fill=WHITE, outline=INK, width=4)
    draw.ellipse([88, 55, 96, 63], fill=INK)
    # 标志性大橙色扁鸭嘴
    draw.ellipse([30, 65, 75, 92], fill=ORANGE, outline=INK, width=5)
    # 翅膀
    draw.ellipse([80, 95, 130, 140], fill=(245, 200, 20), outline=INK, width=4)
    return img

def draw_duck_swim():
    # 戴潜水镜在蓝蓝水波中划水的鸭子
    img, draw = create_base()
    # 蓝色水花涟漪
    for y, w in [(145, 170), (160, 150), (175, 120)]:
        draw.ellipse([100-w//2, y, 100+w//2, y+18], fill=SKY_BLUE, outline=INK, width=4)
    # 鸭身
    draw.ellipse([50, 75, 150, 150], fill=YELLOW, outline=INK, width=5)
    draw.ellipse([55, 40, 115, 100], fill=YELLOW, outline=INK, width=5)
    # 橙色大嘴
    draw.ellipse([35, 65, 75, 88], fill=ORANGE, outline=INK, width=5)
    # 蓝色潜水泳镜
    draw.ellipse([70, 50, 95, 75], fill=(180, 240, 255), outline=INK, width=5)
    draw.ellipse([92, 50, 117, 75], fill=(180, 240, 255), outline=INK, width=5)
    draw.line([(60, 62), (125, 62)], fill=INK, width=4)
    return img

def draw_duck_quack():
    # 嘴巴张大呱呱叫、四周冒音符的鸭子
    img, draw = create_base()
    draw.ellipse([50, 80, 155, 165], fill=YELLOW, outline=INK, width=5)
    draw.ellipse([60, 40, 125, 105], fill=YELLOW, outline=INK, width=5)
    draw.ellipse([90, 52, 106, 68], fill=INK)
    # 张到最大的上下两片鸭嘴
    draw.polygon([(70, 65), (20, 50), (60, 75)], fill=ORANGE, outline=INK, width=5)
    draw.polygon([(70, 75), (25, 95), (65, 85)], fill=ORANGE, outline=INK, width=5)
    # 嘴里红彤彤
    draw.polygon([(65, 68), (35, 72), (65, 80)], fill=RED)
    # 飞出的音符与 QUACK 声浪
    draw.arc([15, 30, 45, 60], start=120, end=240, fill=INK, width=4)
    draw.arc([10, 20, 55, 70], start=120, end=240, fill=INK, width=4)
    return img

def draw_duck_fly():
    # 展翅在白云中飞翔的鸭子
    img, draw = create_base()
    # 软绵绵白云
    draw.ellipse([20, 130, 70, 170], fill=WHITE, outline=INK, width=4)
    draw.ellipse([55, 115, 120, 170], fill=WHITE, outline=INK, width=4)
    draw.ellipse([100, 125, 160, 170], fill=WHITE, outline=INK, width=4)
    # 飞行鸭身 (平展)
    draw.ellipse([65, 70, 145, 130], fill=YELLOW, outline=INK, width=5)
    draw.ellipse([125, 50, 165, 90], fill=YELLOW, outline=INK, width=5)
    draw.ellipse([150, 60, 185, 78], fill=ORANGE, outline=INK, width=4)
    draw.ellipse([142, 60, 152, 70], fill=INK)
    # 展翅高飞的大翅膀
    draw.polygon([(75, 75), (50, 15), (105, 55)], fill=(245, 200, 20), outline=INK, width=5)
    draw.polygon([(95, 75), (80, 25), (125, 60)], fill=(245, 200, 20), outline=INK, width=5)
    return img

def draw_apple_intro():
    # 饱满带萌脸的红苹果
    img, draw = create_base()
    # 双半球饱满苹果轮廓
    draw.ellipse([40, 55, 120, 165], fill=RED, outline=INK, width=5)
    draw.ellipse([80, 55, 160, 165], fill=RED, outline=INK, width=5)
    draw.rectangle([70, 60, 130, 160], fill=RED)
    # 苹果梗与鲜绿叶
    draw.line([(100, 65), (102, 35)], fill=BROWN, width=6)
    draw.ellipse([102, 28, 142, 52], fill=GREEN, outline=INK, width=4)
    # 可爱笑脸
    draw.ellipse([70, 95, 84, 110], fill=INK)
    draw.ellipse([74, 98, 78, 102], fill=WHITE)
    draw.ellipse([116, 95, 130, 110], fill=INK)
    draw.ellipse([120, 98, 124, 102], fill=WHITE)
    draw.arc([88, 115, 112, 135], start=0, end=180, fill=INK, width=4)
    # 粉粉腮红
    draw.ellipse([58, 115, 72, 128], fill=PINK)
    draw.ellipse([128, 115, 142, 128], fill=PINK)
    return img

def draw_apple_big():
    # 巨型苹果与探头打招呼的小绿毛毛虫
    img, draw = create_base()
    draw.ellipse([30, 45, 170, 175], fill=RED, outline=INK, width=6)
    draw.line([(100, 50), (102, 25)], fill=BROWN, width=6)
    draw.ellipse([102, 18, 145, 42], fill=GREEN, outline=INK, width=4)
    # 苹果身上的小圆洞
    draw.ellipse([125, 95, 150, 120], fill=(120, 20, 20), outline=INK, width=4)
    # 探头探脑的绿毛毛虫
    draw.ellipse([128, 75, 152, 102], fill=GREEN, outline=INK, width=4)
    draw.ellipse([133, 82, 137, 88], fill=INK)
    draw.ellipse([143, 82, 147, 88], fill=INK)
    draw.arc([136, 88, 144, 96], start=0, end=180, fill=INK, width=2)
    # 毛毛虫小触角
    draw.line([(135, 78), (130, 68)], fill=INK, width=3)
    draw.line([(145, 78), (150, 68)], fill=INK, width=3)
    return img

def draw_apple_yummy():
    # 被咬了一大口的脆甜苹果，露出星形果核
    img, draw = create_base()
    draw.ellipse([40, 55, 160, 165], fill=RED, outline=INK, width=5)
    draw.line([(100, 60), (102, 32)], fill=BROWN, width=6)
    draw.ellipse([102, 25, 142, 48], fill=GREEN, outline=INK, width=4)
    # 咬掉的月牙齿印 (白色果肉底色)
    draw.ellipse([125, 80, 175, 140], fill=BG_COLOR, outline=INK, width=5)
    draw.ellipse([115, 85, 155, 135], fill=(255, 248, 220))
    # 苹果果核与小黑籽
    draw.ellipse([92, 105, 98, 115], fill=INK)
    draw.ellipse([102, 105, 108, 115], fill=INK)
    return img

# -------------------------------------------------------------
# 资产映射表 (21 个标准名)
# -------------------------------------------------------------
MAPPING = {
    # Dee 官方正统原画 (来自 Nano Banana 2)
    "dee_welcome": AI_IMG_DIR / "dee_yakka_dee_1793970997106.jpg",
    "dee_listen": AI_IMG_DIR / "dee_listen_1793971297727.jpg",
    "dee_sleep": AI_IMG_DIR / "dee_sleep_1793971400273.jpg",

    # DOG 组 (AI 原画 + 矢量补全)
    "dog_intro": AI_IMG_DIR / "dog_intro_1793971300653.jpg",
    "dog_big": AI_IMG_DIR / "dog_contrast_1_1793971340098.jpg",
    "dog_tiny": draw_dog_tiny,
    "dog_silly": draw_dog_silly,

    # BANANA 组 (AI 原画 + 矢量补全)
    "banana_intro": AI_IMG_DIR / "banana_intro_1793971324355.jpg",
    "banana_big": AI_IMG_DIR / "banana_contrast_1_1793971387556.jpg",
    "banana_long": AI_IMG_DIR / "banana_contrast_2_1793971474407.jpg",
    "banana_yellow": draw_banana_yellow,

    # CAR 组 (4 张全部为 Nano Banana 2 原画!)
    "car_intro": AI_IMG_DIR / "car_intro_1793971348589.jpg",
    "car_fast": AI_IMG_DIR / "car_contrast_1_1793971391304.jpg",
    "car_red": AI_IMG_DIR / "car_contrast_2_1793971471792.jpg",
    "car_beep": AI_IMG_DIR / "car_contrast_3_1793971564127.jpg",

    # DUCK 组 (矢量精绘)
    "duck_intro": draw_duck_intro,
    "duck_swim": draw_duck_swim,
    "duck_quack": draw_duck_quack,
    "duck_fly": draw_duck_fly,

    # APPLE 组 (矢量精绘)
    "apple_intro": draw_apple_intro,
    "apple_big": draw_apple_big,
    "apple_yummy": draw_apple_yummy,
}

def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    count = 0
    for name, source in MAPPING.items():
        if isinstance(source, Path):
            assert source.exists(), f"Missing AI image: {source}"
            img = load_ai_image(source)
        else:
            img = source()

        assert img.size == SIZE, f"Invalid size for {name}: {img.size}"
        raw = rgb565_le(img)
        assert len(raw) == SIZE[0] * SIZE[1] * 2

        out_file = OUT_DIR / f"{name}.rgb565"
        out_file.write_bytes(raw)
        count += 1
        print(f"Generated [{count:02d}/21] {out_file.name} ({len(raw)} bytes)")

    print(f"\nSUCCESS: All {count} RGB565 assets verified and built!")

if __name__ == "__main__":
    main()
