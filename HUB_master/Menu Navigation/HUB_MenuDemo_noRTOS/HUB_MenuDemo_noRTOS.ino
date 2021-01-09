// Demo for Menu Navigation and time setting using 3 push buttons and 
//    1 liquid crystal display and FreeRTOS and RTC clock
#include "RTClib.h"
#include <LiquidCrystal.h>
#include "Wire.h"
#include <SPI.h>
//#include <Audio.h>
#include "SD.h"
#include <Adafruit_NeoPixel.h>

// Clock Display
uint8_t blue = 0;
uint8_t green = 0;
uint8_t red = 0;
const int ledPin = 4;
const int ledCount = 2;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(ledCount, ledPin, NEO_GRB + NEO_KHZ800);


// Clock with battery back up
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
uint16_t localYear = 0;
uint8_t localMonth = 0, localDay = 0, localHour = 0, localMinute = 0, localSecond = 0, localDayOfTheWeek = 0;
int hourTens = 0, hourOnes = 0, minuteTens = 0, minuteOnes = 0;
int saveTo = 0;
uint8_t morningHour = 7, morningMinute = 0, morningSecond = 0; // 7am
uint8_t eveningHour = 15, eveningMinute = 0, eveningSecond = 0;// 7pm

// LCD pins
const int rs = 37, en = 36, d4 = 39, d5 = 38, d6 = 41, d7 = 40;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  // TODO: replace with LED Matrix
int currentScreen = 0, previousScreen = 0;


// Function Prototypes
void displayScreen(int screen);
int adjustTime(int currentVal, int maxVal, bool increment);
void updateTime(int hourTens, int hourOnes, int minuteTens, int minuteOnes, int saveTo);
void updateDisplay(int input);

// Menu Navigation buttons (2, 3, 18, 19, 20, 21 available for hardware interrupts)
const int upPin = 3;
const int selPin = 2;
const int downPin = 18;       // TODO: consider deboucing buttons

// Audio files
char* morningFiles[] = {"file0.wav", "file1.wav", "file2.wav", "file3.wav",
                        "file4.wav", "file5.wav", "file6.wav", "file7.wav",
                        "file8.wav", "file9.wav", "file10.wav", "file11.wav"};
int numMorningFiles = 12;

// SYSTEM OPERATING VARIABLES
bool morningAlarmOn = false;
bool scaleDisabled = false;
bool sequenceDisabled = false;
bool eStop = false;
bool upButtonPressed = false;
bool selButtonPressed = false;
bool downButtonPressed = false;
int mode = 0;
bool screenTimerOn = false;
uint8_t screenTimerTime = 0;
int timeCounter = 0;


