/**
  @file main_sdl.c

  This is an SDL2 implementation of the game front end. It can be used to
  compile a native executable or a transpiled JS browser version with
  emscripten.

  This frontend is not strictly minimal, it could be reduced a lot. If you want
  a learning example of frontend, look at another, simpler one, e.g. terminal.

  To compile with emscripten run:

  emcc ./main_sdl.c -s USE_SDL=2 -O3 --shell-file HTMLshell.html -o game.html


  SPDX-License-Identifier: GPL-3.0-or-later
*/

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__APPLE__)
  #define LHR_OS_IS_MALWARE 1
#endif

// #define LHR_START_LEVEL 1
// #define LHR_QUICK_WIN 1
// #define LHR_IMMORTAL 1
// #define LHR_ALL_LEVELS 1
// #define LHR_UNLOCK_DOOR 1
// #define LHR_REVEAL_MAP 1
// #define LHR_INFINITE_AMMO 1
// #define LHR_TIME_MULTIPLIER 512
// #define LHR_CPU_LOAD(percent) printf("CPU load: %d%\n",percent);
// #define GAME_LQ

#ifndef __EMSCRIPTEN__
  #ifndef GAME_LQ
    // higher quality
    #define LHR_FPS 60
    #define LHR_LOG(str) puts(str);
    #define LHR_SCREEN_RESOLUTION_X 700
    #define LHR_SCREEN_RESOLUTION_Y 512
    #define LHR_DITHERED_SHADOW 1
    #define LHR_DIMINISH_SPRITES 1
    #define LHR_HEADBOB_SHEAR (-1 * LHR_SCREEN_RESOLUTION_Y / 80)
    #define LHR_BACKGROUND_BLUR 1
  #else
    // lower quality
    #define LHR_FPS 35
    #define LHR_SCREEN_RESOLUTION_X 640
    #define LHR_SCREEN_RESOLUTION_Y 480
    #define LHR_RAYCASTING_SUBSAMPLE 2
    #define LHR_RESOLUTION_SCALEDOWN 2
    #define LHR_LOG(str) puts(str);
    #define LHR_DIMINISH_SPRITES 0
    #define LHR_DITHERED_SHADOW 0
    #define LHR_BACKGROUND_BLUR 0
    #define LHR_RAYCASTING_MAX_STEPS 18
    #define LHR_RAYCASTING_MAX_HITS 8
  #endif
#else
  // emscripten
  #define LHR_FPS 35
  #define LHR_SCREEN_RESOLUTION_X 512
  #define LHR_SCREEN_RESOLUTION_Y 320
  #define LHR_CAN_EXIT 0
  #define LHR_RESOLUTION_SCALEDOWN 2
  #define LHR_DITHERED_SHADOW 1
  #define LHR_BACKGROUND_BLUR 0
  #define LHR_RAYCASTING_MAX_STEPS 18
  #define LHR_RAYCASTING_MAX_HITS 8

  #include <emscripten.h>
#endif

/*
  SDL is easier to play thanks to nice controls so make the player take full
  damage to make it a bit harder.
*/
#define LHR_PLAYER_DAMAGE_MULTIPLIER 1024

#define SDL_MUSIC_VOLUME 16

#define SDL_ANALOG_DIVIDER 1024

#if !LHR_OS_IS_MALWARE
  #include <signal.h>
#endif

#define SDL_MAIN_HANDLED 1

#define SDL_DISABLE_IMMINTRIN_H 1

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>

#define LHR_PC 1
#include "game.h"
#include "sounds.h"
#include "wipe_effect.h"
#include <sys/select.h>

uint8_t agentSyncMode = 0;
uint16_t agentKeyBitmask = 0;
int16_t agentMouseDx = 0;
int16_t agentMouseDy = 0;
uint8_t screen[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y]; // Needed by agent_harness.h
#include "agent_harness.h"

const uint8_t *sdlKeyboardState;
uint8_t webKeyboardState[LHR_KEY_COUNT];
#ifdef __EMSCRIPTEN__
int16_t webMouseX = 0;
int16_t webMouseY = 0;
#endif

uint8_t sdlMouseButtonState = 0;
int8_t sdlMouseWheelState = 0;
SDL_GameController *sdlController;

uint16_t sdlScreen[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y]; // RGB565

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

// now implement the L'Homme Révolté API functions (LHR_*)

void LHR_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  sdlScreen[y * LHR_SCREEN_RESOLUTION_X + x] = paletteRGB565[colorIndex];
  screen[y * LHR_SCREEN_RESOLUTION_X + x] = colorIndex;
}

