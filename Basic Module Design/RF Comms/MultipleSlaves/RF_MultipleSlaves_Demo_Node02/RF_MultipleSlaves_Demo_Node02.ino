// RF Network Demo
// 5 Transceivers
// https://howtomechatronics.com/tutorials/arduino/how-to-build-an-arduino-wireless-network-with-multiple-nrf24l01-modules/

// NODE 02 Code
//        Node 00
//       /       \
//   Node 01    ~Node 02~
//     /           \
//  Node 012      Node 022

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>


RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 02;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

const unsigned long interval = 10;  //ms  // How often to send data to the other unit
unsigned long last_sent;            // When did we last send?


void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
}

void loop() {
  network.update();
  
  //===== Sending =====//
  unsigned long now = millis();
  
  if (now - last_sent >= interval) {   // If it's time to send a data, send it!
    last_sent = now;
    unsigned long potValue = analogRead(A0);
    unsigned long ledBrightness = map(potValue, 0, 1023, 0, 255);
    RF24NetworkHeader header(master00);   // (Address where the data is going)
    bool ok = network.write(header, &ledBrightness, sizeof(ledBrightness)); // Send the data
  }
}
