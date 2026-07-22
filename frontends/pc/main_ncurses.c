/**
  @file main_ncurses.c

  This is ncurses (terminal) implementation of the game front end. This isn't
  a full fledged version but rather a "show off" of what can be done with the
  game; especially the limited ability of ncurses to handle input makes this
  version very hard to play.


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <ncurses.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define LHR_SCREEN_RESOLUTION_X 120
#define LHR_SCREEN_RESOLUTION_Y 40
#define LHR_FPS 30

#define LHR_PC 1
#include "game.h"

uint8_t ncScreen[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
uint8_t ncButtonStates[LHR_KEY_COUNT];
uint32_t timeStart;

const char asciiPalette[] = 
#if 1
  " .',:;lcoxkXK0MW";
#else
  " -.,;!/clfsxaVO#"; 
#endif

uint32_t currentTime()
{
  struct timeval t;
  gettimeofday(&t,NULL);
  return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int8_t LHR_keyPressed(uint8_t key)
{
  return ncButtonStates[key];
}

void LHR_getMouseOffset(int16_t *x, int16_t *y)
{
  return;
}

uint32_t LHR_getTimeMs()
{
  return currentTime() - timeStart;
}

void LHR_sleepMs(uint16_t timeMs)
{
  usleep(1000 * timeMs);
}

void LHR_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  ncScreen[y * LHR_SCREEN_RESOLUTION_X + x] = asciiPalette[colorIndex % 16];
}

void LHR_playSound(uint8_t soundIndex, uint8_t volume)
{
}

void LHR_setMusic(uint8_t value)
{
}

void LHR_processEvent(uint8_t event, uint8_t data)
{
}

void LHR_save(uint8_t data[LHR_SAVE_SIZE])
{
}

uint8_t LHR_load(uint8_t data[LHR_SAVE_SIZE])
{
  return 0;
}

int main(void)
{
  timeStart = currentTime();

  initscr();
  halfdelay(1);
  keypad(stdscr,TRUE);
  noecho();
  curs_set(0);

  LHR_init();

  int goOn = 1;

  while (goOn)
  {
    erase();
    move(0,0);

    const uint8_t *scr = ncScreen;

    for (int y = 0; y < LHR_SCREEN_RESOLUTION_Y; ++y)
    {
      move(y,0);

      for (int x = 0; x < LHR_SCREEN_RESOLUTION_X; ++x)
      {
        addch(*scr);
        scr++;
      }
    }

    refresh();

    goOn = LHR_mainLoopBody();

    for (int i = 0; i < LHR_KEY_COUNT; ++i)
      ncButtonStates[i] = 0;

    for (int i = 0; i < 1024; ++i)
    {
      int c = getch();

      if (c == ERR)
        break;

      switch (c)
      {
        case KEY_UP:      ncButtonStates[LHR_KEY_UP] = 1; break;
        case KEY_LEFT:    ncButtonStates[LHR_KEY_LEFT] = 1; break;
        case KEY_RIGHT:   ncButtonStates[LHR_KEY_RIGHT] = 1; break;
        case KEY_DOWN:    ncButtonStates[LHR_KEY_DOWN] = 1; break;
        case 'a':         // fallthrough
        case KEY_ENTER:   ncButtonStates[LHR_KEY_A] = 1; break;
        case KEY_CANCEL:  // fallthrough
        case KEY_CLOSE:   // fallthrough
        case 's':         ncButtonStates[LHR_KEY_B] = 1; break;
        case 'd':         ncButtonStates[LHR_KEY_C] = 1; break;
        case ' ':         ncButtonStates[LHR_KEY_JUMP] = 1; break;
        case 'q':         ncButtonStates[LHR_KEY_MENU] = 1; break;
        case 'f':         ncButtonStates[LHR_KEY_NEXT_WEAPON] = 1; break;
        case 'g':         ncButtonStates[LHR_KEY_PREVIOUS_WEAPON] = 1; break;
        case 'n':         ncButtonStates[LHR_KEY_STRAFE_LEFT] = 1; break;
        case 'm':         ncButtonStates[LHR_KEY_STRAFE_RIGHT] = 1; break;
        default: break;
      }
    }
  }

  endwin();

  return 0;
}
