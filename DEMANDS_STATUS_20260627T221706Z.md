# Comprehensive Demands Status (Conversation)

Generated: 2026-06-27T19:42:52Z
Revised: 2026-06-27T22:17:06Z
Scope: full conversation demand inventory across all user requests in this session.

| Row ID | Demand | Status | Justification |
|---|---|---|---|
| D-001 | Locate references to media/texts.txt | Done | Workspace search and ripgrep performed; no direct runtime path references found. |
| D-002 | Confirm AGENTS.md was read | Done | Confirmed in chat and repeatedly consulted afterward. |
| D-003 | Add README disclaimer that only Linux SDL is tested and others are AS-IS | Done | Testing-status section added in README. |
| D-004 | Remove ANARCH branding from Pokitto frontend | Done | save namespace switched to REVOLTE in main_pokitto.cpp. |
| D-005 | Find current default SDL text sources | Done | Mapped to core text symbols consumed via core/game.h in SDL build path. |
| D-006 | Find CC0/public-domain references and propose fixes | Done | Repository scan performed and remediation strategy documented/partially applied in licensing docs. |
| D-007 | Create checkpoint commit before more work | Done | Checkpoint commit created as requested. |
| D-008 | Implement licensing matrix | Done | LICENSES.md created/updated with matrix and file scopes. |
| D-009 | Force-push when explicitly approved | Done | Force push executed after explicit approval. |
| D-010 | API-002: Implement/validate gif2map retirement/parity path | Done | C toolchain + parity script + workflow implemented and validated. |
| D-011 | API-004: Implement localization | Partially done | Core locale infrastructure and runtime switching added; further refinements remained requested later. |
| D-012 | API-003: Implement per-map intro/outro behavior | Partially done | Story routing and metadata hooks started; additional design constraints captured in plans. |
| D-013 | Purge specific git email metadata from history | Not done (in main repo state file scope) | Discussed and partially investigated; full destructive rewrite requires explicit controlled operation sequence. |
| D-014 | Keep only main branch on remote | Done | Remote branch pruning was executed previously (as summarized and rechecked). |
| D-015 | Create formal game-state-machine diagram in a Game Design Bible | Not done | No committed/verified bible diagram artifact found in current tracked state. |
| D-016 | Produce long-form deterministic-games report with bibliography + downloads | Not done | No finalized tracked report with downloaded references found in current tracked state. |
| D-017 | Move .github/copilot-instructions.md ignore to git exclude | Done | Removed from .gitignore and inserted in .git/info/exclude. |
| D-018 | R-005: assess SemVer + git hooks difficulty | Done | Difficulty assessment provided (medium; hook+CI enforcement required). |
| D-019 | R-006: enforce no placeholders/stubs rule in AGENTS.md | Done | TECH-013 added to AGENTS.md. |
| D-020 | R-007: check Git LFS usage | Done | git lfs ls-files and .gitattributes confirmed active LFS tracking. |
| D-021 | Explain malware OS macro | Done | Located define/use sites; behavior explanation provided. |
| D-022 | SAF is exception for malloc/RAM restrictions; document in PROJECT_RULES.md | Done | Exception documented in PROJECT_RULES.md. |
| D-023 | Provide SAF references | Done | File-level references collected (core/saf.h, main_saf.c, make.sh, etc.). |
| D-024 | Fix all @file tag mismatches | Done (with diff semantics correction) | Source/header mismatches fixed; diff payload headers restored to target-file semantics. |
| D-025 | Document all compile targets in README | Done | README compile section lists sdl, sdl_lq, x11, ncurses, saf, terminal, csfml, test, pokitto, emscripten. |
| D-026 | Produce an extensive possibilities report from the original thematic request, while keeping README user-friendly | Done | Extensive standalone report created in docs at docs/REPORT_THEME_POSSIBILITIES_20260627T194721Z.md; developer-process content kept out of README. |
| D-027 | Execute in order: update GPL file list, then GPL headers/docstrings | Done | Applied in requested order. |
| D-028 | Keep no retirement-gate or external-python-repo mentions in main repo | Done | Workflow/test names switched to parity naming and wording scrubbed. |
| D-029 | Revise how headers on diff files work after patch | Done | Diff-embedded headers reverted to represent target file (demo.h). |
| D-030 | Remove developer-only section from README | Done | Theme Direction Proposal section removed. |
| D-031 | Continue pending tasks broadly | In progress | Multiple pending macro tasks remained; this ledger now tracks explicit status. |
| D-032 | API-002: compile and test gif2map now | Done | make -C tools/gif2map test passed (4/4). |
| D-033 | API-002: check if gif2map_mcp.c copied from relic | Done | Direct parser-style comparison performed; implementations differ (no direct copy evidence). |
| D-034 | API-002: explain how to test gif2map_mcp manually | Done | JSON-RPC stdin smoke-test flow provided and executed. |
| D-035 | API-D/ME-002: explain map_editor.diff in detail + testing instructions | Done | Detailed operational walkthrough provided. |
| D-036 | API-D/ME-002: compile a variant with map editor and clarify mouse support | Done | Patch path remap + compile validated; mouse support behavior explained from frontend/game controls. |
| D-037 | Fix patch apply failure for mods/map_editor.diff on reorganized paths | Done | Path-adjusted apply method demonstrated and validated. |
| D-038 | API-003: where map-specific entry/exit text is stored and legacy no-text behavior | Done | Captured in per-map plan artifact with fallback behavior described. |
| D-039 | API-003: commit per-map plan markdown and provide link | Done | Plan file created under docs with timestamped naming. |
| D-040 | API-004: prefer one locale file per language; address pt_BR non-ASCII detail | Done | Localization plan updated with per-file locale architecture and encoding strategy notes. |
| D-041 | API-004: commit localization plan markdown and provide link | Done | Plan file created under docs with timestamped naming. |
| D-042 | API-005: remove “legal determination rests with distributor” wording | Done | Wording removed from tracked docs where requested. |
| D-043 | API-005: avoid “upstream” references unless necessary; document in PROJECT_RULES.md | Done | Documentation style rule inserted into PROJECT_RULES.md. |
| D-044 | API-005: raycastlib clause is memo only, not for repo docs | Done | Removed from LICENSES and adjusted policy placement per instruction. |
| D-045 | Check whether ~/code/research/Anarch was merged and update merge plan doc | Done | Merge history checked and plan document updated with status section. |
| D-046 | List all Python scripts currently in repo | Done | Full list generated via rg --files -g '*.py'. |
| D-047 | Ensure no rollback/revert without authorization | Done | Changes were additive and targeted; no unauthorized rollback performed. |
| D-048 | Keep a comprehensive demands table and revise when incomplete | Done | This revised table expands missing items and preserves status rationale. |
| D-049 | D-011 (legacy row reference): extensive report expected instead of README process log | Done | Covered by D-026 with a dedicated long-form report file and README kept concise/player-facing. |
| D-050 | Explain how to run the Emscripten version | Done | Provided instructions to compile with `make.sh emscripten`, start a local HTTP server, and open in a browser. |
| D-051 | Create a justfile with standard build options and a serve target | Done | Created [justfile](file:///home/lgms/code/research/lhomme-revolte/justfile) containing compile targets and a browser-opening `serve` target. |
| D-052 | Clarify Conventional Commit rules | Done | Documented the types and scopes rules from PROJECT_RULES.md. |
| D-053 | Disallow unscoped Conventional Commits in docs and retry staging/commit | Done | Updated [PROJECT_RULES.md](file:///home/lgms/code/research/lhomme-revolte/PROJECT_RULES.md) to remove `type: summary` and successfully committed the justfile using `build(engine)` scope. |
| D-054 | Add technology descriptions, linking details, and alternative C library builds to README.md | Done | Expanded [README.md](file:///home/lgms/code/research/lhomme-revolte/README.md) with sections for frontends, static/dynamic linking, and musl/dietlibc C library commands. |
| D-055 | Copy and update DEMANDS_STATUS ledger to track current conversation demands | Done | Created [DEMANDS_STATUS_20260627T221706Z.md](file:///home/lgms/code/research/lhomme-revolte/DEMANDS_STATUS_20260627T221706Z.md) with updated rows D-050 to D-056. |
| D-056 | Verify if a justfile target exists to serve the Emscripten version | Done | Confirmed that `serve` is present and targets local serving and automated browser opening. |

## Notes

- Local git exclude changes are intentionally non-trackable in git history.
- Some broad strategic requests remain marked Not done/In progress because no final tracked artifact exists yet in the current repository state.
- This file is the authoritative running ledger for demand coverage in this session.
