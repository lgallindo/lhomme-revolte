from PIL import Image

try:
    # Open the newly approved 1024x1024 concept
    img = Image.open('/home/lgms/code/research/worktrees/camera-effects/media/concept_vibe_revolte.png')
    
    # Upscale to 4096 x 4096 using nearest neighbor to preserve hard retro pixel edges
    # (Since it's an 8-bit style generation, Nearest is best for upscaling)
    img_max = img.resize((4096, 4096), Image.Resampling.NEAREST)
    
    # Save as the official promotional logo
    img_max.save('/home/lgms/code/research/worktrees/camera-effects/media/logo_big.png', 'PNG')
    print("Successfully upscaled promotional logo to 4096x4096.")
except Exception as e:
    print(f"Error: {e}")
