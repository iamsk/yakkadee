<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# Yakka Dee 口袋开口机 (Yakka Dee Pocket Talker)

专为 **2 岁语言爆发期幼儿** 定制的高密度正反馈开口引导实体玩具。基于 **FoloToy AI Passport**（ESP32-C3 硬件平台）深度自研，**100% 纯离线运行（零网络、零延迟、开机即玩）**。

> **“Yakka Dee, Yakka Dee, talk, talk, talk! You say it!”**  
> 告别手机/平板被动灌输与用眼伤害，让宝宝在快乐大喊中自信开口！

---

## 🌟 核心特色与专有设计

### 1. 全程免按键交互（Zero-Button Automated Flow）
* **2 岁幼儿认知适配**：宝宝无需理解复杂的菜单或按键，开机后自动播放引导并循环推进。
* **物理声音能量侦听**：内置 ES8311 I2S 麦克风持续计算短时能量振幅（RMS）。只要宝宝大声发音、模仿叫声或兴奋大叫，**100ms 内瞬间判定成功**！
* **慢速彩带正反馈**：发音成功后屏幕立即绽放慢速粒子彩带（Confetti）并伴随全家欢呼声，建立“我开口 ➔ 机器放烟花”的强烈因果因果正反馈。
* **静默温柔鼓励**：若幼儿害羞未发声，系统播放温柔动物叫声提示重试；超时后平滑进入下一趣味词汇，**绝不出现任何挫败感音效**。

### 2. 医疗级光学护眼与防沉迷
* **#F6F2E5 暖纸米黄底色**：全系统严格杜绝纯白高能眩光与纯黑强反差刺眼背光，呈现纸质绘本般的温和反光感。
* **高识别 4px 粗轮廓**：200×200 主物体面积占据视窗 80%，在 40~50cm 正常视距下一目了然，从根本上杜绝“凑近贴屏看”。
* **15kHz 高频无频闪 PWM 调光**：背光调光频率远超普通消费级屏幕，消除眼肌隐性疲劳。
* **“Dee 睡午觉啦”拟人化防沉迷**：连续交互达到 25 组单词或无操作 15 分钟后，Dee 换上睡衣抱泰迪熊甜甜入睡，背光柔和降至 **10% 微光夜灯**。家长或孩子可随时**长按 OK 键**即刻唤醒，避免突兀黑屏引发幼儿情绪对抗。

### 3. 防抓握误触与家长管理模式
* **按键全屏蔽**：正常玩耍期间物理屏蔽短按、狂拍等所有按键操作，无视啃咬乱按。
* **长按 OK 键 2 秒解锁**：根据硬件 ADC 分压梯电路特性，定制长按 OK 唤醒手势，方便家长调节：
  * **主音量档位**（25% / 50% / 75% / 100% 出厂默认满电平，PCM 经过 0 dBFS 峰值归一化，声音清脆响亮）；
  * **麦克风灵敏度**（低 / 中 / 高 三档动态自适应）；
  * **背光亮度**（25% / 50% / 75% 档位）；
  * **手动选词测试**。

### 4. BBC CBeebies 官方原版动画美学
* 100% 还原 Chris Allen 原版角色造型：南亚裔小女孩 Dee、整齐齐刘海、旋转双马尾（红发圈）、亮天蓝黄色闪电 ⚡ T 恤、红短裤。
* 全套 22 张插画由 **Nano Banana 2.1** 深度对齐生成，涵盖 5 组启蒙词汇（DOG、BANANA、CAR、DUCK、APPLE）的反差场景。

---

## 🛠️ 硬件与系统架构

| 模块 | 规格参数 | 工程优化决策 |
| :--- | :--- | :--- |
| **主控芯片** | ESP32-C3 (单核 RISC-V @ 160MHz) | 单核无外置 PSRAM，极致轻量化与低功耗 |
| **内存/存储** | 400KB SRAM / 8MB SPI Flash | 以 Flash 空间换 CPU 算力，0-Heap 零拷贝直显 |
| **显示屏幕** | 240×320 ST7789P3 SPI LCD | 200×200 RGB565 小端序裸数据经 SPI DMA 直通显示 |
| **音频外设** | ES8311 I2S Codec / 腔体扬声器 / 驻极体麦克风 | 16kHz 16-bit 异步音频管道，支持短时 RMS 能量检测 |
| **电量管理** | CW2017 I2C 硬件电量芯片 | 软硬件结合低功耗与电量采样 |
| **物理按键** | 单路 ADC1_CH0 (GPIO0) 阻抗分压梯 | 长按 OK 键 2 秒抗误触单键手势 |

---

## 🚀 快速上手与固件烧录

### 1. 烧录预编译合并固件（一键 0x0 刷机）

通过 USB 数据线将 AI Passport 连接至电脑（长按电源键开机）：

```bash
# 查看连接的串口（macOS 示例）
ls /dev/cu.usbmodem*

# 从 0x0 地址直接刷入合并固件（包含 bootloader、分区表与 factory app）
esptool.py -p /dev/cu.usbmodem1101 -b 460800 write_flash 0x0 build/FoloToy-AI-Passport-full.bin
```

### 2. 预览与检查图像素材

项目中提供了一键解码 raw RGB565 素材并转为 PNG 打开的实用脚本：

```bash
# 查看单张素材
python3 tools/view_image.py main/assets/dee_sleep.rgb565

# 一键导出全部 22 张素材并在访达中连续预览
python3 tools/view_image.py --all
```

### 3. 本地构建与验证

```bash
# 1. 运行代码检查与 Host 单元测试（无需连接开发板）
./tools/validate.sh --static

# 2. 编译固件并生成合并二进制镜像
./tools/validate.sh --firmware

# 3. 运行完整门禁验证
./tools/validate.sh
```

---

## 📁 目录结构

```text
├── main/
│   ├── assets/              # 22 张 200x200 RGB565 裸数据与 31 个 PCM 音频文件
│   ├── audio_energy.c/h     # I2S 麦克风音频能量（RMS）检测与动态底噪自适应
│   ├── audio_player.c/h     # 异步 I2S 音频播放器（DMA 缓冲驱动）
│   ├── confetti.c/h         # 慢速粒子彩带下落物理引擎
│   ├── lv_font_cn_16.c/h    # 精准覆盖的 16px 中文字库
│   ├── ui_yakka.c/h         # Yakka Dee 护眼 UI 与动画管理
│   ├── yakka_assets.c/h     # Flash 静态内嵌资源映射表（0 堆内存消耗）
│   ├── yakka_state.c/h      # 开口引导主状态机与防误触家长逻辑
│   └── main.c               # 硬件 BSP 调度与主事件循环
├── tools/
│   ├── view_image.py        # RGB565 查看器（Mac 预览 / QuickLook）
│   ├── build_all_images.py  # 图像资产打包流水线
│   └── validate.sh          # 静态检查、固件构建与门禁脚本
├── tests/                   # 宿主机单元测试（状态机、能量检测、彩带粒子等）
├── PRD.md                   # 详细产品需求与工程技术规格说明书
└── sdkconfig.defaults       # ESP-IDF 硬件配置基线
```

---

## 📄 开源许可证

本项目基于 MIT License 开源。
