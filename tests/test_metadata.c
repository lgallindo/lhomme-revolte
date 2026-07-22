#include <stdio.h>
#include <string.h>
#define LHR_PROGRAM_MEMORY
#include "minunit.h"
#include "../core/locale.h"

int tests_run = 0;

static char * test_metadata_struct() {
    uint8_t mock_data[4] = {0, 1, 2, 3};
    LHR_MapImage img = {
        .data = mock_data,
        .width = 2,
        .height = 2,
        .frames = 1,
        .fps = 0
    };
    
    LHR_LevelMeta meta = {
        .introText = "Test",
        .introImage = &img
    };
    
    mu_assert("error, introText != Test", strcmp(meta.introText, "Test") == 0);
    mu_assert("error, introImage is null", meta.introImage != 0);
    mu_assert("error, image width != 2", meta.introImage->width == 2);
    
    return 0;
}

static char * test_frame_calc() {
    LHR_MapImage static_img = { .frames = 1, .fps = 10 };
    LHR_MapImage anim_img = { .frames = 4, .fps = 10 }; // 10 frames per second means each frame is 100ms
    
    // Static image should always return 0
    mu_assert("static img failed", LHR_getMapImageFrame(&static_img, 500) == 0);
    
    // Animated image tests
    // 0ms = frame 0
    mu_assert("anim 0ms", LHR_getMapImageFrame(&anim_img, 0) == 0);
    // 50ms = 0.5 frames = frame 0
    mu_assert("anim 50ms", LHR_getMapImageFrame(&anim_img, 50) == 0);
    // 100ms = 1 frame = frame 1
    mu_assert("anim 100ms", LHR_getMapImageFrame(&anim_img, 100) == 1);
    // 250ms = 2.5 frames = frame 2
    mu_assert("anim 250ms", LHR_getMapImageFrame(&anim_img, 250) == 2);
    // 400ms = 4 frames = frame 0 (wrap around modulo 4)
    mu_assert("anim 400ms wrap", LHR_getMapImageFrame(&anim_img, 400) == 0);
    
    return 0;
}

static char * all_tests() {
    mu_run_test(test_metadata_struct);
    mu_run_test(test_frame_calc);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    
    return result != 0;
}
