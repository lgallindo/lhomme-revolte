/**
  @file game.h
 
  Main source file of L'Homme Révolté the game that puts together all the pieces. main
  game logic is implemented here.

  physics notes (you can break this when messing around with game constants):

  - Lowest ceiling under which player can fit is 4 height steps.
  - Widest hole over which player can run without jumping is 1 square.
  - Widest hole over which the player can jump is 3 squares.
  - Highest step a player can walk onto without jumping is 2 height steps.
  - Highest step a player can jump onto is 3 height steps.


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_GAME_H
#define _LHR_GAME_H

#include <stdint.h> // Needed for fixed width types, can easily be replaced.

/*
  The following keys are mandatory to be implemented on any platform in order
  for the game to be playable. Enums are bloat.
*/
#define LHR_KEY_UP 0
#define LHR_KEY_RIGHT 1
#define LHR_KEY_DOWN 2
#define LHR_KEY_LEFT 3
#define LHR_KEY_A 4     ///< fire, confirm
#define LHR_KEY_B 5     ///< cancel, strafe, look up/down
#define LHR_KEY_C 6     ///< menu, jump, switch weapons

/*
  The following keys are optional for a platform to implement. They just make
  the controls more comfortable.
*/
#define LHR_KEY_JUMP 7
#define LHR_KEY_STRAFE_LEFT 8
#define LHR_KEY_STRAFE_RIGHT 9
#define LHR_KEY_MAP 10
#define LHR_KEY_TOGGLE_FREELOOK 11
#define LHR_KEY_NEXT_WEAPON 12
#define LHR_KEY_PREVIOUS_WEAPON 13
#define LHR_KEY_MENU 14
#define LHR_KEY_CYCLE_WEAPON 15

#define LHR_KEY_COUNT 16 ///< Number of keys.

/* ============================= PORTING =================================== */

/* When porting, do the following:
   - Include this file (and possibly other optional files, like sounds.h) in
     your main_*.c frontend source.
   - Implement the following functions in your frontend source.
   - Call LHR_init() from your frontend initialization code.
   - Call LHR_mainLoopBody() from within your frontend main loop.

   If your platform is an AVR CPU (e.g. some Arduinos) and so has Harvard
   architecture, define #LHR_AVR 1 before including this file in your frontend
   source. */

#ifndef LHR_LOG
  #define LHR_LOG(str) {} ///< Can be redefined to log game messages.
#endif

#ifndef LHR_CPU_LOAD
  #define LHR_CPU_LOAD(percent) {} ///< Can be redefined to check CPU load in %.
#endif

#ifndef LHR_GAME_STEP_COMMAND
  #define LHR_GAME_STEP_COMMAND {} /**< Will be called each simulation step
                                   (good for creating deterministic behavior
                                   such as demos (LHR_mainLoopBody() calls
                                   potentially multiple simulation steps). */
#endif

/** 
  Returns 1 (0) if given key is pressed (not pressed). At least the mandatory
  keys have to be implemented, the optional keys don't have to ever return 1.
  See the key constant definitions to see which ones are mandatory.
*/
int8_t LHR_keyPressed(uint8_t key);

/**
  Optional function for mouse/joystick/analog controls, gets mouse x and y
  offset in pixels from the game screen center (to achieve classic FPS mouse
  controls the platform should center the mouse after this call). If the
  platform isn't using a mouse, this function can simply return [0,0] offset at
  each call, or even do nothing at all (leave the variables as are).
*/
void LHR_getMouseOffset(int16_t *x, int16_t *y);

/**
  Returns time in milliseconds sice program start.
*/
uint32_t LHR_getTimeMs(void);

/** 
  Sleep (yield CPU) for specified amount of ms. This is used to relieve CPU
  usage. If your platform doesn't need this or handles it in other way, this
  function can do nothing.
*/
void LHR_sleepMs(uint16_t timeMs);

/**
  Set specified screen pixel. ColorIndex is the index of the game's palette.
  The function doesn't have to (and shouldn't, for the sake of performance)
  check whether the coordinates are within screen bounds.
*/
static inline void LHR_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex);

/**
  Play given sound effect (SFX). This function may or may not use the sound
  samples provided in sounds.h, and it may or may not ignore the (logarithmic)
  volume parameter (0 to 255). Depending on the platform, the function can play
  completely different samples or even e.g. just beeps. If the platform can't
  play sounds, this function implementation can simply be left empty. This
  function doesn't have to implement safety measures, the back end takes cares
  of them.
*/
void LHR_playSound(uint8_t soundIndex, uint8_t volume);

#define LHR_MUSIC_TURN_OFF 0
#define LHR_MUSIC_TURN_ON 1
#define LHR_MUSIC_NEXT 2

/**
  Informs the frontend how music should play, e.g. turn on/off, change track,
  ... See LHR_MUSIC_* constants. Playing music is optional and the frontend may
  ignore this. If a frontend wants to implement music, it can use the bytebeat
  provided in sounds.h or use its own.
*/
void LHR_setMusic(uint8_t value);

#define LHR_EVENT_VIBRATE 0 ///< the controller should vibrate (or blink etc.)
#define LHR_EVENT_PLAYER_HURT 1 
#define LHR_EVENT_PLAYER_DIES 2
#define LHR_EVENT_LEVEL_STARTS 3
#define LHR_EVENT_LEVEL_WON 4
#define LHR_EVENT_MONSTER_DIES 5
#define LHR_EVENT_PLAYER_TAKES_ITEM 6
#define LHR_EVENT_EXPLOSION 7
#define LHR_EVENT_PLAYER_TELEPORTS 8
#define LHR_EVENT_PLAYER_CHANGES_WEAPON 9

/**
  This is an optional function that informs the frontend about special events
  which may trigger something special on the platform, such as a controller
  vibration, logging etc. The implementation of this function may be left empty.
*/
void LHR_processEvent(uint8_t event, uint8_t data);

#define LHR_SAVE_SIZE 12 ///< size of the save in bytes

/**
  Optional function for permanently saving the game state. Platforms that don't
  have permanent storage (HDD, EEPROM etc.)  may let this function simply do
  nothing. If implemented, the function should save the passed data into its
  permanent storage, e.g. a file, a cookie etc.
*/
void LHR_save(uint8_t data[LHR_SAVE_SIZE]);

/**
  Optional function for retrieving game data that were saved to permanent
  storage. Platforms without permanent storage may let this function do nothing.
  If implemented, the function should fill the passed array with data from
  permanent storage, e.g. a file, a cookie etc.

  If this function is called before LHR_save was ever called and no data is
  present in permanent memory, this function should do nothing (leave the data
  array as is).

  This function should return 1 if saving/loading is possible or 0 if not (this
  will be used by the game to detect saving/loading capability).
*/
uint8_t LHR_load(uint8_t data[LHR_SAVE_SIZE]);

/* ========================================================================= */

/**
  Main game loop body, call this inside your platform's specific main loop.
  Returns 1 if the game continues or 0 if the game was exited and program should
  halt. This functions handles reaching the target FPS and sleeping for
  relieving CPU, so don't do this.
*/
uint8_t LHR_mainLoopBody(void);

/**
  Initializes the game, call this in the platform's initialization code.
*/
void LHR_init(void);

#include "settings.h"

#if LHR_AVR
  #include <avr/pgmspace.h>

  #define LHR_PROGRAM_MEMORY const PROGMEM
  #define LHR_PROGRAM_MEMORY_U8(addr) pgm_read_byte(addr)
#else
  #define LHR_PROGRAM_MEMORY static const
  #define LHR_PROGRAM_MEMORY_U8(addr) ((uint8_t) (*(addr)))
#endif

#include "images.h" // don't change the order of these includes
#include "levels.h"
#include "texts.h"
#include "palette.h"

#if LHR_TEXTURE_DISTANCE == 0
  #define RCL_COMPUTE_WALL_TEXCOORDS 0
#endif

#define RCL_PIXEL_FUNCTION LHR_pixelFunc
#define RCL_TEXTURE_VERTICAL_STRETCH 0

#define RCL_CAMERA_COLL_HEIGHT_BELOW 800
#define RCL_CAMERA_COLL_HEIGHT_ABOVE 200

#define RCL_HORIZONTAL_FOV LHR_FOV_HORIZONTAL
#define RCL_VERTICAL_FOV LHR_FOV_VERTICAL

#include "raycastlib.h" 

#include "constants.h"

typedef struct
{
  uint8_t coords[2];
  uint8_t state;    /**< door state in format:
                          
                         MSB  ccbaaaaa  LSB

                         aaaaa: current door height (how much they're open)
                         b:     whether currently going up (0) or down (1)
                         cc:    by which card (key) the door is unlocked, 00
                                means no card (unlocked), 1 means card 0 etc. */
} LHR_DoorRecord;

#define LHR_SPRITE_SIZE(size0to3) \
  (((size0to3 + 3) * LHR_BASE_SPRITE_SIZE) / 4)

#define LHR_SPRITE_SIZE_TO_HEIGHT_ABOVE_GROUND(size0to3) \
  (LHR_SPRITE_SIZE(size0to3) / 2)

#define LHR_SPRITE_SIZE_PIXELS(size0to3) \
  ((LHR_SPRITE_SIZE(size0to3) * LHR_SPRITE_MAX_SIZE) / RCL_UNITS_PER_SQUARE)

/**
  Holds information about one instance of a level item (a type of level element,
  e.g. pickable items, decorations etc.). The format is following:

  MSB  abbbbbbb  LSB

  a:        active flag, 1 means the item is nearby to player and is active
  bbbbbbb:  index to elements array of the current level, pointing to element
            representing this item 
*/
typedef uint8_t LHR_ItemRecord;

#define LHR_ITEM_RECORD_ACTIVE_MASK 0x80

#define LHR_ITEM_RECORD_LEVEL_ELEMENT(itemRecord) \
  (LHR_currentLevel.levelPointer->elements[itemRecord & \
  ~LHR_ITEM_RECORD_ACTIVE_MASK])

typedef struct 
{
  uint8_t stateType;  /**< Holds state (lower 4 bits) and type of monster (upper
                           4 bits). */
  uint8_t coords[2];  /**< monster position, in 1/4s of a square */
  uint8_t health;
} LHR_MonsterRecord;

#define LHR_MR_STATE(mr) ((mr).stateType & LHR_MONSTER_MASK_STATE)
#define LHR_MR_TYPE(mr) \
  (LHR_MONSTER_INDEX_TO_TYPE(((mr).stateType & LHR_MONSTER_MASK_TYPE) >> 4))

#define LHR_MONSTER_COORD_TO_RCL_UNITS(c) ((RCL_UNITS_PER_SQUARE / 8) + c * 256)
#define LHR_MONSTER_COORD_TO_SQUARES(c) (c / 4)

#define LHR_ELEMENT_COORD_TO_RCL_UNITS(c) \
  (c * RCL_UNITS_PER_SQUARE + RCL_UNITS_PER_SQUARE / 2)

#define LHR_MONSTER_MASK_STATE 0x0f
#define LHR_MONSTER_MASK_TYPE  0xf0

#define LHR_MONSTER_STATE_INACTIVE  0 ///< Not nearby, not actively updated.
#define LHR_MONSTER_STATE_IDLE      1
#define LHR_MONSTER_STATE_ATTACKING 2
#define LHR_MONSTER_STATE_HURTING   3
#define LHR_MONSTER_STATE_DYING     4
#define LHR_MONSTER_STATE_GOING_N   5
#define LHR_MONSTER_STATE_GOING_NE  6
#define LHR_MONSTER_STATE_GOING_E   7
#define LHR_MONSTER_STATE_GOING_SE  8
#define LHR_MONSTER_STATE_GOING_S   9
#define LHR_MONSTER_STATE_GOING_SW  10
#define LHR_MONSTER_STATE_GOING_W   11
#define LHR_MONSTER_STATE_GOING_NW  12
#define LHR_MONSTER_STATE_DEAD      13

typedef struct
{
  uint8_t  type;
  uint8_t  doubleFramesToLive; /**< This number times two (because 255 could be
                                    too little at high FPS) says after how many
                                    frames the projectile is destroyed. */
  uint16_t position[3]; /**< Current position, stored as u16 to save space, as
                             that is exactly enough to store position on 64x64
                             map. */
  int16_t direction[3]; /**< Added to position each game step. */
} LHR_ProjectileRecord;

#define LHR_GAME_STATE_INIT 0 ///< first state, waiting for key releases
#define LHR_GAME_STATE_PLAYING 1
#define LHR_GAME_STATE_WIN 2
#define LHR_GAME_STATE_LOSE 3
#define LHR_GAME_STATE_INTRO 4
#define LHR_GAME_STATE_OUTRO 5
#define LHR_GAME_STATE_MAP 6
#define LHR_GAME_STATE_LEVEL_START 7
#define LHR_GAME_STATE_MENU 8  

#ifndef LHR_DEMO_MODE
  #define LHR_DEMO_MODE 0
#endif

#define LHR_STORY_GLOBAL_INTRO 0
#define LHR_STORY_GLOBAL_OUTRO 1
#define LHR_STORY_MAP_INTRO 2
#define LHR_STORY_MAP_OUTRO 3

#define LHR_MENU_ITEM_CONTINUE 0
#define LHR_MENU_ITEM_MAP 1
#define LHR_MENU_ITEM_PLAY 2
#define LHR_MENU_ITEM_LOAD 3
#define LHR_MENU_ITEM_SFX 4
#define LHR_MENU_ITEM_MUSIC 5
#define LHR_MENU_ITEM_SHEAR 6
#define LHR_MENU_ITEM_LANGUAGE 7
#define LHR_MENU_ITEM_EXIT 8

#define LHR_MENU_ITEM_NONE 255

/*
  GLOBAL VARIABLES
===============================================================================
*/

/**
  Groups global variables related to the game as such in a single struct. There
  are still other global structs for player, level etc.
*/
struct
{
  uint8_t state;                 ///< Current game state.
  uint32_t stateTime;            ///< Time in ms from last state change.
  uint8_t currentRandom;         ///< for RNG
  uint8_t spriteAnimationFrame;
  uint8_t soundsPlayedThisFrame; /**< Each bit says whether given sound was
                                    played this frame, prevents playing too many
                                    sounds at once. */
  RCL_RayConstraints rayConstraints; ///< Ray constraints for rendering.
  RCL_RayConstraints visibilityRayConstraints; ///< Constraints for visibility.
  uint8_t keyStates[LHR_KEY_COUNT]; /**< Pressed states of keys, each value
                                    stores the number of frames for which the
                                    key has been held. */
  uint8_t zBuffer[LHR_Z_BUFFER_SIZE];
  uint8_t textureAverageColors[LHR_WALL_TEXTURE_COUNT]; /**< Contains average
                                    color for each wall texture. */
  int8_t backgroundScaleMap[LHR_GAME_RESOLUTION_Y];
  uint16_t backgroundScroll;
  uint8_t spriteSamplingPoints[LHR_MAX_SPRITE_SIZE]; /**< Helper for
                                                     precomputing sprite
                                                     sampling positions for
                                                     drawing. */
  uint32_t frameTime;      ///< time (in ms) of the current frame start
  uint32_t frame;          ///< frame number
  uint8_t selectedMenuItem;
  uint8_t selectedLevel;   ///< level to play selected in the main menu
  uint8_t storyType;       ///< LHR_STORY_* selector for text/music source
  uint8_t introTargetLevel;
  uint8_t outroTargetLevel;
  uint8_t antiSpam;        ///< Prevents log message spamming.
  uint8_t settings;   /**< dynamic game settings (can be changed at runtime),
                           bit meaning:

                           MSB -------- LSB
                                   ||||
                                   |||\_ sound (SFX)
                                   ||\__ music
                                   |\___ shearing
                                   \____ freelook (shearing not sliding back) */
  uint8_t blink;      ///< Says whether blinkg is currently on or off.
  uint8_t saved;      /**< Helper variable to know if game was saved. Can be
                           0 (not saved), 1 (just saved) or 255 (can't save).*/
  uint8_t cheatState; /**< Highest bit say whether cheat is enabled, other bits
                           represent the state of typing the cheat code. */
  uint8_t save[LHR_SAVE_SIZE];  /**< Stores the game save state that's kept in
                           the persistent memory.

                           The save format is binary and platform independent.
                           The save contains game settings, game progress and a
                           saved position. The format is as follows:

         0  4b  (less signif.) highest level that has been reached
         0  4b  (more signif.) level number of the saved position (0: no save)
         1  8b  game settings (LHR_game.settings)
         2  8b  health at saved position
         3  8b  bullet ammo at saved position
         4  8b  rocket ammo at saved position
         5  8b  plasma ammo at saved position
         6  32b little endian total play time, in 10ths of sec
         10 16b little endian total enemies killed from start */
  uint8_t continues;  ///< Whether the game continues or was exited.
} LHR_game;

#define LHR_SAVE_TOTAL_TIME (LHR_game.save[6] + LHR_game.save[7] * 256 + \
  LHR_game.save[8] * 65536 + LHR_game.save[9] * 4294967296)

/**
  Stores player state.
*/
struct
{
  RCL_Camera camera;
  int8_t squarePosition[2];
  RCL_Vector2D direction;
  RCL_Unit verticalSpeed;
  RCL_Unit previousVerticalSpeed;  /**< Vertical speed in previous frame, needed
                                   for determining whether player is in the
                                   air. */
  uint16_t headBobFrame;
  uint8_t  weapon;                 ///< currently selected weapon
  uint8_t  health;
  uint32_t weaponCooldownFrames;   ///< frames left for weapon cooldown
  uint32_t lastHurtFrame;
  uint32_t lastItemTakenFrame;
  uint8_t  ammo[LHR_AMMO_TOTAL];
  uint8_t  cards;                  /**< Lowest 3 bits say which access cards
                                   have been taken, the next 3 bits say
                                   which cards should be blinking in the HUD,
                                   the last 2 bits are a blink reset counter. */
  uint8_t  justTeleported;
  int8_t   previousWeaponDirection; ///< Direction (+/0/-) of previous weapon.
} LHR_player;

/**
  Stores the current level and helper precomputed values for better performance.
*/
struct
{
  const LHR_Level *levelPointer;
  uint8_t levelNumber;
  const uint8_t* textures[7];    ///< textures the level is using
  uint32_t timeStart;
  uint32_t frameStart;
  uint32_t completionTime10sOfS; ///< completion time in 10ths of second
  uint8_t floorColor;
  uint8_t ceilingColor;

  LHR_DoorRecord doorRecords[LHR_MAX_DOORS];
  uint8_t doorRecordCount;
  uint8_t checkedDoorIndex; ///< Says which door are currently being checked.

  LHR_ItemRecord itemRecords[LHR_MAX_ITEMS]; ///< Holds level items.
  uint8_t itemRecordCount;
  uint8_t checkedItemIndex; ///< Same as checkedDoorIndex, but for items.

  LHR_MonsterRecord monsterRecords[LHR_MAX_MONSTERS];
  uint8_t monsterRecordCount;
  uint8_t checkedMonsterIndex; 

  LHR_ProjectileRecord projectileRecords[LHR_MAX_PROJECTILES];
  uint8_t projectileRecordCount;
  uint8_t bossCount;
  uint8_t monstersDead;
  uint8_t backgroundImage;
  uint8_t teleporterCount;
  uint16_t mapRevealMask; /**< Bits say which parts of the map have been
                               revealed. */
  uint8_t itemCollisionMap[(LHR_MAP_SIZE * LHR_MAP_SIZE) / 8];
                          /**< Bit array, for each map square says whether there
                               is a colliding item or not. */
} LHR_currentLevel;

#if LHR_AVR
/**
  Copy of the current level that is stored in RAM. This is only done on Arduino
  because accessing it in program memory (PROGMEM) directly would be a pain.
  Because of this Arduino needs more RAM.
*/
LHR_Level LHR_ramLevel;
#endif

/**
  Helper function for accessing the itemCollisionMap bits.
*/
void LHR_getItemCollisionMapIndex(
  uint8_t x, uint8_t y, uint16_t *byte, uint8_t *bit)
{
  uint16_t index = y * LHR_MAP_SIZE + x;

  *byte = index / 8;
  *bit = index % 8;
}

void LHR_setItemCollisionMapBit(uint8_t x, uint8_t y, uint8_t value)
{
  uint16_t byte;
  uint8_t bit;

  LHR_getItemCollisionMapIndex(x,y,&byte,&bit);

  LHR_currentLevel.itemCollisionMap[byte] &= ~(0x01 << bit);
  LHR_currentLevel.itemCollisionMap[byte] |= (value & 0x01) << bit;
}

uint8_t LHR_getItemCollisionMapBit(uint8_t x, uint8_t y)
{
  uint16_t byte;
  uint8_t bit;

  LHR_getItemCollisionMapIndex(x,y,&byte,&bit);
  return (LHR_currentLevel.itemCollisionMap[byte] >> bit) & 0x01;
}

#if LHR_DITHERED_SHADOW
static const uint8_t LHR_ditheringPatterns[] =
{
  0,0,0,0,
  0,0,0,0,

  0,0,0,0,
  0,1,0,0,

  0,0,0,0,
  0,1,0,1,

  1,0,1,0,
  0,1,0,0,

  1,0,1,0,
  0,1,0,1,

  1,0,1,0,
  0,1,1,1,

  1,1,1,1,
  0,1,0,1,

  1,1,1,1,
  0,1,1,1,
 
  1,1,1,1,
  1,1,1,1
};
#endif

/*
  FUNCTIONS
===============================================================================
*/

/**
  Returns a pseudorandom byte. This is a very simple congruent generator, its
  parameters have been chosen so that each number (0-255) is included in the
  output exactly once!
*/
uint8_t LHR_random(void)
{
  LHR_game.currentRandom *= 13;
  LHR_game.currentRandom += 7;
  
  return LHR_game.currentRandom;
}

void LHR_playGameSound(uint8_t soundIndex, uint8_t volume)
{
  if (!(LHR_game.settings & 0x01))
    return;

  uint8_t mask = 0x01 << soundIndex;

  if (!(LHR_game.soundsPlayedThisFrame & mask))
  {
    LHR_playSound(soundIndex,volume);
    LHR_game.soundsPlayedThisFrame |= mask;
  }
}

/**
  Returns a damage value for specific attack type (LHR_WEAPON_FIRE_TYPE_...),
  with added randomness (so the values will differ). For explosion pass
  LHR_WEAPON_FIRE_TYPE_FIREBALL.
*/
uint8_t LHR_getDamageValue(uint8_t attackType)
{
  if (attackType >= LHR_WEAPON_FIRE_TYPES_TOTAL)
    return 0;

  int32_t value = LHR_attackDamageTable[attackType]; // has to be signed
  int32_t maxAdd = (value * LHR_DAMAGE_RANDOMNESS) / 256;

  value = value + (maxAdd / 2) - (LHR_random() * maxAdd / 256);

  if (value < 0)
    value = 0;

  return value;
}

/**
  Saves game data to persistent storage.
*/
void LHR_gameSave(void)
{
  if (LHR_game.saved == LHR_CANT_SAVE)
    return;

  LHR_LOG("saving game data");

  LHR_save(LHR_game.save);
}

/**
  Loads game data from persistent storage.
*/
void LHR_gameLoad(void)
{
  if (LHR_game.saved == LHR_CANT_SAVE)
    return;

  LHR_LOG("loading game data");

  uint8_t result = LHR_load(LHR_game.save);

  if (result == 0)
    LHR_game.saved = LHR_CANT_SAVE;
}

