/**
  @file settings.h

  This file contains settings (or setting hints) for the game. Values here can
  fine tune performance vs quality and personalize the final compiled game. Some
  of these settings may be overriden by the specific platform used according to
  its limitations. You are advised to NOT change value in this file directly,
  but rather predefine them somewhere before this file gets included, e.g. in
  you personal settings file.


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_SETTINGS_H
#define _LHR_SETTINGS_H

/**
  Path to the save file. You may want to set this to some absolute path,
  depending on your system.
*/
#ifndef LHR_SAVE_FILE_PATH
  #define LHR_SAVE_FILE_PATH "revolte.sav"
#endif

/**
  Time multiplier in LHR_Units (1.0 == 1024). This can be used to slow down or
  speed up the game. Note that this also changes the rendering FPS accordingly
  (e.g. half FPS at half speed), so if you want to keep the FPS, divide it by
  the multiplier value.
*/
#ifndef LHR_TIME_MULTIPLIER
  #define LHR_TIME_MULTIPLIER 1024
#endif

/**
  Target FPS (frames per second). This sets the game logic FPS and will try to
  render at the same rate. If such fast rendering can't be achieved, frames will
  be droped, but the game logic will still be forced to run at this speed, so
  the game may actually become slowed down if FPS is set too high. Too high or
  too low FPS can also negatively affect game speeds which are computed as
  integers and rounding errors can occur soon, so don't set this to extreme
  values (try to keep between 20 to 100). FPS also determines the game
  simulation step length, so different FPS values may result in very slight
  differences in game behavior (not very noticeable but affecting demos etc.).
*/
#ifndef LHR_FPS
  #define LHR_FPS 60
#endif

/**
  Increases or decreases the brightness of the rendered world (but not menu,
  HUD etc.). Effective values are -8 to 8.
*/
#ifndef LHR_BRIGHTNESS
  #define LHR_BRIGHTNESS 0
#endif

/**
  On platforms with mouse this sets its horizontal sensitivity. 128 means 1
  RCL_Unit turn angle per mouse pixel travelled.
*/
#ifndef LHR_MOUSE_SENSITIVITY_HORIZONTAL
  #define LHR_MOUSE_SENSITIVITY_HORIZONTAL 32
#endif

/**
  Like LHR_MOUSE_SENSITIVITY_HORIZONTAL but for vertical look. 128 means 1
  shear pixel per mouse pixel travelled.
*/
#ifndef LHR_MOUSE_SENSITIVITY_VERTICAL
  #define LHR_MOUSE_SENSITIVITY_VERTICAL 64
#endif

/**
  Width of the screen in pixels. Set this to ACTUAL resolution. If you want the
  game to run at smaller resolution (with bigger pixels), do this using
  LHR_RESOLUTION_SCALEDOWN. 
*/
#ifndef LHR_SCREEN_RESOLUTION_X
  #define LHR_SCREEN_RESOLUTION_X 800
#endif

/**
  Like LHR_SCREEN_RESOLUTION_X, but for y resolution.
*/
#ifndef LHR_SCREEN_RESOLUTION_Y
  #define LHR_SCREEN_RESOLUTION_Y 600
#endif

/**
  How quickly player turns left/right, in degrees per second.
*/
#ifndef LHR_PLAYER_TURN_SPEED
  #define LHR_PLAYER_TURN_SPEED 180
#endif

/**
  Horizontal FOV (field of vision) in RCL_Units (1024 means 360 degrees).
*/
#ifndef LHR_FOV_HORIZONTAL
  #define LHR_FOV_HORIZONTAL 256
#endif

/**
  Like LHR_FOV_HORIZONTAL but for vertical angle.
*/
#ifndef LHR_FOV_VERTICAL
  #define LHR_FOV_VERTICAL 330
#endif

/**
  Distance, in RCL_Units, to which textures will be drawn. Textures behind this
  distance will be replaced by an average constant color, which maybe can help
  performance and also serves as an antialiasim (2 level MIP map). Value 0 turns
  texturing completely off, which is much faster than having just a low value,
  values >= 65535 activate texturing completely, which can be a little faster
  than setting having a high value lower than this limit.
*/
#ifndef LHR_TEXTURE_DISTANCE
  #define LHR_TEXTURE_DISTANCE 100000
#endif

