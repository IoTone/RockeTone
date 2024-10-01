# Overview

POC1 is about attaching an IMU sensor (https://www.adafruit.com/product/4517) to an M5StampS3 and making this a foundational component of the RockeTone architecture.  It also includes a simple NFC concept for getting rocket launch information or possibly even collecting the rocket data (in the future).

![image](https://private-user-images.githubusercontent.com/64202/372331234-3c8f7d34-4a6a-43d5-a5d4-5e59dfea9548.gif)

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

## Wiring Diagram

![image](https://private-user-images.githubusercontent.com/64202/372333345-8522988b-6eca-4e7d-88a2-8cf72e5cd658.png)

The M5StampS3 does not have a grove connector.  To wire it easily, see the diagram below.
![image](https://github.com/user-attachments/assets/f147cf9d-11e9-43dc-895d-1b8214a6975f)

A Qwiic connector can be used to make connections to the M5StampS3 via the breadboard.  If you don't have a connector like this https://www.sparkfun.com/products/17912 you can simply solder directly to M5StampS3.

The IMU used in this case is an Adafruit LSM6DSOX + LIS3MDL - Precision 9 DoF IMU - STEMMA QT / Qwiic.  When wired correctly, you should minimally see the power led on (green).

## Setup

- Need to do pre-req M5StampS3 Setup
- Need to install Adafruit Sensor Lab
- Run Adafruit Sensor Calibration -> sensor_calibration_write and sensor_calibration_read (to verify).
- Run the modified version of the calibrated_orientation in the calibrated_orientation directory.  This has the needed PINs and headers defined.  A bit tricky because docs don't really put the pieces together for how to make it work.
- Copy the files from here https://github.com/IoTone/Adafruit_WebSerial_3DModelViewer into your webserver.  NOTE: your web server must use HTTPS.  Modern chrome browser will not function without https being served.

## Demo

Use the test site: https://iotoneai.site/rocketone/poc1/modelviewer

This requires the Arduino set up, and running the calibrated_orientation sample.