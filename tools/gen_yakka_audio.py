#!/usr/bin/env python3
# tools/gen_yakka_audio.py
"""Generate 16kHz 16-bit mono PCM audio clips for Yakka Dee Pocket Talker."""

import math
from pathlib import Path
import struct
import subprocess
import wave

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "main" / "assets"
SAMPLE_RATE = 16000

# -------------------------------------------------------------
# 基础波形与音效合成
# -------------------------------------------------------------
def make_sine(freq: float, duration_s: float, volume: float = 0.5) -> list[int]:
    n_samples = int(SAMPLE_RATE * duration_s)
    samples = []
    for i in range(n_samples):
        t = i / SAMPLE_RATE
        val = int(32767 * volume * math.sin(2.0 * math.pi * freq * t))
        samples.append(val)
    return samples

def apply_envelope(samples: list[int], attack_s: float, release_s: float) -> list[int]:
    n = len(samples)
    att_samples = int(SAMPLE_RATE * attack_s)
    rel_samples = int(SAMPLE_RATE * release_s)
    out = []
    for i, s in enumerate(samples):
        gain = 1.0
        if i < att_samples and att_samples > 0:
            gain = i / att_samples
        elif i >= n - rel_samples and rel_samples > 0:
            gain = (n - i) / rel_samples
        out.append(int(s * max(0.0, min(1.0, gain))))
    return out

def synth_bark(pitch_hz: float = 240, duration_s: float = 0.22, vol: float = 0.7) -> list[int]:
    n = int(SAMPLE_RATE * duration_s)
    samples = []
    phase = 0.0
    for i in range(n):
        t = i / SAMPLE_RATE
        f = pitch_hz * (1.0 - 0.5 * (t / duration_s))
        phase += 2.0 * math.pi * f / SAMPLE_RATE
        # 基频 + 3 次谐波
        val = math.sin(phase) + 0.4 * math.sin(2 * phase) + 0.2 * math.sin(3 * phase)
        # 包络
        env = math.exp(-6.0 * t / duration_s)
        samples.append(int(32767 * vol * val * env))
    return samples

def synth_slide_whistle(duration_s: float = 0.35, vol: float = 0.5) -> list[int]:
    n = int(SAMPLE_RATE * duration_s)
    samples = []
    phase = 0.0
    for i in range(n):
        t = i / SAMPLE_RATE
        f = 350.0 + 850.0 * (t / duration_s)
        phase += 2.0 * math.pi * f / SAMPLE_RATE
        samples.append(int(32767 * vol * math.sin(phase)))
    return apply_envelope(samples, 0.03, 0.05)

def synth_duck_quack(duration_s: float = 0.25, vol: float = 0.6) -> list[int]:
    n = int(SAMPLE_RATE * duration_s)
    samples = []
    for i in range(n):
        t = i / SAMPLE_RATE
        f = 420.0
        # 鼻音调制
        val = math.sin(2 * math.pi * f * t) + 0.6 * math.sin(2 * math.pi * (f * 2.2) * t)
        env = math.sin(math.pi * (t / duration_s)) ** 2
        samples.append(int(32767 * vol * val * env))
    return samples

def synth_car_honk(duration_s: float = 0.28, vol: float = 0.6) -> list[int]:
    n = int(SAMPLE_RATE * duration_s)
    samples = []
    for i in range(n):
        t = i / SAMPLE_RATE
        # 双音和弦 (440Hz + 554Hz)
        val = 0.5 * math.sin(2 * math.pi * 440.0 * t) + 0.5 * math.sin(2 * math.pi * 554.0 * t)
        samples.append(int(32767 * vol * val))
    return apply_envelope(samples, 0.02, 0.03)

def synth_apple_crunch(duration_s: float = 0.25, vol: float = 0.5) -> list[int]:
    import random
    n = int(SAMPLE_RATE * duration_s)
    samples = []
    for i in range(n):
        t = i / SAMPLE_RATE
        noise = (random.random() * 2.0 - 1.0)
        env = math.exp(-12.0 * t / duration_s)
        samples.append(int(32767 * vol * noise * env))
    return samples

