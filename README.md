# L'Homme Révolté

**L'Homme Révolté** is a FPS game engine designed to run on absolutely anything (from modern PCs to tiny microcontrollers). It features 10 levels, raycasted graphics, and a minimalist design.

## Philosophy

Inspired by Albert Camus' *L'Homme Révolté* (The Rebel), this project rebels against the bloated, corporate-dominated software industry. We reject DRM, telemetry, hardware lock-in, and unnecessary complexity. The engine proves that rich experiences can be built without heavy dependencies or modern engine bloat.

## Features

- **Extreme Portability:** Runs on X11, SDL, SDL 1.2, terminal, ncurses, CSFML, web browsers (Emscripten), Pokitto, and more.
- **Zero Dependencies:** Written in pure C99, requiring only a standard C library.
- **Tiny Footprint:** The entire game logic, assets, and engine fit into less than 200KB of compiled binary.
- **Licensing:** See LICENSES.md for the full fork licensing matrix (legacy code ambiguity, GPL-3.0-or-later fork code, CC0 legacy assets, CC BY-NC-SA 4.0 new assets).

## Compiling

You can compile the game for your platform using the included `tools/make.sh` script. The expected output files are as follows:

* `./tools/make.sh sdl`: Compiles the SDL2 build (default) to `build/revolte_sdl` and copies it to the root directory as `./revolte`.
* `./tools/make.sh sdl_lq`: Compiles the SDL2 low-quality profile to `build/revolte_sdl_lq`.
* `./tools/make.sh x11`: Compiles the X11 build to `build/revolte_x11`.
* `./tools/make.sh ncurses`: Compiles the ncurses build to `build/revolte_ncurses`.
* `./tools/make.sh saf`: Compiles the SAF frontend build to `build/revolte_saf`.
* `./tools/make.sh terminal`: Compiles the ANSI terminal build to `build/revolte_terminal`.
* `./tools/make.sh csfml`: Compiles the CSFML build to `build/revolte_csfml`.
* `./tools/make.sh test`: Compiles the unit test binary to `build/revolte_test`.
* `./tools/make.sh pokitto`: Compiles the Pokitto target to `BUILD/firmware.bin` (requires embedded toolchain).
* `./tools/make.sh emscripten`: Compiles the web build via `emcc` to `revolte.html`, `revolte.js`, and `revolte.wasm` in the root directory.

Run the resulting SDL binary with `./revolte`; other build outputs are written under `build/`.

## Map Conversion Tooling

`tools/gif2map` is the canonical map converter.

`assets/img2map.py` is now deprecated and kept for one transition release only.
Its parity check is enforced by `tests/test_img2map_parity.sh`,
which verifies parity against `tools/gif2map` across all `assets/level*.gif`
files and checks deterministic output stability.

## Testing Status

Only the Linux SDL frontend is currently tested.
All other variants are released AS-IS.
Some device-specific frontends were confirmed working in the past, but are
currently untested because the required devices are no longer available.

## Controls

- **Arrows / WASD:** Move
- **Mouse / Left+Right arrows:** Rotate camera
- **Left Mouse / J / Return:** Shoot / Confirm
- **Right Mouse / LShift:** Free look / Strafe / Cancel
- **Space:** Jump
- **Tab / Backspace:** Map

## Credits
Forked and maintained by lgallindo, 2026.
Original engine (*Anarch*) created 2020.