uint32_t LHR_getTimeMs(void)
{
  if (agentSyncMode) {
    static uint32_t t = 0;
    t += 33; // LHR_MS_PER_FRAME is typically 33
    return t;
  }
  return SDL_GetTicks();
}

void LHR_save(uint8_t data[LHR_SAVE_SIZE])
{
  FILE *f = fopen(LHR_SAVE_FILE_PATH,"wb");

  puts("SDL: opening and writing save file");

  if (f == NULL)
  {
    puts("SDL: could not open the file!");
    return;
  }

  fwrite(data,1,LHR_SAVE_SIZE,f);

  fclose(f);
}

uint8_t LHR_load(uint8_t data[LHR_SAVE_SIZE])
{
#ifndef __EMSCRIPTEN__
  FILE *f = fopen(LHR_SAVE_FILE_PATH,"rb");

  puts("SDL: opening and reading save file");

  if (f == NULL)
  {
    puts("SDL: no save file to open");
  }
  else
  {
    if (fread(data, 1, LHR_SAVE_SIZE, f) != LHR_SAVE_SIZE)
    {
      puts("SDL: warning: save file size mismatch or read error");
    }
    fclose(f);
  }

  return 1;
#else
  // no saving for web version
  return 0;
#endif
}

void LHR_sleepMs(uint16_t timeMs)
{
#ifndef __EMSCRIPTEN__
  usleep(timeMs * 1000);
#endif
}

#ifdef __EMSCRIPTEN__
void webButton(uint8_t key, uint8_t down) // HTML button pressed
{
  webKeyboardState[key] = down;
}
#endif

int8_t mouseMoved = 0; /* Whether the mouse has moved since program started,
                          this is needed to fix an SDL limitation. */

void LHR_getMouseOffset(int16_t *x, int16_t *y)
{
  if (agentSyncMode)
  {
    *x = agentMouseDx;
    *y = agentMouseDy;
    agentMouseDx = 0;
    agentMouseDy = 0;
    return;
  }
#ifdef __EMSCRIPTEN__
  *x = webMouseX;
  *y = webMouseY;
  webMouseX = 0;
  webMouseY = 0;
#else
  if (mouseMoved)
  {
    int mX, mY;

    SDL_GetMouseState(&mX,&mY);

    *x = mX - LHR_SCREEN_RESOLUTION_X / 2;
    *y = mY - LHR_SCREEN_RESOLUTION_Y / 2;

    SDL_WarpMouseInWindow(window,
      LHR_SCREEN_RESOLUTION_X / 2, LHR_SCREEN_RESOLUTION_Y / 2);
  }

  if (sdlController != NULL)
  {
    *x +=
      (SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_RIGHTX) + 
      SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_LEFTX)) /
      SDL_ANALOG_DIVIDER;

    *y +=
      (SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_RIGHTY) + 
      SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_LEFTY)) /
      SDL_ANALOG_DIVIDER;
  }
#endif
}

void LHR_processEvent(uint8_t event, uint8_t data)
{
}

