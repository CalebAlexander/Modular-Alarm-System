// RF Network Demo
// 5 Transceivers
// https://howtomechatronics.com/tutorials/arduino/how-to-build-an-arduino-wireless-network-with-multiple-nrf24l01-modules/

// NODE 012 Code
//        Node 00
//       /       \
//   Node 01    Node 02
//     /           \
//  ~Node 012~      Node 022

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#define led 2
#define IR 3

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 012;  // Address of our node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;    // Address of the other node in Octal format

void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  pinMode(led, OUTPUT);
  pinMode(IR, INPUT);
}

void loop() {
  network.update();
  
  //===== Receiving =====//
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    unsigned long buttonState;
    network.read(header, &buttonState, sizeof(buttonState)); // Read the incoming data
    
    digitalWrite(led, !buttonState); // Turn on or off the LED
  }
  
  //===== Sending =====//
  unsigned long irV = digitalRead(IR); // Read IR sensor
  RF24NetworkHeader header8(node01);
  
  bool ok = network.write(header8, &irV, sizeof(irV)); // Send the data
}
