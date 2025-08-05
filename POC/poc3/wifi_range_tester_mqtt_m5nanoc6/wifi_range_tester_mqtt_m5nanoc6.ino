/**
 * 2025 IoTone Japan
 */
/****************************************************************************************************************************
  ESP_WiFi_MQTT.ino
  For ESP8266 / ESP32 boards

  ESP_WiFiManager_Lite (https://github.com/khoih-prog/ESP_WiFiManager_Lite) is a library 
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy 
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager_Lite
  Licensed under MIT license
  *****************************************************************************************************************************/

/****************************************************************************************************************************
  You have to modify file ./libraries/Adafruit_MQTT_Library/Adafruit_MQTT.cpp  as follows to avoid dtostrf error
   
  //#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) ||             \
  //    defined(ARDUINO_ARCH_SAMD)
  #if !( ESP32 || ESP8266 || defined(CORE_TEENSY) || defined(STM32F1) || defined(STM32F2) || defined(STM32F3) || defined(STM32F4) || defined(STM32F7) )
  static char *dtostrf(double val, signed char width, unsigned char prec, char *sout) 
  {
    char fmt[20];
    sprintf(fmt, "%%%d.%df", width, prec);
    sprintf(sout, fmt, val);
    return sout;
  }
  #endif
 *****************************************************************************************************************************/

#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"
// #include "M5Atom.h"
// #include <M5Stack.h>
// Include this to enable the M5 global instance.
#include <M5Unified.h>

#define LOCAL_DEBUG       true  //false

#include "Adafruit_MQTT.h"                //https://github.com/adafruit/Adafruit_MQTT_Library
#include "Adafruit_MQTT_Client.h"         //https://github.com/adafruit/Adafruit_MQTT_Library

#include <Adafruit_NeoPixel.h>

// Create a WiFiClient class to connect to the MQTT server
WiFiClient *client                    = NULL;

Adafruit_MQTT_Client    *mqtt         = NULL;
Adafruit_MQTT_Publish   *Temperature  = NULL;
Adafruit_MQTT_Subscribe *LED_Control  = NULL;
Adafruit_MQTT_Publish   *Call_Button  = NULL;
// You have to get from a sensor. Here is just an example
uint32_t measuredTemp = 5;
bool callButtonState = false;
int lastState = HIGH;
int currentState;  
uint32_t fakertc = 0L;
uint32_t counter = 0;


#define BUTTON      9
#define LED         2
#define NUMPIXELS   1
#define M5NANO_C6_RGB_LED_PWR_PIN  19
#define M5NANO_C6_RGB_LED_DATA_PIN 20
Adafruit_NeoPixel pixels(NUMPIXELS, M5NANO_C6_RGB_LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

ESP_WiFiManager_Lite* ESP_WiFiManager;

void heartBeatPrint()
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print(".");        // H means connected to WiFi
  else
  {
    if (ESP_WiFiManager->isConfigMode())
      Serial.print("C");        // C means in Config Mode
    else
      Serial.print("x");        // x means not connected to WiFi
      pixels.setPixelColor(0, pixels.Color(30, 20, 189));
      pixels.show();

  }

  /*
  if (num == 40)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 5 == 0)
  {
    Serial.print(" ");
  }
  */
}

void publishMQTT()
{
  pixels.clear();
  MQTT_connect();

  // Instead of temp, we increment the counter, and send
  counter++;
  if (Temperature->publish(counter))
  {
    Serial.print(F("T"));        // T means publishing OK
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
  }
  else
  {
    Serial.print(F("F"));        // F means publishing failure
    Serial.print("\n");
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
  }
}

void subscribeMQTT()
{
  Adafruit_MQTT_Subscribe *subscription;

  MQTT_connect();
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

  //KH
#define HEARTBEAT_INTERVAL    1000L
  // Print WiFi hearbeat, Publish MQTT Topic every HEARTBEAT_INTERVAL (5) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      // MQTT related jobs
      publishMQTT();
      subscribeMQTT();
    }

    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}

void deleteOldInstances()
{
  // Delete previous instances
  if (mqtt)
  {
    delete mqtt;
    mqtt = NULL;
    Serial.println(F("Deleting old MQTT object"));
  }

  if (Temperature)
  {
    delete Temperature;
    Temperature = NULL;
    Serial.println(F("Deleting old Temperature object"));
  }

  if (Call_Button)
  {
    delete Call_Button;
    Call_Button = NULL;
    Serial.println(F("Deleting old Call_Button object"));
  }
}

