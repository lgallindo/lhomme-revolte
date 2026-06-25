#ifndef WIPE_EFFECT_H
#define WIPE_EFFECT_H

#include <stdint.h>
#include <stdlib.h>

/**
 * Initializes the randomized Y-offsets for the screen melt effect.
 * @param wipe_y An array of size `width` to hold the offsets.
 * @param width The width of the screen.
 */
static inline void wipe_initMelt(int* wipe_y, int width) {
    wipe_y[0] = -(rand() % 16);
    for (int i = 1; i < width; i++) {
        wipe_y[i] = wipe_y[i-1] + (rand() % 3) - 1;
        if (wipe_y[i] > 0) wipe_y[i] = 0;
        else if (wipe_y[i] == -16) wipe_y[i] = -15;
    }
}

/**
 * Performs a single frame of the melt effect.
 * @param screen The active framebuffer being pushed to the display.
 * @param start_scr The outgoing frame.
 * @param end_scr The incoming frame.
 * @param wipe_y The state array containing Y-offsets for each column.
 * @param width The screen width.
 * @param height The screen height.
 * @return 1 if the melt is complete, 0 if it is still ongoing.
 */
static inline int wipe_doMelt(uint16_t* screen, const uint16_t* start_scr, const uint16_t* end_scr, int* wipe_y, int width, int height) {
    int done = 1;
    for (int i = 0; i < width; i++) {
        if (wipe_y[i] < 0) {
            wipe_y[i] += 2; // initial drop delay speed
            done = 0;
        } else if (wipe_y[i] < height) {
            int dy = (wipe_y[i] < 16) ? wipe_y[i] + 1 : 8; // acceleration
            if (wipe_y[i] + dy >= height) dy = height - wipe_y[i];
            wipe_y[i] += dy;
            
            // Draw the incoming frame (stationary, revealing from top)
            for (int y = 0; y < wipe_y[i]; y++) {
                screen[y * width + i] = end_scr[y * width + i];
            }
            // Draw the outgoing frame (falling downwards)
            for (int y = wipe_y[i]; y < height; y++) {
                screen[y * width + i] = start_scr[(y - wipe_y[i]) * width + i];
            }
            done = 0;
        }
    }
    return done;
}

#endif // WIPE_EFFECT_H
