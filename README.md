prysmalight-esp8266
===========================================

<p align="center">
  <img alt="prysmalight-esp8266" src="./esp8266.png" width="480">
</p>

<p align="center">
  An arduino sketch to control addressable RGB led strips
</p>

# MQTT API
## Connection Topic: ```prysma/<id>/connected```
  - Fields:
    - name ```<String>```: id of the light
    - connection ```<[0, 2]>```: 0=disconnected, 2=connected
  - Example Response:
  ```
  {
    "name": "Prysma-84F3EBB45500",
    "connection": "2"
  }
  ```
## Effect List Topic: ```prysma/<id>/effects```
  - Fields:
    - name ```<String>```: id of the light
    - effectList ```<Array>```: list of supported effects
  - Example Response:
  ```
  {
    "name": "Prysma-84F3EBB45500",
    "effectList": [
      "Flash",
      "Fade",
      "Rainbow",
      "Cylon",
      "Sinelon",
      "Confetti",
      "BPM",
      "Juggle",
      "Visualize",
      "Dots",
      "Fire",
      "Lightning",
      "Noise"
    ]
  }
  ```
## State Topic: ```prysma/<id>/state```
  - Fields:
    - mutationId ```<Number> (optional)```: Unique id of command that triggered change in state
    - name ```<String>```: id of the light
    - state ```<["ON", "OFF"]>```: light is on or off
    - color ```<Object {r, g, b}>```: RGB color of light from 0-255
    - brightness ```<Number 0-100>```: Brightness of light
    - effect ```<String>```: Name of the current effect or "None" for no effect
    - speed ```<Number 1-7>```: Effect speed
  - Example Response:
  ```
  {
    "mutationId": 67,
    "name": "Prysma-84F3EBB45500",
    "state": "ON",
    "color": {
      "r": 255,
      "g": 255,
      "b": 255
    },
    "brightness": 50,
    "effect": "Flash",
    "speed": 4
  }
  ```
## Command Topic: ```prysma/<id>/command```
  - Fields:
    - mutationId ```<Number> (optional)```: Unique id of command
    - name ```<String>```: id of the light
    - state ```<["ON", "OFF"]>```: light is on or off
    - color ```<Object {r, g, b}>```: RGB color of light from 0-255
    - brightness ```<Number 0-100>```: Brightness of light
    - effect ```<String>```: Name of the current effect or "None" for no effect
    - speed ```<Number 1-7>```: Effect speed
  - Example Command:
  ```
  {
    "mutationId": 67,
    "name": "Prysma-84F3EBB45500",
    "state": "ON",
    "brightness": 50,
    "color": {
      "r": 127,
      "g": 255,
      "b": 0
    },
    "effect": "Flash",
    "speed": 4
  }
  ```
## Configuration Topic: ```prysma/<id>/config```
  - Fields:
    - id ```<String>```: id of the light
    - name ```<String>```: id of the light
    - version ```<String>```: Semantic Versioning version of light firmware
    - hardware ```<String>```: Type of hardware running the light strip
    - colorOrder ```<String>```: order of RGB colors in the light strip
    - stripType ```<String>```: Type of LED strip being used
    - ipAddress ```<String>```: IP address of the light strip
    - macAddress ```<String>```: Mac address of the light strip
    - numLeds ```<String>```: number of addressable leds the light strip has
    - udpPort ```<String>```: udp port the strip is listening on for visualization packets
  - Example Response:
  ```
  {
    "id": "Prysma-84F3EBB45500",
    "name": "Prysma-84F3EBB45500",
    "version": "1.0.0",
    "hardware": "8266",
    "colorOrder": "GRB",
    "stripType": "WS2812B",
    "ipAddress": "10.0.0.114",
    "macAddress": "84:F3:EB:B4:55:00",
    "numLeds": 60,
    "udpPort": 7778
  }
  ```
## Discovery Topic: ```prysma/<id>/discovery```
  - Fields:
    - *: This can be anything
  - Example Command:
  ```
  Discover
  ```
## Discovery Response Topic: ```prysma/<id>/hello```
  - Fields:
    - id ```<String>```: id of the light
    - name ```<String>```: id of the light
    - version ```<String>```: Semantic Versioning version of light firmware
    - hardware ```<String>```: Type of hardware running the light strip
    - colorOrder ```<String>```: order of RGB colors in the light strip
    - stripType ```<String>```: Type of LED strip being used
    - ipAddress ```<String>```: IP address of the light strip
    - macAddress ```<String>```: Mac address of the light strip
    - numLeds ```<String>```: number of addressable leds the light strip has
    - udpPort ```<String>```: udp port the strip is listening on for visualization packets
  - Example Response:
  ```
  {
    "id": "Prysma-84F3EBB45500",
    "name": "Prysma-84F3EBB45500",
    "version": "1.0.0",
    "hardware": "8266",
    "colorOrder": "GRB",
    "stripType": "WS2812B",
    "ipAddress": "10.0.0.114",
    "macAddress": "84:F3:EB:B4:55:00",
    "numLeds": 60,
    "udpPort": 7778
  }
  ```


# Changing the light settings
## Inside config.h
- You can choose to have a dynamic ip address if you comment out STATIC_IP
  - If you choose to have a STATIC_IP address, it must be unique to the device
- Change CONFIG_NUM_LEDS to be the total number of LEDs you are controlling
- Change CONFIG_MAX_BRIGHTNESS to set a maximum brightness value from 0-255.
- Change CONFIG_CHIPSET to be the chipset of the leds you are using

# Arduino IDE Setup 
## Set up ESP8266 Arduino Support
- (NOTE: Arduino version 1.8.8 REQUIRED)
  - v1.8.9 breaks the WiFiUDP library for some reason
- Install ESP8266 Board Manager: https://github.com/esp8266/Arduino#installing-with-boards-manager
- Select the correct board: Tools -> Board -> NodeMCU 1.0
- Set a fast upload speed: Upload Speed -> 512000

## Install Arduino Libraries
- WiFiManager by Tzapu: Version 0.14.0 (or latest)
- PubSubClient by Niick O'Leary: Version 2.7.0 (or latest)
- ArduinoJson by Benoit Blanchon: Version 5.13.3 (Will need to manually select this one)
  - Go to ~/Documents/Arduino/libraries/PubSubClient/src/PubSubClient.h and change MQTT_MAX_PACKET_SIZE to 512 instead of 128. This is because the messages sent by this app are greater than 128 bytes and will be ignored by the pubsubclient unless increased.
- FastLED by Daniel Garcia: Version 3.2.6 (or latest)

## Set up OTA upload
- Install Python 2.7 for OTA uploads: http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html

# Notes
## First time upload and wifi connection
- Make sure you set up wifi using the WiFi manager the first time you upload a sketch on an ESP8266 or you move to a new network.
- If you have a static IP address set, The ESP8266 may say it connects to WiFi the first time you upload a sketch to it, but it really didn't. Just set it back to a dynamic IP address and it should work. Once that happens, set it back to a Static IP address and it should work just fine now.

## Project Structure
- All configuration data should go in config.h
  - This should be the only file typical users will want to change
- All logic relating to controlling the LED strips is located in Light.h and Light.cpp
- All logic relating to connecting to MQTT and handling messages is located in the main sketch: LightController.ino

