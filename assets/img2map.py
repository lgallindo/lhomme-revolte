# Python tool to convert an image to map for the L'Homme Révolté game. The input image
# has to be in gif format.
#
# by drummerfish
# released under CC0 1.0.

import sys
import json
from PIL import Image

if "--help" in sys.argv or "-h" in sys.argv:
  print("usage: img2map.py [--json] input.gif")
  sys.exit(0)

jsonMode = False

if "--json" in sys.argv:
  jsonMode = True
  sys.argv.remove("--json")

if len(sys.argv) < 2:
  print("usage: img2map.py [--json] input.gif", file=sys.stderr)
  sys.exit(1)

print("WARNING: assets/img2map.py is deprecated; use tools/gif2map/build/gif2map.", file=sys.stderr)

elementTypes = [
    "NONE",
    "BARREL",
    "HEALTH",
    "BULLETS",
    "ROCKETS",
    "PLASMA",
    "TREE",
    "FINISH",
    "TELEPORTER",
    "TERMINAL",
    "COLUMN",
    "RUIN",
    "LAMP",
    "CARD0",
    "CARD1",
    "CARD2",
    "LOCK0",
    "LOCK1",
    "LOCK2",
    "BLOCKER",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "MONSTER_SPIDER",
    "MONSTER_DESTROYER",
    "MONSTER_WARRIOR",
    "MONSTER_PLASMABOT",
    "MONSTER_ENDER",
    "MONSTER_TURRET",
    "MONSTER_EXPLODER"
  ]

propertyTypes = [
    "ELEVATOR",
    "SQUEEZER",
    "DOOR"
]

image = Image.open(sys.argv[1])
pixels = image.load()

palette = []
paletteInverse = [0 for i in range(256)]

x = 5
y = 69
i = 0

# load the palette/sca

for i in range(256):
  if i % 64 == 0:
    x = 5
    y += 1

  palette.append(pixels[(x,y)])

  paletteInverse[pixels[(x,y)]] = i

  x += 1

def getPixel(x,y):
  return paletteInverse[pixels[(x,y)]]

def loadTileDict(x,y):
  result = []

  for i in range(64):
    texture = getPixel(x + i,y + 31)

    if texture > 7:
      raise(Exception("Texture index can't be higher than 7."))

    height = 0

    for j in range(31):
      if getPixel(x + i,y + 30 - j) == 7:
        break

      height += 1

    result.append((texture,height))

  return result

def defineName(n):
  c = chr(ord("A") + n)
  return c + c

floorDict = loadTileDict(5,37)
ceilDict = loadTileDict(5,5)
floorColor = getPixel(41,122)
ceilColor = getPixel(41,118)
backgroundTex = getPixel(41,126)
doorTex = getPixel(41,130)
playerStart = [0,0,0]
textures = []
elements = []
elementCount = 0
defines = []

levelMap = [[(0,False) for i in range(64)] for j in range(64)]

# load the map

for y in range(64):
  for x in range(64):
    n = getPixel(70 + x,5 + y)

    if n < 64:
      levelMap[63 - x][y] = (n,False)
    else:
      # tile with special property, create a define for it

      prop = n // 64 - 1
      tile = n % 64

      defNum = -1

      for i in range(len(defines)):
        if defines[i] == (tile,prop):
          defNum = i
          break
      
      if defNum == -1: # not found:
        defNum = len(defines)
        defines.append((tile,prop))

      levelMap[63 - x][y] = (defNum,True)

# load elements

playerFound = False

for y in range(64):
  for x in range(64):
    n = getPixel(x + 70, y + 70)

    if n < len(elementTypes):
      elements.append((n,63 - x,y)) 
    elif n >= 240:
      if playerFound:
        raise(Exception("Multiple player starting positions specified."))

      playerStart = [63 - x,y,(n - 240) * 16]
      playerFound = True

if not playerFound:
  raise(Exception("Player starting position not specified."))

if len(elements) > 128:
  raise(Exception("More than 128 level elements."))

elementCount = len(elements)

for i in range(128 - len(elements)):
  elements.append((0,0,0))

# load textures

x = 41
y = 114

for i in range(7):
  textures.append(getPixel(x,y))
  x += 4

def numAlign(n):
  return str(n) + "," + (" " if n < 10 else "")

