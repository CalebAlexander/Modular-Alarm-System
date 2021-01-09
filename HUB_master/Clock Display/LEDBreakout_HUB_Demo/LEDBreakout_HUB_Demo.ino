#include <Adafruit_NeoPixel.h>

#define PIN 4
#define LED_COUNT 2

// Create an instance of the Adafruit_NeoPixel class called "leds".
// That'll be what we refer to from here on...
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  leds.begin();
  leds.clear(); // Set all pixel colors to 'off'
  
}

void loop() {
  leds.setPixelColor(0, 0xFF0000);    // Set fourth LED to full red, no green, full blue
  leds.setPixelColor(1, 0xFF, 0x00, 0x00);  // Also set fourth LED to full red, no green, full blue
  leds.show();   // Send the updated pixel colors to the hardware.
  delay(1000);

  leds.setPixelColor(0, 0x00FF00);    // Set fourth LED to full red, no green, full blue
  leds.setPixelColor(1, 0x00, 0xFF, 0x00);  // Also set fourth LED to full red, no green, full blue
  leds.show();   // Send the updated pixel colors to the hardware.
  delay(1000);

  leds.setPixelColor(0, 0x0000FF);    // Set fourth LED to full red, no green, full blue
  leds.setPixelColor(1, 0x00, 0x00, 0xFF);  // Also set fourth LED to full red, no green, full blue
  leds.show();   // Send the updated pixel colors to the hardware.
  delay(1000);

  leds.setPixelColor(0, 0xFF00FF);    // Set fourth LED to full red, no green, full blue
  leds.setPixelColor(1, 0xFF, 0x00, 0xFF);  // Also set fourth LED to full red, no green, full blue
  leds.show();   // Send the updated pixel colors to the hardware.
  delay(1000);

  leds.setPixelColor(0, 0xFFFF00);    // Set fourth LED to full red, no green, full blue
  leds.setPixelColor(1, 0xFF, 0x0FF, 0x00);  // Also set fourth LED to full red, no green, full blue
  leds.show();   // Send the updated pixel colors to the hardware.
  delay(1000);

  leds.setPixelColor(0, 0x00FFFF);    // Set fourth LED to full red, no green, full blue
  leds.setPixelColor(1, 0x00, 0xFF, 0xFF);  // Also set fourth LED to full red, no green, full blue
  leds.show();   // Send the updated pixel colors to the hardware.
  delay(1000);
}
