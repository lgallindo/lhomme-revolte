# Plan: LLM-Based Thematic Map Analysis & Repositioning System (TMAR)

- **Timestamp**: 2026-06-28T20:51:00Z
- **Goal**: Establish a concrete framework for LLMs to extract, analyze, and theme the levels of *L'Homme révolté* based on the repository's philosophical references (Camus, Berkman, Goldman, CrimethInc, etc.).
- **Scope**: Outlining map parsing, thematic mapping algorithms, LLM prompt engineering, validation logic, and execution steps. No existing game engine code is edited in this phase.

---

## 1. System Architecture

The proposed **Thematic Map Analyzer & Repositioner (TMAR)** pipeline consists of four distinct phases:

```
+------------------+      +-------------------+      +----------------------+      +--------------------+
|  Map Parser &    | ---> |   Thematic        | ---> |  LLM Re-generator    | ---> | Playability        |
|  Fact Extractor  |      |   Concept Matcher |      |  (Overlay Generator) |      | Validator (BFS)    |
+------------------+      +-------------------+      +----------------------+      +--------------------+
```

1. **Map Parser & Fact Extractor**: Read `core/levels.h` (or JSON from `tools/gif2map`) to build a topological connectivity map and inventory of elements (enemies, weapons, cards, locks, terminals, health).
2. **Thematic Concept Matcher**: Align the calculated metrics with conceptual indices (such as Camus' *limits*, *absurd endurance*, and *neither victim nor executioner*).
3. **LLM Overlay Generator**: Pass the facts and target themes to the LLM to propose repositioning coordinates for monsters, weapons, and locks.
4. **Playability Validator**: Perform a breadth-first search (BFS) simulation over the 64x64 grid to guarantee that:
   - The player can reach the finish.
   - Cards are reachable before the corresponding locks are encountered on the critical path.
   - Element counts do not exceed `SFG_MAX_LEVEL_ELEMENTS` (128).

---

## 2. Structural Blueprint (Data Schemas & Stubs)

### 2.1 Extracted Fact JSON Schema

The extractor yields a JSON object for each level. Below is the target schema structure:

```json
{
  "level_index": 0,
  "metrics": {
    "lock_pressure": 0.35,
    "confinement_score": 0.45,
    "resource_austerity": 0.82,
    "enemy_enforcement_density": 0.65,
    "verticality_score": 0.20
  },
  "player_start": [32, 32, 0],
  "elements": [
    {"type": "SFG_LEVEL_ELEMENT_TERMINAL", "coords": [30, 28]},
    {"type": "SFG_LEVEL_ELEMENT_CARD0", "coords": [15, 12]},
    {"type": "SFG_LEVEL_ELEMENT_MONSTER_SPIDER", "coords": [32, 35]}
  ]
}
```

### 2.2 Extraction Algorithm & Code Stub (Python)

The following Python script parses the C definitions from `core/levels.h` to extract map coordinates and levels details:

```python
import re
import json

class MapFactExtractor:
    ELEMENT_MAP = {
        0x01: "BARREL", 0x02: "HEALTH", 0x03: "BULLETS", 0x04: "ROCKETS",
        0x05: "PLASMA", 0x06: "TREE", 0x07: "FINISH", 0x08: "TELEPORTER",
        0x09: "TERMINAL", 0x0d: "CARD0", 0x0e: "CARD1", 0x0f: "CARD2",
        0x10: "LOCK0", 0x11: "LOCK1", 0x12: "LOCK2",
        0x20: "MONSTER_SPIDER", 0x21: "MONSTER_DESTROYER", 0x22: "MONSTER_WARRIOR",
        0x23: "MONSTER_PLASMABOT", 0x24: "MONSTER_ENDER", 0x25: "MONSTER_TURRET",
        0x26: "MONSTER_EXPLODER"
    }

    def __init__(self, levels_h_path: str):
        self.path = levels_h_path

    def parse_levels(self):
        with open(self.path, "r", encoding="utf-8") as f:
            content = f.read()

        # Find SFG_level structs
        level_blocks = re.findall(
            r"SFG_PROGRAM_MEMORY\s+SFG_Level\s+SFG_level(\d+)\s*=\s*\{(.*?)\};", 
            content, 
            re.DOTALL
        )

        parsed_levels = {}
        for idx, block in level_blocks:
            parsed_levels[int(idx)] = self._extract_details(block)
        return parsed_levels

    def _extract_details(self, block_text: str):
        # Extract player starting coordinates
        player_start = [32, 32, 0]
        start_match = re.search(r"playerStart\s*=\s*\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\}", block_text)
        if start_match:
            player_start = [int(start_match.group(1)), int(start_match.group(2)), int(start_match.group(3))]

        # Extract level elements
        elements = []
        elements_section = re.search(r"elements\s*=\s*\{(.*?)\}", block_text, re.DOTALL)
        if elements_section:
            elem_lines = re.findall(r"\{\s*([^,]+)\s*,\s*\{\s*(\d+)\s*,\s*(\d+)\s*\}\s*\}", elements_section.group(1))
            for etype, x, y in elem_lines:
                etype_clean = etype.strip()
                val = self._resolve_element_type(etype_clean)
                if val != "NONE":
                    elements.append({
                        "type": val,
                        "coords": [int(x), int(y)]
                    })
        return {
            "player_start": player_start,
            "elements": elements
        }

    def _resolve_element_type(self, type_str: str):
        if "SFG_LEVEL_ELEMENT_" in type_str:
            return type_str.replace("SFG_LEVEL_ELEMENT_", "")
        # Try fallback hex conversion if present in static code
        return type_str
```

---

## 3. LLM Theme Overlay Generator

The LLM is provided with:
1. The **Parsed Map Facts** (JSON).
2. The **Thematic Reference Tags** (extracted from English/Portuguese translations of Camus, Berkman, Goldman).
3. **Aesthetic/Design Constraints**: For example, Camus' *measure* requires avoiding "monster rooms" that encourage mindless bullet-hell slaughter, favoring instead precise, isolated skirmishes.

### 3.1 Prompt Template for LLM Analysis

```text
You are an expert game designer aligning level layout files for the 3D retro shooter "L'Homme Révolté" with its philosophical source text index.

Philosophical Concepts to Apply:
- CON-003 (Revolt without redemption): Limit excessive health/ammo items. Emphasize endurance.
- CON-004 (Limits against murder-logic): Reduce high-HP monster spams. Prioritize tactical, low-count placement.
- CON-008 (Bureaucracy as Enemy AI): Cluster turrets and locks around terminals to simulate locked control nodes.

Input Level Data:
{level_facts_json}

Task:
Propose a modified JSON list of elements. You may:
1. Change coordinates of existing enemies, weapons, or health packs.
2. Replace excessive heavy weapons (e.g., PLASMA, ROCKETS) with lower-tier assets to force tactical caution.
3. Move access cards further from locks to stretch the exploration path, increasing lock pressure.

Respond STRICTLY with a valid JSON array of updated elements matching the schema:
[
  {"type": "MONSTER_SPIDER", "coords": [x, y]},
  ...
]
Do not include markdown or explanations.
```

---

## 4. Playability Validator

To ensure that the LLM's suggested changes do not break the level, the validator runs a pathfinding check.

### 4.1 Breadth-First Search Solver (Pseudo-code)

```python
def validate_level(map_grid, player_start, elements):
    # 1. Identify lock/key positions
    cards = {e['type']: e['coords'] for e in elements if 'CARD' in e['type']}
    locks = {e['type']: e['coords'] for e in elements if 'LOCK' in e['type']}
    finish_coords = next(e['coords'] for e in elements if e['type'] == 'FINISH')

    # 2. Track accessible keys and state
    obtained_cards = set()
    
    # BFS loop that restarts or resumes when a card is picked up
    def can_reach_target(start, target, unlocked_types):
        queue = [start]
        visited = {start}
        while queue:
            curr = queue.pop(0)
            if curr == target:
                return True
            for neighbor in get_valid_neighbors(map_grid, curr):
                if neighbor in visited:
                    continue
                # Check door lock compatibility
                tile_type = map_grid[neighbor[1]][neighbor[0]] & 0xc0
                if tile_type == 0xc0: # Door
                    lock_type = get_lock_at_coords(neighbor, locks)
                    if lock_type and lock_type.replace("LOCK", "CARD") not in unlocked_types:
                        continue # Locked door
                visited.add(neighbor)
                queue.append(neighbor)
        return False

    # Simulate collection sequence
    current_pos = player_start
    while True:
        progress = False
        for card_type, card_coords in list(cards.items()):
            if card_type not in obtained_cards:
                if can_reach_target(current_pos, card_coords, obtained_cards):
                    obtained_cards.add(card_type)
                    current_pos = card_coords
                    progress = True
        if not progress:
            break

    # Final check: Can we reach the finish with the obtained cards?
    return can_reach_target(current_pos, finish_coords, obtained_cards)
```

---

## 5. Execution Roadmap

1. **Step 1: Metric Script Implementation**: Write a python script under `tools/thematic_metrics.py` to extract map metadata.
2. **Step 2: Theme Index Parser**: Integrate the translations in `docs/source_texts/theme/` to dynamically build prompt tags.
3. **Step 3: Run Validation Suite**: Test the BFS solver on the current levels to establish a baseline.
4. **Step 4: Integrate LLM Client**: Set up a pipeline file `tools/llm_theme_generator.py` that takes the generated JSON, queries the model, and validates the output.
5. **Step 5: Apply Theme Refinement**: Stage the modified level designs to the repository branch and create a Git revision.
