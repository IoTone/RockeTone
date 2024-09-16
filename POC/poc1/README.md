# Overview

POC1 is about attaching an IMU sensor (https://www.adafruit.com/product/4517) to an M5StampS3 and making this a foundational component of the RockeTone architecture.  It also includes a simple NFC concept for getting rocket launch information or possibly even collecting the rocket data (in the future).

## Goals

- Verify power consumption
- Verify network range: test ESP-NOW, BLE
- Verify Temperature
- Build a prototype visual dashboard for sensors (maybe use Project MagNET)
- Get an estimated BOM cost for rough design
- Feed above input into POC2, which will be about creating a board and software to make the first RockeTone sensors.

## PoC Features

- A visual viewer of the 9DOF sensor using browser
- An NFC tap concept to collect rocket launch data and maybe rocket "captain" info.

## Setup

- Need to do pre-req M5StampS3 Setup
- Need to install Adafruit Sensor Lab
- Run Adafruit Sensor Calibration -> sensor_calibration_write and sensor_calibration_read (to verify).
- Run the modified version of the calibrated_orientation in the calibrated_orientation directory.  This has the needed PINs and headers defined.  A bit tricky because docs don't really put the pieces together for how to make it work.
- Copy the files from here https://github.com/IoTone/Adafruit_WebSerial_3DModelViewer into your webserver.  NOTE: your web server must use HTTPS.  Modern chrome browser will not function without https being served.
