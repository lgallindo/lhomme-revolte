/**
  @file agent_test.c

  Headless AI Agent Harness Execution Frontend for L'Homme Révolté.
  Exposes tick-by-tick I/O loop: reads actions, advances game logic,
  and outputs JSON telemetry.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LHR_SCREEN_RESOLUTION_X 64
#define LHR_SCREEN_RESOLUTION_Y 32
#define LHR_BACKGROUND_BLUR 0
#define LHR_DITHERED_SHADOW 0
#define LHR_FPS 30

#include "game.h"
#include "sounds.h"
#include "agent_harness.h"

uint8_t screen[LHR_SCREEN_RESOLUTION_X * LHR_SCREEN_RESOLUTION_Y];
static uint16_t currentKeyBitmask = 0;
static int16_t currentMouseDx = 0;
static int16_t currentMouseDy = 0;
static uint32_t simulatedTimeMs = 0;

int8_t LHR_keyPressed(uint8_t key)
{
  if (key >= LHR_KEY_COUNT) return 0;
  return (currentKeyBitmask >> key) & 0x01;
}

void LHR_getMouseOffset(int16_t *x, int16_t *y)
{
  *x = currentMouseDx;
  *y = currentMouseDy;
  currentMouseDx = 0;
  currentMouseDy = 0;
}

uint32_t LHR_getTimeMs(void)
{
  return simulatedTimeMs;
}

void LHR_sleepMs(uint16_t timeMs)
{
  simulatedTimeMs += timeMs;
}

static inline void LHR_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
  if (x < LHR_SCREEN_RESOLUTION_X && y < LHR_SCREEN_RESOLUTION_Y)
    screen[y * LHR_SCREEN_RESOLUTION_X + x] = colorIndex;
}

void LHR_playSound(uint8_t soundIndex, uint8_t volume) {}
void LHR_setMusic(uint8_t value) {}
void LHR_processEvent(uint8_t event, uint8_t data) {}
void LHR_save(uint8_t data[LHR_SAVE_SIZE]) {}
uint8_t LHR_load(uint8_t data[LHR_SAVE_SIZE]) { return 0; }

int main(int argc, char *argv[])
{
  int targetLevel = 0;
  int numTicks = 5;
  uint8_t interactiveMode = 0;

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "--level") == 0 && i + 1 < argc)
    {
      targetLevel = atoi(argv[i + 1]);
      i++;
    }
    else if (strcmp(argv[i], "--ticks") == 0 && i + 1 < argc)
    {
      numTicks = atoi(argv[i + 1]);
      i++;
    }
    else if (strcmp(argv[i], "--interactive") == 0)
    {
      interactiveMode = 1;
    }
  }

  LHR_init();

  if (targetLevel >= 0 && targetLevel < LHR_NUMBER_OF_LEVELS)
  {
    LHR_game.selectedLevel = targetLevel;
    LHR_setAndInitLevel(targetLevel);
    LHR_game.state = LHR_GAME_STATE_PLAYING;
  }

  if (interactiveMode)
  {
    fprintf(stderr, "=== L'Homme Révolté AI Harness (Interactive Mode) ===\n");
    fprintf(stderr, "Format: <key_bitmask_hex> <mouse_dx> <mouse_dy> <step_ticks>\n");
    fprintf(stderr, "Example: '0001 0 0 1' (Press UP for 1 frame)\n\n");

    char buffer[256];
    LHR_agentDumpStateJSON(stdout);

    while (fgets(buffer, sizeof(buffer), stdin))
    {
      LHR_AgentAction act = LHR_agentParseActionLine(buffer);
      currentKeyBitmask = act.keyBitmask;
      currentMouseDx = act.mouseDx;
      currentMouseDy = act.mouseDy;

      for (uint8_t t = 0; t < act.stepTicks; ++t)
      {
        simulatedTimeMs += (1000 / LHR_FPS);
        if (!LHR_mainLoopBody()) break;
      }

      LHR_agentDumpStateJSON(stdout);
    }
  }
  else
  {
    // Batch execution mode: run specified ticks and output initial & final JSON state
    fprintf(stderr, "[Agent Harness] Running level %d for %d ticks...\n", targetLevel, numTicks);
    
    // Initial State Dump
    LHR_agentDumpStateJSON(stdout);

    // Simulate straight walk forward (LHR_KEY_UP is key index 0 -> bitmask 0x0001)
    currentKeyBitmask = 0x0001; 

    for (int t = 0; t < numTicks; ++t)
    {
      simulatedTimeMs += (1000 / LHR_FPS);
      if (!LHR_mainLoopBody()) break;
    }

    // Final State Dump
    LHR_agentDumpStateJSON(stdout);
  }

  return 0;
}