/**
  Returns ammo type for given weapon.
*/
uint8_t LHR_weaponAmmo(uint8_t weapon)
{
  if (weapon == LHR_WEAPON_KNIFE)
    return LHR_AMMO_NONE;
  if (weapon == LHR_WEAPON_MACHINE_GUN ||
      weapon == LHR_WEAPON_SHOTGUN)
    return LHR_AMMO_BULLETS;
  else if (weapon == LHR_WEAPON_ROCKET_LAUNCHER)
    return LHR_AMMO_ROCKETS;
  else
    return LHR_AMMO_PLASMA;
}

RCL_Unit LHR_taxicabDistance(
  RCL_Unit x0, RCL_Unit y0, RCL_Unit z0, RCL_Unit x1, RCL_Unit y1, RCL_Unit z1)
{
  return (RCL_abs(x0 - x1) + RCL_abs(y0 - y1) + RCL_abs(z0 - z1));
}

uint8_t LHR_isInActiveDistanceFromPlayer(RCL_Unit x, RCL_Unit y, RCL_Unit z)
{
  return LHR_taxicabDistance(
    x,y,z,LHR_player.camera.position.x,LHR_player.camera.position.y,
    LHR_player.camera.height) <= LHR_LEVEL_ELEMENT_ACTIVE_DISTANCE;
}

/**
  Function called when a level end to compute the stats etc.
*/
void LHR_levelEnds(void)
{
  LHR_currentLevel.completionTime10sOfS = (LHR_MS_PER_FRAME *
    (LHR_game.frame - LHR_currentLevel.frameStart)) / 100; 

  if (
   (LHR_player.health != 0) &&
   (LHR_currentLevel.levelNumber >= (LHR_game.save[0] & 0x0f)) &&
   ((LHR_currentLevel.levelNumber + 1) < LHR_NUMBER_OF_LEVELS))
  {
    LHR_game.save[0] = // save progress
      (LHR_game.save[0] & 0xf0) | (LHR_currentLevel.levelNumber + 1);

    LHR_gameSave();
  }

  LHR_currentLevel.monstersDead = 0;
       
  for (uint16_t i = 0; i < LHR_currentLevel.monsterRecordCount; ++i)
    if (LHR_currentLevel.monsterRecords[i].health == 0)
      LHR_currentLevel.monstersDead++;

  uint32_t totalTime = LHR_SAVE_TOTAL_TIME;

  if ((LHR_currentLevel.levelNumber == 0) || (totalTime != 0))
  {
    LHR_LOG("Updating save totals.");
  
    totalTime += LHR_currentLevel.completionTime10sOfS;

    for (uint8_t i = 0; i < 4; ++i)
    {
      LHR_game.save[6 + i] = totalTime % 256;
      totalTime /= 256;
    }

    LHR_game.save[10] += LHR_currentLevel.monstersDead % 256;
    LHR_game.save[11] += LHR_currentLevel.monstersDead / 256;
  }

  LHR_game.save[2] = LHR_player.health;
  LHR_game.save[3] = LHR_player.ammo[0];
  LHR_game.save[4] = LHR_player.ammo[1];
  LHR_game.save[5] = LHR_player.ammo[2];
}

static inline uint8_t LHR_RCLUnitToZBuffer(RCL_Unit x)
{
  x /= (RCL_UNITS_PER_SQUARE / 8);

  uint8_t okay = x < 256;

  return okay * (x + 1) - 1;
}

const uint8_t *LHR_getMonsterSprite(
  uint8_t monsterType, uint8_t state, uint8_t frame)
{
  uint8_t index = 
    state == LHR_MONSTER_STATE_DEAD ? 18 : 17;
  // ^ makes the compiled binary smaller compared to returning pointers directly

  if ((state != LHR_MONSTER_STATE_DYING) && (state != LHR_MONSTER_STATE_DEAD))
    switch (monsterType)
    {
      case LHR_LEVEL_ELEMENT_MONSTER_SPIDER:
        switch (state)
        {
          case LHR_MONSTER_STATE_ATTACKING: index = 1; break;
          case LHR_MONSTER_STATE_IDLE: index = 0; break;
          default: index = frame ? 0 : 2; break;
        }
        break;

      case LHR_LEVEL_ELEMENT_MONSTER_WARRIOR:
        index = state != LHR_MONSTER_STATE_ATTACKING ? 6 : 7;
        break;

      case LHR_LEVEL_ELEMENT_MONSTER_DESTROYER:
        switch (state)
        {
          case LHR_MONSTER_STATE_ATTACKING: index = 4; break;
          case LHR_MONSTER_STATE_IDLE: index = 3; break;
          default: index = frame ? 3 : 5; break;
        }
        break;

      case LHR_LEVEL_ELEMENT_MONSTER_PLASMABOT:
        index = state != LHR_MONSTER_STATE_ATTACKING ? 8 : 9;
        break;

      case LHR_LEVEL_ELEMENT_MONSTER_ENDER:
        switch (state)
        {
          case LHR_MONSTER_STATE_ATTACKING: index = 12; break;
          case LHR_MONSTER_STATE_IDLE: index = 10; break;
          default: index = frame ? 10 : 11; break;
        }
        break;

      case LHR_LEVEL_ELEMENT_MONSTER_TURRET:
        switch (state)
        {
          case LHR_MONSTER_STATE_ATTACKING: index = 15; break;
          case LHR_MONSTER_STATE_IDLE: index = 13; break;
          default: index = frame ? 13 : 14; break;
        }
        break;

      case LHR_LEVEL_ELEMENT_MONSTER_EXPLODER:
      default:
        index = 16; 
        break;
    }
  
  return LHR_monsterSprites + index * LHR_TEXTURE_STORE_SIZE;
}

/**
  Says whether given key is currently pressed (down). This should be preferred
  to LHR_keyPressed().
*/
uint8_t LHR_keyIsDown(uint8_t key)
{
  return LHR_game.keyStates[key] != 0;
}

/**
  Says whether given key has been pressed in the current frame.
*/
uint8_t LHR_keyJustPressed(uint8_t key)
{
  return (LHR_game.keyStates[key]) == 1;
}

/**
  Says whether a key is being repeated after being held for certain time.
*/
uint8_t LHR_keyRepeated(uint8_t key)
{
  return
    ((LHR_game.keyStates[key] >= LHR_KEY_REPEAT_DELAY_FRAMES) ||
    (LHR_game.keyStates[key] == 255)) &&
    (LHR_game.frame % LHR_KEY_REPEAT_PERIOD_FRAMES == 0);
}

uint16_t LHR_keyRegisters(uint8_t key)
{
  return LHR_keyJustPressed(key) || LHR_keyRepeated(key);
}

#if LHR_RESOLUTION_SCALEDOWN == 1
  #define LHR_setGamePixel LHR_setPixel
#else

/**
  Sets the game pixel (a pixel that can potentially be bigger than the screen
  pixel).
*/
static inline void LHR_setGamePixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  uint16_t screenY = y * LHR_RESOLUTION_SCALEDOWN;
  uint16_t screenX = x * LHR_RESOLUTION_SCALEDOWN;

  for (uint16_t j = screenY; j < screenY + LHR_RESOLUTION_SCALEDOWN; ++j)
    for (uint16_t i = screenX; i < screenX + LHR_RESOLUTION_SCALEDOWN; ++i)
      LHR_setPixel(i,j,colorIndex);
}
#endif

void LHR_recomputePLayerDirection(void)
{
  LHR_player.camera.direction =
    RCL_wrap(LHR_player.camera.direction,RCL_UNITS_PER_SQUARE);

  LHR_player.direction = RCL_angleToDirection(LHR_player.camera.direction);

  LHR_player.direction.x =
    (LHR_player.direction.x * LHR_PLAYER_MOVE_UNITS_PER_FRAME)
    / RCL_UNITS_PER_SQUARE;

  LHR_player.direction.y =
    (LHR_player.direction.y * LHR_PLAYER_MOVE_UNITS_PER_FRAME)
    / RCL_UNITS_PER_SQUARE;

  LHR_game.backgroundScroll =
    ((LHR_player.camera.direction * 8) * LHR_GAME_RESOLUTION_Y)
    / RCL_UNITS_PER_SQUARE; 
}

#if LHR_BACKGROUND_BLUR != 0
uint8_t LHR_backgroundBlurIndex = 0;

static const int8_t LHR_backgroundBlurOffsets[8] =
  {
    0  * LHR_BACKGROUND_BLUR,
    16 * LHR_BACKGROUND_BLUR,
    7  * LHR_BACKGROUND_BLUR,
    17 * LHR_BACKGROUND_BLUR,
    1  * LHR_BACKGROUND_BLUR,
    4  * LHR_BACKGROUND_BLUR,
    15 * LHR_BACKGROUND_BLUR,
    9  * LHR_BACKGROUND_BLUR,
  };
#endif

static inline uint8_t LHR_fogValueDiminish(RCL_Unit depth)
{
  return depth / LHR_FOG_DIMINISH_STEP;
}

static inline uint8_t
  LHR_getTexelFull(uint8_t textureIndex,RCL_Unit u, RCL_Unit v)
{
  return
    LHR_getTexel(
      textureIndex != 255 ?
        LHR_currentLevel.textures[textureIndex] :
          (LHR_wallTextures + LHR_currentLevel.levelPointer->doorTextureIndex
          * LHR_TEXTURE_STORE_SIZE), 
          u / (RCL_UNITS_PER_SQUARE / LHR_TEXTURE_SIZE), 
          v / (RCL_UNITS_PER_SQUARE / LHR_TEXTURE_SIZE));
}

static inline uint8_t LHR_getTexelAverage(uint8_t textureIndex)
{
  return
    textureIndex != 255 ?
      LHR_game.textureAverageColors[
        LHR_currentLevel.levelPointer->textureIndices[textureIndex]]
      :
      (
        LHR_game.textureAverageColors[
          LHR_currentLevel.levelPointer->doorTextureIndex]
        + 1 // to distinguish from normal walls
      );
}

void LHR_pixelFunc(RCL_PixelInfo *pixel)
{ 
  uint8_t color;
  uint8_t shadow = 0;

  if (pixel->isHorizon && pixel->depth > RCL_UNITS_PER_SQUARE * 16)
  {
    color = LHR_TRANSPARENT_COLOR;
  }
  else if (pixel->isWall)
  {
    uint8_t textureIndex =
      pixel->isFloor ?
      (
        ((pixel->hit.type & LHR_TILE_PROPERTY_MASK) != LHR_TILE_PROPERTY_DOOR) ?
        (pixel->hit.type & 0x7)
        :
        (
          (pixel->texCoords.y > RCL_UNITS_PER_SQUARE) ?
          (pixel->hit.type & 0x7) : 255
        )
      ):
      ((pixel->hit.type & 0x38) >> 3); 

#if LHR_TEXTURE_DISTANCE != 0
    RCL_Unit textureV = pixel->texCoords.y;

    if ((pixel->hit.type & LHR_TILE_PROPERTY_MASK) ==
      LHR_TILE_PROPERTY_SQUEEZER)
      textureV += pixel->wallHeight;
#endif

    color =
      textureIndex != LHR_TILE_TEXTURE_TRANSPARENT ?
      (
#if LHR_TEXTURE_DISTANCE >= 65535
      LHR_getTexelFull(textureIndex,pixel->texCoords.x,textureV)
#elif LHR_TEXTURE_DISTANCE == 0 
      LHR_getTexelAverage(textureIndex)
#else
      pixel->depth <= LHR_TEXTURE_DISTANCE ?
        LHR_getTexelFull(textureIndex,pixel->texCoords.x,textureV) :
        LHR_getTexelAverage(textureIndex)
#endif
      )
      :
      LHR_TRANSPARENT_COLOR;

    shadow = pixel->hit.direction >> 1;
  }
  else // floor/ceiling
  {
    color = pixel->isFloor ?
      (
#if LHR_DIFFERENT_FLOOR_CEILING_COLORS
        2 + (pixel->height / LHR_WALL_HEIGHT_STEP) % 4
#else
        LHR_currentLevel.floorColor
#endif
      ) : 
      (pixel->height < LHR_CEILING_MAX_HEIGHT ?
        (
#if LHR_DIFFERENT_FLOOR_CEILING_COLORS
          18 + (pixel->height / LHR_WALL_HEIGHT_STEP) % 4
#else
          LHR_currentLevel.ceilingColor 
#endif
        )
        : LHR_TRANSPARENT_COLOR);
  }

  if (color != LHR_TRANSPARENT_COLOR)
  {
#if LHR_DITHERED_SHADOW
    uint8_t fogShadow = (pixel->depth * 8) / LHR_FOG_DIMINISH_STEP;

    uint8_t fogShadowPart = fogShadow & 0x07;

    fogShadow /= 8;

    uint8_t xMod4 = pixel->position.x & 0x03;
    uint8_t yMod2 = pixel->position.y & 0x01;

    shadow +=
      fogShadow + LHR_ditheringPatterns[fogShadowPart * 8 + yMod2 * 4 + xMod4];
#else
    shadow += LHR_fogValueDiminish(pixel->depth);
#endif

#if LHR_ENABLE_FOG
    color = palette_minusValue(color,shadow);
#endif
  }
  else
  {
#if LHR_DRAW_LEVEL_BACKGROUND
    color = LHR_getTexel(LHR_backgroundImages + 
        LHR_currentLevel.backgroundImage * LHR_TEXTURE_STORE_SIZE,
      LHR_game.backgroundScaleMap[((pixel->position.x 
  #if LHR_BACKGROUND_BLUR != 0
        + LHR_backgroundBlurOffsets[LHR_backgroundBlurIndex]
  #endif
        ) * LHR_RAYCASTING_SUBSAMPLE + LHR_game.backgroundScroll) % LHR_GAME_RESOLUTION_Y], 
      (LHR_game.backgroundScaleMap[(pixel->position.y          // ^ TODO: get rid of mod?
  #if LHR_BACKGROUND_BLUR != 0
        + LHR_backgroundBlurOffsets[(LHR_backgroundBlurIndex + 1) % 8]
  #endif
        ) % LHR_GAME_RESOLUTION_Y])                                               
      );

  #if LHR_BACKGROUND_BLUR != 0
      LHR_backgroundBlurIndex = (LHR_backgroundBlurIndex + 1) % 8;
  #endif
#else
    color = 1;
#endif
  }

#if LHR_BRIGHTNESS > 0
  color = palette_plusValue(color,LHR_BRIGHTNESS);
#elif LHR_BRIGHTNESS < 0
  color = palette_minusValue(color,-1 * LHR_BRIGHTNESS);
#endif

#if LHR_RAYCASTING_SUBSAMPLE == 1
  // the other version will probably get optimized to this, but just in case
  LHR_setGamePixel(pixel->position.x,pixel->position.y,color);
#else
  RCL_Unit screenX = pixel->position.x * LHR_RAYCASTING_SUBSAMPLE;

  for (int_fast8_t i = 0; i < LHR_RAYCASTING_SUBSAMPLE; ++i)
  {
    LHR_setGamePixel(screenX,pixel->position.y,color);
    screenX++;
  }
#endif
}

/**
  Draws image on screen, with transparency. This is faster than sprite drawing.
  For performance sake drawing near screen edges is not pixel perfect.
*/
void LHR_blitImage(
  const uint8_t *image,
  int16_t posX,
  int16_t posY,
  uint8_t scale)
{
  if (scale == 0)
    return;
 
  uint16_t x0 = posX,
           x1,
           y0 = posY, 
           y1;
 
  uint8_t u0 = 0, v0 = 0;

  if (posX < 0)
  {
    x0 = 0;
    u0 = (-1 * posX) / scale;
  }

  posX += scale * LHR_TEXTURE_SIZE;

  uint16_t limitX = LHR_GAME_RESOLUTION_X - scale;
  uint16_t limitY = LHR_GAME_RESOLUTION_Y - scale;

  x1 = posX >= 0 ?
       (posX <= limitX ? posX : limitX)
       : 0;

  if (x1 >= LHR_GAME_RESOLUTION_X)
    x1 = LHR_GAME_RESOLUTION_X - 1;

  if (posY < 0)
  {
    y0 = 0;
    v0 = (-1 * posY) / scale;
  }

  posY += scale * LHR_TEXTURE_SIZE;

  y1 = posY >= 0 ? (posY <= limitY ? posY : limitY) : 0;

  if (y1 >= LHR_GAME_RESOLUTION_Y)
    y1 = LHR_GAME_RESOLUTION_Y - 1;

  uint8_t v = v0;

  for (uint16_t y = y0; y < y1; y += scale)
  {
    uint8_t u = u0;

    for (uint16_t x = x0; x < x1; x += scale)
    {
      uint8_t color = LHR_getTexel(image,u,v);

      if (color != LHR_TRANSPARENT_COLOR)
      {
        uint16_t sY = y;

        for (uint8_t j = 0; j < scale; ++j)
        {
          uint16_t sX = x;

          for (uint8_t i = 0; i < scale; ++i)
          {
            LHR_setGamePixel(sX,sY,color);
            sX++;
          }
          
          sY++;
        }
      }
      u++;
    }
    v++;
  }
}

void LHR_drawScaledSprite(
  const uint8_t *image,
  int16_t centerX,
  int16_t centerY,
  int16_t size,
  uint8_t minusValue,
  RCL_Unit distance)
{
  if (size == 0)
    return;

  if (size > LHR_MAX_SPRITE_SIZE)
    size = LHR_MAX_SPRITE_SIZE;

  uint16_t halfSize = size / 2;

  int16_t topLeftX = centerX - halfSize;
  int16_t topLeftY = centerY - halfSize; 

  int16_t x0, u0;

  if (topLeftX < 0)
  {
    u0 = -1 * topLeftX;
    x0 = 0;
  }
  else
  {
    u0 = 0;
    x0 = topLeftX;
  }

  int16_t x1 = topLeftX + size - 1;

  if (x1 >= LHR_GAME_RESOLUTION_X)
    x1 = LHR_GAME_RESOLUTION_X - 1;

  int16_t y0, v0;

  if (topLeftY < 0)
  {
    v0 = -1 * topLeftY;
    y0 = 0;
  }
  else
  {
    v0 = 0;
    y0 = topLeftY;
  }

  int16_t y1 = topLeftY + size - 1;

  if (y1 >= LHR_GAME_RESOLUTION_Y)
    y1 = LHR_GAME_RESOLUTION_Y - 1;

  if ((x0 > x1) || (y0 > y1) || (u0 >= size) || (v0 >= size)) // outside screen?
    return; 

  int16_t u1 = u0 + (x1 - x0);
  int16_t v1 = v0 + (y1 - y0);

  // precompute sampling positions:

  int16_t uMin = RCL_min(u0,u1);
  int16_t vMin = RCL_min(v0,v1);
  int16_t uMax = RCL_max(u0,u1);
  int16_t vMax = RCL_max(v0,v1);

  int16_t precompFrom = RCL_min(uMin,vMin);
  int16_t precompTo = RCL_max(uMax,vMax);

  precompFrom = RCL_max(0,precompFrom);
  precompTo = RCL_min(LHR_MAX_SPRITE_SIZE - 1,precompTo);

  #define PRECOMP_SCALE 512

  int16_t precompStepScaled = ((LHR_TEXTURE_SIZE) * PRECOMP_SCALE) / size;
  int16_t precompPosScaled = precompFrom * precompStepScaled;

  for (int16_t i = precompFrom; i <= precompTo; ++i)
  {
    LHR_game.spriteSamplingPoints[i] = precompPosScaled / PRECOMP_SCALE;
    precompPosScaled += precompStepScaled;
  }

  #undef PRECOMP_SCALE

  uint8_t zDistance = LHR_RCLUnitToZBuffer(distance);

  for (int16_t x = x0, u = u0; x <= x1; ++x, ++u)
  {
    if (LHR_game.zBuffer[x] >= zDistance)
    {
      int8_t columnTransparent = 1;

      for (int16_t y = y0, v = v0; y <= y1; ++y, ++v)
      {
        uint8_t color =
          LHR_getTexel(image,LHR_game.spriteSamplingPoints[u],
            LHR_game.spriteSamplingPoints[v]);

        if (color != LHR_TRANSPARENT_COLOR)
        {
#if LHR_DIMINISH_SPRITES
          color = palette_minusValue(color,minusValue);
#endif 
          columnTransparent = 0;

          LHR_setGamePixel(x,y,color);
        }
      }

      if (!columnTransparent)
        LHR_game.zBuffer[x] = zDistance;
    }
  }
}

RCL_Unit LHR_texturesAt(int16_t x, int16_t y)
{
  uint8_t p;

  LHR_TileDefinition tile =
    LHR_getMapTile(LHR_currentLevel.levelPointer,x,y,&p);

  return
    LHR_TILE_FLOOR_TEXTURE(tile) | (LHR_TILE_CEILING_TEXTURE(tile) << 3) | p;
    // ^ store both textures (floor and ceiling) and properties in one number
}

RCL_Unit LHR_movingWallHeight
(
  RCL_Unit low,
  RCL_Unit high,
  uint32_t time    
)
{
  RCL_Unit height = RCL_nonZero(high - low);
  RCL_Unit halfHeight = height / 2;

  RCL_Unit sinArg =
    (time * ((LHR_MOVING_WALL_SPEED * RCL_UNITS_PER_SQUARE) / 1000)) / height;

  return
    low + halfHeight + (RCL_sin(sinArg) * halfHeight) / RCL_UNITS_PER_SQUARE;
}

RCL_Unit LHR_floorHeightAt(int16_t x, int16_t y)
{
  uint8_t properties;

  LHR_TileDefinition tile =
    LHR_getMapTile(LHR_currentLevel.levelPointer,x,y,&properties);

  RCL_Unit doorHeight = 0;

  if (properties == LHR_TILE_PROPERTY_DOOR)
  {
    for (uint8_t i = 0; i < LHR_currentLevel.doorRecordCount; ++i)
    {
      LHR_DoorRecord *door = &(LHR_currentLevel.doorRecords[i]);

      if ((door->coords[0] == x) && (door->coords[1] == y))
      {
        doorHeight = door->state & LHR_DOOR_VERTICAL_POSITION_MASK;

        doorHeight = doorHeight != (0xff & LHR_DOOR_VERTICAL_POSITION_MASK)    ? 
          doorHeight * LHR_DOOR_HEIGHT_STEP : RCL_UNITS_PER_SQUARE;

        break;
      }
    }
  }
  else if (properties == LHR_TILE_PROPERTY_ELEVATOR)
  {
    RCL_Unit height =
      LHR_TILE_FLOOR_HEIGHT(tile) * LHR_WALL_HEIGHT_STEP;

    return LHR_movingWallHeight(
      height,
      height + LHR_TILE_CEILING_HEIGHT(tile) * LHR_WALL_HEIGHT_STEP,
      LHR_game.frameTime - LHR_currentLevel.timeStart);
  }
 
  return LHR_TILE_FLOOR_HEIGHT(tile) * LHR_WALL_HEIGHT_STEP - doorHeight;
}

/**
  Like LHR_floorCollisionHeightAt, but takes into account colliding items on
  the map, so the squares that have these items are higher. The former function
  is for rendering, this one is for collision checking.
*/
RCL_Unit LHR_floorCollisionHeightAt(int16_t x, int16_t y)
{
  return LHR_floorHeightAt(x,y) +
    LHR_getItemCollisionMapBit(x,y) * RCL_UNITS_PER_SQUARE; 
}

