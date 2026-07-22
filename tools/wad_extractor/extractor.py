import os
import re
from omg import WAD, Graphic

def extract_lumps(wad_path, output_dir, pattern_str=".*"):
    """
    Extracts Doom Picture Format graphic lumps from a WAD and saves them as PNGs.
    """
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        
    wad = WAD(wad_path)
    pattern = re.compile(pattern_str, re.IGNORECASE)
    
    extracted = []
    
    # We will search the WAD lumps.
    # Note: Graphics in Doom WADs can be in different namespaces (Sprites, Patches, etc.)
    # omgifol loads them automatically. We just need to iterate over graphics or lumps.
    # WAD object acts as a dictionary of lumps.
    groups = [wad.graphics, wad.sprites, wad.patches, wad.flats]
    
    for group in groups:
        for name in group:
            if pattern.match(name):
                try:
                    # Load as graphic
                    lump = group[name]
                    # Convert to PIL Image
                    img = lump.to_Image()
                    
                    # Convert palette mode to RGBA if needed, but saving as PNG handles 'P' mode fine
                    # However, to be safe for img2array we can convert to RGBA
                    img = img.convert("RGBA")
                    
                    out_path = os.path.join(output_dir, f"{name}.png")
                    img.save(out_path)
                    extracted.append(out_path)
                except Exception as e:
                    print(f"Failed to extract {name}: {e}")
                
    return extracted

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 3:
        print("Usage: python extractor.py <wad_path> <output_dir> [regex_pattern]")
        sys.exit(1)
    
    wad_file = sys.argv[1]
    out_dir = sys.argv[2]
    regex = sys.argv[3] if len(sys.argv) > 3 else ".*"
    
    extracted = extract_lumps(wad_file, out_dir, regex)
    print(f"Extracted {len(extracted)} lumps.")
