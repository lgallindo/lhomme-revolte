# justfile for L'Homme Révolté

default: sdl

all:
    ./tools/make.sh

sdl:
    ./tools/make.sh sdl

sdl_lq:
    ./tools/make.sh sdl_lq

x11:
    ./tools/make.sh x11

ncurses:
    ./tools/make.sh ncurses

saf:
    ./tools/make.sh saf

terminal:
    ./tools/make.sh terminal

csfml:
    ./tools/make.sh csfml

test:
    ./tools/make.sh test

pokitto:
    ./tools/make.sh pokitto

emscripten:
    ./tools/make.sh emscripten

serve: emscripten
    python3 -m http.server 8000 & \
    PID=$! ; \
    sleep 1 ; \
    python3 -m webbrowser http://localhost:8000/revolte.html ; \
    wait $PID