void LHR_getPlayerWeaponInfo(
  uint8_t *ammoType, uint8_t *projectileCount, uint8_t *canShoot)
{
  *ammoType = LHR_weaponAmmo(LHR_player.weapon);

  *projectileCount = LHR_GET_WEAPON_PROJECTILE_COUNT(LHR_player.weapon);

#if LHR_INFINITE_AMMO
  *canShoot = 1;
#else
  *canShoot = 
    ((*ammoType == LHR_AMMO_NONE) || 
     (LHR_player.ammo[*ammoType] >= *projectileCount) ||
     (LHR_game.cheatState & 0x80));
#endif
}

void LHR_playerRotateWeapon(uint8_t next)
{
  uint8_t initialWeapon = LHR_player.weapon;
  int8_t increment = next ? 1 : -1;

  while (1)
  {
    LHR_player.weapon =
      (LHR_WEAPONS_TOTAL + LHR_player.weapon + increment) % LHR_WEAPONS_TOTAL;

    if (LHR_player.weapon == initialWeapon)
      break;

    uint8_t ammo, projectileCount, canShoot;

    LHR_getPlayerWeaponInfo(&ammo,&projectileCount,&canShoot);
 
    if (canShoot)
      break;
  }
}

void LHR_initPlayer(void)
{
  RCL_initCamera(&LHR_player.camera);

  LHR_player.camera.resolution.x =
    LHR_GAME_RESOLUTION_X / LHR_RAYCASTING_SUBSAMPLE;

  LHR_player.camera.resolution.y = LHR_GAME_RESOLUTION_Y - LHR_HUD_BAR_HEIGHT;

  LHR_player.camera.position.x = RCL_UNITS_PER_SQUARE / 2 +
    LHR_currentLevel.levelPointer->playerStart[0] *  RCL_UNITS_PER_SQUARE;

  LHR_player.camera.position.y = RCL_UNITS_PER_SQUARE / 2 +
    LHR_currentLevel.levelPointer->playerStart[1] *  RCL_UNITS_PER_SQUARE;

  LHR_player.squarePosition[0] =
    LHR_player.camera.position.x / RCL_UNITS_PER_SQUARE;

  LHR_player.squarePosition[1] =
    LHR_player.camera.position.y / RCL_UNITS_PER_SQUARE;
  
  LHR_player.camera.height = LHR_floorHeightAt( 
      LHR_currentLevel.levelPointer->playerStart[0],
      LHR_currentLevel.levelPointer->playerStart[1]) +
      RCL_CAMERA_COLL_HEIGHT_BELOW;

  LHR_player.camera.direction = LHR_currentLevel.levelPointer->playerStart[2] *
    (RCL_UNITS_PER_SQUARE / 256);

  LHR_recomputePLayerDirection(); 

  LHR_player.previousVerticalSpeed = 0;

  LHR_player.headBobFrame = 0;

  LHR_player.weapon = LHR_WEAPON_KNIFE;

  LHR_player.weaponCooldownFrames = 0;
  LHR_player.lastHurtFrame = LHR_game.frame;
  LHR_player.lastItemTakenFrame = LHR_game.frame;

  LHR_player.health = LHR_PLAYER_START_HEALTH;

  LHR_player.previousWeaponDirection = 0;

  LHR_player.cards = 
#if LHR_UNLOCK_DOOR
  0x07;
#else
  0;
#endif

  LHR_player.justTeleported = 0;

  for (uint8_t i = 0; i < LHR_AMMO_TOTAL; ++i)
    LHR_player.ammo[i] = 0;
}

RCL_Unit LHR_ceilingHeightAt(int16_t x, int16_t y)
{
  uint8_t properties;
  LHR_TileDefinition tile =
    LHR_getMapTile(LHR_currentLevel.levelPointer,x,y,&properties);

  if (properties == LHR_TILE_PROPERTY_ELEVATOR)
    return LHR_CEILING_MAX_HEIGHT;

  uint8_t height = LHR_TILE_CEILING_HEIGHT(tile);

  return properties != LHR_TILE_PROPERTY_SQUEEZER ?
    (
      height != LHR_TILE_CEILING_MAX_HEIGHT ?
      ((LHR_TILE_FLOOR_HEIGHT(tile) + height) * LHR_WALL_HEIGHT_STEP) :
      LHR_CEILING_MAX_HEIGHT
    ) :
    LHR_movingWallHeight(
      LHR_TILE_FLOOR_HEIGHT(tile) * LHR_WALL_HEIGHT_STEP,
      (LHR_TILE_CEILING_HEIGHT(tile) + LHR_TILE_FLOOR_HEIGHT(tile))
         * LHR_WALL_HEIGHT_STEP,
      LHR_game.frameTime - LHR_currentLevel.timeStart);
}

/**
  Gets sprite (image and sprite size) for given item.
*/
void LHR_getItemSprite(
  uint8_t elementType, const uint8_t **sprite, uint8_t *spriteSize)
{
  *spriteSize = 0;
  *sprite = LHR_itemSprites + (elementType - 1) * LHR_TEXTURE_STORE_SIZE;

  switch (elementType)
  {
    case LHR_LEVEL_ELEMENT_TREE:
    case LHR_LEVEL_ELEMENT_RUIN:
    case LHR_LEVEL_ELEMENT_LAMP:
    case LHR_LEVEL_ELEMENT_TELEPORTER:
      *spriteSize = 2;
      break;

    case LHR_LEVEL_ELEMENT_TERMINAL:
      *spriteSize = 1;
      break;

    case LHR_LEVEL_ELEMENT_FINISH:
    case LHR_LEVEL_ELEMENT_COLUMN:
      *spriteSize = 3;
      break;

    case LHR_LEVEL_ELEMENT_CARD0:
    case LHR_LEVEL_ELEMENT_CARD1:
    case LHR_LEVEL_ELEMENT_CARD2:
      *sprite = LHR_itemSprites + 
        (LHR_LEVEL_ELEMENT_CARD0 - 1) * LHR_TEXTURE_STORE_SIZE;
      break;

    case LHR_LEVEL_ELEMENT_BLOCKER:
      *sprite = 0;
      break;

    default:
      break;
  }
}

/**
  Says whether given item type collides, i.e. stops player from moving.
*/
uint8_t LHR_itemCollides(uint8_t elementType)
{
  return 
    elementType == LHR_LEVEL_ELEMENT_BARREL ||
    elementType == LHR_LEVEL_ELEMENT_TREE ||
    elementType == LHR_LEVEL_ELEMENT_TERMINAL ||
    elementType == LHR_LEVEL_ELEMENT_COLUMN ||
    elementType == LHR_LEVEL_ELEMENT_RUIN ||
    elementType == LHR_LEVEL_ELEMENT_BLOCKER ||
    elementType == LHR_LEVEL_ELEMENT_LAMP;
}

void LHR_setGameState(uint8_t state)
{
  LHR_LOG("changing game state");
  LHR_game.state = state;
  LHR_game.stateTime = 0;
}

void LHR_setAndInitLevel(uint8_t levelNumber)
{
  LHR_LOG("setting and initializing level");

  const LHR_Level *level;

#if LHR_AVR
  memcpy_P(&LHR_ramLevel,LHR_levels[levelNumber],sizeof(LHR_Level));
  level = &LHR_ramLevel;
#else
  level = LHR_levels[levelNumber];
#endif

  LHR_game.currentRandom = 0;

  if (LHR_game.saved != LHR_CANT_SAVE)
    LHR_game.saved = 0;

  LHR_currentLevel.levelNumber = levelNumber;
  LHR_currentLevel.monstersDead = 0;
  LHR_currentLevel.backgroundImage = level->backgroundImage;
  LHR_currentLevel.levelPointer = level;
  LHR_currentLevel.bossCount = 0;
  LHR_currentLevel.floorColor = level->floorColor;
  LHR_currentLevel.ceilingColor = level->ceilingColor;
  LHR_currentLevel.completionTime10sOfS = 0;

  for (uint8_t i = 0; i < 7; ++i)
    LHR_currentLevel.textures[i] =
      LHR_wallTextures + level->textureIndices[i] * LHR_TEXTURE_STORE_SIZE;

  LHR_LOG("initializing doors");

  LHR_currentLevel.checkedDoorIndex = 0;
  LHR_currentLevel.doorRecordCount = 0;
  LHR_currentLevel.projectileRecordCount = 0;
  LHR_currentLevel.teleporterCount = 0;
  LHR_currentLevel.mapRevealMask = 
#if LHR_REVEAL_MAP
    0xffff;
#else
    0;
#endif

  for (uint8_t j = 0; j < LHR_MAP_SIZE; ++j)
  {
    for (uint8_t i = 0; i < LHR_MAP_SIZE; ++i)
    {
      uint8_t properties;
     
      LHR_getMapTile(level,i,j,&properties);

      if ((properties & LHR_TILE_PROPERTY_MASK) == LHR_TILE_PROPERTY_DOOR)
      {
        LHR_DoorRecord *d =
          &(LHR_currentLevel.doorRecords[LHR_currentLevel.doorRecordCount]);

        d->coords[0] = i;
        d->coords[1] = j;
        d->state = 0x00;

        LHR_currentLevel.doorRecordCount++;
      }

      if (LHR_currentLevel.doorRecordCount >= LHR_MAX_DOORS)
      {
        LHR_LOG("warning: too many doors!");
        break;
      }
    }

    if (LHR_currentLevel.doorRecordCount >= LHR_MAX_DOORS)
      break;
  }

  LHR_LOG("initializing level elements");

  LHR_currentLevel.itemRecordCount = 0;
  LHR_currentLevel.checkedItemIndex = 0;

  LHR_currentLevel.monsterRecordCount = 0;
  LHR_currentLevel.checkedMonsterIndex = 0;

  LHR_MonsterRecord *monster;

  for (uint16_t i = 0; i < ((LHR_MAP_SIZE * LHR_MAP_SIZE) / 8); ++i)
    LHR_currentLevel.itemCollisionMap[i] = 0;

  for (uint8_t i = 0; i < LHR_MAX_LEVEL_ELEMENTS; ++i)
  {
    const LHR_LevelElement *e = &(LHR_currentLevel.levelPointer->elements[i]);

    if (e->type != LHR_LEVEL_ELEMENT_NONE)
    {
      if (LHR_LEVEL_ELEMENT_TYPE_IS_MOSTER(e->type))
      {
        monster =
        &(LHR_currentLevel.monsterRecords[LHR_currentLevel.monsterRecordCount]);

        monster->stateType = (LHR_MONSTER_TYPE_TO_INDEX(e->type) << 4)
          | LHR_MONSTER_STATE_INACTIVE;
 
        monster->health =
          LHR_GET_MONSTER_MAX_HEALTH(LHR_MONSTER_TYPE_TO_INDEX(e->type));

        monster->coords[0] = e->coords[0] * 4 + 2;
        monster->coords[1] = e->coords[1] * 4 + 2;

        LHR_currentLevel.monsterRecordCount++;

        if (e->type == LHR_LEVEL_ELEMENT_MONSTER_ENDER)
          LHR_currentLevel.bossCount++;
      }
      else if ((e->type < LHR_LEVEL_ELEMENT_LOCK0) ||
        (e->type > LHR_LEVEL_ELEMENT_LOCK2))
      {
        LHR_currentLevel.itemRecords[LHR_currentLevel.itemRecordCount] = i;
        LHR_currentLevel.itemRecordCount++;

        if (e->type == LHR_LEVEL_ELEMENT_TELEPORTER)
          LHR_currentLevel.teleporterCount++;

        if (LHR_itemCollides(e->type))
          LHR_setItemCollisionMapBit(e->coords[0],e->coords[1],1);
      }
      else
      {
        uint8_t properties;
     
        LHR_getMapTile(level,e->coords[0],e->coords[1],&properties);

        if ((properties & LHR_TILE_PROPERTY_MASK) == LHR_TILE_PROPERTY_DOOR)
        {
          // find the door record and lock the door:
          for (uint16_t j = 0; j < LHR_currentLevel.doorRecordCount; ++j)
          {
            LHR_DoorRecord *d = &(LHR_currentLevel.doorRecords[j]);

            if (d->coords[0] == e->coords[0] && d->coords[1] == e->coords[1])
            {
              d->state |= (e->type - LHR_LEVEL_ELEMENT_LOCK0 + 1) << 6;
              break;
            }
          }
        }
        else
        {
          LHR_LOG("warning: lock not put on door tile!");
        }
      }
    }
  } 

  LHR_currentLevel.timeStart = LHR_game.frameTime; 
  LHR_currentLevel.frameStart = LHR_game.frame;

  LHR_game.spriteAnimationFrame = 0;

  LHR_initPlayer();
  LHR_setGameState(LHR_GAME_STATE_LEVEL_START);
  LHR_setMusic(LHR_MUSIC_NEXT);
  LHR_processEvent(LHR_EVENT_LEVEL_STARTS,levelNumber);
}

void LHR_createDefaultSaveData(uint8_t *memory)
{
  for (uint16_t i = 0; i < LHR_SAVE_SIZE; ++i)
    memory[i] = 0;
    
  memory[1] = LHR_DEFAULT_SETTINGS;
}

void LHR_init(void)
{
  LHR_LOG("initializing game")

  LHR_game.frame = 0;
  LHR_game.frameTime = 0;
  LHR_game.currentRandom = 0;
  LHR_game.cheatState = 0;
  LHR_game.continues = 1;

  RCL_initRayConstraints(&LHR_game.rayConstraints);
  LHR_game.rayConstraints.maxHits = LHR_RAYCASTING_MAX_HITS;
  LHR_game.rayConstraints.maxSteps = LHR_RAYCASTING_MAX_STEPS;

  RCL_initRayConstraints(&LHR_game.visibilityRayConstraints);
  LHR_game.visibilityRayConstraints.maxHits = 
    LHR_RAYCASTING_VISIBILITY_MAX_HITS;
  LHR_game.visibilityRayConstraints.maxSteps =
    LHR_RAYCASTING_VISIBILITY_MAX_STEPS;

  LHR_game.antiSpam = 0;

  LHR_LOG("computing average texture colors")

  for (uint8_t i = 0; i < LHR_WALL_TEXTURE_COUNT; ++i)
  {
    /** For simplicity, we round colors so that there is only 64 of them, and
      we count them up to 256. */

    uint8_t colorHistogram[64];

    for (uint8_t j = 0; j < 64; ++j)
      colorHistogram[j] = 0;

    for (uint8_t y = 0; y < LHR_TEXTURE_SIZE; ++y)
      for (uint8_t x = 0; x < LHR_TEXTURE_SIZE; ++x)
      {
        uint8_t color =
          LHR_getTexel(LHR_wallTextures + i * LHR_TEXTURE_STORE_SIZE,x,y) / 4;

        colorHistogram[color] += 1;

        if (colorHistogram[color] == 255)
          break;
      }

    uint8_t maxIndex = 0;

    for (uint8_t j = 0; j < 64; ++j)
    {
      if (colorHistogram[j] == 255)
      {
        maxIndex = j;
        break;
      }

      if (colorHistogram[j] > colorHistogram[maxIndex])
        maxIndex = j;
    }

    LHR_game.textureAverageColors[i] = maxIndex * 4;
  }

  for (uint16_t i = 0; i < LHR_GAME_RESOLUTION_Y; ++i)
    LHR_game.backgroundScaleMap[i] =
      (i * LHR_TEXTURE_SIZE) / LHR_GAME_RESOLUTION_Y;

  for (uint8_t i = 0; i < LHR_KEY_COUNT; ++i)
    LHR_game.keyStates[i] = 0;

  LHR_currentLevel.levelPointer = 0;
  LHR_game.backgroundScroll = 0;
  LHR_game.selectedMenuItem = 0;
  LHR_game.selectedLevel = 0;
  LHR_game.storyType = LHR_STORY_GLOBAL_INTRO;
  LHR_game.introTargetLevel = 0;
  LHR_game.outroTargetLevel = 255;
  LHR_game.settings = LHR_DEFAULT_SETTINGS;
  LHR_game.saved = 0;

  LHR_createDefaultSaveData(LHR_game.save);

  LHR_gameLoad(); // attempt to load settings

  if (LHR_game.saved != LHR_CANT_SAVE)
  {
    LHR_LOG("settings loaded");
    LHR_game.settings = LHR_game.save[1]; 
  }
  else
  {
    LHR_LOG("saving/loading not possible");
    LHR_game.save[0] = LHR_NUMBER_OF_LEVELS - 1; // revealed all levels
  }

#if LHR_ALL_LEVELS
  LHR_game.save[0] = LHR_NUMBER_OF_LEVELS - 1;
#endif

  LHR_setMusic((LHR_game.settings & 0x02) ?
    LHR_MUSIC_TURN_ON : LHR_MUSIC_TURN_OFF);

#if LHR_START_LEVEL == 0
  LHR_setGameState(LHR_GAME_STATE_INIT);
#else
  LHR_setAndInitLevel(LHR_START_LEVEL - 1);
#endif
}

/**
  Adds new projectile to the current level, returns 1 if added, 0 if not (max
  count reached).
*/
uint8_t LHR_createProjectile(LHR_ProjectileRecord projectile)
{
  if (LHR_currentLevel.projectileRecordCount >= LHR_MAX_PROJECTILES)
    return 0; 

  LHR_currentLevel.projectileRecords[LHR_currentLevel.projectileRecordCount] =
    projectile;
  
  LHR_currentLevel.projectileRecordCount++;

  return 1;
}

/**
  Launches projectile of given type from given position in given direction
  (has to be normalized), with given offset (so as to not collide with the
  shooting entity). Returns the same value as LHR_createProjectile.
*/
uint8_t LHR_launchProjectile(
  uint8_t type,   
  RCL_Vector2D shootFrom,
  RCL_Unit shootFromHeight,
  RCL_Vector2D direction,
  RCL_Unit verticalSpeed,
  RCL_Unit offsetDistance
  )
{
  if (type == LHR_PROJECTILE_NONE)
    return 0;

  LHR_ProjectileRecord p;

  p.type = type;
  p.doubleFramesToLive = 
    RCL_nonZero(LHR_GET_PROJECTILE_FRAMES_TO_LIVE(type) / 2);
  
  p.position[0] =
    shootFrom.x + (direction.x * offsetDistance) / RCL_UNITS_PER_SQUARE;
  p.position[1] = 
    shootFrom.y + (direction.y * offsetDistance) / RCL_UNITS_PER_SQUARE; 
  p.position[2] = shootFromHeight;

  p.direction[0] = 
    (direction.x * LHR_GET_PROJECTILE_SPEED_UPF(type)) / RCL_UNITS_PER_SQUARE;
  p.direction[1] =
    (direction.y * LHR_GET_PROJECTILE_SPEED_UPF(type)) / RCL_UNITS_PER_SQUARE;
  p.direction[2] = verticalSpeed;

  return LHR_createProjectile(p);
}

/**
  Pushes a given position away from a center by given distance, with collisions.
  Returns 1 if push away happened, otherwise 0.
*/
uint8_t LHR_pushAway(
  RCL_Unit pos[3],
  RCL_Unit centerX,
  RCL_Unit centerY,
  RCL_Unit preferredDirection,
  RCL_Unit distance)
{
  RCL_Vector2D fromCenter;

  fromCenter.x = pos[0] - centerX;
  fromCenter.y = pos[1] - centerY;

  RCL_Unit l = RCL_len(fromCenter);

  if (l < 128)
  {
    fromCenter = RCL_angleToDirection(preferredDirection);
    l = RCL_UNITS_PER_SQUARE;
  }

  RCL_Vector2D offset;

  offset.x = (fromCenter.x * distance) / l; 
  offset.y = (fromCenter.y * distance) / l; 

  RCL_Camera c;

  RCL_initCamera(&c);

  c.position.x = pos[0];
  c.position.y = pos[1];
  c.height = pos[2];

  RCL_moveCameraWithCollision(&c,offset,0,LHR_floorCollisionHeightAt,
    LHR_ceilingHeightAt,1,1);

  pos[0] = c.position.x;
  pos[1] = c.position.y;
  pos[2] = c.height;

  return 1;
}

uint8_t LHR_pushPlayerAway(
  RCL_Unit centerX, RCL_Unit centerY, RCL_Unit distance)
{
  RCL_Unit p[3];

  p[0] = LHR_player.camera.position.x; 
  p[1] = LHR_player.camera.position.y; 
  p[2] = LHR_player.camera.height; 

  uint8_t result = LHR_pushAway(p,centerX,centerY,
    LHR_player.camera.direction - RCL_UNITS_PER_SQUARE / 2,
    distance);

  LHR_player.camera.position.x = p[0]; 
  LHR_player.camera.position.y = p[1]; 
  LHR_player.camera.height = p[2];

  return result;
}

/**
  Helper function to resolve collision with level element. The function supposes
  the collision already does happen and only resolves it. Returns adjusted move
  offset.
*/
RCL_Vector2D LHR_resolveCollisionWithElement(
  RCL_Vector2D position, RCL_Vector2D moveOffset, RCL_Vector2D elementPos)
{
  RCL_Unit dx = RCL_abs(elementPos.x - position.x);
  RCL_Unit dy = RCL_abs(elementPos.y - position.y);

  if (dx > dy)
  {
    // colliding from left/right

    if ((moveOffset.x > 0) == (position.x < elementPos.x))
      moveOffset.x = 0;
      // ^ only stop if heading towards element, to avoid getting stuck
  }
  else
  {
    // colliding from up/down

    if ((moveOffset.y > 0) == (position.y < elementPos.y))
      moveOffset.y = 0;
  }

  return moveOffset;  
}

/**
  Adds or subtracts player's health during the playing state due to taking
  damage (negative value) or getting healed. Negative value will be corrected by
  LHR_PLAYER_DAMAGE_MULTIPLIER in this function.
*/
void LHR_playerChangeHealth(int8_t healthAdd)
{          
  if (LHR_game.state != LHR_GAME_STATE_PLAYING)
    return; // don't hurt during level starting phase

  if (healthAdd < 0)
  {
    if (LHR_game.cheatState & 0x80) // invincible?
      return;

    healthAdd =
      RCL_min(-1,
      (((RCL_Unit) healthAdd) * LHR_PLAYER_DAMAGE_MULTIPLIER) /
      RCL_UNITS_PER_SQUARE);

    LHR_player.lastHurtFrame = LHR_game.frame;
    LHR_processEvent(LHR_EVENT_VIBRATE,0);
    LHR_processEvent(LHR_EVENT_PLAYER_HURT,-1 * healthAdd);
  }

  int16_t health = LHR_player.health;
  health += healthAdd;
  health = RCL_clamp(health,0,LHR_PLAYER_MAX_HEALTH);

  LHR_player.health = health;
}

uint8_t LHR_distantSoundVolume(RCL_Unit x, RCL_Unit y, RCL_Unit z)
{
  RCL_Unit distance = LHR_taxicabDistance(x,y,z,
                        LHR_player.camera.position.x,
                        LHR_player.camera.position.y,
                        LHR_player.camera.height);

  if (distance >= LHR_SFX_MAX_DISTANCE)
    return 0;

  uint32_t result = 255 - (distance * 255) / LHR_SFX_MAX_DISTANCE;

  return (result * result) / 256;
}

