<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Yakka Dee Pocket Talker Documentation Center

This repository contains the complete firmware implementation of the **Yakka Dee Pocket Talker** running on the **FoloToy AI Passport** (ESP32-C3 architecture).

For the full product overview and quick start guide, please refer to:
* [**README.md (Root Homepage)**](../README.md)
* [**PRD.md (Product Requirements & Engineering Specs)**](../PRD.md)

---

## Core System Specifications

* **Target Platform**: ESP32-C3 (160MHz Single-core RISC-V, 400KB SRAM, 8MB Flash)
* **Display Driver**: ST7789P3 SPI LCD (240×320), 200×200 RGB565 raw data via SPI DMA direct-to-screen
* **Audio Codec**: ES8311 I2S DAC/ADC, 16kHz 16-bit mono, real-time RMS vocal energy detection
* **Fuel Gauge**: CW2017 I2C hardware battery gauge
* **Buttons**: GPIO0 / ADC1_CH0 3-key resistor ladder, hold OK for 2s to unlock parent mode
* **Eye-Care Design**: #F6F2E5 warm paper background, 15kHz PWM stroboscopic-free dimming, 25-word/15-minute nap mode with 10% soft nightlight
* **Asset Art**: 100% authentic BBC CBeebies character design generated via OpenRouter Nano Banana 2.1

---

## Common Development & Verification Commands

```bash
# Static checks and host unit tests
./tools/validate.sh --static

# Build firmware and generate merged binary
./tools/validate.sh --firmware

# Full validation gate
./tools/validate.sh

# Preview raw RGB565 assets
python3 tools/view_image.py main/assets/dee_sleep.rgb565
python3 tools/view_image.py --all
```
