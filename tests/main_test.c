/**
  @file main_test.c

  This is a front end that serves as a basic automatic test of the game. At this
  point it's very simple and basically just checks if anything really
  substantial wasn't broken.

  This fronted tries to play the game and see if it behaves how expected. If you
  change anything substantial in the game, this test may start to fail and you 
  may need to adjust it.


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <stdio.h>
#include <time.h>

#define LHR_SCREEN_RESOLUTION_X 67
#define LHR_SCREEN_RESOLUTION_Y 31
#define LHR_BACKGROUND_BLUR 1
#define LHR_DITHERED_SHADOW 1
#define LHR_FPS 30

#include "game.h"
#include "sounds.h"

uint8_t screen[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
uint8_t keys[LHR_KEY_COUNT];

uint32_t gameTime = 0;

int8_t LHR_keyPressed(uint8_t key)
{
  return keys[key];
}

void LHR_getMouseOffset(int16_t *x, int16_t *y)
{
}

uint32_t LHR_getTimeMs()
{
  return gameTime;
}

void LHR_sleepMs(uint16_t gameTimeMs)
{
}

int aaa = 100;

static inline void LHR_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  screen[y * LHR_SCREEN_RESOLUTION_X + x] = colorIndex;
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

void printTestHeading(const char *text)
{
  printf("\n~~~~~ testing: %s ~~~~~\n\n",text);
}

const char colors[9] = " .,-;imX";

void printScreen()
{
  const char *c = (const char *)screen;

  for (uint8_t y = 0; y < LHR_SCREEN_RESOLUTION_Y; ++y)
  {
    for (uint8_t x = 0; x < LHR_SCREEN_RESOLUTION_X; ++x)
    {
      putchar(*c != 7 ? colors[((uint8_t)*c) % 8] : '@');
      ++c;
    }

    putchar('\n');
  }
}

int main(void)
{
  puts("===== TESTING REVOLTE =====\n");

  puts("initializing");

  #define ASSERT(text,cond) { printf("checking \"%s\": ",text); if (cond)  puts("OK"); else { puts("ERROR"); return 1; }}

  LHR_init();

  double msPerFrame = 0;

  ASSERT("frame == 0",LHR_game.frame == 0);

  {
    printTestHeading("music and sounds");

    static const uint8_t expectedSamples[] = { 1, 0, 0, 0, 0, 0, 255, 251, 80, 240, 240, 10, 0, 6, 4, 0 };

    uint16_t pos = 0;

    for (uint32_t i = 0; i < (LHR_TRACK_COUNT * LHR_TRACK_SAMPLES); ++i)
    {
      uint8_t sample = LHR_getNextMusicSample();

      if (i % 200000 == 0)
      {
        ASSERT("music sample", sample == expectedSamples[pos]);
        pos++;
      }
    }

    ASSERT("sfx sample",LHR_GET_SFX_SAMPLE(0,0) == 128);
    ASSERT("sfx sample",LHR_GET_SFX_SAMPLE(1,200) == 112);
    ASSERT("sfx sample",LHR_GET_SFX_SAMPLE(3,512) == 112);
    ASSERT("sfx sample",LHR_GET_SFX_SAMPLE(4,1000) == 128);
  }

  {
    printTestHeading("levels");
    
    LHR_TileDefinition t;
    uint8_t p;

    t = LHR_getMapTile(&LHR_level1,10,8,&p);
    ASSERT("level1 tile",LHR_TILE_FLOOR_HEIGHT(t) == 14 && LHR_TILE_CEILING_HEIGHT(t) == 0 && LHR_TILE_FLOOR_TEXTURE(t) == 4 && p == 0);

    t = LHR_getMapTile(&LHR_level3,9,20,&p);
    ASSERT("level3 tile",LHR_TILE_FLOOR_HEIGHT(t) == 17 && LHR_TILE_CEILING_HEIGHT(t) == 13 && LHR_TILE_FLOOR_TEXTURE(t) == 0 && p == 128);

    t = LHR_getMapTile(&LHR_level5,-9,0,&p);
    ASSERT("outside tile",LHR_TILE_FLOOR_HEIGHT(t) == 31 && LHR_TILE_CEILING_HEIGHT(t) == 0 && LHR_TILE_FLOOR_TEXTURE(t) == 7 && p == 0);
  }
 
  {
    printTestHeading("gameplay");

    for (uint8_t i = 0; i < LHR_KEY_COUNT; ++i)
      keys[i] = 0;

    #define STEP(ms) { printf("(fr %d, step %d ms) ",LHR_game.frame,ms); gameTime += ms; LHR_mainLoopBody(); }
    #define PRESS(k) { printf("(press %d) ",k); keys[k] = 1; }
    #define RELEASE(k) { printf("(release %d) ",k); keys[k] = 0; }
    #define TEST_PIXEL(x,y,v) { printf("(testing pixel %d %d)",x,y); uint8_t val =  screen[y * LHR_SCREEN_RESOLUTION_X + y]; if (val != v) { printf("\nERROR: expcted %d, got %d\n",v,val); return 1; }}

    STEP(10)
    STEP(100)
    PRESS(LHR_KEY_DOWN) // select "exit"
    STEP(1000)
    RELEASE(LHR_KEY_DOWN)
    TEST_PIXEL(10,20,64)

    putchar('\n');
    ASSERT("menu item == exit",LHR_getMenuItem(LHR_game.selectedMenuItem) == LHR_MENU_ITEM_EXIT)

    PRESS(LHR_KEY_UP) // select "play"
    STEP(700)
    RELEASE(LHR_KEY_UP)
    PRESS(LHR_KEY_A) // confirm "play"
    STEP(100)
    TEST_PIXEL(30,21,0)
    RELEASE(LHR_KEY_A)
    STEP(100)
    PRESS(LHR_KEY_A) // skip intro
    STEP(2000)

    putchar('\n');
    ASSERT("state == playing",LHR_game.state == LHR_GAME_STATE_PLAYING)

    RELEASE(LHR_KEY_A)
    PRESS(LHR_KEY_RIGHT) // turn
    STEP(400)
    RELEASE(LHR_KEY_RIGHT)
    PRESS(LHR_KEY_UP) // take ammo
    STEP(400)

    putchar('\n');
    ASSERT("weapon == shotgun",LHR_player.weapon == LHR_WEAPON_SHOTGUN)

    RELEASE(LHR_KEY_UP)
    PRESS(LHR_KEY_LEFT) // turn back
    STEP(700)
    RELEASE(LHR_KEY_LEFT)
    PRESS(LHR_KEY_UP) // go to barrels
    STEP(1000)
    RELEASE(LHR_KEY_UP)
    PRESS(LHR_KEY_RIGHT)
    STEP(200)
    RELEASE(LHR_KEY_RIGHT)
    PRESS(LHR_KEY_A) // shoot barrels
    STEP(700)
    RELEASE(LHR_KEY_A)

    putchar('\n');
    ASSERT("health < 100",LHR_player.health < 100)

    PRESS(LHR_KEY_UP)
    STEP(720)
    RELEASE(LHR_KEY_UP)
    PRESS(LHR_KEY_LEFT)
    STEP(300)
    RELEASE(LHR_KEY_LEFT)
    PRESS(LHR_KEY_UP)
    STEP(700)
    RELEASE(LHR_KEY_UP)
    PRESS(LHR_KEY_RIGHT)
    STEP(700)
    RELEASE(LHR_KEY_RIGHT)
    PRESS(LHR_KEY_UP)
    STEP(850)
    RELEASE(LHR_KEY_UP)
    STEP(2500)
    PRESS(LHR_KEY_A) // shoot monster
    STEP(200)
    RELEASE(LHR_KEY_A) // shoot monster
    STEP(900)
    PRESS(LHR_KEY_LEFT)
    PRESS(LHR_KEY_NEXT_WEAPON) // switch to machine gun
    STEP(100)
    RELEASE(LHR_KEY_LEFT)
    RELEASE(LHR_KEY_NEXT_WEAPON)

    putchar('\n');
    ASSERT("weapon == machine gun",LHR_player.weapon == LHR_WEAPON_MACHINE_GUN)

    STEP(1000)
    PRESS(LHR_KEY_A) // shoot
    STEP(2000)
    
    putchar('\n');
    ASSERT("health == 74",LHR_player.health == 74)
  
    RELEASE(LHR_KEY_A)

    STEP(100)
      PRESS(LHR_KEY_LEFT)

      #define FRAMES 1000000

      printf("\nbenchmarking frame time on %d frames.\n",FRAMES);
       
      clock_t t1, t2;
       
      t1 = clock();
      STEP(FRAMES);
      t2 = clock();
      msPerFrame = (((double) (t2 - t1)) * 1000.0) / (CLOCKS_PER_SEC * FRAMES);
      RELEASE(LHR_KEY_LEFT)
    STEP(100)

    PRESS(LHR_KEY_C) // open menu
    PRESS(LHR_KEY_DOWN)
    STEP(200)
    RELEASE(LHR_KEY_C)

    putchar('\n');
    ASSERT("state == menu",LHR_game.state == LHR_GAME_STATE_MENU)

    STEP(1000)
    PRESS(LHR_KEY_A) // exit game
    STEP(100)

    putchar('\n');
    ASSERT("game exitted",LHR_mainLoopBody() == 0)

    putchar('\n');
    printScreen();

    #undef PRESS
    #undef RELEASE
    #undef STEP
  }
 
  puts("======================================\n\nDone.\nEverything seems OK.");

  printf("benchmarked ms per frame: %lf\n",msPerFrame);

  return 0;
}
