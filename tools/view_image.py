#!/usr/bin/env python3
"""
Convenience script to decode and preview raw RGB565 images.

Usage:
    python3 tools/view_image.py main/assets/dee_sleep.rgb565
    python3 tools/view_image.py --all
"""
import sys
import argparse
import subprocess
from pathlib import Path
from PIL import Image
import numpy as np

DEFAULT_WIDTH = 200
DEFAULT_HEIGHT = 200

def rgb565_to_png(rgb565_path: Path, width: int = DEFAULT_WIDTH, height: int = DEFAULT_HEIGHT, out_path: Path = None) -> Path:
    if out_path is None:
        out_path = Path(f"/tmp/{rgb565_path.stem}.png")
    
    raw = np.fromfile(rgb565_path, dtype="<u2")
    expected_pixels = width * height
    if len(raw) != expected_pixels:
        # Check if square or known ratio
        total_pixels = len(raw)
        side = int(total_pixels ** 0.5)
        if side * side == total_pixels:
            width, height = side, side
        else:
            raise ValueError(f"File size {len(raw)*2} bytes does not match {width}x{height} (expected {expected_pixels*2} bytes)")

    r = (((raw >> 11) & 0x1F) * 255 // 31).astype("uint8")
    g = (((raw >> 5) & 0x3F) * 255 // 63).astype("uint8")
    b = ((raw & 0x1F) * 255 // 31).astype("uint8")
    rgb = np.stack([r, g, b], axis=-1).reshape(height, width, 3)

    img = Image.fromarray(rgb)
    img.save(out_path)
    return out_path

def main():
    parser = argparse.ArgumentParser(description="Decode and preview RGB565 raw image files")
    parser.add_argument("file", nargs="?", help="Path to .rgb565 file")
    parser.add_argument("--all", action="store_true", help="Convert all main/assets/*.rgb565 to PNG in /tmp/yakka_pngs")
    parser.add_argument("--width", type=int, default=DEFAULT_WIDTH, help="Image width (default: 200)")
    parser.add_argument("--height", type=int, default=DEFAULT_HEIGHT, help="Image height (default: 200)")
    parser.add_argument("--no-open", action="store_true", help="Do not open preview automatically")

    args = parser.parse_args()

    if args.all:
        out_dir = Path("/tmp/yakka_pngs")
        out_dir.mkdir(parents=True, exist_ok=True)
        assets_dir = Path(__file__).resolve().parents[1] / "main" / "assets"
        files = sorted(assets_dir.glob("*.rgb565"))
        print(f"Converting {len(files)} RGB565 assets to {out_dir}...")
        for f in files:
            p = rgb565_to_png(f, args.width, args.height, out_dir / f"{f.stem}.png")
            print(f"  -> {p.name}")
        if not args.no_open and sys.platform == "darwin":
            subprocess.run(["open", str(out_dir)])
        return

    if not args.file:
        parser.print_help()
        sys.exit(1)

    path = Path(args.file)
    if not path.is_file():
        print(f"Error: file not found: {path}", file=sys.stderr)
        sys.exit(1)

    out = rgb565_to_png(path, args.width, args.height)
    print(f"Exported to: {out}")
    if not args.no_open and sys.platform == "darwin":
        subprocess.run(["open", str(out)])

if __name__ == "__main__":
    main()
