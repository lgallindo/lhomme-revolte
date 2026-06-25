#include "unity.h"
#include "gif2map.h"

#include <string.h>

static uint16_t td(uint8_t floor_h, uint8_t ceil_h, uint8_t floor_t,
  uint8_t ceil_t)
{
  return (uint16_t)
    ((floor_h & 0x1f) |
    ((floor_t & 0x07) << 5) |
    ((ceil_h & 0x1f) << 8) |
    ((ceil_t & 0x07) << 13));
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_loads_level0_core_fields(void)
{
  Gif2MapLevel level;
  uint8_t expected_textures[GIF2MAP_TEXTURE_COUNT] = {1, 3, 12, 14, 5, 11, 6};
  Gif2MapResult result = gif2map_load_gif("../../assets/level0.gif", &level);

  TEST_ASSERT_TRUE_MESSAGE(result.ok, result.error_message);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_textures, level.texture_indices,
    GIF2MAP_TEXTURE_COUNT);
  TEST_ASSERT_EQUAL_UINT8(13, level.door_texture_index);
  TEST_ASSERT_EQUAL_UINT8(20, level.floor_color);
  TEST_ASSERT_EQUAL_UINT8(3, level.ceiling_color);
  TEST_ASSERT_EQUAL_UINT8(52, level.player_start[0]);
  TEST_ASSERT_EQUAL_UINT8(55, level.player_start[1]);
  TEST_ASSERT_EQUAL_UINT8(80, level.player_start[2]);
  TEST_ASSERT_EQUAL_UINT8(0, level.background_image);
  TEST_ASSERT_EQUAL_UINT16(td(7, 31, 1, 0), level.tile_dictionary[0]);
  TEST_ASSERT_EQUAL_UINT16(td(26, 31, 0, 0), level.tile_dictionary[1]);
  TEST_ASSERT_EQUAL_UINT8(1, level.map_array[0]);
  TEST_ASSERT_EQUAL_UINT8(23, level.map_array[7]);
  TEST_ASSERT_EQUAL_UINT8(0, level.map_array[39]);
}

void test_loads_level0_elements_and_padding(void)
{
  Gif2MapLevel level;
  Gif2MapResult result = gif2map_load_gif("../../assets/level0.gif", &level);

  TEST_ASSERT_TRUE_MESSAGE(result.ok, result.error_message);
  TEST_ASSERT_GREATER_THAN_UINT64(0, level.element_count);
  TEST_ASSERT_LESS_OR_EQUAL_UINT64(GIF2MAP_MAX_LEVEL_ELEMENTS,
    level.element_count);
  TEST_ASSERT_EQUAL_UINT8(3, level.elements[0].type);
  TEST_ASSERT_EQUAL_UINT8(31, level.elements[0].x);
  TEST_ASSERT_EQUAL_UINT8(2, level.elements[0].y);
  TEST_ASSERT_EQUAL_UINT8(3, level.elements[1].type);
  TEST_ASSERT_EQUAL_UINT8(29, level.elements[1].x);
  TEST_ASSERT_EQUAL_UINT8(2, level.elements[1].y);

  if (level.element_count < GIF2MAP_MAX_LEVEL_ELEMENTS)
  {
    TEST_ASSERT_EQUAL_UINT8(0, level.elements[level.element_count].type);
    TEST_ASSERT_EQUAL_UINT8(0, level.elements[level.element_count].x);
    TEST_ASSERT_EQUAL_UINT8(0, level.elements[level.element_count].y);
  }
}

void test_emit_c_contains_sfg_level_initializer(void)
{
  FILE *out;
  char buffer[8192];
  Gif2MapLevel level;
  Gif2MapResult result = gif2map_load_gif("../../assets/level0.gif", &level);

  TEST_ASSERT_TRUE_MESSAGE(result.ok, result.error_message);

  out = tmpfile();
  TEST_ASSERT_NOT_NULL(out);
  result = gif2map_emit_c(out, &level, "SFG_level_test");
  TEST_ASSERT_TRUE_MESSAGE(result.ok, result.error_message);
  rewind(out);

  TEST_ASSERT_NOT_NULL(fgets(buffer, sizeof(buffer), out));
  TEST_ASSERT_NOT_NULL(strstr(buffer, "SFG_Level SFG_level_test"));
  fclose(out);
}

void test_convert_rejects_missing_input(void)
{
  Gif2MapConfig cfg;
  Gif2MapResult result;

  cfg.input_path = NULL;
  cfg.output_path = NULL;
  cfg.format = "c";
  cfg.symbol_name = NULL;
  cfg.strict = 1;
  cfg.write_stdout = 0;

  result = gif2map_convert(&cfg);
  TEST_ASSERT_FALSE(result.ok);
  TEST_ASSERT_EQUAL_INT(2, result.exit_code);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_loads_level0_core_fields);
  RUN_TEST(test_loads_level0_elements_and_padding);
  RUN_TEST(test_emit_c_contains_sfg_level_initializer);
  RUN_TEST(test_convert_rejects_missing_input);
  return UNITY_END();
}
