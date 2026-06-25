from PIL import Image

try:
    # 1. Open original high-res concept
    img = Image.open('/home/lgms/code/research/worktrees/camera-effects/media/concept_vibe_revolte.png').convert("RGBA")
    
    # 2. Resize to strict 32x32 bounds BEFORE compiler (Nearest Neighbor to preserve retro feel)
    img = img.resize((32, 32), Image.Resampling.NEAREST)
    
    # 3. Create native alpha mask instead of magenta hack
    data = img.getdata()
    new_data = []
    
    for item in data:
        # If it's a dark background pixel, make it completely transparent (Alpha = 0)
        if item[0] < 20 and item[1] < 20 and item[2] < 20:
            new_data.append((0, 0, 0, 0)) # Fully transparent
        else:
            # Ensure fully opaque for logo pixels
            new_data.append((item[0], item[1], item[2], 255))
            
    img.putdata(new_data)
    
    # 4. Save as true RGBA .png
    img.save('/home/lgms/code/research/worktrees/camera-effects/media/logo32_alpha.png')
    print("Created strict 32x32 RGBA logo with native transparency.")
except Exception as e:
    print(f"Error: {e}")
