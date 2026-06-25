#include "gif2map.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define GIF2MAP_MAX_GIF_CODES 4096
#define GIF2MAP_TILE_PROPERTY_MASK 0xc0
#define GIF2MAP_TILE_PROPERTY_ELEVATOR 0x40
#define GIF2MAP_TILE_PROPERTY_SQUEEZER 0x80
#define GIF2MAP_TILE_PROPERTY_DOOR 0xc0

typedef struct GifImage
{
  uint16_t width;
  uint16_t height;
  uint8_t *pixels;
} GifImage;

typedef struct FileBuffer
{
  uint8_t *data;
  size_t size;
} FileBuffer;

typedef struct GifCursor
{
  const uint8_t *data;
  size_t size;
  size_t pos;
} GifCursor;

typedef struct BitReader
{
  const uint8_t *data;
  size_t size;
  size_t bit_pos;
} BitReader;

static Gif2MapResult result_make(int ok, int exit_code, const char *fmt, ...)
{
  Gif2MapResult result;
  va_list args;

  result.ok = ok;
  result.exit_code = exit_code;
  result.error_message[0] = '\0';

  if (fmt != NULL)
  {
    va_start(args, fmt);
    vsnprintf(result.error_message, sizeof(result.error_message), fmt, args);
    va_end(args);
  }

  return result;
}

