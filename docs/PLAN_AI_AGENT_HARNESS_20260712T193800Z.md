# AI Agent Harness Patterns for L'Homme Révolté

**Created:** 2026-07-12T19:38:00Z  
**Status:** [ACTIVE]  
**Source:** Synthesized from cross-repo research session 2026-07-12 (Cursor `7c49ce95`)  
**Related:** `docs/PLAN_LLM_MAP_ANALYSIS_20260628T205100Z.md`, `mods/demo.diff`

---

## Executive summary

Connecting an AI to a game is a **harness problem**: translate between agent reasoning and the game's I/O boundary. Six architectural families apply, from structured APIs (easiest) to vision/OS automation (most general).

| Family | Observation | Action | Best for |
|--------|-------------|--------|----------|
| **A. Symbolic** | Parsed state (JSON) | Named actions | Roguelikes, chess |
| **B. RL / Gym** | Tensors | Action vectors | ViZDoom, Atari |
| **C. Code API** | Event logs | Generated SDK code | Voyager / Mineflayer |
| **D. Input replay (TAS)** | Tick-indexed inputs | Button bitmask tape | Speedruns, CI |
| **E. State injection** | Full snapshots | State + input | Non-deterministic physics |
| **F. Vision + automation** | Screenshots | Clicks/keys | Arbitrary UIs |

*L'Homme Révolté* today spans **D** (`mods/demo.diff`), **partial F** (`webButton`, `--screenshot`), and lacks **A/B/C** structured runtime APIs.

---

## Current repo surfaces

### SDL / native

| Mechanism | Role |
|-----------|------|
| `--screenshot` / F12 | BMP capture |
| `--lhrwarp N` | Jump to level |
| `--lhrmap` | Reveal map |
| `revolte.sav` | Persistent save |
| `mods/demo.diff` | Record/replay/rewind input tapes |

### Browser / WASM

| Mechanism | Role |
|-----------|------|
| `webButton(0–6)` | 7 buttons (directions, shoot, strafe, menu) |
| Keyboard via SDL | Full map in WASM |
| Screenshot export | **Not implemented** |
| Structured state | **Not implemented** |

### Existing MCP tooling (assets only)

`tools/gif2map_mcp.c`, `img2array_mcp.c`, `snd2array_mcp.c` — map/image/audio pipelines, **not gameplay**.

---

## Recommended harness roadmap

### Phase 1 — Observation (low effort)

- `--screenshot-path` + `--screenshot-every N`
- `--dump-state` JSON per frame: level, health, ammo, position

### Phase 2 — Action (medium effort)

- Stdin JSON each tick: `{"press": "shoot", "frames": 5}`
- Or extend `webButton` to all 16 `SFG_KEY_*` values

### Phase 3 — Deterministic agent CI

- Integrate `mods/demo.diff` into test target
- LLM generates demo tapes; CI asserts level completion

### Phase 4 — Gameplay MCP server

- `tools/revolte_mcp/` following `gif2map_mcp.c` pattern
- Tools: `screenshot`, `press`, `warp`, `state`, `play_demo`

### Phase 5 — Gymnasium wrapper (high effort)

- `gymnasium.make("LHommeRevolte-v0")` over headless SDL or `main_test.c`

### Phase 6 — Browser agent

- `canvas.toDataURL()` on demand
- `webInput(key, down)` for full keyboard in `revolte_shell.js`

---

## Comparative position

| Approach | In repo? | LLM-ready? |
|----------|----------|------------|
| Demo input tapes (`demo.diff`) | **Yes** | Generate tape text |
| `--screenshot` | **Yes** | Needs vision driver |
| `webButton` | **Yes** | Limited 7 buttons |
| Gym wrapper | No | RL training |
| Gameplay MCP | No | Native tool calls |
| Symbolic state API | No | Best for agents |

---

## References (external)

- Yao et al., ReAct (ICLR 2023)
- Wang et al., Voyager (2023)
- ViZDoom / Gymnasium game environments
- Playwright MCP, OpenAI/Gemini computer-use APIs
- Socratopia, *Game Code Anatomy* Ch. 26 (input replay theory)
