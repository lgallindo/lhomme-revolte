import subprocess
import json
import time

def play():
    # Sequence that just moves and turns in a loop
    actions = [
        "0000 0 0 100\n", # skip intro
        "0001 0 0 30\n", # UP
        "0002 0 0 15\n", # RIGHT
        "0001 0 0 30\n", # UP
        "0008 0 0 15\n", # LEFT
        "0011 0 0 10\n", # UP + SHOOT
    ]
    
    p = subprocess.Popen(["../build/revolte_sdl", "--agent-sync", "--lhrwarp", "0"], 
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    while True:
        for action in actions:
            try:
                p.stdin.write(action)
                p.stdin.flush()
                
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
                    return
                print(f"Agent received frame data! Player position: {json.loads(out)['player']['position']}")
            except Exception as e:
                print(f"Agent error: {e}")
                return

if __name__ == "__main__":
    play()
