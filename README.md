# DoNotDisturb - HomeKit version

This repository contains code for a HomeKit-enabled TFT Display controller using an ESP32 microcontroller. The project utilizes the HomeSpan library for HomeKit integration.

![NTI Banner](NTIBanner.png)

## Features
- Control the TFT Display state through HomeKit
- Timer functionality for automatic state changes
- Customizable display text and colors

## What you need
- [Arduino IDE](https://www.arduino.cc/en/software) installed
- ESP32 board support installed in Arduino IDE
- HomeSpan library installed in Arduino IDE

## Getting Started
1. Clone this repository to your local machine.
2. Open the Arduino IDE and load the project.
3. Customize the code as needed, especially the network credentials and display settings.
4. Upload the code to your ESP32 board.

## Configuration
- Replace `ssid` and `password` with your Wi-Fi network credentials.
- Customize display text and colors by modifying the corresponding variables in the code.

## TFT Display
The code uses the TFT_eSPI library to control the TFT display. Make sure to include the required font file (`MyFont.h`) attached to this sketch.

## HomeKit Integration
The HomeSpan library is used for seamless HomeKit integration. The code defines custom switches and accessories for HomeKit control.

## Usage
1. Connect the ESP32 to your Wi-Fi network.
2. Open the HomeKit app on your iOS device.
3. Add a new accessory. Use the defualt code of the HomeSpan library or create your own code using the library.
4. Customize the HomeKit settings as needed.

## Acknowledgments
- HomeSpan library: [GitHub - HomeSpan](https://github.com/HomeSpan/HomeSpan)

## Credits
© Kenny Dos Santos

For more information about NTI Johanneberg, check [NTI Johanneberg 2023](https://ntigymnasiet.se/johanneberg/).
