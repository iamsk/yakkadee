#!/usr/bin/env python3
# tools/generate_season1_images.py
# 为 Yakka Dee 第一季全套 20 词生成 CBeebies 风格暖色插画卡片 (200x200 JPEG)

import base64
import json
import os
import time
import urllib.request
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from PIL import Image

# 读取 OpenRouter API Key
env_lines = Path('.env').read_text().splitlines()
KEY = [l.split('=', 1)[1].strip() for l in env_lines if l.startswith('OPENROUTER_API_KEY=')][0]
MODEL = "google/gemini-nano-banana-2.1"
OUT_DIR = Path("main/assets")
OUT_DIR.mkdir(parents=True, exist_ok=True)

IMAGE_SPECS = [
    # 3. book
    ("book_intro", "A colorful closed story book with bright red and yellow cover, cute and cheerful children picture book, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("book_big", "A gigantic huge story book with a tiny bookmark, funny contrasting scale, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("book_open", "An open story book with magical pop-up stars and rainbow floating out, cheerful, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("book_story", "A cute picture book with funny smiling animal characters drawn on pages, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 4. boots
    ("boots_intro", "A pair of shiny bright red toddler rubber rain boots standing together, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("boots_red", "A pair of glowing scarlet red shiny boots, vibrant colors, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("boots_muddy", "A pair of funny muddy rain boots covered with mud splatters, playful, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("boots_shiny", "A pair of bright boots splashing in a crystal puddle of water with water drops, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 5. bike
    ("bike_intro", "A cute colorful toddler bicycle with training wheels and front basket, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bike_fast", "A super fast bicycle with colorful wind speed motion lines, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bike_little", "A tiny cute balance bike for toddler with smiling bell, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bike_ring", "A bicycle with a shiny golden bell and sound ring waves, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 7. cup
    ("cup_intro", "A cute ceramic tea cup with a happy smiling face, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("cup_blue", "A bright cheerful sky-blue polka-dot mug cup, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("cup_big", "An enormous giant cup next to a tiny teaspoon, funny scale contrast, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("cup_sippy", "A colorful toddler sippy cup with two handles and yellow lid, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 8. bus
    ("bus_intro", "A friendly red double-decker bus with smiling headlights, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bus_big", "A huge long passenger bus filling the view, cheerful, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bus_red", "A vibrant iconic bright red London bus, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bus_beep", "A cheerful bus honking its horn with musical notes 'beep beep', CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 9. peas
    ("peas_intro", "A green pea pod opened showing round smiling little green peas inside, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("peas_green", "Bright emerald green fresh pea pod, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("peas_tiny", "Cute tiny miniature baby peas rolling around playfully, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("peas_sweet", "Little happy peas popping happily out of a pod, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 10. hat
    ("hat_intro", "A cheerful colorful party hat with stripes and a pompom on top, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("hat_sun", "A wide brim yellow summer straw sun hat with pink ribbon, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("hat_silly", "A funny tall silly spiral clown hat with little jingle bells, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("hat_warm", "A cozy knitted wool winter beanie hat with a big fluffy bobble, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 11. worm
    ("worm_intro", "A cute pink cartoon earthworm with big happy eyes peeking out of garden soil, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("worm_wiggly", "A funny worm twisting in playful wiggly curvy waves, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("worm_long", "A super long cartoon worm stretched all the way across the card, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("worm_earth", "A cute friendly worm wearing little cool sunglasses, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 12. boat
    ("boat_intro", "A cheerful wooden toy sailboat with white and blue sails on gentle water, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("boat_sailing", "A sailboat gliding with wind filled sails, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("boat_speed", "A bright red motor speed boat splashing white foam waves, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("boat_float", "A cute folded origami paper boat floating in clear water, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 13. house
    ("house_intro", "A cozy little cottage house with red pitched roof, chimney, and round window, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("house_cozy", "A warm cottage with glowing yellow windows and cute chimney smoke puff, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("house_tall", "A funny whimsical tall narrow townhouse stretching upwards, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("house_brick", "A sturdy red brick country house with a blue wooden door, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 14. lion
    ("lion_intro", "An adorable friendly toddler lion cub with round fluffy mane and sweet smile, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("lion_roar", "A playful lion cub opening mouth wide pretending to roar, funny and cute, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("lion_big", "A big majestic friendly lion with magnificent fluffy round mane, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("lion_golden", "A sparkling golden lion shining bright with sunshine yellow fur, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 15. apple (补全第 4 张)
    ("apple_crunch", "A crisp bright red apple with a clean fresh bite taken out and playful crunch lines, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 16. ball
    ("ball_intro", "A colorful multi-stripe beach ball with yellow, red, and blue wedges, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("ball_bounce", "A bright bouncy ball bouncing high with cartoon spring bounce arcs, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("ball_round", "A classic black and white patterned soccer ball perfectly round, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("ball_roll", "A shiny red bowling ball rolling fast leaving speed motion lines, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 17. mouse
    ("mouse_intro", "A tiny cute grey mouse holding a wedge of Swiss cheese with both paws, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("mouse_little", "An ultra tiny miniature baby mouse sitting comfortably on a leaf, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("mouse_squeak", "A cheerful mouse singing with mouth open and cute musical squeak notes, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("mouse_quiet", "A quiet little mouse curled up sleeping under a soft leaf blanket, peaceful, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 18. beans
    ("beans_intro", "A small ceramic bowl filled with colorful smiling magic beans, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("beans_magic", "Glow-in-the-dark magical fairy tale beans with sparkly stars around them, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("beans_green", "A bunch of fresh crisp long green string beans, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("beans_jumping", "Funny lively jumping beans leaping happily in the air with motion arcs, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),

    # 20. bed
    ("bed_intro", "A cozy wooden toddler bed with soft blue quilt and fluffy white pillow, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bed_soft", "A super puffy cloud-like bed with thick feather duvet and cushions, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bed_sleepy", "A cozy bed with a gentle crescent moon and smiling sleepy stars above, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
    ("bed_cozy", "A warm snuggly bed tucked under a colorful patchwork quilt, CBeebies Yakka Dee 2D flat cartoon style, warm cream background (#FFF9EE), centered"),
]

def generate_one_image(item):
    name, prompt = item
    out_jpg = OUT_DIR / f"{name}.jpg"
    if out_jpg.exists() and out_jpg.stat().st_size > 2000:
        print(f"Skipping {name}.jpg (already exists, {out_jpg.stat().st_size} bytes)")
        return True

    payload = {
        'model': MODEL,
        'messages': [{'role': 'user', 'content': prompt}]
    }

    for attempt in range(3):
        try:
            req = urllib.request.Request(
                'https://openrouter.ai/api/v1/chat/completions',
                headers={'Authorization': f'Bearer {KEY}', 'Content-Type': 'application/json'},
                data=json.dumps(payload).encode('utf-8')
            )
            with urllib.request.urlopen(req, timeout=90) as resp:
                data = json.loads(resp.read().decode('utf-8'))
                imgs = data['choices'][0]['message'].get('images', [])
                if imgs:
                    url = imgs[0]['image_url']['url']
                    header, b64 = url.split(',', 1)
                    raw_bytes = base64.b64decode(b64)
                    tmp_img = Path(f"/tmp/{name}_raw.png")
                    tmp_img.write_bytes(raw_bytes)

                    im = Image.open(str(tmp_img))
                    w, h = im.size
                    crop_sz = min(w, h)
                    left = (w - crop_sz) // 2
                    top = (h - crop_sz) // 2
                    im_sq = im.crop((left, top, left + crop_sz, top + crop_sz)).resize((200, 200), Image.Resampling.LANCZOS)
                    im_sq.save(str(out_jpg), 'JPEG', quality=85)
                    print(f"Successfully generated {name}.jpg ({out_jpg.stat().st_size} bytes)")
                    tmp_img.unlink(missing_ok=True)
                    return True
                else:
                    print(f"No image in response for {name}, attempt {attempt+1}")
                    time.sleep(2)
        except Exception as e:
            print(f"Error generating {name} (attempt {attempt+1}): {e}")
            time.sleep(3)

    return False

def main():
    print(f"Total image cards to generate: {len(IMAGE_SPECS)}")
    # 使用 4 线程并发加速
    with ThreadPoolExecutor(max_workers=4) as executor:
        futures = {executor.submit(generate_one_image, item): item[0] for item in IMAGE_SPECS}
        for future in as_completed(futures):
            name = futures[future]
            try:
                success = future.result()
                if not success:
                    print(f"FAILED: {name}")
            except Exception as e:
                print(f"EXCEPTION for {name}: {e}")

    print("Season 1 images generation complete!")

if __name__ == "__main__":
    main()
