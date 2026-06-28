#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"
#include "img2array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Img2ArrayResult result_make(int ok, int exit_code, const char *fmt, ...)
{
    Img2ArrayResult result;
    va_list args;
    result.ok = ok;
    result.exit_code = exit_code;
    result.error_message[0] = '\0';
    if (fmt != NULL) {
        va_start(args, fmt);
        vsnprintf(result.error_message, sizeof(result.error_message), fmt, args);
        va_end(args);
    }
    return result;
}

static uint16_t rgbTo565(const uint8_t *rgb)
{
    return (uint16_t)(((rgb[0] >> 3) << 11) | ((rgb[1] >> 2) << 5) | (rgb[2] >> 3));
}

static int findClosestColor(const uint8_t *pixel, const uint8_t *paletteColors, int paletteSize)
{
    int closestIndex = 0;
    int closestDiff = 1000000;
    for (int i = 0; i < paletteSize; i++) {
        int pr = paletteColors[i * 3 + 0];
        int pg = paletteColors[i * 3 + 1];
        int pb = paletteColors[i * 3 + 2];
        int diff = abs(pixel[0] - pr) + abs(pixel[1] - pg) + abs(pixel[2] - pb);
        if (diff < closestDiff) {
            closestIndex = i;
            closestDiff = diff;
        }
    }
    return closestIndex;
}

