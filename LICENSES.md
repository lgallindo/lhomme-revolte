# L'Homme Revolte Licensing Matrix

This matrix defines licensing policy for this fork.

## Principles

- Legacy upstream code keeps original in-file notices and provenance as-is.
- New code added in this fork is GPL-3.0-or-later.
- Legacy assets remain CC0 / Public Domain.
- New assets added in this fork are CC BY-NC-SA 4.0.

## Scope Matrix

| ID | Scope | License Classification | Notes |
| --- | --- | --- | --- |
| LM-001 | Legacy upstream code portions | CC0 / Public Domain (upstream); legal determination for the fork rests with the distributor | Keep historical notices untouched. |
| LM-002 | New fork code (all code added or substantially modified in this fork) | GPL-3.0-or-later | |
| LM-003 | `core/raycastlib.h` (Miloslav Číž) | CC0 / Public Domain unless major rewrite occurs | Retain upstream notice; track any major rewrite in commit messages. |
| LM-004 | Legacy upstream assets | CC0 / Public Domain | Existing upstream asset licensing retained. |
| LM-005 | New fork assets | CC BY-NC-SA 4.0 | Applies to newly added fork-era media/assets. |

## GPL Code Files (Fork Contributions)

The following tracked code/script/build files have been authored or substantially modified in this fork and are therefore classified as GPL-3.0-or-later:

- Doxyfile
- Makefile
- assets/font2array.py
- assets/img2array.py
- assets/img2map.py
- assets/snd2array.py
- core/My_settings.h
- core/constants.h
- core/game.h
- core/images.h
- core/levels.h
- core/mbed_config.h
- core/new_logo.h
- core/palette.h
- core/raycastlib.h
- core/saf.h
- core/settings.h
- core/smallinput.h
- core/sounds.h
- core/texts.h
- core/wipe_effect.h
- frontends/embedded/main_espboy.ino
- frontends/embedded/main_gbmeta.ino
- frontends/embedded/main_nibble.ino
- frontends/embedded/main_pokitto.cpp
- frontends/embedded/main_ringo.ino
- frontends/pc/main_csfml.c
- frontends/pc/main_ncurses.c
- frontends/pc/main_saf.c
- frontends/pc/main_sdl.c
- frontends/pc/main_sdl1.c
- frontends/pc/main_terminal.c
- frontends/pc/main_x11.c
- mods/consoomer_edition.sh
- mods/makeMapImages.c
- mods/makeMapModel.c
- tests/main_test.c
- tests/test_warp_cli.sh
- tests/test_wipe.c
- tools/check_color.py
- tools/fix_logo.py
- tools/gif2map/gif2map.c
- tools/gif2map/gif2map.h
- tools/gif2map/gif2map_cli.c
- tools/gif2map/gif2map_mcp.c
- tools/gif2map/test_gif2map.c
- tools/make.sh
- tools/make_alpha_logo.py
- tools/make_menu_logo.py
- tools/patch_logo.py
- tools/resize.py
- tools/upscale_promo.py

## Asset Classification Notes

- Legacy assets (upstream-era) remain CC0 / Public Domain.
- New fork-era assets are CC BY-NC-SA 4.0.
- If a legacy asset is modified in this fork, retain provenance notes and document the fork contribution explicitly in commit messages and release notes.

## Raycastlib Note

`core/raycastlib.h` is the original work of Miloslav Číž and was released by the upstream author under CC0 / Public Domain. It must remain under CC0 / Public Domain in this repository unless a major rewrite of its logic is performed in this fork. In that event, the rewritten portions are GPL-3.0-or-later and must be clearly delineated in the file and noted in commit history.
