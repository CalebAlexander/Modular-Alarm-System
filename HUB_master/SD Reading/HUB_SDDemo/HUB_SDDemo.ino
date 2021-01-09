// HUB_SDDemo


#include <SD.h>
#include <SPI.h>
#include <Audio.h>

char* morningFiles[] = {"file0.wav", "file1.wav", "file2.wav", "file3.wav",
                        "file4.wav", "file5.wav", "file6.wav", "file7.wav",
                        "file8.wav", "file9.wav", "file10.wav", "file11.wav"}
int numMorningFiles = 12;

void playMorningAlarm();

void setup() {
  // debug output
  Serial.begin(9600);

  // setup SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(4))
  {
    Serial.println("Failed!");
    while(1);
  }
  Serial.println(" done.");
  // hi-speed SPI transfers

  // 44100Hz stereo => 88200 sample rate
  // 100 mSec of prebuffering
  Audio.begin(88200, 100);
}

void loop() {
  int count = 0;

  // open wave file from sdcard
  File myFile = SD.open("test.wav");
  

  const int S = 1024; // number of samples to read in block
  short buffer[S];

  Serial.print("Playing");
  // until the file is not finished
  while (myFile.available())
  {
    myFile.read(buffer, sizeof(buffer));

    // prepare samples
    int volume = 1024;
    Audio.prepare(buffer, S, volume);
    // Feed samples to audio
    Audio.write(buffer, S);

    // Every 100 blocks print a '.'
    count++;
    if (count == 100) {
      Serial.print(".");
      count = 0;
    }
  }
  myFile.close();
}


void playMorningAlarm()
{
  File myFile = SD.open(morningFile[random(0, 11));
  if (myFile) {
    const int S = 1024; // number of samples to read in block
    short buffer[S];

    while (myFile.available())
    {
      myFile.read(buffer, sizeof(buffer));
  
      // prepare samples
      int volume = 1024;
      Audio.prepare(buffer, S, volume);
      // Feed samples to audio
      Audio.write(buffer, S);
  
      // Every 100 blocks print a '.'
      count++;
      if (count == 100) 
      {
        Serial.print(".");
        count = 0;
      }
    }
  }
  myFile.close();
  }

  // if the file didn't open, throw an error
  Serial.println("error opening test.wav");
}
