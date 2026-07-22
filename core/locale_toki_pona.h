/**
  @file locale_toki_pona.h

  Toki Pona locale payload for menu, HUD and story text.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_LOCALE_TOKI_PONA_H
#define _LHR_LOCALE_TOKI_PONA_H

static const LHR_LevelMeta LHR_levelMeta_tok[LHR_NUMBER_OF_LEVELS] = {0};

static const char *const LHR_menuItemTexts_tok[] =
{
  "awen",
  "sitelen",
  "open",
  "kama jo",
  "kalama utala",
  "kalama musi",
  "lukin",
  "toki",
  "pini"
};

static const LHR_Locale LHR_locale_tok =
{
  "tok",
  LHR_menuItemTexts_tok,
  "moli",
  "awen? L ala lon R",
  "awen",
  "ma pini",
  "ma li nasa. ilo pi Macrochip li wile lawa e ale. sina ken utala.",
  "ilo li pini, taso ma li nasa kin. sina kama tawa suno sin.",
  "MALWARE OS DETECTED",
  "lon",
  "ala",
  LHR_levelMeta_tok
};

#endif