/**
  Same as LHR_playerChangeHealth but for monsters.
*/
void LHR_monsterChangeHealth(LHR_MonsterRecord *monster, int8_t healthAdd)
{
  int16_t health = monster->health;

  health += healthAdd;
  health = RCL_clamp(health,0,255);
  monster->health = health;

  if (healthAdd < 0)
  {
    // play hurt sound
    
    uint8_t volume = LHR_distantSoundVolume( 
      LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]),
      LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]),
      LHR_floorHeightAt(
        LHR_MONSTER_COORD_TO_SQUARES(monster->coords[0]),
        LHR_MONSTER_COORD_TO_SQUARES(monster->coords[1])));
    
      LHR_playGameSound(5,volume);
    
    if (monster->health == 0)
      LHR_playGameSound(2,volume);
  }
}

void LHR_removeItem(uint8_t index)
{
  LHR_LOG("removing item");

  for (uint16_t j = index; j < LHR_currentLevel.itemRecordCount - 1; ++j)
    LHR_currentLevel.itemRecords[j] =
      LHR_currentLevel.itemRecords[j + 1];

  LHR_currentLevel.itemRecordCount--; 
}

/**
  Checks a 3D point visibility from player's position (WITHOUT considering
  facing direction).
*/
static inline uint8_t LHR_spriteIsVisible(RCL_Vector2D pos, RCL_Unit height)
{
  return
    RCL_castRay3D(
      LHR_player.camera.position,
      LHR_player.camera.height,
      pos,
      height,
      LHR_floorHeightAt,
      LHR_ceilingHeightAt,
      LHR_game.visibilityRayConstraints
    ) == RCL_UNITS_PER_SQUARE;
}

RCL_Unit LHR_directionTangent(RCL_Unit dirX, RCL_Unit dirY, RCL_Unit dirZ)
{
  RCL_Vector2D v;

  v.x = dirX;
  v.y = dirY;

  return (dirZ * RCL_UNITS_PER_SQUARE) / RCL_len(v);
}

/**
  Returns a tangent in RCL_Unit of vertical autoaim, given current game state.
*/
RCL_Unit LHR_autoaimVertically(void)
{
  for (uint16_t i = 0; i < LHR_currentLevel.monsterRecordCount; ++i)
  {
    LHR_MonsterRecord m = LHR_currentLevel.monsterRecords[i];
    
    uint8_t state = LHR_MR_STATE(m);
 
    if (state == LHR_MONSTER_STATE_INACTIVE ||
        state == LHR_MONSTER_STATE_DEAD)
      continue;

    RCL_Vector2D worldPosition, toMonster;

    worldPosition.x = LHR_MONSTER_COORD_TO_RCL_UNITS(m.coords[0]);
    worldPosition.y = LHR_MONSTER_COORD_TO_RCL_UNITS(m.coords[1]);
    
    toMonster.x = worldPosition.x - LHR_player.camera.position.x;
    toMonster.y = worldPosition.y - LHR_player.camera.position.y;

    if (RCL_abs(
         RCL_vectorsAngleCos(LHR_player.direction,toMonster) 
         - RCL_UNITS_PER_SQUARE) < LHR_VERTICAL_AUTOAIM_ANGLE_THRESHOLD)
    {
      uint8_t spriteSize = LHR_GET_MONSTER_SPRITE_SIZE(
        LHR_MONSTER_TYPE_TO_INDEX(LHR_MR_TYPE(m)));
        
      RCL_Unit worldHeight = 
        LHR_floorHeightAt(
          LHR_MONSTER_COORD_TO_SQUARES(m.coords[0]),
          LHR_MONSTER_COORD_TO_SQUARES(m.coords[1]))
          + 
          LHR_SPRITE_SIZE_TO_HEIGHT_ABOVE_GROUND(spriteSize);
        
      if (LHR_spriteIsVisible(worldPosition,worldHeight))
        return LHR_directionTangent(toMonster.x,toMonster.y,
               worldHeight - (LHR_player.camera.height));
    }
  }

  return 0;
}

/**
  Helper function, returns a pointer to level element representing item with
  given index, but only if the item is active (otherwise 0 is returned).
*/
static inline const LHR_LevelElement *LHR_getActiveItemElement(uint8_t index)
{
  LHR_ItemRecord item = LHR_currentLevel.itemRecords[index];

  if ((item & LHR_ITEM_RECORD_ACTIVE_MASK) == 0)
    return 0;

  return &(LHR_currentLevel.levelPointer->elements[item &
           ~LHR_ITEM_RECORD_ACTIVE_MASK]);
}

static inline const LHR_LevelElement *LHR_getLevelElement(uint8_t index)
{
  LHR_ItemRecord item = LHR_currentLevel.itemRecords[index];

  return &(LHR_currentLevel.levelPointer->elements[item &
           ~LHR_ITEM_RECORD_ACTIVE_MASK]);
}
  
void LHR_createExplosion(RCL_Unit, RCL_Unit, RCL_Unit); // forward decl

void LHR_explodeBarrel(uint8_t itemIndex, RCL_Unit x, RCL_Unit y, RCL_Unit z)
{
  const LHR_LevelElement *e = LHR_getLevelElement(itemIndex);
  LHR_setItemCollisionMapBit(e->coords[0],e->coords[1],0);
  LHR_removeItem(itemIndex);
  LHR_createExplosion(x,y,z);
}

void LHR_createExplosion(RCL_Unit x, RCL_Unit y, RCL_Unit z)
{
  LHR_ProjectileRecord explosion;

  LHR_playGameSound(2,LHR_distantSoundVolume(x,y,z));
  LHR_processEvent(LHR_EVENT_EXPLOSION,0);

  explosion.type = LHR_PROJECTILE_EXPLOSION;

  explosion.position[0] = x;
  explosion.position[1] = y;
  explosion.position[2] = z;

  explosion.direction[0] = 0;
  explosion.direction[1] = 0;
  explosion.direction[2] = 0;

  explosion.doubleFramesToLive = RCL_nonZero(
    LHR_GET_PROJECTILE_FRAMES_TO_LIVE(LHR_PROJECTILE_EXPLOSION) / 2);

  LHR_createProjectile(explosion);

  uint8_t damage = LHR_getDamageValue(LHR_WEAPON_FIRE_TYPE_FIREBALL);

  if (LHR_taxicabDistance(x,y,z,LHR_player.camera.position.x,
    LHR_player.camera.position.y,LHR_player.camera.height)
    <= LHR_EXPLOSION_RADIUS)
  {
    LHR_playerChangeHealth(-1 * damage);
    LHR_pushPlayerAway(x,y,LHR_EXPLOSION_PUSH_AWAY_DISTANCE);
  }

  for (uint16_t i = 0; i < LHR_currentLevel.monsterRecordCount; ++i)
  {
    LHR_MonsterRecord *monster = &(LHR_currentLevel.monsterRecords[i]);

    uint16_t state = LHR_MR_STATE(*monster); 

    if ((state == LHR_MONSTER_STATE_INACTIVE) ||
        (state == LHR_MONSTER_STATE_DEAD))
      continue; 

    RCL_Unit monsterHeight =
      LHR_floorHeightAt(
        LHR_MONSTER_COORD_TO_SQUARES(monster->coords[0]),
        LHR_MONSTER_COORD_TO_SQUARES(monster->coords[1]))
        + RCL_UNITS_PER_SQUARE / 2;

    if (LHR_taxicabDistance(
      LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]),
      LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]),monsterHeight,
      x,y,z) <= LHR_EXPLOSION_RADIUS)
    {
      LHR_monsterChangeHealth(monster,
        -1 * LHR_getDamageValue(LHR_WEAPON_FIRE_TYPE_FIREBALL));
    }
  }

  // explode nearby barrels

  if (damage >= LHR_BARREL_EXPLOSION_DAMAGE_THRESHOLD)
    for (uint16_t i = 0; i < LHR_currentLevel.itemRecordCount; ++i)
    {
      LHR_ItemRecord item = LHR_currentLevel.itemRecords[i];

      /* We DON'T check just active barrels but all, otherwise it looks weird
         that out of sight barrels in a line didn't explode.*/

      LHR_LevelElement element = LHR_ITEM_RECORD_LEVEL_ELEMENT(item);

      if (element.type != LHR_LEVEL_ELEMENT_BARREL)
        continue;

      RCL_Unit elementX =
        element.coords[0] * RCL_UNITS_PER_SQUARE + RCL_UNITS_PER_SQUARE / 2;

      RCL_Unit elementY =
        element.coords[1] * RCL_UNITS_PER_SQUARE + RCL_UNITS_PER_SQUARE / 2;

      RCL_Unit elementHeight =
        LHR_floorHeightAt(element.coords[0],element.coords[1]);

      if (LHR_taxicabDistance(
        x,y,z,elementX,elementY,elementHeight) <= LHR_EXPLOSION_RADIUS)
      {
        LHR_explodeBarrel(i,elementX,elementY,elementHeight);
        i--;
      }
    }
}

void LHR_createDust(RCL_Unit x, RCL_Unit y, RCL_Unit z)
{
  LHR_ProjectileRecord dust;

  dust.type = LHR_PROJECTILE_DUST;

  dust.position[0] = x;
  dust.position[1] = y;
  dust.position[2] = z;

  dust.direction[0] = 0;
  dust.direction[1] = 0;
  dust.direction[2] = 0;

  dust.doubleFramesToLive =
    RCL_nonZero(LHR_GET_PROJECTILE_FRAMES_TO_LIVE(LHR_PROJECTILE_DUST) / 2);

  LHR_createProjectile(dust);
}

void LHR_getMonsterWorldPosition(LHR_MonsterRecord *monster, RCL_Unit *x,
  RCL_Unit *y, RCL_Unit *z)
{
  *x = LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]);
  *y = LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]);
  *z = LHR_floorHeightAt(
         LHR_MONSTER_COORD_TO_SQUARES(monster->coords[0]),
         LHR_MONSTER_COORD_TO_SQUARES(monster->coords[1]))
       + RCL_UNITS_PER_SQUARE / 2;
}

void LHR_monsterPerformAI(LHR_MonsterRecord *monster)
{
  uint8_t state = LHR_MR_STATE(*monster);
  uint8_t type = LHR_MR_TYPE(*monster);
  uint8_t monsterNumber = LHR_MONSTER_TYPE_TO_INDEX(type);
  uint8_t attackType = LHR_GET_MONSTER_ATTACK_TYPE(monsterNumber);

  int8_t coordAdd[2];

  coordAdd[0] = 0;
  coordAdd[1] = 0;

  uint8_t notRanged =
    (attackType == LHR_MONSTER_ATTACK_MELEE) || 
    (attackType == LHR_MONSTER_ATTACK_EXPLODE); 

  uint8_t monsterSquare[2];
   /* because of some insanely retarded C++ compilers that error on narrowing
      conversion between { } we init this way: */
  monsterSquare[0] = LHR_MONSTER_COORD_TO_SQUARES(monster->coords[0]);
  monsterSquare[1] = LHR_MONSTER_COORD_TO_SQUARES(monster->coords[1]);

  RCL_Unit currentHeight =
    LHR_floorCollisionHeightAt(monsterSquare[0],monsterSquare[1]);

  if ( // ranged monsters: sometimes randomly attack
       !notRanged &&
       (LHR_random() < 
       LHR_GET_MONSTER_AGGRESSIVITY(LHR_MONSTER_TYPE_TO_INDEX(type)))
     )
  { 
    RCL_Vector2D pos;
    pos.x = LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]);
    pos.y = LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]);

    if (LHR_random() % 4 != 0 &&
      LHR_spriteIsVisible(pos,currentHeight + // only if player is visible
        LHR_SPRITE_SIZE_TO_HEIGHT_ABOVE_GROUND(
        LHR_GET_MONSTER_SPRITE_SIZE(
        LHR_MONSTER_TYPE_TO_INDEX(type)))))
    {
      // ranged attack
 
      state = LHR_MONSTER_STATE_ATTACKING;

      RCL_Vector2D dir;

      dir.x = LHR_player.camera.position.x - pos.x
        - 128 * LHR_MONSTER_AIM_RANDOMNESS + 
        LHR_random() * LHR_MONSTER_AIM_RANDOMNESS;

      dir.y = LHR_player.camera.position.y - pos.y
        - 128 * LHR_MONSTER_AIM_RANDOMNESS + 
        LHR_random() * LHR_MONSTER_AIM_RANDOMNESS;

      uint8_t projectile;  

      switch (LHR_GET_MONSTER_ATTACK_TYPE(monsterNumber))
      {
        case LHR_MONSTER_ATTACK_FIREBALL:
          projectile = LHR_PROJECTILE_FIREBALL; 
          break;

        case LHR_MONSTER_ATTACK_BULLET:
          projectile = LHR_PROJECTILE_BULLET; 
          break;

        case LHR_MONSTER_ATTACK_PLASMA:
          projectile = LHR_PROJECTILE_PLASMA;
          break;

        case LHR_MONSTER_ATTACK_FIREBALL_BULLET:
          projectile = (LHR_random() < 128) ?
            LHR_PROJECTILE_FIREBALL : 
            LHR_PROJECTILE_BULLET;
          break;

        case LHR_MONSTER_ATTACK_FIREBALL_PLASMA:
          projectile = (LHR_random() < 128) ?
            LHR_PROJECTILE_FIREBALL : 
            LHR_PROJECTILE_PLASMA;
          break;

        default:
          projectile = LHR_PROJECTILE_NONE; 
          break;
      }

      if (projectile == LHR_PROJECTILE_BULLET)
        LHR_playGameSound(0,
          LHR_distantSoundVolume( 
            LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]),
            LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]),
            currentHeight)
          );

      RCL_Unit middleHeight = currentHeight +
        LHR_SPRITE_SIZE_TO_HEIGHT_ABOVE_GROUND(LHR_GET_MONSTER_SPRITE_SIZE(
        LHR_MONSTER_TYPE_TO_INDEX(LHR_MR_TYPE(*monster))));

      RCL_Unit verticalSpeed = (
        ((projectile != LHR_PROJECTILE_NONE) ? 
        LHR_GET_PROJECTILE_SPEED_UPF(projectile) : 0) * 
        LHR_directionTangent(dir.x,dir.y,LHR_player.camera.height -
        middleHeight)) / RCL_UNITS_PER_SQUARE;
      
      dir = RCL_normalize(dir);

      LHR_launchProjectile(
        projectile,
        pos,
        middleHeight,
        dir,
        verticalSpeed,
        LHR_PROJECTILE_SPAWN_OFFSET
      );
    } // if visible
    else
      state = LHR_MONSTER_STATE_IDLE;
  }
  else if (state == LHR_MONSTER_STATE_IDLE)
  {
    if (notRanged)
    {
      // non-ranged monsters: walk towards player

      RCL_Unit pX, pY, pZ;
      LHR_getMonsterWorldPosition(monster,&pX,&pY,&pZ);

      uint8_t isClose = // close to player?
        LHR_taxicabDistance(pX,pY,pZ,
          LHR_player.camera.position.x,
          LHR_player.camera.position.y,
          LHR_player.camera.height) <= LHR_MELEE_RANGE;

      if (!isClose)
      {
        // walk towards player

        if (monsterSquare[0] > LHR_player.squarePosition[0])
        {
          if (monsterSquare[1] > LHR_player.squarePosition[1])
            state = LHR_MONSTER_STATE_GOING_NW;
          else if (monsterSquare[1] < LHR_player.squarePosition[1])
            state = LHR_MONSTER_STATE_GOING_SW;
          else
            state = LHR_MONSTER_STATE_GOING_W;
        }
        else if (monsterSquare[0] < LHR_player.squarePosition[0])
        {
          if (monsterSquare[1] > LHR_player.squarePosition[1])
            state = LHR_MONSTER_STATE_GOING_NE;
          else if (monsterSquare[1] < LHR_player.squarePosition[1])
            state = LHR_MONSTER_STATE_GOING_SE;
          else
            state = LHR_MONSTER_STATE_GOING_E;
        }
        else
        {
          if (monsterSquare[1] > LHR_player.squarePosition[1])
            state = LHR_MONSTER_STATE_GOING_N;
          else if (monsterSquare[1] < LHR_player.squarePosition[1])
            state = LHR_MONSTER_STATE_GOING_S;
        }
      }
      else // is close
      {
        // melee, close-up attack

        if (attackType == LHR_MONSTER_ATTACK_MELEE)
        {
          // melee attack

          state = LHR_MONSTER_STATE_ATTACKING;

          LHR_playerChangeHealth(
            -1 * LHR_getDamageValue(LHR_WEAPON_FIRE_TYPE_MELEE)); 
              
          LHR_playGameSound(3,255);
        }
        else // LHR_MONSTER_ATTACK_EXPLODE
        {
          // explode

          LHR_createExplosion(pX,pY,pZ);
          monster->health = 0;
        }
      }
    }
    else // ranged monsters
    {
      // choose walk direction randomly

      switch (LHR_random() % 8)
      {
        case 0: state = LHR_MONSTER_STATE_GOING_E; break;
        case 1: state = LHR_MONSTER_STATE_GOING_W; break;
        case 2: state = LHR_MONSTER_STATE_GOING_N; break;
        case 3: state = LHR_MONSTER_STATE_GOING_S; break;
        case 4: state = LHR_MONSTER_STATE_GOING_NE; break;
        case 5: state = LHR_MONSTER_STATE_GOING_NW; break;
        case 6: state = LHR_MONSTER_STATE_GOING_SE; break;
        case 7: state = LHR_MONSTER_STATE_GOING_SW; break;
        default: break;
      }
    }
  }
  else if (state == LHR_MONSTER_STATE_ATTACKING)
  {
    state = LHR_MONSTER_STATE_IDLE;
  }
  else
  {
    int8_t add = 1;

    if (attackType == LHR_MONSTER_ATTACK_MELEE)
      add = 2;
    else if (attackType == LHR_MONSTER_ATTACK_EXPLODE)
      add = 3;

    if (state == LHR_MONSTER_STATE_GOING_E ||
        state == LHR_MONSTER_STATE_GOING_NE ||
        state == LHR_MONSTER_STATE_GOING_SE)
      coordAdd[0] = add;
    else if (state == LHR_MONSTER_STATE_GOING_W ||
        state == LHR_MONSTER_STATE_GOING_SW ||
        state == LHR_MONSTER_STATE_GOING_NW)
      coordAdd[0] = -1 * add;

    if (state == LHR_MONSTER_STATE_GOING_N ||
        state == LHR_MONSTER_STATE_GOING_NE ||
        state == LHR_MONSTER_STATE_GOING_NW)
      coordAdd[1] = -1 * add;
    else if (state == LHR_MONSTER_STATE_GOING_S ||
        state == LHR_MONSTER_STATE_GOING_SE ||
        state == LHR_MONSTER_STATE_GOING_SW)
      coordAdd[1] = add;

    if ((coordAdd[0] != 0 || coordAdd[1] != 0) && LHR_random() <
        LHR_MONSTER_SOUND_PROBABILITY)
      LHR_playGameSound(5,
          LHR_distantSoundVolume( 
          LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]),
          LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]),
          currentHeight) / 2);

    state = LHR_MONSTER_STATE_IDLE;
  }

  int16_t newPos[2];

  newPos[0] = monster->coords[0] + coordAdd[0];
  newPos[1] = monster->coords[1] + coordAdd[1];

  int8_t collision = 0;

  if (newPos[0] < 0 || newPos[0] >= 256 || newPos[1] < 0 || newPos[1] >= 256)
  {
    collision = 1;
  }
  else
  {
    uint8_t movingDiagonally = (coordAdd[0] != 0) && (coordAdd[1] != 0);

    // when moving diagonally, we need to check extra tiles

    for (uint8_t i = 0; i < (1 + movingDiagonally); ++i)
    {
      newPos[0] = monster->coords[0] + (i != 1) * coordAdd[0];

      RCL_Unit newHeight =
        LHR_floorCollisionHeightAt(
          LHR_MONSTER_COORD_TO_SQUARES(newPos[0]),
          LHR_MONSTER_COORD_TO_SQUARES(newPos[1]));

      collision =
        RCL_abs(currentHeight - newHeight) > RCL_CAMERA_COLL_STEP_HEIGHT;

      if (!collision)
        collision = (LHR_ceilingHeightAt(
          LHR_MONSTER_COORD_TO_SQUARES(newPos[0]),
          LHR_MONSTER_COORD_TO_SQUARES(newPos[1])) - newHeight) <
          LHR_MONSTER_COLLISION_HEIGHT;

      if (collision)
        break;
    }

    newPos[0] = monster->coords[0] + coordAdd[0];
  }

  if (collision)
  {
    state = LHR_MONSTER_STATE_IDLE;
    // ^ will force the monster to choose random direction in the next update
 
    newPos[0] = monster->coords[0];
    newPos[1] = monster->coords[1];
  }

  monster->stateType = state | (monsterNumber << 4);
  monster->coords[0] = newPos[0];
  monster->coords[1] = newPos[1];;
}

static inline uint8_t LHR_elementCollides(
  RCL_Unit pointX,
  RCL_Unit pointY,
  RCL_Unit pointZ,
  RCL_Unit elementX,
  RCL_Unit elementY,
  RCL_Unit elementHeight
)
{
  return
    LHR_taxicabDistance(pointX,pointY,pointZ,elementX,elementY,elementHeight)
    <= LHR_ELEMENT_COLLISION_RADIUS;
}

/**
  Checks collision of a projectile with level element at given position.
*/
uint8_t LHR_projectileCollides(LHR_ProjectileRecord *projectile,
  RCL_Unit x, RCL_Unit y, RCL_Unit z)
{
  if (!LHR_elementCollides(x,y,z,
    projectile->position[0],projectile->position[1],projectile->position[2]))
    return 0;

  if ((projectile->type == LHR_PROJECTILE_EXPLOSION) ||
      (projectile->type == LHR_PROJECTILE_DUST))
    return 0;

  /* For directional projectiles we only register a collision if its direction
     is "towards" the element so that the shooter doesn't get shot by his own
     projectile. */

  RCL_Vector2D projDir, toElement;

  projDir.x = projectile->direction[0]; 
  projDir.y = projectile->direction[1];

  toElement.x = x - projectile->position[0];
  toElement.y = y - projectile->position[1];
   
  return RCL_vectorsAngleCos(projDir,toElement) >= 0;
}