void setup() {
cli();// stop interrupts

  

  // Audio Setup
  // 44100Hz stereo => 88200 sample rate
  // 100 mSec of prebuffering
  /*Audio.begin(88200, 100);
  
  // SD Setup
  Serial.print("Initializing SD card...");
  if (!SD.begin(4))
  {
    Serial.println("Failed!");
    while(1);
  }
  Serial.println(" done.");
  */
  // set up local TIMER at 1Hz
  TCCR3A = 0;// set entire TCCR3A register to 0
  TCCR3B = 0;// same for TCCR3B
  TCNT3  = 0;//initialize counter value to 0
  TCNT3  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR3A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode (Mega: WGM3:0 = 0b0100)
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR3B |= (1 << WGM32) | (1 << CS32) | (1 << CS30);
  // disable timer compare interrupt
  disableLocalTimerInterrupt();
  
  
  
  

  // initialize buttons
  /*pinMode(upPin, INPUT);
  pinMode(selPin, INPUT);
  pinMode(downPin, INPUT); */
  // attach nav buttons to interrupts
  attachInterrupt(digitalPinToInterrupt(upPin), upInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(selPin), selInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(downPin), downInterrupt, FALLING);

sei();// allow interrupts

  Serial.begin(57600);
  Serial.println("Start HUB_MenuDemo");
  
  // Clock Display Initialization
  leds.begin();
  leds.clear();

  // initialize LCD
  lcd.begin(20,4);
  lcd.noDisplay();

  // initialize clock
  if (! rtc.begin()) {
    // TODO: throw error for unconnected clock and reset
    Serial.println("Couldn't find RTC");
    while(1);
  }  
  if (! rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  
  
  while(true)
  {
    
    switch(mode)
    {
      //  ~~ Idle Mode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case 0:
        // update time
        //timeCounter++;
        delay(500);
        //Serial.print(timeCounter);
        if (true)
        {
          timeCounter = 0;
          updateRTCTime();
          // Check Screen Timer 
          // TODO: Replace with timer register
          if (screenTimerOn) {
            if (localSecond == screenTimerTime) {
              screenTimerTime = localSecond + 10;
              if (screenTimerTime > 59) {
                screenTimerTime = screenTimerTime - 59;
              }
              updateDisplay(3); // Screen Timer has expired
            }
          }
        }
        
        // check if go to alarm mode
        if (checkMorningAlarm())
        {
          // go to Morning Alarm Mode
          morningAlarmOn = true;
          scaleDisabled = false;
          sequenceDisabled = false;
          eStop = false;
          enableLocalTimerInterrupt();
          // show screen saver
          previousScreen = 0;
          currentScreen = 1;
          displayScreen(1);
          // activate connected devices
          openBlinds();
          enableShaker();
          enableScale();
          enableSequence();
          mode = 1;
          
          break;
        }
        else if (checkEveningAlarm())
        {
          // go to Evening Alarm
          closeBlinds();
          break;
        }
        // update screen if buttons pressed
        if (upButtonPressed)
        {
          upButtonPressed = false;
          updateDisplay(0);
        }
        else if (selButtonPressed)
        {
          selButtonPressed = false;
          updateDisplay(1);
        }
        else if (downButtonPressed)
        {
          downButtonPressed = false;
          updateDisplay(2);
        }
        
        // check if go to bluetooth mode
        updateBluetoothConnectivity();
        // check for messages received
        updateMessages();
      break;



      // ~~ Alarm Mode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case 1:
        // start Audio
        while (morningAlarmOn || eStop)
        {
          int count = 0;
          /*File myFile = SD.open(morningFiles[random(0, numMorningFiles - 1)]);
          if (myFile) {
            const int S = 1024; // number of samples to read in block
            short buffer[S];
        
            while (myFile.available() && morningAlarmOn)
            {
              myFile.read(buffer, sizeof(buffer));
          
              // prepare samples
              int volume = 1024;
              //Audio.prepare(buffer, S, volume);
              // Feed samples to audio
              //Audio.write(buffer, S);
          
              // Every 100 blocks check morningAlarmOn
              count++;
              if (count >= 100) 
              {
                if ((scaleDisabled && sequenceDisabled) || eStop) {
                  morningAlarmOn = false;
                  disableShaker();
                  disableScale();
                  disableSequence();
                  mode = 0; // Go back to Idle Mode
                  disableLocalTimerInterrupt();
                  // screen off
                  previousScreen = 0;
                  currentScreen = 0;
                  displayScreen(0);
                  
                  myFile.close();
                  break;
                }
                count = 0;
              }
              // check for messages
              updateMessages();
            }
          }
          myFile.close();*/
        }
      break;



      // ~~ Bluetooth Player Mode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case 2:

      break;
    }
  }
}

//  ~~ INTERRUPTS ~~
static void upInterrupt() // input 0
{
  // notify up pressed
  upButtonPressed = true;
}
static void selInterrupt() // input 1
{
  // notify select pressed
  selButtonPressed = true;
  if (morningAlarmOn) {
    eStop = true;
  }
}
static void downInterrupt() // input 2
{
  // notify down pressed
  downButtonPressed = true;
}
void enableLocalTimerInterrupt()
{
	TIMSK3 |= 2;		// 0b010;
}
void disableLocalTimerInterrupt()
{
	TIMSK3 &= 5;		// 0b101;
}
ISR(TIMER3_COMPA_vect)
{
  incrementLocalTime();
}





void updateRTCTime()
{
  DateTime now = rtc.now();
  localYear = now.year();
  localMonth = now.month();
  localDay = now.day();
  localHour = now.hour();
  localMinute = now.minute();
  localSecond = now.second();
  localDayOfTheWeek = (uint8_t)daysOfTheWeek[now.dayOfTheWeek()];
  updateClockDisplay();
  Serial.println("Time updated from RTC");
  Serial.print(localHour);
  Serial.print(":");
  Serial.print(localMinute);
  Serial.print(":");
  Serial.println(localSecond);
  Serial.print("\n\n");
}
void incrementLocalTime()
{
  localSecond = localSecond + 1;
  if (localSecond > 59) {
    localSecond = 0;
    localMinute = localMinute + 1;
  }
  if (localMinute > 59) {
    localMinute = 0;
    localHour = localHour + 1;
  }
  if (localHour > 23) {
    localHour = 0;
  }
  updateClockDisplay();
  
  Serial.print("Time updated locally: ");
  Serial.print(localHour);
  Serial.print(":");
  Serial.print(localMinute);
  Serial.print(":");
  Serial.println(localSecond);
  Serial.print("\n\n");
}

bool checkMorningAlarm()
{
  if (localHour == morningHour && localMinute == morningMinute && localSecond == morningSecond)
  {
    return true;
  }
  return false;
}
bool checkEveningAlarm()
{
  if (localHour == eveningHour && localMinute == eveningMinute && localSecond == eveningSecond)
  {
    return true;
  }
  return false;
}



void updateDisplay(int input)
{
  // SCREEN OFF
  if (currentScreen == 0) 
  {
    // turn screen saver on
    previousScreen = currentScreen;
    currentScreen = 1;
    displayScreen(1);
    // start timeout timer
    screenTimerOn = true;
    screenTimerTime = localSecond + 10;
    if (screenTimerTime > 59) {
      screenTimerTime = screenTimerTime - 59;
    }
  }

  // SCREEN SAVER
  else if(currentScreen == 1)
  {
    switch(input)
    {
      case 0: // up button pressed
        // go to main menu
      case 1: // select button pressed
        // go to main menu
      case 2: // down button pressed
        // go to main menu
        displayScreen(2);
        previousScreen = currentScreen;
        currentScreen = 2;
        break;
      case 3: // timeout timer expired
        // turn screen saver off
        displayScreen(0);
        currentScreen = 0; 
        screenTimerOn = false;
        screenTimerTime = 0;
        break;
    }
  }

  // MAIN MENU 0
  else if(currentScreen == 2)
  {
    switch(input)
    {
      case 0: // up button pressed
        // do nothing
        break;
      case 1: // select button pressed
        // go to Alarms 0
        displayScreen(6);
        previousScreen = currentScreen;
        currentScreen = 6;
        break;
      case 2: // down button pressed
        // go to Main Menu 1
        displayScreen(3);
        currentScreen = 3;
        break;
      case 3: // timeout timer expired
        // go to Screen Saver
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 0; // Screen Off
        break;
    }
  }

  // MAIN MENU 1
  else if(currentScreen == 3)
  {
    switch(input)
    {
      case 0: // up button pressed
        // go to Main Menu 0
        displayScreen(2);
        currentScreen = 2;
        break;
      case 1: // select button pressed
        // go to Devices 0
        displayScreen(12);
        previousScreen = currentScreen;
        currentScreen = 12;
        break;
      case 2: // down button pressed
        // go to Main Menu 2
        displayScreen(4);
        currentScreen = 4;
        break;
      case 3: // timeout timer expired
        // go to Screen Saver
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 0; // Screen Off
        break;
    }
  }

  // MAIN MENU 2
  else if(currentScreen == 4)
  {
    switch(input)
    {
      case 0: // up button pressed
        // go to Main Menu 3
        displayScreen(5);
        currentScreen = 5;
        break;
      case 1: // select button pressed
        // go to Clock 00 -- Current Time
        displayScreen(8);
        previousScreen = currentScreen;
        currentScreen = 8;
        saveTo = 0;
        break;
      case 2: // down button pressed
        // do nothing
        break;
      case 3: // timeout timer expired
        // go to Screen Saver
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 0; // Screen Off
        break;
    }
  }
  // MAIN MENU 3
  else if(currentScreen == 5)
  {
    switch(input)
    {
      case 0: // up button pressed
        // go to Main Menu 0
        displayScreen(2);
        currentScreen = 2;
        break;
      case 1: // select button pressed
        // go to Devices 0
        displayScreen(12);
        previousScreen = currentScreen;
        currentScreen = 12;
        break;
      case 2: // down button pressed
        // go to Main Menu 2
        displayScreen(4);
        currentScreen = 4;
        break;
      case 3: // timeout timer expired
        // go to Screen Saver
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 0; // Screen Off
        break;
    }
  }

  // Alarm 0
  else if(currentScreen == 6)
  {
    switch(input)
    {
      case 0: // up button pressed
        // do nothing
        break;
      case 1: // select button pressed
        // go to Clock 00 -- Morning Alarm
        displayScreen(8);
        previousScreen = currentScreen;
        currentScreen = 8;
        saveTo = 1;
        break;
      case 2: // down button pressed
        // go to Alarm 1
        displayScreen(7);
        currentScreen = 7;
        break;
      case 3: // timeout timer expired
        // go to Main Menu
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 1; // Screen Saver
        break;
    }
  }

  // Alarm 1
  else if(currentScreen == 7)
  {
    switch(input)
    {
      case 0: // up button pressed
        // go to Alarm 0
        displayScreen(6);
        currentScreen = 6;
        break;
      case 1: // select button pressed
        // go to Clock 00 -- Evening Alarm
        displayScreen(8);
        previousScreen = currentScreen;
        currentScreen = 8;
        saveTo = 2;
        break;
      case 2: // down button pressed
        // do nothing
        break;
      case 3: // timeout timer expired
        // go to Main Menu
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 1; // Screen Saver
        break;
    }
  }

  // Clock 00 (_X:XX)
  else if(currentScreen == 8)
  {
    switch(input)
    {
      case 0: // up button pressed
        // increment digit
        hourTens = adjustTime(hourTens, 2, true);
        displayScreen(8);
        break;
      case 1: // select button pressed
        // go to next digit
        displayScreen(9);
        currentScreen = 9;
        break;
      case 2: // down button pressed
        // decrement digit
        hourTens = adjustTime(hourTens, 2, false);
        displayScreen(8);
        break;
      case 3: // timeout timer expired
        // go to Alarm 0
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 2; // Main Menu
        break;
    }
  }

  // Clock 01 (X_:XX)
  else if(currentScreen == 9)
  {
    switch(input)
    {
      case 0: // up button pressed
        // increment digit
        if (hourTens == 2) // max hours is 24
        {
          hourOnes = adjustTime(hourOnes, 3, true);
        }
        else
        {
          hourOnes = adjustTime(hourOnes, 9, true);
        } 
        displayScreen(9);
        break;
      case 1: // select button pressed
        // go to next digit
        displayScreen(10);
        currentScreen = 10;
        break;
      case 2: // down button pressed
        // decrement digit
        if (hourTens == 2) // max hours is 24
        {
          hourOnes = adjustTime(hourOnes, 3, true);
        }
        else
        {
          hourOnes = adjustTime(hourOnes, 9, true);
        } 
        displayScreen(9);
        break;
      case 3: // timeout timer expired
        // go to Alarm 0
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 2; // Main Menu
        break;
    }
  }

  // Clock 02 (XX:_X)
  else if(currentScreen == 10)
  {
    switch(input)
    {
      case 0: // up button pressed
        // increment digit
        minuteTens = adjustTime(minuteTens, 5, true);
        displayScreen(10);
        break;
      case 1: // select button pressed
        // go to next digit
        displayScreen(11);
        currentScreen = 11;
        break;
      case 2: // down button pressed
        // decrement digit
        minuteTens = adjustTime(minuteTens, 5, false);
        displayScreen(10);
        break;
      case 3: // timeout timer expired
        // go to Alarm 0
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 2; // Main Menu
        break;
    }
  }

  // Clock 03 (XX:X_)
  else if(currentScreen == 11)
  {
    switch(input)
    {
      case 0: // up button pressed
        // increment digit
        minuteOnes = adjustTime(minuteOnes, 9, true);
        displayScreen(11);
        break;
      case 1: // select button pressed
        // update time and go back to Main Menu 0
        updateTime(hourTens, hourOnes, minuteTens, minuteOnes, saveTo);
        // reset default time
        hourTens = 0; hourOnes = 0; minuteTens = 0; minuteOnes = 0;
        displayScreen(2);
        currentScreen = 2;
        previousScreen = 1;
        break;
      case 2: // down button pressed
        // decrement digit
        minuteOnes = adjustTime(minuteOnes, 9, false);
        displayScreen(11);
        break;
      case 3: // timeout timer expired
        // go to Alarm 0
        displayScreen(previousScreen);
        currentScreen = previousScreen;
        previousScreen = 2; // Main Menu
        break;
    }
  }
  
}

int adjustTime(int currentVal, int maxVal, bool increment)
{
  int temp = 0;
  if (increment)
  {
    temp = currentVal + 1;
    if (temp > maxVal)
    {
      temp = 0;
    }
  }
  else
  {
    temp = currentVal - 1;
    if (temp < 0)
    {
      temp = maxVal;
    }
  }
  return temp;
}

void updateTime(int hourTens, int hourOnes, int minuteTens, int minuteOnes, int saveTo)
{
  uint8_t hours = (hourTens * 10) + hourOnes;
  uint8_t minutes = (minuteTens * 10) + minuteOnes;
  if (saveTo == 0) // Set RTC Time
  {
    localHour = hours;
    localMinute = minutes;
    localSecond = 0;
    //rtc.adjust(DateTime(localYear, localMonth, localDay, hours, minutes, 0));
    Serial.print("Updated RTC Time to ");
    Serial.print(hours, DEC);
    Serial.print(":");
    Serial.println(minutes, DEC);
  }
  else if (saveTo == 1) // Set Morning Alarm
  {
    Serial.print("Updated Morning Alarm to ");
    Serial.print(hours, DEC);
    Serial.print(":");
    Serial.println(minutes, DEC);
    morningHour = hours;
    morningMinute = minutes;
  }
  else if (saveTo == 2) // Set Evening Alarm
  {
    Serial.print("Updated Evening Alarm to ");
    Serial.print(hours, DEC);
    Serial.print(":");
    Serial.println(minutes, DEC);
    eveningHour = hours;
    eveningMinute = minutes;
  }
}  

void displayScreen(int screen)
{
  lcd.clear();

  // SCREEN OFF
  if (screen == 0)
  {
    lcd.noDisplay();
  }

  // SCREEN SAVER
  else if (screen == 1)
  {
    lcd.print("Screen Saver");
    lcd.display();
  }

  // MAIN MENU 0
  else if (screen == 2)
  {
    lcd.print("Main Menu 0");
    lcd.display();
  }

  // MAIN MENU 1
  else if (screen == 3)
  {
    lcd.print("Main Menu 1");
    lcd.display();
  }

  // MAIN MENU 2
  else if (screen == 4)
  {
    lcd.print("Main Menu 2");
    lcd.display();
  }

  // MAIN MENU 3
  else if (screen == 5)
  {
    lcd.print("Main Menu 3");
    lcd.display();
  }

  // ALARM 0
  else if (screen == 6)
  {
    lcd.print("Alarm 0");
    lcd.display();
  }

  // ALARM 1
  else if (screen == 7)
  {
    lcd.print("Alarm 1");
    lcd.display();
  }

  // CLOCK 00
  else if (screen == 8)
  {
    lcd.print("Clock 00");
    lcd.display();
  }

  // CLOCK 01
  else if (screen == 9)
  {
    lcd.print("Clock 01");
    lcd.display();
  }

  // CLOCK 02
  else if (screen == 10)
  {
    lcd.print("Clock 02");
    lcd.display();
  }

  // CLOCK 03
  else if (screen == 11)
  {
    lcd.print("Clock 03");
    lcd.display();
  }
  
  
  Serial.print("Screen Number: ");
  Serial.println(screen, DEC);
}


// CONNECTED DEVICE FUNCTIONS

void updateMessages()
{
  if (morningAlarmOn)
  {
    
  }
}

void updateBluetoothConnectivity()
{
  
}

void updateClockDisplay()
{
  blue = random(0,255);
  red = random(0,255);
  green = random(0,255);
  leds.setPixelColor(0, red, green, blue);
  leds.setPixelColor(1, red, green, blue);
  leds.show(); 
  Serial.print("Color: 0x");
  Serial.println(((red << 4) | (green << 2) | (blue)), HEX);
}

void openBlinds()
{
  
}
void closeBlinds()
{
  
}

void enableShaker()
{
  
}
void disableShaker()
{
  
}

void enableScale()
{
  
}
void disableScale()
{
  
}

void enableSequence()
{
  
}
void disableSequence()
{
  
}
