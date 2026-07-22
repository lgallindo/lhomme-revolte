# Agent Harness Documentation

This document provides excruciating detail on the implementation of the LLM/Agent Harness for L'Homme Révolté. The harness is designed to allow automated AI scripts, autonomous agents, and telemetry observers to both control the game and receive high-fidelity state information in real-time.

## Overview

The harness operates by transforming the game engine into an Interactive-Process-Communication (IPC) server. Instead of polling physical keyboard and mouse hardware, the game pauses its execution loop, waits for string-based commands via `stdin`, executes the specified actions for an exact number of physics ticks, and then dumps the resulting game state as a JSON payload to `stdout`.

This implementation includes two primary operation modes:
1. **Headless Execution (`revolte_agent`)**: A pure simulation environment built in `tests/agent_test.c` that runs independently of SDL or graphical dependencies.
2. **Synchronized Visual Execution (`revolte_sdl --agent-sync`)**: A fully native SDL graphical mode where the engine operates synchronously with an agent, allowing human observers to watch the agent play in real-time on a native macOS/Linux/Windows window.

---

## 1. The Telemetry & Action Protocol (`core/agent_harness.h`)

At the core of the implementation is `agent_harness.h`. This header provides two major capabilities: State Serialization (JSON) and Action Deserialization.

### State Serialization
The engine state is serialized using `SFG_agentDumpStateJSON()`. It exposes:
- **Game State Variables**: Current frame, game state enumeration (e.g., `LEVEL_START`, `PLAYING`), and selected level.
- **Player State**: Health, equipped weapon, global absolute coordinates (x, y, z), viewing angles, grid/square coordinates, inventory bitmasks, and ammunition arrays.
- **Entity State**: Number of active monsters, and for each active monster, its internal ID, state, health, and absolute coordinates.
- **Visual Telemetry (`screen_ascii`)**: A downsampled 64x32 character grid representing the raycasted scene. Because the native game runs at higher resolutions (e.g., 700x512 on PC), the buffer is procedurally sampled down to a compact ASCII representation mapped to the 8-color engine palette.

### Action Deserialization
The agent controls the game by sending space-separated string commands over `stdin` in the format:
`<KeyBitmask> <MouseDx> <MouseDy> <StepTicks>\n`

- **KeyBitmask**: A 16-bit integer (provided as a 4-character hex string, e.g., `0001` or `0011`) where each bit corresponds to an `SFG_KEY` macro (Up, Right, Down, Left, Shoot, Jump, etc.).
- **MouseDx/MouseDy**: Horizontal and vertical coordinate deltas for mouselook controls.
- **StepTicks**: The number of physics frames to simulate before pausing and requesting the next action. This allows an agent to say "Hold UP for 100 frames".

---

## 2. Headless Mode (`tests/agent_test.c`)

This is a standalone C file that integrates directly with the `core/game.h` engine logic without calling any UI or rendering backends. 
It defines the required system hooks (e.g., `SFG_getTimeMs()`, `SFG_setPixel()`, `SFG_playSound()`) as simple stubs or synthetic counters. When compiled via `./tools/make.sh agent`, it produces the `revolte_agent` binary. It is extremely fast and suitable for large-scale reinforcement learning.

---

## 3. Visual Agent Mode (`frontends/pc/main_sdl.c`)

To allow human observation of the AI, the native SDL front-end was heavily modified to support `--agent-sync`. When this argument is passed, the frontend executes a specialized synchronous loop.

### A. Synthetic Physics Clock
Normally, the game engine uses `SDL_GetTicks()` to pace its physics iterations. An agent sending inputs back-to-back would cause zero milliseconds to elapse in the real world, preventing the physics engine from ever updating the player's position. In `--agent-sync` mode, `SFG_getTimeMs()` is overridden to return a synthetic clock that increments by exactly 33ms per tick, ensuring deterministic physics execution decoupled from actual wall-clock time.

### B. Input Hooking & Idle Handling
Inside `mainLoopIteration()`, the game employs `select()` on `STDIN_FILENO` with a 10ms timeout.
- While waiting for the agent to compute and send its next move, the loop continues to pump `SDL_PollEvent` and issue `SDL_RenderPresent()`. This satisfies window managers (specifically macOS Cocoa and WindowServer), preventing the application from freezing, rendering a black screen, or showing the "Beachball of Death".
- Hardware mouse and keyboard inputs are bypassed completely; `SFG_keyPressed` and `SFG_getMouseOffset` return the values stored from the agent's last JSON packet.

### C. Display & Rendering Fixes
Several core rendering adjustments were made for this mode to function on modern operating systems:
- **macOS Texture Compatibility**: The master render texture was shifted from `SDL_TEXTUREACCESS_STATIC` to `SDL_TEXTUREACCESS_STREAMING` to resolve a black-screen bug observed on Mac machines when rendering agent-driven frames.
- **Visual Wiping Bypass**: The engine features a full-screen melt/wipe effect during level transitions. Because this effect traps the game in an isolated render loop (ignoring physics), it burned through the agent's tick commands without advancing the game. Wiping (`isWiping = 1`) is bypassed when `agentSyncMode` is true.
- **Human Observation Pacing**: Since the simulation executes instantaneously, a 30-frame agent sequence would finish in ~2ms. To allow users to watch the agent, an explicit `SDL_Delay(16)` was added to the end of the frame draw to throttle the simulation to ~60FPS.

---

## 4. The Agent Brain (`agent_brain.py`)

A Python orchestrator script was implemented to act as the autonomous test pilot. 
It utilizes `subprocess.Popen` to launch the SDL binary, maintaining persistent bidirectional pipes.
It executes an infinite loop:
1. Ingests stdout.
2. Skips native initialization logs (`SDL: initializing...`) until it encounters the opening `{` of the JSON payload.
3. Parses the state tree.
4. Dispatches the next predefined movement command in a cycle (skip intro -> walk forward -> turn right -> walk forward -> turn left -> shoot).

## Build Instructions
To build the headless agent:
`./tools/make.sh agent`

To build the SDL visual agent:
`LINK_MODE=dynamic ./tools/make.sh sdl`

To run the full visual test or the LLM agent:
We use `uv` for python dependency management. The scripts are located in the `agent/` directory.

```bash
cd agent
uv run llm_agent.py
# or for the fixed sequence script:
uv run agent_brain.py
# or for a local llama.cpp server:
uv run local_llama_agent.py
```

## Demo Recording and Replay

Because the game engine's harness processes raw text commands synchronously, it naturally functions as a complete record/replay system out of the box!

To **record** a demo, pass a file to the script:
```bash
uv run local_llama_agent.py --record demo.txt
```

To **replay** that demo, simply pipe the text file directly into the C engine. The engine will pace the frames visually and exit cleanly when EOF is reached:
```bash
cat demo.txt | ../build/revolte_sdl --agent-sync --lhrwarp 0
```
