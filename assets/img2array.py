# Python tool to convert an image to C array for small3dlib.
#
# by drummerfish
# released under CC0 1.0.

import sys
from PIL import Image

def printHelp():
  print("Convert image to C array for small3dlib.")
  print("usage:\n")
  print("  python img2array.py [-xW -yH -h -nS -pT -5] file\n")
  print("  -xW     set width of the output to W pixels")
  print("  -yH     set height of the output to H pixels")
  print("  -h      include header guards (for texture per file)")
  print("  -nS     use the name S for the texture (defaut: \"texture\")")
  print("  -pT     use palette from file T and indexed colors (otherwise direct colors)")
  print("  -5      use 565 format instead of RGB8")
  print("  -c      compress (4 bpp, 16 color palette), only with -pT")
  print("  -t      transpose (store by columns)")
  print("");
  print("Original Engine")
  print("released under CC0 1.0")

def rgbTo565(rgb):
  return ((rgb[0] >> 3) << 11) | ((rgb[1] >> 2) << 5) | ((rgb[2] >> 3))

if len(sys.argv) < 2:
  printHelp()
  quit()

FILENAME = ""
PALETTE = ""
USE_PALETTE = False
NAME = "texture"
GUARDS = False
OUT_WIDTH = 64
OUT_HEIGHT = 64
USE_565 = False
TRANSPOSE = True
COMPRESS = False

for s in sys.argv:
  if s [:2] == "-x":
    OUT_WIDTH = int(s[2:])
  elif s [:2] == "-y":
    OUT_HEIGHT = int(s[2:])
  elif s == "-h":
    GUARDS = True
  elif s[:2] == "-n":
    NAME = s[2:]
  elif s[:2] == "-5":
    USE_565 = True
  elif s[:2] == "-p":
    PALETTE = s[2:]
    USE_PALETTE = True
  elif s[:2] == "-t":
    TRANSPOSE = True
  elif s[:2] == "-c":
    COMPRESS = True
  else:
    FILENAME = s

if not USE_PALETTE:
  COMPRESS = False

imageArray = []
paletteColors = []
paletteArray = []
image = Image.open(FILENAME).convert("RGBA")
if image.size != (OUT_WIDTH, OUT_HEIGHT):
  print(f"FATAL ERROR: Input image dimensions ({image.size[0]}x{image.size[1]}) do not match expected output dimensions ({OUT_WIDTH}x{OUT_HEIGHT}).")
  sys.exit(1)
pixels = image.load()

if USE_PALETTE > 0:
  palette = Image.open(PALETTE).convert("RGB")
  pixelsPal = palette.load()

  for y in range(palette.size[1]):
    for x in range(palette.size[0]):
      c = pixelsPal[x,y]
      paletteColors.append(c)

      if USE_565:
        paletteArray.append(rgbTo565(c))
      else:
        paletteArray.append(c[0])
        paletteArray.append(c[1])
        paletteArray.append(c[2])

image2 = Image.new("RGB",(OUT_WIDTH,OUT_HEIGHT),color="white")
pixels2 = image2.load()

def findClosestColor(pixel,paletteColors):
  closestIndex = 0     
  closestDiff = 1024

  # find the index of the closest color:

  for i in range(len(paletteColors)):
    c = paletteColors[i]
    diff = abs(pixel[0] - c[0]) + abs(pixel[1] - c[1]) + abs(pixel[2] - c[2])

    if diff < closestDiff:
      closestIndex = i
      closestDiff = diff

  return closestIndex

for y in range(OUT_HEIGHT):
  for x in range(OUT_WIDTH):
    x2 = y if TRANSPOSE else x
    y2 = x if TRANSPOSE else y

    pixel = pixels[x2,y2]

    if USE_PALETTE:
      if pixel[3] < 128:
        closestIndex = 175
      else:
        closestIndex = findClosestColor(pixel[:3],paletteColors)     
      imageArray.append(closestIndex)
      pixels2[x,y] = paletteColors[closestIndex] if closestIndex < len(paletteColors) else (255, 4, 5)
    else:
      if USE_565:
        imageArray.append(rgbTo565(pixel))
      else:
        imageArray.append(pixel[0])
        imageArray.append(pixel[1])
        imageArray.append(pixel[2])

      pixels2[x,y] = pixel[:3]

#-----------------------

def compressImageArray(a):
  result = []

  reducedPalette = []

  histogram = [0 for i in range(256)]

  for c in a:
    histogram[c] += 1

  if histogram[175] > 0:
    reducedPalette.append(175)
    histogram[175] = 0

  while len(reducedPalette) < 16:
    maxValue = -1
    maxIndex = 0

    for j in range(256):
      if histogram[j] > maxValue:
        maxValue = histogram[j]
        maxIndex = j

    reducedPalette.append(maxIndex)
    histogram[maxIndex] = 0

  paletteMap = [0 for i in range(256)]
  for i in range(256):
    if i == 175 and 175 in reducedPalette:
      paletteMap[i] = reducedPalette.index(175)
    else:
      paletteMap[i] = findClosestColor(paletteColors[i], [paletteColors[j] for j in reducedPalette])

  oneByte = 0
  byteCount = 0

  for c in a:
    if byteCount % 2 == 0:
      oneByte = paletteMap[c]
    else:
      oneByte = (oneByte << 4) | paletteMap[c]
      result.append(oneByte)

    byteCount += 1

  result = reducedPalette + result

  return result

def printArray(array, name, sizeString, dataType="const uint8_t"):
  print(dataType + " " + name + "[" + sizeString + "] = {")
  arrayString = ""

  lineLen = 0

  for v in array:
    item = str(v) + ","

    lineLen += len(item)

    if lineLen > 80:
      arrayString += "\n"
      lineLen = len(item)

    arrayString += item

  print(arrayString[:-1])
  print("}; // " + name)

if GUARDS:
  print("#ifndef " + NAME.upper() + "_TEXTURE_H")
  print("#define " + NAME.upper() + "_TEXTURE_H\n")

if USE_PALETTE:
  printArray(paletteArray,NAME + "Palette",str(len(paletteArray)),"const uint16_t" if USE_565 else "const uint8_t")
  print("")

print("#define " + NAME.upper() + "_TEXTURE_WIDTH " + str(OUT_WIDTH))
print("#define " + NAME.upper() + "_TEXTURE_HEIGHT " + str(OUT_HEIGHT))
print("")
  
if COMPRESS:
  imageArray = compressImageArray(imageArray)    

printArray(imageArray,NAME + "Texture",str(len(imageArray)))

if GUARDS:
  print("\n#endif // guard")

image2.save(NAME + "_preview.png")
