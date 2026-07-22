import argparse
import os
import sys
import numpy as np
from PIL import Image
from omg import WAD

# Ensure cmt.py can be imported
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
from cmt import process_image, load_palette, generate_c_header

def extract_graphic(wad_path, graphic_name, palette_path, output_dir):
    print(f"Opening WAD: {wad_path}")
    wad = WAD(wad_path)
    
    graphic_lump = None
    if graphic_name in wad.sprites:
        graphic_lump = wad.sprites[graphic_name]
    elif graphic_name in wad.graphics:
        graphic_lump = wad.graphics[graphic_name]
    elif graphic_name in wad.patches:
        graphic_lump = wad.patches[graphic_name]
        
    if not graphic_lump:
        raise ValueError(f"Graphic {graphic_name} not found in sprites, graphics, or patches.")
        
    print(f"Extracting graphic: {graphic_name}...")
    
    # Extract to PIL Image
    # Omgifol converts the Doom picture format and renders empty space as transparent.
    # When converted to RGB, the transparent pixels (index 255 in the generated palette)
    # usually become Magenta (255, 0, 255).
    img = graphic_lump.to_Image()
    
    # Save the intermediate PNG for verification
    temp_png = os.path.join(output_dir, f"{graphic_name}_raw.png")
    img.save(temp_png)
    print(f"Saved intermediate graphic to {temp_png}")
    
    # Pass through Color Mapping Tool (cmt.py)
    print("Mapping to LHR strict 256-color palette...")
    lhr_palette = load_palette(palette_path)
    
    # For graphics, we use Magenta as the chromakey to map to LHR transparent index 0
    chromakey = np.array([255, 0, 255], dtype=np.uint8)
    frames, width, height, num_frames, fps = process_image(temp_png, lhr_palette, dither=True, chromakey=chromakey)
    
    # Generate C Array
    out_h = os.path.join(output_dir, f"{graphic_name.lower()}.h")
    header_str = generate_c_header(frames, width, height, num_frames, fps, out_h, f"LHR_GRAPHIC_{graphic_name}")
    with open(out_h, "w") as f:
        f.write(header_str)
        
    print(f"Extraction successful! C-array written to {out_h}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Extract a graphic from a DOOM WAD and convert it to LHR format.")
    parser.add_argument("--wad", required=True, help="Path to the DOOM WAD file")
    parser.add_argument("--graphic", required=True, help="Name of the graphic lump (e.g., TITLEPIC, POSSA1)")
    parser.add_argument("--outdir", default="tests/samples/", help="Output directory")
    parser.add_argument("--palette", default="core/palette.h", help="Path to LHR palette.h")
    
    args = parser.parse_args()
    
    if not os.path.exists(args.outdir):
        os.makedirs(args.outdir)
        
    extract_graphic(args.wad, args.graphic, args.palette, args.outdir)
