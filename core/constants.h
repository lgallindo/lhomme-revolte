/**
  @file constants.h

  This file contains definitions of game constants that are not considered
  part of game settings and whose change can ffect the game balance and
  playability, e.g. physics constants.


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_CONSTANTS_H
#define _LHR_CONSTANTS_H

/**
  How quickly player moves, in squares per second.
*/
#define LHR_PLAYER_MOVE_SPEED 7

/**
  Gravity acceleration in squares / (second^2).
*/
#define LHR_GRAVITY_ACCELERATION 30

/**
  Initial upwards speed of player's jump, in squares per second. 
*/
#define LHR_PLAYER_JUMP_SPEED 5

/**
  Melee and close-up attack range, in RCL_Units.
*/
#define LHR_MELEE_RANGE 1600

/**
  When a projectile is shot, it'll be offset by this distance (in RCL_Units)
  from the shooter.
*/

#define LHR_PROJECTILE_SPAWN_OFFSET 256

/**
  Player's melee hit range, in RCL_Units (RCL_UNITS_PER_SQUARE means full angle,
  180 degrees to both sides).
*/
#define LHR_PLAYER_MELEE_ANGLE 512

/**
  How quickly elevators and squeezers move, in RCL_Unit per second.
*/
#define LHR_MOVING_WALL_SPEED 1024

/**
  How quickly doors open and close, in RCL_Unit per second.
*/
#define LHR_DOOR_OPEN_SPEED 2048

/**
  Helper special state value.
*/
#define LHR_CANT_SAVE 255

/**
  Says the distance in RCL_Units at which level elements (items, monsters etc.)
  are active.
*/
#define LHR_LEVEL_ELEMENT_ACTIVE_DISTANCE (12 * 1024)

/**
  Rate at which AI will be updated, which also affects how fast enemies will
  appear.
*/
#define LHR_AI_FPS 4

/**
  Says a probability (0 - 255) of the AI changing its state during one update
  step.
*/
#define LHR_AI_RANDOM_CHANGE_PROBABILITY 40

/**
  Distance at which level elements (sprites) collide, in RCL_Unit (1024 per
  square).
*/
#define LHR_ELEMENT_COLLISION_RADIUS 1800

/**
  Height, in RCL_Units, at which collisions happen with level elements
  (sprites).
*/
#define LHR_ELEMENT_COLLISION_HEIGHT 1024

/**
  Distance at which explosion does damage and throws away the player, in
  RCL_Units. Should be higher than LHR_ELEMENT_COLLISION_RADIUS so that
  exploded rockets also hurt the target.
*/
#define LHR_EXPLOSION_RADIUS 2000

/**
  Distance in RCL_Units which the player is pushed away by an explosion. Watch
  out, a slightly higher value can make player go through walls. Rather keep
  this under RCL_UNITS_PER_SQUARE;
*/
#define LHR_EXPLOSION_PUSH_AWAY_DISTANCE 1023

/**
  How much damage triggers a barrel explosion.
*/

#define LHR_BARREL_EXPLOSION_DAMAGE_THRESHOLD 3

/**
  Maximum player health.
*/
#define LHR_PLAYER_MAX_HEALTH 125

/**
  Start health of player.
*/
#define LHR_PLAYER_START_HEALTH 255

/**
  At which value health indicator shows a warning (red color).
*/
#define LHR_PLAYER_HEALTH_WARNING_LEVEL 20

/**
  Amount of health that is increased by taking a health kit.
*/
#define LHR_HEALTH_KIT_VALUE 20

/**
  How much randomness (positive and negative) will be added to damage
  (e.g. by weapons, explosions, ...). This constant is is 0 to 255, 255 meaning
  100% of the base value.
*/
#define LHR_DAMAGE_RANDOMNESS 64 

/**
  Height of monster collision BBox in RCL_Units.
*/
#define LHR_MONSTER_COLLISION_HEIGHT 1024

/**
  Specifies key repeat delay, in ms.
*/
#define LHR_KEY_REPEAT_DELAY 500

/**
  Specifies key repeat period, in ms.
*/
#define LHR_KEY_REPEAT_PERIOD 150

/**
  Angle in which multiple projectiles are spread, RCL_Units.
*/
#define LHR_PROJECTILE_SPREAD_ANGLE 100

