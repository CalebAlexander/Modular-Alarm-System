// Demo Code for RN-52 Bluetooth Audio Example

int rxPin = 17;
int txPin = 16;
const int btCmdPin = 6;

const int volUpPin = 7;
const int prevTrackPin = 8;
const int playPausePin = 9;
const int nextTrackPin = 10;
const int volDownPin = 11;

const int audioSwitchPin = 5;

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);    // 115200, 8, N , 1

  pinMode(audioSwitchPin, Output);
  pinMode(btCmdPin, Output);
  
  pinMode(volUpPin, Output);
  pinMode(prevTrackPin, Output);
  pinMode(playPausePin, Output);
  pinMode(nextTrackPin, Output);
  pinMode(volDownPin, Output);
  
  digitalWrite(btCmdPin, HIGH); // Use external pull up resistor
  
  // Configure RN-52
  digitalWrite(btCmdPin, LOW);  // Enter CMD mode
  Serial2.write("S%,07(/r)");   // Bit 0: enable AVRCP buttons for EK
                                // Bit 1: enable reconnect on power-on
                                // Bit 2: discoverable on start-up
  Serial2.write("R,1(/r)");
  digitalWrite(btCmdPin, HIGH);  // Exit CMD mode

  // Set Audio Relay Switch
  digitalWrite(audioSwitchPin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}