/**
  Updates a frame of the currently loaded level, i.e. enemies, projectiles,
  animations etc., with the exception of player.
*/
void LHR_updateLevel(void)
{
  // update projectiles:

  uint8_t subtractFrames =
    ((LHR_game.frame - LHR_currentLevel.frameStart) & 0x01) ? 1 : 0;
    /* ^ only subtract frames to live every other frame because a maximum of
       256 frames would be too few */

  for (int8_t i = 0; i < LHR_currentLevel.projectileRecordCount; ++i)
  { // ^ has to be signed
    LHR_ProjectileRecord *p = &(LHR_currentLevel.projectileRecords[i]);

    uint8_t attackType = 255;

    if (p->type == LHR_PROJECTILE_BULLET)
      attackType = LHR_WEAPON_FIRE_TYPE_BULLET;
    else if (p->type == LHR_PROJECTILE_PLASMA)
      attackType = LHR_WEAPON_FIRE_TYPE_PLASMA;

    RCL_Unit pos[3] = {0,0,0}; /* we have to convert from uint16_t because of
                                  under/overflows */
    uint8_t eliminate = 0;

    for (uint8_t j = 0; j < 3; ++j) 
    {
      pos[j] = p->position[j];
      pos[j] += p->direction[j];

      if ( // projectile outside map?
        (pos[j] < 0) ||
        (pos[j] >= (LHR_MAP_SIZE * RCL_UNITS_PER_SQUARE)))
      {
        eliminate = 1;
        break;
      }
    }

    if (p->doubleFramesToLive == 0) // no more time to live?
    {
      eliminate = 1;
    }
    else if (
      (p->type != LHR_PROJECTILE_EXPLOSION) &&
      (p->type != LHR_PROJECTILE_DUST))
    {
      if (LHR_projectileCollides( // collides with player?
            p,
            LHR_player.camera.position.x,
            LHR_player.camera.position.y,
            LHR_player.camera.height))
        {
          eliminate = 1;

          LHR_playerChangeHealth(-1 * LHR_getDamageValue(attackType));
        }

      /* Check collision with the map (we don't use LHR_floorCollisionHeightAt
         because collisions with items have to be done differently for
         projectiles). */

      if (!eliminate &&
          ((LHR_floorHeightAt(pos[0] / RCL_UNITS_PER_SQUARE,pos[1] / 
            RCL_UNITS_PER_SQUARE) >= pos[2])
          ||
          (LHR_ceilingHeightAt(pos[0] / RCL_UNITS_PER_SQUARE,pos[1] /
            RCL_UNITS_PER_SQUARE) <= pos[2]))
        )
        eliminate = 1;

      // check collision with active level elements

      if (!eliminate) // monsters 
        for (uint16_t j = 0; j < LHR_currentLevel.monsterRecordCount; ++j)
        {
          LHR_MonsterRecord *m = &(LHR_currentLevel.monsterRecords[j]);

          uint8_t state = LHR_MR_STATE(*m);

          if ((state != LHR_MONSTER_STATE_INACTIVE) &&
              (state != LHR_MONSTER_STATE_DEAD))
          {
            if (LHR_projectileCollides(p,
                  LHR_MONSTER_COORD_TO_RCL_UNITS(m->coords[0]),
                  LHR_MONSTER_COORD_TO_RCL_UNITS(m->coords[1]),
                  LHR_floorHeightAt(
                    LHR_MONSTER_COORD_TO_SQUARES(m->coords[0]),
                    LHR_MONSTER_COORD_TO_SQUARES(m->coords[1]))
                   ))
            {
              eliminate = 1;
              LHR_monsterChangeHealth(m,-1 * LHR_getDamageValue(attackType));
              break;
            }
          }
        }

      if (!eliminate) // items (can't check itemCollisionMap because of barrels)
        for (uint16_t j = 0; j < LHR_currentLevel.itemRecordCount; ++j)
        {
          const LHR_LevelElement *e = LHR_getActiveItemElement(j);

          if (e != 0 && LHR_itemCollides(e->type))
          {
            RCL_Unit x = LHR_ELEMENT_COORD_TO_RCL_UNITS(e->coords[0]);
            RCL_Unit y = LHR_ELEMENT_COORD_TO_RCL_UNITS(e->coords[1]);
            RCL_Unit z = LHR_floorHeightAt(e->coords[0],e->coords[1]);

            if (LHR_projectileCollides(p,x,y,z))
            {
              if (
                   (e->type == LHR_LEVEL_ELEMENT_BARREL) &&
                   (LHR_getDamageValue(attackType) >= 
                     LHR_BARREL_EXPLOSION_DAMAGE_THRESHOLD)
                 )
              {
                LHR_explodeBarrel(j,x,y,z);
              }

              eliminate = 1;
              break;
            }
          }
        }
    }

    if (eliminate)
    {
      if (p->type == LHR_PROJECTILE_FIREBALL)
        LHR_createExplosion(p->position[0],p->position[1],p->position[2]);
      else if (p->type == LHR_PROJECTILE_BULLET)
        LHR_createDust(p->position[0],p->position[1],p->position[2]);
      else if (p->type == LHR_PROJECTILE_PLASMA)
        LHR_playGameSound(4,LHR_distantSoundVolume(pos[0],pos[1],pos[2]));

      // remove the projectile

      for (uint8_t j = i; j < LHR_currentLevel.projectileRecordCount - 1; ++j)
        LHR_currentLevel.projectileRecords[j] =
          LHR_currentLevel.projectileRecords[j + 1];

      LHR_currentLevel.projectileRecordCount--;

      i--;
    }
    else
    {
      p->position[0] = pos[0];
      p->position[1] = pos[1];
      p->position[2] = pos[2];
    }

    p->doubleFramesToLive -= subtractFrames;
  }

  // handle door:
  if (LHR_currentLevel.doorRecordCount > 0) // has to be here
  {
    /* Check door on whether a player is standing nearby. For performance
       reasons we only check a few doors and move to others in the next
       frame. */
   
    if (LHR_currentLevel.checkedDoorIndex == 0)
    {
      uint8_t count = LHR_player.cards >> 6;

      LHR_player.cards = (count <= 1) ?
        (LHR_player.cards & 0x07) :
        ((LHR_player.cards & 0x7f) | ((count - 1) << 6));
    }
 
    for (uint16_t i = 0;
         i < RCL_min(LHR_ELEMENT_DISTANCES_CHECKED_PER_FRAME,
           LHR_currentLevel.doorRecordCount);
         ++i) 
    {
      LHR_DoorRecord *door =
        &(LHR_currentLevel.doorRecords[LHR_currentLevel.checkedDoorIndex]);

      uint8_t upDownState = door->state & LHR_DOOR_UP_DOWN_MASK;

      uint8_t newUpDownState = 0;
      
      uint8_t lock = LHR_DOOR_LOCK(door->state);

      if ( // player near door?
        (door->coords[0] >= (LHR_player.squarePosition[0] - 1)) &&
        (door->coords[0] <= (LHR_player.squarePosition[0] + 1)) &&
        (door->coords[1] >= (LHR_player.squarePosition[1] - 1)) &&
        (door->coords[1] <= (LHR_player.squarePosition[1] + 1)))
      {
        if (lock == 0)
        {
          newUpDownState = LHR_DOOR_UP_DOWN_MASK;    
        }
        else
        {
          lock = 1 << (lock - 1);

          if (LHR_player.cards & lock) // player has the card?
            newUpDownState = LHR_DOOR_UP_DOWN_MASK;
          else
            LHR_player.cards = 
              (LHR_player.cards & 0x07) | (lock << 3) | (2 << 6);
        }
      }

      if (upDownState != newUpDownState)
        LHR_playGameSound(1,255);

      door->state = (door->state & ~LHR_DOOR_UP_DOWN_MASK) | newUpDownState;

      LHR_currentLevel.checkedDoorIndex++;

      if (LHR_currentLevel.checkedDoorIndex >= LHR_currentLevel.doorRecordCount)
        LHR_currentLevel.checkedDoorIndex = 0;
    }

    // move door up/down:
    for (uint32_t i = 0; i < LHR_currentLevel.doorRecordCount; ++i)
    {
      LHR_DoorRecord *door = &(LHR_currentLevel.doorRecords[i]);

      int8_t height = door->state & LHR_DOOR_VERTICAL_POSITION_MASK;

      height = (door->state & LHR_DOOR_UP_DOWN_MASK) ?
            RCL_min(0x1f,height + LHR_DOOR_INCREMENT_PER_FRAME) :
            RCL_max(0x00,height - LHR_DOOR_INCREMENT_PER_FRAME);

      door->state = (door->state & ~LHR_DOOR_VERTICAL_POSITION_MASK) | height;
    }
  }

  // handle items, in a similar manner to door:
  if (LHR_currentLevel.itemRecordCount > 0) // has to be here
  {
    // check item distances:

    for (uint16_t i = 0;
         i < RCL_min(LHR_ELEMENT_DISTANCES_CHECKED_PER_FRAME,
           LHR_currentLevel.itemRecordCount);
         ++i) 
    {
      LHR_ItemRecord item =
        LHR_currentLevel.itemRecords[LHR_currentLevel.checkedItemIndex];

      item &= ~LHR_ITEM_RECORD_ACTIVE_MASK;

      LHR_LevelElement e =
        LHR_currentLevel.levelPointer->elements[item];

      if (
        LHR_isInActiveDistanceFromPlayer(
          e.coords[0] * RCL_UNITS_PER_SQUARE + RCL_UNITS_PER_SQUARE / 2,
          e.coords[1] * RCL_UNITS_PER_SQUARE + RCL_UNITS_PER_SQUARE / 2,
          LHR_floorHeightAt(e.coords[0],e.coords[1]) + RCL_UNITS_PER_SQUARE / 2)
        )
        item |= LHR_ITEM_RECORD_ACTIVE_MASK;

      LHR_currentLevel.itemRecords[LHR_currentLevel.checkedItemIndex] = item;

      LHR_currentLevel.checkedItemIndex++;

      if (LHR_currentLevel.checkedItemIndex >= LHR_currentLevel.itemRecordCount)
        LHR_currentLevel.checkedItemIndex = 0;
    }
  }

  // similarly handle monsters:
  if (LHR_currentLevel.monsterRecordCount > 0) // has to be here
  {
    // check monster distances:

    for (uint16_t i = 0;
         i < RCL_min(LHR_ELEMENT_DISTANCES_CHECKED_PER_FRAME,
           LHR_currentLevel.monsterRecordCount);
         ++i) 
    {
      LHR_MonsterRecord *monster =
      &(LHR_currentLevel.monsterRecords[LHR_currentLevel.checkedMonsterIndex]);

      if ( // far away from the player?
        !LHR_isInActiveDistanceFromPlayer(
          LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]),
          LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]),
          LHR_floorHeightAt(
            LHR_MONSTER_COORD_TO_SQUARES(monster->coords[0]),
            LHR_MONSTER_COORD_TO_SQUARES(monster->coords[1]))
            + RCL_UNITS_PER_SQUARE / 2
          )
        )
      {
        monster->stateType = 
           (monster->stateType & LHR_MONSTER_MASK_TYPE) |
           LHR_MONSTER_STATE_INACTIVE;
      }
      else if (LHR_MR_STATE(*monster) == LHR_MONSTER_STATE_INACTIVE)
      {
        monster->stateType = 
          (monster->stateType & LHR_MONSTER_MASK_TYPE) |
          (monster->health != 0 ? 
            LHR_MONSTER_STATE_IDLE : LHR_MONSTER_STATE_DEAD);
      }

      LHR_currentLevel.checkedMonsterIndex++;

      if (LHR_currentLevel.checkedMonsterIndex >=
        LHR_currentLevel.monsterRecordCount)
        LHR_currentLevel.checkedMonsterIndex = 0;
    }
  }

  // update AI and handle dead monsters:
  if ((LHR_game.frame - LHR_currentLevel.frameStart) %
      LHR_AI_UPDATE_FRAME_INTERVAL == 0)
  {
    for (uint16_t i = 0; i < LHR_currentLevel.monsterRecordCount; ++i)
    {
      LHR_MonsterRecord *monster = &(LHR_currentLevel.monsterRecords[i]);
      uint8_t state = LHR_MR_STATE(*monster);

      if ((state == LHR_MONSTER_STATE_INACTIVE) || 
          (state == LHR_MONSTER_STATE_DEAD))
        continue;

      if (state == LHR_MONSTER_STATE_DYING)
      {
        monster->stateType =
          (monster->stateType & 0xf0) | LHR_MONSTER_STATE_DEAD;
      }
      else if (monster->health == 0)
      {
        monster->stateType = (monster->stateType & LHR_MONSTER_MASK_TYPE) |
          LHR_MONSTER_STATE_DYING;

        if (LHR_MR_TYPE(*monster) == LHR_LEVEL_ELEMENT_MONSTER_ENDER)
        {
          LHR_currentLevel.bossCount--;

          // last boss killed gives player a key card

          if (LHR_currentLevel.bossCount == 0)
          {
            LHR_LOG("boss killed, giving player a card");
            LHR_player.cards |= 0x04;
          }
        }

        LHR_processEvent(LHR_EVENT_MONSTER_DIES,LHR_MR_TYPE(*monster));

        if (LHR_MR_TYPE(*monster) == LHR_LEVEL_ELEMENT_MONSTER_EXPLODER)
          LHR_createExplosion(
            LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[0]),
            LHR_MONSTER_COORD_TO_RCL_UNITS(monster->coords[1]),
            LHR_floorCollisionHeightAt(
              LHR_MONSTER_COORD_TO_SQUARES(monster->coords[0]),
              LHR_MONSTER_COORD_TO_SQUARES(monster->coords[0])) +
            RCL_UNITS_PER_SQUARE / 2);
      }
      else
      {
#if LHR_PREVIEW_MODE == 0
        LHR_monsterPerformAI(monster);
#endif
      }
    }
  }
}

/**
  Maps square position on the map to a bit in map reveal mask.
*/
static inline uint16_t LHR_getMapRevealBit(uint8_t squareX, uint8_t squareY)
{
  return 1 << ((squareY / 16) * 4 + squareX / 16);
}

/**
  Draws text on screen using the bitmap font stored in assets.
*/
void LHR_drawText(
  const char *text,
  uint16_t x,
  uint16_t y,
  uint8_t size,
  uint8_t color,
  uint16_t maxLength,
  uint16_t limitX)
{
  if (size == 0)
    size = 1;

  if (limitX == 0)
    limitX = 65535;

  if (maxLength == 0)
    maxLength = 65535;

  uint16_t pos = 0;

  uint16_t currentX = x;
  uint16_t currentY = y;

  while (pos < maxLength && text[pos] != 0) // for each character
  {
    uint16_t character = 
      //LHR_PROGRAM_MEMORY_U8(LHR_font + LHR_charToFontIndex(text[pos]));
      LHR_font[LHR_charToFontIndex(text[pos])];

    for (uint8_t i = 0; i < LHR_FONT_CHARACTER_SIZE; ++i) // for each line
    {
      currentY = y;

      for (uint8_t j = 0; j < LHR_FONT_CHARACTER_SIZE; ++j) // for each row
      {
        if (character & 0x8000)
          for (uint8_t k = 0; k < size; ++k)
            for (uint8_t l = 0; l < size; ++l)
            {
              uint16_t drawX = currentX + k;
              uint16_t drawY = currentY + l;

              if (drawX < LHR_GAME_RESOLUTION_X &&
                drawY < LHR_GAME_RESOLUTION_Y)
                LHR_setGamePixel(drawX,drawY,color);
            }

        currentY += size;
        character = character << 1;
      }

      currentX += size;
    }
    
    currentX += size; // space
      
    if (currentX > limitX)
    {
      currentX = x;
      y += (LHR_FONT_CHARACTER_SIZE + 1) * size;
    }

    pos++;    
  }
}

void LHR_drawLevelStartOverlay(void)
{
  uint8_t stage = (LHR_game.stateTime * 4) / LHR_LEVEL_START_DURATION;

  // fade in:

  for (uint16_t y = 0; y < LHR_GAME_RESOLUTION_Y; ++y)
    for (uint16_t x = 0; x < LHR_GAME_RESOLUTION_X; ++x)
    {
      uint8_t draw = 0;

      switch (stage)
      {
        case 0: draw = 1; break;
        case 1: draw = (x % 2) || (y % 2); break;
        case 2: draw = (x % 2) == (y % 2); break;
        case 3: draw = (x % 2) && (y % 2); break;
        default: break;
      }

      if (draw)
        LHR_setGamePixel(x,y,0);
    }

  if (LHR_game.saved == 1)
    LHR_drawText(LHR_TEXT_SAVED,LHR_HUD_MARGIN,LHR_HUD_MARGIN,
      LHR_FONT_SIZE_MEDIUM,7,255,0);
}

/**
  Sets player's height to match the floor height below him.
*/
void LHR_updatePlayerHeight(void)
{
  LHR_player.camera.height =
    LHR_floorCollisionHeightAt(
      LHR_player.squarePosition[0],LHR_player.squarePosition[1]) +
      RCL_CAMERA_COLL_HEIGHT_BELOW;
}

void LHR_winLevel(void)
{
  LHR_levelEnds();
  LHR_setGameState(LHR_GAME_STATE_WIN);
  LHR_playGameSound(2,255); 
  LHR_processEvent(LHR_EVENT_VIBRATE,0);
  LHR_processEvent(LHR_EVENT_LEVEL_WON,LHR_currentLevel.levelNumber + 1);
}

/**
  Part of LHR_gameStep() for LHR_GAME_STATE_PLAYING.
*/
static inline void LHR_updateAnimatedEnvironments(void)
{
#if LHR_ANIMATED_ENVIRONMENTS
  // Cycle floor and ceiling colors for a basic pulsating effect
  // Every 8 frames, increment color index within a safe 8-color gradient range
  uint8_t tick = (LHR_game.frame / 8) % 4;
  LHR_currentLevel.floorColor = LHR_currentLevel.levelPointer->floorColor + tick;
  LHR_currentLevel.ceilingColor = LHR_currentLevel.levelPointer->ceilingColor + tick;
  
  // Skybox animation: cycle through up to 3 background images
  // For demonstration, cycles every 16 frames if animation is enabled
  LHR_currentLevel.backgroundImage = (LHR_currentLevel.levelPointer->backgroundImage + (LHR_game.frame / 16)) % 3;
#endif
}

void LHR_gameStepPlaying(void)
{
#if LHR_QUICK_WIN
  if (LHR_game.stateTime > 500)
    LHR_winLevel();
#endif

  if (
    (LHR_keyIsDown(LHR_KEY_C) && LHR_keyIsDown(LHR_KEY_DOWN)) ||
    LHR_keyIsDown(LHR_KEY_MENU))
  {
    LHR_setGameState(LHR_GAME_STATE_MENU);
    LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);
    return;
  }

  LHR_updateLevel();
  LHR_updateAnimatedEnvironments();

  int8_t recomputeDirection = LHR_currentLevel.frameStart == LHR_game.frame;

  RCL_Vector2D moveOffset;

  moveOffset.x = 0;
  moveOffset.y = 0;

  int8_t strafe = 0;

  uint8_t currentWeapon = LHR_player.weapon;

#if LHR_HEADBOB_ENABLED
  int8_t bobbing = 0;
#endif

  int8_t shearing = 0;

  if (LHR_player.weaponCooldownFrames > 0)
    LHR_player.weaponCooldownFrames--;

  if (LHR_keyJustPressed(LHR_KEY_TOGGLE_FREELOOK))
    LHR_game.settings = (LHR_game.settings & 0x04) ?
      (LHR_game.settings & ~0x0c) : (LHR_game.settings | 0x0c);

  int8_t canSwitchWeapon = LHR_player.weaponCooldownFrames == 0;

  if (LHR_keyJustPressed(LHR_KEY_NEXT_WEAPON) && canSwitchWeapon)
    LHR_playerRotateWeapon(1);
  else if (LHR_keyJustPressed(LHR_KEY_PREVIOUS_WEAPON) && canSwitchWeapon)
    LHR_playerRotateWeapon(0);
  else if (LHR_keyJustPressed(LHR_KEY_CYCLE_WEAPON) &&
    LHR_player.previousWeaponDirection)
    LHR_playerRotateWeapon(LHR_player.previousWeaponDirection > 0);

  uint8_t shearingOn = LHR_game.settings & 0x04;

  if (LHR_keyIsDown(LHR_KEY_B))
  {
    if (shearingOn)                      // B + U/D: shearing (if on)
    {
      if (LHR_keyIsDown(LHR_KEY_UP))
      {
        LHR_player.camera.shear =
          RCL_min(LHR_CAMERA_MAX_SHEAR_PIXELS,
                  LHR_player.camera.shear + LHR_CAMERA_SHEAR_STEP_PER_FRAME);

        shearing = 1;
      }
      else if (LHR_keyIsDown(LHR_KEY_DOWN))
      {
        LHR_player.camera.shear =
          RCL_max(-1 * LHR_CAMERA_MAX_SHEAR_PIXELS,
                  LHR_player.camera.shear - LHR_CAMERA_SHEAR_STEP_PER_FRAME);

        shearing = 1;
      }
    }

    if (!LHR_keyIsDown(LHR_KEY_C))
    {                                    // B + L/R: strafing
      if (LHR_keyIsDown(LHR_KEY_LEFT))
        strafe = -1;
      else if (LHR_keyIsDown(LHR_KEY_RIGHT))
        strafe = 1;
    }
  }

  if (LHR_keyIsDown(LHR_KEY_C))          // C + A/B/L/R: weapon switching
  {
    if ((LHR_keyJustPressed(LHR_KEY_LEFT) || LHR_keyJustPressed(LHR_KEY_A)) &&
      canSwitchWeapon)
      LHR_playerRotateWeapon(0);
    else if (
      (LHR_keyJustPressed(LHR_KEY_RIGHT) || LHR_keyJustPressed(LHR_KEY_B)) &&
      canSwitchWeapon)
      LHR_playerRotateWeapon(1);
  }
  else if (!LHR_keyIsDown(LHR_KEY_B))    // L/R: turning
  {
    if (LHR_keyIsDown(LHR_KEY_LEFT))
    {
      LHR_player.camera.direction -= LHR_PLAYER_TURN_UNITS_PER_FRAME;
      recomputeDirection = 1;
    }
    else if (LHR_keyIsDown(LHR_KEY_RIGHT))
    {
      LHR_player.camera.direction += LHR_PLAYER_TURN_UNITS_PER_FRAME;
      recomputeDirection = 1;
    } 
  }

  if (!LHR_keyIsDown(LHR_KEY_B) || !shearingOn)     // U/D: movement
  {
    if (LHR_keyIsDown(LHR_KEY_UP))
    {
      moveOffset.x += LHR_player.direction.x;
      moveOffset.y += LHR_player.direction.y;
#if LHR_HEADBOB_ENABLED
      bobbing = 1;
#endif
    }
    else if (LHR_keyIsDown(LHR_KEY_DOWN))
    {
      moveOffset.x -= LHR_player.direction.x;
      moveOffset.y -= LHR_player.direction.y;
#if LHR_HEADBOB_ENABLED
      bobbing = 1;
#endif
    }
  }

  int16_t mouseX = 0, mouseY = 0;

  LHR_getMouseOffset(&mouseX,&mouseY);

  if (mouseX != 0)                                  // mouse turning
  {
    LHR_player.camera.direction += 
      (mouseX * LHR_MOUSE_SENSITIVITY_HORIZONTAL) / 128;

    recomputeDirection = 1;
  }

  if ((mouseY != 0) && shearingOn)                  // mouse shearing
    LHR_player.camera.shear =
      RCL_max(RCL_min(
        LHR_player.camera.shear - 
        (mouseY * LHR_MOUSE_SENSITIVITY_VERTICAL) / 128,
        LHR_CAMERA_MAX_SHEAR_PIXELS),
        -1 * LHR_CAMERA_MAX_SHEAR_PIXELS);

  if (recomputeDirection)
    LHR_recomputePLayerDirection();

  if (LHR_keyIsDown(LHR_KEY_STRAFE_LEFT))
    strafe = -1;
  else if (LHR_keyIsDown(LHR_KEY_STRAFE_RIGHT))
    strafe = 1;

  if (strafe != 0)
  {
    uint8_t normalize = (moveOffset.x != 0) || (moveOffset.y != 0);

    moveOffset.x += strafe * LHR_player.direction.y;
    moveOffset.y -= strafe * LHR_player.direction.x;

    if (normalize)
    {
      // This prevents reaching higher speed when moving diagonally.

      moveOffset = RCL_normalize(moveOffset);

      moveOffset.x = (moveOffset.x * LHR_PLAYER_MOVE_UNITS_PER_FRAME)
        / RCL_UNITS_PER_SQUARE;

      moveOffset.y = (moveOffset.y * LHR_PLAYER_MOVE_UNITS_PER_FRAME)
        / RCL_UNITS_PER_SQUARE;
    }
  }

