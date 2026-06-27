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
  uint8_t     showIntro;  /* 1 = trigger intro on level entry (non-final levels too) */
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

### Trigger points

| Trigger | When | Condition |
|---|---|---|
| Per-map intro | Level load (any level) | `SFG_levelMeta[n].introText != NULL && SFG_levelMeta[n].showIntro` |
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
typedef struct { const char *introText; const char *outroText; uint8_t showIntro; }
  SFG_LevelMeta;

/* NULL entries = no per-map text for that level */
static const SFG_LevelMeta SFG_levelMeta[SFG_NUMBER_OF_LEVELS] = {
  { NULL, NULL, 0 }, /* level 0 */
  { NULL, NULL, 0 }, /* level 1 */
  /* ... */
};
```

`SFG_drawStoryText()` change (AS-IS → TO-BE):
- AS-IS: directly reads `SFG_outroText` / `SFG_introText` based on `levelNumber`.
- TO-BE: if `SFG_game.storyTextIsPerMap`, reads from `SFG_levelMeta[n]`; otherwise reads global strings.

## Open Questions

- Should per-map outro always advance automatically after `SFG_STORYTEXT_DURATION`,
  or require a key press like the current global outro?
- Should per-map text inherit the same sprite display logic as the global screen?
