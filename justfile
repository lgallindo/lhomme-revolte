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
    cp -f revolte.html index.html

emscripten_pt_br:
    ./tools/make.sh emscripten_pt_br
    cp -f revolte_pt_br.html index-pt_br.html

serve port="8000": emscripten
    python3 -m http.server {{port}} & \
    PID=$! ; \
    sleep 1 ; \
    python3 -m webbrowser http://localhost:{{port}}/index.html ; \
    wait $PID

serve-pt-br port="8000": emscripten_pt_br
    python3 -m http.server {{port}} & \
    PID=$! ; \
    sleep 1 ; \
    python3 -m webbrowser http://localhost:{{port}}/index-pt_br.html ; \
    wait $PID
