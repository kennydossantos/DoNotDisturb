// TFT_eSPI - Version: Latest 
#include <TFT_eSPI.h>

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// 
//
//  This sketch needs the following libraries to be installed (use the library manager):
//      ESP32 core
//      SPI, Wifi, WiFiUdp and a specifci ESP32 version of time.h (all part of the ESP32 package)
//      TFT_eSPI
//
//  The TFT library functions do need a local font file "MyFont.h" to be copied from the lib package
//  directory to the sketch diractory
//  Make sure all the display driver and pin comnenctions are correct by
//  editting the User_Setup.h file in the TFT_eSPI library folder:
//
//  pin on TFT    pin on ESP32 Dev Kit
//      VCC         V5 (+5 Volt)
//      GND         GND (DO NOT USE THE ONE NEXT TO V5!)
//      CS          G15 (chip select)
//      RESET       G4
//      D/C         G2  (device instruction)
//      SDI         G23 (SPI MOSI) OBS
//      SCK         G18 (SPI clock) OBS
//      LED         3V3 (3.3>V for backlight)
//      SDO         G19 (SPI MISO) OBS
//      T_CLK       G18 (Touch SPI clock) OBS
//      T_CS        G21 (Chipselect Touch)
//      T_DIN       G23 (SPI MOSI) OBS
//      T_DO        G19 (SPI MISO) OBS
//      T_IRQ       not used, leave open!
//
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// Include libraries. 

#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "HomeSpan.h"
// The custom font file attached to this sketch must be included
#include "MyFont.h"
// Stock font and GFXFF reference handle
#define GFXFF 1
#define GFXFF2 4
// Easily remembered name for the font
#define MYFONT32 &myFont32pt8b
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Replace with your network credentials
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const char* ssid     = "Dos Santos Family"; // NTIG Guest
const char* password = "ri6f9n4e"; //TeknikPassion ri6f9n4e
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

const char* ntpServer = "pool.ntp.org";

String greenbuttontext = "Come In"; // text on html page
String greentext1 = "KOM";         // first line on TFT Display
String greentext2 = "IN";           // secons line on TFT Display

String redbuttontext = "Do not Enter"; // text on html page
String redtext1 = "KOM INTE";            // first line on TFT Display
String redtext2 = "IN";             // secons line on TFT Display

String orangebuttontext = "Please Ask"; // text on html page
String orangetext1 = "VÄNTA,";          // first line on TFT Display
String orangetext2 = "TACK!";             // secons line on TFT Display

String bluebuttontext = "Requested to Enter!";    // text on html page
String bluetext1 = "VANTAR PA SVAR....";      // text for active request on TFT Display
String bluetext2 = "Klicka for att fa komma in";  // text for inactive request on TFT Display

// Set web server port number to original 80

// construct tft objet
TFT_eSPI tft = TFT_eSPI();

String header;  // for storing the HTTP request
String timestr; // for time output

// flags for the current button state
boolean greenbutton;
boolean redbutton;
boolean orangebutton;
boolean bluebutton;

int lastmillis;   // for detecting time elapsed
int secondsleft;  // for counting seconds left until greenbutton gets activated
String minleft;   // for displaying minutes left until greenbutton gets activated

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// Configures the time on the board. 

