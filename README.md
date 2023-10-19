# DoNotDisturb

This project involves an ESP32 module combined with a 2.8" TFT Touch display, functioning as a door sign for a home office. The system indicates whether people are allowed to enter the room based on different states displayed on the TFT screen.

![NTI Banner](NTIBanner.png)

## Features

- **Three States:** 
  - Green: Entry permitted.
  - Red: Do not enter.
  - Orange: Permission to enter must be requested.

- **Web Interface:**
  - Control the system through a web interface.
  - Three buttons to set the state: Green, Red, and Orange.
  - Countdown timer adjustment: +5 minutes, +10 minutes, reset.

- **Countdown Timer:**
  - In Red or Orange state, a countdown timer is activated.
  - When the timer elapses, the state changes to Green.

- **Permission Request:**
  - People can request entry by pressing the touch display.
  - A pending request is indicated on the TFT screen.

## Instructions

1. **Hardware Setup:**
   - Use an ESP32 module and a 2.8" TFT Touch display.
   - Connect the pins according to the specified configuration.

2. **Libraries:**
   - Install the necessary libraries using the library manager:
     - ESP32 core
     - SPI, WiFi, WiFiUdp, and a specific ESP32 version of time.h
     - TFT_eSPI

3. **Font File:**
   - Copy the local font file "MyFont.h" from the library package directory to the sketch directory.

4. **WLAN Configuration:**
   - Replace the placeholder SSID and password with your Wi-Fi credentials.

5. **Web Interface:**
   - Access the device's web interface through any browser.
   - Three buttons for Green, Red, and Orange states.
   - Additional buttons to adjust the countdown timer.

## Known Issues

- The web server is limited to a single client, and issues may arise if a client holds the connection.

## Requirements

- ESP32 core
- SPI, WiFi, WiFiUdp, and ESP32 version of time.h libraries
- TFT_eSPI library

## Credits
© Kenny Dos Santos

For more information about NTI Johanneberg, check [NTI Johanneberg 2023](https://ntigymnasiet.se/johanneberg/).
