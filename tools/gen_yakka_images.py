#!/usr/bin/env python3
# tools/gen_yakka_images.py
"""Generate eye-care RGB565 images for Yakka Dee Pocket Talker."""

import math
from pathlib import Path
import struct
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "main" / "assets"
SIZE = (200, 200)

BG_COLOR = (246, 242, 229)       # #F6F2E5 暖纸米黄
INK_COLOR = (44, 44, 44)         # #2C2C2C 粗轮廓墨黑 (>= 4px)
SOFT_RED = (229, 91, 91)         # 柔和红
SOFT_YELLOW = (242, 201, 76)     # 柔和暖金黄
SOFT_ORANGE = (242, 153, 74)     # 柔和橙
SOFT_GREEN = (111, 207, 151)     # 柔和绿
SOFT_BLUE = (86, 204, 242)       # 柔和天蓝
SOFT_BROWN = (160, 106, 59)      # 柔和棕
WHITE = (250, 248, 245)
PINK = (255, 182, 193)

def create_base():
    img = Image.new("RGB", SIZE, BG_COLOR)
    draw = ImageDraw.Draw(img)
    return img, draw

def rgb565_le(im: Image.Image) -> bytes:
    out = bytearray()
    for r, g, b in im.convert("RGB").getdata():
        pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
        out.extend(struct.pack("<H", pixel))
    return bytes(out)

# -------------------------------------------------------------
# DOG 组
# -------------------------------------------------------------
def draw_dog_intro():
    img, draw = create_base()
    # 头部底色
    draw.ellipse([40, 45, 160, 165], fill=SOFT_BROWN, outline=INK_COLOR, width=5)
    # 两只垂耳
    draw.ellipse([20, 40, 60, 130], fill=(130, 80, 40), outline=INK_COLOR, width=5)
    draw.ellipse([140, 40, 180, 130], fill=(130, 80, 40), outline=INK_COLOR, width=5)
    # 鼻子区域 (白色嘴套)
    draw.ellipse([70, 100, 130, 150], fill=WHITE, outline=INK_COLOR, width=4)
    # 黑鼻子
    draw.ellipse([88, 105, 112, 125], fill=INK_COLOR)
    # 眼睛
    draw.ellipse([65, 75, 85, 95], fill=INK_COLOR)
    draw.ellipse([72, 78, 78, 84], fill=WHITE)
    draw.ellipse([115, 75, 135, 95], fill=INK_COLOR)
    draw.ellipse([122, 78, 128, 84], fill=WHITE)
    # 嘴巴
    draw.arc([80, 120, 100, 140], start=0, end=180, fill=INK_COLOR, width=4)
    draw.arc([100, 120, 120, 140], start=0, end=180, fill=INK_COLOR, width=4)
    return img

def draw_dog_big():
    img, draw = create_base()
    # 放大占满屏幕
    draw.ellipse([15, 15, 185, 185], fill=SOFT_BROWN, outline=INK_COLOR, width=6)
    draw.ellipse([0, 15, 45, 140], fill=(130, 80, 40), outline=INK_COLOR, width=6)
    draw.ellipse([155, 15, 200, 140], fill=(130, 80, 40), outline=INK_COLOR, width=6)
    # 巨大开怀大笑的嘴巴
    draw.ellipse([50, 85, 150, 175], fill=WHITE, outline=INK_COLOR, width=5)
    draw.chord([65, 115, 135, 165], start=0, end=180, fill=SOFT_RED, outline=INK_COLOR, width=5)
    draw.ellipse([75, 130, 125, 165], fill=PINK)
    draw.ellipse([82, 92, 118, 118], fill=INK_COLOR)
    # 眯眯开怀笑眼
    draw.arc([45, 55, 85, 85], start=180, end=360, fill=INK_COLOR, width=6)
    draw.arc([115, 55, 155, 85], start=180, end=360, fill=INK_COLOR, width=6)
    return img

