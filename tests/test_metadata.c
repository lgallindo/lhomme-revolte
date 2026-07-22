#include <stdio.h>
#include <string.h>
#define SFG_PROGRAM_MEMORY
#include "minunit.h"
#include "../core/locale.h"

int tests_run = 0;

static char * test_metadata_struct() {
    uint8_t mock_data[4] = {0, 1, 2, 3};
    SFG_MapImage img = {
        .data = mock_data,
        .width = 2,
        .height = 2,
        .frames = 1,
        .fps = 0
    };
    
    SFG_LevelMeta meta = {
        .introText = "Test",
        .introImage = &img
    };
    
    mu_assert("error, introText != Test", strcmp(meta.introText, "Test") == 0);
    mu_assert("error, introImage is null", meta.introImage != 0);
    mu_assert("error, image width != 2", meta.introImage->width == 2);
    
    return 0;
}

static char * all_tests() {
    mu_run_test(test_metadata_struct);
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
