/************ Configuration Variables ******************/
#define VERSION "1.0.0"
#define HARDWARE "8266"
#define COLOR_ORDER "GRB"
#define STRIP_TYPE "WS2812b"

// how many leds in your strip?
#define CONFIG_NUM_LEDS 60 // TODO: Change this to match your LED strip

// the maximum value you can set brightness to out of 255
#define CONFIG_MAX_BRIGHTNESS 255 // TODO: Change this if you need

// Wifi manager access point config
#define CONFIG_WIFI_MANAGER_PW "991f76a6ab"
//#define STATIC_IP "10.0.0.205" // TODO: Change this (must be unique)
//#define STATIC_GW "10.0.0.1"
//#define STATIC_SN "255.255.252.0"

// pin used for the rgb led strip (PWM)
#define CONFIG_DATA_PIN 5 // This is pin D5 on the NodeMCU ESP8266

// Which LED strip are you using?
#define CONFIG_CHIPSET WS2812B

// What is the color order of your LED strip?
#define CONFIG_COLOR_ORDER GRB

// MQTT client config options
#define CONFIG_MQTT_SERVER_PORT 1883
#define CONFIG_MQTT_USER "pi"
#define CONFIG_MQTT_PASSWORD "MQTTIsBetterThanUDP"

// MQTT topic-name strings
#define CONFIG_MQTT_TOP "prysmalight"
#define CONFIG_MQTT_CONNECTION "connected"
#define CONFIG_MQTT_EFFECT_LIST "effects"
#define CONFIG_MQTT_STATE "state"
#define CONFIG_MQTT_COMMAND "command"
#define CONFIG_MQTT_CONFIG "config"
#define CONFIG_MQTT_DISCOVERY "discovery"

// MDNS config
#define CONFIG_MDNS_HOSTNAME "prysma"
#define CONFIG_MDNS_HOSTNAME_LOCAL "prysma.local"

// OTA config
#define CONFIG_OTA_PASSWORD "ESP8266Rulez"

// Enables Serial and print statements
#define CONFIG_DEBUG false
