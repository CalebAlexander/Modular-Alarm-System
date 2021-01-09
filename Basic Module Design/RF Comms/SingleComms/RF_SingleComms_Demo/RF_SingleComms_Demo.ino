// Example with 2 transceivers
// https://howtomechatronics.com/tutorials/arduino/how-to-build-an-arduino-wireless-network-with-multiple-nrf24l01-modules/
// HUB Side Code

#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

RF24 radio(10,9);             // nRF24L01 (CE, CSN)
RF24Network network(radio);          // include this radio in the network
const uint16_t this_node = 00;// address of thise node in octal format
const uint16_t node01 = 01;

void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);// (channel, node address)
}

void loop() {
  network.update();
  unsigned long potValue = analogRead(A0);
  unsigned long angleValue = map(potValue, 0, 1023, 0, 180);
  RF24NetworkHeader header(node01);
  bool ok = network.write(header, &angleValue, sizeof(angleValue));
}
