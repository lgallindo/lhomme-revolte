#include "gif2map.h"

#include <stdio.h>
#include <string.h>

static void print_usage(FILE *out)
{
  fprintf(out,
    "usage: gif2map --input LEVEL.gif [--output OUT] [--stdout]\n"
    "               [--format c|json] [--symbol NAME] [--strict]\n");
}

int main(int argc, char **argv)
{
  Gif2MapConfig cfg;
  Gif2MapResult result;
  int i;

  cfg.input_path = NULL;
  cfg.output_path = NULL;
  cfg.format = "c";
  cfg.symbol_name = NULL;
  cfg.strict = 0;
  cfg.write_stdout = 0;

  for (i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      print_usage(stdout);
      return 0;
    }
    else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc)
      cfg.input_path = argv[++i];
    else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc)
      cfg.output_path = argv[++i];
    else if (strcmp(argv[i], "--format") == 0 && i + 1 < argc)
      cfg.format = argv[++i];
    else if (strcmp(argv[i], "--symbol") == 0 && i + 1 < argc)
      cfg.symbol_name = argv[++i];
    else if (strcmp(argv[i], "--stdout") == 0)
      cfg.write_stdout = 1;
    else if (strcmp(argv[i], "--strict") == 0)
      cfg.strict = 1;
    else if (argv[i][0] != '-' && cfg.input_path == NULL)
      cfg.input_path = argv[i];
    else
    {
      fprintf(stderr, "gif2map: unknown or incomplete argument: %s\n",
        argv[i]);
      print_usage(stderr);
      return 2;
    }
  }

  result = gif2map_convert(&cfg);

  if (!result.ok)
  {
    fprintf(stderr, "gif2map: %s\n", result.error_message);
    return result.exit_code;
  }

  return 0;
}
