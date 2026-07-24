# Spec Driven Definition: LHR Color Mapping Tool (CMT)

## 1. Intent and Scope
This specification serves as the primary artifact for the LHR Color Mapping Tool (CMT). The CMT is responsible for mapping arbitrary 24-bit RGB/RGBA assets (GIFs, PNGs, WAD graphics) into the strict 256-color RGB565 palette required by the L'Homme Révolté engine.

## 2. Preconditions and Constraints
- **Input:** 24-bit PNGs or GIFs (potentially animated).
- **Target Palette:** 256 colors defined in `core/palette.h`.
- **Output:** A C header (`.h`) containing a `const uint8_t` array of mapped indices (0-255) and a populated `SFG_MapImage` struct.
- **Dependency Constraint:** The tool should rely exclusively on Python standard libraries or standard image processing packages (e.g., Pillow, NumPy, SciPy) mapped within the `uv` environment.

## 3. Algorithmic Alternatives Evaluated
To map 24-bit colors to our 256-color palette, several mathematical approaches are defined:

### 3.1. Euclidean RGB Distance (Naive Approach)
- **Mechanism:** Calculates $\sqrt{(R_1-R_2)^2 + (G_1-G_2)^2 + (B_1-B_2)^2}$ for every pixel against all 256 palette colors.
- **Pros:** Computationally simple, easy to implement in NumPy.
- **Cons:** Human perception of color is non-linear in the RGB space; greens might map to greys inappropriately.

### 3.2. LAB Color Space Distance (CIEDE2000)
- **Mechanism:** Converts both the input pixel and the 256-color palette to the CIELAB color space, then applies the Delta E 2000 formula to find the closest visual match.
- **Pros:** Most accurate reflection of human perception.
- **Cons:** Very computationally expensive; requires SciPy/skimage dependencies.

### 3.3. K-D Tree in RGB or LAB Space
- **Mechanism:** The 256 palette colors are structured into a K-Dimensional Tree. Each pixel of the input image queries the K-D tree for the nearest neighbor.
- **Pros:** Vastly accelerates lookup (from $O(N)$ per pixel to $O(\log N)$).
- **Cons:** Slight overhead in tree construction (negligible for 256 colors).

### 3.4. Dithering (Floyd-Steinberg)
- **Mechanism:** Maps a pixel to the closest color, calculates the quantization error, and pushes the error to adjacent pixels (right, down-left, down, down-right).
- **Pros:** Preserves gradients and reduces color banding significantly, especially on Doom-style low-color skies.
- **Cons:** Cannot be perfectly parallelized with basic matrix operations due to sequential error propagation.

## 4. Business Logic and Selected Implementation
**Decision:** The CMT shall implement a hybrid approach: **K-D Tree lookup in Euclidean RGB space**, with an optional **Floyd-Steinberg Dithering** toggle. 

### Execution Flow:
1. **Palette Extraction:** The script parses `core/palette.h`, extracts the 256 `RGB565` hex values, and converts them to 8-bit `(R, G, B)` tuples.
2. **K-D Tree Generation:** A `scipy.spatial.KDTree` is built using the 256 tuples.
3. **Asset Ingestion:** Pillow opens the target file. If it's a GIF, it iterates through all frames.
4. **Quantization:** 
   - If Dithering is OFF: NumPy vectorization maps all pixels to the K-D Tree in one pass.
   - If Dithering is ON: A sequential Floyd-Steinberg algorithm iterates over the image, applying the K-D Tree lookup and propagating quantization error.
5. **C-Array Generation:** The matched 8-bit indices are formatted into a comma-separated C string, packed into the `SFG_MapImage` struct syntax, and dumped to a file.
