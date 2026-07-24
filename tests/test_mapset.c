/**
  @file test_mapset.c

  Unit tests for core/mapset.h capability checking logic.

  Compile:
    cc -std=c99 -Icore -o tests/test_mapset tests/test_mapset.c && ./tests/test_mapset

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Simulate a PC platform with most capabilities */
#define LHR_PLATFORM_CAPABILITIES ( \
  LHR_CAP_KEYS_16 | LHR_CAP_MOUSE | LHR_CAP_AUDIO_PCM | \
  LHR_CAP_AUDIO_BYTEBEAT | LHR_CAP_FILESYSTEM | LHR_CAP_SAVE_FILE )

#define LHR_LOG(s) fprintf(stderr, "%s\n", s)

#include "mapset.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { \
  tests_run++; \
  printf("  TEST: %-50s ", #name); \
} while(0)

#define PASS() do { tests_passed++; printf("[PASS]\n"); } while(0)
#define FAIL(msg) do { printf("[FAIL] %s\n", msg); } while(0)

/* ---------- helpers ---------- */

static LHR_MapsetMeta make_meta(const char *name, uint32_t required)
{
  LHR_MapsetMeta m;
  memset(&m, 0, sizeof(m));
  strncpy(m.name, name, LHR_MAPSET_NAME_MAX - 1);
  m.requiredCapabilities = required;
  m.levelCount = 1;
  m.version = 1;
  return m;
}

/* ---------- tests ---------- */

void test_compatible_mapset_accepted(void)
{
  TEST(compatible_mapset_accepted);
  LHR_MapsetMeta m = make_meta("basic", LHR_CAP_KEYS_16 | LHR_CAP_FILESYSTEM);
  if (LHR_mapsetCompatible(&m) == 1 && LHR_mapsetMissingCaps(&m) == 0)
    PASS();
  else
    FAIL("should be compatible");
}

void test_incompatible_mapset_rejected(void)
{
  TEST(incompatible_mapset_rejected);
  /* Require dual screen — our simulated platform doesn't have it */
  LHR_MapsetMeta m = make_meta("dual_screen_mod",
    LHR_CAP_KEYS_16 | LHR_CAP_DUAL_SCREEN);
  if (LHR_mapsetCompatible(&m) == 0)
    PASS();
  else
    FAIL("should be incompatible (dual screen missing)");
}

void test_missing_caps_reports_correct_bits(void)
{
  TEST(missing_caps_reports_correct_bits);
  LHR_MapsetMeta m = make_meta("midi_mod",
    LHR_CAP_AUDIO_MIDI | LHR_CAP_DUAL_SCREEN | LHR_CAP_KEYS_16);
  uint32_t missing = LHR_mapsetMissingCaps(&m);
  /* We defined platform WITHOUT MIDI and WITHOUT DUAL_SCREEN */
  if ((missing & LHR_CAP_AUDIO_MIDI) && (missing & LHR_CAP_DUAL_SCREEN)
      && !(missing & LHR_CAP_KEYS_16))
    PASS();
  else
    FAIL("wrong missing bits");
}

void test_no_requirements_always_compatible(void)
{
  TEST(no_requirements_always_compatible);
  LHR_MapsetMeta m = make_meta("empty", 0);
  if (LHR_mapsetCompatible(&m) == 1)
    PASS();
  else
    FAIL("zero requirements should always pass");
}

void test_optional_caps_dont_block(void)
{
  TEST(optional_caps_dont_block);
  LHR_MapsetMeta m = make_meta("optional_midi", LHR_CAP_KEYS_16);
  m.optionalCapabilities = LHR_CAP_AUDIO_MIDI; /* optional, not required */
  if (LHR_mapsetCompatible(&m) == 1)
    PASS();
  else
    FAIL("optional caps should not block loading");
}

