import re

with open('images.h', 'r') as f:
    images_h = f.read()

with open('new_logo.h', 'r') as f:
    new_logo = f.read()

match = re.search(r'SFG_logoImageTexture\[528\] = \{(.*?)\};', new_logo, re.DOTALL)
if match:
    texture_content = match.group(1)
    images_h = re.sub(
        r'SFG_PROGRAM_MEMORY uint8_t SFG_logoImage\[SFG_TEXTURE_STORE_SIZE\] =\s*\{.*?\};', 
        f'SFG_PROGRAM_MEMORY uint8_t SFG_logoImage[SFG_TEXTURE_STORE_SIZE] =\n{{{texture_content}}};', 
        images_h, 
        flags=re.DOTALL
    )
    with open('images.h', 'w') as f:
        f.write(images_h)
    print("Patched images.h successfully.")
else:
    print("Could not find texture in new_logo.h")
