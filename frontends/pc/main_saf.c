/**
  @file main_saf.c

  This is a SAF (smallabstractfish) implementation of the game front end.


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#define SAF_PROGRAM_NAME "L'Homme Révolté"
#define SAF_PLATFORM_SDL2

#include "saf.h"

#if SAF_PLATFORM_HARWARD
  #define LHR_AVR 1
#endif

#define LHR_FPS 25
#define LHR_DIMINISH_SPRITES 1
#define LHR_RAYCASTING_MAX_HITS 7
#define LHR_CAN_EXIT 1

#define LHR_SCREEN_RESOLUTION_X SAF_SCREEN_WIDTH
#define LHR_SCREEN_RESOLUTION_Y SAF_SCREEN_HEIGHT

#define LHR_DITHERED_SHADOW 1

#define LHR_PC 1
#include "game.h"

uint8_t palette[256];

void LHR_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  SAF_drawPixel(x,y,palette[colorIndex]);
}

uint32_t LHR_getTimeMs(void)
{
  return SAF_time();
}

void LHR_sleepMs(uint16_t timeMs)
{
}

int8_t LHR_keyPressed(uint8_t key)
{
  switch (key)
  {
    case LHR_KEY_UP: return SAF_buttonPressed(SAF_BUTTON_UP); break; 
    case LHR_KEY_DOWN: return SAF_buttonPressed(SAF_BUTTON_DOWN); break;
    case LHR_KEY_RIGHT: return SAF_buttonPressed(SAF_BUTTON_RIGHT); break;
    case LHR_KEY_LEFT: return SAF_buttonPressed(SAF_BUTTON_LEFT); break;
    case LHR_KEY_A: return SAF_buttonPressed(SAF_BUTTON_A); break;
    case LHR_KEY_B: return SAF_buttonPressed(SAF_BUTTON_B); break;
    case LHR_KEY_C: return SAF_buttonPressed(SAF_BUTTON_C); break;
    default: return 0; break;
  }
}

void LHR_getMouseOffset(int16_t *x, int16_t *y)
{
}

void LHR_setMusic(uint8_t value)
{
}

void LHR_save(uint8_t data[LHR_SAVE_SIZE])
{
  for (uint8_t i = 0; i < LHR_SAVE_SIZE; ++i)
    SAF_save(i,data[i]);
}

void LHR_processEvent(uint8_t event, uint8_t data)
{
}

uint8_t LHR_load(uint8_t data[LHR_SAVE_SIZE])
{
  for (uint8_t i = 0; i < LHR_SAVE_SIZE; ++i)
    data[i] = SAF_load(i);

  return 1;
}

void LHR_playSound(uint8_t soundIndex, uint8_t volume)
{
  switch (soundIndex)
  {
    case 0: SAF_playSound(SAF_SOUND_BUMP); break;
    case 1: SAF_playSound(SAF_SOUND_CLICK); break;
    case 2: SAF_playSound(SAF_SOUND_BOOM); break;
    case 3: SAF_playSound(SAF_SOUND_CLICK); break;
    case 4: SAF_playSound(SAF_SOUND_BUMP); break;
    case 5: SAF_playSound(SAF_SOUND_BEEP); break;
    default: break;
  }
}

void SAF_init(void)
{
  for (uint16_t i = 0; i < 256; ++i)
  {
    uint16_t c = paletteRGB565[i];

    uint8_t 
      r = (c >> 8) & 0xf8,
      g = (c >> 3) & 0xf8,
      b = (c << 3) & 0xf8;

    palette[i] = SAF_colorFromRGB(r,g,b);
  }

  LHR_init();
}

uint8_t SAF_loop(void)
{
  return LHR_mainLoopBody();
}
