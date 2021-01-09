// RF Network Demo
// 5 Transceivers
// https://howtomechatronics.com/tutorials/arduino/how-to-build-an-arduino-wireless-network-with-multiple-nrf24l01-modules/

// NODE 022 Code
//        Node 00
//       /       \
//   Node 01    Node 02
//     /           \
//  Node 012      ~Node 022~

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define led1 2
#define led2 3
#define led3 4
#define led4 5

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 022;  // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}

void loop() {
  network.update();
  
  //===== Receiving =====//
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    unsigned long potValue;
    network.read(header, &potValue, sizeof(potValue)); // Read the incoming data
    
    // Turn on the LEDs as depending on the incoming value from the potentiometer
    if (potValue > 240) {
      digitalWrite(led1, HIGH);
    } else {
      digitalWrite(led1, LOW);
    }
    
    if (potValue > 480) {
      digitalWrite(led2, HIGH);
    } else {
      digitalWrite(led2, LOW);
    }
    
    if (potValue > 720) {
      digitalWrite(led3, HIGH);
    } else {
      digitalWrite(led3, LOW);
    }
    
    if (potValue > 960) {
      digitalWrite(led4, HIGH);
    } else {
      digitalWrite(led4, LOW);
    }
  }
}
