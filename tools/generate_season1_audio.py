#!/usr/bin/env python3
# tools/generate_season1_audio.py
# 为 Yakka Dee 第一季全套 20 个词汇生成纯正英音儿童跟读音频 (16kHz ADPCM)

import os
import struct
import subprocess
from pathlib import Path

step_table = [
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
]
index_table = [-1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1, -1, 2, 4, 6, 8]

def decode_nibble(nib, state):
    step = step_table[state['index']]
    diff = step >> 3
    if nib & 4: diff += step
    if nib & 2: diff += step >> 1
    if nib & 1: diff += step >> 2
    if nib & 8: diff = -diff
    samp = state['prev'] + diff
    samp = max(-32768, min(32767, samp))
    state['prev'] = samp
    idx = state['index'] + index_table[nib & 0xf]
    state['index'] = max(0, min(88, idx))
    return samp

def encode_sample(samp, state):
    step = step_table[state['index']]
    diff = samp - state['prev']
    nib = 0
    if diff < 0:
        nib |= 8
        diff = -diff
    if diff >= step:
        nib |= 4
        diff -= step
    if diff >= (step >> 1):
        nib |= 2
        diff -= (step >> 1)
    if diff >= (step >> 2):
        nib |= 1
    decode_nibble(nib, state)
    return nib

def pcm_to_adpcm(pcm_bytes):
    samples = struct.unpack(f'<{len(pcm_bytes)//2}h', pcm_bytes)
    state = {'prev': 0, 'index': 0}
    out = bytearray()
    for i in range(0, len(samples), 2):
        nib0 = encode_sample(samples[i], state)
        nib1 = encode_sample(samples[i+1], state) if i+1 < len(samples) else 0
        out.append((nib1 << 4) | (nib0 & 0x0F))
    return bytes(out)

CURRICULUM = [
    {"id": "banana", "name": "Banana", "phrase": "Banana! Yellow banana!", "c1": "Big banana!", "c2": "Long banana!", "c3": "Yellow banana!"},
    {"id": "dog",    "name": "Dog",    "phrase": "Dog! Woof woof! Big dog!", "c1": "Big dog!", "c2": "Tiny dog!", "c3": "Silly dog!"},
    {"id": "book",   "name": "Book",   "phrase": "Book! Look at the book!", "c1": "Big book!", "c2": "Story book!", "c3": "Open book!"},
    {"id": "boots",  "name": "Boots",  "phrase": "Boots! Splash in the boots!", "c1": "Red boots!", "c2": "Muddy boots!", "c3": "Shiny boots!"},
    {"id": "bike",   "name": "Bike",   "phrase": "Bike! Ride the bike!", "c1": "Fast bike!", "c2": "Little bike!", "c3": "Ring-ring bike!"},
    {"id": "duck",   "name": "Duck",   "phrase": "Duck! Quack quack! Little duck!", "c1": "Swimming duck!", "c2": "Quacking duck!", "c3": "Yellow duck!"},
    {"id": "cup",    "name": "Cup",    "phrase": "Cup! Drink from the cup!", "c1": "Blue cup!", "c2": "Big cup!", "c3": "Sippy cup!"},
    {"id": "bus",    "name": "Bus",    "phrase": "Bus! Beep beep! The bus is here!", "c1": "Big bus!", "c2": "Red bus!", "c3": "Beep-beep bus!"},
    {"id": "peas",   "name": "Peas",   "phrase": "Peas! Green peas! Pop the peas!", "c1": "Green peas!", "c2": "Tiny peas!", "c3": "Sweet peas!"},
    {"id": "hat",    "name": "Hat",    "phrase": "Hat! Put on your hat!", "c1": "Sun hat!", "c2": "Silly hat!", "c3": "Warm hat!"},
    {"id": "worm",   "name": "Worm",   "phrase": "Worm! Wiggly worm!", "c1": "Wiggly worm!", "c2": "Long worm!", "c3": "Earth worm!"},
    {"id": "boat",   "name": "Boat",   "phrase": "Boat! Float the boat!", "c1": "Sailing boat!", "c2": "Speed boat!", "c3": "Floating boat!"},
    {"id": "house",  "name": "House",  "phrase": "House! In the house!", "c1": "Cozy house!", "c2": "Tall house!", "c3": "Brick house!"},
    {"id": "lion",   "name": "Lion",   "phrase": "Lion! Roar! Big lion!", "c1": "Roaring lion!", "c2": "Big lion!", "c3": "Golden lion!"},
    {"id": "apple",  "name": "Apple",  "phrase": "Apple! Crunch! Red apple!", "c1": "Red apple!", "c2": "Sweet apple!", "c3": "Crunchy apple!"},
    {"id": "ball",   "name": "Ball",   "phrase": "Ball! Bounce the ball!", "c1": "Bouncing ball!", "c2": "Round ball!", "c3": "Rolling ball!"},
    {"id": "mouse",  "name": "Mouse",  "phrase": "Mouse! Squeak! Little mouse!", "c1": "Little mouse!", "c2": "Squeaky mouse!", "c3": "Quiet mouse!"},
    {"id": "beans",  "name": "Beans",  "phrase": "Beans! Jumping beans!", "c1": "Magic beans!", "c2": "Green beans!", "c3": "Jumping beans!"},
    {"id": "car",    "name": "Car",    "phrase": "Car! Vroom vroom! Fast car!", "c1": "Fast car!", "c2": "Red car!", "c3": "Beep-beep car!"},
    {"id": "bed",    "name": "Bed",    "phrase": "Bed! Sleepy time! Big bed!", "c1": "Soft bed!", "c2": "Sleepy bed!", "c3": "Cozy bed!"},
]

