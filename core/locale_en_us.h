/**
  @file locale_en_us.h

  English (US) locale payload for menu, HUD and story text.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_LOCALE_EN_US_H
#define _LHR_LOCALE_EN_US_H

static const LHR_LevelMeta LHR_levelMeta_en_us[LHR_NUMBER_OF_LEVELS] = {
  { "Entering the facility...", "You survived the first sector.", 1, 2 },
  { "The deeper you go...", "Sector 2 cleared.", 3, 4 },
  { "No turning back.", "Sector 3 cleared.", 5, 0 },
  { "They are waiting.", "Sector 4 cleared.", 1, 0 },
  { "Halfway there.", "Sector 5 cleared.", 2, 0 },
  { "The heat rises.", "Sector 6 cleared.", 3, 0 },
  { "Almost out.", "Sector 7 cleared.", 4, 0 },
  { "The final stretch.", "Sector 8 cleared.", 5, 0 },
  { "The core approaches.", "Sector 9 cleared.", 1, 0 },
  { "The heart of the absurdity.", "The nightmare is finally over.", 2, 6 }
};

static const char *const LHR_menuItemTexts_en_us[] =
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

static const LHR_Locale LHR_locale_en_us =
{
  "en_US",
  LHR_menuItemTexts_en_us,
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
  LHR_levelMeta_en_us
};

#endif
