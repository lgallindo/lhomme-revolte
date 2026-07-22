#!/usr/bin/env python3
import os
import re
import numpy as np
from PIL import Image, ImageTk
import tkinter as tk
from tkinter import filedialog, messagebox
from scipy.spatial import KDTree

def rgb565_to_rgb(val):
    r = (val >> 11) & 0x1F
    g = (val >> 5) & 0x3F
    b = val & 0x1F
    return ((r * 255) // 31, (g * 255) // 63, (b * 255) // 31)

def load_palette(header_path):
    palette = []
    with open(header_path, 'r') as f:
        content = f.read()
    match = re.search(r'paletteRGB565\[256\]\s*=\s*\{([^}]+)\}', content)
    if not match:
        raise ValueError("Could not find paletteRGB565 array")
    vals = re.findall(r'\b\d+\b', match.group(1))
    if len(vals) < 256:
        raise ValueError("Expected 256 colors")
    for v in vals[:256]:
        palette.append(rgb565_to_rgb(int(v)))
    return np.array(palette)

def apply_nearest(img_arr, tree, palette):
    h, w, _ = img_arr.shape
    flat = img_arr.reshape(-1, 3)
    _, indices = tree.query(flat)
    return palette[indices].reshape((h, w, 3)).astype(np.uint8)

def apply_floyd_steinberg(img_arr, tree, palette):
    h, w, _ = img_arr.shape
    float_arr = img_arr.astype(np.float32)
    out_arr = np.zeros_like(img_arr)
    
    for y in range(h):
        for x in range(w):
            old_p = float_arr[y, x]
            clamped = np.clip(old_p, 0, 255)
            _, idx = tree.query(clamped)
            new_p = palette[idx]
            out_arr[y, x] = new_p
            
            err = old_p - new_p
            
            if x + 1 < w: float_arr[y, x + 1] += err * 7 / 16
            if y + 1 < h:
                if x - 1 >= 0: float_arr[y + 1, x - 1] += err * 3 / 16
                float_arr[y + 1, x] += err * 5 / 16
                if x + 1 < w: float_arr[y + 1, x + 1] += err * 1 / 16
    return out_arr

class CMTGui:
    def __init__(self, root):
        self.root = root
        self.root.title("LHR Color Mapping Tool - Algorithm Comparator")
        
        try:
            self.palette = load_palette("core/palette.h")
            self.tree = KDTree(self.palette)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load palette: {e}")
            sys.exit(1)
            
        self.original_img = None
        
        # UI
        top_frame = tk.Frame(root)
        top_frame.pack(pady=10)
        
        tk.Button(top_frame, text="Load Image / GIF", command=self.load_image).pack()
        
        self.img_frame = tk.Frame(root)
        self.img_frame.pack(fill=tk.BOTH, expand=True)
        
        self.lbl_orig = tk.Label(self.img_frame, text="Original")
        self.lbl_orig.grid(row=0, column=0, padx=10)
        self.panel_orig = tk.Label(self.img_frame)
        self.panel_orig.grid(row=1, column=0, padx=10)
        
        self.lbl_nearest = tk.Label(self.img_frame, text="Nearest Neighbor (KDTree)")
        self.lbl_nearest.grid(row=0, column=1, padx=10)
        self.panel_nearest = tk.Label(self.img_frame)
        self.panel_nearest.grid(row=1, column=1, padx=10)
        
        self.lbl_dither = tk.Label(self.img_frame, text="Floyd-Steinberg Dithering")
        self.lbl_dither.grid(row=0, column=2, padx=10)
        self.panel_dither = tk.Label(self.img_frame)
        self.panel_dither.grid(row=1, column=2, padx=10)

    def load_image(self):
        filepath = filedialog.askopenfilename(filetypes=[("Images", "*.png;*.gif;*.jpg;*.bmp")])
        if not filepath:
            return
            
        img = Image.open(filepath).convert("RGB")
        # Resize if too large
        if img.width > 400 or img.height > 400:
            img.thumbnail((400, 400))
            
        img_arr = np.array(img)
        
        # Original
        self.tk_orig = ImageTk.PhotoImage(img)
        self.panel_orig.config(image=self.tk_orig)
        
        # Nearest
        near_arr = apply_nearest(img_arr, self.tree, self.palette)
        near_img = Image.fromarray(near_arr)
        self.tk_near = ImageTk.PhotoImage(near_img)
        self.panel_nearest.config(image=self.tk_near)
        
        # Dithered
        dith_arr = apply_floyd_steinberg(img_arr, self.tree, self.palette)
        dith_img = Image.fromarray(dith_arr)
        self.tk_dith = ImageTk.PhotoImage(dith_img)
        self.panel_dither.config(image=self.tk_dith)

if __name__ == "__main__":
    root = tk.Tk()
    app = CMTGui(root)
    root.mainloop()
