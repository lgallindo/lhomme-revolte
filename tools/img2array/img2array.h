#ifndef IMG2ARRAY_H
#define IMG2ARRAY_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
    int ok;
    int exit_code;
    char error_message[256];
} Img2ArrayResult;

typedef struct {
    int out_width;
    int out_height;
    int use_guards;
    const char *name;
    const char *palette_file;
    int use_565;
    int transpose;
    int compress;
    const char *filename;
} Img2ArrayConfig;

Img2ArrayResult img2array_convert(const Img2ArrayConfig *cfg, FILE *out);

#endif
