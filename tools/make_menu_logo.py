from PIL import Image

try:
    img = Image.open('/home/lgms/code/research/worktrees/camera-effects/media/concept_vibe_revolte.png')
    img = img.resize((32, 32), Image.Resampling.LANCZOS)
    img.save('/home/lgms/code/research/worktrees/camera-effects/media/logo32.png')
    print("Resized to 32x32 successfully.")
except Exception as e:
    print(f"Error: {e}")