#define LHR_MAX_MONSTERS 64

#define LHR_MAX_PROJECTILES 12

#define LHR_MAX_DOORS 32

#define LHR_AMMO_BULLETS 0
#define LHR_AMMO_ROCKETS 1
#define LHR_AMMO_PLASMA 2

#define LHR_AMMO_TOTAL 3

#define LHR_AMMO_NONE LHR_AMMO_TOTAL

#define LHR_AMMO_INCREASE_BULLETS 10
#define LHR_AMMO_INCREASE_ROCKETS 5
#define LHR_AMMO_INCREASE_PLASMA 8

#define LHR_AMMO_MAX_BULLETS 200
#define LHR_AMMO_MAX_ROCKETS 100
#define LHR_AMMO_MAX_PLASMA 150

/**
  Duration of story text (intro/outro) in ms.
*/
#define LHR_STORYTEXT_DURATION 15000

/**
  Time in ms of the player death animation.
*/
#define LHR_LOSE_ANIMATION_DURATION 2000

/**
  Time in ms of the level win animation.
*/
#define LHR_WIN_ANIMATION_DURATION 2500

/**
  Time in ms of the level start stage.
*/
#define LHR_LEVEL_START_DURATION 1500

/**
  Vertical sprite size, in RCL_Units.
*/
#define LHR_BASE_SPRITE_SIZE RCL_UNITS_PER_SQUARE

/**
  Default value of the settings byte.
*/
#define LHR_DEFAULT_SETTINGS 0x03

// -----------------------------------------------------------------------------
// derived constants

#define LHR_GAME_RESOLUTION_X \
  (LHR_SCREEN_RESOLUTION_X / LHR_RESOLUTION_SCALEDOWN)

#define LHR_GAME_RESOLUTION_Y \
  (LHR_SCREEN_RESOLUTION_Y / LHR_RESOLUTION_SCALEDOWN)

#define LHR_MS_PER_FRAME (1000 / LHR_FPS) // ms per frame with target FPS

#if LHR_MS_PER_FRAME == 0
  #undef LHR_MS_PER_FRAME
  #define LHR_MS_PER_FRAME 1
#endif

#define LHR_KEY_REPEAT_DELAY_FRAMES \
  (LHR_KEY_REPEAT_DELAY / LHR_MS_PER_FRAME)

#if LHR_KEY_REPEAT_DELAY_FRAMES == 0
  #undef LHR_KEY_REPEAT_DELAY_FRAMES
  #define LHR_KEY_REPEAT_DELAY_FRAMES 1
#endif

#define LHR_KEY_REPEAT_PERIOD_FRAMES \
  (LHR_KEY_REPEAT_PERIOD / LHR_MS_PER_FRAME)

#if LHR_KEY_REPEAT_PERIOD_FRAMES == 0
  #undef LHR_KEY_REPEAT_PERIOD_FRAMES
  #define LHR_KEY_REPEAT_PERIOD_FRAMES 1
#endif

#define LHR_WEAPON_IMAGE_SCALE \
  (LHR_GAME_RESOLUTION_X / (LHR_TEXTURE_SIZE * 5))

#if LHR_WEAPON_IMAGE_SCALE == 0
  #undef LHR_WEAPON_IMAGE_SCALE
  #define LHR_WEAPON_IMAGE_SCALE 1
#endif

#define LHR_WEAPONBOB_OFFSET_PIXELS \
  (LHR_WEAPONBOB_OFFSET * LHR_WEAPON_IMAGE_SCALE)

#define LHR_WEAPON_IMAGE_POSITION_X \
  (LHR_GAME_RESOLUTION_X / 2 - (LHR_WEAPON_IMAGE_SCALE * LHR_TEXTURE_SIZE) / 2)

#if LHR_GAME_RESOLUTION_Y > 70
  #define LHR_WEAPON_IMAGE_POSITION_Y \
    (LHR_GAME_RESOLUTION_Y - (LHR_WEAPON_IMAGE_SCALE * LHR_TEXTURE_SIZE))
#elif LHR_GAME_RESOLUTION_Y > 50
  #define LHR_WEAPON_IMAGE_POSITION_Y (LHR_GAME_RESOLUTION_Y \
    - ((LHR_WEAPON_IMAGE_SCALE * 3 * LHR_TEXTURE_SIZE) / 4))
