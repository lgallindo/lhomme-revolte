# Pixel Art Pipeline: Adversarial Analysis & Revision Proposal

**Author**: Senior Technical Pixel Artist (Agent Sub-Persona)
**Date**: 2026-06-25
**Context**: Reviewing the automated pipeline used to inject the "REVOLTE" vibe concept into `small3dlib`.

## 1. Aggressive Adversarial Critique

The current automated approach for converting AI-generated high-res concept art into engine-native 4-bit, 32x32 textures is fundamentally flawed and structurally destructive. 

### A. The "Down-sampling" Disaster
Taking a 1024x1024 RGB image and crushing it to 32x32 using `Image.Resampling.LANCZOS` (or even `NEAREST`) is artistic vandalism. 
*   **Loss of Form**: The brutalist typography is reduced to illegible sub-pixel noise.
*   **Contrast Bleed**: High-contrast edges blend into mush, destroying the "crunchy" retro vibe entirely.
*   **Orphan Pixels**: Nearest-neighbor scaling leaves disconnected, noisy pixels that jitter on the screen.

### B. Naive Color Quantization
The `img2array.py` script maps RGB values to the engine's 256-color palette using raw Euclidean distance.
*   It ignores perceptual color spaces (like Oklab) and fails to utilize **dithering**.
*   This results in extreme color banding and "flat" unshaded zones where subtle gradients should exist.

### C. The Transparency Hack
Scripting a replacement of "dark pixels (RGB < 20)" with the engine's transparent key `(255, 4, 5)` is highly dangerous.
*   It destroys dark shadows and inner outlines. 
*   It creates a "halo" effect around the logo where anti-aliased dark pixels survived the threshold and were wrongly mapped to solid background colors instead of being culled.

### D. The Transpose Fragility
The logo compiled sideways because `small3dlib` expects column-major order for memory-efficient vertical raycasting/blitting. Relying on manually remembering to append `-t` to a CLI script is fragile. 

---

## 2. Proposed Pipeline Revision

To achieve true 90s-era "id Software" quality, the pipeline must be completely overhauled.

### Step 1: Palette-Constrained Generation
Do not generate full RGB art. If generative AI is used, it should be immediately quantized to `assets/palette_adjusted.gif` at the highest resolution using a perceptual mapping algorithm with ordered dithering (e.g., Floyd-Steinberg).

### Step 2: Dedicated Pixel-Art Pass (Human/Aseprite)
A 32x32 texture cannot be mathematically derived from a 1024x1024 image without manual cleanup.
*   **Outline Restoration**: Hard-code solid outlines.
*   **Alpha Masking**: Define a strict Alpha channel in the `.png` rather than relying on "black" backgrounds.
*   **Contrast Tweaks**: Manually push shadows and highlights to use the extreme ends of the engine palette.

### Step 3: Rewrite `img2array.py` 
The engine asset compiler must be modernized:
1.  **Native Alpha Support**: Read the `.png` Alpha channel directly. Any pixel with `A < 128` is automatically assigned the `SFG_TRANSPARENT_COLOR` index (175).
2.  **Strict Dimension Assertions**: The script should throw a fatal error if fed an image that isn't exactly `SFG_TEXTURE_SIZE` x `SFG_TEXTURE_SIZE`.
3.  **Automatic Orientation**: The compiler should read an `engine_settings.json` or flag to automatically enforce Column-Major (`-t`) memory layouts for all textures, removing human error.
