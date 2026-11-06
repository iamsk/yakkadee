#!/usr/bin/env python3
"""
Generate official CBeebies 'Yakka Dee!' style images using OpenRouter API
(google/gemini-nano-banana-2.1) and compile them directly to 200x200 RGB565 assets.
"""
import os
import sys
import json
import base64
import struct
import urllib.request
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
ASSETS_DIR = ROOT / "main" / "assets"
PNG_DIR = Path("/tmp/yakka_pngs")
PNG_DIR.mkdir(parents=True, exist_ok=True)

ENV_FILE = ROOT / ".env"
API_KEY = ""
if ENV_FILE.is_file():
    for line in ENV_FILE.read_text().splitlines():
        if line.startswith("OPENROUTER_API_KEY="):
            API_KEY = line.split("=", 1)[1].strip()

if not API_KEY:
    print("Error: OPENROUTER_API_KEY not found in .env", file=sys.stderr)
    sys.exit(1)

MODEL = "google/gemini-nano-banana-2.1"

# The 10 images that need to be generated / updated
TASKS = [
    {
        "name": "dog_tiny",
        "file": "dog_tiny.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. An adorable tiny Chihuahua puppy sitting inside a cute colorful tea cup, with huge cartoon eyes and big perky ears, looking super cute. Bold black outlines, flat solid colors, minimalist vector aesthetic, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "dog_silly",
        "file": "dog_silly.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A silly goofy cartoon dog balancing a large white bone on its nose, with funny crossed eyes and tongue sticking out sideways. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "banana_yellow",
        "file": "banana_yellow.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A playful Banana Phone - a bright yellow curved banana shaped like a retro telephone receiver ringing with sound waves ('Ring Ring!'), with a cute smiling face. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "duck_intro",
        "file": "duck_intro.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A cute, chubby bright yellow duck with an orange bill, friendly round black eye, and tiny wings. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "duck_swim",
        "file": "duck_swim.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A cute yellow duck swimming happily wearing blue swim goggles, paddling in blue water ripples and splashes. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "duck_quack",
        "file": "duck_quack.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A quacking yellow duck with its orange bill wide open shouting 'QUACK!', musical notes and cartoon sound waves floating around. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "duck_fly",
        "file": "duck_fly.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A happy yellow duck flying with wings spread wide, gliding happily with fluffy white clouds, joyful smile. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "apple_intro",
        "file": "apple_intro.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A plump bright red shiny apple with a green leaf and brown stem, with a happy smiling cartoon face. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "apple_big",
        "file": "apple_big.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A big giant red cartoon apple taking up the frame, with a friendly cute green caterpillar popping its head out and waving with a smile. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    },
    {
        "name": "apple_yummy",
        "file": "apple_yummy.rgb565",
        "prompt": "Official 2D cartoon illustration in CBeebies 'Yakka Dee!' animated series style by Chris Allen. A delicious yummy red apple with a clean crunchy bite taken out showing creamy fruit and tiny seed, with happy sparkly eyes. Bold black outlines, flat solid colors, on a solid warm cream background (#F6F2E5)."
    }
]

def rgb565_le(im: Image.Image) -> bytes:
    out = bytearray()
    for r, g, b in im.convert("RGB").getdata():
        pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
        out.extend(struct.pack("<H", pixel))
    return bytes(out)

def process_and_save(img: Image.Image, name: str, file_name: str):
    # Center crop to square
    w, h = img.size
    crop_size = min(w, h)
    left = (w - crop_size) // 2
    top = (h - crop_size) // 2
    cropped = img.crop((left, top, left + crop_size, top + crop_size))
    im_200 = cropped.resize((200, 200), Image.Resampling.LANCZOS)
    
    # Save PNG
    png_path = PNG_DIR / f"{name}.png"
    im_200.save(png_path)
    
    # Save RGB565
    raw = rgb565_le(im_200)
    out_file = ASSETS_DIR / file_name
    out_file.write_bytes(raw)
    print(f"[{name}] Saved RGB565 ({len(raw)} bytes) -> {out_file.name}, PNG -> {png_path}")

def generate_one(task):
    name = task["name"]
    file_name = task["file"]
    prompt = task["prompt"]
    print(f"Generating [{name}] via {MODEL}...")

    payload = {
        "model": MODEL,
        "messages": [
            {"role": "user", "content": prompt}
        ]
    }
    req = urllib.request.Request(
        "https://openrouter.ai/api/v1/chat/completions",
        headers={
            "Authorization": f"Bearer {API_KEY}",
            "Content-Type": "application/json"
        },
        data=json.dumps(payload).encode("utf-8")
    )

    with urllib.request.urlopen(req, timeout=90) as resp:
        data = json.loads(resp.read().decode("utf-8"))
        msg = data["choices"][0]["message"]
        images = msg.get("images", [])
        if not images:
            raise RuntimeError(f"No image returned in response: {msg}")
        
        img_info = images[0]
        url = img_info.get("image_url", {}).get("url", "")
        if not url.startswith("data:image/"):
            raise RuntimeError(f"Unexpected image format: {url[:50]}")
        
        b64 = url.split(",", 1)[1]
        raw_bytes = base64.b64decode(b64)
        
        from io import BytesIO
        img = Image.open(BytesIO(raw_bytes))
        process_and_save(img, name, file_name)

def main():
    success_count = 0
    for i, t in enumerate(TASKS):
        print(f"\n--- Progress ({i+1}/{len(TASKS)}) ---")
        try:
            generate_one(t)
            success_count += 1
        except Exception as e:
            print(f"Error generating {t['name']}: {e}", file=sys.stderr)
    
    print(f"\nCompleted {success_count}/{len(TASKS)} image generations.")

if __name__ == "__main__":
    main()