#else
  #define LHR_WEAPON_IMAGE_POSITION_Y \
    (LHR_GAME_RESOLUTION_Y - LHR_TEXTURE_SIZE / 2)
#endif

#define LHR_PLAYER_TURN_UNITS_PER_FRAME \
  ((LHR_PLAYER_TURN_SPEED * RCL_UNITS_PER_SQUARE) / (360 * LHR_FPS))

#if LHR_PLAYER_TURN_UNITS_PER_FRAME == 0
  #undef LHR_PLAYER_TURN_UNITS_PER_FRAME
  #define LHR_PLAYER_TURN_UNITS_PER_FRAME 1
#endif

#define LHR_PLAYER_MOVE_UNITS_PER_FRAME \
  ((LHR_PLAYER_MOVE_SPEED * RCL_UNITS_PER_SQUARE) / LHR_FPS)

#if LHR_PLAYER_MOVE_UNITS_PER_FRAME == 0
  #undef LHR_PLAYER_MOVE_UNITS_PER_FRAME
  #define LHR_PLAYER_MOVE_UNITS_PER_FRAME 1
#endif

#define LHR_GRAVITY_SPEED_INCREASE_PER_FRAME \
  ((LHR_GRAVITY_ACCELERATION * RCL_UNITS_PER_SQUARE) / (LHR_FPS * LHR_FPS))

#if LHR_GRAVITY_SPEED_INCREASE_PER_FRAME == 0
  #undef LHR_GRAVITY_SPEED_INCREASE_PER_FRAME
  #define LHR_GRAVITY_SPEED_INCREASE_PER_FRAME 1
#endif

#define LHR_PLAYER_JUMP_OFFSET_PER_FRAME \
  (((LHR_PLAYER_JUMP_SPEED * RCL_UNITS_PER_SQUARE) / LHR_FPS) \
  - LHR_GRAVITY_SPEED_INCREASE_PER_FRAME / 2) 
  /* ^ This subtraction corrects the initial veloc. so that the numeric curve
     copies the analytical (smooth) curve. Without it the numeric curve goes
     ABOVE and makes player jump higher with lower FPS. To make sense of this
     try to solve the differential equation and plot it. */

#if LHR_PLAYER_JUMP_OFFSET_PER_FRAME == 0
  #undef LHR_PLAYER_JUMP_OFFSET_PER_FRAME
  #define LHR_PLAYER_JUMP_OFFSET_PER_FRAME 1
#endif

#define LHR_HEADBOB_FRAME_INCREASE_PER_FRAME \
  (LHR_HEADBOB_SPEED / LHR_FPS)

#if LHR_HEADBOB_FRAME_INCREASE_PER_FRAME == 0
  #undef LHR_HEADBOB_FRAME_INCREASE_PER_FRAME
  #define LHR_HEADBOB_FRAME_INCREASE_PER_FRAME 1
#endif

#define LHR_HEADBOB_ENABLED (LHR_HEADBOB_SPEED > 0 && LHR_HEADBOB_OFFSET > 0)

#define LHR_CAMERA_SHEAR_STEP_PER_FRAME \
  ((LHR_GAME_RESOLUTION_Y * LHR_CAMERA_SHEAR_SPEED) / LHR_FPS)

#if LHR_CAMERA_SHEAR_STEP_PER_FRAME == 0
  #undef LHR_CAMERA_SHEAR_STEP_PER_FRAME
  #define LHR_CAMERA_SHEAR_STEP_PER_FRAME 1
#endif

#define LHR_CAMERA_MAX_SHEAR_PIXELS \
  ((LHR_CAMERA_MAX_SHEAR * LHR_GAME_RESOLUTION_Y) / 1024)

#define LHR_FONT_SIZE_SMALL \
 (LHR_GAME_RESOLUTION_X / (LHR_FONT_CHARACTER_SIZE * 50))

#if LHR_FONT_SIZE_SMALL == 0
  #undef LHR_FONT_SIZE_SMALL
  #define LHR_FONT_SIZE_SMALL 1
#endif

#define LHR_FONT_SIZE_MEDIUM \
  (LHR_GAME_RESOLUTION_X / (LHR_FONT_CHARACTER_SIZE * 30))

