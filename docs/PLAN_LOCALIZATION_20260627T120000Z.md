# Plan: Localization

Timestamp: 2026-06-27T12:00:00Z

## Goal

Support en_US, pt_BR, and Toki Pona as first-class locales. A distributor can
build a single-locale binary or a multi-locale binary with a runtime language
switch (menu + CLI flag).

## One File Per Locale

Rather than a monolithic `core/locale.h`, one file per language is used for
clarity and ease of contribution:

```
core/locale_en_us.h
core/locale_pt_br.h
core/locale_toki_pona.h
core/locale.h         ← master: defines SFG_Locale struct + includes locale files
```

`core/locale.h` owns the `SFG_Locale` struct definition and the
`SFG_activeLocale` pointer. It conditionally includes the per-language headers
based on compile-time defines.

## SFG_Locale Struct

```c
typedef struct
{
  /* menu items (must match SFG_menuItemTexts[] indices) */
  const char *menuContinue;
  const char *menuMap;
  const char *menuPlay;
  const char *menuLoad;
  const char *menuSound;
  const char *menuLook;
  const char *menuExit;
  const char *menuLanguage;   /* NULL in single-locale builds */

  /* HUD / overlays */
  const char *textKills;
  const char *textSavePrompt;
  const char *textSaved;
  const char *textLevelComplete;

  /* global story texts */
  const char *introText;
  const char *outroText;

  /* per-map story texts (see PLAN_PER_MAP_TEXT) */
  const SFG_LevelMeta *levelMeta;   /* pointer to per-locale level meta array */

  /* locale identifier (ASCII, e.g. "en_US") */
  const char *localeId;
} SFG_Locale;
```

Each per-language `.h` defines a single `static const SFG_Locale` instance,
e.g. `SFG_LOCALE_EN_US`.

## Character Encoding

| Locale | Encoding | Notes |
|---|---|---|
| en_US | ASCII (7-bit) | No issues with the current pixel font. |
| Toki Pona | ASCII (7-bit) | Uses ASCII romanization only (e.g. "toki pona li pona"). No special glyphs needed. |
| pt_BR | **Latin-1 / UTF-8** | Requires characters beyond ASCII: ã ç ê ó õ etc. |

### pt_BR encoding strategy (decision required)

The current text renderer uses a custom bitmap font. Whether it can render
Latin-1/UTF-8 characters depends on the font bitmap coverage. Three options:

| Option | Effort | Quality |
|---|---|---|
| **A. ASCII transliteration** | None | Low (e.g. "sao" for "são") |
| **B. Extend font bitmap** | Medium | High (add ~20 glyphs for pt_BR) |
| **C. UTF-8 multi-byte renderer** | High | Full fidelity |

**Recommended starting point: Option B** (extend font bitmap for the ~20 most
common pt_BR diacritics: ã, ç, ê, é, ó, ô, õ, ú, â, í, à). Option A can be
used as a temporary fallback while the font is extended.

## Compile-Time Single-Locale Build

```
-DSFG_LOCALE_ONLY=en_US   # or pt_BR, toki_pona
```

When `SFG_LOCALE_ONLY` is defined:
- Only the matching locale `.h` is compiled in.
- `SFG_activeLocale` is a compile-time constant pointer (zero runtime overhead).
- The "language" menu item is excluded via `#if`.
- Binary size is minimized (no unused locale strings).

## Runtime Multi-Locale Build (default when SFG_LOCALE_ONLY is absent)

- All three locale files are compiled in.
- `SFG_activeLocale` is a mutable pointer, defaulting to `&SFG_LOCALE_EN_US`.
- A "language" menu item is added to the in-game menu.
- A `-l <locale_id>` CLI flag (PC frontends) overrides the initial locale before
  `SFG_init()`.
- The active locale is stored in save slot byte (TBD) so it persists across
  sessions. If that byte is not available, falls back to compile-time default.

## Migration from texts.h

`core/texts.h` is retired in favour of `core/locale.h`. Migration steps:

1. Create `core/locale.h` with the `SFG_Locale` struct.
2. Create `core/locale_en_us.h` with the current strings from `texts.h`.
3. Create `core/locale_pt_br.h` and `core/locale_toki_pona.h` as stubs (strings
   initially matching en_US, to be translated).
4. Update `core/game.h` to `#include "locale.h"` instead of `"texts.h"` and
   replace direct string references with `SFG_activeLocale->fieldName`.
5. Remove `core/texts.h` or reduce it to a legacy shim.

## Accepted Commit Scope

Localization work uses commit type `localization` per PROJECT_RULES.md.
