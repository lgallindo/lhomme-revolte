#include "/home/lgms/code/research/Unity/src/unity.h"
#include "wipe_effect.h"

void setUp(void) {
    // Unity requires this, runs before each test
}

void tearDown(void) {
    // Unity requires this, runs after each test
}

void test_wipe_initMelt_should_populate_array_with_negatives(void) {
    int width = 100;
    int wipe_y[100];
    wipe_initMelt(wipe_y, width);
    
    // The DOOM algorithm guarantees starting Y is between -16 and 0
    for (int i = 0; i < width; i++) {
        TEST_ASSERT_LESS_OR_EQUAL_INT32(0, wipe_y[i]);
        TEST_ASSERT_GREATER_OR_EQUAL_INT32(-16, wipe_y[i]);
    }
}

void test_wipe_doMelt_should_finish_eventually(void) {
    int width = 10;
    int height = 10;
    uint16_t screen[100] = {0};
    uint16_t start_scr[100] = {0};
    uint16_t end_scr[100] = {1}; // dummy value
    int wipe_y[10];
    
    wipe_initMelt(wipe_y, width);
    
    int done = 0;
    int iterations = 0;
    
    // Simulate game loop ticking
    while (!done && iterations < 100) {
        done = wipe_doMelt(screen, start_scr, end_scr, wipe_y, width, height);
        iterations++;
    }
    
    // It should complete well before 100 frames for a 10px height
    TEST_ASSERT_TRUE_MESSAGE(done, "Melt did not complete within expected ticks.");
    TEST_ASSERT_LESS_THAN(100, iterations);
    
    // Once done, the screen should exactly match the end_scr
    for (int i = 0; i < 100; i++) {
        TEST_ASSERT_EQUAL_UINT16(end_scr[i], screen[i]);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_wipe_initMelt_should_populate_array_with_negatives);
    RUN_TEST(test_wipe_doMelt_should_finish_eventually);
    return UNITY_END();
}