#if LHR_FONT_SIZE_MEDIUM == 0
  #undef LHR_FONT_SIZE_MEDIUM
  #define LHR_FONT_SIZE_MEDIUM 1
#endif

#define LHR_FONT_SIZE_BIG \
  (LHR_GAME_RESOLUTION_X / (LHR_FONT_CHARACTER_SIZE * 18))

#if LHR_FONT_SIZE_BIG == 0
  #undef LHR_FONT_SIZE_BIG
  #define LHR_FONT_SIZE_BIG 1
#endif

#define LHR_Z_BUFFER_SIZE LHR_GAME_RESOLUTION_X

/**
  Step in which walls get higher, in raycastlib units.
*/
#define LHR_WALL_HEIGHT_STEP (RCL_UNITS_PER_SQUARE / 4)

#define LHR_CEILING_MAX_HEIGHT\
  (16 * RCL_UNITS_PER_SQUARE - RCL_UNITS_PER_SQUARE / 2 )

#define LHR_DOOR_UP_DOWN_MASK 0x20
#define LHR_DOOR_LOCK(doorRecord) ((doorRecord) >> 6)
#define LHR_DOOR_VERTICAL_POSITION_MASK 0x1f
#define LHR_DOOR_HEIGHT_STEP (RCL_UNITS_PER_SQUARE / 0x1f)

#define LHR_DOOR_INCREMENT_PER_FRAME \
  (LHR_DOOR_OPEN_SPEED / (LHR_DOOR_HEIGHT_STEP * LHR_FPS))

#if LHR_DOOR_INCREMENT_PER_FRAME == 0
  #undef LHR_DOOR_INCREMENT_PER_FRAME
  #define LHR_DOOR_INCREMENT_PER_FRAME 1
#endif

#define LHR_MAX_ITEMS LHR_MAX_LEVEL_ELEMENTS

#define LHR_MAX_SPRITE_SIZE LHR_GAME_RESOLUTION_X

#define LHR_MAP_PIXEL_SIZE (LHR_GAME_RESOLUTION_Y / LHR_MAP_SIZE)

#if LHR_MAP_PIXEL_SIZE == 0
  #undef LHR_MAP_PIXEL_SIZE
  #define LHR_MAP_PIXEL_SIZE 1
#endif

#define LHR_AI_UPDATE_FRAME_INTERVAL \
  (LHR_FPS / LHR_AI_FPS)

#if LHR_AI_UPDATE_FRAME_INTERVAL == 0
  #undef LHR_AI_UPDATE_FRAME_INTERVAL
  #define LHR_AI_UPDATE_FRAME_INTERVAL 1
#endif

#define LHR_SPRITE_ANIMATION_FRAME_DURATION \
  (LHR_FPS / LHR_SPRITE_ANIMATION_SPEED)

#if LHR_SPRITE_ANIMATION_FRAME_DURATION == 0
  #undef LHR_SPRITE_ANIMATION_FRAME_DURATION
  #define LHR_SPRITE_ANIMATION_FRAME_DURATION 1
#endif

#define LHR_HUD_MARGIN (LHR_GAME_RESOLUTION_X / 40)

#define LHR_HUD_BORDER_INDICATOR_WIDTH_PIXELS \
  (LHR_GAME_RESOLUTION_Y / LHR_HUD_BORDER_INDICATOR_WIDTH)

#define LHR_HUD_BORDER_INDICATOR_DURATION_FRAMES \
  (LHR_HUD_BORDER_INDICATOR_DURATION / LHR_MS_PER_FRAME)

#if LHR_HUD_BORDER_INDICATOR_DURATION_FRAMES == 0
  #define LHR_HUD_BORDER_INDICATOR_DURATION_FRAMES 1
#endif

#define LHR_BLINK_PERIOD_FRAMES (LHR_BLINK_PERIOD / LHR_MS_PER_FRAME)

#define LHR_HUD_BAR_HEIGHT \
  (LHR_FONT_CHARACTER_SIZE * LHR_FONT_SIZE_MEDIUM + LHR_HUD_MARGIN * 2 + 1)

// -----------------------------------------------------------------------------
// monsters

