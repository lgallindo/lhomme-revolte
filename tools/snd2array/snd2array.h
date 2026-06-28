#ifndef SND2ARRAY_H
#define SND2ARRAY_H

#include <stdio.h>

typedef struct {
    int ok;
    int exit_code;
    char error_message[256];
} Snd2ArrayResult;

typedef struct {
    const char *filename;
} Snd2ArrayConfig;

Snd2ArrayResult snd2array_convert(const Snd2ArrayConfig *cfg, FILE *out);

#endif