#if LHR_PREVIEW_MODE
  if (LHR_keyIsDown(LHR_KEY_B))
    LHR_player.verticalSpeed = LHR_PLAYER_MOVE_UNITS_PER_FRAME;
  else if (LHR_keyIsDown(LHR_KEY_C))
    LHR_player.verticalSpeed = -1 * LHR_PLAYER_MOVE_UNITS_PER_FRAME;
  else
    LHR_player.verticalSpeed = 0;
#else
  RCL_Unit verticalOffset = 
    (  
      (
        LHR_keyIsDown(LHR_KEY_JUMP) ||
        (LHR_keyIsDown(LHR_KEY_UP) && LHR_keyIsDown(LHR_KEY_C))
      ) &&
      (LHR_player.verticalSpeed == 0) &&
      (LHR_player.previousVerticalSpeed == 0)) ?
    LHR_PLAYER_JUMP_OFFSET_PER_FRAME : // jump
    (LHR_player.verticalSpeed - LHR_GRAVITY_SPEED_INCREASE_PER_FRAME);
#endif

  if (!shearing && LHR_player.camera.shear != 0 && !(LHR_game.settings & 0x08))
  {
    // gradually shear back to zero

    LHR_player.camera.shear =
      (LHR_player.camera.shear > 0) ?
      RCL_max(0,LHR_player.camera.shear - LHR_CAMERA_SHEAR_STEP_PER_FRAME) :
      RCL_min(0,LHR_player.camera.shear + LHR_CAMERA_SHEAR_STEP_PER_FRAME);
  }

#if LHR_HEADBOB_ENABLED && !LHR_PREVIEW_MODE
  if (bobbing)
  {
    LHR_player.headBobFrame += LHR_HEADBOB_FRAME_INCREASE_PER_FRAME; 
  }
  else if (LHR_player.headBobFrame != 0)
  {
    // smoothly stop bobbing

    uint8_t quadrant = (LHR_player.headBobFrame % RCL_UNITS_PER_SQUARE) /
      (RCL_UNITS_PER_SQUARE / 4);

    /* When in quadrant in which sin is going away from zero, switch to the
       same value of the next quadrant, so that bobbing starts to go towards
       zero immediately. */

    if (quadrant % 2 == 0)
      LHR_player.headBobFrame =
        ((quadrant + 1) * RCL_UNITS_PER_SQUARE / 4) +
        (RCL_UNITS_PER_SQUARE / 4 - LHR_player.headBobFrame %
        (RCL_UNITS_PER_SQUARE / 4));

    RCL_Unit currentFrame = LHR_player.headBobFrame;
    RCL_Unit nextFrame = LHR_player.headBobFrame + 16;

    // only stop bobbing when we pass a frame at which sin crosses zero
    LHR_player.headBobFrame =
      (currentFrame / (RCL_UNITS_PER_SQUARE / 2) ==
       nextFrame / (RCL_UNITS_PER_SQUARE / 2)) ?
       nextFrame : 0;
  }
#endif

  RCL_Unit previousHeight = LHR_player.camera.height;

  // handle player collision with level elements:

  // monsters:
  for (uint16_t i = 0; i < LHR_currentLevel.monsterRecordCount; ++i)
  {
    LHR_MonsterRecord *m = &(LHR_currentLevel.monsterRecords[i]);

    uint8_t state = LHR_MR_STATE(*m);

    if (state == LHR_MONSTER_STATE_INACTIVE || state == LHR_MONSTER_STATE_DEAD)
      continue;

    RCL_Vector2D mPos;

    mPos.x = LHR_MONSTER_COORD_TO_RCL_UNITS(m->coords[0]);
    mPos.y = LHR_MONSTER_COORD_TO_RCL_UNITS(m->coords[1]);

    if (
         LHR_elementCollides(
           LHR_player.camera.position.x,
           LHR_player.camera.position.y,
           LHR_player.camera.height,
           mPos.x,
           mPos.y,
           LHR_floorHeightAt(
               LHR_MONSTER_COORD_TO_SQUARES(m->coords[0]),
               LHR_MONSTER_COORD_TO_SQUARES(m->coords[1]))
         )
       )
    {
      moveOffset = LHR_resolveCollisionWithElement(
        LHR_player.camera.position,moveOffset,mPos);
    }
  }

  uint8_t collidesWithTeleporter = 0;

  /* item collisions with player (only those that don't stop player's movement,
     as those are handled differently, via itemCollisionMap): */
  for (int16_t i = 0; i < LHR_currentLevel.itemRecordCount; ++i)
    // ^ has to be int16_t (signed)
  {
    if (!(LHR_currentLevel.itemRecords[i] & LHR_ITEM_RECORD_ACTIVE_MASK))
      continue;

    const LHR_LevelElement *e = LHR_getActiveItemElement(i);

    if (e != 0)
    {
      RCL_Vector2D ePos;

      ePos.x = LHR_ELEMENT_COORD_TO_RCL_UNITS(e->coords[0]);
      ePos.y = LHR_ELEMENT_COORD_TO_RCL_UNITS(e->coords[1]);

      if (!LHR_itemCollides(e->type) &&
          LHR_elementCollides(
            LHR_player.camera.position.x,
            LHR_player.camera.position.y,
            LHR_player.camera.height,
            ePos.x,
            ePos.y,
            LHR_floorHeightAt(e->coords[0],e->coords[1]))
         )
      {
        uint8_t eliminate = 1;

        uint8_t onlyKnife = 1;

        for (uint8_t j = 0; j < LHR_AMMO_TOTAL; ++j)
          if (LHR_player.ammo[j] != 0)
          {
            onlyKnife = 0;
            break;
          }

        switch (e->type)
        {
          case LHR_LEVEL_ELEMENT_HEALTH:
            if (LHR_player.health < LHR_PLAYER_MAX_HEALTH)
              LHR_playerChangeHealth(LHR_HEALTH_KIT_VALUE);
            else
              eliminate = 0;
            break;

#define addAmmo(type) \
  if (LHR_player.ammo[LHR_AMMO_##type] < LHR_AMMO_MAX_##type) \
  {\
    LHR_player.ammo[LHR_AMMO_##type] = RCL_min(LHR_AMMO_MAX_##type,\
      LHR_player.ammo[LHR_AMMO_##type] + LHR_AMMO_INCREASE_##type);\
    if (onlyKnife) LHR_playerRotateWeapon(1); \
  }\
  else\
    eliminate = 0;

          case LHR_LEVEL_ELEMENT_BULLETS:
            addAmmo(BULLETS)
            break;

          case LHR_LEVEL_ELEMENT_ROCKETS:
            addAmmo(ROCKETS)
            break;

          case LHR_LEVEL_ELEMENT_PLASMA:
            addAmmo(PLASMA)
            break;

#undef addAmmo

          case LHR_LEVEL_ELEMENT_CARD0:
          case LHR_LEVEL_ELEMENT_CARD1:
          case LHR_LEVEL_ELEMENT_CARD2:
            LHR_player.cards |= 1 << (e->type - LHR_LEVEL_ELEMENT_CARD0);
            break;

          case LHR_LEVEL_ELEMENT_TELEPORTER:
            collidesWithTeleporter = 1;
            eliminate = 0;
            break;

          case LHR_LEVEL_ELEMENT_FINISH:
            LHR_winLevel();
            eliminate = 0;
            break;

          default:
            eliminate = 0;
            break;
        }

        if (eliminate) // take the item
        {
#if !LHR_PREVIEW_MODE
          LHR_removeItem(i);
          LHR_player.lastItemTakenFrame = LHR_game.frame;
          i--;
          LHR_playGameSound(3,255);
          LHR_processEvent(LHR_EVENT_PLAYER_TAKES_ITEM,e->type);
#endif
        }
        else if (
          e->type == LHR_LEVEL_ELEMENT_TELEPORTER &&
          LHR_currentLevel.teleporterCount > 1 &&
          !LHR_player.justTeleported)
        {
          // teleport to random destination teleporter

          uint8_t teleporterNumber =
            LHR_random() % (LHR_currentLevel.teleporterCount - 1) + 1;

          for (uint16_t j = 0; j < LHR_currentLevel.itemRecordCount; ++j)
          {
            LHR_LevelElement e2 = 
              LHR_currentLevel.levelPointer->elements
                [LHR_currentLevel.itemRecords[j] &
                ~LHR_ITEM_RECORD_ACTIVE_MASK];

            if ((e2.type == LHR_LEVEL_ELEMENT_TELEPORTER) && (j != i))
              teleporterNumber--;

            if (teleporterNumber == 0)
            {
              LHR_player.camera.position.x =
                LHR_ELEMENT_COORD_TO_RCL_UNITS(e2.coords[0]);

              LHR_player.camera.position.y =
                LHR_ELEMENT_COORD_TO_RCL_UNITS(e2.coords[1]);

              LHR_player.camera.height =
                LHR_floorHeightAt(e2.coords[0],e2.coords[1]) +
                RCL_CAMERA_COLL_HEIGHT_BELOW;

              LHR_currentLevel.itemRecords[j] |= LHR_ITEM_RECORD_ACTIVE_MASK;
              /* ^ we have to make the new teleporter immediately active so
                 that it will immediately collide */

              LHR_player.justTeleported = 1;

              LHR_playGameSound(4,255);
              LHR_processEvent(LHR_EVENT_PLAYER_TELEPORTS,0);

              break;
            } // if teleporterNumber == 0
          } // for level items
        } // if eliminate
      } // if item collides
    } // if element != 0 
  } // for, item collision check

  if (!collidesWithTeleporter)
    LHR_player.justTeleported = 0;

#if LHR_PREVIEW_MODE
  LHR_player.camera.position.x +=
    LHR_PREVIEW_MODE_SPEED_MULTIPLIER * moveOffset.x;

  LHR_player.camera.position.y +=
    LHR_PREVIEW_MODE_SPEED_MULTIPLIER * moveOffset.y;

  LHR_player.camera.height += 
    LHR_PREVIEW_MODE_SPEED_MULTIPLIER * LHR_player.verticalSpeed;
#else
  RCL_moveCameraWithCollision(&(LHR_player.camera),moveOffset,
    verticalOffset,LHR_floorCollisionHeightAt,LHR_ceilingHeightAt,1,1);

  LHR_player.previousVerticalSpeed = LHR_player.verticalSpeed;

  RCL_Unit limit = RCL_max(RCL_max(0,verticalOffset),LHR_player.verticalSpeed);
  
  LHR_player.verticalSpeed =
    RCL_min(limit,LHR_player.camera.height - previousHeight);
  /* ^ By "limit" we assure height increase caused by climbing a step doesn't
     add vertical velocity. */
#endif

#if LHR_PREVIEW_MODE == 0
  if (
    LHR_keyIsDown(LHR_KEY_A) &&
    !LHR_keyIsDown(LHR_KEY_C) &&
    (LHR_player.weaponCooldownFrames == 0) &&
    (LHR_game.stateTime > 400) // don't immediately shoot if returning from menu
    )
  {
    /* Player attack/shoot/fire, this has to be done AFTER the player is moved,
       otherwise he could shoot himself while running forward. */

    uint8_t ammo, projectileCount, canShoot;

    LHR_getPlayerWeaponInfo(&ammo,&projectileCount,&canShoot);

    if (canShoot)
    {
      uint8_t sound;

      switch (LHR_player.weapon)
      {
        case LHR_WEAPON_KNIFE:           sound = 255; break;
        case LHR_WEAPON_ROCKET_LAUNCHER: 
        case LHR_WEAPON_SHOTGUN:         sound = 2; break; 
        case LHR_WEAPON_PLASMAGUN:
        case LHR_WEAPON_SOLUTION:        sound = 4; break;
        default:                         sound = 0; break;
      }

      if (sound != 255)
        LHR_playGameSound(sound,255);

      if (ammo != LHR_AMMO_NONE)
        LHR_player.ammo[ammo] -= projectileCount;

      uint8_t projectile;

      switch (LHR_GET_WEAPON_FIRE_TYPE(LHR_player.weapon))
      {
        case LHR_WEAPON_FIRE_TYPE_PLASMA:
          projectile = LHR_PROJECTILE_PLASMA;
          break;

        case LHR_WEAPON_FIRE_TYPE_FIREBALL:
          projectile = LHR_PROJECTILE_FIREBALL;
          break;

        case LHR_WEAPON_FIRE_TYPE_BULLET:
          projectile = LHR_PROJECTILE_BULLET;
          break;

        case LHR_WEAPON_FIRE_TYPE_MELEE:
          projectile = LHR_PROJECTILE_NONE;
          break;

        default:
          projectile = 255;
          break;
      }
          
      if (projectile != LHR_PROJECTILE_NONE)
      {
        uint16_t angleAdd = LHR_PROJECTILE_SPREAD_ANGLE / (projectileCount + 1);

        RCL_Unit direction =
          (LHR_player.camera.direction - LHR_PROJECTILE_SPREAD_ANGLE / 2) 
          + angleAdd;
          
        RCL_Unit projectileSpeed = LHR_GET_PROJECTILE_SPEED_UPF(projectile);
        
        /* Vertical speed will be either determined by autoaim (if shearing is
           off) or the camera shear value. */
        RCL_Unit verticalSpeed = (LHR_game.settings & 0x04) ?
          (LHR_player.camera.shear * projectileSpeed * 2) / // only approximate
            LHR_CAMERA_MAX_SHEAR_PIXELS
          :
          (projectileSpeed * LHR_autoaimVertically()) / RCL_UNITS_PER_SQUARE;

        for (uint8_t i = 0; i < projectileCount; ++i)
        {
          LHR_launchProjectile(
            projectile,
            LHR_player.camera.position,
            LHR_player.camera.height,
            RCL_angleToDirection(direction),
            verticalSpeed,  
            LHR_PROJECTILE_SPAWN_OFFSET
            );

          direction += angleAdd;
        }
      }
      else
      {
        // player's melee attack

        for (uint16_t i = 0; i < LHR_currentLevel.monsterRecordCount; ++i)
        {
          LHR_MonsterRecord *m = &(LHR_currentLevel.monsterRecords[i]);

          uint8_t state = LHR_MR_STATE(*m);

          if ((state == LHR_MONSTER_STATE_INACTIVE) || 
              (state == LHR_MONSTER_STATE_DEAD))
            continue;

          RCL_Unit pX, pY, pZ;
          LHR_getMonsterWorldPosition(m,&pX,&pY,&pZ);

          if (LHR_taxicabDistance(pX,pY,pZ,
              LHR_player.camera.position.x,
              LHR_player.camera.position.y,
              LHR_player.camera.height) > LHR_MELEE_RANGE)
            continue;
   
          RCL_Vector2D toMonster;

          toMonster.x = pX - LHR_player.camera.position.x;
          toMonster.y = pY - LHR_player.camera.position.y;

          if (RCL_vectorsAngleCos(LHR_player.direction,toMonster) >=
              (RCL_UNITS_PER_SQUARE - LHR_PLAYER_MELEE_ANGLE))
          {
            LHR_monsterChangeHealth(m,
              -1 * LHR_getDamageValue(LHR_WEAPON_FIRE_TYPE_MELEE));

            LHR_createDust(pX,pY,pZ);

            break;
          }
        }
      }

      LHR_player.weaponCooldownFrames =
        RCL_max(
          LHR_GET_WEAPON_FIRE_COOLDOWN_FRAMES(LHR_player.weapon),
          LHR_MIN_WEAPON_COOLDOWN_FRAMES);

      LHR_getPlayerWeaponInfo(&ammo,&projectileCount,&canShoot);

      if (!canShoot)
      {
        // No more ammo, switch to the second strongest weapon.

        LHR_playerRotateWeapon(1);

        uint8_t previousWeapon = LHR_player.weapon;

        LHR_playerRotateWeapon(0);

        if (previousWeapon > LHR_player.weapon)
          LHR_playerRotateWeapon(1);
      }
    } // endif: has enough ammo?
  } // attack
#endif // LHR_PREVIEW_MODE == 0

  LHR_player.squarePosition[0] =
    LHR_player.camera.position.x / RCL_UNITS_PER_SQUARE;

  LHR_player.squarePosition[1] =
    LHR_player.camera.position.y / RCL_UNITS_PER_SQUARE;

  LHR_currentLevel.mapRevealMask |= 
    LHR_getMapRevealBit(
      LHR_player.squarePosition[0],
      LHR_player.squarePosition[1]);
              
  uint8_t properties;

  LHR_getMapTile(LHR_currentLevel.levelPointer,LHR_player.squarePosition[0],
    LHR_player.squarePosition[1],&properties);

  if ( // squeezer check
     (properties == LHR_TILE_PROPERTY_SQUEEZER) &&
     ((LHR_ceilingHeightAt(
       LHR_player.squarePosition[0],LHR_player.squarePosition[1]) -
     LHR_floorHeightAt(
       LHR_player.squarePosition[0],LHR_player.squarePosition[1]))
     <
     (RCL_CAMERA_COLL_HEIGHT_ABOVE + RCL_CAMERA_COLL_HEIGHT_BELOW)))
  {
    LHR_LOG("player is squeezed");
    LHR_player.health = 0;
  }

  if (LHR_player.weapon != currentWeapon) // if weapon switched, start cooldown
  {
    if (LHR_player.weapon == (currentWeapon + 1) % LHR_WEAPONS_TOTAL)
      LHR_player.previousWeaponDirection = -1;
    else if (currentWeapon == (LHR_player.weapon + 1) % LHR_WEAPONS_TOTAL)
      LHR_player.previousWeaponDirection = 1;
    else
      LHR_player.previousWeaponDirection = 0;

    LHR_player.weaponCooldownFrames =
      LHR_GET_WEAPON_FIRE_COOLDOWN_FRAMES(LHR_player.weapon) / 2;
  }

#if LHR_IMMORTAL == 0
  if (LHR_player.health == 0)
  {
    LHR_LOG("player dies");
    LHR_levelEnds();
    LHR_processEvent(LHR_EVENT_VIBRATE,0);
    LHR_processEvent(LHR_EVENT_PLAYER_DIES,0);
    LHR_setGameState(LHR_GAME_STATE_LOSE);
  }
#endif
}

/**
  This function defines which items are displayed in the menu.
*/
uint8_t LHR_getMenuItem(uint8_t index)
{
  uint8_t current = 0;

  while (1) // find first legitimate item
  {
    if ( // skip non-legitimate items
      ((current <= LHR_MENU_ITEM_MAP) && (LHR_currentLevel.levelPointer == 0))
      || ((current == LHR_MENU_ITEM_LOAD) && ((LHR_game.save[0] >> 4) == 0))
      || ((current == LHR_MENU_ITEM_LANGUAGE) && !LHR_LOCALE_RUNTIME_SWITCH)
      || ((current == LHR_MENU_ITEM_SHEAR) && LHR_PC))
    {
      current++;
      continue;
    }

    if (index == 0)
      return (current <= (LHR_MENU_ITEM_EXIT - (LHR_CAN_EXIT ? 0 : 1))
        ) ? current : LHR_MENU_ITEM_NONE;

    current++;
    index--;
  }

  return LHR_MENU_ITEM_NONE;
}

void LHR_handleCheats(void)
{
  // this is a state machine handling cheat typing

  uint8_t expectedKey;

  switch (LHR_game.cheatState & 0x0f)
  {
    case 0: case 3: case 5: case 7: case 10:
      expectedKey = LHR_KEY_A; break;
    case 1: case 8:
      expectedKey = LHR_KEY_B; break;
    case 2: case 9:
      expectedKey = LHR_KEY_RIGHT; break;
    case 4:
      expectedKey = LHR_KEY_C; break;
    case 6: default:
      expectedKey = LHR_KEY_DOWN; break;
  }

  for (uint8_t i = 0; i < LHR_KEY_COUNT; ++i) // no other keys must be pressed
    if ((i != expectedKey) && LHR_keyJustPressed(i))
    {
      LHR_game.cheatState &= 0xf0; // back to start state
      return;
    }
 
  if (!LHR_keyJustPressed(expectedKey))
    return;

  LHR_game.cheatState++; // go to next state

  if ((LHR_game.cheatState & 0x0f) > 10) // final state resolved?
  {
    if (LHR_game.cheatState & 0x80)
    {
      LHR_LOG("cheat disabled");
      LHR_game.cheatState = 0;
    }
    else
    {
      LHR_LOG("cheat activated");
      LHR_playGameSound(4,255);
      LHR_playerChangeHealth(LHR_PLAYER_MAX_HEALTH);
      LHR_player.ammo[LHR_AMMO_BULLETS] = LHR_AMMO_MAX_BULLETS;
      LHR_player.ammo[LHR_AMMO_ROCKETS] = LHR_AMMO_MAX_ROCKETS;
      LHR_player.ammo[LHR_AMMO_PLASMA] = LHR_AMMO_MAX_PLASMA;
      LHR_player.weapon = LHR_WEAPON_SOLUTION;
      LHR_player.cards |= 0x07;
      LHR_game.cheatState = 0x80;
    }
  }
}

void LHR_gameStepMenu(void)
{
  uint8_t menuItems = 0;

  while (LHR_getMenuItem(menuItems) != LHR_MENU_ITEM_NONE)
    menuItems++;

  uint8_t item = LHR_getMenuItem(LHR_game.selectedMenuItem);

  if ((LHR_keyRegisters(LHR_KEY_DOWN) || LHR_keyRegisters(LHR_KEY_PREVIOUS_WEAPON)) && 
    (LHR_game.selectedMenuItem < menuItems - 1))
  {
    LHR_game.selectedMenuItem++;
    LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);
  }
  else if ((LHR_keyRegisters(LHR_KEY_UP) || LHR_keyRegisters(LHR_KEY_NEXT_WEAPON)) && (LHR_game.selectedMenuItem > 0))
  {
    LHR_game.selectedMenuItem--;
    LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);
  }
  else if (LHR_keyJustPressed(LHR_KEY_A))
  {
    switch (item)
    {
      case LHR_MENU_ITEM_PLAY:
        for (uint8_t i = 6; i < LHR_SAVE_SIZE; ++i) // reset totals in save
          LHR_game.save[i] = 0;

        if (LHR_game.selectedLevel == 0)
        {
          if (LHR_DEMO_MODE)
            LHR_setAndInitLevel(0);
          else
          {
            LHR_game.storyType = LHR_STORY_GLOBAL_INTRO;
            LHR_game.introTargetLevel = 0;
            LHR_setGameState(LHR_GAME_STATE_INTRO);
          }
        }
        else
        {
          const LHR_LevelMeta *meta =
            LHR_activeLocale->levelMeta + LHR_game.selectedLevel;

          if (!LHR_DEMO_MODE && meta->introText != 0)
          {
            LHR_game.storyType = LHR_STORY_MAP_INTRO;
            LHR_game.introTargetLevel = LHR_game.selectedLevel;

            if (meta->introMusicTrack > 0)
            {
              LHR_setMusic(LHR_MUSIC_NEXT);
              LHR_setMusic(LHR_MUSIC_TURN_ON);
            }

            LHR_setGameState(LHR_GAME_STATE_INTRO);
          }
          else
            LHR_setAndInitLevel(LHR_game.selectedLevel);
        }

        break;

      case LHR_MENU_ITEM_LOAD:
      {
        LHR_gameLoad();

        uint8_t saveBackup[LHR_SAVE_SIZE];

        for (uint8_t i = 0; i < LHR_SAVE_SIZE; ++i)
          saveBackup[i] = LHR_game.save[i];

        LHR_setAndInitLevel(LHR_game.save[0] >> 4);

        for (uint8_t i = 0; i < LHR_SAVE_SIZE; ++i)
          LHR_game.save[i] = saveBackup[i];

        LHR_player.health = LHR_game.save[2];
        LHR_player.ammo[0] = LHR_game.save[3];
        LHR_player.ammo[1] = LHR_game.save[4];
        LHR_player.ammo[2] = LHR_game.save[5];

        LHR_playerRotateWeapon(1); // this chooses weapon with ammo available
        break;
      }

      case LHR_MENU_ITEM_CONTINUE:
        LHR_setGameState(LHR_GAME_STATE_PLAYING);
        break;

      case LHR_MENU_ITEM_MAP:
        LHR_setGameState(LHR_GAME_STATE_MAP);
        break;

      case LHR_MENU_ITEM_SFX:
        LHR_LOG("sfx changed");

        LHR_game.settings ^= 0x01;

        LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);

        LHR_game.save[1] = LHR_game.settings;
        LHR_gameSave();

        break;

      case LHR_MENU_ITEM_MUSIC:
        LHR_LOG("music changed");

        LHR_game.settings ^= 0x02;

        LHR_setMusic((LHR_game.settings & 0x02) ? 
          LHR_MUSIC_TURN_ON : LHR_MUSIC_TURN_OFF);

        LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);

        LHR_game.save[1] = LHR_game.settings;
        LHR_gameSave();

        break;

      case LHR_MENU_ITEM_SHEAR:
      {
        uint8_t current = (LHR_game.settings >> 2) & 0x03;

        current++;

        if (current == 2) // option that doesn't make sense, skip
          current++;

        LHR_game.settings = 
          (LHR_game.settings & ~0x0c) | ((current & 0x03) << 2);

        LHR_game.save[1] = LHR_game.settings;
        LHR_gameSave();

        break;
      }

      case LHR_MENU_ITEM_LANGUAGE:
      {
        LHR_setLocaleByIndex(LHR_activeLocaleIndex + 1);
        LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);
        break;
      }

      case LHR_MENU_ITEM_EXIT:
        LHR_game.continues = 0;
        break;

      default:
        break;
    }
  }
  else if (item == LHR_MENU_ITEM_PLAY)
  {
    if (LHR_keyRegisters(LHR_KEY_RIGHT) && 
      (LHR_game.selectedLevel < (LHR_game.save[0] & 0x0f)))
    {
      LHR_game.selectedLevel++;
      LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);
    }
    else if (LHR_keyRegisters(LHR_KEY_LEFT) && LHR_game.selectedLevel > 0)
    {
      LHR_game.selectedLevel--;
      LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);
    }
  }
}

