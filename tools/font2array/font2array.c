#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"
#include "font2array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static Font2ArrayResult result_make(int ok, int exit_code, const char *fmt, ...)
{
    Font2ArrayResult result;
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

const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,!?0123456789/-+()%";

Font2ArrayResult font2array_convert(const Font2ArrayConfig *cfg, FILE *out)
{
    int w, h, channels;
    unsigned char *pixels = stbi_load(cfg->filename, &w, &h, &channels, 3);
    if (!pixels) {
        return result_make(0, 1, "Failed to load image file '%s'", cfg->filename);
    }

    fprintf(out, "{\n");

    int column = 0;
    uint16_t value = 0;
    int index = 0;

    for (int x = 0; x < w; x++) {
        if (column == 4) {
            column = 0;
            value = 0;
            index++;
            continue;
        }

        for (int y = 0; y < h; y++) {
            unsigned char r = pixels[(y * w + x) * 3];
            value = value * 2 + (r < 128 ? 1 : 0);
        }

        if (column == 3) {
            fprintf(out, "  0x%04x, // %d \"%c\"\n", value, index, chars[index]);
        }

        column++;
    }

    fprintf(out, "}\n");

    stbi_image_free(pixels);
    return result_make(1, 0, NULL);
}
