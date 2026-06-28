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
    cp -f docs/revolte.html index-en_us.html
    cp -f docs/revolte.html docs/index-en_us.html
    cp -f frontends/web/index.html docs/index.html
    cp -f frontends/web/index.css docs/index.css
    cp -f frontends/web/revolte.css docs/revolte.css
    cp -f frontends/web/revolte_shell.js docs/revolte_shell.js

emscripten_pt_br:
    ./tools/make.sh emscripten_pt_br
    cp -f docs/revolte_pt_br.html index-pt_br.html
    cp -f docs/revolte_pt_br.html docs/index-pt_br.html
    cp -f frontends/web/index.html docs/index.html
    cp -f frontends/web/index.css docs/index.css
    cp -f frontends/web/revolte.css docs/revolte.css
    cp -f frontends/web/revolte_shell.js docs/revolte_shell.js

emscripten_tok:
    ./tools/make.sh emscripten_tok
    cp -f docs/revolte_tok.html index-tok.html
    cp -f docs/revolte_tok.html docs/index-tok.html
    cp -f frontends/web/index.html docs/index.html
    cp -f frontends/web/index.css docs/index.css
    cp -f frontends/web/revolte.css docs/revolte.css
    cp -f frontends/web/revolte_shell.js docs/revolte_shell.js

serve port="8000": emscripten
    @python3 -c "import socket; s = socket.socket(); s.bind(('127.0.0.1', {{port}}))" 2>/dev/null || { echo "Error: Port {{port}} is already in use. Try specifying a different port (e.g. 'just serve 8001')."; exit 1; }
    python3 -m http.server {{port}} --directory docs & \
    PID=$! ; \
    sleep 1 ; \
    python3 -m webbrowser http://localhost:{{port}}/index.html ; \
    wait $PID

serve-pt-br port="8000": emscripten_pt_br
    @python3 -c "import socket; s = socket.socket(); s.bind(('127.0.0.1', {{port}}))" 2>/dev/null || { echo "Error: Port {{port}} is already in use. Try specifying a different port (e.g. 'just serve-pt-br 8001')."; exit 1; }
    python3 -m http.server {{port}} --directory docs & \
    PID=$! ; \
    sleep 1 ; \
    python3 -m webbrowser http://localhost:{{port}}/index-pt_br.html ; \
    wait $PID

serve-tok port="8000": emscripten_tok
    @python3 -c "import socket; s = socket.socket(); s.bind(('127.0.0.1', {{port}}))" 2>/dev/null || { echo "Error: Port {{port}} is already in use. Try specifying a different port (e.g. 'just serve-tok 8001')."; exit 1; }
    python3 -m http.server {{port}} --directory docs & \
    PID=$! ; \
    sleep 1 ; \
    python3 -m webbrowser http://localhost:{{port}}/index-tok.html ; \
    wait $PID