#define USE_GLOBAL_TOPIC    true

#if USE_GLOBAL_TOPIC
  String completePubTopic;
  String completeSubTopic;
  String completePubTopicPresence;
#endif

void createNewInstances()
{
  if (!client)
  {
    client = new WiFiClient;

    if (client)
    {
      Serial.println(F("\nCreating new WiFi client object OK"));
    }
    else
      Serial.println(F("\nCreating new WiFi client object failed"));
  }

  // Create new instances from new data
  if (!mqtt)
  {
    // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
    mqtt = new Adafruit_MQTT_Client(client, AIO_SERVER, atoi(AIO_SERVERPORT), AIO_USERNAME, AIO_KEY);

    if (mqtt)
    {
      Serial.println(F("Creating new MQTT object OK"));
      Serial.print(F("AIO_SERVER = ")); Serial.print(AIO_SERVER); Serial.print(F(", AIO_SERVERPORT = ")); Serial.println(AIO_SERVERPORT);
      Serial.print(F("AIO_USERNAME = ")); Serial.print(AIO_USERNAME); Serial.print(F(", AIO_KEY = ")); Serial.println(AIO_KEY);
    }
    else
      Serial.println(F("Creating new MQTT object failed"));
  }

  if (!Temperature)
  {
#if USE_GLOBAL_TOPIC
    completePubTopic = String(AIO_USERNAME) + String(AIO_PUB_TOPIC) + String("/ping");
#else
    // Must be static or global
    static String completePubTopic = String(AIO_USERNAME) + String(AIO_PUB_TOPIC)  + String("/ping");
#endif

    Temperature = new Adafruit_MQTT_Publish(mqtt, completePubTopic.c_str(), MQTT_QOS_1);
    Serial.print(F("Creating new MQTT_Pub_Topic, Temperature = ")); Serial.println(completePubTopic);

    if (Temperature)
    {
      Serial.println(F("Creating new Temperature object OK"));
      Serial.print(F("Temperature MQTT_Pub_Topic = ")); Serial.println(completePubTopic);
    }
    else
      Serial.println(F("Creating new Temperature object failed"));
  }

  if (!LED_Control)
  {
#if USE_GLOBAL_TOPIC
    completeSubTopic = String(AIO_USERNAME) + String(AIO_SUB_TOPIC);
#else
    // Must be static or global
    static String completeSubTopic = String(AIO_USERNAME) + String(AIO_SUB_TOPIC);
#endif

    LED_Control = new Adafruit_MQTT_Subscribe(mqtt, completeSubTopic.c_str());

    Serial.print(F("Creating new AIO_SUB_TOPIC, LED_Control = ")); Serial.println(completeSubTopic);

    if (LED_Control)
    {
      Serial.println(F("Creating new LED_Control object OK"));
      Serial.print(F("LED_Control AIO_SUB_TOPIC = ")); Serial.println(completeSubTopic);

      mqtt->subscribe(LED_Control);
    }
    else
      Serial.println(F("Creating new LED_Control object failed"));
  }

  if (!Call_Button)
  {
#if USE_GLOBAL_TOPIC
    completePubTopicPresence = String(AIO_USERNAME) + String(AIO_PUB_TOPIC) + String("/btn1");
#else
    // Must be static or global
    static String completePubTopicPresence = String(AIO_USERNAME) + String(AIO_PUB_TOPIC) + String("/btn1");
#endif

    Call_Button = new Adafruit_MQTT_Publish(mqtt, completePubTopicPresence.c_str(), MQTT_QOS_1);
    Serial.print(F("Creating new MQTT_Pub_Topic, Call_Button = ")); Serial.println(completePubTopicPresence);

    if (Call_Button)
    {
      Serial.println(F("Creating new Call_Button object OK"));
      Serial.print(F("Call_Button MQTT_Pub_Topic = ")); Serial.println(completePubTopicPresence);
    }
    else
      Serial.println(F("Creating new Call_Button object failed"));
  }
}

