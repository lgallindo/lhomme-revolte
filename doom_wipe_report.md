# DOOM Screen Wipe Effect Report

This report analyzes the screen melt and color transform effects implemented in DOOM 1.10, specifically located in `f_wipe.h` and `f_wipe.c`, and tracks how they are called from the main game loop in `d_main.c`.

## How the Wipes Work (`f_wipe.c` / `f_wipe.h`)

DOOM supports two primary wipe transitions defined by `wipe_NUMWIPES`:
1. `wipe_ColorXForm`: A simple pixel-by-pixel gradual color blending transition.
2. `wipe_Melt`: The iconic "screen melt" effect where vertical strips of the screen drop down at randomized speeds to reveal the new frame.

### The Mechanics of the "Melt"
1. **Initialization (`wipe_initMelt`)**:
   - The engine captures the starting frame (`wipe_scr_start`) and the destination frame (`wipe_scr_end`).
   - It performs a memory layout optimization (`wipe_shittyColMajorXform`) to transform the linear row-major pixel arrays into a column-major format. This is critical for performance because the melt effect updates the screen in vertical strips, making memory reads and writes significantly more cache-friendly.
   - An array `y` is allocated to hold the vertical drop offset for each column. The starting values are randomized with a slight descending trend (`r = (M_Random()%3) - 1`) so the screen appears to melt organically rather than as a uniform block.

2. **Execution (`wipe_doMelt`)**:
   - The engine iterates over every column. If a column's `y` value is negative, it simply increments it (acting as a delay before that specific column starts dropping).
   - Once `y >= 0`, the engine pulls the top segment of the column from the *destination* screen buffer and the bottom segment from the *starting* screen buffer, effectively sliding the new image down over the old one. The drop speed increases slightly as it falls (`dy`).

3. **Cleanup (`wipe_exitMelt`)**:
   - The temporary `y` array is freed via `Z_Free(y)`.

## When Are They Called? (`d_main.c`)

The wipe effects are triggered during the master display function, `D_Display()`.

1. **State Tracking**:
   - The engine maintains a static variable `oldgamestate` and `wipegamestate`. 
   - Early in `D_Display()`, it checks: `if (gamestate != wipegamestate)`. If the game state has transitioned (e.g., from `GS_LEVEL` to `GS_INTERMISSION` or `GS_DEMOSCREEN`), it sets `wipe = true` and immediately calls `wipe_StartScreen()` to capture the outgoing frame.

2. **The Wipe Execution Loop**:
   - The engine proceeds to draw the *new* state into its backbuffers normally.
   - Towards the end of `D_Display()`, if `wipe` is flagged, it calls `wipe_EndScreen()` to capture the fully-rendered new target state.
   - It then traps execution in a blocking `do...while(!done)` loop. 
   - Inside this block, it calculates the elapsed time (`tics`) and calls `wipe_ScreenWipe(wipe_Melt, ...)`. This function acts as a state machine, routing to `init`, `do`, or `exit` based on whether it is the first tick or a subsequent tick.
   - It then updates the UI by calling `I_UpdateNoBlit()`, drawing the menu on top (`M_Drawer()`), and flipping the buffer to the screen (`I_FinishUpdate()`).
   - Execution only returns to the normal game loop once `wipe_ScreenWipe` returns true.