/**
  How many times the screen resolution will be divided (how many times a game
  pixel will be bigger than the screen pixel).
*/
#ifndef LHR_RESOLUTION_SCALEDOWN
  #define LHR_RESOLUTION_SCALEDOWN 1
#endif

/**
  Multiplier, in RCL_Units (1024 == 1.0), of the damager player takes. This can
  be used to balance difficulty.
*/
#ifndef LHR_PLAYER_DAMAGE_MULTIPLIER
  #define LHR_PLAYER_DAMAGE_MULTIPLIER 512
#endif

/**
  Hint as to whether to run in fullscreen, if the platform allows it.
*/
#ifndef LHR_FULLSCREEN
  #define LHR_FULLSCREEN 0
#endif

/**
  Whether shadows (fog) should be dithered, i.e. more smooth (needs a bit more
  CPU performance and memory).
*/
#ifndef LHR_DITHERED_SHADOW
  #define LHR_DITHERED_SHADOW 0
#endif

/**
  Depth step (in RCL_Units) after which fog diminishes a color by one value
  point. For performance reasons this number should be kept a power of two!
*/
#ifndef LHR_FOG_DIMINISH_STEP
  #define LHR_FOG_DIMINISH_STEP 2048
#endif

/**
  If set, floor and ceiling will be colored differently depending on their
  height. This can be useful when fog is turned on and different floor levels
  are hard to distinguish.
*/
#ifndef LHR_DIFFERENT_FLOOR_CEILING_COLORS
  #define LHR_DIFFERENT_FLOOR_CEILING_COLORS 0
#endif

/**
  Maximum number of squares that will be traversed by any cast ray. Smaller
  number is faster but can cause visual artifacts.
*/
#ifndef LHR_RAYCASTING_MAX_STEPS
  #define LHR_RAYCASTING_MAX_STEPS 30
#endif

/**
  Maximum number of hits any cast ray will register. Smaller number is faster
  but can cause visual artifacts.
*/
#ifndef LHR_RAYCASTING_MAX_HITS
  #define LHR_RAYCASTING_MAX_HITS 10
#endif

/**
  Same as LHR_RAYCASTING_MAX_STEPS but for visibility rays that are used to
  check whether sprites are visible etc.
*/
#ifndef LHR_RAYCASTING_VISIBILITY_MAX_STEPS
  #if LHR_RAYCASTING_MAX_STEPS < 15
    #define LHR_RAYCASTING_VISIBILITY_MAX_STEPS 15
  #else
    #define LHR_RAYCASTING_VISIBILITY_MAX_STEPS LHR_RAYCASTING_MAX_STEPS
  #endif
#endif

/**
  Same as LHR_RAYCASTING_MAX_HITS but for visibility rays that are used to check
  whether sprites are visible etc.
*/
#ifndef LHR_RAYCASTING_VISIBILITY_MAX_HITS
  #if LHR_RAYCASTING_MAX_HITS < 6
    #define LHR_RAYCASTING_VISIBILITY_MAX_HITS 6
  #else
    #define LHR_RAYCASTING_VISIBILITY_MAX_HITS LHR_RAYCASTING_MAX_HITS
  #endif
#endif

/**
  How many times rendering should be subsampled horizontally. Bigger number
  can significantly improve performance (by casting fewer rays), but can look
  a little worse. This number should be a divisor of LHR_SCREEN_RESOLUTION_X!
*/
#ifndef LHR_RAYCASTING_SUBSAMPLE
  #define LHR_RAYCASTING_SUBSAMPLE 1
#endif

/**
  Enables or disables fog (darkness) due to distance. Recommended to keep on
  for good look, but can be turned off for performance.
*/
#ifndef LHR_ENABLE_FOG
  #define LHR_ENABLE_FOG 1
#endif

/**
  Says whether sprites should diminish in fog. This takes more performance but
  looks better.
*/
#ifndef LHR_DIMINISH_SPRITES
  #define LHR_DIMINISH_SPRITES 1
#endif

/**
  How quick player head bob is, 1024 meaning once per second. 0 Means turn off
  head bob.
*/
#ifndef LHR_HEADBOB_SPEED
  #define LHR_HEADBOB_SPEED 900
#endif

/**
  Sets head bob offset, in RCL_UNITS_PER_SQUARE. 0 Means turn off head bob.
*/
#ifndef LHR_HEADBOB_OFFSET
  #define LHR_HEADBOB_OFFSET 200
