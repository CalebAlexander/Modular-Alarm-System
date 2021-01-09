// Example with 2 transceivers
// https://howtomechatronics.com/tutorials/arduino/how-to-build-an-arduino-wireless-network-with-multiple-nrf24l01-modules/
// Slave Side Code


#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <Servo.h>
RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 01;   // Address of our node in Octal format ( 04,031, etc)
Servo myservo;  // create servo object to control a servo
void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); //(channel, node address)
  myservo.attach(3);   // (servo pin)
}
void loop() {
  network.update();
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    unsigned long incomingData;
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    myservo.write(incomingData);  // tell servo to go to a particular angle
  }
}