#define LHR_MONSTER_ATTACK_MELEE 0
#define LHR_MONSTER_ATTACK_FIREBALL 1
#define LHR_MONSTER_ATTACK_BULLET 2
#define LHR_MONSTER_ATTACK_FIREBALL_BULLET 3
#define LHR_MONSTER_ATTACK_PLASMA 4
#define LHR_MONSTER_ATTACK_EXPLODE 5
#define LHR_MONSTER_ATTACK_FIREBALL_PLASMA 6

#define LHR_MONSTER_ATTRIBUTE(attackType,aggressivity0to255,health0to255,spriteSize0to3) \
  ((uint16_t) ( \
   attackType | \
   ((aggressivity0to255 / 8) << 3) | \
   (spriteSize0to3 << 8) | \
   ((health0to255 / 4) << 10)))

#define LHR_GET_MONSTER_ATTACK_TYPE(monsterNumber) \
  (LHR_monsterAttributeTable[monsterNumber] & 0x0007)

#define LHR_GET_MONSTER_AGGRESSIVITY(monsterNumber) \
  (((LHR_monsterAttributeTable[monsterNumber] >> 3) & 0x1F) * 8)

#define LHR_GET_MONSTER_SPRITE_SIZE(monsterNumber) \
  ((LHR_monsterAttributeTable[monsterNumber] >> 8) & 0x03)

#define LHR_GET_MONSTER_MAX_HEALTH(monsterNumber) \
  (((LHR_monsterAttributeTable[monsterNumber] >> 10) & 0x3F) * 4)

/**
  Table of monster attributes, each as a 16bit word in format:

  MSB hhhhhhssaaaattt LSB

  ttt:    attack type
  aaaaa:  aggressivity (frequence of attacks), 0 to 31
  ss:     sprite size
  hhhhhh: health, 0 to 63
*/
uint16_t LHR_monsterAttributeTable[LHR_MONSTERS_TOTAL] =
{
  /* spider  */ LHR_MONSTER_ATTRIBUTE(LHR_MONSTER_ATTACK_FIREBALL,40,61,2),
  /* destr.  */ LHR_MONSTER_ATTRIBUTE(LHR_MONSTER_ATTACK_FIREBALL_BULLET,90,170,3),
  /* warrior */ LHR_MONSTER_ATTRIBUTE(LHR_MONSTER_ATTACK_MELEE,255,40,1),
  /* plasma  */ LHR_MONSTER_ATTRIBUTE(LHR_MONSTER_ATTACK_PLASMA,56,92,1),
  /* ender   */ LHR_MONSTER_ATTRIBUTE(LHR_MONSTER_ATTACK_FIREBALL_PLASMA,128,255,3),
  /* turret  */ LHR_MONSTER_ATTRIBUTE(LHR_MONSTER_ATTACK_BULLET,32,23,0),
  /* explod. */ LHR_MONSTER_ATTRIBUTE(LHR_MONSTER_ATTACK_EXPLODE,255,36,1)
};

// -----------------------------------------------------------------------------
// weapons and projectiles

#define LHR_WEAPON_KNIFE 0
#define LHR_WEAPON_SHOTGUN 1
#define LHR_WEAPON_MACHINE_GUN 2
#define LHR_WEAPON_ROCKET_LAUNCHER 3
#define LHR_WEAPON_PLASMAGUN 4
#define LHR_WEAPON_SOLUTION 5

#define LHR_WEAPONS_TOTAL 6

#define LHR_WEAPON_ATTRIBUTE(fireType,projectileCount,fireCooldownMs) \
  ((uint8_t) (fireType | ((projectileCount - 1) << 2) | ((fireCooldownMs / (LHR_MS_PER_FRAME * 16)) << 4)))

#define LHR_GET_WEAPON_FIRE_TYPE(weaponNumber) \
  (LHR_weaponAttributeTable[weaponNumber] & 0x03)

#define LHR_GET_WEAPON_FIRE_COOLDOWN_FRAMES(weaponNumber) \
  ((LHR_weaponAttributeTable[weaponNumber] >> 4) * 16)

#define LHR_GET_WEAPON_PROJECTILE_COUNT(weaponNumber) \
  (((LHR_weaponAttributeTable[weaponNumber] >> 2) & 0x03) + 1)

#define LHR_MIN_WEAPON_COOLDOWN_FRAMES 8

#define LHR_WEAPON_FIRE_TYPE_MELEE 0
#define LHR_WEAPON_FIRE_TYPE_BULLET 1
#define LHR_WEAPON_FIRE_TYPE_FIREBALL 2
#define LHR_WEAPON_FIRE_TYPE_PLASMA 3

