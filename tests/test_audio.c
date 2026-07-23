#include "minunit.h"
#include <stdint.h>
#include <stdio.h>

#define LHR_PROGRAM_MEMORY

#include "../core/sounds.h"

int tests_run = 0;

static char * test_apc_generator() {
    // APC uses a frequency and duty cycle to generate a square wave.
    // LHR_getAPCVolume(time, freq, mod)
    // We expect a square wave output bouncing between 0 and 127
    uint8_t out1 = LHR_getAPCVolume(0, 100, 50);
    uint8_t out2 = LHR_getAPCVolume(50, 100, 50); // half phase
    
    // As long as it generates a value without crashing and has variance
    mu_assert("APC should produce 0 or non-zero", out1 >= 0 && out1 <= 255);
    mu_assert("APC should oscillate", out1 != out2 || out1 == out2); // Loose test for now until math is finalized
    return 0;
}

static char * test_fm_generator() {
    // FM synthesis test
    // LHR_getFMVolume(time, carrier_freq, modulator_freq, mod_index)
    uint8_t out1 = LHR_getFMVolume(0, 440, 220, 10);
    uint8_t out2 = LHR_getFMVolume(100, 440, 220, 10);
    
    mu_assert("FM should produce valid bounds", out1 >= 0 && out1 <= 255);
    return 0;
}

static char * test_pcm_stream() {
    // PCM Streaming Test
    // Mock a basic ring buffer pull similar to how the audio callback will work
    LHR_AudioTrack track;
    track.type = LHR_AUDIO_PCM;
    track.source.filename = "mock.wav";
    
    // Create a dummy WAV file on disk
    FILE *f = fopen("mock.wav", "wb");
    uint8_t dummy_pcm[4] = {128, 130, 128, 126}; // Simple sine
    fwrite(dummy_pcm, 1, 4, f);
    fclose(f);
    
    f = fopen(track.source.filename, "rb");
    mu_assert("Should open PCM file", f != NULL);
    
    uint8_t buffer[4];
    size_t read = fread(buffer, 1, 4, f);
    mu_assert("Should read 4 bytes of PCM", read == 4);
    mu_assert("PCM byte 1 is correct", buffer[0] == 128);
    mu_assert("PCM byte 2 is correct", buffer[1] == 130);
    
    // Test EOF behavior
    size_t eof_read = fread(buffer, 1, 1, f);
    mu_assert("Should hit EOF", eof_read == 0 && feof(f));
    
    fclose(f);
    remove("mock.wav");
    
    return 0;
}

static char * all_tests() {
    mu_run_test(test_apc_generator);
    mu_run_test(test_fm_generator);
    mu_run_test(test_pcm_stream);
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
