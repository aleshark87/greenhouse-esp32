#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Ping.h>
#include <ArduinoJson.h>

#define PIN 13 //NeoPixl
#define BAUD_RATE 9600
#define SEALEVELPRESSURE_HPA (1013.25)
#define MSG_LENGTH 1024

// TODO: set valid WiFi Credentials and SSID
const char* ssid = "FASTWEB-B482E1";
const char* pass = "***REMOVED***";

// MQTT Broker
const char* mqtt_server = "test.mosquitto.org";

// Topics
// inTopic: Channel + thingId
const char* inTopic = "ditto-tutorial/my.test:octopus";
const char* outTopic = "ditto-tutorial/my.test:octopus";
const char* thingId = "octopus";

// 5.196.95.208 -> IP of test.mosquitto.org
const IPAddress remote_ip(5, 196, 95, 208);

WiFiClient wifiClient;
PubSubClient client;

// Function Prototypes
void initSensors();
void readSensors();
void wifiConnect();
void mqttConnect();

/**
 * subStr
 *  Create a substring from a arbitrary input_string.
 */
char* subStr (const char* input_string, char* separator, int segment_number) {
    char *act, *sub, *ptr;
    static char copy[100];
    int i;

    strcpy(copy, input_string);
    for (i = 1, act = copy; i <= segment_number; i++, act = NULL)
    {
        sub = strtok_r(act, separator, &ptr);
        if (sub == NULL) break;
    }
    return sub;
}

/**
 * Callback for MQTT incoming message
 */
void messageReceived(char* topic, byte* payload, unsigned int length) {
    Serial.println();
}

/**
 * Setup the octopus: initialize the sensors and connect to Wifi and MQTT.
 */
void setup() {
    Serial.begin(BAUD_RATE);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    Serial.println("Initializing board ...");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    //initSensors();
    wifiConnect();
    delay(100);

    Serial.println("[info] - Pinging Host.");
    if (Ping.ping(remote_ip)){
        Serial.println("[info] - Ping successfull.");
    } else {
        Serial.print("[error] - Host not reachable - Wifi Status: ");
        Serial.print(WiFi.status());
        Serial.println();
    }
    delay(100);
    
    client.setServer(mqtt_server, 1883);
    delay(100);
    client.setClient(wifiClient);
    delay(100);
    client.setCallback(messageReceived);
    delay(100);

    Serial.println("[info] - Setup done. Starting MQTT Connectionloop.");

}

/**
 * Main loop.
 */
void loop() {
  
    if (!client.connected()) {
        mqttConnect();
    }
    client.loop();
    //readSensors();
  
}

/**
 * wifiConnect
 *  Connect to Wifi.
 */
void wifiConnect(){
    delay(10);
    WiFi.begin(ssid, pass);
    Serial.print("[info] - Connecting to Wifi: ");
    Serial.print(ssid);
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("[info] - Connected, IP address: ");
    Serial.println(WiFi.localIP());
}

/**
 * mqttConnect
 *  Connect to the mqtt Broker.
 */
void mqttConnect(){
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("[info] - Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(thingId)) {
            Serial.println("connected!");
            Serial.print("[info] - Subscribed on: ");
            Serial.print(inTopic);
            Serial.println();
            // Once connected, publish an announcement...
            // client.publish(outTopic, "Octopus is online, Baby!");
            // ... and resubscribe
            client.subscribe(inTopic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 2 seconds");
            delay(2000);
        }
    }
}

/**
 * readSensors
 *  Read sensors of octopus board and push them to the globally initialized mqtt topic.
 */
void readSensors(){
    // Readable sensors -> reduced on temp and altitude
    

    //client.publish(outTopic, jsonChar);

}