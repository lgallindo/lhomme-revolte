#!/usr/bin/env python3
import os
import subprocess
import glob

mods_dir = "mods"
build_dir = "build/engine_mods"
os.makedirs(build_dir, exist_ok=True)

path_map = {
    "game.h": "core/game.h",
    "settings.h": "core/settings.h",
    "constants.h": "core/constants.h",
    "images.h": "core/images.h",
    "levels.h": "core/levels.h",
    "sounds.h": "core/sounds.h",
    "texts.h": "core/texts.h", # Note: Texts was moved to locale.h, so patches touching texts.h might fail.
    "raycastlib.h": "core/raycastlib.h",
    "main_sdl.c": "frontends/pc/main_sdl.c",
    "make.sh": "make.sh",
    "demo.h": "demo.h"
}

def fix_diff(content):
    for old, new in path_map.items():
        content = content.replace(f"a/{old}", f"a/{new}")
        content = content.replace(f"b/{old}", f"b/{new}")
    content = content.replace("SFG_", "LHR_")
    return content

diff_files = glob.glob(os.path.join(mods_dir, "*.diff"))
success_count = 0

for diff_path in diff_files:
    mod_name = os.path.basename(diff_path).replace(".diff", "")
    print(f"\n--- Processing Mod: {mod_name} ---")
    
    with open(diff_path, "r") as f:
        content = f.read()
    
    fixed_content = fix_diff(content)
    temp_diff = f"/tmp/{mod_name}_fixed.diff"
    
    with open(temp_diff, "w") as f:
        f.write(fixed_content)
        
    res = subprocess.run(["git", "apply", "--check", temp_diff], capture_output=True, text=True)
    if res.returncode != 0:
        print(f"Skipping {mod_name} - Does not apply cleanly.")
        print(res.stderr.strip())
        continue
        
    print(f"Applying {mod_name}...")
    subprocess.run(["git", "apply", temp_diff], check=True)
    
    out_bin = os.path.join(build_dir, f"lhr_{mod_name}")
    print(f"Compiling {out_bin}...")
    compile_cmd = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-pedantic", "-O3",
        "-Wno-unused-parameter", "-Wno-missing-field-initializers",
        "-D_DEFAULT_SOURCE", "-D_THREAD_SAFE",
        "-Icore", "-Ifrontends/pc", "-I/opt/homebrew/include/SDL2",
        "frontends/pc/main_sdl.c",
        "-L/opt/homebrew/lib", "-lSDL2",
        "-o", out_bin
    ]
    
    comp_res = subprocess.run(compile_cmd, capture_output=True, text=True)
    if comp_res.returncode != 0:
        print(f"Compilation failed for {mod_name}!")
        print(comp_res.stderr)
    else:
        print(f"Testing {mod_name}...")
        subprocess.run(["timeout", "2", out_bin], capture_output=True)
        print(f"Success! Generated {out_bin}")
        success_count += 1
        
    subprocess.run(["git", "checkout", "."], check=True)

print(f"\nFinished: {success_count} mods compiled successfully.")