#endif

/**
  If head bob is on, this additionally sets additional camera shear bob, in
  pixels, which can make bobbing look more "advanced". 0 turns this option off.
*/
#ifndef LHR_HEADBOB_SHEAR
  #define LHR_HEADBOB_SHEAR 0
#endif

/**
  Weapon bobbing offset in weapon image pixels.
*/
#ifndef LHR_WEAPONBOB_OFFSET
  #define LHR_WEAPONBOB_OFFSET 4 
#endif

/**
  Camera shearing (looking up/down) speed, in vertical resolutions per second.
*/
#ifndef LHR_CAMERA_SHEAR_SPEED
  #define LHR_CAMERA_SHEAR_SPEED 3
#endif

/**
  Maximum camera shear (vertical angle). 1024 means 1.0 * vertical resolution.
*/
#ifndef LHR_CAMERA_MAX_SHEAR
  #define LHR_CAMERA_MAX_SHEAR 1024
#endif

/**
  Specifies how quick some sprite animations are, in frames per second.
*/
#ifndef LHR_SPRITE_ANIMATION_SPEED
  #define LHR_SPRITE_ANIMATION_SPEED 4
#endif

/**
  How wide the border indicator is, in fractions of screen width.
*/
#ifndef LHR_HUD_BORDER_INDICATOR_WIDTH
  #define LHR_HUD_BORDER_INDICATOR_WIDTH 32
#endif

/**
  For how long border indication (being hurt etc.) stays shown, in ms.
*/
#ifndef LHR_HUD_BORDER_INDICATOR_DURATION
  #define LHR_HUD_BORDER_INDICATOR_DURATION 500
#endif

/**
  Color (palette index) by which being hurt is indicated.
*/
#ifndef LHR_HUD_HURT_INDICATION_COLOR
  #define LHR_HUD_HURT_INDICATION_COLOR 175
#endif

/**
  Color (palette index) by which taking an item is indicated.
*/
#ifndef LHR_HUD_ITEM_TAKEN_INDICATION_COLOR
  #define LHR_HUD_ITEM_TAKEN_INDICATION_COLOR 207
#endif

/**
  How many element (items, monsters, ...) distances will be checked per frame
  for distance. Higher value may decrease performance a tiny bit, but things
  will react more quickly and appear less "out of thin air".
*/
#ifndef LHR_ELEMENT_DISTANCES_CHECKED_PER_FRAME
  #define LHR_ELEMENT_DISTANCES_CHECKED_PER_FRAME 8
#endif

/**
  Maximum distance at which sound effects (SFX) will be played. The SFX volume
  will gradually drop towards this distance.
*/
#ifndef LHR_SFX_MAX_DISTANCE
  #define LHR_SFX_MAX_DISTANCE (1024 * 60)
#endif

/**
  Says the intensity of background image blur. 0 means no blur, which improves
  performance and lowers memory usage. Blur doesn't look very good in small
  resolutions.
*/
#ifndef LHR_BACKGROUND_BLUR
  #define LHR_BACKGROUND_BLUR 0
#endif

/**
  Defines the period, in ms, of things that blink, such as text.
*/
#ifndef LHR_BLINK_PERIOD
  #define LHR_BLINK_PERIOD 500
#endif

/**
  Probability (0 - 255) of how often a monster makes sound during movement.
*/
#ifndef LHR_MONSTER_SOUND_PROBABILITY
  #define LHR_MONSTER_SOUND_PROBABILITY 64
#endif

/**
  Affects how precise monsters are in aiming, specify random range in
  fourths of a game square. Should be power of 2 for performance.
*/
#ifndef LHR_MONSTER_AIM_RANDOMNESS
  #define LHR_MONSTER_AIM_RANDOMNESS 4
#endif

/// Color 1 index of player on map.
#ifndef LHR_MAP_PLAYER_COLOR1
  #define LHR_MAP_PLAYER_COLOR1 93
#endif

/// Color 2 index of player on map.
#ifndef LHR_MAP_PLAYER_COLOR2
  #define LHR_MAP_PLAYER_COLOR2 111
#endif

/// Color index of elevators on map.
#ifndef LHR_MAP_ELEVATOR_COLOR
  #define LHR_MAP_ELEVATOR_COLOR 214
#endif

