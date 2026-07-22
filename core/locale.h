/**
  @file locale.h

  Locale registry and runtime locale-selection helpers for fork-localized text.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_LOCALE_H
#define _LHR_LOCALE_H

#include <stdint.h>
#include <string.h>
#include "levels.h"

#ifndef LHR_LOCALE_ONLY_EN_US
  #ifndef LHR_LOCALE_ONLY_PT_BR
    #ifndef LHR_LOCALE_ONLY_TOK
      #define LHR_LOCALE_RUNTIME_SWITCH 1
    #else
      #define LHR_LOCALE_RUNTIME_SWITCH 0
    #endif
  #else
    #define LHR_LOCALE_RUNTIME_SWITCH 0
  #endif
#else
  #define LHR_LOCALE_RUNTIME_SWITCH 0
#endif

typedef struct {
  const uint8_t *data;
  uint16_t width;
  uint16_t height;
  uint8_t frames;
  uint8_t fps;
} LHR_MapImage;

typedef struct
{
  const char *introText;
  const char *outroText;
  uint8_t introMusicTrack;
  uint8_t outroMusicTrack;
  const LHR_MapImage *introImage;
  const LHR_MapImage *outroImage;
} LHR_LevelMeta;

static inline uint16_t LHR_getMapImageFrame(const LHR_MapImage *img, uint32_t stateTimeMs) {
  if (img == 0 || img->frames <= 1 || img->fps == 0) return 0;
  return ((stateTimeMs * img->fps) / 1000) % img->frames;
}

typedef struct
{
  const char *localeId;
  const char *const *menuItemTexts;
  const char *textKills;
  const char *textSavePrompt;
  const char *textSaved;
  const char *textLevelComplete;
  const char *globalIntroText;
  const char *globalOutroText;
  const char *malwareWarning;
  const char *textOn;
  const char *textOff;
  const LHR_LevelMeta *levelMeta;
} LHR_Locale;

#include "locale_en_us.h"
#include "locale_pt_br.h"
#include "locale_toki_pona.h"

#if defined(LHR_LOCALE_ONLY_EN_US)
  static const LHR_Locale *LHR_availableLocales[] = {&LHR_locale_en_us};
  #define LHR_LOCALE_COUNT 1
#elif defined(LHR_LOCALE_ONLY_PT_BR)
  static const LHR_Locale *LHR_availableLocales[] = {&LHR_locale_pt_br};
  #define LHR_LOCALE_COUNT 1
#elif defined(LHR_LOCALE_ONLY_TOK)
  static const LHR_Locale *LHR_availableLocales[] = {&LHR_locale_tok};
  #define LHR_LOCALE_COUNT 1
#else
  static const LHR_Locale *LHR_availableLocales[] = {
    &LHR_locale_en_us,
    &LHR_locale_pt_br,
    &LHR_locale_tok
  };
  #define LHR_LOCALE_COUNT 3
#endif

static uint8_t LHR_activeLocaleIndex = 0;
#if defined(LHR_LOCALE_ONLY_EN_US)
  static const LHR_Locale *LHR_activeLocale = &LHR_locale_en_us;
#elif defined(LHR_LOCALE_ONLY_PT_BR)
  static const LHR_Locale *LHR_activeLocale = &LHR_locale_pt_br;
#elif defined(LHR_LOCALE_ONLY_TOK)
  static const LHR_Locale *LHR_activeLocale = &LHR_locale_tok;
#else
  static const LHR_Locale *LHR_activeLocale = &LHR_locale_en_us;
#endif

static inline void LHR_setLocaleByIndex(uint8_t index)
{
#if LHR_LOCALE_RUNTIME_SWITCH
  LHR_activeLocaleIndex = index % LHR_LOCALE_COUNT;
  LHR_activeLocale = LHR_availableLocales[LHR_activeLocaleIndex];
#else
  (void) index;
  LHR_activeLocaleIndex = 0;
  LHR_activeLocale = LHR_availableLocales[0];
#endif
}

static inline uint8_t LHR_setLocaleByName(const char *name)
{
  uint8_t i;

  if (name == 0)
    return 0;

  for (i = 0; i < LHR_LOCALE_COUNT; ++i)
  {
    if (strcmp(name, LHR_availableLocales[i]->localeId) == 0)
    {
      LHR_setLocaleByIndex(i);
      return 1;
    }
  }

  return 0;
}

#endif
