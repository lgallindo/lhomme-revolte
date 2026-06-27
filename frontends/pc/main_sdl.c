/**
  @file main_sdl.c

  This is an SDL2 implementation of the game front end. It can be used to
  compile a native executable or a transpiled JS browser version with
  emscripten.

  This frontend is not strictly minimal, it could be reduced a lot. If you want
  a learning example of frontend, look at another, simpler one, e.g. terminal.

  To compile with emscripten run:

  emcc ./main_sdl.c -s USE_SDL=2 -O3 --shell-file HTMLshell.html -o game.html


  Released under CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/)
  plus a waiver of all other intellectual property. The goal of this work is to
  be and remain completely in the public domain forever, available for any use
  whatsoever.
*/

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__APPLE__)
  #define SFG_OS_IS_MALWARE 1
#endif

// #define SFG_START_LEVEL 1
// #define SFG_QUICK_WIN 1
// #define SFG_IMMORTAL 1
// #define SFG_ALL_LEVELS 1
// #define SFG_UNLOCK_DOOR 1
// #define SFG_REVEAL_MAP 1
// #define SFG_INFINITE_AMMO 1
// #define SFG_TIME_MULTIPLIER 512
// #define SFG_CPU_LOAD(percent) printf("CPU load: %d%\n",percent);
// #define GAME_LQ

#ifndef __EMSCRIPTEN__
  #ifndef GAME_LQ
    // higher quality
    #define SFG_FPS 60
    #define SFG_LOG(str) puts(str);
    #define SFG_SCREEN_RESOLUTION_X 700
    #define SFG_SCREEN_RESOLUTION_Y 512
    #define SFG_DITHERED_SHADOW 1
    #define SFG_DIMINISH_SPRITES 1
    #define SFG_HEADBOB_SHEAR (-1 * SFG_SCREEN_RESOLUTION_Y / 80)
    #define SFG_BACKGROUND_BLUR 1
  #else
    // lower quality
    #define SFG_FPS 35
    #define SFG_SCREEN_RESOLUTION_X 640
    #define SFG_SCREEN_RESOLUTION_Y 480
    #define SFG_RAYCASTING_SUBSAMPLE 2
    #define SFG_RESOLUTION_SCALEDOWN 2
    #define SFG_LOG(str) puts(str);
    #define SFG_DIMINISH_SPRITES 0
    #define SFG_DITHERED_SHADOW 0
    #define SFG_BACKGROUND_BLUR 0
    #define SFG_RAYCASTING_MAX_STEPS 18
    #define SFG_RAYCASTING_MAX_HITS 8
  #endif
#else
  // emscripten
  #define SFG_FPS 35
  #define SFG_SCREEN_RESOLUTION_X 512
  #define SFG_SCREEN_RESOLUTION_Y 320
  #define SFG_CAN_EXIT 0
  #define SFG_RESOLUTION_SCALEDOWN 2
  #define SFG_DITHERED_SHADOW 1
  #define SFG_BACKGROUND_BLUR 0
  #define SFG_RAYCASTING_MAX_STEPS 18
  #define SFG_RAYCASTING_MAX_HITS 8

  #include <emscripten.h>
#endif

/*
  SDL is easier to play thanks to nice controls so make the player take full
  damage to make it a bit harder.
*/
#define SFG_PLAYER_DAMAGE_MULTIPLIER 1024

#define SDL_MUSIC_VOLUME 16

#define SDL_ANALOG_DIVIDER 1024

#if !SFG_OS_IS_MALWARE
  #include <signal.h>
#endif

#define SDL_MAIN_HANDLED 1

#define SDL_DISABLE_IMMINTRIN_H 1

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "game.h"
#include "sounds.h"
#include "wipe_effect.h"

const uint8_t *sdlKeyboardState;
uint8_t webKeyboardState[SFG_KEY_COUNT];
uint8_t sdlMouseButtonState = 0;
int8_t sdlMouseWheelState = 0;
SDL_GameController *sdlController;