void test_all_platform_caps_satisfied(void)
{
  TEST(all_platform_caps_satisfied);
  /* Require exactly what the platform has */
  LHR_MapsetMeta m = make_meta("full_pc",
    LHR_CAP_KEYS_16 | LHR_CAP_MOUSE | LHR_CAP_AUDIO_PCM |
    LHR_CAP_AUDIO_BYTEBEAT | LHR_CAP_FILESYSTEM | LHR_CAP_SAVE_FILE);
  if (LHR_mapsetCompatible(&m) == 1 && LHR_mapsetMissingCaps(&m) == 0)
    PASS();
  else
    FAIL("all caps present, should pass");
}

#if LHR_DYNAMIC_MAPSETS
void test_load_bad_magic_rejected(void)
{
  TEST(load_bad_magic_rejected);
  const char *path = "/tmp/lhr_test_bad.lhrm";
  FILE *f = fopen(path, "wb");
  uint32_t bad_magic = 0xDEADBEEF;
  uint32_t ver = LHR_MAPSET_VERSION;
  LHR_MapsetMeta m = make_meta("bad", 0);
  fwrite(&bad_magic, 4, 1, f);
  fwrite(&ver, 4, 1, f);
  fwrite(&m, sizeof(m), 1, f);
  fclose(f);

  LHR_MapsetFileHeader hdr;
  if (LHR_loadMapsetHeader(path, &hdr) == 0)
    PASS();
  else
    FAIL("bad magic should be rejected");
}

void test_load_valid_header_accepted(void)
{
  TEST(load_valid_header_accepted);
  const char *path = "/tmp/lhr_test_good.lhrm";
  FILE *f = fopen(path, "wb");
  uint32_t magic = LHR_MAPSET_MAGIC;
  uint32_t ver = LHR_MAPSET_VERSION;
  LHR_MapsetMeta m = make_meta("good", LHR_CAP_KEYS_16);
  fwrite(&magic, 4, 1, f);
  fwrite(&ver, 4, 1, f);
  fwrite(&m, sizeof(m), 1, f);
  fclose(f);

  LHR_MapsetFileHeader hdr;
  if (LHR_loadMapsetHeader(path, &hdr) == 1
      && hdr.magic == LHR_MAPSET_MAGIC
      && strcmp(hdr.meta.name, "good") == 0)
    PASS();
  else
    FAIL("valid header should be accepted");
}

void test_tryload_rejects_incompatible(void)
{
  TEST(tryload_rejects_incompatible);
  const char *path = "/tmp/lhr_test_incompat.lhrm";
  FILE *f = fopen(path, "wb");
  uint32_t magic = LHR_MAPSET_MAGIC;
  uint32_t ver = LHR_MAPSET_VERSION;
  LHR_MapsetMeta m = make_meta("needs_dual", LHR_CAP_DUAL_SCREEN);
  fwrite(&magic, 4, 1, f);
  fwrite(&ver, 4, 1, f);
  fwrite(&m, sizeof(m), 1, f);
  fclose(f);

  LHR_MapsetFileHeader hdr;
  if (LHR_tryLoadMapset(path, &hdr) == 0)
    PASS();
  else
    FAIL("incompatible mapset should be rejected by tryload");
}
#endif /* LHR_DYNAMIC_MAPSETS */

int main(void)
{
  printf("=== Mapset Capability Tests ===\n");

  test_compatible_mapset_accepted();
  test_incompatible_mapset_rejected();
  test_missing_caps_reports_correct_bits();
  test_no_requirements_always_compatible();
  test_optional_caps_dont_block();
  test_all_platform_caps_satisfied();

#if LHR_DYNAMIC_MAPSETS
  printf("\n=== Dynamic Loading Tests ===\n");
  test_load_bad_magic_rejected();
  test_load_valid_header_accepted();
  test_tryload_rejects_incompatible();
#else
  printf("\n(Dynamic loading tests skipped: LHR_DYNAMIC_MAPSETS=0)\n");
#endif

  printf("\n%d/%d tests passed.\n", tests_passed, tests_run);
  return (tests_passed == tests_run) ? 0 : 1;
}
