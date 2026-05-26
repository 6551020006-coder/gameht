"""
Ghep 3 anh thien ha thanh 1 hinh nen cao (1920x4860).
Co chuyen tiep mo giua tat ca cac doan, bao gom ca top va bottom de tao thanh
1 vong lap cuon tron (seamless vertical loop) hoan hao, khong vet cat.
"""
from PIL import Image
import os

SCREEN_W = 1920
IMG_H = 1920
BLEND_H = 300
H_STEP = IMG_H - BLEND_H  # 1620
TOTAL_H = H_STEP * 3      # 4860

TOP_PATH  = r"C:\Users\canht\.gemini\antigravity\brain\d9fdb74e-0b84-4cfb-9378-647c63a50be9\galaxy_top_1778568176463.png"
MID_PATH  = r"C:\Users\canht\.gemini\antigravity\brain\d9fdb74e-0b84-4cfb-9378-647c63a50be9\galaxy_mid_1778568204134.png"
BOT_PATH  = r"C:\Users\canht\.gemini\antigravity\brain\d9fdb74e-0b84-4cfb-9378-647c63a50be9\galaxy_bot_1778568219066.png"
OUT_PATH  = r"c:\Gametauvutru (1)\Graphics\galaxy_seamless.png"

def load_resize(path, w, h):
    img = Image.open(path).convert("RGBA")
    return img.resize((w, h), Image.LANCZOS)

print("Dang tai va resize cac doan anh...")
img_a = load_resize(TOP_PATH, SCREEN_W, IMG_H)
img_b = load_resize(MID_PATH, SCREEN_W, IMG_H)
img_c = load_resize(BOT_PATH, SCREEN_W, IMG_H)

print(f"Tao canvas {SCREEN_W}x{TOTAL_H}...")
canvas = Image.new("RGBA", (SCREEN_W, TOTAL_H), (0, 0, 0, 255))

# 1. Dan cac phan khong mix (non-overlapping)
canvas.paste(img_a.crop((0, BLEND_H, SCREEN_W, H_STEP)), (0, BLEND_H))
canvas.paste(img_b.crop((0, BLEND_H, SCREEN_W, H_STEP)), (0, H_STEP + BLEND_H))
canvas.paste(img_c.crop((0, BLEND_H, SCREEN_W, H_STEP)), (0, 2 * H_STEP + BLEND_H))

print("Dang xu ly cac vung chuyen tiep (blending seams)...")

# 2. Blend Seam 1: A -> B
for y in range(BLEND_H):
    alpha = y / float(BLEND_H)
    row_a = img_a.crop((0, H_STEP + y, SCREEN_W, H_STEP + y + 1))
    row_b = img_b.crop((0, y, SCREEN_W, y + 1))
    blended = Image.blend(row_a, row_b, alpha)
    canvas.paste(blended, (0, H_STEP + y))

# 3. Blend Seam 2: B -> C
for y in range(BLEND_H):
    alpha = y / float(BLEND_H)
    row_b = img_b.crop((0, H_STEP + y, SCREEN_W, H_STEP + y + 1))
    row_c = img_c.crop((0, y, SCREEN_W, y + 1))
    blended = Image.blend(row_b, row_c, alpha)
    canvas.paste(blended, (0, 2 * H_STEP + y))

# 4. Blend Seam 3 (Wrap-around): C -> A
for y in range(BLEND_H):
    alpha = y / float(BLEND_H)
    row_c = img_c.crop((0, H_STEP + y, SCREEN_W, H_STEP + y + 1))
    row_a = img_a.crop((0, y, SCREEN_W, y + 1))
    blended = Image.blend(row_c, row_a, alpha)
    canvas.paste(blended, (0, y))

final = canvas.convert("RGB")
print(f"Dang luu {OUT_PATH} ...")
final.save(OUT_PATH, "PNG", optimize=False)

size_mb = os.path.getsize(OUT_PATH) / (1024 * 1024)
print(f"Hoan tat! Kich thuoc anh: {SCREEN_W}x{TOTAL_H} px ({size_mb:.1f} MB)")
