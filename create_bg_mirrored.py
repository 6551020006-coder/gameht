"""
Tao hinh nen seamless bang cach ghep doi xung (mirroring) giong nhu hinh mau cua user.
"""
from PIL import Image
import os

SCREEN_W = 1000
IMG_H = 1000
# 4 doan = 4000 pixel chieu cao
TOTAL_H = IMG_H * 4

BASE_PATH = r"C:\Users\canht\.gemini\antigravity\brain\d9fdb74e-0b84-4cfb-9378-647c63a50be9\base_space_1778568824028.png"
OUT_PATH  = r"c:\Gametauvutru (1)\Graphics\galaxy_seamless.png"

def load_resize(path, w, h):
    img = Image.open(path).convert("RGBA")
    return img.resize((w, h), Image.LANCZOS)

print("Dang tai anh co so...")
img_normal = load_resize(BASE_PATH, SCREEN_W, IMG_H)

# Tao ban sao lat nguoc (flip vertically)
img_flipped = img_normal.transpose(Image.FLIP_TOP_BOTTOM)

print(f"Tao canvas {SCREEN_W}x{TOTAL_H}...")
canvas = Image.new("RGBA", (SCREEN_W, TOTAL_H), (0, 0, 0, 255))

# Ghep xen ke de tao loop hoan hao khong vet cat
# Doan 1: Binh thuong
canvas.paste(img_normal, (0, 0))
# Doan 2: Lat nguoc (khop hoan hao voi day doan 1)
canvas.paste(img_flipped, (0, IMG_H))
# Doan 3: Binh thuong (khop hoan hao voi day doan 2 lat nguoc)
canvas.paste(img_normal, (0, IMG_H * 2))
# Doan 4: Lat nguoc (khop hoan hao voi day doan 3, va day cua doan 4 cung se khop hoan hao voi dinh doan 1!)
canvas.paste(img_flipped, (0, IMG_H * 3))

final = canvas.convert("RGB")
print(f"Dang luu {OUT_PATH} ...")
final.save(OUT_PATH, "PNG", optimize=False)

size_mb = os.path.getsize(OUT_PATH) / (1024 * 1024)
print(f"Hoan tat! Kich thuoc anh: {SCREEN_W}x{TOTAL_H} px ({size_mb:.1f} MB)")