static uint16_t read_le16(const uint8_t *p)
{
  return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint16_t tile_definition(uint8_t floor_h, uint8_t ceil_h,
  uint8_t floor_t, uint8_t ceil_t)
{
  return (uint16_t)
    ((floor_h & 0x1f) |
    ((floor_t & 0x07) << 5) |
    ((ceil_h & 0x1f) << 8) |
    ((ceil_t & 0x07) << 13));
}

static int cursor_read_u8(GifCursor *cur, uint8_t *out)
{
  if (cur->pos >= cur->size)
    return 0;

  *out = cur->data[cur->pos++];
  return 1;
}

static int cursor_skip(GifCursor *cur, size_t bytes)
{
  if (bytes > cur->size - cur->pos)
    return 0;

  cur->pos += bytes;
  return 1;
}

static int skip_sub_blocks(GifCursor *cur)
{
  uint8_t block_size;

  for (;;)
  {
    if (!cursor_read_u8(cur, &block_size))
      return 0;

    if (block_size == 0)
      return 1;

    if (!cursor_skip(cur, block_size))
      return 0;
  }
}

static int append_bytes(uint8_t **buf, size_t *size, size_t *cap,
  const uint8_t *src, size_t count)
{
  uint8_t *next;
  size_t next_cap;

  if (count > ((size_t)-1) - *size)
    return 0;

  if (*size + count <= *cap)
  {
    memcpy(*buf + *size, src, count);
    *size += count;
    return 1;
  }

  next_cap = *cap == 0 ? 1024 : *cap;

  while (next_cap < *size + count)
  {
    if (next_cap > ((size_t)-1) / 2)
      return 0;

    next_cap *= 2;
  }

  next = (uint8_t *)realloc(*buf, next_cap);

  if (next == NULL)
    return 0;

  *buf = next;
  *cap = next_cap;
  memcpy(*buf + *size, src, count);
  *size += count;
  return 1;
}

static int read_sub_blocks(GifCursor *cur, uint8_t **out, size_t *out_size)
{
  uint8_t *buf = NULL;
  size_t size = 0;
  size_t cap = 0;
  uint8_t block_size;

  for (;;)
  {
    if (!cursor_read_u8(cur, &block_size))
    {
      free(buf);
      return 0;
    }

    if (block_size == 0)
      break;

    if (block_size > cur->size - cur->pos)
    {
      free(buf);
      return 0;
    }

    if (!append_bytes(&buf, &size, &cap, cur->data + cur->pos, block_size))
    {
      free(buf);
      return 0;
    }

    cur->pos += block_size;
  }

  *out = buf;
  *out_size = size;
  return 1;
}

static int read_code(BitReader *reader, int code_size)
{
  int code = 0;
  int bit;

  if (code_size <= 0 || code_size > 12)
    return -1;

  if ((size_t)code_size > reader->size * 8 - reader->bit_pos)
    return -1;

  for (bit = 0; bit < code_size; ++bit)
  {
    size_t byte_pos = reader->bit_pos / 8;
    int bit_pos = (int)(reader->bit_pos % 8);

    if ((reader->data[byte_pos] >> bit_pos) & 1)
      code |= 1 << bit;

    reader->bit_pos++;
  }

  return code;
}

static int lzw_decode(const uint8_t *data, size_t data_size, int min_code_size,
  uint8_t *out, size_t expected_size, char *err, size_t err_size)
{
  BitReader reader;
  uint16_t prefix[GIF2MAP_MAX_GIF_CODES];
  uint8_t suffix[GIF2MAP_MAX_GIF_CODES];
  uint8_t stack[GIF2MAP_MAX_GIF_CODES];
  int clear_code;
  int end_code;
  int next_code;
  int code_size;
  int old_code = -1;
  int first_char = 0;
  size_t out_pos = 0;
  int i;

  if (min_code_size < 2 || min_code_size > 8)
  {
    snprintf(err, err_size, "unsupported GIF LZW code size %d", min_code_size);
    return 0;
  }

  clear_code = 1 << min_code_size;
  end_code = clear_code + 1;
  next_code = clear_code + 2;
  code_size = min_code_size + 1;

  for (i = 0; i < clear_code; ++i)
  {
    prefix[i] = 0xffff;
    suffix[i] = (uint8_t)i;
  }

  reader.data = data;
  reader.size = data_size;
  reader.bit_pos = 0;

  while (out_pos < expected_size)
  {
    int code = read_code(&reader, code_size);
    int in_code;
    int stack_len = 0;

    if (code < 0)
    {
      snprintf(err, err_size, "truncated GIF LZW stream");
      return 0;
    }

    if (code == clear_code)
    {
      next_code = clear_code + 2;
      code_size = min_code_size + 1;
      old_code = -1;
      continue;
    }

    if (code == end_code)
      break;

    in_code = code;

    if (code >= next_code)
    {
      if (old_code < 0 || code != next_code)
      {
        snprintf(err, err_size, "invalid GIF LZW code %d", code);
        return 0;
      }

      stack[stack_len++] = (uint8_t)first_char;
      code = old_code;
    }

    while (code >= clear_code)
    {
      if (code >= next_code || stack_len >= GIF2MAP_MAX_GIF_CODES - 1)
      {
        snprintf(err, err_size, "invalid GIF LZW dictionary chain");
        return 0;
      }

      stack[stack_len++] = suffix[code];
      code = prefix[code];
    }

    first_char = code;
    stack[stack_len++] = (uint8_t)first_char;

    while (stack_len > 0)
    {
      if (out_pos >= expected_size)
      {
        snprintf(err, err_size, "GIF LZW stream decoded too many pixels");
        return 0;
      }

      out[out_pos++] = stack[--stack_len];
    }

    if (old_code >= 0 && next_code < GIF2MAP_MAX_GIF_CODES)
    {
      prefix[next_code] = (uint16_t)old_code;
      suffix[next_code] = (uint8_t)first_char;
      next_code++;

      if (next_code == (1 << code_size) && code_size < 12)
        code_size++;
    }

    old_code = in_code;
  }

  if (out_pos != expected_size)
  {
    snprintf(err, err_size, "GIF image decoded %lu of %lu pixels",
      (unsigned long)out_pos, (unsigned long)expected_size);
    return 0;
  }

  return 1;
}

static void blit_decoded_image(GifImage *image, const uint8_t *decoded,
  uint16_t left, uint16_t top, uint16_t width, uint16_t height, int interlaced)
{
  static const uint8_t pass_start[4] = {0, 4, 2, 1};
  static const uint8_t pass_step[4] = {8, 8, 4, 2};
  size_t src = 0;

  if (!interlaced)
  {
    uint16_t y;

    for (y = 0; y < height; ++y)
    {
      uint16_t dst_y = (uint16_t)(top + y);

      if (dst_y < image->height && left < image->width)
      {
        uint16_t copy = width;

        if (left + copy > image->width)
          copy = (uint16_t)(image->width - left);

        memcpy(image->pixels + dst_y * image->width + left,
          decoded + src, copy);
      }

      src += width;
    }

    return;
  }

  for (int pass = 0; pass < 4; ++pass)
  {
    uint16_t row;

    for (row = pass_start[pass]; row < height; row += pass_step[pass])
    {
      uint16_t dst_y = (uint16_t)(top + row);

      if (dst_y < image->height && left < image->width)
      {
        uint16_t copy = width;

        if (left + copy > image->width)
          copy = (uint16_t)(image->width - left);

        memcpy(image->pixels + dst_y * image->width + left,
          decoded + src, copy);
      }

      src += width;
    }
  }
}

static int read_file(const char *path, FileBuffer *out, char *err,
  size_t err_size)
{
  FILE *f;
  long size;
  uint8_t *data;

  out->data = NULL;
  out->size = 0;

  f = fopen(path, "rb");

  if (f == NULL)
  {
    snprintf(err, err_size, "could not open input file: %s", path);
    return 0;
  }

  if (fseek(f, 0, SEEK_END) != 0)
  {
    fclose(f);
    snprintf(err, err_size, "could not seek input file: %s", path);
    return 0;
  }

  size = ftell(f);

  if (size < 0)
  {
    fclose(f);
    snprintf(err, err_size, "could not determine input file size: %s", path);
    return 0;
  }

  if (fseek(f, 0, SEEK_SET) != 0)
  {
    fclose(f);
    snprintf(err, err_size, "could not rewind input file: %s", path);
    return 0;
  }

  data = (uint8_t *)malloc((size_t)size == 0 ? 1 : (size_t)size);

  if (data == NULL)
  {
    fclose(f);
    snprintf(err, err_size, "out of memory reading input file");
    return 0;
  }

  if (fread(data, 1, (size_t)size, f) != (size_t)size)
  {
    free(data);
    fclose(f);
    snprintf(err, err_size, "could not read input file: %s", path);
    return 0;
  }

  fclose(f);
  out->data = data;
  out->size = (size_t)size;
  return 1;
}

static int decode_gif(const char *path, GifImage *image, char *err,
  size_t err_size)
{
  FileBuffer file;
  GifCursor cur;
  uint8_t packed;
  int image_seen = 0;

  image->width = 0;
  image->height = 0;
  image->pixels = NULL;

  if (!read_file(path, &file, err, err_size))
    return 0;

  if (file.size < 13 ||
    (memcmp(file.data, "GIF87a", 6) != 0 &&
    memcmp(file.data, "GIF89a", 6) != 0))
  {
    free(file.data);
    snprintf(err, err_size, "input is not a GIF87a/GIF89a file");
    return 0;
  }

  image->width = read_le16(file.data + 6);
  image->height = read_le16(file.data + 8);
  packed = file.data[10];

  if (image->width == 0 || image->height == 0 ||
    (size_t)image->width > ((size_t)-1) / image->height)
  {
    free(file.data);
    snprintf(err, err_size, "invalid GIF logical screen size");
    return 0;
  }

  image->pixels = (uint8_t *)calloc((size_t)image->width * image->height, 1);

  if (image->pixels == NULL)
  {
    free(file.data);
    snprintf(err, err_size, "out of memory allocating GIF pixels");
    return 0;
  }

  cur.data = file.data;
  cur.size = file.size;
  cur.pos = 13;

  if (packed & 0x80)
  {
    size_t gct_size = (size_t)3 * (1u << ((packed & 0x07) + 1));

    if (!cursor_skip(&cur, gct_size))
    {
      free(image->pixels);
      free(file.data);
      snprintf(err, err_size, "truncated GIF global color table");
      return 0;
    }
  }

  while (cur.pos < cur.size)
  {
    uint8_t marker;

    if (!cursor_read_u8(&cur, &marker))
      break;

    if (marker == 0x3b)
      break;

    if (marker == 0x21)
    {
      uint8_t label;

      if (!cursor_read_u8(&cur, &label) || !skip_sub_blocks(&cur))
      {
        free(image->pixels);
        free(file.data);
        snprintf(err, err_size, "truncated GIF extension block");
        return 0;
      }

      continue;
    }

    if (marker == 0x2c)
    {
      uint16_t left;
      uint16_t top;
      uint16_t width;
      uint16_t height;
      uint8_t image_packed;
      uint8_t min_code_size;
      uint8_t *compressed = NULL;
      uint8_t *decoded = NULL;
      size_t compressed_size = 0;
      size_t decoded_size;

      if (cur.size - cur.pos < 9)
      {
        free(image->pixels);
        free(file.data);
        snprintf(err, err_size, "truncated GIF image descriptor");
        return 0;
      }

      left = read_le16(cur.data + cur.pos);
      top = read_le16(cur.data + cur.pos + 2);
      width = read_le16(cur.data + cur.pos + 4);
      height = read_le16(cur.data + cur.pos + 6);
      image_packed = cur.data[cur.pos + 8];
      cur.pos += 9;

      if (width == 0 || height == 0 ||
        (size_t)width > ((size_t)-1) / height)
      {
        free(image->pixels);
        free(file.data);
        snprintf(err, err_size, "invalid GIF image size");
        return 0;
      }

      if (image_packed & 0x80)
      {
        size_t lct_size = (size_t)3 * (1u << ((image_packed & 0x07) + 1));

        if (!cursor_skip(&cur, lct_size))
        {
          free(image->pixels);
          free(file.data);
          snprintf(err, err_size, "truncated GIF local color table");
          return 0;
        }
      }

      if (!cursor_read_u8(&cur, &min_code_size) ||
        !read_sub_blocks(&cur, &compressed, &compressed_size))
      {
        free(image->pixels);
        free(file.data);
        snprintf(err, err_size, "truncated GIF image data");
        return 0;
      }

      decoded_size = (size_t)width * height;
      decoded = (uint8_t *)malloc(decoded_size);

      if (decoded == NULL)
      {
        free(compressed);
        free(image->pixels);
        free(file.data);
        snprintf(err, err_size, "out of memory decoding GIF image");
        return 0;
      }

      if (!lzw_decode(compressed, compressed_size, min_code_size, decoded,
        decoded_size, err, err_size))
      {
        free(decoded);
        free(compressed);
        free(image->pixels);
        free(file.data);
        return 0;
      }

      blit_decoded_image(image, decoded, left, top, width, height,
        (image_packed & 0x40) != 0);

      free(decoded);
      free(compressed);
      image_seen = 1;
      break;
    }

    free(image->pixels);
    free(file.data);
    snprintf(err, err_size, "unexpected GIF block marker 0x%02x", marker);
    return 0;
  }

  free(file.data);

  if (!image_seen)
  {
    free(image->pixels);
    image->pixels = NULL;
    snprintf(err, err_size, "GIF contains no image block");
    return 0;
  }

  return 1;
}

static uint8_t image_pixel(const GifImage *image, uint16_t x, uint16_t y)
{
  return image->pixels[(size_t)y * image->width + x];
}

static uint8_t logical_pixel(const GifImage *image,
  const uint8_t palette_inverse[256], uint16_t x, uint16_t y)
{
  return palette_inverse[image_pixel(image, x, y)];
}

static int load_tile_dict(const GifImage *image,
  const uint8_t palette_inverse[256], uint16_t origin_x, uint16_t origin_y,
  uint8_t textures[GIF2MAP_TILE_DICTIONARY_SIZE],
  uint8_t heights[GIF2MAP_TILE_DICTIONARY_SIZE], char *err, size_t err_size)
{
  uint8_t i;

  for (i = 0; i < GIF2MAP_TILE_DICTIONARY_SIZE; ++i)
  {
    uint8_t texture = logical_pixel(image, palette_inverse,
      (uint16_t)(origin_x + i), (uint16_t)(origin_y + 31));
    uint8_t height = 0;
    uint8_t j;

    if (texture > 7)
    {
      snprintf(err, err_size, "texture index %u is higher than 7", texture);
      return 0;
    }

    for (j = 0; j < 31; ++j)
    {
      if (logical_pixel(image, palette_inverse, (uint16_t)(origin_x + i),
        (uint16_t)(origin_y + 30 - j)) == 7)
        break;

      height++;
    }

    textures[i] = texture;
    heights[i] = height;
  }

  return 1;
}

static const char *element_name(uint8_t type)
{
  static const char *names[] =
  {
    "NONE",
    "BARREL",
    "HEALTH",
    "BULLETS",
    "ROCKETS",
    "PLASMA",
    "TREE",
    "FINISH",
    "TELEPORTER",
    "TERMINAL",
    "COLUMN",
    "RUIN",
    "LAMP",
    "CARD0",
    "CARD1",
    "CARD2",
    "LOCK0",
    "LOCK1",
    "LOCK2",
    "BLOCKER",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "MONSTER_SPIDER",
    "MONSTER_DESTROYER",
    "MONSTER_WARRIOR",
    "MONSTER_PLASMABOT",
    "MONSTER_ENDER",
    "MONSTER_TURRET",
    "MONSTER_EXPLODER"
  };

  if (type >= sizeof(names) / sizeof(names[0]))
    return NULL;

  return names[type];
}

static int parse_gif_map(const GifImage *image, Gif2MapLevel *level,
  char *err, size_t err_size)
{
  uint8_t palette_inverse[256];
  uint8_t floor_textures[GIF2MAP_TILE_DICTIONARY_SIZE];
  uint8_t floor_heights[GIF2MAP_TILE_DICTIONARY_SIZE];
  uint8_t ceil_textures[GIF2MAP_TILE_DICTIONARY_SIZE];
  uint8_t ceil_heights[GIF2MAP_TILE_DICTIONARY_SIZE];
  uint16_t x;
  uint16_t y;
  int player_found = 0;

  memset(level, 0, sizeof(*level));
  memset(palette_inverse, 0, sizeof(palette_inverse));

  if (image->width != GIF2MAP_GIF_WIDTH || image->height != GIF2MAP_GIF_HEIGHT)
  {
    snprintf(err, err_size, "expected %ux%u GIF, got %ux%u",
      GIF2MAP_GIF_WIDTH, GIF2MAP_GIF_HEIGHT, image->width, image->height);
    return 0;
  }

  x = 5;
  y = 69;

  for (uint16_t i = 0; i < 256; ++i)
  {
    if (i % 64 == 0)
    {
      x = 5;
      y++;
    }

    palette_inverse[image_pixel(image, x, y)] = (uint8_t)i;
    x++;
  }

  if (!load_tile_dict(image, palette_inverse, 5, 37, floor_textures,
    floor_heights, err, err_size))
    return 0;

  if (!load_tile_dict(image, palette_inverse, 5, 5, ceil_textures,
    ceil_heights, err, err_size))
    return 0;

  for (uint8_t i = 0; i < GIF2MAP_TILE_DICTIONARY_SIZE; ++i)
  {
    level->tile_dictionary[i] = tile_definition(floor_heights[i],
      ceil_heights[i], floor_textures[i], ceil_textures[i]);
  }

  level->floor_color = logical_pixel(image, palette_inverse, 41, 122);
  level->ceiling_color = logical_pixel(image, palette_inverse, 41, 118);
  level->background_image = logical_pixel(image, palette_inverse, 41, 126);
  level->door_texture_index = logical_pixel(image, palette_inverse, 41, 130);

  for (uint8_t i = 0; i < GIF2MAP_TEXTURE_COUNT; ++i)
  {
    level->texture_indices[i] = logical_pixel(image, palette_inverse,
      (uint16_t)(41 + 4 * i), 114);
  }

  for (y = 0; y < GIF2MAP_MAP_SIZE; ++y)
  {
    for (x = 0; x < GIF2MAP_MAP_SIZE; ++x)
    {
      uint8_t n = logical_pixel(image, palette_inverse,
        (uint16_t)(70 + x), (uint16_t)(5 + y));
      level->map_array[(size_t)y * GIF2MAP_MAP_SIZE + (63 - x)] = n;
    }
  }

  for (y = 0; y < GIF2MAP_MAP_SIZE; ++y)
  {
    for (x = 0; x < GIF2MAP_MAP_SIZE; ++x)
    {
      uint8_t n = logical_pixel(image, palette_inverse,
        (uint16_t)(70 + x), (uint16_t)(70 + y));

      if (n < 39)
      {
        if (element_name(n) == NULL)
        {
          snprintf(err, err_size, "invalid level element type %u at %u,%u",
            n, (unsigned)x, (unsigned)y);
          return 0;
        }

        if (level->element_count >= GIF2MAP_MAX_LEVEL_ELEMENTS)
        {
          snprintf(err, err_size, "more than %u level elements",
            GIF2MAP_MAX_LEVEL_ELEMENTS);
          return 0;
        }

        level->elements[level->element_count].type = n;
        level->elements[level->element_count].x = (uint8_t)(63 - x);
        level->elements[level->element_count].y = (uint8_t)y;
        level->element_count++;
      }
      else if (n >= 240)
      {
        if (player_found)
        {
          snprintf(err, err_size, "multiple player starting positions");
          return 0;
        }

        level->player_start[0] = (uint8_t)(63 - x);
        level->player_start[1] = (uint8_t)y;
        level->player_start[2] = (uint8_t)((n - 240) * 16);
        player_found = 1;
      }
    }
  }

  if (!player_found)
  {
    snprintf(err, err_size, "player starting position not specified");
    return 0;
  }

  return 1;
}

Gif2MapResult gif2map_load_gif(const char *input_path, Gif2MapLevel *level)
{
  GifImage image;
  char err[256];

  if (input_path == NULL || input_path[0] == '\0')
    return result_make(0, 2, "input path is required");

  if (level == NULL)
    return result_make(0, 2, "output level pointer is required");

  if (!decode_gif(input_path, &image, err, sizeof(err)))
    return result_make(0, 1, "%s", err);

  if (!parse_gif_map(&image, level, err, sizeof(err)))
  {
    free(image.pixels);
    return result_make(0, 1, "%s", err);
  }

  free(image.pixels);
  return result_make(1, 0, NULL);
}

static int put_csv_u8(FILE *out, const uint8_t *values, size_t count)
{
  size_t i;

  for (i = 0; i < count; ++i)
  {
    if (fprintf(out, "%u%s", values[i], i + 1 < count ? "," : "") < 0)
      return 0;
  }

  return 1;
}

Gif2MapResult gif2map_emit_c(FILE *out, const Gif2MapLevel *level,
  const char *symbol_name)
{
  size_t i;

  if (out == NULL || level == NULL)
    return result_make(0, 2, "output stream and level are required");

  if (symbol_name != NULL && symbol_name[0] != '\0')
  {
    if (fprintf(out, "SFG_Level %s =\n", symbol_name) < 0)
      return result_make(0, 1, "failed writing C output");
  }

  if (fprintf(out, "{\n  {\n") < 0)
    return result_make(0, 1, "failed writing C output");

  for (i = 0; i < GIF2MAP_MAP_SIZE * GIF2MAP_MAP_SIZE; ++i)
  {
    if (i % GIF2MAP_MAP_SIZE == 0 && fprintf(out, "    ") < 0)
      return result_make(0, 1, "failed writing C output");

    if (fprintf(out, "%u%s", level->map_array[i],
      i + 1 < GIF2MAP_MAP_SIZE * GIF2MAP_MAP_SIZE ? "," : "") < 0)
      return result_make(0, 1, "failed writing C output");

    if (i % GIF2MAP_MAP_SIZE == GIF2MAP_MAP_SIZE - 1 &&
      fprintf(out, "\n") < 0)
      return result_make(0, 1, "failed writing C output");
  }

  if (fprintf(out, "  },\n  {\n") < 0)
    return result_make(0, 1, "failed writing C output");

  for (i = 0; i < GIF2MAP_TILE_DICTIONARY_SIZE; ++i)
  {
    uint16_t t = level->tile_dictionary[i];
    uint8_t floor_h = (uint8_t)(t & 0x1f);
    uint8_t floor_t = (uint8_t)((t >> 5) & 0x07);
    uint8_t ceil_h = (uint8_t)((t >> 8) & 0x1f);
    uint8_t ceil_t = (uint8_t)((t >> 13) & 0x07);

    if (i % 4 == 0 && fprintf(out, "    ") < 0)
      return result_make(0, 1, "failed writing C output");

    if (fprintf(out, "SFG_TD(%u,%u,%u,%u)%s", floor_h, ceil_h,
      floor_t, ceil_t, i + 1 < GIF2MAP_TILE_DICTIONARY_SIZE ? "," : "") < 0)
      return result_make(0, 1, "failed writing C output");

    if (i % 4 == 3 && fprintf(out, "\n") < 0)
      return result_make(0, 1, "failed writing C output");
  }

  if (fprintf(out, "  },\n  {") < 0 ||
    !put_csv_u8(out, level->texture_indices, GIF2MAP_TEXTURE_COUNT) ||
    fprintf(out, "},\n  %u,\n  %u,\n  %u,\n  {%u,%u,%u},\n  %u,\n  {\n",
      level->door_texture_index, level->floor_color, level->ceiling_color,
      level->player_start[0], level->player_start[1],
      level->player_start[2], level->background_image) < 0)
    return result_make(0, 1, "failed writing C output");

  for (i = 0; i < GIF2MAP_MAX_LEVEL_ELEMENTS; ++i)
  {
    const char *name = element_name(level->elements[i].type);

    if (name == NULL)
      name = "NONE";

    if (fprintf(out, "    {SFG_LEVEL_ELEMENT_%s,{%u,%u}}%s\n", name,
      level->elements[i].x, level->elements[i].y,
      i + 1 < GIF2MAP_MAX_LEVEL_ELEMENTS ? "," : "") < 0)
      return result_make(0, 1, "failed writing C output");
  }

  if (fprintf(out, "  }\n}") < 0)
    return result_make(0, 1, "failed writing C output");

  if (symbol_name != NULL && symbol_name[0] != '\0')
  {
    if (fprintf(out, ";\n") < 0)
      return result_make(0, 1, "failed writing C output");
  }
  else if (fprintf(out, "\n") < 0)
    return result_make(0, 1, "failed writing C output");

  return result_make(1, 0, NULL);
}

Gif2MapResult gif2map_emit_json(FILE *out, const Gif2MapLevel *level)
{
  size_t i;

  if (out == NULL || level == NULL)
    return result_make(0, 2, "output stream and level are required");

  if (fprintf(out, "{\n\"map_array\":[") < 0)
    return result_make(0, 1, "failed writing JSON output");

  for (i = 0; i < GIF2MAP_MAP_SIZE * GIF2MAP_MAP_SIZE; ++i)
  {
    if (fprintf(out, "%u%s", level->map_array[i],
      i + 1 < GIF2MAP_MAP_SIZE * GIF2MAP_MAP_SIZE ? "," : "") < 0)
      return result_make(0, 1, "failed writing JSON output");
  }

  if (fprintf(out, "],\n\"tile_dictionary\":[") < 0)
    return result_make(0, 1, "failed writing JSON output");

  for (i = 0; i < GIF2MAP_TILE_DICTIONARY_SIZE; ++i)
  {
    if (fprintf(out, "%u%s", level->tile_dictionary[i],
      i + 1 < GIF2MAP_TILE_DICTIONARY_SIZE ? "," : "") < 0)
      return result_make(0, 1, "failed writing JSON output");
  }

  if (fprintf(out, "],\n\"texture_indices\":[") < 0 ||
    !put_csv_u8(out, level->texture_indices, GIF2MAP_TEXTURE_COUNT) ||
    fprintf(out, "],\n\"door_texture_index\":%u,\n\"floor_color\":%u,"
      "\n\"ceiling_color\":%u,\n\"player_start\":[%u,%u,%u],"
      "\n\"background_image\":%u,\n\"element_count\":%lu,"
      "\n\"elements\":[",
      level->door_texture_index, level->floor_color, level->ceiling_color,
      level->player_start[0], level->player_start[1],
      level->player_start[2], level->background_image,
      (unsigned long)level->element_count) < 0)
    return result_make(0, 1, "failed writing JSON output");

  for (i = 0; i < GIF2MAP_MAX_LEVEL_ELEMENTS; ++i)
  {
    if (fprintf(out, "{\"type\":%u,\"x\":%u,\"y\":%u}%s",
      level->elements[i].type, level->elements[i].x, level->elements[i].y,
      i + 1 < GIF2MAP_MAX_LEVEL_ELEMENTS ? "," : "") < 0)
      return result_make(0, 1, "failed writing JSON output");
  }

  if (fprintf(out, "]\n}\n") < 0)
    return result_make(0, 1, "failed writing JSON output");

  return result_make(1, 0, NULL);
}

Gif2MapResult gif2map_convert(const Gif2MapConfig *cfg)
{
  Gif2MapLevel level;
  Gif2MapResult result;
  FILE *out = NULL;
  const char *format;
  int close_output = 0;

  if (cfg == NULL)
    return result_make(0, 2, "configuration is required");

  if (cfg->input_path == NULL || cfg->input_path[0] == '\0')
    return result_make(0, 2, "input path is required");

  format = cfg->format == NULL || cfg->format[0] == '\0' ? "c" : cfg->format;

  result = gif2map_load_gif(cfg->input_path, &level);

  if (!result.ok)
    return result;

  if (cfg->write_stdout || cfg->output_path == NULL || cfg->output_path[0] == '\0')
    out = stdout;
  else
  {
    out = fopen(cfg->output_path, "w");

    if (out == NULL)
      return result_make(0, 1, "could not open output file: %s",
        cfg->output_path);

    close_output = 1;
  }

  if (strcmp(format, "c") == 0)
    result = gif2map_emit_c(out, &level, cfg->symbol_name);
  else if (strcmp(format, "json") == 0)
    result = gif2map_emit_json(out, &level);
  else
    result = result_make(0, 2, "unsupported output format: %s", format);

  if (close_output && fclose(out) != 0 && result.ok)
    result = result_make(0, 1, "failed closing output file: %s",
      cfg->output_path);

  (void)cfg->strict;
  return result;
}
