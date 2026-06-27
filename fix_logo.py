from PIL import Image

try:
    img = Image.open('/home/lgms/code/research/worktrees/camera-effects/media/concept_vibe_revolte.png').convert("RGBA")
    data = img.getdata()
    new_data = []
    
    # Replace black/dark background with the engine's transparent color (255, 4, 5)
    for item in data:
        if item[0] < 20 and item[1] < 20 and item[2] < 20:
            new_data.append((255, 4, 5, 255))
        else:
            new_data.append(item)
            
    img.putdata(new_data)
    img = img.resize((32, 32), Image.Resampling.NEAREST) # Use nearest so we don't blend transparent color into edges
    img = img.convert("RGB")
    img.save('/home/lgms/code/research/worktrees/camera-effects/media/logo32_transparent.png')
    print("Fixed transparency and resized to 32x32 successfully.")
except Exception as e:
    print(f"Error: {e}")
