/**
  @file locale.h

  Locale registry and runtime locale-selection helpers for fork-localized text.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _SFG_LOCALE_H
#define _SFG_LOCALE_H

#include <stdint.h>
#include <string.h>
#include "levels.h"

#ifndef SFG_LOCALE_ONLY_EN_US
  #ifndef SFG_LOCALE_ONLY_PT_BR
    #ifndef SFG_LOCALE_ONLY_TOK
      #define SFG_LOCALE_RUNTIME_SWITCH 1
    #else
      #define SFG_LOCALE_RUNTIME_SWITCH 0
    #endif
  #else
    #define SFG_LOCALE_RUNTIME_SWITCH 0
  #endif
#else
  #define SFG_LOCALE_RUNTIME_SWITCH 0
#endif

typedef struct
{
  const char *introText;
  const char *outroText;
  uint8_t introMusicTrack;
  uint8_t outroMusicTrack;
} SFG_LevelMeta;

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
  const SFG_LevelMeta *levelMeta;
} SFG_Locale;

#include "locale_en_us.h"
#include "locale_pt_br.h"
#include "locale_toki_pona.h"

#if defined(SFG_LOCALE_ONLY_EN_US)
  static const SFG_Locale *SFG_availableLocales[] = {&SFG_locale_en_us};
  #define SFG_LOCALE_COUNT 1
#elif defined(SFG_LOCALE_ONLY_PT_BR)
  static const SFG_Locale *SFG_availableLocales[] = {&SFG_locale_pt_br};
  #define SFG_LOCALE_COUNT 1
#elif defined(SFG_LOCALE_ONLY_TOK)
  static const SFG_Locale *SFG_availableLocales[] = {&SFG_locale_tok};
  #define SFG_LOCALE_COUNT 1
#else
  static const SFG_Locale *SFG_availableLocales[] = {
    &SFG_locale_en_us,
    &SFG_locale_pt_br,
    &SFG_locale_tok
  };
  #define SFG_LOCALE_COUNT 3
#endif

static uint8_t SFG_activeLocaleIndex = 0;
#if defined(SFG_LOCALE_ONLY_EN_US)
  static const SFG_Locale *SFG_activeLocale = &SFG_locale_en_us;
#elif defined(SFG_LOCALE_ONLY_PT_BR)
  static const SFG_Locale *SFG_activeLocale = &SFG_locale_pt_br;
#elif defined(SFG_LOCALE_ONLY_TOK)
  static const SFG_Locale *SFG_activeLocale = &SFG_locale_tok;
#else
  static const SFG_Locale *SFG_activeLocale = &SFG_locale_en_us;
#endif

static inline void SFG_setLocaleByIndex(uint8_t index)
{
#if SFG_LOCALE_RUNTIME_SWITCH
  SFG_activeLocaleIndex = index % SFG_LOCALE_COUNT;
  SFG_activeLocale = SFG_availableLocales[SFG_activeLocaleIndex];
#else
  (void) index;
  SFG_activeLocaleIndex = 0;
  SFG_activeLocale = SFG_availableLocales[0];
#endif
}

static inline uint8_t SFG_setLocaleByName(const char *name)
{
  uint8_t i;

  if (name == 0)
    return 0;

  for (i = 0; i < SFG_LOCALE_COUNT; ++i)
  {
    if (strcmp(name, SFG_availableLocales[i]->localeId) == 0)
    {
      SFG_setLocaleByIndex(i);
      return 1;
    }
  }

  return 0;
}

#endif
