import subprocess
import json
import os
import sys
import requests
import time
import argparse

# Local llama-server endpoint (OpenAI compatible)
LLAMA_API_URL = "http://127.0.0.1:8080/v1/chat/completions"

def build_prompt(state):
    """Constructs a prompt for the LLM based on the game state."""
    screen = "\n".join(state.get("screen_ascii", []))
    player = state.get("player", {})
    health = player.get("health", 0)
    ammo = player.get("ammo", [0, 0, 0])
    
    prompt = f"""You are an autonomous AI playing a retro 3D raycasting game (similar to DOOM). 
Your goal is to survive, navigate the level, and shoot enemies.

CURRENT GAME STATE:
Health: {health}
Ammo: {ammo}

VISUAL TELEMETRY (64x32 ASCII downsample of your view):
{screen}

(Note: '@' typically represents walls/obstacles, and spaces are open areas or floors. Enemies are usually distinct characters.)

AVAILABLE CONTROLS (Hex Bitmask):
0001 : Move Forward
0004 : Move Backward
0008 : Turn/Strafe Left
0002 : Turn/Strafe Right
0010 : Shoot Weapon
0011 : Move Forward + Shoot
0020 : Jump

INSTRUCTIONS:
You must respond with EXACTLY ONE LINE containing four space-separated values:
<KeyBitmask> <MouseDx> <MouseDy> <StepTicks>

- KeyBitmask: The 4-digit hex code from the list above.
- MouseDx: 0 (we use keyboard turning for now)
- MouseDy: 0
- StepTicks: How many frames to execute this action for (e.g., 5 for a quick action, 15 for walking).

Example response if you want to walk forward for 15 frames:
0001 0 0 15

What is your next move? Respond ONLY with the command string.
"""
    return prompt

def call_local_llama(prompt):
    """Calls the local llama.cpp server and returns the raw command."""
    payload = {
        "messages": [
            {"role": "system", "content": "You are a rigid game-playing agent. You output nothing but the 4 parameter command string."},
            {"role": "user", "content": prompt}
        ],
        "temperature": 0.1, # Keep it highly deterministic
        "max_tokens": 20
    }
    
    try:
        response = requests.post(LLAMA_API_URL, json=payload, timeout=30)
        response.raise_for_status()
        data = response.json()
        return data["choices"][0]["message"]["content"].strip()
    except Exception as e:
        print(f"Failed to communicate with llama-server: {e}")
        return None

def play(record_file=None):
    print("Launching game process...")
    p = subprocess.Popen(["../build/revolte_sdl", "--agent-sync", "--lhrwarp", "0"], 
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # Open demo recording file if requested
    f_record = None
    if record_file:
        f_record = open(record_file, "w")
        print(f"Recording demo to {record_file}")
    
    # Skip intro automatically
    print("Skipping intro...")
    intro_cmd = "0000 0 0 100\n"
    p.stdin.write(intro_cmd)
    p.stdin.flush()
    if f_record:
        f_record.write(intro_cmd)

    while True:
        try:
            # Wait for json object
            out = ""
            while True:
                line = p.stdout.readline()
                if not line:
                    break
                if not out and line.strip() != "{":
                    continue # Skip SDL logs
                out += line
                if line.strip() == "}":
                    break
            
            if not out:
                print("Game process ended.")
                break
                
            state = json.loads(out)
            
            print(f"--- FRAME {state.get('frame')} ---")
            prompt = build_prompt(state)
            
            start_time = time.time()
            print("Thinking...")
            
            # Call Local LLM
            action = call_local_llama(prompt)
            if not action:
                print("LLM failed to produce an action, skipping frame.")
                action = "0000 0 0 1"
            
            action = action.replace("`", "").strip()
            
            # Ensure it has a newline
            if not action.endswith("\n"):
                action += "\n"
                
            elapsed = time.time() - start_time
            print(f"LLM decided: {action.strip()} (Took {elapsed:.2f}s)")
            
            # Record the command
            if f_record:
                f_record.write(action)
                f_record.flush()
            
            # Send to game
            p.stdin.write(action)
            p.stdin.flush()
            
        except Exception as e:
            print(f"Agent loop error: {e}")
            break
            
    if f_record:
        f_record.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run L'Homme Revolte agent via local llama-server")
    parser.add_argument("--record", type=str, help="File to record the demo to (e.g., demo.txt)")
    args = parser.parse_args()
    
    play(record_file=args.record)
