<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Yakka Dee Pocket Talker

A specialized physical speech motivation toy tailored for **toddlers in their language burst period (around 2 years old)**. Powered by the **FoloToy AI Passport** (ESP32-C3 hardware architecture), operating **100% offline with zero network dependency, zero cloud latency, and instant play upon powering on**.

> **“Yakka Dee, Yakka Dee, talk, talk, talk! You say it!”**  
> Say goodbye to passive screen addiction and eye strain—empower your child to speak up with joyful cheers!

---

## 🌟 Core Features & Child-Centric Design

### 1. Zero-Button Automated Interaction
* **2-Year-Old Cognitive Adaptation**: Toddlers do not need to understand buttons, menus, or navigation. The state machine automatically plays prompts and advances through scenes.
* **Acoustic Energy Detection**: An ES8311 I2S microphone continuously calculates short-time Root-Mean-Square (RMS) amplitude. As long as the toddler speaks, babbles, or shouts, **success is triggered within 100ms**!
* **Slow-Motion Confetti Celebration**: Upon speaking, the screen bursts with gentle particle confetti accompanied by joyful cheers, reinforcing the causal relationship: *"I speak ➔ fireworks burst"*.
* **Gentle Encouragement on Silence**: If the child stays silent, Dee gently repeats with a soft animal prompt. After timeout, it smoothly transitions to the next word without any frustrating buzzer or error sound.

### 2. Medical-Grade Eye-Care Optics & Nap-Time Mode
* **#F6F2E5 Warm Paper Background**: Eliminates harsh high-energy pure whites and high-contrast pitch blacks, providing a soothing reflection like paper picture books.
* **High-Visibility 4px Bold Outlines**: 200×200 silhouettes fill 80% of the display, making shapes clear at a healthy 40–50cm distance to discourage leaning in close.
* **15kHz High-Frequency Stroboscopic-Free PWM**: Backlight dimming frequency eliminates hidden ocular muscle fatigue.
* **"Dee Takes a Nap" Anti-Addiction**: After 25 words or 15 minutes of inactivity, Dee yawns, hugs her brown teddy bear, and falls asleep (`Zzz...`). The screen softly dims to a **10% nightlight** rather than cutting off abruptly. Parents can wake Dee anytime with a **long press on the OK key**.

### 3. Anti-Touch Grab Shield & Parent Mode
* **Complete Button Muting**: Normal toddler play completely disables button interruptions.
* **Hold OK for 2s to Unlock Parent Menu**: Tailored for the single-channel ADC resistor ladder circuit on GPIO0, allowing parents to adjust:
  * **Master Volume** (25% / 50% / 75% / 100%, 0 dBFS peak normalized);
  * **Mic Sensitivity** (Low / Mid / High dynamic noise floor gating);
  * **Backlight Brightness** (25% / 50% / 75%);
  * **Manual Vocabulary Selector**.

### 4. Canonical BBC CBeebies Artwork
* 100% faithful to the official character design by Chris Allen: Deepangali (Dee), South Asian toddler girl with straight bangs, red-scrunchie rotating pigtails, cerulean blue T-shirt with yellow lightning bolt ⚡, and red shorts.
* All 22 illustrations generated via **Nano Banana 2.1** for authentic CBeebies picture book style across 5 foundational words: DOG, BANANA, CAR, DUCK, and APPLE.

---

## 🛠️ Hardware & Architecture

| Module | Specification | Engineering Decisions |
| :--- | :--- | :--- |
| **Main SoC** | ESP32-C3 (Single-core RISC-V @ 160MHz) | Lightweight, energy-efficient, no external PSRAM |
| **Memory / Storage**| 400KB SRAM / 8MB SPI Flash | Trades flash capacity for CPU cycles; zero-heap flash streaming |
| **Display** | 240×320 ST7789P3 SPI LCD | 200×200 RGB565 Little-Endian raw data via SPI DMA direct-to-screen |
| **Audio** | ES8311 I2S Codec / Cavity Speaker / Mic | 16kHz 16-bit asynchronous pipeline with real-time RMS detection |
| **Fuel Gauge** | CW2017 I2C Hardware Fuel Gauge | Low-power battery profile monitoring |
| **Keys** | Single ADC1_CH0 (GPIO0) Resistor Ladder | Single-key long-press gesture (Hold OK 2s) for parent mode |

---

## 🚀 Quick Start & Flashing

### 1. Flash Precompiled Merged Firmware at 0x0

Connect the AI Passport to your computer via USB (press and hold power button to turn on):

```bash
# Locate serial port (macOS example)
ls /dev/cu.usbmodem*

# Flash the complete merged image from offset 0x0
esptool.py -p /dev/cu.usbmodem1101 -b 460800 write_flash 0x0 build/FoloToy-AI-Passport-full.bin
```

### 2. Preview Raw RGB565 Image Assets

Use the built-in helper script to decode and preview raw image assets:

```bash
# View a single raw asset in macOS Preview
python3 tools/view_image.py main/assets/dee_sleep.rgb565

# Export all 22 assets to PNG in Finder
python3 tools/view_image.py --all
```

### 3. Build & Validate

```bash
# 1. Run static checks and host unit tests
./tools/validate.sh --static

# 2. Build firmware and produce merged binary
./tools/validate.sh --firmware

# 3. Run complete gate verification
./tools/validate.sh
```

---

## 📁 Repository Layout

```text
├── main/
│   ├── assets/              # 22 RGB565 images & 31 PCM audio assets
│   ├── audio_energy.c/h     # Real-time I2S RMS vocal energy detector
│   ├── audio_player.c/h     # Asynchronous I2S DMA audio driver
│   ├── confetti.c/h         # Slow particle confetti celebration engine
│   ├── lv_font_cn_16.c/h    # Font file covering all Chinese glyphs
│   ├── ui_yakka.c/h         # Eye-care UI screens and transitions
│   ├── yakka_assets.c/h     # Flash-embedded zero-heap asset map
│   ├── yakka_state.c/h      # Core state machine & parent lock logic
│   └── main.c               # Hardware BSP coordinator & event loop
├── tools/
│   ├── view_image.py        # RGB565 previewer
│   ├── build_all_images.py  # Image asset pipeline
│   └── validate.sh          # Static checks, build, and test gate
├── tests/                   # Host unit tests
├── PRD.md                   # Product requirements & engineering specs
└── sdkconfig.defaults       # ESP-IDF board configuration baseline
```

---

## 📄 License

This project is licensed under the MIT License.
