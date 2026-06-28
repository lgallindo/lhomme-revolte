#include "img2array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    Img2ArrayConfig cfg;
    cfg.out_width = 64;
    cfg.out_height = 64;
    cfg.use_guards = 0;
    cfg.name = "texture";
    cfg.palette_file = NULL;
    cfg.use_565 = 0;
    cfg.transpose = 0;
    cfg.compress = 0;
    cfg.filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-x", 2) == 0) {
            cfg.out_width = atoi(argv[i] + 2);
        } else if (strncmp(argv[i], "-y", 2) == 0) {
            cfg.out_height = atoi(argv[i] + 2);
        } else if (strcmp(argv[i], "-h") == 0) {
            cfg.use_guards = 1;
        } else if (strncmp(argv[i], "-n", 2) == 0) {
            cfg.name = argv[i] + 2;
        } else if (strncmp(argv[i], "-p", 2) == 0) {
            cfg.palette_file = argv[i] + 2;
        } else if (strcmp(argv[i], "-5") == 0) {
            cfg.use_565 = 1;
        } else if (strcmp(argv[i], "-t") == 0) {
            cfg.transpose = 1;
        } else if (strcmp(argv[i], "-c") == 0) {
            cfg.compress = 1;
        } else {
            cfg.filename = argv[i];
        }
    }

    if (!cfg.filename) {
        fprintf(stderr, "Convert image to C array for small3dlib.\n");
        fprintf(stderr, "usage:\n\n");
        fprintf(stderr, "  img2array [-xW -yH -h -nS -pT -5 -c -t] file\n\n");
        fprintf(stderr, "  -xW     set width of the output to W pixels\n");
        fprintf(stderr, "  -yH     set height of the output to H pixels\n");
        fprintf(stderr, "  -h      include header guards (for texture per file)\n");
        fprintf(stderr, "  -nS     use the name S for the texture (default: \"texture\")\n");
        fprintf(stderr, "  -pT     use palette from file T and indexed colors (otherwise direct colors)\n");
        fprintf(stderr, "  -5      use 565 format instead of RGB8\n");
        fprintf(stderr, "  -c      compress (4 bpp, 16 color palette), only with -pT\n");
        fprintf(stderr, "  -t      transpose (store by columns)\n");
        return 1;
    }

    Img2ArrayResult res = img2array_convert(&cfg, stdout);
    if (!res.ok) {
        fprintf(stderr, "%s\n", res.error_message);
        return res.exit_code;
    }

    return 0;
}
