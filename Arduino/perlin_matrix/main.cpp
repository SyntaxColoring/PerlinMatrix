#include <cmath>
#include <cstdint>

// https://forum.arduino.cc/t/how-to-avoid-the-quirks-of-the-ide-sketch-file-pre-preprocessing/262594/2
#include <Arduino.h>

#include <Adafruit_Protomatter.h> // For RGB matrix

#include "perlin.hpp"

// In pixels:
const static unsigned int HEIGHT = 32;
const static unsigned int WIDTH = 32;

const static unsigned int BIT_DEPTH = 4;

// For 32-pixel tall matrices.
uint8_t addr_pins[] = {17, 18, 19, 20};

// For 64-pixel tall matrices.
// const uint8_t addr_pins[] = {17, 18, 19, 20, 21};

// Remaining pins are the same for all matrix sizes. These values
// are for MatrixPortal M4. See "simple" example for other boards.
uint8_t rgb_pins[]  = {7, 8, 9, 10, 11, 12};
const static uint8_t clock_pin   = 14;
const static uint8_t latch_pin   = 15;
const static uint8_t oe_pin      = 16;

const static unsigned int MAX_FPS = 45;

Adafruit_Protomatter matrix(
    WIDTH,
    BIT_DEPTH,
    1, // rgbCount
    rgb_pins,
    sizeof(addr_pins),
    addr_pins,
    clock_pin,
    latch_pin,
    oe_pin,
    false // doubleBuffer
);

uint32_t prevTime = 0; // Used for frames-per-second throttle
float global_z = 0;

// Takes [0, 1] and returns [0, 1].
static float gamma_correct(float in)
{
    // Stolen from https://learn.adafruit.com/led-tricks-gamma-correction/the-longer-fix.
    const float gamma = 2.8;
    return std::pow(in, gamma);
}

void err(int x) {
    uint8_t i;
    pinMode(LED_BUILTIN, OUTPUT);       // Using onboard LED
    for(i=1;;i++) {                     // Loop forever...
        digitalWrite(LED_BUILTIN, i & 1); // LED on/off blink to alert user
        delay(x);
    }
}

void setup() {
    init(); // https://forum.arduino.cc/t/how-to-avoid-the-quirks-of-the-ide-sketch-file-pre-preprocessing/262594/2
    Serial.begin(115200);

    while (!Serial) { }

    ProtomatterStatus status = matrix.begin();
    Serial.printf("Protomatter begin() status: %d\n", status);
}

void loop() {
    //  Limit the animation frame rate to MAX_FPS.
    uint32_t t;
    while(((t = micros()) - prevTime) < (1000000L / MAX_FPS));
    prevTime = t;

    auto perlin_slice = PerlinSlice2D<5, 5>(global_z);

    for (unsigned int row = 0; row < HEIGHT; row++)
    {
        for (unsigned int column = 0; column < WIDTH; column++)
        {
            float x = float(column) / WIDTH;
            float y = float(row) / HEIGHT;
            float perlin_value = perlin_slice.value_at(x, y);

            const uint8_t uncorrected_value = 255*perlin_value;
            const uint8_t gamma_corrected_value = uncorrected_value; // gamma_correct(uncorrected_value);

            matrix.drawPixel(column, row, matrix.color565(gamma_corrected_value, 0, 0));
        }
    }

    matrix.show();

    global_z = float(t)/1000/1000;
}