Img2ArrayResult img2array_convert(const Img2ArrayConfig *cfg, FILE *out)
{
    int w, h, channels;
    uint8_t *pixels = stbi_load(cfg->filename, &w, &h, &channels, 4);
    if (!pixels) {
        return result_make(0, 1, "Failed to load input image '%s'", cfg->filename);
    }

    if (w != cfg->out_width || h != cfg->out_height) {
        stbi_image_free(pixels);
        return result_make(0, 1, "FATAL ERROR: Input image dimensions (%dx%d) do not match expected output dimensions (%dx%d).",
                           w, h, cfg->out_width, cfg->out_height);
    }

    uint8_t *paletteColors = NULL;
    int paletteSize = 0;
    uint16_t *paletteArray565 = NULL;
    uint8_t *paletteArray8 = NULL;

    if (cfg->palette_file && strlen(cfg->palette_file) > 0) {
        int pw, ph, pchannels;
        uint8_t *p_pixels = stbi_load(cfg->palette_file, &pw, &ph, &pchannels, 3);
        if (!p_pixels) {
            stbi_image_free(pixels);
            return result_make(0, 1, "Failed to load palette image '%s'", cfg->palette_file);
        }
        paletteSize = pw * ph;
        paletteColors = (uint8_t *)malloc(paletteSize * 3);
        memcpy(paletteColors, p_pixels, paletteSize * 3);
        stbi_image_free(p_pixels);

        if (cfg->use_565) {
            paletteArray565 = (uint16_t *)malloc(paletteSize * sizeof(uint16_t));
            for (int i = 0; i < paletteSize; i++) {
                paletteArray565[i] = rgbTo565(&paletteColors[i * 3]);
            }
        } else {
            paletteArray8 = (uint8_t *)malloc(paletteSize * 3);
            for (int i = 0; i < paletteSize; i++) {
                paletteArray8[i * 3 + 0] = paletteColors[i * 3 + 0];
                paletteArray8[i * 3 + 1] = paletteColors[i * 3 + 1];
                paletteArray8[i * 3 + 2] = paletteColors[i * 3 + 2];
            }
        }
    }

    int imageArraySize = cfg->out_width * cfg->out_height;
    int *imageArray = (int *)malloc(imageArraySize * sizeof(int));

    for (int y = 0; y < cfg->out_height; y++) {
        for (int x = 0; x < cfg->out_width; x++) {
            int x2 = cfg->transpose ? y : x;
            int y2 = cfg->transpose ? x : y;
            int srcIdx = (y2 * w + x2) * 4;
            uint8_t r = pixels[srcIdx + 0];
            uint8_t g = pixels[srcIdx + 1];
            uint8_t b = pixels[srcIdx + 2];
            uint8_t a = pixels[srcIdx + 3];

            int outIdx = y * cfg->out_width + x;

            if (cfg->palette_file && strlen(cfg->palette_file) > 0) {
                if (a < 128) {
                    imageArray[outIdx] = 175; // Transparent key
                } else {
                    uint8_t rgb[3] = {r, g, b};
                    imageArray[outIdx] = findClosestColor(rgb, paletteColors, paletteSize);
                }
            } else {
                if (cfg->use_565) {
                    uint8_t rgb[3] = {r, g, b};
                    imageArray[outIdx] = rgbTo565(rgb);
                } else {
                    imageArray[outIdx * 3 + 0] = r;
                    imageArray[outIdx * 3 + 1] = g;
                    imageArray[outIdx * 3 + 2] = b;
                }
            }
        }
    }

    // Compression
    int finalArraySize = imageArraySize;
    uint8_t *compressedArray = NULL;

    if (cfg->palette_file && strlen(cfg->palette_file) > 0 && cfg->compress) {
        // Histogram calculation to select the 16 most common palette indices in the image
        int histogram[256];
        memset(histogram, 0, sizeof(histogram));
        for (int i = 0; i < imageArraySize; i++) {
            int idx = imageArray[i];
            if (idx >= 0 && idx < 256) {
                histogram[idx]++;
            }
        }

        int reducedPalette[16];
        int reducedPaletteSize = 0;

        // If transparent color is present, force it to be at reducedPalette[0]
        if (histogram[175] > 0) {
            reducedPalette[reducedPaletteSize++] = 175;
            histogram[175] = 0;
        }

        while (reducedPaletteSize < 16) {
            int maxValue = -1;
            int maxIndex = 0;
            for (int j = 0; j < 256; j++) {
                if (histogram[j] > maxValue) {
                    maxValue = histogram[j];
                    maxIndex = j;
                }
            }
            reducedPalette[reducedPaletteSize++] = maxIndex;
            histogram[maxIndex] = 0;
        }

        int paletteMap[256];
        for (int i = 0; i < 256; i++) {
            int foundIdx = -1;
            for (int j = 0; j < 16; j++) {
                if (reducedPalette[j] == i) {
                    foundIdx = j;
                    break;
                }
            }
            if (i == 175 && foundIdx != -1) {
                paletteMap[i] = foundIdx;
            } else {
                // Find closest color in the reduced palette
                int closestIdx = 0;
                int closestDiff = 1000000;
                const uint8_t *origC = &paletteColors[i * 3];
                for (int j = 0; j < 16; j++) {
                    const uint8_t *reducedC = &paletteColors[reducedPalette[j] * 3];
                    int diff = abs(origC[0] - reducedC[0]) + abs(origC[1] - reducedC[1]) + abs(origC[2] - reducedC[2]);
                    if (diff < closestDiff) {
                        closestIdx = j;
                        closestDiff = diff;
                    }
                }
                paletteMap[i] = closestIdx;
            }
        }

        finalArraySize = 16 + imageArraySize / 2;
        compressedArray = (uint8_t *)malloc(finalArraySize);

        // First 16 bytes are the reduced palette indices
        for (int i = 0; i < 16; i++) {
            compressedArray[i] = (uint8_t)reducedPalette[i];
        }

        uint8_t oneByte = 0;
        int byteCount = 0;
        int outIdx = 16;

        for (int i = 0; i < imageArraySize; i++) {
            int subIdx = paletteMap[imageArray[i]];
            if (byteCount % 2 == 0) {
                oneByte = subIdx;
            } else {
                oneByte = (oneByte << 4) | subIdx;
                compressedArray[outIdx++] = oneByte;
            }
            byteCount++;
        }
    }

    // Output formatting to 'out' stream
    char uppercaseName[256];
    strncpy(uppercaseName, cfg->name, sizeof(uppercaseName));
    uppercaseName[sizeof(uppercaseName) - 1] = '\0';
    for (int i = 0; uppercaseName[i]; i++) {
        if (uppercaseName[i] >= 'a' && uppercaseName[i] <= 'z') {
            uppercaseName[i] = uppercaseName[i] - 'a' + 'A';
        }
    }

    if (cfg->use_guards) {
        fprintf(out, "#ifndef %s_TEXTURE_H\n", uppercaseName);
        fprintf(out, "#define %s_TEXTURE_H\n\n", uppercaseName);
    }

    if (cfg->palette_file && strlen(cfg->palette_file) > 0) {
        if (cfg->use_565) {
            fprintf(out, "const uint16_t %sPalette[%d] = {", cfg->name, paletteSize);
            for (int i = 0; i < paletteSize; i++) {
                if (i % 12 == 0) fprintf(out, "\n  ");
                fprintf(out, "%d,", paletteArray565[i]);
            }
            fprintf(out, "\n};\n\n");
        } else {
            fprintf(out, "const uint8_t %sPalette[%d] = {", cfg->name, paletteSize * 3);
            for (int i = 0; i < paletteSize * 3; i++) {
                if (i % 12 == 0) fprintf(out, "\n  ");
                fprintf(out, "%d,", paletteArray8[i]);
            }
            fprintf(out, "\n};\n\n");
        }
    }

    fprintf(out, "#define %s_TEXTURE_WIDTH %d\n", uppercaseName, cfg->out_width);
    fprintf(out, "#define %s_TEXTURE_HEIGHT %d\n\n", uppercaseName, cfg->out_height);

    if (cfg->compress && compressedArray) {
        fprintf(out, "const uint8_t %sTexture[%d] = {\n", cfg->name, finalArraySize);
        int lineLen = 0;
        for (int i = 0; i < finalArraySize; i++) {
            char item[32];
            sprintf(item, "%d,", compressedArray[i]);
            lineLen += strlen(item);
            if (lineLen > 80) {
                fprintf(out, "\n");
                lineLen = strlen(item);
            }
            fprintf(out, "%s", item);
        }
        fprintf(out, "\n}; // %sTexture\n", cfg->name);
    } else {
        int totalOutElements = imageArraySize;
        if (!cfg->palette_file && !cfg->use_565) {
            totalOutElements = imageArraySize * 3;
        }
        fprintf(out, "const uint8_t %sTexture[%d] = {\n", cfg->name, totalOutElements);
        int lineLen = 0;
        for (int i = 0; i < totalOutElements; i++) {
            char item[32];
            sprintf(item, "%d,", imageArray[i]);
            lineLen += strlen(item);
            if (lineLen > 80) {
                fprintf(out, "\n");
                lineLen = strlen(item);
            }
            fprintf(out, "%s", item);
        }
        fprintf(out, "\n}; // %sTexture\n", cfg->name);
    }

    if (cfg->use_guards) {
        fprintf(out, "\n#endif // guard\n");
    }

    // Clean up
    stbi_image_free(pixels);
    if (paletteColors) free(paletteColors);
    if (paletteArray565) free(paletteArray565);
    if (paletteArray8) free(paletteArray8);
    if (imageArray) free(imageArray);
    if (compressedArray) free(compressedArray);

    return result_make(1, 0, NULL);
}