VOICE = "Flo (English (UK))"
OUT_DIR = Path("main/assets")
OUT_DIR.mkdir(parents=True, exist_ok=True)
TMP_DIR = Path("/tmp/yakka_audio_gen")
TMP_DIR.mkdir(parents=True, exist_ok=True)

def synthesize_clip(text, filename):
    adpcm_path = OUT_DIR / f"{filename}.adpcm"
    # 如果已存在且有效，复用
    if adpcm_path.exists() and adpcm_path.stat().st_size > 500:
        return

    aiff_tmp = TMP_DIR / f"{filename}.aiff"
    pcm_tmp = TMP_DIR / f"{filename}.pcm"

    subprocess.run(["say", "-v", VOICE, "-r", "165", "-o", str(aiff_tmp), text], check=True)
    subprocess.run([
        "ffmpeg", "-y", "-i", str(aiff_tmp),
        "-ar", "16000", "-ac", "1", "-f", "s16le", str(pcm_tmp)
    ], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    pcm_data = pcm_tmp.read_bytes()
    adpcm_data = pcm_to_adpcm(pcm_data)
    adpcm_path.write_bytes(adpcm_data)
    print(f"Generated {filename}.adpcm: {len(adpcm_data)} bytes ({text})")

def main():
    # 系统音频
    system_clips = [
        ("audio_boot", "Yakka Dee! Talk talk talk!"),
        ("audio_cheer", "Hurray! You did it! Brilliant!"),
        ("audio_skip", "Whoosh!"),
        ("audio_nap", "Yawn... Time for a little nap! Night night!"),
        ("audio_pop", "Pop!"),
        ("audio_parent_on", "Hello grown-up!"),
    ]
    for fn, txt in system_clips:
        synthesize_clip(txt, fn)

    # 20 组词汇音频
    for item in CURRICULUM:
        w_id = item["id"]
        w_name = item["name"]
        phrase = item["phrase"]
        # 引出词汇展示：使用官方精炼短语
        synthesize_clip(phrase, f"audio_{w_id}_intro")
        synthesize_clip(item["c1"], f"audio_{w_id}_c1")
        synthesize_clip(item["c2"], f"audio_{w_id}_c2")
        synthesize_clip(item["c3"], f"audio_{w_id}_c3")
        synthesize_clip(f"You choose! You say {w_name}!", f"audio_{w_id}_q")
        synthesize_clip(f"Go on! You can say it: {w_name}!", f"audio_{w_id}_retry")

    print("All Season 1 audio clips generated successfully!")

if __name__ == "__main__":
    main()
