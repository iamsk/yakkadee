<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# Yakka Dee 口袋开口机 文档中心

本项目是运行在 **FoloToy AI Passport**（ESP32-C3 架构）上的 **Yakka Dee 口袋开口机 (Yakka Dee Pocket Talker)** 完整产品实现。

项目完整产品介绍与快速上手，请参阅根目录文档：
* [**README.md (根目录主页)**](../README.zh_CN.md)
* [**PRD.md (详细产品需求与工程规格)**](../PRD.md)

---

## 核心系统规格

* **目标平台**：ESP32-C3 (160MHz 单核 RISC-V, 400KB SRAM, 8MB Flash)
* **显示驱动**：ST7789P3 SPI LCD (240×320)，200×200 RGB565 裸数据 MMU 零拷贝送显
* **音频编解码**：ES8311 I2S DAC/ADC，16kHz 16-bit 单声道，动态 RMS 声音能量检测
* **电量芯片**：CW2017 I2C 硬件电量监测
* **按键配置**：GPIO0 / ADC1_CH0 3 键电阻分压，长按 OK 键 2 秒解锁家长模式
* **护眼模式**：#F6F2E5 暖纸米黄护眼底色，15kHz PWM 无频闪调光，25 词/15 分钟“Dee 睡午觉”微光夜灯模式
* **插画生成**：基于 OpenRouter Nano Banana 2.1 高清引擎 100% 对齐 CBeebies 官方原版原画

---

## 常用开发与测试命令

```bash
# 静态代码检查与宿主机单元测试
./tools/validate.sh --static

# 固件编译构建与合并
./tools/validate.sh --firmware

# 完整门禁检查
./tools/validate.sh

# RGB565 图片预览
python3 tools/view_image.py main/assets/dee_sleep.rgb565
python3 tools/view_image.py --all
```
