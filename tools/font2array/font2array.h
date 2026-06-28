#ifndef FONT2ARRAY_H
#define FONT2ARRAY_H

#include <stdio.h>

typedef struct {
    int ok;
    int exit_code;
    char error_message[256];
} Font2ArrayResult;

typedef struct {
    const char *filename;
} Font2ArrayConfig;

Font2ArrayResult font2array_convert(const Font2ArrayConfig *cfg, FILE *out);

#endif
