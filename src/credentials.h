#define NETWORK_PASSWORD "network password"
#define NETWORK_LIST { \
	WIFI_CONN("NETLLAR-Dani", "86799461", NULL, 0), \
        NULL, \
        }
#define MESH_PASSWORD    "esp8266_sensor_mesh"
#define MQTT_SERVER      "m20.cloudmqtt.com"
#define MQTT_PORT        19123

/* Only used if SSL is enabled */
#define MESH_SECURE      true
#define MQTT_SECURE      false
#define MQTT_FINGERPRINT NULL
//const uint8_t MQTT_FINGERPRINT[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00,0x11,0x22,0x33};

