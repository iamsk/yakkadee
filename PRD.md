<p align="right">
  <strong>English</strong> · <a href="PRD.zh_CN.md">简体中文</a>
</p>

# Yakka Dee Pocket Talker Product Requirements & Engineering Specification (PRD)

---

## 1. Document Information & Version Control

* **Project Name**: Yakka Dee Pocket Talker
* **Target Hardware Platform**: FoloToy AI Passport (ESP32-C3 single-core RISC-V 160MHz / 400KB SRAM / 8MB Flash)
* **Peripheral Specifications**:
  * **Display**: ST7789P3 240×320 SPI LCD
  * **Audio Codec**: ES8311 I2S DAC / ADC, onboard high-SNR electret mic and 1W cavity speaker
  * **Fuel Gauge**: CW2017 I2C hardware battery gauge
  * **Button Circuit**: 3 physical keys (UP / OK / DOWN) sharing a single ADC1_CH0 (GPIO0) resistor ladder
* **Core Runtime Environment**: **100% offline (zero network, zero latency, instant play, distraction-free)**
* **Target Users**: Toddlers around 2 years old (language explosion phase, intuitive cognition) and accompanying parents
* **Document Version**: v2.0 (Production Release)
* **Release Date**: 2026-11-06

---

## 2. Product Background & Core Value

### 2.1 Background
The BBC CBeebies animated series *Yakka Dee!* motivates toddlers to speak through rhythm, visual contrast, and the iconic catchphrase: *"Yakka Dee, Yakka Dee, talk, talk, talk! You say it!"*. However, mobile phones and tablets subject toddlers to passive screen consumption and vision strain.

### 2.2 Core Proposition
Utilizing the wearable form factor of the AI Passport, we build a dedicated physical interactive toy for 2-year-olds:
* **Zero-Button Operation**: The entire play session requires no button presses, flowing automatically;
* **Physical Acoustic Feedback**: Listens for the child's vocal energy via microphone, bursting confetti on the screen instantly;
* **Medical-Grade Eye Care**: Color palette, backlighting, and nap-time locks protect developing vision;
* **Anti-Touch Shield**: Physical buttons are muted during toddler play to prevent interruptions;
* **Canonical CBeebies Artwork**: Aligns faithfully with BBC official character model sheets for Dee.

---

## 3. Toddler Behavioral Profile (Age 2)

| Dimension | Typical 2-Year-Old Behavior | Product Response Design |
| :--- | :--- | :--- |
| **Cognition** | Cannot read words; relies on single large silhouettes; understands cause-and-effect (*"I shout ➔ fireworks burst"*). | Screen fills 80% with minimalist bold silhouettes; zero text reliance. |
| **Motor Control** | Immature fine motor skills; grips tight, pounds surfaces, or bites devices. | Normal play requires zero button presses; all keys are muted. |
| **Speech** | Babbling, unclear articulation; fragile resilience to error buzzers. | Uses physical RMS energy detection; any vocalization triggers success. |
| **Attention** | Short attention span (10–15 minutes); needs high-density positive reinforcement. | Fast 30–45s word loops; nap-time mode after 25 words or 15 mins. |

---

## 4. Core Features & Interaction Pipeline

```text
 ┌────────────────────────────────────────────────────────┐
 │                      Boot Intro                        │
 │     Audio: "Yakka Dee, Yakka Dee, talk, talk, talk!"   │
 └──────────────────────────┬─────────────────────────────┘
                            │ (Auto advance 2.0s)
                            ▼
 ┌────────────────────────────────────────────────────────┐
 │                   Phase 1: Vocabulary Intro            │
 │       Screen displays introductory card (e.g., DOG 🐶) │
 │       Audio: "Let's talk about... DOG!"                │
 └──────────────────────────┬─────────────────────────────┘
                            │ (Auto advance 1.5s)
                            ▼
 ┌────────────────────────────────────────────────────────┐
 │                 Phase 2: Contrast Triad                │
 │   1. "Big dog!" 🐶 (St. Bernard + deep bark)           │
 │   2. "Tiny dog!" 🐩 (Teacup Chihuahua + high yap)      │
 │   3. "Silly dog!" 🐕 (Bone on nose + goofy whistle)    │
 └──────────────────────────┬─────────────────────────────┘
                            │ (Auto advance 1.5s)
                            ▼
 ┌────────────────────────────────────────────────────────┐
 │               Phase 3: Prompt (You say it!)            │
 │       Dee leans in with ear listening prompt           │
 │       Audio: "Your turn! You say... DOG!"              │
 │                   Microphone listens                   │
 └──────────────────────────┬─────────────────────────────┘
                            │
            ┌───────────────┴───────────────┐
            │                               │
            ▼ (Toddler vocalizes)           ▼ (Silence 6s)
 ┌────────────────────────┐      ┌────────────────────────┐
 │  Phase 4: Celebration  │      │   Gentle Retry Prompt  │
 │ Confetti particles fall│      │ Soft animal sound cue  │
 │ Audio: "You did it!    │      │ Audio: "Say DOG! 🐶"   │
 │       Hooray! Bark!"   │      │ (Wait 4s; advance smoothly│
 └──────────┬─────────────┘      │  to next word if silent) │
            │                    └───────────┬────────────┘
            │                                │
            └────────────────┬───────────────┘
                             │ (Auto advance 2.0s)
                             ▼
 ┌────────────────────────────────────────────────────────┐
 │                 Phase 5: Next Word Loop                │
 │                 Cycles to next word (e.g., BANANA 🍌)  │
 └────────────────────────────────────────────────────────┘
```

