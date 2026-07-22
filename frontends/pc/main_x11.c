/**
  @file main_x11.c

  This is X11 implementation of the game front end, without sound. This can be
  used as a fallback on most Unices where other frontends somehow fail.


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__APPLE__)
  #define LHR_OS_IS_MALWARE 1
#endif

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdlib.h>

#define LHR_FPS 30
#define LHR_SCREEN_RESOLUTION_X 320
#define LHR_SCREEN_RESOLUTION_Y 240
#define LHR_RAYCASTING_SUBSAMPLE 2
#define LHR_RESOLUTION_SCALEDOWN 2
#define LHR_LOG(str) puts(str);
#define LHR_DITHERED_SHADOW 1
#define LHR_BACKGROUND_BLUR 0
#define LHR_RAYCASTING_MAX_STEPS 18
#define LHR_RAYCASTING_MAX_HITS 8

#define LHR_PC 1
#include "game.h"
#include "palette.h"
#define WIPE_PIXEL_TYPE uint_least16_t
#include "wipe_effect.h"
#include <string.h>

static uint_least16_t wipe_scr_start[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
static uint_least16_t wipe_scr_end[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
static int wipe_y[LHR_SCREEN_RESOLUTION_X];

uint_least16_t x11_screen[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
// ^ Each value is current (lower 8 bits) and previous pixel.

uint8_t x11_buttons[LHR_KEY_COUNT];
unsigned long x11_palette[256];
unsigned int interlaceFrame = 0;
unsigned int drawLine = 0;

void LHR_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  uint_least16_t *word = x11_screen + y * LHR_SCREEN_RESOLUTION_X + x;
  *word = ((*word) & 0xff00) | colorIndex;
}

uint32_t LHR_getTimeMs(void)
{
  struct timeval now;
  gettimeofday(&now,NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
}

void LHR_save(uint8_t data[LHR_SAVE_SIZE])
{
  FILE *f = fopen(LHR_SAVE_FILE_PATH,"wb");

  puts("X11: opening and writing save file");

  if (f == NULL)
  {
    puts("X11: could not open the file!");
    return;
  }

  fwrite(data,1,LHR_SAVE_SIZE,f);

  fclose(f);
}

uint8_t LHR_load(uint8_t data[LHR_SAVE_SIZE])
{
  FILE *f = fopen(LHR_SAVE_FILE_PATH,"rb");

  puts("X11: opening and reading save file");

  if (f == NULL)
  {
    puts("X11: no save file to open");
  }
  else
  {
    if (fread(data, 1, LHR_SAVE_SIZE, f) != LHR_SAVE_SIZE)
    {
      puts("X11: warning: save file size mismatch or read error");
    }
    fclose(f);
  }

  return 1;
}

void LHR_sleepMs(uint16_t timeMs)
{
  usleep(timeMs * 1000);
}

void LHR_getMouseOffset(int16_t *x, int16_t *y)
{
}

void LHR_processEvent(uint8_t event, uint8_t data)
{
  printf("event: %d\n",event);
}

int8_t LHR_keyPressed(uint8_t key)
{
  return x11_buttons[key];
}

void LHR_setMusic(uint8_t value)
{
  printf("music: %d\n",value);
}

void LHR_playSound(uint8_t soundIndex, uint8_t volume)
{
  printf("sound: %d\n",soundIndex);
}

int main(int argc, char *argv[])
{
  int targetStartLevel = -1;
  int forceMapReveal = 0;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--lhrwarp") == 0 && i + 1 < argc) {
      targetStartLevel = atoi(argv[i+1]);
      i++;
    }
    else if (strcmp(argv[i], "--lhrmap") == 0) {
      forceMapReveal = 1;
    }
  }

  puts(
    "L'Homme Révolté v" LHR_VERSION_STRING "X11 frontend (no sound)\n");

  LHR_init();

  if (targetStartLevel >= 0 && targetStartLevel < LHR_NUMBER_OF_LEVELS)
  {
    LHR_game.save[0] = (LHR_game.save[0] & 0xF0) | (LHR_NUMBER_OF_LEVELS - 1);
    LHR_game.selectedLevel = targetStartLevel;
    LHR_setAndInitLevel(targetStartLevel);
  }

  Display *display = XOpenDisplay(0);
  
  if (display == 0) 
  {
    puts("X11: could not open a display");
    return 0;
  }

  int screen = DefaultScreen(display);

  Window window = XCreateSimpleWindow(display,RootWindow(display,screen),10,10,
    LHR_SCREEN_RESOLUTION_X,LHR_SCREEN_RESOLUTION_Y,1,
    BlackPixel(display,screen),BlackPixel(display,screen));

  XMapWindow(display,window);

  XSizeHints *hints = XAllocSizeHints();
  hints->flags = PMinSize | PMaxSize;
  hints->min_width = LHR_SCREEN_RESOLUTION_X;
  hints->max_width = hints->min_width;
  hints->min_height = LHR_SCREEN_RESOLUTION_Y;
  hints->max_height = hints->min_height;
  XSetWMNormalHints(display,window,hints);
  XFree(hints);

  XSelectInput(display,window,KeyPressMask | KeyReleaseMask | ExposureMask);

  // create the palette:

  for (int i = 0; i < 256; ++i)
  {
    XColor color;

    color.red = ((paletteRGB565[i] & 0xf800) >> 8) * 256;
    color.green = ((paletteRGB565[i] & 0x07e0) >> 3) * 256;
    color.blue = ((paletteRGB565[i] & 0x00ff) << 3) * 256;
    color.flags = DoRed | DoGreen | DoBlue;

    XAllocColor(display,DefaultColormap(display,0),&color);

    x11_palette[i] = color.pixel;
  }

  GC context = DefaultGC(display,screen);

  int running = 1;

  /*
    rendering (and game update) loop follows:

    With xlib it's a little difficult to do realtime rendering efficiently, so
    the algorithm here is a bit sophisticated. Here is a summary:
   
    - We start drawing from global drawLine and draw a certain number of lines,
      the next frame we start drawing from drawLine again etc., so we're drawing
      by lines and rolling over to top after reaching bottom.
    - We don't redraw pixels of same color and we don't call XSetForeground if
      we'd be setting to same color.
    - Each frame we keep track of how many "pixel drawing operations" we've
      performed and stop rendering after certain limit, as drawing too many
      pixels slows the frame rendering (and hence the whole game) down.

    This makes most typical frames render quickly in one go (with only a few
    necessary updates of the previous frames) but frames that would take too
    much time (very different from previous frame) will be split to be drawn in
    several parts, resulting in kind of "vsync" like artifacts but retaining the
    game speed. If the game's still slow, fiddle with the hardcoded limit in the
    loop.
  */

  while (running)
  {
    uint_least16_t *pixel;

    uint8_t stateBefore = LHR_game.state;
    if (stateBefore == LHR_GAME_STATE_MENU || stateBefore == LHR_GAME_STATE_WIN || stateBefore == LHR_GAME_STATE_LOSE || stateBefore == LHR_GAME_STATE_INTRO) {
        memcpy(wipe_scr_start, x11_screen, sizeof(x11_screen));
    }

    running = LHR_mainLoopBody();

    if ( (stateBefore == LHR_GAME_STATE_MENU && LHR_game.state != LHR_GAME_STATE_MENU) ||
         (stateBefore == LHR_GAME_STATE_WIN && LHR_game.state != LHR_GAME_STATE_WIN) ||
         (stateBefore == LHR_GAME_STATE_LOSE && LHR_game.state == LHR_GAME_STATE_MENU) ) {
         
         memcpy(wipe_scr_end, x11_screen, sizeof(x11_screen));
         wipe_initMelt(wipe_y, LHR_SCREEN_RESOLUTION_X);
         
         int done = 0;
         while (!done && running) {
            done = wipe_doMelt(x11_screen, wipe_scr_start, wipe_scr_end, wipe_y, LHR_SCREEN_RESOLUTION_X, LHR_SCREEN_RESOLUTION_Y);
            
            uint_least16_t pixelNow = 0, pixelPrev = 1;
            XSetForeground(display,context,x11_palette[pixelNow]);
            for (int i = 0; i < LHR_SCREEN_RESOLUTION_Y; ++i) {
                uint_least16_t *px = x11_screen + LHR_SCREEN_RESOLUTION_X * drawLine;
                for (int x = 0; x < LHR_SCREEN_RESOLUTION_X; ++x) {
                    uint_least16_t pixels = *px;
                    if ((pixels >> 8) != (pixels & 0x00ff)) {
                        pixelNow = pixels & 0x00ff;
                        if (pixelNow != pixelPrev) {
                            XSetForeground(display,context,x11_palette[pixelNow]);
                        }
                        pixelPrev = pixelNow;
                        *px = ((*px) & 0x00ff) | ((*px) << 8);
                        XDrawPoint(display,window,context,x,drawLine);
                    }
                    px++;
                }
                drawLine = (drawLine + 1) % LHR_SCREEN_RESOLUTION_Y;
            }
            XFlush(display);
            LHR_sleepMs(10);
         }
    }

    uint_least16_t pixelNow = 0, pixelPrev = 1;
    XSetForeground(display,context,x11_palette[pixelNow]);

    int drawCounter = 0;

    for (int i = 0; i < LHR_SCREEN_RESOLUTION_Y; ++i)
    {
      pixel = x11_screen + LHR_SCREEN_RESOLUTION_X * drawLine;

      for (int x = 0; x < LHR_SCREEN_RESOLUTION_X; ++x)
      {
        uint_least16_t pixels = *pixel;

        if ((pixels >> 8) != (pixels & 0x00ff)) // don't redraw same color
        {
          pixelNow = pixels & 0x00ff;

          if (pixelNow != pixelPrev)
          {
            XSetForeground(display,context,x11_palette[pixelNow]);
            drawCounter++;
          }

          pixelPrev = pixelNow;
          drawCounter++;

          *pixel = ((*pixel) & 0x00ff) | ((*pixel) << 8);

          XDrawPoint(display,window,context,x,drawLine);
        }

        pixel++;
      }

      drawLine = (drawLine + 1) % LHR_SCREEN_RESOLUTION_Y;

      if (drawCounter > 40000) // if too many pixels drawn, stop
        break;
    }

    XEvent event;

    while (XCheckWindowEvent(display,window,KeyPressMask | KeyReleaseMask | ExposureMask  ,&event) != False)
    {
      if (event.type == Expose)
      {
        // exposed => make next frame render as a whole:
        for (long int i = 0; i < LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y; ++i)
          x11_screen[i] = 1;
      }
      else
      {
        uint8_t state = event.xkey.type == KeyPress;
        KeySym keysym = XKeycodeToKeysym(display,event.xkey.keycode,0);

        if (state && keysym != XK_Shift_L && keysym != XK_Shift_R && keysym != XK_Caps_Lock)
        {
          if (LHR_game.state == LHR_GAME_STATE_MENU)
          {
            static const KeySym lhrwarp_seq[] = {
              XK_l, XK_h, XK_r, XK_w, XK_a, XK_r, XK_p
            };
            static uint8_t lhrwarp_idx = 0;
            
            if (keysym == lhrwarp_seq[lhrwarp_idx])
            {
              lhrwarp_idx++;
              if (lhrwarp_idx == 7)
              {
                LHR_game.save[0] = (LHR_game.save[0] & 0xF0) | (LHR_NUMBER_OF_LEVELS - 1);
                puts("LHRWARP cheat activated: All levels unlocked!");
                lhrwarp_idx = 0;
              }
            }
            else
              lhrwarp_idx = 0;
          }

          static const KeySym lhrmap_seq[] = {
            XK_l, XK_h, XK_r, XK_m, XK_a, XK_p
          };
          static uint8_t lhrmap_idx = 0;

          if (keysym == lhrmap_seq[lhrmap_idx])
          {
            lhrmap_idx++;
            if (lhrmap_idx == 6)
            {
              forceMapReveal = 1;
              puts("LHRMAP cheat activated: Map revealed!");
              lhrmap_idx = 0;
            }
          }
          else
            lhrmap_idx = 0;
        }

        switch (keysym)
        {
          case XK_Escape: running = 0; break;
          case XK_Up: // fallthrough
          case XK_KP_8:
          case XK_w: x11_buttons[LHR_KEY_UP] = state; break;
          case XK_Left: // fallthrough
          case XK_KP_7:
          case XK_q: x11_buttons[LHR_KEY_LEFT] = state; break;
          case XK_Right: // fallthrough
          case XK_KP_9:
          case XK_e: x11_buttons[LHR_KEY_RIGHT] = state; break;
          case XK_Down: // fallthrough
          case XK_KP_5:
          case XK_s: x11_buttons[LHR_KEY_DOWN] = state; break;
          case XK_y: // fallthrough
          case XK_z: // fallthrough
          case XK_j: // fallthrough
          case XK_Control_L:
          case XK_Control_R:
          case XK_Return: x11_buttons[LHR_KEY_A] = state; break;
          case XK_Alt_L:
          case XK_Alt_R:
          case XK_x: // fallthrough
          case XK_k: x11_buttons[LHR_KEY_B] = state; break;
          case XK_c: // fallthrough
          case XK_l: x11_buttons[LHR_KEY_C] = state; break;

          // extra keys:
          case XK_space: x11_buttons[LHR_KEY_JUMP] = state; break;
          case XK_Tab: x11_buttons[LHR_KEY_MAP] = state; break;
          case XK_a: // fallthrough 
          case XK_KP_4: x11_buttons[LHR_KEY_STRAFE_LEFT] = state; break;
          case XK_d: // fallthrough
          case XK_KP_6: x11_buttons[LHR_KEY_STRAFE_RIGHT] = state; break;
          case XK_v: x11_buttons[LHR_KEY_CYCLE_WEAPON] = state; break;
          case XK_r: // fallthrough
          case XK_F1: // fallthrough
          case XK_Page_Down: x11_buttons[LHR_KEY_PREVIOUS_WEAPON] = state; break;
          case XK_t: // fallthrough
          case XK_F2: // fallthrough
          case XK_Page_Up: x11_buttons[LHR_KEY_NEXT_WEAPON] = state; break;
          case XK_p: // fallthrough
          case XK_Home: x11_buttons[LHR_KEY_TOGGLE_FREELOOK] = state; break;

          default: break;
        }
      }
    }
    
    XFlush(display);
  }

  puts("X11: ending");
  XCloseDisplay(display);

  return 0;
}