void MQTT_connect()
{
  int8_t ret;

  createNewInstances();

  // Return if already connected
  if (mqtt->connected())
  {
    return;
  }

#if LOCAL_DEBUG
  Serial.println(F("\nConnecting to WiFi MQTT (3 attempts)..."));
#endif

  uint8_t attempt = 3;

  while ( (ret = mqtt->connect()) )
  {
    // connect will return 0 for connected
    Serial.println(mqtt->connectErrorString(ret));

#if LOCAL_DEBUG
    Serial.println(F("Another attemtpt to connect to MQTT in 5 seconds..."));
#endif

    mqtt->disconnect();
    delay(5000);  // wait 5 seconds
    attempt--;

    if (attempt == 0)
    {
      Serial.println(F("WiFi MQTT connection failed. Continuing with program..."));
      pixels.setPixelColor(0, pixels.Color(218, 37, 98));
      pixels.show();
      return;
    }
  }

#if LOCAL_DEBUG
  Serial.println(F("WiFi MQTT connection successful!"));
#endif
}

#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] PROGMEM = "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}"\
"button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  // pinMode(LED_PIN, OUTPUT);
  pinMode(M5NANO_C6_RGB_LED_PWR_PIN, OUTPUT);
  digitalWrite(M5NANO_C6_RGB_LED_PWR_PIN, HIGH);
  //Initialize LED PIN
  // M5.begin(true, false, true);
  
  // auto cfg = M5.config();
  // M5.begin(cfg);

  pixels.begin();
  // pinMode(BUTTON, INPUT_PULLUP);

  delay(200);

  Serial.print(F("\nStarting ESP_WiFi_MQTT using ")); Serial.print(FS_Name);
  Serial.print(F(" on ")); Serial.println(ARDUINO_BOARD);
  Serial.println(ESP_WIFI_MANAGER_LITE_VERSION);

#if USING_MRD  
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
#else
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
#endif

  ESP_WiFiManager = new ESP_WiFiManager_Lite();

  String AP_SSID = "IOTJ_ROCKETONE";
  String AP_PWD  = "IOTJ_rocketone2025!";
  
  // Set customized AP SSID and PWD
  ESP_WiFiManager->setConfigPortal(AP_SSID, AP_PWD);

  // Optional to change default AP IP(192.168.4.1) and channel(10)
  //ESP_WiFiManager->setConfigPortalIP(IPAddress(192, 168, 120, 1));
  ESP_WiFiManager->setConfigPortalChannel(0);

#if USING_CUSTOMS_STYLE
  ESP_WiFiManager->setCustomsStyle(NewCustomsStyle);
#endif

#if USING_CUSTOMS_HEAD_ELEMENT
  ESP_WiFiManager->setCustomsHeadElement(PSTR("<style>html{filter: invert(10%);}</style>"));
#endif

#if USING_CORS_FEATURE  
  ESP_WiFiManager->setCORSHeader(PSTR("Your Access-Control-Allow-Origin"));
#endif

  // Set customized DHCP HostName
  ESP_WiFiManager->begin(HOST_NAME);
  //Or use default Hostname "ESP-WiFi-XXXXXX"
  //ESP_WiFiManager->begin();
  // M5.dis.clear();
  setLed();
  fakertc = 1741900517558; // TODO: set this via wifi setup?
}

#if USE_DYNAMIC_PARAMETERS
void displayCredentials()
{
  Serial.println(F("\nYour stored Credentials :"));

  for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(F(" = "));
    Serial.println(myMenuItems[i].pdata);
  }
}

void displayCredentialsInLoop()
{
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (int i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if ( i == (NUM_MENU_ITEMS - 1) )
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }
}

#endif

int high = 255;
int mid = 128;
int low = 0;

void setLed() {
  /*
  pixels.clear();
  if (callButtonState) {
      pixels.setPixelColor(0, pixels.Color(high, mid, low));
  } else {
      pixels.setPixelColor(0, pixels.Color(low, high, low));
  }
  pixels.show();
  */
}
void loop()
{
  ESP_WiFiManager->run();
  /*
  if (M5.BtnA.wasReleased()) {
    Serial.println("Button Released");
    
    String data = "";
    if (callButtonState) {
      callButtonState = false;
      data = "event:callbutton_off,ts:" + String(fakertc++);
      setLed();
    } else {
      callButtonState = true;
      data = "event:callbutton_on,ts:" + String(fakertc++);
      setLed();
    }
    MQTT_connect();
    if (Call_Button->publish(data.c_str(), true))
    {
      Serial.print(data);        // T means publishing OK
    } else {
      Serial.print("send to mqtt not ok");
    }
    Serial.print("\n");
  }
  */
  check_status();

#if USE_DYNAMIC_PARAMETERS
  displayCredentialsInLoop();
#endif
  // M5.update();
}