uint16_t sdlScreen[SFG_SCREEN_RESOLUTION_X * SFG_SCREEN_RESOLUTION_Y]; // RGB565

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

// now implement the L'Homme Révolté API functions (SFG_*)

void SFG_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  sdlScreen[y * SFG_SCREEN_RESOLUTION_X + x] = paletteRGB565[colorIndex];
}

uint32_t SFG_getTimeMs(void)
{
  return SDL_GetTicks();
}

void SFG_save(uint8_t data[SFG_SAVE_SIZE])
{
  FILE *f = fopen(SFG_SAVE_FILE_PATH,"wb");

  puts("SDL: opening and writing save file");

  if (f == NULL)
  {
    puts("SDL: could not open the file!");
    return;
  }

  fwrite(data,1,SFG_SAVE_SIZE,f);

  fclose(f);
}

uint8_t SFG_load(uint8_t data[SFG_SAVE_SIZE])
{
#ifndef __EMSCRIPTEN__
  FILE *f = fopen(SFG_SAVE_FILE_PATH,"rb");

  puts("SDL: opening and reading save file");

  if (f == NULL)
  {
    puts("SDL: no save file to open");
  }
  else
  {
    if (fread(data, 1, SFG_SAVE_SIZE, f) != SFG_SAVE_SIZE)
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

void SFG_sleepMs(uint16_t timeMs)
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

void SFG_getMouseOffset(int16_t *x, int16_t *y)
{
#ifndef __EMSCRIPTEN__
  if (mouseMoved)
  {
    int mX, mY;

    SDL_GetMouseState(&mX,&mY);

    *x = mX - SFG_SCREEN_RESOLUTION_X / 2;
    *y = mY - SFG_SCREEN_RESOLUTION_Y / 2;

    SDL_WarpMouseInWindow(window,
      SFG_SCREEN_RESOLUTION_X / 2, SFG_SCREEN_RESOLUTION_Y / 2);
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

void SFG_processEvent(uint8_t event, uint8_t data)
{
}

int8_t SFG_keyPressed(uint8_t key)
{
  if (webKeyboardState[key]) // this only takes effect in the web version 
    return 1;

  #define k(x) sdlKeyboardState[SDL_SCANCODE_ ## x]
  #define b(x) ((sdlController != NULL) && \
    SDL_GameControllerGetButton(sdlController,SDL_CONTROLLER_BUTTON_ ## x))

  switch (key)
  {
    case SFG_KEY_UP: return k(UP) || k(W) || k(KP_8) || b(DPAD_UP); break;
    case SFG_KEY_RIGHT: 
      return k(RIGHT) || k(E) || k(KP_6) || b(DPAD_RIGHT); break;
    case SFG_KEY_DOWN: 
      return k(DOWN) || k(S) || k(KP_5) || k(KP_2) || b(DPAD_DOWN); break;
    case SFG_KEY_LEFT: return k(LEFT) || k(Q) || k(KP_4) || b(DPAD_LEFT); break;
    case SFG_KEY_A: return k(J) || k(RETURN) || k(LCTRL) || k(RCTRL) || b(X) ||
      b(RIGHTSTICK) || (sdlMouseButtonState & SDL_BUTTON_LMASK); break;
    case SFG_KEY_B: return k(K) || k(LSHIFT) || b(B); break;
    case SFG_KEY_C: return k(L) || b(Y); break;
    case SFG_KEY_JUMP: return k(SPACE) || b(A); break;
    case SFG_KEY_STRAFE_LEFT: return k(A) || k(KP_7); break;
    case SFG_KEY_STRAFE_RIGHT: return k(D) || k(KP_9); break;
    case SFG_KEY_MAP: return k(TAB) || b(BACK); break;
    case SFG_KEY_CYCLE_WEAPON: return k(F) ||
      (sdlMouseButtonState & SDL_BUTTON_MMASK); break;
    case SFG_KEY_TOGGLE_FREELOOK: return b(LEFTSTICK) ||
      (sdlMouseButtonState & SDL_BUTTON_RMASK); break;
    case SFG_KEY_MENU: return k(ESCAPE) || b(START); break;
    case SFG_KEY_NEXT_WEAPON:
      if (k(P) || k(X) || b(RIGHTSHOULDER))
        return 1;

#define checkMouse(cmp)\
  if (sdlMouseWheelState cmp 0) { sdlMouseWheelState = 0; return 1; }

      checkMouse(>)
        
      return 0;
      break;

    case SFG_KEY_PREVIOUS_WEAPON:
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

static uint16_t wipe_scr_start[SFG_SCREEN_RESOLUTION_X * SFG_SCREEN_RESOLUTION_Y];
static uint16_t wipe_scr_end[SFG_SCREEN_RESOLUTION_X * SFG_SCREEN_RESOLUTION_Y];
static int wipe_y[SFG_SCREEN_RESOLUTION_X];

void mainLoopIteration(void)
{
  SDL_Event event;

  uint8_t stateBefore = SFG_game.state;
  if (stateBefore == SFG_GAME_STATE_MENU || stateBefore == SFG_GAME_STATE_WIN || stateBefore == SFG_GAME_STATE_LOSE) {
      memcpy(wipe_scr_start, sdlScreen, sizeof(sdlScreen));
  }

#ifdef __EMSCRIPTEN__
  // hack, without it sound won't work because of shitty browser audio policies

  if (SFG_game.frame % 512 == 0)
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
      mouseMoved = 1; 
    else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
    {
      SDL_Keycode sym = event.key.keysym.sym;
      if (sym != SDLK_LSHIFT && sym != SDLK_RSHIFT && sym != SDLK_CAPSLOCK)
      {
        // LHRWARP logic
        if (SFG_game.state == SFG_GAME_STATE_MENU)
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
              SFG_game.save[0] = (SFG_game.save[0] & 0xF0) | (SFG_NUMBER_OF_LEVELS - 1);
              SFG_playGameSound(4, 255);
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
            SFG_playGameSound(4, 255);
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

  if (!SFG_mainLoopBody())
    running = 0;

  if (forceMapReveal)
    SFG_currentLevel.mapRevealMask = 0xffff;

  if ( (stateBefore == SFG_GAME_STATE_MENU && SFG_game.state != SFG_GAME_STATE_MENU) ||
       (stateBefore == SFG_GAME_STATE_WIN && SFG_game.state != SFG_GAME_STATE_WIN) ||
       (stateBefore == SFG_GAME_STATE_LOSE && SFG_game.state == SFG_GAME_STATE_MENU) ) {
       
       memcpy(wipe_scr_end, sdlScreen, sizeof(sdlScreen));
       wipe_initMelt(wipe_y, SFG_SCREEN_RESOLUTION_X);
       
       int done = 0;
       while (!done && running) {
          done = wipe_doMelt(sdlScreen, wipe_scr_start, wipe_scr_end, wipe_y, SFG_SCREEN_RESOLUTION_X, SFG_SCREEN_RESOLUTION_Y);
          while (SDL_PollEvent(&event)) {
             if (event.type == SDL_QUIT) { running = 0; done = 1; }
          }
          SDL_UpdateTexture(texture,NULL,sdlScreen, SFG_SCREEN_RESOLUTION_X * sizeof(uint16_t));
          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer,texture,NULL,NULL);
          SDL_RenderPresent(renderer);
          SDL_Delay(10);
       }
  }

  SDL_UpdateTexture(texture,NULL,sdlScreen,
    SFG_SCREEN_RESOLUTION_X * sizeof(uint16_t));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer,texture,NULL,NULL);
  SDL_RenderPresent(renderer);
}

uint16_t audioBuff[SFG_SFX_SAMPLE_COUNT];
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
      (SFG_getNextMusicSample() - SFG_musicTrackAverages[SFG_MusicState.track]))
      : audioBuff[audioPos];

    audioBuff[audioPos] = 0;
    audioPos = (audioPos < SFG_SFX_SAMPLE_COUNT - 1) ? (audioPos + 1) : 0;
  }

  audioUpdateFrame = SFG_game.frame;
}

void SFG_setMusic(uint8_t value)
{
  switch (value)
  {
    case SFG_MUSIC_TURN_ON: musicOn = 1; break;
    case SFG_MUSIC_TURN_OFF: musicOn = 0; break;
    case SFG_MUSIC_NEXT: SFG_nextMusicTrack(); break;
    default: break;
  }
}

void SFG_playSound(uint8_t soundIndex, uint8_t volume)
{
  uint16_t pos = (audioPos +
    ((SFG_game.frame - audioUpdateFrame) * SFG_MS_PER_FRAME * 8)) %
    SFG_SFX_SAMPLE_COUNT;

  uint16_t volumeScale = 1 << (volume / 37);

  for (int i = 0; i < SFG_SFX_SAMPLE_COUNT; ++i)
  {
    audioBuff[pos] = mixSamples(audioBuff[pos], 
      (128 - SFG_GET_SFX_SAMPLE(soundIndex,i)) * volumeScale);

    pos = (pos < SFG_SFX_SAMPLE_COUNT - 1) ? (pos + 1) : 0;
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

  for (uint8_t i = 0; i < SFG_KEY_COUNT; ++i)
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
    else
      puts("SDL: unknown argument"); 
  }

  if (argHelp)
  {
    puts("L'Homme Révolté (SDL), version " SFG_VERSION_STRING "\n");
    puts("L'Homme Révolté is a unique FPS game. Collect weapons and items and destroy");
    puts("robot enemies in your way in order to get to the level finish. Some door are");
    puts("locked and require access cards. Good luck!\n");
    puts("CLI flags:\n");
    puts("-h   print this help and exit");
    puts("-w   force window");
    puts("-f   force fullscreen\n");
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

  SFG_init();

  if (targetLocale != 0 && !SFG_setLocaleByName(targetLocale))
  {
    puts("SDL: unknown locale, keeping default");
    puts("SDL: available locales:");

    for (uint8_t i = 0; i < SFG_LOCALE_COUNT; ++i)
      puts(SFG_availableLocales[i]->localeId);
  }

  if (targetStartLevel >= 0 && targetStartLevel < SFG_NUMBER_OF_LEVELS)
  {
    SFG_game.save[0] = (SFG_game.save[0] & 0xF0) | (SFG_NUMBER_OF_LEVELS - 1);
    SFG_game.selectedLevel = targetStartLevel;
    SFG_setAndInitLevel(targetStartLevel);
  }

  puts("SDL: initializing SDL");

  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

  window =
    SDL_CreateWindow("L'Homme Révolté", SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, SFG_SCREEN_RESOLUTION_X, SFG_SCREEN_RESOLUTION_Y,
    SDL_WINDOW_SHOWN); 

  renderer = SDL_CreateRenderer(window,-1,0);

  texture =
    SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB565,SDL_TEXTUREACCESS_STATIC,
    SFG_SCREEN_RESOLUTION_X,SFG_SCREEN_RESOLUTION_Y);

#if SFG_FULLSCREEN
  argForceFullscreen = 1;
#endif

  if (!argForceWindow && argForceFullscreen)
  {
    puts("SDL: setting fullscreen");
    SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
  }

  sdlKeyboardState = SDL_GetKeyboardState(NULL);

  sdlController = SDL_GameControllerOpen(0);

#if !SFG_OS_IS_MALWARE
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

  for (int16_t i = 0; i < SFG_SFX_SAMPLE_COUNT; ++i)
    audioBuff[i] = 0;

  SDL_PauseAudio(0);

  running = 1;

  SDL_ShowCursor(0);

  SDL_PumpEvents();
  SDL_GameControllerUpdate();

  SDL_WarpMouseInWindow(window,
    SFG_SCREEN_RESOLUTION_X / 2, SFG_SCREEN_RESOLUTION_Y / 2);

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