void StringLocalTime()
{
  char buffer[80];
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println("secondsleft: " + String(secondsleft) + " minleft: " + minleft);
  strftime (buffer, 80, "%A, %B %d %Y _ %H:%M:%S", &timeinfo);
  timestr = buffer;
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// This function makes the TFT display switch to the "green" state.

void setgreen() { // set screen and flags to green state
  greenbutton = true;
  redbutton = false;
  orangebutton = false;
  bluebutton = false;
  minleft = "0";
  secondsleft = 0;
  tft.fillScreen(TFT_DARKGREEN);
  tft.drawString(greentext1, 160, 40, GFXFF);
  tft.drawString(greentext2, 160, 100, GFXFF);
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// This function makes the TFT display switch to the "red" state.

void setred() {
  greenbutton = false;
  redbutton = true;
  orangebutton = false;
  bluebutton = false;
  tft.fillScreen(TFT_RED);
  tft.drawString(redtext1, 160, 40, GFXFF);
  tft.drawString(redtext2, 160, 100, GFXFF);
  tft.setTextColor(TFT_BLUE);
  tft.drawString(bluetext2, 160, 170, 4);
  tft.setTextColor(TFT_BLACK);
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// This function makes the TFT display switch to the "orange/yellow" state.

void setorange() {
  greenbutton = false;
  redbutton = false;
  orangebutton = true;
  bluebutton = false;
  tft.fillScreen(TFT_ORANGE);
  tft.drawString(orangetext1, 160, 40, GFXFF);
  tft.drawString(orangetext2, 160, 100, GFXFF);
  tft.setTextColor(TFT_BLUE);
  tft.drawString(bluetext2, 160, 170, 4);
  tft.setTextColor(TFT_BLACK);
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// This function makes the TFT display switch to the "blue" state.

void setblue() {
  tft.fillScreen(TFT_BLUE);
  if (redbutton) {
    tft.drawString(redtext1, 160, 40, GFXFF);
    tft.drawString(redtext2, 160, 100, GFXFF);
  } else {
    tft.drawString(orangetext1, 160, 40, GFXFF);
    tft.drawString(orangetext2, 160, 100, GFXFF);
  }
  tft.drawString(bluetext1, 160, 170, 4);
  
  }
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// This function checks if a user has touched the tft display and turns on the setblue() state/function.

void checktouch() { // there is no need to debounce this function. more than 1 consecutive events are ok
  uint16_t t_x = 0, t_y = 0; // touch coordinates, do not matter in our case
  if (tft.getTouch(&t_x, &t_y)) {
    // user has pressed touch screen on TFT Display
    Serial.println("pressed");
    // if not in green state change to blue version of red/orange state
    if (not greenbutton) {
      bluebutton = true;
      //below was all commented
      setblue();
            tft.fillScreen(TFT_BLUE);
            if (redbutton){
              tft.drawString(redtext1, 160, 40, GFXFF);
              tft.drawString(redtext2, 160, 100, GFXFF);
            } else {
              tft.drawString(orangetext1, 160, 40, GFXFF);
              tft.drawString(orangetext2, 160, 100, GFXFF);
            }
            tft.drawString(bluetext1, 160, 170, 4);
            if (secondsleft > 0) {
              tft.drawString("Okej att komma in om" + minleft + " min.", 160, 200, 4);
            }
    }
  }
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// A struct is a way to group several related variables into one place. Here we have a bunch of HomeSpan related code. We declare some booleans, checks time and states. 

struct CustomSwitch : Service::Switch {

  SpanCharacteristic *onState = new Characteristic::On(0);
  SpanCharacteristic *enabled = new Characteristic::IsConfigured(1, true);
  SpanCharacteristic *name;  // Move the name to the CustomSwitch class

  bool switchIsOn = false;  // New member to store the state of the switch
  bool switchWasOn = false;  // New member to store the previous state of the switch
  bool switchTurnedOnFlag = false;  // Flag to track if the switch was turned on for the first time
  unsigned long turnOffTime = 0;  // Variable to store the time to turn off the switch



  CustomSwitch(const char *switchName) : Service::Switch() {
    name = new Characteristic::ConfiguredName(switchName, true);
    enabled->addPerms(PW);
  }

  boolean update() override {
    switchWasOn = switchIsOn;  // Store the previous state before updating

    if (enabled->updated()) {
      if (enabled->getNewVal()) {
        Serial.printf("Switch '%s' enabled\n", name->getString());
      } else {
        Serial.printf("Switch '%s' disabled\n", name->getString());
        if (onState->getVal()) {
          onState->setVal(0);
          switchIsOn = false;  // Turn off the switch state when turning off
          switchTurnedOnFlag = false;  // Reset the flag when turning off
          Serial.printf("Switch '%s' is turning OFF\n", name->getString());
        }
      }
    }

    if (onState->updated()) {
      if (onState->getNewVal()) {
        Serial.printf("Switch '%s' is turning ON\n", name->getString());
        switchIsOn = true;  // Turn on the switch state when turning on

        // Check if the switch was turned on for the first time
        if (!switchWasOn) {
          switchTurnedOnFlag = true;
          turnOffTime = millis() + 2000;  // Set the time to turn off after 2 seconds
        }
      } else {
        Serial.printf("Switch '%s' is turning OFF\n", name->getString());
        switchIsOn = false;  // Turn off the switch state when turning off
        switchTurnedOnFlag = false;  // Reset the flag when turning off
      }
    }

    // Check if it's time to turn off the switch
    if (millis() >= turnOffTime && switchIsOn) {
      onState->setVal(0);
      switchIsOn = false;
      Serial.printf("Switch '%s' is turning OFF after 2 seconds\n", name->getString());
    }

    return true; // returns true when the whole function is done.
  }

  // This boolean looks at the code above and returns true or false based on the variable "switchTurnedOnFlag", which is set above. 
  bool justTurnedOn() {
    return switchTurnedOnFlag;
  }
};

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
CustomSwitch* switch1;
CustomSwitch* switch2;
CustomSwitch* switch3;
CustomSwitch* lampSwitch;
CustomSwitch* fiveMinuteSwitch;
CustomSwitch* tenMinuteSwitch;
CustomSwitch* resetTimerSwitch;

// In the setup function we start the serial monitor and print some wifi information. We also set the time and initalise the TFT screen. We also initalise the HomeSpan library. 

void setup() {
  Serial.begin(115200);  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start Webserver and sync local time to NTP server
  configTime(0, 0, ntpServer);
  setenv("TZ", "CET-1CEST,M3.5.0/02,M10.5.0/03", 1);
  StringLocalTime();

  // initialize flags and variables
  lastmillis = millis();

  // Initialise the TFT screen
  tft.init();
  tft.setRotation(3);
  tft.setFreeFont(MYFONT32);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK);
  setgreen();

  // Initalise the HomeSpan library
  homeSpan.begin(Category::Switches, "DoNotDisturb Controls");

  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();

  switch1 = new CustomSwitch("Switch1");
  switch2 = new CustomSwitch("Switch2");
  switch3 = new CustomSwitch("Switch3");
  lampSwitch = new CustomSwitch("LampSwitch");
  fiveMinuteSwitch = new CustomSwitch("fiveMinuteSwitch");
  tenMinuteSwitch = new CustomSwitch("tenMinuteSwitch");
  resetTimerSwitch = new CustomSwitch("resetTimerSwitch");
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// Delcare some booleans, which are used in the next function.

bool fiveMinuteSwitchState = false;
bool tenMinuteSwitchState = false;
bool resetTimerSwitchState = false;

// In the Loop function we call all other functions and declare some switch states. We also chech if a swich is on or not and change the display based on that information.

void loop() {
  checktimer();
  checktouch();

  
  
  homeSpan.poll();

    // Check if the switches were turned on for the first time
  if (switch1->justTurnedOn()) {
    Serial.println("Switch1 is ON for the first time!");
    setgreen();
    // Add your additional actions here for switch1
    switch1->switchTurnedOnFlag = false;  // Reset the flag

    switch2->onState->setVal(0);
    switch3->onState->setVal(0);
  }

  if (switch2->justTurnedOn()) {
    Serial.println("Switch2 is ON for the first time!");
    setred();
    // Add your additional actions here for switch2
    switch2->switchTurnedOnFlag = false;  // Reset the flag

    switch1->onState->setVal(0);
    switch3->onState->setVal(0);
  }

  if (switch3->justTurnedOn()) {
    Serial.println("Switch3 is ON for the first time!");
    setorange();
    // Add your additional actions here for switch3
    switch3->switchTurnedOnFlag = false;  // Reset the flag

    switch1->onState->setVal(0);
    switch2->onState->setVal(0);
  }

  if (lampSwitch->justTurnedOn()) {
      // Add your additional actions here for the lamp
      Serial.println("Lamp is ON for the first time!");
      // You can add any code here to control the digital lamp when it's turned on
      // For example: digitalWrite(lampDigitalPin, HIGH);
      // Reset the timer when the lamp is turned on
      secondsleft = 0;
      lastmillis = millis();  // Reset the timer
      lampSwitch->switchTurnedOnFlag = false;  // Reset the flag
  }

  bool currentFiveSwitchState = fiveMinuteSwitch->onState->getNewVal();
  if (currentFiveSwitchState && !fiveMinuteSwitchState) {
      // Switch has changed from OFF to ON
      secondsleft += 300;  // Add 5 minutes in seconds
      lastmillis = -1000;  // This value forces checktimer to re-calculate timer values
      delay(1000);  // Wait 2 seconds
      fiveMinuteSwitch->onState->setVal(0);  // Turn off the switch to allow adding five more minutes
  }
  bool currentTenSwitchState = tenMinuteSwitch->onState->getNewVal();
    if (currentTenSwitchState && !tenMinuteSwitchState) {
        // Switch has changed from OFF to ON
        secondsleft += 600;  // Add 5 minutes in seconds
        lastmillis = -1000;  // This value forces checktimer to re-calculate timer values
        delay(1000);  // Wait 2 seconds
        tenMinuteSwitch->onState->setVal(0);  // Turn off the switch to allow adding five more minutes
  }

  bool currentResetSwitchState = resetTimerSwitch->onState->getNewVal();
    if (currentResetSwitchState && !resetTimerSwitchState) {
        // Switch has changed from OFF to ON
        secondsleft = 0;  // Add 5 minutes in seconds
        lastmillis = -1000;  // This value forces checktimer to re-calculate timer values
        delay(1000);  // Wait 2 seconds
        resetTimerSwitch->onState->setVal(0);  // Turn off the switch to allow adding five more minutes
        
        setgreen();
        switch1->onState->setVal(1);
        switch2->onState->setVal(0);
        switch3->onState->setVal(0);

  }

  // Update the switch state
  fiveMinuteSwitchState = currentFiveSwitchState;
  tenMinuteSwitchState = currentTenSwitchState;
  resetTimerSwitchState = currentResetSwitchState;

  if (bluebutton) {
        lampSwitch->onState->setVal(1);  // Turn on the lamp switch
    } else {
        lampSwitch->onState->setVal(0);  // Turn off the lamp switch
    }
}

// This function checks how long left there is on the timer, after the user had set a timer on the display. If the timer is 0, we reset to green stage. 

void checktimer() {
  // get current time every second and coutn down timer if >0
  if (millis() > lastmillis + 1000) { // check every seconds
    StringLocalTime();  // get curren time and date into string for web display
    lastmillis = millis();
    // check if countdown timer is active
    if (secondsleft > 0) {
      // decrement timer and check if elapsed
      if (--secondsleft == 0) {
        setgreen(); // timer elapsed, change state to green state
        secondsleft = 0;
        switch2->onState->setVal(0);
        switch3->onState->setVal(0);
        switch1->onState->setVal(1);
      } else {
        // calculate minutes from seconds, round up to the full integer:
        int x = secondsleft / 60;
        if (secondsleft % 60) {
          x++;
        }
        // check if another minute has elapsed
        if (minleft != String(x)) {
          minleft = String(x); // store new value in string for html page
          // minute has elapsed, display new value on TFT
          if (bluebutton) {
            setblue();
          } else {
            if (redbutton) {
              setred();
            } else {
              setorange();
            }
          }
          tft.drawString("VANTA: " + minleft + " min:)", 160, 200, 4);
        }
      }
    }
  }
}