### 4.1 Microphone Acoustic Energy Detection Algorithm
* **Audio Capture**: ES8311 continuously samples 16kHz 16-bit mono audio;
* **Energy Calculation**: Computes Root-Mean-Square (RMS) amplitude:
  $$\text{RMS} = \sqrt{\frac{1}{N}\sum_{i=1}^N x[i]^2}$$
* **Adaptive Noise Gating**: Dynamically tracks ambient background noise ($\text{RMS} \approx 800 \sim 1500$). When vocal energy exceeds threshold for $> 100\text{ms}$, success is awarded immediately.

---

## 5. Anti-Touch Grab Shield & Parent Mode

### 5.1 Hardware Circuit Constraints & Gesture Design
* **Hardware Design**:
  * AI Passport's three physical buttons (UP, DOWN, OK) connect to a single ADC1_CH0 (GPIO0) pin through a 10kΩ pull-up resistor ladder;
  * Voltage windows: UP ($0 \sim 150\text{mV}$), DOWN ($150 \sim 447\text{mV}$), OK ($447 \sim 1900\text{mV}$);
  * Pressing UP ($0\Omega$) and DOWN ($1\text{k}\Omega$) simultaneously yields an equivalent parallel resistance of $0\Omega$, which falls squarely into the UP button voltage window. Thus, dual-key combinations are electrically impossible.
* **Parent Gesture**:
  * To overcome this hardware constraint, parent mode is accessed by **holding the OK key for 2 seconds**.
  * Normal play mutes short presses, preventing interruptions from toddler grabbing.

### 5.2 Parent Menu Options
1. **Volume Level**: 25%, 50%, 75%, 100% (defaults to 100% with 0 dBFS peak normalized audio);
2. **Mic Sensitivity**: Low, Medium, High adaptive thresholds;
3. **Backlight Brightness**: 25%, 50%, 75% steps;
4. **Manual Word Selection**;
5. **Auto-Relock**: Exits and saves to NVS after 8 seconds of inactivity.

---

## 6. Eye-Care Optics & Nap-Time Mode

### 6.1 Optical Standards
1. **#F6F2E5 Warm Paper Background**: Protects against blue light glare and pupil strain;
2. **4px Bold Outlines**: Encourages a safe 40–50cm viewing distance;
3. **15kHz PWM Dimming**: High-frequency switching prevents stroboscopic ocular strain;
4. **Gentle Animations**: Particle confetti uses smooth gravity physics (25–30 fps) without screen strobing.

### 6.2 Nap-Time Mode ("Dee Takes a Nap")
* **Trigger Threshold**: 25 vocabulary loops or 15 minutes of inactivity;
* **Nap Routine**:
  1. Dee hugs her brown teddy bear and falls asleep (`Zzz...`);
  2. Gentle lullaby audio plays: *"Dee is sleepy... Time for a nap! Shh, see you later!"*;
  3. Screen dims to a soothing **10% soft nightlight**;
* **Parent Wake-Up**: Holding the OK key immediately wakes Dee without needing a full reboot.

---

## 7. Zero-Heap Embedded Architecture

### 7.1 Why RGB565 instead of JPG?
* **SRAM Limitations**: ESP32-C3 has only 400KB internal SRAM and lacks external PSRAM. A 200×200 JPEG requires 80KB of contiguous heap space to decode, causing out-of-memory crashes alongside LVGL and I2S audio tasks.
* **CPU Audio Glitch Prevention**: Software JPEG decoding consumes 40–80ms of CPU time on the 160MHz single-core RISC-V processor, starving I2S DMA audio buffers and causing speaker crackling.
* **Zero-Copy Flash Delivery**: 200×200 RGB565 Little-Endian data resides directly in memory-mapped 8MB Flash, consuming **0 bytes of dynamic RAM heap** and streaming over SPI DMA at zero latency.

### 7.2 Asset Bundle (5 Words, 22 Illustrations)
1. **Dee States (4 cards)**: `dee_welcome`, `dee_listen`, `dee_success`, `dee_sleep`;
2. **5 Word Triads (18 cards)**:
   * **DOG**: `dog_intro`, `dog_big`, `dog_tiny`, `dog_silly`;
   * **BANANA**: `banana_intro`, `banana_big`, `banana_long`, `banana_yellow`;
   * **CAR**: `car_intro`, `car_fast`, `car_red`, `car_beep`;
   * **DUCK**: `duck_intro`, `duck_swim`, `duck_quack`, `duck_fly`;
   * **APPLE**: `apple_intro`, `apple_big`, `apple_yummy`;
3. **Audio PCM (31 tracks)**: 16kHz 16-bit mono, 0 dBFS peak normalized for clear output.

---

## 8. Verification & Delivery Status

| Check Item | Method | Status |
| :--- | :--- | :--- |
| **Zero-Button Automation** | Automatic loop progression upon vocalization | **PASS** |
| **Mic Sensitivity** | Detection at 30–50cm distance | **PASS (RMS 1100~3500)** |
| **Audio Quality** | Default 100% volume, clear output | **PASS (0 dBFS normalized)** |
| **Parent Mode Lock** | Hold OK for 2s | **PASS** |
| **Nap-Time Mode** | 10% nightlight, instant wake on OK | **PASS** |
| **Authentic Artwork** | Generated via Nano Banana 2.1 | **PASS** |
| **Full Merged Image** | `build/FoloToy-AI-Passport-full.bin` (4.9MB, flash at 0x0) | **PASS** |
