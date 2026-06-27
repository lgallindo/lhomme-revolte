from PIL import Image
p = Image.open('assets/palette_adjusted.gif').convert('RGB')
print(p.getdata()[175])
