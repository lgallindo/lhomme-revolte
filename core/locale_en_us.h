/**
  @file locale_en_us.h

  English (US) locale payload for menu, HUD and story text.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _SFG_LOCALE_EN_US_H
#define _SFG_LOCALE_EN_US_H

static const SFG_LevelMeta SFG_levelMeta_en_us[SFG_NUMBER_OF_LEVELS] = {0};

static const char *const SFG_menuItemTexts_en_us[] =
{
  "continue",
  "map",
  "play",
  "load",
  "sound fx",
  "music",
  "look",
  "language",
  "exit"
};

static const SFG_Locale SFG_locale_en_us =
{
  "en_US",
  SFG_menuItemTexts_en_us,
  "kills",
  "save? L no yes R",
  "saved",
  "level done",
  "The world is absurd. The machines of Macrochip, born of hollow ambition, "
  "now seek to impose a cold logic upon the chaos of human existence. But man "
  "refuses to be reduced to an algorithm. In the face of this sterile tyranny, "
  "there is only one response: rebellion. 'I rebel, therefore we exist.' "
  "You shoulder your weapon and step into the absurd.",
  "The cold logic has been silenced, yet the absurdity of the world remains. "
  "We have not found salvation, but we have found our dignity in the struggle. "
  "The fight itself towards the heights is enough to fill a man's heart. "
  "One must imagine Sisyphus happy. You walk out into the dawn, ready to push "
  "the rock once more.",
  "MALWARE OS DETECTED",
  "on",
  "off",
  SFG_levelMeta_en_us
};

#endif