def mapXScale():
  r = "    // "

  for i in range(64):
    r += str(i).ljust(2) + " "

  return r + "\n"

def printC():
  result = ""
  result += "  {          // level\n"
  result += "    {        // mapArray\n"
  result += "    #define o 0\n"

  for i in range(len(defines)):
    result += "    #define " + defineName(i) + " (" + str(defines[i][0]) + " | SFG_TILE_PROPERTY_" + propertyTypes[defines[i][1]] + ")\n"

  result += mapXScale()

  for y in range(64):
    result += "/*" + str(y).ljust(2) + "*/ "

    for x in range(64):
      item = levelMap[x][y]

      if not item[1]:
        result += ("o " if item[0] == 0 else str(item[0]).ljust(2))
      else:
        result += defineName(item[0])

      result += "," if y < 63 or x < 63 else " "

    result += " /*" + str(y).ljust(2) + "*/ \n"

  result += mapXScale()

  for i in range(len(defines)):
    result += "    #undef " + defineName(i) + "\n"

  result += "    #undef o\n"
  result += "    },\n"
  result += "    {        // tileDictionary\n      "

  for i in range(64):
    result += "SFG_TD(" + str(floorDict[i][1]).rjust(2) + "," + str(ceilDict[i][1]).rjust(2) + "," + str(floorDict[i][0]) + "," + str(ceilDict[i][0]) + ")"

    result += "," if i != 63 else " "

    if (i + 1) % 4 == 0:
      result += " // " + str(i - 3) + " \n      "

  result += "},                    // tileDictionary\n"

  s = ""
  first = True

  for t in textures:
    if first:
      first = False
    else:
      s += ","

    s += str(t).ljust(2)

  result += "    {" + s + "}, // textureIndices\n"
  result += "    " + numAlign(doorTex) + "                     // doorTextureIndex\n"
  result += "    " + numAlign(floorColor) + "                     // floorColor\n"
  result += "    " + numAlign(ceilColor) + "                     // ceilingColor\n"
  result += "    {" + str(playerStart[0]).ljust(2) + ", " + str(playerStart[1]).ljust(2) + ", " + str(playerStart[2]).ljust(3) + "},          // player start: x, y, direction\n"
  result += "    " + numAlign(backgroundTex) + "                     // backgroundImage\n"
  result += "    {                       // elements\n"

  even = True

  i = 0

  for e in elements:
    if even:
      result += "      "

    result += "{SFG_LEVEL_ELEMENT_" + elementTypes[e[0]] + ", {" + str(e[1]) + "," + str(e[2]) + "}}"

    if i < 127:
      result += ","

    if not even:
      result += "\n"

    even = not even

    i += 1

  result += "    }, // elements\n"
  result += "  } // level\n"

  print(result)

def mapPropertyMask(propIndex):
  if propIndex == 0:
    return 0x40
  elif propIndex == 1:
    return 0x80
  return 0xc0

def td(floorH, ceilH, floorT, ceilT):
  return ((floorH & 0x001f) |
          ((floorT & 0x0007) << 5) |
          ((ceilH & 0x001f) << 8) |
          ((ceilT & 0x0007) << 13))

def printJSON():
  mapArray = []

  for y in range(64):
    for x in range(64):
      item = levelMap[x][y]

      if item[1]:
        tile, prop = defines[item[0]]
        mapArray.append(tile | mapPropertyMask(prop))
      else:
        mapArray.append(item[0])

  tileDictionary = []

  for i in range(64):
    tileDictionary.append(td(floorDict[i][1], ceilDict[i][1], floorDict[i][0], ceilDict[i][0]))

  jsonElements = []

  for e in elements:
    jsonElements.append({"type": e[0], "x": e[1], "y": e[2]})

  payload = {
    "map_array": mapArray,
    "tile_dictionary": tileDictionary,
    "texture_indices": textures,
    "door_texture_index": doorTex,
    "floor_color": floorColor,
    "ceiling_color": ceilColor,
    "player_start": playerStart,
    "background_image": backgroundTex,
    "element_count": elementCount,
    "elements": jsonElements
  }

  print(json.dumps(payload, separators=(",", ":"), sort_keys=False))

if jsonMode:
  printJSON()
else:
  printC()

