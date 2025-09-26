from PIL import Image

def crop_image(img, x, y, width, height):
    box = (x, y, x + width, y + height)
    return img.crop(box)

def resize_image(img, width, height):
    size = (width, height)
    return img.resize(size)

base_image = 'Gemini_Generated_Image_nrp240nrp240nrp2.png'
img = Image.open(base_image);
print(f"{img.size}")

img = crop_image(img, 0, 216, 928, 522)
print(f"{img.size}")

#img = resize_image(img, 800, 450)
#print(f"{img.size}")

img.save("shuubun.jpg")
