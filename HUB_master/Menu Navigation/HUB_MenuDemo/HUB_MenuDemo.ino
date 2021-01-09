// Demo for Menu Navigation and time setting using 3 push buttons and 
//    1 liquid crystal display and FreeRTOS and RTC clock

#include <LiquidCrystal.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "RTClib.h"

// Clock with battery back up
//RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
uint16_t rtcYear = 0;
uint8_t rtcMonth = 0, rtcDay = 0, rtcHour = 0, rtcMinute = 0, rtcSecond = 0, rtcDayOfTheWeek = 0;
int hourTens = 0, hourOnes = 0, minuteTens = 0, minuteOnes = 0;
int saveTo = 0;
uint8_t morningHour = 7, morningMinute = 0, morningSecond = 0; // 7am
uint8_t eveningHour = 15, eveningMinute = 0, eveningSecond = 0;// 7pm

// LCD pins
const int rs = 37, en = 36, d4 = 39, d5 = 38, d6 = 41, d7 = 40;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  // TODO: replace with LED Matrix
int currentScreen = 0, previousScreen = 0;


// Sample tasks
TaskHandle_t RFPollTask_Handle;
TaskHandle_t BluetoothTask_Handle;
TaskHandle_t AudioTask_Handle;// TODO: adjust needed tasks
TaskHandle_t IdleTask_Handle;
TaskHandle_t ClockPollTask_Handle;

// Function Prototypes
void displayScreen(int screen);
int adjustTime(int currentVal, int maxVal, bool increment);
void updateTime(int hourTens, int hourOnes, int minuteTens, int minuteOnes, int saveTo);
void updateDisplay(int input);

// Semaphores
//SemaphoreHandle_t xSerialSemaphore;
SemaphoreHandle_t xClockSemaphore;


// Menu Navigation buttons (2, 3, 18, 19, 20, 21 available for hardware interrupts)
const int upPin = 18;
const int selPin = 19;
const int downPin = 20;       // TODO: consider deboucing buttons

// 

void setup() {
cli();// stop interrupts

  // initialize serial and it's semaphore
  /*while (!Serial) {
    delay(1);
  }*/
  Serial.begin(9600);
  Serial.println("Start HUB_MenuDemo");
  // initialize clock
  /*if (! rtc.begin()) {
    // TODO: throw error for unconnected clock and reset
    Serial.println("Couldn't find RTC");
    while(1);
  }
  if (! rtc.initialized())
  {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }*/
  if ( xClockSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xClockSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xClockSemaphore ) != NULL )
      xSemaphoreGive( ( xClockSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }
  
  // initialize LCD
  lcd.begin(20,4);
  lcd.noDisplay();

  // initialize buttons
  /*pinMode(upPin, INPUT);
  pinMode(selPin, INPUT);
  pinMode(downPin, INPUT); */
  // attach nav buttons to interrupts
  attachInterrupt(digitalPinToInterrupt(upPin), upInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(selPin), selInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(downPin), downInterrupt, FALLING);
  
  // Create tasks and automatically start scheduler
  xTaskCreate(RFPollTask, "RFPoll", 128, NULL, 1, &RFPollTask_Handle);
  xTaskCreate(BluetoothTask, "Bluetooth", 128, NULL, 2, &BluetoothTask_Handle);
  xTaskCreate(AudioTask, "Audio", 128, NULL, 3, &AudioTask_Handle);
  xTaskCreate(ClockPollTask, "Clock", 128, NULL, 4, &ClockPollTask_Handle);
  //xTaskCreate(IdleTask, "IdleTask", 128, NULL, 0, &IdleTask_Handle);

sei();// allow interrupts
}

void loop() {
  //    do nothing
}

//  ~~ INTERRUPTS ~~
static void upInterrupt() // input 0
{
  // move menu up
  updateDisplay(0);
  Serial.println(F("UP Interrupt Executed"));
}
static void selInterrupt() // input 1
{
  // select menu item
  updateDisplay(1);
  Serial.println(F("SELECT Interrupt Executed"));
}
static void downInterrupt() // input 2
{
  // move menu down
  updateDisplay(2);
  Serial.println(F("DOWN Interrupt Executed"));
}

//  ~~ TASKS ~~
static void RFPollTask()
{
  for(;;)
  {
    Serial.println(F("RF Task Executed"));
    vTaskDelay(5000/portTICK_PERIOD_MS);
  }
}
static void BluetoothTask()
{
  for(;;)
  {
    Serial.println(F("Bluetooth Task Executed"));
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }
}
static void AudioTask()
{
  for(;;)
  {
    Serial.println(F("Audio Task Executed"));
    vTaskDelay(10000/portTICK_PERIOD_MS);
  }// 1 tick = 15ms?
}
static void IdleTask()
{
  Serial.println(F("Idle Task Executed"));
  delay(1000);
}
static void ClockPollTask()
{
  DateTime now;
  for(;;)
  {
    //now = rtc.now();
    
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xClockSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // update clock values
      /*rtcYear = now.year();
      rtcMonth = now.month();
      rtcDay = now.day();
      rtcHour = now.hour();
      rtcMinute = now.minute();
      rtcSecond = now.second();
      rtcDayOfTheWeek = daysOfTheWeek[now.dayOfTheWeek()];*/
      xSemaphoreGive( xClockSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    // TODO: update Clock Display
    Serial.println(F("Clock Poll Task Executed"));
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}


void updateDisplay(int input)
{
  // SCREEN OFF
  if (currentScreen == 0) 
  {
    // turn screen saver on
    previousScreen = currentScreen;
    currentScreen = 1;
    // TODO: start timeout timer
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
    if ( xSemaphoreTake( xClockSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      //rtc.adjust(DateTime(rtcYear, rtcMonth, rtcDay, hours, minutes, 0));
      Serial.print("Updated RTC Time to ");
      Serial.print(hours, DEC);
      Serial.print(":");
      Serial.println(minutes, DEC);
      xSemaphoreGive( xClockSemaphore ); // Now free or "Give" the Serial Port for others.
    }
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
