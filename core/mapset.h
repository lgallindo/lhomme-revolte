/**
  @file mapset.h

  Mapset definition, platform capability checking, and dynamic loading gate.

  A Mapset is a self-contained bundle of levels, resources, and metadata that
  declares its hardware/platform requirements. The engine MUST refuse to load
  a mapset if the target platform does not satisfy every declared requirement.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_MAPSET_H
#define _LHR_MAPSET_H

#include <stdint.h>

/* ======================== PLATFORM CAPABILITY FLAGS ======================== */

/** Bitmask flags describing what a platform can provide. */
#define LHR_CAP_KEYS_4        (1u << 0)  /**< Platform has >= 4 action keys. */
#define LHR_CAP_KEYS_7        (1u << 1)  /**< Platform has >= 7 keys (A/B/C + dpad). */
#define LHR_CAP_KEYS_16       (1u << 2)  /**< Platform has full 16-key layout. */
#define LHR_CAP_MOUSE          (1u << 3)  /**< Platform has a pointing device. */
#define LHR_CAP_DUAL_SCREEN    (1u << 4)  /**< Platform renders to two displays. */
#define LHR_CAP_AUDIO_PCM      (1u << 5)  /**< Platform can stream PCM WAV. */
#define LHR_CAP_AUDIO_MIDI     (1u << 6)  /**< Platform has embedded MIDI synth. */
#define LHR_CAP_AUDIO_BYTEBEAT (1u << 7)  /**< Platform has procedural bytebeat. */
#define LHR_CAP_FILESYSTEM     (1u << 8)  /**< Platform can fopen/fread files. */
#define LHR_CAP_HIGH_RES       (1u << 9)  /**< Platform supports >= 640x480. */
#define LHR_CAP_COLOR_DEPTH_16 (1u << 10) /**< Platform has >= 16-bit color. */
#define LHR_CAP_SAVE_FILE      (1u << 11) /**< Platform can persist save data. */

/* ======================== MAPSET METADATA ================================= */

/**
  Magic bytes identifying a mapset file on disk.
  ASCII: "LHRM" (L'Homme Révolté Mapset).
*/
#define LHR_MAPSET_MAGIC 0x4D52484Cu

/**
  Current mapset format version. Increment when the binary layout changes.
*/
#define LHR_MAPSET_VERSION 1u

/**
  Maximum length for mapset identifiers (name, author) including NUL.
*/
#define LHR_MAPSET_NAME_MAX 64

/**
  Metadata header for a Mapset. This struct is used both at compile-time
  (baked-in mapsets) and at runtime (dynamic loading from file).
*/
typedef struct
{
  char name[LHR_MAPSET_NAME_MAX];       /**< Human-readable mapset name. */
  char author[LHR_MAPSET_NAME_MAX];     /**< Author / team. */
  uint32_t version;                      /**< Mapset content version. */
  uint32_t levelCount;                   /**< Number of levels in the mapset. */
  uint32_t requiredCapabilities;         /**< Bitmask of LHR_CAP_* flags the
                                              mapset REQUIRES to function. */
  uint32_t optionalCapabilities;         /**< Bitmask of LHR_CAP_* flags the
                                              mapset CAN USE but doesn't need. */
} LHR_MapsetMeta;

/* ======================== PLATFORM DECLARATION ============================ */

/**
  Each frontend must define this before including game.h (or this file) to
  declare what the compilation target can actually do.
  Example for PC/SDL:
    #define LHR_PLATFORM_CAPABILITIES ( \
      LHR_CAP_KEYS_16 | LHR_CAP_MOUSE | LHR_CAP_AUDIO_PCM | \
      LHR_CAP_AUDIO_MIDI | LHR_CAP_AUDIO_BYTEBEAT | \
      LHR_CAP_FILESYSTEM | LHR_CAP_HIGH_RES | LHR_CAP_SAVE_FILE )
*/
#ifndef LHR_PLATFORM_CAPABILITIES
  #define LHR_PLATFORM_CAPABILITIES 0u
#endif

/* ======================== CAPABILITY CHECKING ============================= */

/**
  Returns 1 if the platform satisfies ALL required capabilities of a mapset,
  0 otherwise. This is the gatekeeper: the engine MUST call this before loading.
*/
static inline int LHR_mapsetCompatible(const LHR_MapsetMeta *meta)
{
  return (meta->requiredCapabilities & LHR_PLATFORM_CAPABILITIES)
         == meta->requiredCapabilities;
}

