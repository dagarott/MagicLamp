#define BLUETOOTH_ENABLE 0
#define MQTT_ENABLE 1
#define WIFI_ENABLE 1
#define MESH_ENABLE 0
#define TEMP_HUMIDITY_ENABLE 0
#define SK6812_ENABLE 1
#define DEBUG_ENABLE 1

#if DEBUG_ENABLE
#include "TelnetSpy.h"
TelnetSpy SerialAndTelnet;
#define Serial SerialAndTelnet
#endif

#if WIFI_ENABLE
// BEGIN WIFI
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#define ssid "NETLLAR-Dani" // the SSID of your WiFi network
#define password "86799461" // you wifi password
//END WIFI
#endif

#if MESH_ENABLE
//BEGIN MESH
#include "credentials.h"
#include <ESP8266MQTTMesh.h>
#define FIRMWARE_ID 0x1337
#define FIRMWARE_VER "0.1"
wifi_conn networks[] = NETWORK_LIST;
const char *mesh_password = MESH_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT;
const char *Mqttusername = "hovjocvq";     //The username used to login to the MQTT broker
const char *Mqttpassword = "4n6h6CLEmsa7"; //The password used to login to the MQTT broker
#if ASYNC_TCP_SSL_ENABLED
const uint8_t *mqtt_fingerprint = MQTT_FINGERPRINT;
bool mqtt_secure = MQTT_SECURE;
#if MESH_SECURE
#include "ssl_cert.h"
#endif
#endif
#ifdef ESP32
String ID = String((unsigned long)ESP.getEfuseMac());
#else
String ID = String(ESP.getChipId());
#endif
// Note: All of the '.set' options below are optional.  The default values can be
// found in ESP8266MQTTMeshBuilder.h
ESP8266MQTTMesh mesh = ESP8266MQTTMesh::Builder(networks, mqtt_server, mqtt_port)
                           .setVersion(FIRMWARE_VER, FIRMWARE_ID)
                           .setMeshPassword(mesh_password)
#if ASYNC_TCP_SSL_ENABLED
                           .setMqttSSL(mqtt_secure, mqtt_fingerprint)
#if MESH_SECURE
                           .setMeshSSL(ssl_cert, ssl_cert_len, ssl_key, ssl_key_len, ssl_fingerprint)
#endif //MESH_SECURE
#endif //ASYNC_TCP_SSL_ENABLED
                           .setMqttAuth(Mqttusername, Mqttpassword)
                           .build();

void Callback(const char *topic, const char *msg);
//END MESH
#endif

#if MQTT_ENABLE
// BEGIN MQTT
#include <PubSubClient.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#define mqtt_server "m20.cloudmqtt.com"
#define port 19123
#define dioty_id "hovjocvq"           // email address used to register with DIoTY
#define dioty_password "4n6h6CLEmsa7" // your DIoTY password
#define clientNr "0"                  // used for client id (increment when deploying
// multiple instances at the same time)
// Some project settings
// DIoTY requires topic names to start with your userId, so stick it in the front
#define Modconcat(first, second) first second
#define slash "/"                         // all topics are prefixed with slash and your dioty_id
#define topicConnect "/ESP8266/connected" // topic to say we're alive
#define topicIn "/ESP8266/inTopic"        // topic to switch off power system
#define RGBValueIn "/ESP8266/RGBValue"    // topic to switch off power system
//#define topicOut        "/ESP8266/outTopic"    // topic to subscribe
char msg[50];  // message to publish
int value = 0; // connection attempt
WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char *connectTopic = Modconcat(Modconcat(slash, dioty_id), topicConnect);
const char *inTopic = Modconcat(Modconcat(slash, dioty_id), topicIn);
const char *RGBinTopic = Modconcat(Modconcat(slash, dioty_id), RGBValueIn);
const char *client_id = Modconcat(clientNr, dioty_id);
unsigned long mqttConnectionPreviousMillis = millis();
const long mqttConnectionInterval = 60000;
char cmnd_power_topic[44];
char cmnd_color_topic[44];
///END  MQTT
#endif

#if SK6812_ENABLE
// BEGIN RGBW LEDS
#include "LedStripControl.h"
//END RGBW LEDS
#endif

#if BLUETOOTH_ENABLE
//BEGIN BLUETOOTH
#include <SoftwareSerial.h>
SoftwareSerial blueToothSerial(5, 4);
long previousMillis = 0;
long interval = 8000;
String blueToothBuff;
String RssiVal;
char blueToothChar;
int8_t BuffIndex = 0;
//END BLUETOOTH
#endif

#if TEMP_HUMIDITY_ENABLE
#include "SHT15.h"
#include "TempHumidity.h"
float Temperature = 0;
float Humidity = 0;
#endif

#if BLUETOOTH_ENABLE
void at(char *cmd)
{
  blueToothSerial.write(cmd);
  //Serial.print(cmd);
  while (!blueToothSerial.find("OK"))
    ;
  //Serial.print(".");

  //Serial.println(" .. OK");
}
#endif

