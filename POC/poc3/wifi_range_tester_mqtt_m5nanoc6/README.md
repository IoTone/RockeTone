# Overview

This device performs the work of a "call button" used in hospitals and care homes.  It is wifi connected.  It sends data to the local MQTT server.

## Dependencies

- https://github.com/khoih-prog/ESP_WiFiManager_Lite/tree/main
- https://github.com/khoih-prog/ESP_MultiResetDetector
- Adafruit MQTT
- Adafruit NeoPixel

Example: https://github.com/khoih-prog/ESP_WiFiManager_Lite/tree/main/examples/ESP_WiFi_MQTT

## Configuration

- You need to put this device on wifi, so once set up, it can function
- your MQTT server needs to be available and known in advance.  This is plausibly hardcoded, set this IP address up.

## Use

Click button.

## Known Issues

- lacks an RTC ... would be ideal to set the local time somehow
- TODO: add some kind of NTP date functionality