/**
  Performs one game step (logic, physics, menu, ...), happening LHR_MS_PER_FRAME
  after the previous step.
*/
void LHR_gameStep(void)
{
  LHR_GAME_STEP_COMMAND

  LHR_game.soundsPlayedThisFrame = 0;
  
  LHR_game.blink = (LHR_game.frame / LHR_BLINK_PERIOD_FRAMES) % 2;

  for (uint8_t i = 0; i < LHR_KEY_COUNT; ++i)
    if (!LHR_keyPressed(i))
      LHR_game.keyStates[i] = 0;
    else if (LHR_game.keyStates[i] < 255)
      LHR_game.keyStates[i]++;

  if ((LHR_currentLevel.frameStart - LHR_game.frame) %
      LHR_SPRITE_ANIMATION_FRAME_DURATION == 0)
    LHR_game.spriteAnimationFrame++;

  switch (LHR_game.state)
  {
    case LHR_GAME_STATE_PLAYING:
      LHR_handleCheats();
      LHR_gameStepPlaying();
      break;

    case LHR_GAME_STATE_MENU: 
      LHR_gameStepMenu();
      break;

    case LHR_GAME_STATE_LOSE:
    { 
      // player die animation (lose)

      LHR_updateLevel(); // let monsters and other things continue moving
      LHR_updatePlayerHeight(); // in case player is on elevator 

      int32_t t = LHR_game.stateTime;

      RCL_Unit h = LHR_floorHeightAt(LHR_player.squarePosition[0],
        LHR_player.squarePosition[1]); 

      LHR_player.camera.height = 
        RCL_max(h,h + ((LHR_LOSE_ANIMATION_DURATION - t) *
            RCL_CAMERA_COLL_HEIGHT_BELOW) / LHR_LOSE_ANIMATION_DURATION);

      LHR_player.camera.shear = 
        RCL_min(LHR_CAMERA_MAX_SHEAR_PIXELS / 4,
        (t * (LHR_CAMERA_MAX_SHEAR_PIXELS / 4)) / LHR_LOSE_ANIMATION_DURATION);

      if (t > LHR_LOSE_ANIMATION_DURATION && 
        (LHR_keyIsDown(LHR_KEY_A) || LHR_keyIsDown(LHR_KEY_B)))
      {
        for (uint8_t i = 6; i < LHR_SAVE_SIZE; ++i)
          LHR_game.save[i] = 0;

        LHR_setGameState(LHR_GAME_STATE_MENU);
      }

      break;
    }

    case LHR_GAME_STATE_WIN:
    {
      // win animation
     
      LHR_updateLevel();

      int32_t t = LHR_game.stateTime;

      if (t > LHR_WIN_ANIMATION_DURATION)
      {
        if (LHR_currentLevel.levelNumber == (LHR_NUMBER_OF_LEVELS - 1))
        {
          if (LHR_keyIsDown(LHR_KEY_A))
          {
            if (LHR_DEMO_MODE)
            {
              LHR_currentLevel.levelPointer = 0;
              LHR_currentLevel.levelNumber = 0;
              LHR_setGameState(LHR_GAME_STATE_MENU);
              LHR_setMusic(LHR_MUSIC_TURN_ON);
            }
            else
            {
              LHR_game.storyType = LHR_STORY_GLOBAL_OUTRO;
              LHR_game.outroTargetLevel = 255;
              LHR_setGameState(LHR_GAME_STATE_OUTRO);
              LHR_setMusic(LHR_MUSIC_TURN_OFF);
            }
          }
        }
        else if (LHR_keyIsDown(LHR_KEY_RIGHT) ||
            LHR_keyIsDown(LHR_KEY_LEFT) ||
            LHR_keyIsDown(LHR_KEY_STRAFE_LEFT) ||
            LHR_keyIsDown(LHR_KEY_STRAFE_RIGHT))
        {
          uint8_t nextLevel = LHR_currentLevel.levelNumber + 1;
          const LHR_LevelMeta *meta =
            LHR_activeLocale->levelMeta + LHR_currentLevel.levelNumber;

          if (!LHR_DEMO_MODE && meta->outroText != 0)
          {
            LHR_game.storyType = LHR_STORY_MAP_OUTRO;
            LHR_game.outroTargetLevel = nextLevel;

            if (meta->outroMusicTrack > 0)
            {
              LHR_setMusic(LHR_MUSIC_NEXT);
              LHR_setMusic(LHR_MUSIC_TURN_ON);
            }

            LHR_setGameState(LHR_GAME_STATE_OUTRO);
            break;
          }

          if (!LHR_DEMO_MODE)
          {
            const LHR_LevelMeta *nextMeta = LHR_activeLocale->levelMeta + nextLevel;

            if (nextMeta->introText != 0)
            {
              LHR_game.storyType = LHR_STORY_MAP_INTRO;
              LHR_game.introTargetLevel = nextLevel;

              if (nextMeta->introMusicTrack > 0)
              {
                LHR_setMusic(LHR_MUSIC_NEXT);
                LHR_setMusic(LHR_MUSIC_TURN_ON);
              }

              LHR_setGameState(LHR_GAME_STATE_INTRO);
              break;
            }
          }

          LHR_setAndInitLevel(nextLevel);
          
          LHR_player.health = LHR_game.save[2];
          LHR_player.ammo[0] = LHR_game.save[3];
          LHR_player.ammo[1] = LHR_game.save[4];
          LHR_player.ammo[2] = LHR_game.save[5];
        
          LHR_playerRotateWeapon(1);

          if (LHR_keyIsDown(LHR_KEY_RIGHT) || LHR_keyIsDown(LHR_KEY_STRAFE_RIGHT))
          {
            // save the current position
            LHR_game.save[0] = 
              (LHR_game.save[0] & 0x0f) | (LHR_currentLevel.levelNumber << 4);

            LHR_gameSave();
            LHR_game.saved = 1;
          }
        }
      }

      break;
    }

    case LHR_GAME_STATE_MAP:
      if (LHR_keyIsDown(LHR_KEY_B) || LHR_keyJustPressed(LHR_KEY_MENU))
        LHR_setGameState(LHR_GAME_STATE_MENU);

      break;

    case LHR_GAME_STATE_INTRO:
      if (LHR_keyJustPressed(LHR_KEY_A) || LHR_keyJustPressed(LHR_KEY_B))
        LHR_setAndInitLevel(LHR_game.introTargetLevel);

      break;

    case LHR_GAME_STATE_OUTRO:
      if ((LHR_game.stateTime > LHR_STORYTEXT_DURATION) &&
           (LHR_keyIsDown(LHR_KEY_A) ||
           LHR_keyIsDown(LHR_KEY_B)))
      {
        if (LHR_game.storyType == LHR_STORY_MAP_OUTRO &&
            LHR_game.outroTargetLevel != 255)
        {
          uint8_t level = LHR_game.outroTargetLevel;
          const LHR_LevelMeta *nextMeta = LHR_activeLocale->levelMeta + level;

          if (!LHR_DEMO_MODE && nextMeta->introText != 0)
          {
            LHR_game.storyType = LHR_STORY_MAP_INTRO;
            LHR_game.introTargetLevel = level;

            if (nextMeta->introMusicTrack > 0)
            {
              LHR_setMusic(LHR_MUSIC_NEXT);
              LHR_setMusic(LHR_MUSIC_TURN_ON);
            }

            LHR_setGameState(LHR_GAME_STATE_INTRO);
          }
          else
            LHR_setAndInitLevel(level);
        }
        else
        {
          LHR_currentLevel.levelPointer = 0;
          LHR_currentLevel.levelNumber = 0;
          LHR_setGameState(LHR_GAME_STATE_MENU);
          LHR_playGameSound(3,LHR_MENU_CLICK_VOLUME);
          LHR_setMusic(LHR_MUSIC_TURN_ON);
        }
      }

      break;

    case LHR_GAME_STATE_LEVEL_START:
    {
      LHR_updateLevel();
      LHR_updatePlayerHeight(); // in case player is on elevator

      int16_t x = 0, y = 0;
      
      LHR_getMouseOffset(&x,&y); // this keeps centering the mouse

      if (LHR_game.stateTime >= LHR_LEVEL_START_DURATION)
        LHR_setGameState(LHR_GAME_STATE_PLAYING);

      break;
    }

    default:
      break;
  }

  LHR_game.stateTime += LHR_MS_PER_FRAME;
}

void LHR_fillRectangle(
  uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color)
{
  if ((x + width > LHR_GAME_RESOLUTION_X) ||
      (y + height > LHR_GAME_RESOLUTION_Y))
    return;

  for (uint16_t j = y; j < y + height; ++j)
    for (uint16_t i = x; i < x + width; ++i)
      LHR_setGamePixel(i,j,color);
}

static inline void LHR_clearScreen(uint8_t color)
{
  LHR_fillRectangle(0,0,LHR_GAME_RESOLUTION_X,
    LHR_GAME_RESOLUTION_Y,color);
}

/**
  Draws fullscreen map of the current level.
*/
void LHR_drawMap(void)
{
  LHR_clearScreen(0);
   
  uint16_t maxJ =
    (LHR_MAP_PIXEL_SIZE * LHR_MAP_SIZE) < LHR_GAME_RESOLUTION_Y ?
    (LHR_MAP_SIZE) : (LHR_GAME_RESOLUTION_Y / LHR_MAP_PIXEL_SIZE);

  uint16_t maxI =
    (LHR_MAP_PIXEL_SIZE * LHR_MAP_SIZE) < LHR_GAME_RESOLUTION_X ?
    (LHR_MAP_SIZE) : (LHR_GAME_RESOLUTION_X / LHR_MAP_PIXEL_SIZE);

  uint16_t topLeftX =
    (LHR_GAME_RESOLUTION_X - (maxI * LHR_MAP_PIXEL_SIZE)) / 2;

  uint16_t topLeftY =
    (LHR_GAME_RESOLUTION_Y - (maxJ * LHR_MAP_PIXEL_SIZE)) / 2;

  uint16_t x;
  uint16_t y = topLeftY;

  uint8_t playerColor = 
    LHR_game.blink ? LHR_MAP_PLAYER_COLOR1 : LHR_MAP_PLAYER_COLOR2;

  for (int16_t j = 0; j < maxJ; ++j)
  {
    x = topLeftX;

    for (int16_t i = maxI - 1; i >= 0; --i)
    {
      uint8_t color = 0; // init with non-revealed color

      if (LHR_currentLevel.mapRevealMask & LHR_getMapRevealBit(i,j)) 
      {
        uint8_t properties;

        LHR_TileDefinition tile =
          LHR_getMapTile(LHR_currentLevel.levelPointer,i,j,&properties);

        color = playerColor; // start with player color

        if (i != LHR_player.squarePosition[0] ||
            j != LHR_player.squarePosition[1])
        {
          if (properties == LHR_TILE_PROPERTY_ELEVATOR)
            color = LHR_MAP_ELEVATOR_COLOR;
          else if (properties == LHR_TILE_PROPERTY_SQUEEZER)
            color = LHR_MAP_SQUEEZER_COLOR;
          else if (properties == LHR_TILE_PROPERTY_DOOR)
            color = LHR_MAP_DOOR_COLOR;
          else
          {
            color = 0;

            uint8_t c = LHR_TILE_CEILING_HEIGHT(tile) / 4;

            if (c != 0)
              color = (LHR_TILE_FLOOR_HEIGHT(tile) % 8 + 3) * 8 + c - 1;
          }
        }
      }

      for (int_fast16_t k = 0; k < LHR_MAP_PIXEL_SIZE; ++k)
        for (int_fast16_t l = 0; l < LHR_MAP_PIXEL_SIZE; ++l)
          LHR_setGamePixel(x + l, y + k,color);

      x += LHR_MAP_PIXEL_SIZE;
    }

    y += LHR_MAP_PIXEL_SIZE;
  } 
}

/**
  Draws fullscreen story text (intro/outro).
*/
void LHR_drawStoryText(void)
{
  const char *text = LHR_outroText;
  uint16_t textColor = 23;
  uint8_t clearColor = 9;
  uint8_t sprite = 18;

  const LHR_MapImage *bgImage = 0;

  if (LHR_game.state == LHR_GAME_STATE_INTRO)
  {
    if (LHR_game.storyType == LHR_STORY_MAP_INTRO)
    {
      uint8_t idx = LHR_game.introTargetLevel;

      if (idx >= LHR_NUMBER_OF_LEVELS)
        idx = 0;

      text = LHR_activeLocale->levelMeta[idx].introText;
      bgImage = LHR_activeLocale->levelMeta[idx].introImage;
    }
    else
      text = LHR_introText;

    textColor = 7;
    clearColor = 0;
    sprite = LHR_game.blink * 2;
  }
  else if (LHR_game.storyType == LHR_STORY_MAP_OUTRO)
  {
    if (LHR_currentLevel.levelNumber < LHR_NUMBER_OF_LEVELS) {
      text = LHR_activeLocale->levelMeta[LHR_currentLevel.levelNumber].outroText;
      bgImage = LHR_activeLocale->levelMeta[LHR_currentLevel.levelNumber].outroImage;
    }
  }

  if (text == 0)
    text = "";

  if (bgImage != 0) {
    uint16_t frame = LHR_getMapImageFrame(bgImage, LHR_game.stateTime);
    uint32_t frameOffset = frame * bgImage->width * bgImage->height;
    
    for (uint16_t y = 0; y < LHR_GAME_RESOLUTION_Y; ++y) {
      uint32_t srcY = (y * bgImage->height) / LHR_GAME_RESOLUTION_Y;
      for (uint16_t x = 0; x < LHR_GAME_RESOLUTION_X; ++x) {
        uint32_t srcX = (x * bgImage->width) / LHR_GAME_RESOLUTION_X;
        uint8_t color = bgImage->data[frameOffset + srcY * bgImage->width + srcX];
        LHR_setPixel(x, y, color);
      }
    }
  } else {
    LHR_clearScreen(clearColor);
  }

  if (LHR_GAME_RESOLUTION_Y > 50) 
    LHR_blitImage(LHR_monsterSprites + sprite * LHR_TEXTURE_STORE_SIZE,
        (LHR_GAME_RESOLUTION_X - LHR_TEXTURE_SIZE * LHR_FONT_SIZE_SMALL) / 2,
        LHR_GAME_RESOLUTION_Y - (LHR_TEXTURE_SIZE + 3) * LHR_FONT_SIZE_SMALL,
        LHR_FONT_SIZE_SMALL);  

  uint16_t textLen = 0;

  while (text[textLen] != 0)
    textLen++;

  uint16_t drawLen = RCL_min(
    textLen,(LHR_game.stateTime * textLen) / LHR_STORYTEXT_DURATION + 1);

#define CHAR_SIZE (LHR_FONT_SIZE_SMALL * (LHR_FONT_CHARACTER_SIZE + 1))
#define LINE_LENGTH (LHR_GAME_RESOLUTION_X / CHAR_SIZE)
#define MAX_LENGTH (((LHR_GAME_RESOLUTION_Y / CHAR_SIZE) / 2) * LINE_LENGTH  )

  uint16_t drawShift = (drawLen < MAX_LENGTH) ? 0 :
    (((drawLen - MAX_LENGTH) / LINE_LENGTH) * LINE_LENGTH);

#undef CHAR_SIZE
#undef LINE_LENGTH
#undef MAX_LENGTH

  text += drawShift;
  drawLen -= drawShift;

  LHR_drawText(text,LHR_HUD_MARGIN,LHR_HUD_MARGIN,LHR_FONT_SIZE_SMALL,textColor,
    drawLen,LHR_GAME_RESOLUTION_X - LHR_HUD_MARGIN);
}

/**
  Draws a number as text on screen, returns the number of characters drawn.
*/
uint8_t LHR_drawNumber(
  int16_t number,
  uint16_t x,
  uint16_t y,
  uint8_t size,
  uint8_t color)
{
  char text[7];

  text[6] = 0; // terminate the string

  int8_t positive = 1;

  if (number < 0)
  {
    positive = 0;
    number *= -1;
  }

  int8_t position = 5;

  while (1)
  {
    text[position] = '0' + number % 10;
    number /= 10;

    position--;

    if (number == 0 || position == 0)
      break;
  }

  if (!positive)
  {
    text[position] = '-';
    position--;
  }

  LHR_drawText(text + position + 1,x,y,size,color,0,0);

  return 5 - position;
}

/**
  Draws a screen border that indicates something is happening, e.g. being hurt
  or taking an item.
*/
void LHR_drawIndicationBorder(uint16_t width, uint8_t color)
{
  for (int_fast16_t j = 0; j < width; ++j)
  {
    uint16_t j2 = LHR_GAME_RESOLUTION_Y - 1 - j;

    for (int_fast16_t i = 0; i < LHR_GAME_RESOLUTION_X; ++i)
    {
      if ((i & 0x01) == (j & 0x01))
      {
        LHR_setGamePixel(i,j,color);
        LHR_setGamePixel(i,j2,color);
      }
    }
  }

  for (int_fast16_t i = 0; i < width; ++i)
  {
    uint16_t i2 = LHR_GAME_RESOLUTION_X - 1 - i;

    for (int_fast16_t j = width; j < LHR_GAME_RESOLUTION_Y - width; ++j)
    {
      if ((i & 0x01) == (j & 0x01))
      {
        LHR_setGamePixel(i,j,color);
        LHR_setGamePixel(i2,j,color);
      }
    }
  }
}

/**
  Draws the player weapon, includes handling the shoot animation.
*/
void LHR_drawWeapon(int16_t bobOffset)
{
  uint32_t animationLength =
    RCL_max(LHR_MIN_WEAPON_COOLDOWN_FRAMES,
      LHR_GET_WEAPON_FIRE_COOLDOWN_FRAMES(LHR_player.weapon));

  uint32_t shotAnimationFrame =
    animationLength - LHR_player.weaponCooldownFrames;

  bobOffset -= LHR_HUD_BAR_HEIGHT;
     
  uint8_t fireType = LHR_GET_WEAPON_FIRE_TYPE(LHR_player.weapon);

  if (shotAnimationFrame < animationLength)
  {
    if (fireType == LHR_WEAPON_FIRE_TYPE_MELEE)
    {
      bobOffset = shotAnimationFrame < animationLength / 2 ? 0 :
        2 * LHR_WEAPONBOB_OFFSET_PIXELS;
    }
    else
    {
      bobOffset +=  
          ((animationLength - shotAnimationFrame) * LHR_WEAPON_IMAGE_SCALE * 20)
          / animationLength;
   
      if (
        ((fireType == LHR_WEAPON_FIRE_TYPE_FIREBALL) ||
         (fireType == LHR_WEAPON_FIRE_TYPE_BULLET)) &&
        shotAnimationFrame < animationLength / 2)
        LHR_blitImage(LHR_effectSprites,
          LHR_WEAPON_IMAGE_POSITION_X,
          LHR_WEAPON_IMAGE_POSITION_Y -
            (LHR_TEXTURE_SIZE / 3) * LHR_WEAPON_IMAGE_SCALE + bobOffset,
          LHR_WEAPON_IMAGE_SCALE);
    }
  }

  LHR_blitImage(LHR_weaponImages + LHR_player.weapon * LHR_TEXTURE_STORE_SIZE,
  LHR_WEAPON_IMAGE_POSITION_X,
  LHR_WEAPON_IMAGE_POSITION_Y + bobOffset - 1,
  LHR_WEAPON_IMAGE_SCALE);
}

uint16_t LHR_textLen(const char *text)
{
  uint16_t result = 0;

  while (text[result] != 0)
    result++;

  return result;
}

static inline uint16_t LHR_characterSize(uint8_t textSize)
{
  return (LHR_FONT_CHARACTER_SIZE + 1) * textSize;
}

static inline uint16_t
  LHR_textHorizontalSize(const char *text, uint8_t textSize)
{
  return (LHR_textLen(text) * LHR_characterSize(textSize));
}

void LHR_drawMenu(void)
{
  #define BACKGROUND_SCALE (LHR_GAME_RESOLUTION_X / (4 * LHR_TEXTURE_SIZE))

  #if BACKGROUND_SCALE == 0
    #undef BACKGROUND_SCALE
    #define BACKGROUND_SCALE 1
  #endif

  #define SCROLL_PIXELS_PER_FRAME ((64 * LHR_GAME_RESOLUTION_X) / (8 * LHR_FPS))

  #if SCROLL_PIXELS_PER_FRAME == 0
    #undef SCROLL_PIXELS_PER_FRAME
    #define SCROLL_PIXELS_PER_FRAME 1
  #endif

  #define SELECTION_START_X ((LHR_GAME_RESOLUTION_X - 12 * LHR_FONT_SIZE_MEDIUM\
    * (LHR_FONT_CHARACTER_SIZE + 1)) / 2)

  uint16_t scroll = (LHR_game.frame * SCROLL_PIXELS_PER_FRAME) / 64;

  for (uint16_t y = 0; y < LHR_GAME_RESOLUTION_Y; ++y)
    for (uint16_t x = 0; x < LHR_GAME_RESOLUTION_X; ++x)
      LHR_setGamePixel(x,y,
        (y >= (LHR_TEXTURE_SIZE * BACKGROUND_SCALE)) ? 0 :
        LHR_getTexel(LHR_backgroundImages,((x + scroll) / BACKGROUND_SCALE)
          % LHR_TEXTURE_SIZE,y / BACKGROUND_SCALE));

  uint16_t y = LHR_characterSize(LHR_FONT_SIZE_MEDIUM);

  LHR_blitImage(LHR_logoImage,LHR_GAME_RESOLUTION_X / 2 - 
    (LHR_TEXTURE_SIZE / 2) * LHR_FONT_SIZE_SMALL,y,LHR_FONT_SIZE_SMALL);