#if MQTT_ENABLE
void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    ++value; //connection attempt
    if (mqttClient.connect(client_id, dioty_id, dioty_password))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      snprintf(msg, 50, "Successful connect #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      mqttClient.publish(connectTopic, msg, true);
      // ... and resubscribe
      mqttClient.subscribe(inTopic);
      mqttClient.subscribe(RGBinTopic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // Convert received bytes to a string
  char text[length + 1];
  snprintf(text, length + 1, "%s", payload);

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(text);

  if (strcmp(topic, cmnd_power_topic) == 0)
  {
    //power = strcmp(text, "ON") == 0;
  }
  else if (strcmp(topic, cmnd_color_topic) == 0)
  {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &data = jsonBuffer.parseObject(text);

    if (data.containsKey("state"))
    {
      //power = data["state"] == "ON";
    }

    if (data.containsKey("brightness"))
    {
      const int brightness = data["brightness"];
      if ((0 <= brightness) && (255 >= brightness))
      {
        //lightRed = brightness;
        //lightGreen = brightness;
        //lightBlue = brightness;
      }
    }
    else if (data.containsKey("color"))
    {
      const int r = data["color"]["r"];
      const int g = data["color"]["g"];
      const int b = data["color"]["b"];
      //lightRed = ((0 <= r) && (255 >= r)) ? r : 0;
      //lightGreen = ((0 <= g) && (255 >= g)) ? g : 0;
      //lightBlue = ((0 <= b) && (255 >= b)) ? b : 0;
      // Turn on if any of the colors is greater than 0
      //power = ( (0 < lightRed) || (0 < lightGreen) || (0 < lightBlue) );
    }
  }
}
#endif

#if WIFI_ENABLE
void setupWIFI()
{
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFiManager wifiManager;
  wifiManager.autoConnect("esp8266-setup");
}
void setupOTA()
{
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("lamp-esp8266-ota");

  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("End OTA");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}
#endif

#if DEBUG_ENABLE
void telnetConnected()
{
  Serial.println("Telnet connection established.");
}

void telnetDisconnected()
{
  Serial.println("Telnet connection closed.");
}
#endif
void setup()
{
#if DEBUG_ENABLE
  SerialAndTelnet.setWelcomeMsg("Welcome to the TelnetSpy example\n\n");
  SerialAndTelnet.setCallbackOnConnect(telnetConnected);
  SerialAndTelnet.setCallbackOnDisconnect(telnetDisconnected);
  Serial.begin(74880);
  delay(100); // Wait for serial port
  Serial.setDebugOutput(false);
  Serial.print("\n\nConnecting to WiFi ");
#else
  Serial.begin(115200);
  Serial.println("Booting");
#endif

#if WIFI_ENABLE
  setupWIFI();
  setupOTA();
#endif

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

#if BLUETOOTH_ENABLE
  at("AT"); // check if working, always returns OK
  //at("AT+VERR"); // check if working, always returns OK
  at("AT+ROLE1"); // select master = central
  at("AT+RESET"); // actually more a restart than a reset .. needed after ROLE
  //at("AT+SHOW2"); // include Bluetooth name in response
  at("AT+IMME1"); // "work immediately", not sure what this does
  at("AT+RESET"); // actually more a restart than a reset .. needed after ROLE
  //at("AT+FILT0"); // show all BLE devices, not only HM ones
  delay(1000); // wait a bit, NECESSARY!!
#endif

#if MQTT_ENABLE
  mqttClient.setServer(mqtt_server, port);
  mqttClient.setCallback(callback);
#endif

#if SK6812_ENABLE
  sk6812Init();
#endif
}

void loop()
{
#if DEBUG_ENABLE
  SerialAndTelnet.handle();
#endif

#if WIFI_ENABLE
  ArduinoOTA.handle();
#endif

#if MQTT_ENABLE
  // Reconnect if there is an issue with the MQTT connection
  // const unsigned long mqttConnectionMillis = millis();
  // if ((false == mqttClient.connected()) && (mqttConnectionInterval <= (mqttConnectionMillis - mqttConnectionPreviousMillis)))
  // {
  //   mqttConnectionPreviousMillis = mqttConnectionMillis;
  //   reconnect();
  // }
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
#endif

#if MESH_ENABLE
  if (!mesh.connected())
    return;
#endif

#if TEMP_HUMIDITY_ENABLE
  unsigned long TempHumidityCurrentMillis = millis();
  static unsigned long TempHumidityLastMillis = 0;
  if (TempHumidityCurrentMillis - TempHumidityLastMillis > 2500)
  {
    TempHumidityLastMillis = TempHumidityCurrentMillis;
    TempHumiditySensing(&Temperature, &Humidity);
    Serial.print(Temperature);
    Serial.print(" | ");
    Serial.println(Humidity);
  }
#endif

#if BLUETOOTH_ENABLE
  if (currentMillis - previousMillis > interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    at("AT+DISI?");
  }
  if (blueToothSerial.available() > 0)
  {
    blueToothChar = blueToothSerial.read();
    blueToothBuff.concat(blueToothChar);
    if (blueToothBuff.endsWith("OK+DISCE"))
    {
      //blueToothBuff.concat(blueToothChar);
      //blueToothBuff.concat('\0'); //terminate the string
      Serial.println(blueToothBuff);
      Serial.println("");
      BuffIndex = blueToothBuff.indexOf("EAE83D93F051");
      if (BuffIndex != -1)
      {
        RssiVal = blueToothBuff.substring((BuffIndex + 14), BuffIndex + 17);
        Serial.println("index:");
        Serial.println(BuffIndex);
        Serial.println("Rssi:");
        Serial.println(RssiVal);
        RssiVal = "";
      }
      blueToothBuff = "";
    }
  }
#endif

#if SK6812_ENABLE
  // now = millis();
  // ws2812fx.service();
  // if (now - last_change > TIMER_MS)
  // {
  //   ws2812fx.setMode((ws2812fx.getMode() + 1) % ws2812fx.getModeCount());
  //   last_change = now;
  // }
  FullWhite(125);
#endif
}

#if MESH_ENABLE
/**
 * @brief 
 * 
 * @param topic 
 * @param msg 
 */
void Callback(const char *topic, const char *msg)
{

  if (0 == strcmp(topic, (const char *)ID.c_str()))
  {
    if (String(msg) == "0")
    {
      digitalWrite(LED_PIN, HIGH);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
    }
  }
}
#endif