#define LHR_WEAPON_FIRE_TYPES_TOTAL 4

/**
  Table of weapon attributes, each as a byte in format:

  MSB ccccnnff LSB

  ff:     fire type
  nn:     number of projectiles - 1
  cccc:   fire cooldown in frames, i.e. time after which the next shot can be
          shot again, ccccc has to be multiplied by 16 to get the real value
*/
static const uint8_t LHR_weaponAttributeTable[LHR_WEAPONS_TOTAL] =
{
  /* knife    */ LHR_WEAPON_ATTRIBUTE(LHR_WEAPON_FIRE_TYPE_MELEE,1,650),    // DPS: 6.2
  /* shotgun  */ LHR_WEAPON_ATTRIBUTE(LHR_WEAPON_FIRE_TYPE_BULLET,2,1250),  // DPS: 12.8
  /* m. gun   */ LHR_WEAPON_ATTRIBUTE(LHR_WEAPON_FIRE_TYPE_BULLET,1,700),   // DPS: 11.4
  /* r. laun. */ LHR_WEAPON_ATTRIBUTE(LHR_WEAPON_FIRE_TYPE_FIREBALL,1,850), // DPS: 28.2
  /* plasma   */ LHR_WEAPON_ATTRIBUTE(LHR_WEAPON_FIRE_TYPE_PLASMA,1,550),   // DPS: 32.7
  /* solution */ LHR_WEAPON_ATTRIBUTE(LHR_WEAPON_FIRE_TYPE_PLASMA,4,1050)   // DPS: 85.7
};

static const uint8_t LHR_attackDamageTable[LHR_WEAPON_FIRE_TYPES_TOTAL] =
{
  /* melee                 */ 4,
  /* bullet                */ 8,
  /* explostion (fireball) */ 24,
  /* plasma                */ 18
};

#define LHR_PROJECTILE_EXPLOSION 0
#define LHR_PROJECTILE_FIREBALL 1
#define LHR_PROJECTILE_PLASMA 2
#define LHR_PROJECTILE_DUST 3
#define LHR_PROJECTILE_BULLET 4
#define LHR_PROJECTILE_NONE 255

#define LHR_PROJECTILES_TOTAL 5

#define LHR_PROJECTILE_ATTRIBUTE(speedSquaresPerSec,timeToLiveMs) \
  ((uint8_t) \
   ((((speedSquaresPerSec / 4 == 0) && (speedSquaresPerSec != 0)) ? 1 : speedSquaresPerSec / 4) | \
    ((timeToLiveMs / (8 * LHR_MS_PER_FRAME)) << 3)))

#define LHR_GET_PROJECTILE_SPEED_UPF(projectileNumber) \
  (((LHR_projectileAttributeTable[projectileNumber] & 0x07) * 4 * RCL_UNITS_PER_SQUARE) / LHR_FPS)

#define LHR_GET_PROJECTILE_FRAMES_TO_LIVE(projectileNumber) \
  ((LHR_projectileAttributeTable[projectileNumber] >> 3) * 8)

/**
  Table of projectile attributes, each as a byte in format:

  MSB lllllsss LSB

  fff:   half speed in game squares per second
  lllll: eigth of frames to live
*/

#define LOW_FPS (LHR_FPS < 24) ///< low FPS needs low speeds, because collisions

static const uint8_t LHR_projectileAttributeTable[LHR_PROJECTILES_TOTAL] =
{
  /* explosion */ LHR_PROJECTILE_ATTRIBUTE(0,400),
  /* fireball  */ LHR_PROJECTILE_ATTRIBUTE(10,1000),

#if LOW_FPS
  /* plasma    */ LHR_PROJECTILE_ATTRIBUTE(17,500),
#else
  /* plasma    */ LHR_PROJECTILE_ATTRIBUTE(18,500),
#endif

  /* dust      */ LHR_PROJECTILE_ATTRIBUTE(0,450),

#if LOW_FPS
  /* bullet    */ LHR_PROJECTILE_ATTRIBUTE(17,1000)
#else
  /* bullet    */ LHR_PROJECTILE_ATTRIBUTE(28,1000)
#endif
};

#undef LOW_FPS

#endif // guard
