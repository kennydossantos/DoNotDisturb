#include <EMailSender.h>



// TFT_eSPI - Version: Latest 
#include <TFT_eSPI.h>

//  pin on TFT    pin on ESP32 Dev Kit
//      VCC         V5 (+5 Volt)
//      GND         GND (DO NOT USE THE ONE NEXT TO V5!)
//      CS          G15 (chip select)
//      RESET       G4
//      D/C         G2  (device instruction)
//      SDI         G23 (SPI MOSI)
//      SCK         G18 (SPI clock)
//      LED         3V3 (3.3>V for backlight)
//      SDO         G19 (SPI MISO)
//      T_CLK       G18 (Touch SPI clock)
//      T_CS        G21 (Chipselect Touch)
//      T_DIN       G23 (SPI MOSI)
//      T_DO        G19 (SPI MISO)
//      T_IRQ       not used, leave open!
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>
#include <SPI.h>
#include <TFT_eSPI.h>
// The custom font file attached to this sketch must be included
#include "MyFont.h"
// Stock font and GFXFF reference handle
#define GFXFF 1
// Easily remembered name for the font
#define MYFONT32 &myFont32pt8b
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Replace with your network credentials
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const char* ssid     = "";
const char* password = "";
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

const char* ntpServer = "pool.ntp.org";

String greenbuttontext = "Come In"; // text on html page
String greentext1 = "COME";         // first line on TFT Display
String greentext2 = "IN";           // secons line on TFT Display

String redbuttontext = "Do not Enter"; // text on html page
String redtext1 = "DO NOT";            // first line on TFT Display
String redtext2 = "ENTER";             // secons line on TFT Display

String orangebuttontext = "Please Ask"; // text on html page
String orangetext1 = "PLEASE";          // first line on TFT Display
String orangetext2 = "ASK";             // secons line on TFT Display

String bluebuttontext = "Requested to Enter!";    // text on html page
String bluetext1 = "REQUESTED TO ENTER...";      // text for active request on TFT Display
String bluetext2 = "Press for Request to Enter";  // text for inactive request on TFT Display

// Set web server port number to 80
WiFiServer server(80);

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
String minleft;   // for displaying minutes left until greenbutton gest activated

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

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

void setgreen() { // set screen and flags to green state
  greenbutton = true;
  redbutton = false;
  orangebutton = false;
  bluebutton = false;
  minleft = "0";
  tft.fillScreen(TFT_DARKGREEN);
  tft.drawString(greentext1, 160, 40, GFXFF);
  tft.drawString(greentext2, 160, 100, GFXFF);
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

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
          tft.drawString("WAIT: " + minleft + " min.", 160, 200, 4);
        }
      }
    }
  }
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

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