/**
  Populates a human-readable error string listing the first missing capability.
  Returns the missing bitmask, or 0 if compatible.
*/
static inline uint32_t LHR_mapsetMissingCaps(const LHR_MapsetMeta *meta)
{
  return meta->requiredCapabilities & ~LHR_PLATFORM_CAPABILITIES;
}

/* ======================== CAPABILITY NAME TABLE ============================ */

/**
  Lookup table mapping each capability bit to a human-readable name.
  Useful for error messages ("mapset requires MOUSE but platform lacks it").
*/
static const char *LHR_capabilityNames[] = {
  "4-key input",         /* bit 0  */
  "7-key input",         /* bit 1  */
  "16-key input",        /* bit 2  */
  "mouse/pointer",       /* bit 3  */
  "dual screen",         /* bit 4  */
  "PCM WAV audio",       /* bit 5  */
  "MIDI audio",          /* bit 6  */
  "bytebeat audio",      /* bit 7  */
  "filesystem access",   /* bit 8  */
  "high resolution",     /* bit 9  */
  "16-bit color",        /* bit 10 */
  "save file support"    /* bit 11 */
};

#define LHR_CAPABILITY_COUNT 12

/**
  Log all missing capabilities to the engine log.
  Uses the LHR_LOG macro defined by the frontend.
*/
static inline void LHR_logMissingCapabilities(const LHR_MapsetMeta *meta)
{
  uint32_t missing = LHR_mapsetMissingCaps(meta);
  if (missing == 0) return;

  LHR_LOG("Mapset compatibility failure:");

  for (int i = 0; i < LHR_CAPABILITY_COUNT; ++i)
  {
    if (missing & (1u << i))
    {
      LHR_LOG("  MISSING: ");
      LHR_LOG(LHR_capabilityNames[i]);
    }
  }
}

/* ======================== DYNAMIC LOADING GATE ============================ */

/**
  Compile-time switch. When set to 1, the engine can load .lhrm mapset files
  from disk at runtime. When 0, only baked-in mapsets are available.
*/
#ifndef LHR_DYNAMIC_MAPSETS
  #define LHR_DYNAMIC_MAPSETS 0
#endif

#if LHR_DYNAMIC_MAPSETS

/**
  On-disk file header for a .lhrm mapset file.
  Layout:
    [4 bytes] magic   = LHR_MAPSET_MAGIC
    [4 bytes] version = LHR_MAPSET_VERSION
    [sizeof(LHR_MapsetMeta)] metadata
    [... level data, resources, etc. ...]
*/
typedef struct
{
  uint32_t magic;
  uint32_t formatVersion;
  LHR_MapsetMeta meta;
} LHR_MapsetFileHeader;

#include <stdio.h>

/**
  Attempt to load a mapset file header from disk. Returns 1 on success, 0 on
  failure (bad magic, wrong version, or file I/O error). Does NOT load the
  level data itself — that happens in a separate pass after capability checking.
*/
static inline int LHR_loadMapsetHeader(const char *path,
                                        LHR_MapsetFileHeader *out)
{
  FILE *f = fopen(path, "rb");
  if (!f) return 0;

  if (fread(out, sizeof(LHR_MapsetFileHeader), 1, f) != 1)
  {
    fclose(f);
    return 0;
  }

  fclose(f);

  if (out->magic != LHR_MAPSET_MAGIC)
  {
    LHR_LOG("Mapset file: bad magic number.");
    return 0;
  }

  if (out->formatVersion != LHR_MAPSET_VERSION)
  {
    LHR_LOG("Mapset file: unsupported format version.");
    return 0;
  }

  return 1;
}

/**
  Full dynamic load gate: reads the header, checks compatibility, and rejects
  if the platform doesn't meet requirements. Returns 1 if the mapset is safe
  to proceed loading, 0 otherwise.
*/
static inline int LHR_tryLoadMapset(const char *path,
                                     LHR_MapsetFileHeader *out)
{
  if (!LHR_loadMapsetHeader(path, out))
    return 0;

  if (!LHR_mapsetCompatible(&out->meta))
  {
    LHR_logMissingCapabilities(&out->meta);
    return 0;
  }

  return 1;
}

#endif /* LHR_DYNAMIC_MAPSETS */

#endif /* _LHR_MAPSET_H */
