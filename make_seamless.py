from PIL import Image
import os

IN_PATH  = r"C:\Gametauvutru (1)\Graphics\galaxy_seamless_fullhd.png"
OUT_PATH = r"C:\Gametauvutru (1)\Graphics\galaxy_seamless.png"
BLEND_H = 300

print("Dang tai anh...")
img = Image.open(IN_PATH).convert("RGBA")
W, H = img.size

if H <= BLEND_H:
    BLEND_H = H // 4

H_NEW = H - BLEND_H

canvas = Image.new("RGBA", (W, H_NEW), (0, 0, 0, 255))

# 1. Dan phan tu 0 den H_NEW vao canvas
canvas.paste(img.crop((0, 0, W, H_NEW)), (0, 0))

# 2. Hoa tron (blend) phan day cua anh goc (tu H_NEW den H) vao phan dinh cua canvas
for y in range(BLEND_H):
    alpha = 1.0 - (y / float(BLEND_H))
    row_top = canvas.crop((0, y, W, y + 1))
    row_bottom = img.crop((0, H_NEW + y, W, H_NEW + y + 1))
    blended = Image.blend(row_top, row_bottom, alpha)
    canvas.paste(blended, (0, y))

final = canvas.convert("RGB")
final.save(OUT_PATH, "PNG", optimize=False)
print("Hoan tat lam mo vet cat! Hinh nen da duoc lam muot hoan toan.")
