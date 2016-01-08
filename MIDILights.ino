// MIDILights.ino
// Mitchell Katz
// Wednesday, January 6, 2016

#include "Adafruit_NeoPixel.h"

#define LED_PIN   6
#define NUM_PIX   150
#define MIDI_BAUD 31250
#define NOTE_ON   0x90
#define NOTE_OFF  0x80

struct Color {byte r, g, b;};

Adafruit_NeoPixel light = Adafruit_NeoPixel(NUM_PIX, LED_PIN);

void setup() {
    Serial.begin(MIDI_BAUD);
    light.begin();
    light.setBrightness(100);
    light.show();
}

void loop () {
    midiloop();
}

void testloop() {
    fadeTo(1000, uintToColor(Wheel(0)));
    delay(1000);
    fadeTo(1000, uintToColor(Wheel(127)));
    delay(5000);
    fadeTo(1000, uintToColor(Wheel(255)));
    delay(5000);
    fadeTo(1000, {0,0,0});
    delay(5000);
    fadeTo(1000, uintToColor(Wheel(64)));
    delay(5000);
}

void midiloop() {
    do {
        if (Serial.available()) {
            //MIDI containts of 3 bytes indicating the note (1-127), the
            //command (note on, off, etc.) and the velocity (1-127)
            byte command = Serial.read();
            byte note = Serial.read();
            byte velocity = Serial.read();
            if (command == NOTE_ON) {
                //Note on turns on one pixel with a color related to the note
                //itself and the velocity the key was "pressed" at
                light.setPixelColor(note, Wheel((note * velocity) % 255));
            } else if (command == NOTE_OFF) {
                light.setPixelColor(note, 0);
            }
        }
    } while (Serial.available() > 2);//when at least three bytes available
}

/* EFFECTS CREAED BY MITCHELL KATZ */
void fadeTo(int ms, Color aColor) {
    Color c = uintToColor(light.getPixelColor(0));
    int cycles = ms / 50;
    Color decrements = {(c.r - aColor.r) / cycles,
                        (c.b - aColor.b) / cycles,
                        (c.g - aColor.g) / cycles};
    for (int t = 0; t < cycles; t++) {
        c.r -= decrements.r;
        c.g -= decrements.g;
        c.b -= decrements.b;
        for (int i = 0; i < NUM_PIX; i++) {
            light.setPixelColor(i, c.r, c.g, c.b);
        }
        light.show();
        delay(50);
    }
    for (int i = 0; i < NUM_PIX; i++) {
            light.setPixelColor(i, aColor.r, aColor.g, aColor.b);
    }
    light.show();
}

void lightOff() {
    for (int i = 0; i < NUM_PIX; i++) {
        light.setPixelColor(i, 0, 0, 0);
    }
    light.show();
}

void setColor(Color c) {
    for (int i = 0; i < NUM_PIX; i++) {
        light.setPixelColor(i, c.r, c.g, c.b);
    }
    light.show();
}

//Unpack the bitpacked uint32_t representation of a color
//representation format 0x00RRGGBB
Color uintToColor(uint32_t color) {
    Color c;
    c.b = color & 0xFF;
    c.g = (color >> 8) & 0xFF;
    c.r = (color >> 16) & 0xFF;
    return c;
}

/**EFFECTS CREATED BY NEOPIXEL**/
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<light.numPixels(); i++) {
        light.setPixelColor(i, c);
        light.show();
        delay(wait);
    }
}

void rainbow(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<256; j++) {
        for(i=0; i<light.numPixels(); i++) {
            light.setPixelColor(i, Wheel((i+j) & 255));
        }
        light.show();
        delay(wait);
    }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
        for(i=0; i< light.numPixels(); i++) {
            light.setPixelColor(i, Wheel(((i * 256 / light.numPixels()) + j) & 255));
        }
        light.show();
    delay(wait);
    }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
    for (int j=0; j<10; j++) {  //do 10 cycles of chasing
        for (int q=0; q < 3; q++) {
            for (int i=0; i < light.numPixels(); i=i+3) {
                light.setPixelColor(i+q, c);    //turn every third pixel on
            }
            light.show();

            delay(wait);

            for (int i=0; i < light.numPixels(); i=i+3) {
                light.setPixelColor(i+q, 0);        //turn every third pixel off
            }
        }
    }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
    for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
        for (int q=0; q < 3; q++) {
            for (int i=0; i < light.numPixels(); i=i+3) {
                light.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
            }
            light.show();

            delay(wait);

            for (int i=0; i < light.numPixels(); i=i+3) {
                light.setPixelColor(i+q, 0);        //turn every third pixel off
            }
        }
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
        return light.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
        WheelPos -= 85;
        return light.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return light.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}