/***************************************************
  This is an example for our Adafruit 12-channel PWM/LED driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/

  These drivers uses SPI to communicate, 2 pins are required to
  interface: Data and Clock. The boards are chainable

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_TLC59711.h"
#include "DaisyChain.h"
#include "common.h"

/*
// HSPI pins
#define CLOCK_0_GPIO 14
#define DATA_0_GPIO 13
// VSPI pins
#define CLOCK_1_GPIO 18
#define DATA_1_GPIO 23

uint8_t current_brightness[CHAIN_COUNT][CHAIN_SIZE][LED_COUNT];

Adafruit_TLC59711 chain0_ = Adafruit_TLC59711(CHAIN_SIZE, CLOCK_0_GPIO, DATA_0_GPIO);
Adafruit_TLC59711 chain1_ = Adafruit_TLC59711(CHAIN_SIZE, CLOCK_1_GPIO, DATA_1_GPIO);

void update_chain(ChainIdx idx) {
  Adafruit_TLC59711* chain = &chain0_;
  if (idx == ChainIdx::CHAIN_1) {
    chain = &chain1_;
  }

  for (uint8_t tlc_idx = 0; tlc_idx < CHAIN_SIZE; tlc_idx++) {
    for (uint8_t led_idx = 0; led_idx < (LED_COUNT / 3); led_idx++) {  // LED index is [0, 1, 2, 3]
      uint16_t led_number = tlc_idx * (LED_COUNT / 3) + led_idx;
      chain->setLED(led_number,
                    current_brightness[static_cast<int>(idx)][tlc_idx][led_idx * 3],      // R channel
                    current_brightness[static_cast<int>(idx)][tlc_idx][led_idx * 3 + 1],  // G channel
                    current_brightness[static_cast<int>(idx)][tlc_idx][led_idx * 3 + 2]   // B channel
      );
    }
  }
  chain->write();
}
*/

void setup() {
  Serial.begin(115200);
  Serial.println("Setup ESP32-daisy-chain [...]");

  DaisyChain::getInstance().initialize();

  // chain0_.begin();
  // chain0_.write();
}

void loop() {
  DaisyChain::getInstance().runTestShow();

  // increaseBrightness();

  /*
  // PCB1
  chain0_.setLED(0, 0xffff, 0xffff, 0xffff);
  chain0_.setLED(1, 0xffff, 0xffff, 0xffff);
  chain0_.setLED(2, 0xffff, 0xffff, 0xffff);
  chain0_.setLED(3, 0xffff, 0xffff, 0xffff);

  // PCB2
  tlc.setLED(4, 0xffff, 0xffff, 0xffff);
  tlc.setLED(5, 0xffff, 0xffff, 0xffff);
  tlc.setLED(6, 0xffff, 0xffff, 0xffff);
  tlc.setLED(7, 0xffff, 0xffff, 0xffff);
  // PCB3
  tlc.setLED(8, 0xffff, 0xffff, 0xffff);
  tlc.setLED(9, 0xffff, 0xffff, 0xffff);
  tlc.setLED(10, 0xffff, 0xffff, 0xffff);
  tlc.setLED(11, 0xffff, 0xffff, 0xffff);

  chain0_.simpleSetBrightness(0xff);
  chain0_.write();
  delay(200);

  chain0_.setLED(0, 0, 0, 0);
  chain0_.setLED(1, 0, 0, 0);
  chain0_.setLED(2, 0, 0, 0);
  chain0_.setLED(3, 0, 0, 0);

  tlc.setLED(4, 0, 0, 0);
  tlc.setLED(5, 0, 0, 0);
  tlc.setLED(6, 0, 0, 0);
  tlc.setLED(7, 0, 0, 0);
  tlc.setLED(8, 0, 0, 0);
  tlc.setLED(9, 0, 0, 0);
  tlc.setLED(10, 0, 0, 0);
  tlc.setLED(11, 0, 0, 0);

  chain0_.write();
  delay(800);
  */
}

/*
// Fill the dots one after the other with a color
void colorWipe(uint16_t r, uint16_t g, uint16_t b, uint8_t wait) {
  for (uint16_t i = 0; i < 8 * NUM_TLC59711; i++) {
    chain0_.setLED(i, r, g, b);
    chain0_.write();
    delay(wait);
  }
}

// Rainbow all LEDs at the same time, same color
void rainbow(uint8_t wait) {
  uint32_t i, j;

  for (j = 0; j < 65535; j += 10) {
    for (i = 0; i < 4 * NUM_TLC59711; i++) {
      Wheel(i, (i + j) & 65535);
    }
    chain0_.write();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint32_t i, j;

  for (j = 0; j < 65535; j += 10) { // 1 cycle of all colors on wheel
    for (i = 0; i < 4 * NUM_TLC59711; i++) {
      Wheel(i, ((i * 65535 / (4 * NUM_TLC59711)) + j) & 65535);
    }
    chain0_.write();
    delay(wait);
  }
}

// Input a value 0 to 4095 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(uint8_t ledn, uint16_t WheelPos) {
  if (WheelPos < 21845) {
    chain0_.setLED(ledn, 3 * WheelPos, 65535 - 3 * WheelPos, 0);
  } else if (WheelPos < 43690) {
    WheelPos -= 21845;
    chain0_.setLED(ledn, 65535 - 3 * WheelPos, 0, 3 * WheelPos);
  } else {
    WheelPos -= 43690;
    chain0_.setLED(ledn, 0, 3 * WheelPos, 65535 - 3 * WheelPos);
  }
}

// All RGB Channels on full colour
// Cycles trough all brightness settings from 0 up to 127
void increaseBrightness() {
  for (uint16_t i = 0; i < 8 * CHAIN_SIZE; i++) {
    chain0_.setLED(i, 65535, 65535, 65535);
  }
  for (int i = 0; i < 128; i++) {
    chain0_.simpleSetBrightness(i);
    chain0_.write();
    delay(100);
  }
}
*/
