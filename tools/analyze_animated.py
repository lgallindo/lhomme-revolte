import sys
from omg import WAD
import struct

def analyze(wad_path):
    w = WAD(wad_path)
    if 'ANIMATED' not in w.data:
        print("No ANIMATED lump found.")
        return
        
    data = w.data['ANIMATED'].data
    # ANIMATED lump format (Boom/MBF):
    # struct {
    #     char istexture; // 1 byte
    #     char last[9];   // 9 bytes
    #     char first[9];  // 9 bytes
    #     int speed;      // 4 bytes
    # } (23 bytes total)
    # Ends with 0xFF.
    
    print("--- ANIMATED LUMP CONTENTS ---")
    offset = 0
    while offset + 23 <= len(data):
        istexture = data[offset]
        if istexture == 0xFF:
            break
        last = data[offset+1:offset+10].split(b'\x00')[0].decode('ascii')
        first = data[offset+10:offset+19].split(b'\x00')[0].decode('ascii')
        speed = struct.unpack('<i', data[offset+19:offset+23])[0]
        
        type_str = "Texture" if istexture == 1 else "Flat"
        print(f"[{type_str}] {first} -> {last} (Speed: {speed})")
        offset += 23

if __name__ == '__main__':
    analyze(sys.argv[1])
