# Mapsets & Project Architecture

The LHR engine architecture is being heavily refactored to support self-contained, moddable, and statically baked "Mapsets".

## Definition of a Mapset
A **Mapset** is a bundled collection of:
1. **Levels (Maps):** Layouts, enemy placements, and logical flow.
2. **Resources:** Custom graphics, C-array sprites, palettes, and PCM WAV/MIDI music tracks.
3. **Requirements Manifesto:** A strict definition of the hardware/platform requirements needed to play this mapset.

## Mapset Requirements
Every mapset must declare its minimum hardware requirements in its struct/metadata. A compilation or dynamic loader *must refuse* to launch a mapset if the host platform does not meet these requirements.
Examples of requirements:
- `REQUIRE_KEYS_4`: Requires a 4-button layout (e.g. mobile/handheld constraints).
- `REQUIRE_DUAL_SCREEN`: Requires dual screen rendering output (e.g. Nintendo DS backend).
- `REQUIRE_AUDIO_WAV`: Requires a frontend capable of PCM WAV streaming.
- `REQUIRE_MOUSE`: Requires a pointing device for free-look or UI.

## Loading Architecture
The engine will support two methods of loading Mapsets, controlled via a compilation switch (`LHR_DYNAMIC_MAPSETS`):

1. **Baked-In (Static compilation):**
   - At least one mapset is hardcoded as C-arrays into the binary.
   - Ideal for embedded devices, bare-metal targets, or creating a standalone "game" executable that requires zero external files.
2. **Dynamic Loading (File-based):**
   - If `LHR_DYNAMIC_MAPSETS` is enabled, the engine can load external mapsets from disk at runtime.
   - Allows users on standard PC (SDL) or Web (WASM) to load new mods without recompiling the executable.

## Mod Compilation & E2E Testing Pipeline
For every mod/mapset developed:
1. A bespoke SDL compilation is generated featuring *only* that mapset baked in.
2. The compilation is stored and indexed.
3. **E2E Testing:** It undergoes fully automated testing via headless demo playback or deterministic scripting (avoiding non-deterministic LLM-based agent play unless strictly for exploratory AI demos).