def draw_dog_tiny():
    img, draw = create_base()
    # 小巧玲珑居中
    cx, cy = 100, 100
    draw.ellipse([cx-40, cy-35, cx+40, cy+45], fill=SOFT_BROWN, outline=INK_COLOR, width=4)
    draw.ellipse([cx-55, cy-35, cx-30, cy+25], fill=(130, 80, 40), outline=INK_COLOR, width=4)
    draw.ellipse([cx+30, cy-35, cx+55, cy+25], fill=(130, 80, 40), outline=INK_COLOR, width=4)
    # 大眼睛小脑袋反差萌
    draw.ellipse([cx-25, cy-15, cx-5, cy+10], fill=INK_COLOR)
    draw.ellipse([cx-20, cy-12, cx-12, cy-4], fill=WHITE)
    draw.ellipse([cx+5, cy-15, cx+25, cy+10], fill=INK_COLOR)
    draw.ellipse([cx+10, cy-12, cx+18, cy-4], fill=WHITE)
    draw.ellipse([cx-8, cy+12, cx+8, cy+24], fill=INK_COLOR)
    # 蝴蝶结
    draw.polygon([(cx-18, cy+40), (cx-5, cy+46), (cx-18, cy+52)], fill=SOFT_RED, outline=INK_COLOR)
    draw.polygon([(cx+18, cy+40), (cx+5, cy+46), (cx+18, cy+52)], fill=SOFT_RED, outline=INK_COLOR)
    draw.ellipse([cx-6, cy+42, cx+6, cy+50], fill=SOFT_YELLOW, outline=INK_COLOR)
    return img

def draw_dog_silly():
    img, draw = create_base()
    draw.ellipse([40, 45, 160, 165], fill=SOFT_BROWN, outline=INK_COLOR, width=5)
    draw.ellipse([20, 30, 60, 120], fill=(130, 80, 40), outline=INK_COLOR, width=5)
    draw.ellipse([140, 30, 180, 120], fill=(130, 80, 40), outline=INK_COLOR, width=5)
    # 酷炫墨镜
    draw.rectangle([50, 70, 95, 100], fill=INK_COLOR)
    draw.rectangle([105, 70, 150, 100], fill=INK_COLOR)
    draw.line([95, 80, 105, 80], fill=INK_COLOR, width=5)
    draw.line([55, 75, 85, 95], fill=WHITE, width=3)
    draw.line([110, 75, 140, 95], fill=WHITE, width=3)
    # 歪头吐大舌头
    draw.ellipse([88, 105, 112, 125], fill=INK_COLOR)
    draw.arc([80, 120, 120, 140], start=0, end=180, fill=INK_COLOR, width=4)
    draw.ellipse([98, 130, 126, 175], fill=PINK, outline=INK_COLOR, width=4)
    draw.line([112, 135, 112, 165], fill=INK_COLOR, width=3)
    return img

# -------------------------------------------------------------
# BANANA 组
# -------------------------------------------------------------
def draw_banana_intro():
    img, draw = create_base()
    # 弯月香蕉
    draw.arc([20, 20, 180, 180], start=30, end=150, fill=SOFT_YELLOW, width=48)
    draw.arc([20, 20, 180, 180], start=30, end=150, fill=INK_COLOR, width=5)
    # 蕉柄
    draw.rectangle([140, 45, 160, 65], fill=SOFT_BROWN, outline=INK_COLOR, width=4)
    # 尾巴尖
    draw.ellipse([26, 138, 40, 152], fill=SOFT_BROWN)
    return img

def draw_banana_big():
    img, draw = create_base()
    # 巨大占屏香蕉
    draw.arc([5, 5, 205, 205], start=20, end=160, fill=SOFT_YELLOW, width=64)
    # 蕉柄
    draw.rectangle([160, 30, 185, 55], fill=SOFT_BROWN, outline=INK_COLOR, width=5)
    # 笑脸香蕉
    draw.ellipse([85, 105, 95, 115], fill=INK_COLOR)
    draw.ellipse([115, 100, 125, 110], fill=INK_COLOR)
    draw.arc([90, 115, 120, 135], start=0, end=180, fill=INK_COLOR, width=4)
    return img

