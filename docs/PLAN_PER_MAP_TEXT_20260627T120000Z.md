# Plan: Per-Map Optional Intro/Outro Text

Timestamp: 2026-06-27T12:00:00Z

## Goal

Allow each map to carry optional intro and outro text strings displayed via
the existing `SFG_drawStoryText()` mechanism, without modifying the `SFG_Level`
struct or breaking embedded builds.

## Current Behaviour

- `SFG_drawStoryText()` is called from `SFG_draw()` when the game state is
  `SFG_GAME_STATE_INTRO` or `SFG_GAME_STATE_OUTRO`.
- `SFG_GAME_STATE_INTRO` is entered only when the player selects level 0 from
  the menu (i.e. once per full playthrough start).
- `SFG_GAME_STATE_OUTRO` is entered after winning the final level.
- Intro/outro text is read from global `SFG_introText` / `SFG_outroText` in
  `core/texts.h` (which will migrate to `core/locale.h` under the localization
  plan).

## Design Decisions

### Where texts are stored

A new `SFG_LevelMeta` struct is introduced in `core/texts.h` (migrating later
to `core/locale.h`):

```c
typedef struct
{
  const char *introText;  /* shown before the level starts; NULL = no text */
  const char *outroText;  /* shown after level completion; NULL = no text */
  uint8_t introMusicTrack; /* 0 = none, otherwise frontend-defined track id */
  uint8_t outroMusicTrack; /* 0 = none, otherwise frontend-defined track id */
} SFG_LevelMeta;

static const SFG_LevelMeta SFG_levelMeta[SFG_NUMBER_OF_LEVELS];
```

- The table is a compile-time constant array, one entry per level, indexed the
  same as `SFG_levels[]`.
- `SFG_Level` struct in `core/levels.h` is NOT modified. All metadata is in
  this parallel table, which compiles to zero cost on embedded targets when
  all pointers are null.

### What happens with legacy maps (no text defined)

When `introText` or `outroText` is `NULL` for a given level:

- No story-text state is entered for that level's individual transition.
- There is no fallback to the global `SFG_introText` / `SFG_outroText`.
- The global strings remain exclusively for the game-start intro (level 0) and
  game-end outro (final level), as today.
- This means legacy maps that do not define per-map text behave exactly as they
  do now: no extra story screens.

When `introMusicTrack` or `outroMusicTrack` is `0`:
- No per-map story music override is played.
- Existing gameplay/menu music behavior remains unchanged.

### Trigger points

| Trigger | When | Condition |
|---|---|---|
| Per-map intro | Level load (any level) | `SFG_levelMeta[n].introText != NULL` |
| Game-start global intro | Level 0 selected from menu | Existing behaviour, unchanged |
| Per-map outro | Level win animation complete, before advancing | `SFG_levelMeta[n].outroText != NULL` |
| Game-end global outro | Final level completed | Existing behaviour, unchanged |

Per-map intro and outro use `SFG_drawStoryText()` as the renderer. The state
machine requires two additional game states or reuses `SFG_GAME_STATE_INTRO` /
`SFG_GAME_STATE_OUTRO` with a flag to distinguish per-map from global. The
simpler option (preferred) is reuse with a flag:

```c
// in game state struct:
uint8_t storyTextIsPerMap;  /* 1 = reading from levelMeta, 0 = reading global */
```

### Global strings location

The global strings are moved from `core/texts.h` into locale data:
- `SFG_activeLocale->globalIntroText`
- `SFG_activeLocale->globalOutroText`

These global strings are used only for:
- game-start intro (new game from menu at level 0)
- final-game ending screen (after final level)

Per-map strings are read from:
- `SFG_activeLocale->levelMeta[levelNumber].introText`
- `SFG_activeLocale->levelMeta[levelNumber].outroText`

Per-map optional music is read from:
- `SFG_activeLocale->levelMeta[levelNumber].introMusicTrack`
- `SFG_activeLocale->levelMeta[levelNumber].outroMusicTrack`

### Localization

Per-map text strings participate in the localization system. Each locale's
`SFG_LevelMeta` table is defined in its own locale file
(e.g. `core/locale_en_us.h`) so the per-map texts are translatable.

## Implementation Sketch

AS-IS (in `core/texts.h`):
```c
static const char *SFG_introText = "...";
static const char *SFG_outroText = "...";
```

TO-BE (addition to `core/texts.h`, migrating to `core/locale.h`):
```c
typedef struct {
  const char *introText;
  const char *outroText;
  uint8_t introMusicTrack;
  uint8_t outroMusicTrack;
}
  SFG_LevelMeta;

/* NULL entries = no per-map text for that level */
static const SFG_LevelMeta SFG_levelMeta[SFG_NUMBER_OF_LEVELS] = {
  { NULL, NULL, 0, 0 }, /* level 0 */
  { NULL, NULL, 0, 0 }, /* level 1 */
  /* ... */
};
```

`SFG_drawStoryText()` change (AS-IS → TO-BE):
- AS-IS: directly reads `SFG_outroText` / `SFG_introText` based on `levelNumber`.
- TO-BE: if `SFG_game.storyTextIsPerMap`, reads from `SFG_activeLocale->levelMeta[n]`; otherwise reads `SFG_activeLocale->globalIntroText` / `SFG_activeLocale->globalOutroText`.

### Flow decisions (resolved)

- Per-map story screens wait for key press (same as global ending behavior), they do not auto-advance.
- In demo mode, all story text screens are skipped (global and per-map) to keep autoplay uninterrupted.

### Current sprite display logic (clarification)

Current `SFG_drawStoryText()` behavior:
- Intro-like screens use `sprite = SFG_game.blink * 2` with text color 7 on clear color 0.
- Outro-like screens use fixed `sprite = 18` with text color 23 on clear color 9.
- The sprite is rendered near bottom-center only when vertical resolution allows it (`SFG_GAME_RESOLUTION_Y > 50`).

Alternatives for per-map screens:
- Reuse current logic exactly (recommended for consistency and zero new assets).
- Add per-map sprite index in `SFG_LevelMeta` for bespoke screens.
- Disable sprites on per-map screens for text-only style.

Recommended default: reuse current logic exactly.

## Open Questions

- Should per-map music track IDs map to existing bytebeat tracks only, or should the API permit frontend-specific audio resources?
