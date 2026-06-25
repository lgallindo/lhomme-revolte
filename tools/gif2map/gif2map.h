#ifndef GIF2MAP_H
#define GIF2MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define GIF2MAP_GIF_WIDTH 139
#define GIF2MAP_GIF_HEIGHT 139
#define GIF2MAP_MAP_SIZE 64
#define GIF2MAP_TILE_DICTIONARY_SIZE 64
#define GIF2MAP_TEXTURE_COUNT 7
#define GIF2MAP_MAX_LEVEL_ELEMENTS 128

typedef struct Gif2MapLevelElement
{
  uint8_t type;
  uint8_t x;
  uint8_t y;
} Gif2MapLevelElement;

typedef struct Gif2MapLevel
{
  uint8_t map_array[GIF2MAP_MAP_SIZE * GIF2MAP_MAP_SIZE];
  uint16_t tile_dictionary[GIF2MAP_TILE_DICTIONARY_SIZE];
  uint8_t texture_indices[GIF2MAP_TEXTURE_COUNT];
  uint8_t door_texture_index;
  uint8_t floor_color;
  uint8_t ceiling_color;
  uint8_t player_start[3];
  uint8_t background_image;
  Gif2MapLevelElement elements[GIF2MAP_MAX_LEVEL_ELEMENTS];
  size_t element_count;
} Gif2MapLevel;

typedef struct Gif2MapConfig
{
  const char *input_path;
  const char *output_path;
  const char *format;
  const char *symbol_name;
  int strict;
  int write_stdout;
} Gif2MapConfig;

typedef struct Gif2MapResult
{
  int ok;
  int exit_code;
  char error_message[256];
} Gif2MapResult;

Gif2MapResult gif2map_load_gif(const char *input_path, Gif2MapLevel *level);
Gif2MapResult gif2map_emit_c(FILE *out, const Gif2MapLevel *level,
  const char *symbol_name);
Gif2MapResult gif2map_emit_json(FILE *out, const Gif2MapLevel *level);
Gif2MapResult gif2map_convert(const Gif2MapConfig *cfg);

#endif