def draw_banana_long():
    img, draw = create_base()
    # 超长细香蕉
    points = [(50, 170), (70, 110), (100, 60), (145, 30)]
    draw.line(points, fill=SOFT_YELLOW, width=36, joint="curve")
    draw.line(points, fill=INK_COLOR, width=4, joint="curve")
    draw.rectangle([140, 20, 155, 35], fill=SOFT_BROWN, outline=INK_COLOR, width=3)
    return img

def draw_banana_yellow():
    img, draw = create_base()
    # 亮金金发光香蕉 + 星星闪烁
    draw.arc([25, 25, 175, 175], start=30, end=150, fill=SOFT_YELLOW, width=50)
    draw.rectangle([140, 50, 158, 68], fill=SOFT_BROWN, outline=INK_COLOR, width=4)
    # 闪烁小十字星
    for sx, sy in [(50, 50), (160, 140), (70, 150)]:
        draw.line([sx-10, sy, sx+10, sy], fill=SOFT_ORANGE, width=3)
        draw.line([sx, sy-10, sx, sy+10], fill=SOFT_ORANGE, width=3)
    return img

# -------------------------------------------------------------
# DUCK 组
# -------------------------------------------------------------
def draw_duck_intro():
    img, draw = create_base()
    # 身体
    draw.ellipse([40, 80, 155, 160], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    # 翘翘尾巴
    draw.polygon([(40, 110), (20, 90), (45, 130)], fill=SOFT_YELLOW, outline=INK_COLOR)
    # 头部
    draw.ellipse([100, 40, 165, 105], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    # 眼睛
    draw.ellipse([135, 60, 147, 72], fill=INK_COLOR)
    draw.ellipse([138, 62, 142, 66], fill=WHITE)
    # 橙色鸭嘴
    draw.ellipse([150, 68, 185, 92], fill=SOFT_ORANGE, outline=INK_COLOR, width=4)
    return img

def draw_duck_swim():
    img, draw = create_base()
    # 游泳鸭子
    draw.ellipse([45, 60, 150, 135], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    draw.ellipse([95, 25, 155, 85], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    draw.ellipse([130, 45, 142, 57], fill=INK_COLOR)
    draw.ellipse([142, 52, 175, 74], fill=SOFT_ORANGE, outline=INK_COLOR, width=4)
    # 柔和水波浪
    for y in [140, 158]:
        draw.arc([20, y, 80, y+30], start=0, end=180, fill=SOFT_BLUE, width=5)
        draw.arc([80, y, 140, y+30], start=0, end=180, fill=SOFT_BLUE, width=5)
        draw.arc([140, y, 200, y+30], start=0, end=180, fill=SOFT_BLUE, width=5)
    return img

def draw_duck_quack():
    img, draw = create_base()
    # 大张嘴嘎嘎叫
    draw.ellipse([40, 80, 155, 160], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    draw.ellipse([100, 40, 165, 105], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    draw.ellipse([135, 55, 147, 67], fill=INK_COLOR)
    # 张开的上下喙
    draw.polygon([(150, 70), (185, 60), (155, 78)], fill=SOFT_ORANGE, outline=INK_COLOR)
    draw.polygon([(150, 82), (185, 95), (155, 78)], fill=SOFT_ORANGE, outline=INK_COLOR)
    # 音乐符号音符
    draw.ellipse([165, 30, 175, 40], fill=SOFT_RED)
    draw.line([175, 35, 175, 20], fill=SOFT_RED, width=3)
    draw.line([175, 20, 185, 25], fill=SOFT_RED, width=3)
    return img

def draw_duck_fly():
    img, draw = create_base()
    # 扑哧扑哧飞天鸭
    draw.ellipse([45, 70, 155, 145], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    draw.ellipse([100, 35, 165, 95], fill=SOFT_YELLOW, outline=INK_COLOR, width=5)
    draw.ellipse([135, 50, 147, 62], fill=INK_COLOR)
    draw.ellipse([150, 58, 185, 80], fill=SOFT_ORANGE, outline=INK_COLOR, width=4)
    # 展开的翅膀
    draw.ellipse([65, 30, 115, 90], fill=SOFT_ORANGE, outline=INK_COLOR, width=4)
    # 云朵
    draw.ellipse([20, 145, 60, 175], fill=WHITE, outline=INK_COLOR, width=3)
    draw.ellipse([45, 135, 90, 175], fill=WHITE, outline=INK_COLOR, width=3)
    return img

# -------------------------------------------------------------
# CAR 组
# -------------------------------------------------------------
def draw_car_intro():
    img, draw = create_base()
    # 车顶
    draw.arc([60, 45, 140, 115], start=180, end=360, fill=SOFT_RED, width=28)
    # 车身
    draw.rounded_rectangle([30, 90, 170, 140], radius=15, fill=SOFT_RED, outline=INK_COLOR, width=5)
    # 车窗
    draw.rectangle([75, 65, 125, 92], fill=SOFT_BLUE, outline=INK_COLOR, width=4)
    # 车轮
    draw.ellipse([45, 125, 85, 165], fill=INK_COLOR)
    draw.ellipse([57, 137, 73, 153], fill=WHITE)
    draw.ellipse([115, 125, 155, 165], fill=INK_COLOR)
    draw.ellipse([127, 137, 143, 153], fill=WHITE)
    return img

def draw_car_fast():
    img, draw = create_base()
    # 疾驰小汽车 + 速度线条
    draw.rounded_rectangle([40, 80, 175, 130], radius=15, fill=SOFT_RED, outline=INK_COLOR, width=5)
    draw.arc([75, 40, 150, 110], start=180, end=360, fill=SOFT_RED, width=24)
    draw.rectangle([85, 58, 135, 82], fill=SOFT_BLUE, outline=INK_COLOR, width=4)
    draw.ellipse([55, 115, 90, 150], fill=INK_COLOR)
    draw.ellipse([125, 115, 160, 150], fill=INK_COLOR)
    # 速度冲线
    draw.line([10, 85, 30, 85], fill=INK_COLOR, width=4)
    draw.line([5, 105, 28, 105], fill=INK_COLOR, width=4)
    draw.line([15, 125, 35, 125], fill=INK_COLOR, width=4)
    return img

def draw_car_red():
    img, draw = create_base()
    # 经典红色大卡丁
    draw.rounded_rectangle([25, 85, 175, 140], radius=18, fill=(235, 60, 60), outline=INK_COLOR, width=5)
    draw.arc([55, 38, 145, 118], start=180, end=360, fill=(235, 60, 60), width=32)
    draw.rectangle([70, 60, 130, 88], fill=SOFT_BLUE, outline=INK_COLOR, width=4)
    draw.ellipse([42, 120, 86, 164], fill=INK_COLOR)
    draw.ellipse([56, 134, 72, 150], fill=WHITE)
    draw.ellipse([118, 120, 162, 164], fill=INK_COLOR)
    draw.ellipse([132, 134, 148, 150], fill=WHITE)
    # 车前黄色大灯
    draw.ellipse([165, 96, 182, 114], fill=SOFT_YELLOW, outline=INK_COLOR, width=3)
    return img

def draw_car_beep():
    img, draw = create_base()
    draw.rounded_rectangle([30, 90, 160, 140], radius=15, fill=SOFT_RED, outline=INK_COLOR, width=5)
    draw.arc([60, 48, 135, 118], start=180, end=360, fill=SOFT_RED, width=26)
    draw.rectangle([72, 68, 122, 92], fill=SOFT_BLUE, outline=INK_COLOR, width=4)
    draw.ellipse([45, 125, 85, 165], fill=INK_COLOR)
    draw.ellipse([110, 125, 150, 165], fill=INK_COLOR)
    # 嘟嘟声响声波弧线
    draw.arc([155, 90, 185, 120], start=300, end=420, fill=SOFT_ORANGE, width=5)
    draw.arc([165, 80, 205, 130], start=300, end=420, fill=SOFT_ORANGE, width=5)
    return img

# -------------------------------------------------------------
# APPLE 组
# -------------------------------------------------------------
def draw_apple_intro():
    img, draw = create_base()
    # 苹果主体
    draw.ellipse([40, 50, 125, 165], fill=SOFT_RED, outline=INK_COLOR, width=5)
    draw.ellipse([75, 50, 160, 165], fill=SOFT_RED, outline=INK_COLOR, width=5)
    # 苹果柄
    draw.line([100, 20, 100, 55], fill=SOFT_BROWN, width=6)
    # 嫩绿树叶
    draw.ellipse([100, 20, 140, 45], fill=SOFT_GREEN, outline=INK_COLOR, width=4)
    # 苹果光晕
    draw.arc([55, 65, 90, 130], start=120, end=200, fill=WHITE, width=5)
    return img

def draw_apple_big():
    img, draw = create_base()
    # 巨大苹果占满全屏
    draw.ellipse([15, 30, 125, 185], fill=SOFT_RED, outline=INK_COLOR, width=6)
    draw.ellipse([75, 30, 185, 185], fill=SOFT_RED, outline=INK_COLOR, width=6)
    draw.line([100, 5, 100, 40], fill=SOFT_BROWN, width=8)
    draw.ellipse([100, 8, 155, 38], fill=SOFT_GREEN, outline=INK_COLOR, width=5)
    return img

def draw_apple_yummy():
    img, draw = create_base()
    # 咬了一大口的脆甜苹果
    draw.ellipse([40, 50, 125, 165], fill=SOFT_RED, outline=INK_COLOR, width=5)
    draw.ellipse([75, 50, 160, 165], fill=SOFT_RED, outline=INK_COLOR, width=5)
    draw.line([100, 20, 100, 55], fill=SOFT_BROWN, width=6)
    draw.ellipse([100, 20, 140, 45], fill=SOFT_GREEN, outline=INK_COLOR, width=4)
    # 咬痕凹陷 (背景色半圆)
    draw.ellipse([135, 75, 185, 135], fill=BG_COLOR, outline=INK_COLOR, width=5)
    return img

# -------------------------------------------------------------
# DEE 组
# -------------------------------------------------------------
def draw_dee_welcome():
    img, draw = create_base()
    # Dee 可爱主角大头肖像
    cx, cy = 100, 95
    # 头
    draw.ellipse([cx-55, cy-50, cx+55, cy+50], fill=(255, 224, 189), outline=INK_COLOR, width=5)
    # 棕色短发
    draw.arc([cx-58, cy-58, cx+58, cy+10], start=180, end=360, fill=(110, 60, 30), width=18)
    # 眼睛
    draw.ellipse([cx-32, cy-15, cx-16, cy+5], fill=INK_COLOR)
    draw.ellipse([cx-28, cy-12, cx-22, cy-6], fill=WHITE)
    draw.ellipse([cx+16, cy-15, cx+32, cy+5], fill=INK_COLOR)
    draw.ellipse([cx+20, cy-12, cx+26, cy-6], fill=WHITE)
    # 粉扑扑腮红
    draw.ellipse([cx-45, cy+8, cx-25, cy+22], fill=PINK)
    draw.ellipse([cx+25, cy+8, cx+45, cy+22], fill=PINK)
    # 灿烂微笑
    draw.chord([cx-22, cy+5, cx+22, cy+32], start=0, end=180, fill=SOFT_RED, outline=INK_COLOR, width=4)
    # 挥手致意
    draw.ellipse([cx+55, cy-10, cx+85, cy+20], fill=(255, 224, 189), outline=INK_COLOR, width=4)
    return img

def draw_dee_listen():
    img, draw = create_base()
    # Dee 侧耳倾听 (大耳朵凑近)
    cx, cy = 90, 100
    draw.ellipse([cx-50, cy-50, cx+50, cy+50], fill=(255, 224, 189), outline=INK_COLOR, width=5)
    draw.arc([cx-55, cy-58, cx+55, cy+10], start=180, end=360, fill=(110, 60, 30), width=18)
    draw.ellipse([cx-25, cy-10, cx-10, cy+8], fill=INK_COLOR)
    draw.ellipse([cx+10, cy-10, cx+25, cy+8], fill=INK_COLOR)
    draw.arc([cx-15, cy+12, cx+15, cy+28], start=0, end=180, fill=INK_COLOR, width=4)
    # 巨型倾听大耳朵
    draw.ellipse([cx+42, cy-25, cx+80, cy+30], fill=(255, 200, 170), outline=INK_COLOR, width=5)
    draw.ellipse([cx+52, cy-12, cx+72, cy+18], fill=(230, 160, 140))
    # 声音传入波纹
    draw.arc([cx+75, cy-40, cx+115, cy+40], start=300, end=420, fill=SOFT_BLUE, width=5)
    draw.arc([cx+90, cy-55, cx+135, cy+55], start=300, end=420, fill=SOFT_BLUE, width=5)
    return img

def draw_dee_sleep():
    img, draw = create_base()
    cx, cy = 100, 115
    draw.ellipse([cx-50, cy-50, cx+50, cy+50], fill=(255, 224, 189), outline=INK_COLOR, width=5)
    # 睡帽
    draw.polygon([(cx-50, cy-35), (cx+50, cy-35), (cx-60, cy-75)], fill=SOFT_BLUE, outline=INK_COLOR)
    draw.ellipse([cx-72, cy-85, cx-50, cy-65], fill=WHITE, outline=INK_COLOR, width=3)
    # 安详闭眼 (弯弧线)
    draw.arc([cx-32, cy-12, cx-12, cy+2], start=0, end=180, fill=INK_COLOR, width=4)
    draw.arc([cx+12, cy-12, cx+32, cy+2], start=0, end=180, fill=INK_COLOR, width=4)
    draw.ellipse([cx-8, cy+12, cx+8, cy+26], fill=SOFT_RED)
    # 漂浮的 Zzz 睡眠气泡
    draw.ellipse([cx+45, cy-40, cx+55, cy-30], fill=WHITE, outline=INK_COLOR, width=2)
    draw.ellipse([cx+58, cy-60, cx+72, cy-46], fill=WHITE, outline=INK_COLOR, width=2)
    draw.ellipse([cx+72, cy-85, cx+90, cy-67], fill=WHITE, outline=INK_COLOR, width=3)
    return img

ASSETS = {
    "dog_intro": draw_dog_intro,
    "dog_big": draw_dog_big,
    "dog_tiny": draw_dog_tiny,
    "dog_silly": draw_dog_silly,
    "banana_intro": draw_banana_intro,
    "banana_big": draw_banana_big,
    "banana_long": draw_banana_long,
    "banana_yellow": draw_banana_yellow,
    "duck_intro": draw_duck_intro,
    "duck_swim": draw_duck_swim,
    "duck_quack": draw_duck_quack,
    "duck_fly": draw_duck_fly,
    "car_intro": draw_car_intro,
    "car_fast": draw_car_fast,
    "car_red": draw_car_red,
    "car_beep": draw_car_beep,
    "apple_intro": draw_apple_intro,
    "apple_big": draw_apple_big,
    "apple_yummy": draw_apple_yummy,
    "dee_welcome": draw_dee_welcome,
    "dee_listen": draw_dee_listen,
    "dee_sleep": draw_dee_sleep,
}

def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    for name, func in ASSETS.items():
        img = func()
        assert img.size == SIZE
        raw = rgb565_le(img)
        assert len(raw) == SIZE[0] * SIZE[1] * 2
        bin_path = OUT_DIR / f"{name}.rgb565"
        bin_path.write_bytes(raw)
        print(f"Generated {name}.rgb565: {len(raw)} bytes")

if __name__ == "__main__":
    main()
