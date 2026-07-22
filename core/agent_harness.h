/**
  @file agent_harness.h

  AI Agent Harness & Telemetry Subsystem for L'Homme Révolté.
  Provides structured state serialization (JSON) and programmatic action
  ingestion for LLM and RL agents.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef AGENT_HARNESS_H
#define AGENT_HARNESS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "game.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint16_t keyBitmask;
  int16_t mouseDx;
  int16_t mouseDy;
  uint8_t stepTicks;
} LHR_AgentAction;

/**
 * Returns string name of game state enum.
 */
static inline const char* LHR_agentGetStateName(uint8_t state)
{
  switch (state)
  {
    case LHR_GAME_STATE_INIT:        return "INIT";
    case LHR_GAME_STATE_PLAYING:     return "PLAYING";
    case LHR_GAME_STATE_WIN:         return "WIN";
    case LHR_GAME_STATE_LOSE:        return "LOSE";
    case LHR_GAME_STATE_INTRO:       return "INTRO";
    case LHR_GAME_STATE_OUTRO:       return "OUTRO";
    case LHR_GAME_STATE_MAP:         return "MAP";
    case LHR_GAME_STATE_LEVEL_START: return "LEVEL_START";
    case LHR_GAME_STATE_MENU:        return "MENU";
    default:                         return "UNKNOWN";
  }
}

/**
 * Dumps complete structured state JSON to specified file stream.
 */
static inline void LHR_agentDumpStateJSON(FILE *out)
{
  if (!out) out = stdout;

  fprintf(out, "{\n");
  fprintf(out, "  \"frame\": %u,\n", (unsigned int)LHR_game.frame);
  fprintf(out, "  \"state\": \"%s\",\n", LHR_agentGetStateName(LHR_game.state));
  fprintf(out, "  \"selected_level\": %u,\n", (unsigned int)LHR_game.selectedLevel);
  
  // Player status
  fprintf(out, "  \"player\": {\n");
  fprintf(out, "    \"health\": %u,\n", (unsigned int)LHR_player.health);
  fprintf(out, "    \"weapon\": %u,\n", (unsigned int)LHR_player.weapon);
  fprintf(out, "    \"position\": [%d, %d, %d],\n",
          (int)LHR_player.camera.position.x,
          (int)LHR_player.camera.position.y,
          (int)LHR_player.camera.height);
  fprintf(out, "    \"direction_angle\": %d,\n", (int)LHR_player.camera.direction);
  fprintf(out, "    \"direction_vec\": [%d, %d],\n",
          (int)LHR_player.direction.x,
          (int)LHR_player.direction.y);
  fprintf(out, "    \"square_coords\": [%d, %d],\n",
          (int)LHR_player.squarePosition[0],
          (int)LHR_player.squarePosition[1]);
  fprintf(out, "    \"cards_bitmask\": %u,\n", (unsigned int)(LHR_player.cards & 0x07));
  fprintf(out, "    \"ammo\": [%u, %u, %u]\n",
          (unsigned int)LHR_player.ammo[0],
          (unsigned int)LHR_player.ammo[1],
          (unsigned int)LHR_player.ammo[2]);
  fprintf(out, "  },\n");

  // Active monsters count & array
  uint16_t activeMonsters = 0;
  for (uint8_t i = 0; i < LHR_MAX_MONSTERS; ++i)
  {
    if (LHR_currentLevel.monsterRecords[i].health > 0 &&
        LHR_MR_STATE(LHR_currentLevel.monsterRecords[i]) != LHR_MONSTER_STATE_INACTIVE &&
        LHR_MR_STATE(LHR_currentLevel.monsterRecords[i]) != LHR_MONSTER_STATE_DEAD)
    {
      activeMonsters++;
    }
  }

  fprintf(out, "  \"monsters\": {\n");
  fprintf(out, "    \"active_count\": %u,\n", activeMonsters);
  fprintf(out, "    \"entities\": [\n");

  uint16_t printedMonsters = 0;
  for (uint8_t i = 0; i < LHR_MAX_MONSTERS; ++i)
  {
    if (LHR_currentLevel.monsterRecords[i].health > 0 &&
        LHR_MR_STATE(LHR_currentLevel.monsterRecords[i]) != LHR_MONSTER_STATE_INACTIVE &&
        LHR_MR_STATE(LHR_currentLevel.monsterRecords[i]) != LHR_MONSTER_STATE_DEAD)
    {
      if (printedMonsters > 0) fprintf(out, ",\n");
      fprintf(out, "      {\"id\": %u, \"type\": %u, \"health\": %u, \"coords\": [%u, %u], \"state\": %u}",
              i,
              LHR_MR_TYPE(LHR_currentLevel.monsterRecords[i]),
              LHR_currentLevel.monsterRecords[i].health,
              LHR_currentLevel.monsterRecords[i].coords[0],
              LHR_currentLevel.monsterRecords[i].coords[1],
              LHR_MR_STATE(LHR_currentLevel.monsterRecords[i]));
      printedMonsters++;
    }
  }
  fprintf(out, "    ]\n");
  fprintf(out, "  },\n");
  
  // Add screen ASCII visualization
  extern uint8_t screen[]; // from agent_test.c
  fprintf(out, "  \"screen_ascii\": [\n");
  const char colors[9] = " .,-;imX";
  for (uint8_t y = 0; y < 32; ++y)
  {
    fprintf(out, "    \"");
    for (uint8_t x = 0; x < 64; ++x)
    {
      int srcY = y * LHR_SCREEN_RESOLUTION_Y / 32;
      int srcX = x * LHR_SCREEN_RESOLUTION_X / 64;
      uint8_t colorIdx = screen[srcY * LHR_SCREEN_RESOLUTION_X + srcX];
      char c = (colorIdx != 7) ? colors[colorIdx % 8] : '@';
      if (c == '\\' || c == '\"') fprintf(out, "\\%c", c);
      else fprintf(out, "%c", c);
    }
    fprintf(out, "\"%s\n", (y < 31) ? "," : "");
  }
  fprintf(out, "  ]\n");

  fprintf(out, "}\n");
  fflush(out);
}

/**
 * Parses action input string into LHR_AgentAction struct.
 * Expected input format: "KEYS_MASK MOUSE_DX MOUSE_DY TICKS" or JSON
 */
static inline LHR_AgentAction LHR_agentParseActionLine(const char *line)
{
  LHR_AgentAction action;
  action.keyBitmask = 0;
  action.mouseDx = 0;
  action.mouseDy = 0;
  action.stepTicks = 1;

  if (!line || strlen(line) == 0) return action;

  unsigned int kMask = 0;
  int dx = 0, dy = 0, ticks = 1;
  if (sscanf(line, "%x %d %d %d", &kMask, &dx, &dy, &ticks) >= 1)
  {
    action.keyBitmask = (uint16_t)kMask;
    action.mouseDx = (int16_t)dx;
    action.mouseDy = (int16_t)dy;
    action.stepTicks = (ticks > 0) ? (uint8_t)ticks : 1;
  }
  return action;
}

#ifdef __cplusplus
}
#endif

#endif // AGENT_HARNESS_H
