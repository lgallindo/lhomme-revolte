#include "font2array.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    Font2ArrayConfig cfg;
    cfg.filename = "font.png";

    if (argc > 1) {
        cfg.filename = argv[1];
    }

    Font2ArrayResult res = font2array_convert(&cfg, stdout);
    if (!res.ok) {
        fprintf(stderr, "%s\n", res.error_message);
        return res.exit_code;
    }

    return 0;
}