int8_t LHR_keyPressed(uint8_t key)
{
  if (agentSyncMode) return (agentKeyBitmask >> key) & 0x01;
  if (webKeyboardState[key]) // this only takes effect in the web version 
    return 1;

  #define k(x) sdlKeyboardState[SDL_SCANCODE_ ## x]
  #define b(x) ((sdlController != NULL) && \
    SDL_GameControllerGetButton(sdlController,SDL_CONTROLLER_BUTTON_ ## x))

  switch (key)
  {
    case LHR_KEY_UP: return k(UP) || k(W) || k(KP_8) || b(DPAD_UP); break;
    case LHR_KEY_RIGHT: 
      return k(RIGHT) || k(E) || k(KP_6) || b(DPAD_RIGHT); break;
    case LHR_KEY_DOWN: 
      return k(DOWN) || k(S) || k(KP_5) || k(KP_2) || b(DPAD_DOWN); break;
    case LHR_KEY_LEFT: return k(LEFT) || k(Q) || k(KP_4) || b(DPAD_LEFT); break;
    case LHR_KEY_A: return k(J) || k(RETURN) || k(LCTRL) || k(RCTRL) || b(X) ||
      b(RIGHTSTICK) || (sdlMouseButtonState & SDL_BUTTON_LMASK); break;
    case LHR_KEY_B: return k(K) || k(LSHIFT) || b(B); break;
    case LHR_KEY_C: return k(L) || b(Y); break;
    case LHR_KEY_JUMP: return k(SPACE) || b(A); break;
    case LHR_KEY_STRAFE_LEFT: return k(A) || k(KP_7); break;
    case LHR_KEY_STRAFE_RIGHT: return k(D) || k(KP_9); break;
    case LHR_KEY_MAP: return k(TAB) || b(BACK); break;
    case LHR_KEY_CYCLE_WEAPON: return k(F) ||
      (sdlMouseButtonState & SDL_BUTTON_MMASK); break;
    case LHR_KEY_TOGGLE_FREELOOK: return b(LEFTSTICK) ||
      (sdlMouseButtonState & SDL_BUTTON_RMASK); break;
    case LHR_KEY_MENU: return k(ESCAPE) || b(START); break;
    case LHR_KEY_NEXT_WEAPON:
      if (k(P) || k(X) || b(RIGHTSHOULDER))
        return 1;

#define checkMouse(cmp)\
  if (sdlMouseWheelState cmp 0) { sdlMouseWheelState = 0; return 1; }

      checkMouse(>)
        
      return 0;
      break;

    case LHR_KEY_PREVIOUS_WEAPON:
      if (k(O) || k(Y) || k(Z) || b(LEFTSHOULDER))
        return 1;

      checkMouse(<)

#undef checkMouse
      
      return 0;
      break;

    default: return 0; break;
  }

  #undef k
  #undef b
}
  
int running;
int forceMapReveal = 0;
int autoScreenshot = 0;

static uint16_t wipe_scr_start[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
static uint16_t wipe_scr_end[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
static int wipe_y[LHR_SCREEN_RESOLUTION_X];

static uint8_t isWiping = 0;

void mainLoopIteration(void)
{
  SDL_Event event;

  if (agentSyncMode)
  {
    static uint8_t pendingTicks = 0;
    if (pendingTicks > 0)
    {
      pendingTicks--;
    }
    else
    {
      LHR_agentDumpStateJSON(stdout);
      
      char buffer[256];
      int gotCommand = 0;
      fd_set readfds;
      struct timeval tv;
      
      while (!gotCommand && running)
      {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
        
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &readfds))
        {
          if (fgets(buffer, sizeof(buffer), stdin))
          {
            LHR_AgentAction act = LHR_agentParseActionLine(buffer);
            agentKeyBitmask = act.keyBitmask;
            agentMouseDx = act.mouseDx;
            agentMouseDy = act.mouseDy;
            pendingTicks = act.stepTicks;
            if (pendingTicks > 0) pendingTicks--;
            gotCommand = 1;
          }
          else running = 0;
        }
        
        while (SDL_PollEvent(&event))
          if (event.type == SDL_QUIT) running = 0;

        // Keep Mac window alive and rendered while waiting for agent
        SDL_UpdateTexture(texture, NULL, sdlScreen, LHR_SCREEN_RESOLUTION_X * sizeof(uint16_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
      }
      if (!running) return;
    }
  }

  if (isWiping && !agentSyncMode)
  {
    int done = wipe_doMelt(sdlScreen, wipe_scr_start, wipe_scr_end, wipe_y, LHR_SCREEN_RESOLUTION_X, LHR_SCREEN_RESOLUTION_Y);
    if (done)
    {
      isWiping = 0;
    }
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT) { running = 0; isWiping = 0; }
    }
    SDL_UpdateTexture(texture, NULL, sdlScreen, LHR_SCREEN_RESOLUTION_X * sizeof(uint16_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
#ifndef __EMSCRIPTEN__
    SDL_Delay(10);
#endif
    return;
  }

  uint8_t stateBefore = LHR_game.state;
  if (stateBefore == LHR_GAME_STATE_MENU || stateBefore == LHR_GAME_STATE_WIN || stateBefore == LHR_GAME_STATE_LOSE) {
      memcpy(wipe_scr_start, sdlScreen, sizeof(sdlScreen));
  }

#ifdef __EMSCRIPTEN__
  // hack, without it sound won't work because of shitty browser audio policies

  if (LHR_game.frame % 512 == 0)
    SDL_PauseAudio(0);
#endif

  while (SDL_PollEvent(&event)) // also automatically updates sdlKeyboardState
  {
    if (event.type == SDL_MOUSEWHEEL)
    {
      if (event.wheel.y > 0)      // scroll up
        sdlMouseWheelState = 1;
      else if (event.wheel.y < 0) // scroll down
        sdlMouseWheelState = -1;
    }
    else if (event.type == SDL_QUIT)
      running = 0;
    else if (event.type == SDL_MOUSEMOTION)
    {
      mouseMoved = 1;
#ifdef __EMSCRIPTEN__
      webMouseX += event.motion.xrel;
      webMouseY += event.motion.yrel;
#endif
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_FINGERDOWN)
    {
#ifdef __EMSCRIPTEN__
      SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    }
 
    else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
    {
      SDL_Keycode sym = event.key.keysym.sym;
      if (sym == SDLK_F12)
      {
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);
        SDL_Surface *sshot = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        if (sshot) {
          SDL_RenderReadPixels(renderer, NULL, sshot->format->format, sshot->pixels, sshot->pitch);
          char filename[64];
          snprintf(filename, sizeof(filename), "screenshot_%u.bmp", (unsigned int)SDL_GetTicks());
          SDL_SaveBMP(sshot, filename);
          SDL_FreeSurface(sshot);
          printf("Screenshot saved to %s\n", filename);
        }
      }
      if (sym != SDLK_LSHIFT && sym != SDLK_RSHIFT && sym != SDLK_CAPSLOCK)
      {
        // LHRWARP logic
        if (LHR_game.state == LHR_GAME_STATE_MENU)
        {
          static const SDL_Keycode lhrwarp_seq[] = {
            SDLK_l, SDLK_h, SDLK_r, SDLK_w, SDLK_a, SDLK_r, SDLK_p
          };
          static uint8_t lhrwarp_idx = 0;
          
          if (sym == lhrwarp_seq[lhrwarp_idx])
          {
            lhrwarp_idx++;
            if (lhrwarp_idx == 7)
            {
              LHR_game.save[0] = (LHR_game.save[0] & 0xF0) | (LHR_NUMBER_OF_LEVELS - 1);
              LHR_playGameSound(4, 255);
              puts("LHRWARP cheat activated: All levels unlocked!");
              lhrwarp_idx = 0;
            }
          }
          else
            lhrwarp_idx = 0;
        }

        // LHRMAP logic
        static const SDL_Keycode lhrmap_seq[] = {
          SDLK_l, SDLK_h, SDLK_r, SDLK_m, SDLK_a, SDLK_p
        };
        static uint8_t lhrmap_idx = 0;

        if (sym == lhrmap_seq[lhrmap_idx])
        {
          lhrmap_idx++;
          if (lhrmap_idx == 6)
          {
            forceMapReveal = 1;
            LHR_playGameSound(4, 255);
            puts("LHRMAP cheat activated: Map revealed!");
            lhrmap_idx = 0;
          }
        }
        else
          lhrmap_idx = 0;
      }
    }
  }

  sdlMouseButtonState = SDL_GetMouseState(NULL,NULL);

  if (!LHR_mainLoopBody())
    running = 0;

  if (forceMapReveal)
    LHR_currentLevel.mapRevealMask = 0xffff;

  if ( (stateBefore == LHR_GAME_STATE_MENU && LHR_game.state != LHR_GAME_STATE_MENU) ||
       (stateBefore == LHR_GAME_STATE_WIN && LHR_game.state != LHR_GAME_STATE_WIN) ||
       (stateBefore == LHR_GAME_STATE_LOSE && LHR_game.state == LHR_GAME_STATE_MENU) ) {
       
       memcpy(wipe_scr_end, sdlScreen, sizeof(sdlScreen));
       wipe_initMelt(wipe_y, LHR_SCREEN_RESOLUTION_X);
       isWiping = 1;
  }

  SDL_UpdateTexture(texture,NULL,sdlScreen,
    LHR_SCREEN_RESOLUTION_X * sizeof(uint16_t));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer,texture,NULL,NULL);
  SDL_RenderPresent(renderer);

  if (agentSyncMode) {
    SDL_Delay(16); // Pace it to ~60FPS visually so the user can actually watch the gameplay!
  }

  if (autoScreenshot)
  {
    static int screenshotFrameCount = 0;
    screenshotFrameCount++;
    if (screenshotFrameCount >= 60)
    {
      int w, h;
      SDL_GetRendererOutputSize(renderer, &w, &h);
      SDL_Surface *sshot = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
      if (sshot) {
        SDL_RenderReadPixels(renderer, NULL, sshot->format->format, sshot->pixels, sshot->pitch);
        SDL_SaveBMP(sshot, "screenshot.bmp");
        SDL_FreeSurface(sshot);
        puts("Headless screenshot saved to screenshot.bmp!");
      }
      running = 0;
    }
  }
}

uint16_t audioBuff[LHR_SFX_SAMPLE_COUNT];
uint16_t audioPos = 0; // audio position for the next audio buffer fill
uint32_t audioUpdateFrame = 0; // game frame at which audio buffer fill happened

static inline int16_t mixSamples(int16_t sample1, int16_t sample2)
{
  return sample1 + sample2;
}

uint8_t musicOn = 0;
// ^ this has to be init to 0 (not 1), else a few samples get played at start

void audioFillCallback(void *userdata, uint8_t *s, int l)
{
  uint16_t *s16 = (uint16_t *) s;

  for (int i = 0; i < l / 2; ++i)
  {
    s16[i] = musicOn ?
      mixSamples(audioBuff[audioPos], SDL_MUSIC_VOLUME *
      (LHR_getNextMusicSample() - LHR_musicTrackAverages[LHR_MusicState.track]))
      : audioBuff[audioPos];

    audioBuff[audioPos] = 0;
    audioPos = (audioPos < LHR_SFX_SAMPLE_COUNT - 1) ? (audioPos + 1) : 0;
  }

  audioUpdateFrame = LHR_game.frame;
}

void LHR_setMusic(uint8_t value)
{
  switch (value)
  {
    case LHR_MUSIC_TURN_ON: musicOn = 1; break;
    case LHR_MUSIC_TURN_OFF: musicOn = 0; break;
    case LHR_MUSIC_NEXT: LHR_nextMusicTrack(); break;
    default: break;
  }
}

void LHR_playSound(uint8_t soundIndex, uint8_t volume)
{
  uint16_t pos = (audioPos +
    ((LHR_game.frame - audioUpdateFrame) * LHR_MS_PER_FRAME * 8)) %
    LHR_SFX_SAMPLE_COUNT;

  uint16_t volumeScale = 1 << (volume / 37);

  for (int i = 0; i < LHR_SFX_SAMPLE_COUNT; ++i)
  {
    audioBuff[pos] = mixSamples(audioBuff[pos], 
      (128 - LHR_GET_SFX_SAMPLE(soundIndex,i)) * volumeScale);

    pos = (pos < LHR_SFX_SAMPLE_COUNT - 1) ? (pos + 1) : 0;
  }
}

void handleSignal(int signal)
{
  running = 0;
}

int main(int argc, char *argv[])
{
  uint8_t argHelp = 0;
  uint8_t argForceWindow = 0;
  uint8_t argForceFullscreen = 0;
  int targetStartLevel = -1;
  const char *targetLocale = 0;

#ifndef __EMSCRIPTEN__
  argForceFullscreen = 1;
#endif

  for (uint8_t i = 0; i < LHR_KEY_COUNT; ++i)
    webKeyboardState[i] = 0;

  for (uint8_t i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-' && argv[i][1] == 'h' && argv[i][2] == 0)
      argHelp = 1;
    else if (argv[i][0] == '-' && argv[i][1] == 'w' && argv[i][2] == 0)       
      argForceWindow = 1;
    else if (argv[i][0] == '-' && argv[i][1] == 'f' && argv[i][2] == 0)       
      argForceFullscreen = 1;
    else if (strcmp(argv[i], "--lhrwarp") == 0 && i + 1 < argc)
    {
      targetStartLevel = atoi(argv[i+1]);
      i++;
    }
    else if (argv[i][0] == '-' && argv[i][1] == 'l' && argv[i][2] == 0 &&
             i + 1 < argc)
    {
      targetLocale = argv[i + 1];
      i++;
    }
    else if (strcmp(argv[i], "--lhrlocale") == 0 && i + 1 < argc)
    {
      targetLocale = argv[i + 1];
      i++;
    }
    else if (strcmp(argv[i], "--lhrmap") == 0)
    {
      forceMapReveal = 1;
    }
    else if (strcmp(argv[i], "--screenshot") == 0 || strcmp(argv[i], "-s") == 0)
    {
      autoScreenshot = 1;
      argForceWindow = 1;
    }
    else if (strcmp(argv[i], "--agent-sync") == 0)
    {
      agentSyncMode = 1;
      argForceWindow = 1; // Good to force window so user can watch it easily
    }
    else
      puts("SDL: unknown argument"); 
  }

  if (argHelp)
  {
    puts("L'Homme Révolté (SDL), version " LHR_VERSION_STRING "\n");
    puts("L'Homme Révolté is a unique FPS game. Collect weapons and items and destroy");
    puts("robot enemies in your way in order to get to the level finish. Some door are");
    puts("locked and require access cards. Good luck!\n");
    puts("CLI flags:\n");
    puts("-h   print this help and exit");
    puts("-w   force window");
    puts("-f   force fullscreen\n");
    puts("-s / --screenshot  take screenshot after 60 frames and exit");
    puts("-l <locale> set locale id (en_US, pt_BR, tok)");
    puts("--lhrlocale <locale> same as -l\n");
    puts("controls:\n");
    puts("- arrows, numpad, [W] [S] [A] [D] [Q] [E]: movement");
    puts("- mouse: rotation, [LMB] shoot, [RMB] toggle free look");
    puts("- [SPACE]: jump");
    puts("- [J] [RETURN] [CTRL] [LMB]: game A button (shoot, confirm)");
    puts("- [K] [SHIFT]: game B button (cancel, strafe)");
    puts("- [L]: game C button (+ down = menu, + up = jump, ...)");
    puts("- [F]: cycle next/previous weapon");
    puts("- [O] [P] [X] [Y] [Z] [mouse wheel] [mouse middle]: change weapons");
    puts("- [TAB]: map");
    puts("- [ESCAPE]: menu");

    return 0;
  }

  LHR_init();

  if (targetLocale != 0 && !LHR_setLocaleByName(targetLocale))
  {
    puts("SDL: unknown locale, keeping default");
    puts("SDL: available locales:");

    for (uint8_t i = 0; i < LHR_LOCALE_COUNT; ++i)
      puts(LHR_availableLocales[i]->localeId);
  }

  if (targetStartLevel >= 0 && targetStartLevel < LHR_NUMBER_OF_LEVELS)
  {
    LHR_game.save[0] = (LHR_game.save[0] & 0xF0) | (LHR_NUMBER_OF_LEVELS - 1);
    LHR_game.selectedLevel = targetStartLevel;
    LHR_setAndInitLevel(targetStartLevel);
  }

  puts("SDL: initializing SDL");

  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

  window =
    SDL_CreateWindow("L'Homme Révolté", SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, LHR_SCREEN_RESOLUTION_X, LHR_SCREEN_RESOLUTION_Y,
    SDL_WINDOW_SHOWN); 

  renderer = SDL_CreateRenderer(window,-1,0);

  texture =
    SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB565,SDL_TEXTUREACCESS_STREAMING,
    LHR_SCREEN_RESOLUTION_X,LHR_SCREEN_RESOLUTION_Y);

#if LHR_FULLSCREEN
  argForceFullscreen = 1;
#endif

  if (!argForceWindow && argForceFullscreen)
  {
    puts("SDL: setting fullscreen");
    SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
  }

  sdlKeyboardState = SDL_GetKeyboardState(NULL);

  sdlController = SDL_GameControllerOpen(0);

#if !LHR_OS_IS_MALWARE
  signal(SIGINT,handleSignal);
  signal(SIGQUIT,handleSignal);
  signal(SIGTERM,handleSignal);
#endif

  SDL_AudioSpec audioSpec;

  SDL_memset(&audioSpec, 0, sizeof(audioSpec));
  audioSpec.callback = audioFillCallback;
  audioSpec.freq = 8000;
  audioSpec.format = AUDIO_S16;
  audioSpec.channels = 1;
#ifdef __EMSCRIPTEN__
  audioSpec.samples = 1024;
#else
  audioSpec.samples = 256;
#endif

  if (SDL_OpenAudio(&audioSpec,NULL) < 0)
    puts("SDL: could not initialize audio");

  for (int16_t i = 0; i < LHR_SFX_SAMPLE_COUNT; ++i)
    audioBuff[i] = 0;

  SDL_PauseAudio(0);

  running = 1;

  SDL_ShowCursor(0);

  SDL_PumpEvents();
  SDL_GameControllerUpdate();

  SDL_WarpMouseInWindow(window,
    LHR_SCREEN_RESOLUTION_X / 2, LHR_SCREEN_RESOLUTION_Y / 2);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(mainLoopIteration,0,1);
#else
  while (running)
    mainLoopIteration();
#endif

  puts("SDL: freeing SDL");

  SDL_GameControllerClose(sdlController);
  SDL_PauseAudio(1);
  SDL_CloseAudio();
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer); 
  SDL_DestroyWindow(window); 

  puts("SDL: ending");

  return 0;
}
