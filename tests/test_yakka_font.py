#!/usr/bin/env python3
# tests/test_yakka_font.py
# 验证中文字体库覆盖了 Yakka Dee 家长模式及界面的所有中文字形
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FONT_C = (ROOT / "main/lv_font_cn_16.c").read_text(encoding="utf-8")

class YakkaFontCoverageTest(unittest.TestCase):
    def test_chinese_font_glyph_coverage(self):
        match = re.search(r'unicode_list_cjk_0\[\]\s*=\s*\{([^}]+)\}', FONT_C)
        self.assertIsNotNone(match, "必须包含 CJK 编码映射表")
        raw = match.group(1)
        offsets = [int(x.strip(), 0) for x in raw.split(',') if x.strip()]
        # LV_FONT_FMT_TXT_CMAP_SPARSE_TINY 的码位计算公式: codepoint = range_start (183) + offset
        cjk_chars = set(chr(183 + offset) for offset in offsets)

        # 检查家长菜单与系统提示的所有汉字
        required_chars = (
            "家长模式"
            "主音量"
            "低中高"
            "背光亮度"
            "标准护眼明亮"
            "麦克风灵敏度"
            "词汇循环"
            "全部"
            "小狗香蕉小鸭小汽车大苹果"
            "睡午觉"
            "退出"
            "电量"
            "按住上下键解锁"
            "长按确认键"
        )
        missing = [c for c in required_chars if c not in cjk_chars]
        self.assertEqual(len(missing), 0, f"缺失字形: {missing}")

if __name__ == "__main__":
    unittest.main()