def synth_pop(vol: float = 0.15) -> list[int]:
    # 萌系水滴啵啵声 (约 45ms)
    duration_s = 0.045
    n = int(SAMPLE_RATE * duration_s)
    samples = []
    for i in range(n):
        t = i / SAMPLE_RATE
        f = 600.0 + 300.0 * (1.0 - t / duration_s)
        env = math.sin(math.pi * (t / duration_s))
        samples.append(int(32767 * vol * math.sin(2 * math.pi * f * t) * env))
    return samples

def synth_fanfare() -> list[int]:
    notes = [523.25, 659.25, 783.99, 1046.50] # C5, E5, G5, C6
    samples = []
    for freq in notes:
        s = make_sine(freq, 0.12, 0.45)
        samples.extend(apply_envelope(s, 0.01, 0.03))
    # 结尾长音
    s_end = make_sine(1046.50, 0.35, 0.5)
    samples.extend(apply_envelope(s_end, 0.01, 0.1))
    return samples

def synth_skip_chime() -> list[int]:
    notes = [659.25, 783.99, 880.00] # E5, G5, A5
    samples = []
    for freq in notes:
        s = make_sine(freq, 0.12, 0.4)
        samples.extend(apply_envelope(s, 0.01, 0.04))
    return samples

def synth_parent_unlock() -> list[int]:
    notes = [440.0, 880.0]
    samples = []
    for freq in notes:
        s = make_sine(freq, 0.10, 0.35)
        samples.extend(apply_envelope(s, 0.01, 0.02))
    return samples

def synth_lullaby() -> list[int]:
    notes = [659.25, 587.33, 523.25, 392.00] # E5, D5, C5, G4
    samples = []
    for freq in notes:
        s = make_sine(freq, 0.35, 0.3)
        samples.extend(apply_envelope(s, 0.03, 0.15))
    return samples

# -------------------------------------------------------------
# macOS 语音合成 (say)
# -------------------------------------------------------------
def tts_to_pcm(text: str, voice: str = "Flo") -> list[int]:
    tmp_wav = Path(f"/tmp/yakka_tts_{abs(hash(text))}.wav")
    try:
        cmd = [
            "say",
            "-v", voice,
            "-o", str(tmp_wav),
            "--data-format=LEI16@16000",
            text
        ]
        res = subprocess.run(cmd, capture_output=True, text=True)
        if res.returncode != 0:
            # 降级尝试默认声音
            cmd[2] = "Samantha"
            subprocess.run(cmd, check=True)

        with wave.open(str(tmp_wav), "rb") as wf:
            n = wf.getnframes()
            data = wf.readframes(n)
            samples = list(struct.unpack(f"<{n}h", data))
            return samples
    except Exception as e:
        print(f"TTS warning for '{text}': {e}, using synthesized chime")
        return synth_fanfare()
    finally:
        if tmp_wav.exists():
            tmp_wav.unlink()

def write_pcm(path: Path, samples: list[int]):
    max_val = max(abs(x) for x in samples) if samples else 0
    if max_val > 0 and path.name != "audio_pop.pcm":
        # 归一化至接近满幅 -0.5 dBFS (~31000)，保证小扬声器发音饱满清脆
        gain = 31000.0 / max_val
        samples = [int(x * gain) for x in samples]
    clamped = [max(-32768, min(32767, int(x))) for x in samples]
    raw = struct.pack(f"<{len(clamped)}h", *clamped)
    path.write_bytes(raw)
    print(f"Generated {path.name}: {len(raw)} bytes ({len(clamped)/SAMPLE_RATE:.2f}s, peak {max(abs(x) for x in clamped)})")

