import sys
from PIL import Image

try:
    img = Image.open('/home/lgms/code/research/worktrees/camera-effects/media/concept_vibe_revolte.png')
    img = img.resize((128, 128))
    img.save('/home/lgms/code/research/worktrees/camera-effects/media/logo_big.png')
    print("Successfully resized and saved to logo_big.png")
except Exception as e:
    print(f"Error: {e}")
