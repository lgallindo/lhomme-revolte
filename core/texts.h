/**
  @file texts.h

  This file contains texts to be used in the game.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_TEXTS_H
#define _LHR_TEXTS_H

/* NOTE: We don't use LHR_PROGRAM_MEMORY because that causes issues with drawing
  text (the drawing function gets a pointer and doesn't know if it's progmem or
  RAM). On Arduino these texts will simply be stored in RAM. */

#include "locale.h"

#define LHR_menuItemTexts (LHR_activeLocale->menuItemTexts)

#define LHR_TEXT_KILLS (LHR_activeLocale->textKills)
#define LHR_TEXT_SAVE_PROMPT (LHR_activeLocale->textSavePrompt)
#define LHR_TEXT_SAVED (LHR_activeLocale->textSaved)
#define LHR_TEXT_LEVEL_COMPLETE (LHR_activeLocale->textLevelComplete)

#define LHR_VERSION_STRING "2.0d"
/**<
  Version numbering is following: major.minor for stable releases,
  in-development unstable versions have the version of the latest stable +
  "d" postfix, e.g. 1.0d. This means the "d" versions can actually differ even
  if they're marked the same. */

#define LHR_introText (LHR_activeLocale->globalIntroText)
#define LHR_outroText (LHR_activeLocale->globalOutroText)

#define LHR_MALWARE_WARNING (LHR_activeLocale->malwareWarning)

#endif // gaurd