void makepage(WiFiClient client) {
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<meta http-equiv=\"refresh\" content=\"5; URL=/\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style buttons
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #D9D9D9; border: none; color: white; padding: 16px 10px; border-radius: 12px; width: 250px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".buttong {background-color: #009000;}");
  client.println(".buttonr {background-color: #E00000;}");
  client.println(".buttono {background-color: #FF8000;}");
  client.println(".buttonb1 {background-color: #0000FF; border: 2px solid #00FFFF; color: white;cursor: default; width: 300px;border-radius: 0px}");
  client.println(".buttonb2 {background-color: #D9D9D9; border: 2px solid #00FFFF; color: white;cursor: default; width: 300px;border-radius: 0px}");
  client.println(".buttonlg {background-color: #54FF9F;width: 100px;}");
  client.println(".button2 {background-color: #555555;}");
  client.println(".buttong:hover {box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);}");
  client.println(".buttonr:hover {box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);}");
  client.println(".buttono:hover {box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);}");
  client.println(".button2:hover {box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);}");
  client.println(".buttonlg:hover {box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);}</style></head>");
  // Web Page Heading
  client.println("<body><h2><font color=\"black\">Kenny <font color=\"black\">Dos Santos<font color=\"blue\">Kontor</h2>");
  client.println("<font color=\"black\">" + timestr);

  // Display buttons according to current state
  if (greenbutton) {
    client.println("<p><a href=\"/green\"><button class=\"button buttong\">" + greenbuttontext + "</button></a></p>");
  } else {
    client.println("<p><a href=\"/green\"><button class=\"button button2\">" + greenbuttontext + "</button></a></p>");
  }
  if (redbutton) {
    client.println("<p><a href=\"/red\"><button class=\"button buttonr\">" + redbuttontext + "</button></a></p>");
  } else {
    client.println("<p><a href=\"/red\"><button class=\"button button2\">" + redbuttontext + "</button></a></p>");
  }
  if (orangebutton) {
    client.println("<p><a href=\"/orange\"><button class=\"button buttono\">" + orangebuttontext + "</button></a></p>");
  } else {
    client.println("<p><a href=\"/orange\"><button class=\"button button2\">" + orangebuttontext + "</button></a></p>");
  }
  if (not greenbutton) {
    client.println("<p><a href=\"/5\"><button class=\"button buttonlg\">05</button></a>"
                   " <a href=\"/10\"><button class=\"button buttonlg\">10</button></a>"
                   " <a href=\"/00\"><button class=\"button buttonlg\">00</button></a></p>");

    if (secondsleft > 0) {
      client.println("<p><h2>Come in in  " + minleft + " minutes</h2></p>");
    }

    if (bluebutton) {
      client.println("<p><button class=\"button buttonb1\">" + bluebuttontext + "</button></a></p>");
    } else {
      client.println("<p><button class=\"button buttonb2\">" + bluebuttontext + "</button></a></p>");
    }
  }
  client.println("</body></html>");

  // end HTTP response with another blank line
  client.println();

}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void setup() {
  Serial.begin(9600);

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
  server.begin();
  configTime(0, 0, ntpServer);
  setenv("TZ", "CET-1CEST,M3.5.0/02,M10.5.0/03", 1);
  StringLocalTime();

  // initialize flags and variables
  lastmillis = millis();

  // Initialise the TFT screen
  tft.init();
  tft.setRotation(1);
  tft.setFreeFont(MYFONT32);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK);
  setgreen();
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void loop() {
  checktimer();
  checktouch();
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");
    String currentLine = "";                // build string from incoming data (from the client to server)
    while (client.connected()) {            // loop while the client's connected (some browser will hold connection,
      // so this loop may be blocking everything outside the loop
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // different reactions to the sub-URLs called by pressing buttons on the web page
            if (header.indexOf("GET /green") >= 0) {
              Serial.println("Green button pressed");
              secondsleft = 0;
              setgreen();
            } else if (header.indexOf("GET /red") >= 0) {
              Serial.println("Red button pressed");
              secondsleft = 0;
              setred();
            } else if (header.indexOf("GET /orange") >= 0) {
              Serial.println("Orange button pressed");
              secondsleft = 0;
              setorange();
            } else if (header.indexOf("GET /5") >= 0) {
              Serial.println("+5 minutes");
              secondsleft += 300;
              lastmillis = -1000; // this value forces checktimer to re-calculate timer values
              checktimer();
            } else if (header.indexOf("GET /10") >= 0) {
              Serial.println("+10 minutes");
              secondsleft += 600;
              lastmillis = -1000;// this value forces checktimer to re-calculate timer values
              checktimer();
            } else if (header.indexOf("GET /00") >= 0) {
              Serial.println("reset minutes");
              secondsleft = 0;
              lastmillis = -1000;// this value forces checktimer to re-calculate timer values
              checktimer();
            }
            makepage(client); //Build and send the complete dynamic html code
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      } else {
        // do these two calls, just in case the clients holds the connection and
        // would block the responsiveness of the server
        // if the functions would only be called outside the while loop
        checktimer();
        checktouch();
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
