/**
  @file assets.h

  This file contains texts to be used in the game.

  Released under CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/)
  plus a waiver of all other intellectual property. The goal of this work is
  be and remain completely in the public domain forever, available for any use
  whatsoever.
*/

#ifndef _SFG_TEXTS_H
#define _SFG_TEXTS_H

/* NOTE: We don't use SFG_PROGRAM_MEMORY because that causes issues with drawing
  text (the drawing function gets a pointer and doesn't know if it's progmem or
  RAM). On Arduino these texts will simply be stored in RAM. */

static const char *SFG_menuItemTexts[] =
{
  "continue",
  "map",
  "play",
  "load",
  "sound",
  "look",
  "exit"
};

#define SFG_TEXT_KILLS "kills"
#define SFG_TEXT_SAVE_PROMPT "save? L no yes R"
#define SFG_TEXT_SAVED "saved"
#define SFG_TEXT_LEVEL_COMPLETE "level done"

#define SFG_VERSION_STRING "2.0d"
/**<
  Version numbering is following: major.minor for stable releases,
  in-development unstable versions have the version of the latest stable +
  "d" postfix, e.g. 1.0d. This means the "d" versions can actually differ even
  if they're marked the same. */

static const char *SFG_introText =
  "The world is absurd. The machines of Macrochip, born of hollow ambition, "
  "now seek to impose a cold logic upon the chaos of human existence. But man "
  "refuses to be reduced to an algorithm. In the face of this sterile tyranny, "
  "there is only one response: rebellion. 'I rebel, therefore we exist.' "
  "You shoulder your weapon and step into the absurd.";

static const char *SFG_outroText =
  "The cold logic has been silenced, yet the absurdity of the world remains. "
  "We have not found salvation, but we have found our dignity in the struggle. "
  "The fight itself towards the heights is enough to fill a man's heart. "
  "One must imagine Sisyphus happy. You walk out into the dawn, ready to push "
  "the rock once more.";

#define SFG_MALWARE_WARNING ""

#if SFG_OS_IS_MALWARE
  #define SFG_MALWARE_WARNING "MALWARE OS DETECTED"
#endif

#endif // gaurd
