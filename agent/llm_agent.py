import subprocess
import json
import os
import sys

# Try to import the Google GenAI SDK
try:
    from google import genai
    from google.genai import types
except ImportError:
    print("Error: The 'google-genai' package is not installed. Please install it using 'pip install google-genai'.")
    sys.exit(1)

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
- StepTicks: How many frames to execute this action for (e.g., 10 for a quick action, 30 for walking a full block).

Example response if you want to walk forward for 30 frames:
0001 0 0 30

What is your next move? Respond ONLY with the command string.
"""
    return prompt

def play():
    api_key = os.environ.get("GEMINI_API_KEY")
    if not api_key:
        print("Error: GEMINI_API_KEY environment variable not set.")
        sys.exit(1)
        
    client = genai.Client()
    
    print("Launching game process...")
    p = subprocess.Popen(["../build/revolte_sdl", "--agent-sync", "--lhrwarp", "0"], 
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # Skip intro automatically
    print("Skipping intro...")
    p.stdin.write("0000 0 0 100\n")
    p.stdin.flush()

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
            
            # Call Gemini
            response = client.models.generate_content(
                model='gemini-2.5-flash',
                contents=prompt,
                config=types.GenerateContentConfig(
                    temperature=0.2, # Low temperature for more consistent formatting
                )
            )
            
            action = response.text.strip().replace("`", "").strip()
            
            # Ensure it has a newline
            if not action.endswith("\n"):
                action += "\n"
                
            print(f"LLM decided: {action.strip()}")
            
            # Send to game
            p.stdin.write(action)
            p.stdin.flush()
            
        except Exception as e:
            print(f"Agent loop error: {e}")
            break

if __name__ == "__main__":
    play()
