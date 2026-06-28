#include "snd2array.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    Snd2ArrayConfig cfg;
    cfg.filename = NULL;

    if (argc > 1) {
        cfg.filename = argv[1];
    }

    if (!cfg.filename) {
        fprintf(stderr, "Convert a sound to C array.\n");
        fprintf(stderr, "usage:\n\n");
        fprintf(stderr, "  snd2array <sound.raw>\n\n");
        fprintf(stderr, "The file must be in raw mono 8kHz 8bit PCM format.\n");
        return 1;
    }

    Snd2ArrayResult res = snd2array_convert(&cfg, stdout);
    if (!res.ok) {
        fprintf(stderr, "%s\n", res.error_message);
        return res.exit_code;
    }

    return 0;
}
