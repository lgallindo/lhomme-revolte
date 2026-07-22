import argparse
import os
import sys
from PIL import Image
from omg import WAD

# Ensure cmt.py can be imported
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
from cmt import process_image, load_palette, generate_c_header

def extract_flat(wad_path, flat_name, palette_path, output_dir):
    print(f"Opening WAD: {wad_path}")
    wad = WAD(wad_path)
    
    if "PLAYPAL" not in wad.data:
        raise ValueError("No PLAYPAL lump found in the WAD.")
    
    if flat_name not in wad.flats:
        raise ValueError(f"Flat {flat_name} not found in the WAD flats.")
        
    print("Extracting DOOM Palette (PLAYPAL)...")
    pal_data = wad.data["PLAYPAL"].data
    # Doom palette has 256 RGB colors (3 bytes each = 768 bytes)
    doom_palette = [tuple(pal_data[i:i+3]) for i in range(0, 768, 3)]
    
    print(f"Extracting flat: {flat_name}...")
    flat_lump = wad.flats[flat_name]
    
    if len(flat_lump.data) != 4096:
        print(f"Warning: Flat {flat_name} data is {len(flat_lump.data)} bytes, expected 4096 (64x64).")
    
    # Map raw Doom indices to RGB using PLAYPAL
    rgb_data = bytes([c for idx in flat_lump.data[:4096] for c in doom_palette[idx]])
    
    # Create PIL image
    img = Image.frombytes("RGB", (64, 64), rgb_data)
    temp_png = os.path.join(output_dir, f"{flat_name}_raw.png")
    img.save(temp_png)
    print(f"Saved intermediate RGB graphic to {temp_png}")
    
    # Pass through Color Mapping Tool (cmt.py)
    print("Mapping to LHR strict 256-color palette (Floyd-Steinberg Dithering)...")
    lhr_palette = load_palette(palette_path)
    # process_image returns a tuple: (frames, width, height, num_frames, fps)
    frames, width, height, num_frames, fps = process_image(temp_png, lhr_palette, dither=True)
    
    # Generate C Array
    out_h = os.path.join(output_dir, f"{flat_name.lower()}.h")
    header_str = generate_c_header(frames, 64, 64, 1, 0, out_h, f"LHR_FLAT_{flat_name}")
    with open(out_h, "w") as f:
        f.write(header_str)
    print(f"Extraction successful! C-array written to {out_h}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Extract a flat from a DOOM WAD and convert it to LHR format.")
    parser.add_argument("--wad", required=True, help="Path to the DOOM WAD file")
    parser.add_argument("--flat", required=True, help="Name of the flat lump (e.g., FLOOR4_8)")
    parser.add_argument("--outdir", default="tests/samples/", help="Output directory")
    parser.add_argument("--palette", default="core/palette.h", help="Path to LHR palette.h")
    
    args = parser.parse_args()
    
    if not os.path.exists(args.outdir):
        os.makedirs(args.outdir)
        
    extract_flat(args.wad, args.flat, args.palette, args.outdir)
