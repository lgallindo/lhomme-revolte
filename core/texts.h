/**
  @file texts.h

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

#include "locale.h"

#define SFG_menuItemTexts (SFG_activeLocale->menuItemTexts)

#define SFG_TEXT_KILLS (SFG_activeLocale->textKills)
#define SFG_TEXT_SAVE_PROMPT (SFG_activeLocale->textSavePrompt)
#define SFG_TEXT_SAVED (SFG_activeLocale->textSaved)
#define SFG_TEXT_LEVEL_COMPLETE (SFG_activeLocale->textLevelComplete)

#define SFG_VERSION_STRING "2.0d"
/**<
  Version numbering is following: major.minor for stable releases,
  in-development unstable versions have the version of the latest stable +
  "d" postfix, e.g. 1.0d. This means the "d" versions can actually differ even
  if they're marked the same. */

#define SFG_introText (SFG_activeLocale->globalIntroText)
#define SFG_outroText (SFG_activeLocale->globalOutroText)

#define SFG_MALWARE_WARNING (SFG_activeLocale->malwareWarning)

#endif // gaurd
