# L'Homme Révolté

**L'Homme Révolté** is a FPS game engine designed to run on absolutely anything (from modern PCs to tiny microcontrollers). It features 10 levels, raycasted graphics, and a minimalist design.

## Philosophy

Inspired by Albert Camus' *L'Homme Révolté* (The Rebel), this project rebels against the bloated, corporate-dominated software industry. We reject DRM, telemetry, hardware lock-in, and unnecessary complexity. The engine proves that rich experiences can be built without heavy dependencies or modern engine bloat.

## Features

- **Extreme Portability:** Runs on X11, SDL, SDL 1.2, terminal, ncurses, CSFML, web browsers (Emscripten), Pokitto, and more.
- **Zero Dependencies:** Written in pure C99, requiring only a standard C library.
- **Tiny Footprint:** The entire game logic, assets, and engine fit into less than 200KB of compiled binary.
- **Public Domain / GPL:** The original engine is released under CC0 1.0. All new code added in this fork is licensed under the GPL.

## Compiling

You can compile the game for your platform using the included `make.sh` script:

```sh
./make.sh sdl    # Compiles for SDL2 (Hardware accelerated, sound)
./make.sh sdl1   # Compiles for SDL 1.2 (Legacy hardware)
./make.sh x11    # Compiles for X11 (Fallback, no sound)
./make.sh terminal # Compiles for ANSI terminal
```

Run the resulting binary (e.g., `./revolte_sdl`).

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
