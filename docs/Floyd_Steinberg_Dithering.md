# Floyd-Steinberg Dithering: Technical Specification

## 1. Introduction

**Floyd-Steinberg Dithering** is an image dithering algorithm first published in 1976 by Robert W. Floyd and Louis Steinberg. It is widely used in image manipulation software, such as when an image is converted into GIF format or quantized to a smaller color palette (like our 256-color LHR palette). 

### ELI5 (Explain Like I'm 5)
Imagine you have a big box of crayons with 16 million colors, but your teacher says you can only use 256 specific colors to draw a picture. If you just pick the closest crayon for every spot, the picture will look blocky, and smooth skies will look like a staircase of stripes (we call this *banding*). 
To fix this, when you use a crayon that isn't quite the right color, you take note of how "wrong" it is. Then, you spread that "wrongness" (the error) to the empty spots next to it before you color them. Because the errors cancel each other out over a large area, your eyes blend the dots together, making the picture look like it has more colors than it actually does!

## 2. The Hard Math (Error Diffusion)

The algorithm operates on a simple principle: **error diffusion**. For each pixel, the algorithm quantizes the pixel's color to the nearest available palette color. The difference between the original color and the quantized color is the **quantization error**.

Let $I(x, y)$ be the original pixel color at coordinates $(x, y)$, and $P(c)$ be the quantized color from the palette.

1. **Calculate New Pixel**: 
   $$NewPixel = \text{nearest\_palette\_color}(I(x, y))$$
2. **Calculate Quantization Error**:
   $$E = I(x, y) - NewPixel$$
   *(Note: This is calculated independently for Red, Green, and Blue channels).*
3. **Diffuse Error**:
   The error $E$ is distributed to the neighboring pixels that have not yet been processed, according to the following weights:
   - Pixel $(x+1, y)$: gets $\frac{7}{16} \times E$
   - Pixel $(x-1, y+1)$: gets $\frac{3}{16} \times E$
   - Pixel $(x, y+1)$: gets $\frac{5}{16} \times E$
   - Pixel $(x+1, y+1)$: gets $\frac{1}{16} \times E$

### Matrix Representation
The diffusion can be represented as a kernel:
$$
\begin{bmatrix}
 & \star & \frac{7}{16} \\
\frac{3}{16} & \frac{5}{16} & \frac{1}{16}
\end{bmatrix}
$$
Where $\star$ represents the current pixel being processed. 
*(Because the weights sum to $\frac{16}{16} = 1$, the algorithm preserves the overall brightness and color of the original image).*

## 3. Common Implementations

Because the algorithm depends on the error of the *previous* pixel, it is inherently **sequential**. It cannot be easily vectorized with matrix operations (like NumPy's `dot` or `matmul`) because pixel $(x+1, y)$ cannot be processed until pixel $(x, y)$ is fully computed.

### Python Pseudo-code
```python
for y from 0 to height:
    for x from 0 to width:
        oldpixel  = image[y][x]
        newpixel  = find_closest_palette_color(oldpixel)
        image[y][x]  = newpixel
        quant_error  = oldpixel - newpixel
        
        if x + 1 < width:
            image[y][x + 1] = image[y][x + 1] + quant_error * 7 / 16
        if y + 1 < height:
            if x - 1 >= 0:
                image[y + 1][x - 1] = image[y + 1][x - 1] + quant_error * 3 / 16
            image[y + 1][x] = image[y + 1][x] + quant_error * 5 / 16
            if x + 1 < width:
                image[y + 1][x + 1] = image[y + 1][x + 1] + quant_error * 1 / 16
```

## 4. Worked Out Example

Imagine a grayscale image (0 is black, 255 is white). Our palette only has `[0, 255]` (pure black and pure white).
We are processing a pixel at $(0,0)$ with a grayscale value of **100**.

1. **Quantize**: The closest palette color to 100 is **0** (Black).
2. **Error**: $100 - 0 = 100$. The quantization error is $+100$.
3. **Diffuse**:
   - Pixel to the right $(1,0)$ gets: $100 \times \frac{7}{16} \approx 44$. Its value increases by 44.
   - Pixel below-left $(-1,1)$ gets: (Out of bounds, ignore).
   - Pixel below $(0,1)$ gets: $100 \times \frac{5}{16} \approx 31$.
   - Pixel below-right $(1,1)$ gets: $100 \times \frac{1}{16} \approx 6$.

By pushing this "+100" brightness to the surrounding pixels, the algorithm guarantees that eventually, one of those neighboring pixels will exceed 128 and be quantized to 255 (White), perfectly balancing the localized brightness.

## 5. Bibliography
- Floyd, R. W., & Steinberg, L. (1976). *An adaptive algorithm for spatial grey scale*. Proceedings of the Society of Information Display, 17(2), 75-77. [🔗 Link to abstract](https://dl.acm.org/doi/10.1145/360018.360024)
- *Color Quantization*. Wikipedia. [🔗 Link](https://en.wikipedia.org/wiki/Color_quantization)
- *Dither*. Wikipedia. [🔗 Link](https://en.wikipedia.org/wiki/Dither)
