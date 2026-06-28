#include "snd2array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

static Snd2ArrayResult result_make(int ok, int exit_code, const char *fmt, ...)
{
    Snd2ArrayResult result;
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

Snd2ArrayResult snd2array_convert(const Snd2ArrayConfig *cfg, FILE *out)
{
    FILE *f = fopen(cfg->filename, "rb");
    if (!f) {
        return result_make(0, 1, "Failed to open sound file '%s'", cfg->filename);
    }

    size_t cap = 2048;
    uint8_t *buffer = (uint8_t *)malloc(cap);
    if (!buffer) {
        fclose(f);
        return result_make(0, 1, "Out of memory");
    }
    size_t count = 0;

    int even = 1;
    uint8_t val = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        uint8_t quantized = (uint8_t)(c / 16);
        if (even) {
            val = (uint8_t)(quantized << 4);
        } else {
            val = val | quantized;
            if (count >= cap) {
                cap *= 2;
                uint8_t *new_buf = (uint8_t *)realloc(buffer, cap);
                if (!new_buf) {
                    free(buffer);
                    fclose(f);
                    return result_make(0, 1, "Out of memory");
                }
                buffer = new_buf;
            }
            buffer[count++] = val;
        }
        even = !even;
    }
    fclose(f);

    fprintf(out, "uint8_t sound[%zu] = {\n", count);
    for (size_t i = 0; i < count; i++) {
        if (i > 0) {
            fprintf(out, ",");
            if (i % 20 == 0) {
                fprintf(out, "\n");
            }
        }
        fprintf(out, "%d", buffer[i]);
    }
    fprintf(out, "\n}");

    free(buffer);
    return result_make(1, 0, NULL);
}