# -------------------------------------------------------------
# 生成全部词汇与系统语音包
# -------------------------------------------------------------
def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    # 1. 开机语音
    s = tts_to_pcm("Yakka Dee! Yakka Dee! Talk, talk, talk! You say it!")
    write_pcm(OUT_DIR / "audio_boot.pcm", s)

    # 2. 欢呼与正向大爆发
    s_cheer = tts_to_pcm("You did it! Hooray!") + synth_fanfare()
    write_pcm(OUT_DIR / "audio_cheer.pcm", s_cheer)

    # 3. 兜底与跳过音效
    s_skip = synth_skip_chime()
    write_pcm(OUT_DIR / "audio_skip.pcm", s_skip)

    # 4. Dee 睡午觉
    s_nap = tts_to_pcm("Dee is sleepy. Time for a nap. Shh, see you later!") + synth_lullaby()
    write_pcm(OUT_DIR / "audio_nap.pcm", s_nap)

    # 5. 按钮水滴啵啵声与家长解锁升调
    write_pcm(OUT_DIR / "audio_pop.pcm", synth_pop())
    write_pcm(OUT_DIR / "audio_parent_on.pcm", synth_parent_unlock())

    # 6. 五组单词专属音频
    VOCAB_SPEECH = {
        "dog": {
            "intro": "Let's talk about DOG!",
            "c1": ("Big dog!", synth_bark(160, 0.28, 0.8)),
            "c2": ("Tiny dog!", synth_bark(420, 0.16, 0.7)),
            "c3": ("Silly dog!", synth_slide_whistle(0.3, 0.6)),
            "q": "Your turn! You say DOG!",
            "retry": ("Say DOG!", synth_bark(260, 0.2, 0.6)),
        },
        "banana": {
            "intro": "Let's talk about BANANA!",
            "c1": ("Big banana!", synth_slide_whistle(0.3, 0.6)),
            "c2": ("Long banana!", synth_slide_whistle(0.4, 0.6)),
            "c3": ("Yellow banana!", synth_skip_chime()),
            "q": "Your turn! You say BANANA!",
            "retry": ("Say BANANA!", synth_skip_chime()),
        },
        "duck": {
            "intro": "Let's talk about DUCK!",
            "c1": ("Swimming duck!", synth_duck_quack(0.24, 0.6)),
            "c2": ("Quacking duck!", synth_duck_quack(0.28, 0.7)),
            "c3": ("Flying duck!", synth_slide_whistle(0.32, 0.5)),
            "q": "Your turn! You say DUCK!",
            "retry": ("Say DUCK!", synth_duck_quack(0.2, 0.6)),
        },
        "car": {
            "intro": "Let's talk about CAR!",
            "c1": ("Fast car!", synth_slide_whistle(0.25, 0.6)),
            "c2": ("Red car!", synth_car_honk(0.25, 0.7)),
            "c3": ("Beep-beep car!", synth_car_honk(0.35, 0.8)),
            "q": "Your turn! You say CAR!",
            "retry": ("Say CAR!", synth_car_honk(0.22, 0.6)),
        },
        "apple": {
            "intro": "Let's talk about APPLE!",
            "c1": ("Red apple!", synth_apple_crunch(0.22, 0.6)),
            "c2": ("Big apple!", synth_apple_crunch(0.28, 0.7)),
            "c3": ("Yummy apple!", synth_apple_crunch(0.30, 0.8)),
            "q": "Your turn! You say APPLE!",
            "retry": ("Say APPLE!", synth_apple_crunch(0.22, 0.6)),
        },
    }

    for vocab, clips in VOCAB_SPEECH.items():
        # intro
        s = tts_to_pcm(clips["intro"])
        write_pcm(OUT_DIR / f"audio_{vocab}_intro.pcm", s)

        # c1, c2, c3
        for c in ["c1", "c2", "c3"]:
            txt, sfx = clips[c]
            s = tts_to_pcm(txt) + sfx
            write_pcm(OUT_DIR / f"audio_{vocab}_{c}.pcm", s)

        # q
        s = tts_to_pcm(clips["q"])
        write_pcm(OUT_DIR / f"audio_{vocab}_q.pcm", s)

        # retry
        txt, sfx = clips["retry"]
        s = tts_to_pcm(txt) + sfx
        write_pcm(OUT_DIR / f"audio_{vocab}_retry.pcm", s)

if __name__ == "__main__":
    main()