#if LHR_GAME_RESOLUTION_Y > 50
  y += 32 * LHR_FONT_SIZE_MEDIUM + LHR_characterSize(LHR_FONT_SIZE_MEDIUM);
#else
  y = 2;
#endif

  uint8_t i = 0;

  while (1) // draw menu items
  {
    uint8_t item = LHR_getMenuItem(i);

    if (item == LHR_MENU_ITEM_NONE)
      break;

#if (LHR_GAME_RESOLUTION_Y < 70) || LHR_FORCE_SINGLE_ITEM_MENU
    // with low resolution only display the selected item

    if (i != LHR_game.selectedMenuItem)
    {
      i++;
      continue;
    }
#endif

    const char *text = LHR_menuItemTexts[item];
    uint8_t textLen = LHR_textLen(text);

    uint16_t drawX = (LHR_GAME_RESOLUTION_X -
      LHR_textHorizontalSize(text,LHR_FONT_SIZE_MEDIUM)) / 2;

    uint8_t textColor = 7;

    if (i != LHR_game.selectedMenuItem)
      textColor = 23;
    else
      LHR_fillRectangle( // menu item highlight
        SELECTION_START_X,
        y - LHR_FONT_SIZE_MEDIUM,
        LHR_GAME_RESOLUTION_X - SELECTION_START_X * 2,
        LHR_characterSize(LHR_FONT_SIZE_MEDIUM),2);
  
    LHR_drawText(text,drawX,y,LHR_FONT_SIZE_MEDIUM,textColor,0,0);

        if ((item == LHR_MENU_ITEM_PLAY || item == LHR_MENU_ITEM_SFX || item == LHR_MENU_ITEM_MUSIC
          || item == LHR_MENU_ITEM_SHEAR || item == LHR_MENU_ITEM_LANGUAGE) &&
        ((i != LHR_game.selectedMenuItem) || LHR_game.blink))
    {
      uint32_t x =
        drawX + LHR_characterSize(LHR_FONT_SIZE_MEDIUM) * (textLen + 1);

      uint8_t c = 93;

      if (item == LHR_MENU_ITEM_PLAY)
        LHR_drawNumber(LHR_game.selectedLevel + 1,x,y,LHR_FONT_SIZE_MEDIUM,c);
      else if (item == LHR_MENU_ITEM_SHEAR)
      {
        uint8_t n = (LHR_game.settings >> 2) & 0x03;

        LHR_drawNumber(n == 3 ? 2 : n,x,y,LHR_FONT_SIZE_MEDIUM,c);
      }
      else if (item == LHR_MENU_ITEM_SFX)
      {
        LHR_drawText((LHR_game.settings & 0x01) ? LHR_activeLocale->textOn : LHR_activeLocale->textOff,x,y,LHR_FONT_SIZE_MEDIUM,c,0,0);
      }
      else if (item == LHR_MENU_ITEM_MUSIC)
      {
        LHR_drawText((LHR_game.settings & 0x02) ? LHR_activeLocale->textOn : LHR_activeLocale->textOff,x,y,LHR_FONT_SIZE_MEDIUM,c,0,0);
      }
      else
        LHR_drawText(LHR_activeLocale->localeId,x,y,LHR_FONT_SIZE_MEDIUM,c,0,0);
    }

    y += LHR_characterSize(LHR_FONT_SIZE_MEDIUM) + LHR_FONT_SIZE_MEDIUM;
    i++;
  }
  
  LHR_drawText(LHR_VERSION_STRING " GPL",LHR_HUD_MARGIN,LHR_GAME_RESOLUTION_Y -
    LHR_HUD_MARGIN - LHR_FONT_SIZE_SMALL * LHR_FONT_CHARACTER_SIZE,
    LHR_FONT_SIZE_SMALL,4,0,0);

  #if LHR_OS_IS_MALWARE
    if (LHR_game.blink)
      LHR_drawText(LHR_MALWARE_WARNING,LHR_HUD_MARGIN,LHR_HUD_MARGIN,
        LHR_FONT_SIZE_MEDIUM,95,0,0);
  #endif

  #undef MAX_ITEMS
  #undef BACKGROUND_SCALE
  #undef SCROLL_PIXELS_PER_FRAME
}

void LHR_drawWinOverlay(void)
{
  uint32_t t = RCL_min(LHR_WIN_ANIMATION_DURATION,LHR_game.stateTime);

  uint32_t t2 = RCL_min(t,LHR_WIN_ANIMATION_DURATION / 4);

  #define STRIP_HEIGHT (LHR_GAME_RESOLUTION_Y / 2)
  #define INNER_STRIP_HEIGHT ((STRIP_HEIGHT * 3) / 4)
  #define STRIP_START ((LHR_GAME_RESOLUTION_Y - STRIP_HEIGHT) / 2)

  RCL_Unit l = (t2 * STRIP_HEIGHT * 4) / LHR_WIN_ANIMATION_DURATION;

  for (uint16_t y = STRIP_START; y < STRIP_START + l; ++y)
    for (uint16_t x = 0; x < LHR_GAME_RESOLUTION_X; ++x)
      LHR_setGamePixel(x,y, 
        RCL_abs(y - (LHR_GAME_RESOLUTION_Y / 2)) <= (INNER_STRIP_HEIGHT / 2) ?
          0 : 172);

  const char *textLine = LHR_TEXT_LEVEL_COMPLETE;

  uint16_t y = LHR_GAME_RESOLUTION_Y / 2 - 
    ((STRIP_HEIGHT + INNER_STRIP_HEIGHT) / 2) / 2;

  uint16_t x = (LHR_GAME_RESOLUTION_X - 
    LHR_textHorizontalSize(textLine,LHR_FONT_SIZE_BIG)) / 2;

  LHR_drawText(textLine,x,y,LHR_FONT_SIZE_BIG,7 + LHR_game.blink * 95,255,0);

  uint32_t timeTotal = LHR_SAVE_TOTAL_TIME;

  // don't show totals in level 1:
  uint8_t blink = (LHR_game.blink) && (LHR_currentLevel.levelNumber != 0)
    && (timeTotal != 0);

  if (t >= (LHR_WIN_ANIMATION_DURATION / 2))
  {
    y += (LHR_FONT_SIZE_BIG + LHR_FONT_SIZE_MEDIUM) * LHR_FONT_CHARACTER_SIZE;
    x = LHR_HUD_MARGIN;

    #define CHAR_SIZE (LHR_FONT_SIZE_SMALL * LHR_FONT_CHARACTER_SIZE)

    uint32_t time = blink ? timeTotal : LHR_currentLevel.completionTime10sOfS;

    x += LHR_drawNumber(time / 10,x,y,LHR_FONT_SIZE_SMALL,7) *
      CHAR_SIZE + LHR_FONT_SIZE_SMALL;

    char timeRest[5] = ".X s";

    timeRest[1] = '0' + (time % 10);
    
    LHR_drawText(timeRest,x,y,LHR_FONT_SIZE_SMALL,7,4,0);

    x = LHR_HUD_MARGIN;
    y += (LHR_FONT_SIZE_BIG + LHR_FONT_SIZE_MEDIUM) * LHR_FONT_CHARACTER_SIZE;

    if (blink)
    {
      x += (LHR_drawNumber(LHR_game.save[10] + LHR_game.save[11] * 256,x,y,
        LHR_FONT_SIZE_SMALL,7) + 1) * CHAR_SIZE;
    }
    else
    {
      x += LHR_drawNumber(LHR_currentLevel.monstersDead,x,y,
        LHR_FONT_SIZE_SMALL,7) * CHAR_SIZE;

      LHR_drawText("/",x,y,LHR_FONT_SIZE_SMALL,7,1,0);

      x += CHAR_SIZE;

      x += (LHR_drawNumber(LHR_currentLevel.monsterRecordCount,x,y,
        LHR_FONT_SIZE_SMALL,7) + 1) * CHAR_SIZE;
    }
    
    LHR_drawText(LHR_TEXT_KILLS,x,y,LHR_FONT_SIZE_SMALL,7,255,0);

    if ((t >= (LHR_WIN_ANIMATION_DURATION - 1)) && 
      (LHR_currentLevel.levelNumber != (LHR_NUMBER_OF_LEVELS - 1)) &&
      LHR_game.blink)
    {
      y += (LHR_FONT_SIZE_BIG + LHR_FONT_SIZE_MEDIUM) * LHR_FONT_CHARACTER_SIZE;

      LHR_drawText(LHR_TEXT_SAVE_PROMPT,
        (LHR_GAME_RESOLUTION_X - LHR_textHorizontalSize(LHR_TEXT_SAVE_PROMPT,
          LHR_FONT_SIZE_MEDIUM)) / 2,y,LHR_FONT_SIZE_MEDIUM,7,255,0);
    }

    #undef CHAR_SIZE
  }

  #undef STRIP_HEIGHT
  #undef STRIP_START
  #undef INNER_STRIP_HEIGHT
}

void LHR_draw(void)
{
#if LHR_BACKGROUND_BLUR != 0
  LHR_backgroundBlurIndex = 0;
#endif

  if (LHR_game.state == LHR_GAME_STATE_MENU)
  {
    LHR_drawMenu();
    return;
  }

  if (LHR_game.state == LHR_GAME_STATE_INTRO ||
      LHR_game.state == LHR_GAME_STATE_OUTRO)
  {
    LHR_drawStoryText();
    return;
  }

  if (LHR_keyIsDown(LHR_KEY_MAP) || (LHR_game.state == LHR_GAME_STATE_MAP))
  {
    LHR_drawMap();
  } 
  else
  { 
    for (int_fast16_t i = 0; i < LHR_Z_BUFFER_SIZE; ++i)
      LHR_game.zBuffer[i] = 255;

    int16_t weaponBobOffset = 0;

#if LHR_HEADBOB_ENABLED
    RCL_Unit headBobOffset = 0;

#if LHR_HEADBOB_SHEAR != 0
    int16_t headBobShearOffset = 0;
#endif

    if (LHR_game.state != LHR_GAME_STATE_LOSE)
    {
      RCL_Unit bobSin = RCL_sin(LHR_player.headBobFrame);

      headBobOffset = (bobSin * LHR_HEADBOB_OFFSET) / RCL_UNITS_PER_SQUARE;

#if LHR_HEADBOB_SHEAR != 0
      headBobShearOffset = (bobSin * LHR_HEADBOB_SHEAR) / RCL_UNITS_PER_SQUARE;
      LHR_player.camera.shear += headBobShearOffset;
#endif

      weaponBobOffset =
        (bobSin * LHR_WEAPONBOB_OFFSET_PIXELS) / (RCL_UNITS_PER_SQUARE) + 
        LHR_WEAPONBOB_OFFSET_PIXELS;
    }
    else
    {
      // player die animation

      weaponBobOffset =
        (LHR_WEAPON_IMAGE_SCALE * LHR_TEXTURE_SIZE * LHR_game.stateTime) /
        LHR_LOSE_ANIMATION_DURATION;
    }
      
    // add head bob just for the rendering (we'll will subtract it back later)

    LHR_player.camera.height += headBobOffset;
#endif // headbob enabled?

    RCL_renderComplex(
      LHR_player.camera,
      LHR_floorHeightAt,
      LHR_ceilingHeightAt,
      LHR_texturesAt,
      LHR_game.rayConstraints);
 
    // draw sprites:

    // monster sprites:
    for (int_fast16_t i = 0; i < LHR_currentLevel.monsterRecordCount; ++i)
    {
      LHR_MonsterRecord m = LHR_currentLevel.monsterRecords[i];
      uint8_t state = LHR_MR_STATE(m);

      if (state != LHR_MONSTER_STATE_INACTIVE)
      {
        RCL_Vector2D worldPosition;

        worldPosition.x = LHR_MONSTER_COORD_TO_RCL_UNITS(m.coords[0]);
        worldPosition.y = LHR_MONSTER_COORD_TO_RCL_UNITS(m.coords[1]);

        uint8_t spriteSize = LHR_GET_MONSTER_SPRITE_SIZE(
          LHR_MONSTER_TYPE_TO_INDEX(LHR_MR_TYPE(m)));

        RCL_Unit worldHeight = 
          LHR_floorHeightAt(
            LHR_MONSTER_COORD_TO_SQUARES(m.coords[0]),
            LHR_MONSTER_COORD_TO_SQUARES(m.coords[1]))
            + LHR_SPRITE_SIZE_TO_HEIGHT_ABOVE_GROUND(spriteSize);

        RCL_PixelInfo p =
          RCL_mapToScreen(worldPosition,worldHeight,LHR_player.camera);

        if (p.depth > 0 &&
          LHR_spriteIsVisible(worldPosition,worldHeight))
        {
          const uint8_t *s =
            LHR_getMonsterSprite(
              LHR_MR_TYPE(m),
              state,
              LHR_game.spriteAnimationFrame & 0x01);

          LHR_drawScaledSprite(s,
            p.position.x * LHR_RAYCASTING_SUBSAMPLE,p.position.y,
            RCL_perspectiveScaleVertical(
            LHR_SPRITE_SIZE_PIXELS(spriteSize),
            p.depth),
            LHR_fogValueDiminish(p.depth),
            p.depth);
        }
      }
    }

    // item sprites:
    for (int_fast16_t i = 0; i < LHR_currentLevel.itemRecordCount; ++i)
      if (LHR_currentLevel.itemRecords[i] & LHR_ITEM_RECORD_ACTIVE_MASK)
      {
        RCL_Vector2D worldPosition;

        LHR_LevelElement e = 
          LHR_currentLevel.levelPointer->elements[
            LHR_currentLevel.itemRecords[i] & ~LHR_ITEM_RECORD_ACTIVE_MASK];

        worldPosition.x =
          LHR_ELEMENT_COORD_TO_RCL_UNITS(e.coords[0]);

        worldPosition.y =
          LHR_ELEMENT_COORD_TO_RCL_UNITS(e.coords[1]);

        const uint8_t *sprite;
        uint8_t spriteSize;

        LHR_getItemSprite(e.type,&sprite,&spriteSize);

        if (sprite != 0)
        {
          RCL_Unit worldHeight = LHR_floorHeightAt(e.coords[0],e.coords[1])
            + LHR_SPRITE_SIZE_TO_HEIGHT_ABOVE_GROUND(spriteSize);

          RCL_PixelInfo p =
            RCL_mapToScreen(worldPosition,worldHeight,LHR_player.camera);

          if (p.depth > 0 &&
            LHR_spriteIsVisible(worldPosition,worldHeight))
            LHR_drawScaledSprite(sprite,p.position.x * LHR_RAYCASTING_SUBSAMPLE,
              p.position.y,
              RCL_perspectiveScaleVertical(LHR_SPRITE_SIZE_PIXELS(spriteSize),
              p.depth),
              LHR_fogValueDiminish(p.depth),p.depth);
        }
      }

    // projectile sprites:
    for (uint8_t i = 0; i < LHR_currentLevel.projectileRecordCount; ++i)
    {
      LHR_ProjectileRecord *proj = &(LHR_currentLevel.projectileRecords[i]);

      if (proj->type == LHR_PROJECTILE_BULLET)
        continue; // bullets aren't drawn

      RCL_Vector2D worldPosition;

      worldPosition.x = proj->position[0];
      worldPosition.y = proj->position[1];

      RCL_PixelInfo p =
        RCL_mapToScreen(worldPosition,proj->position[2],LHR_player.camera);
       
      const uint8_t *s =
        LHR_effectSprites + proj->type * LHR_TEXTURE_STORE_SIZE;

      int16_t spriteSize = LHR_SPRITE_SIZE_PIXELS(0);

      if (proj->type == LHR_PROJECTILE_EXPLOSION ||
          proj->type == LHR_PROJECTILE_DUST)
      {
        int16_t doubleFramesToLive =
          RCL_nonZero(LHR_GET_PROJECTILE_FRAMES_TO_LIVE(proj->type) / 2);

        // grow the explosion/dust sprite as an animation
        spriteSize = (
            LHR_SPRITE_SIZE_PIXELS(2) *
            RCL_sin(          
              ((doubleFramesToLive -
               proj->doubleFramesToLive) * RCL_UNITS_PER_SQUARE / 4)
               / doubleFramesToLive) 
          ) / RCL_UNITS_PER_SQUARE;
      }

      if (p.depth > 0 && 
        LHR_spriteIsVisible(worldPosition,proj->position[2]))
        LHR_drawScaledSprite(s,
            p.position.x * LHR_RAYCASTING_SUBSAMPLE,p.position.y,
            RCL_perspectiveScaleVertical(spriteSize,p.depth),
            LHR_fogValueDiminish(p.depth),
            p.depth);  
    }

#if LHR_HEADBOB_ENABLED
    // after rendering sprites subtract back the head bob offset
    LHR_player.camera.height -= headBobOffset;

#if LHR_HEADBOB_SHEAR != 0
    LHR_player.camera.shear -= headBobShearOffset;
#endif

#endif // head bob enabled?

#if LHR_PREVIEW_MODE == 0
    LHR_drawWeapon(weaponBobOffset);
#endif

    // draw HUD:

    // bar

    uint8_t color = 61;
    uint8_t color2 = 48;

    if (LHR_game.cheatState & 0x80)
    {
      color = 170;
      color2 = 0;
    }

    for (uint16_t j = LHR_GAME_RESOLUTION_Y - LHR_HUD_BAR_HEIGHT;
      j < LHR_GAME_RESOLUTION_Y; ++j)
    {
      for (uint16_t i = 0; i < LHR_GAME_RESOLUTION_X; ++i)
        LHR_setGamePixel(i,j,color);

      color = color2;
    }

    #define TEXT_Y (LHR_GAME_RESOLUTION_Y - LHR_HUD_MARGIN - \
      LHR_FONT_CHARACTER_SIZE * LHR_FONT_SIZE_MEDIUM)

    LHR_drawNumber( // health
      LHR_player.health,
      LHR_HUD_MARGIN,
      TEXT_Y,
      LHR_FONT_SIZE_MEDIUM,
      LHR_player.health > LHR_PLAYER_HEALTH_WARNING_LEVEL ? 6 : 175);

    LHR_drawNumber( // ammo
      LHR_player.weapon != LHR_WEAPON_KNIFE ?
        LHR_player.ammo[LHR_weaponAmmo(LHR_player.weapon)] : 0,
      LHR_GAME_RESOLUTION_X - LHR_HUD_MARGIN -
        (LHR_FONT_CHARACTER_SIZE + 1) * LHR_FONT_SIZE_MEDIUM * 3,
      TEXT_Y,
      LHR_FONT_SIZE_MEDIUM,
      6); 

    for (uint8_t i = 0; i < 3; ++i) // access cards
      if ( 
        ((LHR_player.cards >> i) | ((LHR_player.cards >> (i + 3))
        & LHR_game.blink)) & 0x01)
        LHR_fillRectangle(
          LHR_HUD_MARGIN + (LHR_FONT_CHARACTER_SIZE + 1) *
            LHR_FONT_SIZE_MEDIUM * (5 + i),
          TEXT_Y,
          LHR_FONT_SIZE_MEDIUM * LHR_FONT_CHARACTER_SIZE,
          LHR_FONT_SIZE_MEDIUM * LHR_FONT_CHARACTER_SIZE,
          i == 0 ? 93 : (i == 1 ? 124 : 60));

    #undef TEXT_Y

    // border indicator

    if ((LHR_game.frame - LHR_player.lastHurtFrame
        <= LHR_HUD_BORDER_INDICATOR_DURATION_FRAMES) ||
        (LHR_game.state == LHR_GAME_STATE_LOSE))
      LHR_drawIndicationBorder(LHR_HUD_BORDER_INDICATOR_WIDTH_PIXELS,
      LHR_HUD_HURT_INDICATION_COLOR);
    else if (LHR_game.frame - LHR_player.lastItemTakenFrame
        <= LHR_HUD_BORDER_INDICATOR_DURATION_FRAMES)
      LHR_drawIndicationBorder(LHR_HUD_BORDER_INDICATOR_WIDTH_PIXELS,
      LHR_HUD_ITEM_TAKEN_INDICATION_COLOR);

    if (LHR_game.state == LHR_GAME_STATE_WIN)
      LHR_drawWinOverlay();
    else if (LHR_game.state == LHR_GAME_STATE_LEVEL_START)
      LHR_drawLevelStartOverlay();
  }
}

uint8_t LHR_mainLoopBody(void)
{
  /* Standard deterministic game loop, independed of actual achieved FPS.
     Each game logic (physics) frame is performed with the LHR_MS_PER_FRAME
     delta time. */

  if (LHR_game.state != LHR_GAME_STATE_INIT)
  {
    uint32_t timeNow = LHR_getTimeMs();

#if LHR_TIME_MULTIPLIER != 1024
    timeNow = (timeNow * LHR_TIME_MULTIPLIER) / 1024;
#endif

    int32_t timeSinceLastFrame = timeNow - LHR_game.frameTime;

    if (timeSinceLastFrame >= LHR_MS_PER_FRAME)
    {
      uint8_t steps = 0;

      uint8_t wasFirstFrame = LHR_game.frame == 0;

      while (timeSinceLastFrame >= LHR_MS_PER_FRAME)
      {
        uint8_t previousWeapon = LHR_player.weapon;

        LHR_game.frameTime += LHR_MS_PER_FRAME;

        LHR_gameStep();

        if (LHR_player.weapon != previousWeapon)
          LHR_processEvent(LHR_EVENT_PLAYER_CHANGES_WEAPON,LHR_player.weapon);

        timeSinceLastFrame -= LHR_MS_PER_FRAME;

        LHR_game.frame++;
        steps++;
      }

      if ((steps > 1) && (LHR_game.antiSpam == 0) && (!wasFirstFrame))
      {
        LHR_LOG("failed to reach target FPS! consider setting a lower value")
        LHR_game.antiSpam = 30;
      }

      if (LHR_game.antiSpam > 0)
        LHR_game.antiSpam--;

      // render only once
      LHR_draw();

      if (LHR_game.frame % 16 == 0)
        LHR_CPU_LOAD(((LHR_getTimeMs() - timeNow) * 100) / LHR_MS_PER_FRAME);
    }
    else
    {
      // wait, relieve CPU
      LHR_sleepMs(RCL_max(1,
        (3 * (LHR_game.frameTime + LHR_MS_PER_FRAME - timeNow)) / 4));
    }
  }
  else if (!LHR_keyPressed(LHR_KEY_A) && !LHR_keyPressed(LHR_KEY_B))
  {
    /* At the beginning we have to wait for the release of the keys in order not
    to immediatelly confirm a menu item. */
    LHR_setGameState(LHR_GAME_STATE_MENU);
  }

  return LHR_game.continues;
}

#undef LHR_SAVE_TOTAL_TIME

#endif // guard
