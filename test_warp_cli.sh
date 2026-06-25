#!/bin/bash
# Integration test for --lhrwarp CLI parameter

echo "Testing SDL --lhrwarp 7..."
timeout 1 ./revolte_sdl --lhrwarp 7 > sdl_test.log 2>&1
if grep -q "setting and initializing level" sdl_test.log; then
    echo "[OK] SDL CLI test passed."
else
    echo "[FAIL] SDL CLI test failed."
    cat sdl_test.log
    exit 1
fi

echo "Testing X11 --lhrwarp 7..."
timeout 1 ./revolte_x11 --lhrwarp 7 > x11_test.log 2>&1
if grep -q "setting and initializing level" x11_test.log; then
    echo "[OK] X11 CLI test passed."
else
    echo "[FAIL] X11 CLI test failed."
    cat x11_test.log
    exit 1
fi

echo "All CLI tests passed successfully!"
exit 0