/// Color index of squeezers on map.
#ifndef LHR_MAP_SQUEEZER_COLOR
  #define LHR_MAP_SQUEEZER_COLOR 246
#endif

/// Color index of door on map.
#ifndef LHR_MAP_DOOR_COLOR
  #define LHR_MAP_DOOR_COLOR 188
#endif

/**
  Boolean value indicating whether current OS is malware.
*/
#ifndef LHR_OS_IS_MALWARE
  #define LHR_OS_IS_MALWARE 0
#endif

/**
  Angle difference, as a cos value in RCL_Units, between the player and a
  monster, at which vertical autoaim will trigger. If the angle is greater, a
  shot will go directly forward.
*/
#ifndef LHR_VERTICAL_AUTOAIM_ANGLE_THRESHOLD
  #define LHR_VERTICAL_AUTOAIM_ANGLE_THRESHOLD 50
#endif

/**
  Byte (0 - 255) volume of the menu click sound.
*/
#ifndef LHR_MENU_CLICK_VOLUME
  #define LHR_MENU_CLICK_VOLUME 220
#endif

/**
  Says whether the exit item should be showed in the menu. Platforms that can't
  exit (such as some gaming consoles that simply use power off button) can
  define this to 0.
*/
#ifndef LHR_CAN_EXIT
  #define LHR_CAN_EXIT 1
#endif

#ifndef LHR_PC
  #define LHR_PC 0
#endif

/**
  On Arduino platforms this should be set to 1. That will cause some special
  treatment regarding constant variables and PROGMEM.
*/
#ifndef LHR_ARDUINO
  #define LHR_ARDUINO 0
#endif

/**
  Whether levels background (in distance or transparent wall textures) should
  be drawn. If turned off, the background will be constant color, which can 
  noticably increase performance.
*/
#ifndef LHR_DRAW_LEVEL_BACKGROUND
  #define LHR_DRAW_LEVEL_BACKGROUND 1
#endif

/**
  Says the size, in pixels, of a sprite when it is closest to the camera, which
  is the maximum size that can be drawn. Sprites on "weird" aspect ratios can
  look weirdly scaled, so this option can be used to fix that (typically set
  horizontal screen resolution instead of vertical).
*/
#ifndef LHR_SPRITE_MAX_SIZE
  #define LHR_SPRITE_MAX_SIZE \
    (LHR_SCREEN_RESOLUTION_Y / LHR_RESOLUTION_SCALEDOWN)
#endif

/**
  If set, single item menu will be forced.
*/
#ifndef LHR_FORCE_SINGLE_ITEM_MENU
  #define LHR_FORCE_SINGLE_ITEM_MENU 0
#endif

//------ developer/debug settings ------

/**
  Developer cheat for having infinite ammo in all weapons.
*/
#ifndef LHR_INFINITE_AMMO
  #define LHR_INFINITE_AMMO 0
#endif

/**
  Developer cheat for immortality.
*/
#ifndef LHR_IMMORTAL
  #define LHR_IMMORTAL 0
#endif

/**
  Developer setting, with 1 every level is won immediately after start.
*/
#ifndef LHR_QUICK_WIN
  #define LHR_QUICK_WIN 0
#endif

/**
  Reveals all levels to be played.
*/
#ifndef LHR_ALL_LEVELS
  #define LHR_ALL_LEVELS 0
#endif

/**
  Turn on for previes mode for map editing (flying, noclip, fast movement etc.).
*/
#ifndef LHR_PREVIEW_MODE
  #define LHR_PREVIEW_MODE 0
#endif

/**
  How much faster movement is in the preview mode.
*/
#ifndef LHR_PREVIEW_MODE_SPEED_MULTIPLIER
  #define LHR_PREVIEW_MODE_SPEED_MULTIPLIER 2
#endif

/**
  Skips menu and starts given level immediatelly, for development. 0 means this
  options is ignored, 1 means load level 1 etc.
*/
#ifndef LHR_START_LEVEL
  #define LHR_START_LEVEL 0
#endif

/**
  Reveals whole level map from start.
*/
#ifndef LHR_REVEAL_MAP
  #define LHR_REVEAL_MAP 0
#endif

/**
  Gives player all keys from start.
*/
#ifndef LHR_UNLOCK_DOOR
  #define LHR_UNLOCK_DOOR 0
#endif

#endif // guard
