#include <PinSetup.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Ping.h>
#include <ArduinoJson.h>
#include <Actuators/Led.h>
#include <Sensors/PhotoResistor.h>
#include <Sensors/DHT11Sensor.h>
#include <State.h>
#include <Ticker.h>

#define BAUD_RATE 9600
#define MSG_LENGTH 1024
#define TEMP_TRESHOLD 24.3


// Sensors
PhotoResistor* photoResistor;
DHT11Sensor* dht11Sensor;
// Actuators
Led* led;
WiFiClient wifiClient;
PubSubClient client;
Ticker ticker;
const float tickerPeriod = 1.0;
volatile boolean tickerFlag;
int state;

// valid WiFi Credentials and SSID
const char* ssid = "Redmi";
const char* pass = "stefano34";
// MQTT Broker
const char* mqtt_server = "test.mosquitto.org";
const char* thingId = "com.project.thesis:greenhouse01";
// Topics
const char* inTopic = "com.greenhouse.notification/com.project.thesis:greenhouse01";
const char* outTopic = "com.greenhouse/com.project.thesis:greenhouse01";

// 5.196.95.208 -> IP of test.mosquitto.org
const IPAddress remote_ip(5, 196, 95, 208);

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

void messageReceived(char* topic, byte* payload, unsigned int length) {
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);
    if(doc.size() > 0 && doc.containsKey("path") && doc.containsKey("value")){
        const char* path = doc["path"];
        const char* payload = doc["value"];

        char* command = subStr(path, "/", 3);

        if (strcmp(command, "switchLight") == 0){
            // Message with subject LED and payload ON
            if((strcmp(payload, "on") == 0) || (strcmp(payload, "ON") == 0)){
                led->on();
            }
            else{
                // Message with subject LED and payload OFF
                if((strcmp(payload, "off") == 0) || (strcmp(payload, "OFF") == 0)){
                    led->off();
                }
            }
        }
    }
    else{
        Serial.println("[error] - Deserializing Message Arrived.");
    }
}

void initSensors(){
    dht11Sensor = new DHT11Sensor(DHT_PIN);
    led = new Led(LED_PIN);
    led->off();
    photoResistor = new PhotoResistor(PHOTORESISTOR_PIN);
}

void tickerINT() {
  tickerFlag = true;
}

void wifiConnect(){
    delay(10);
    WiFi.begin(ssid, pass);
    Serial.print("[info] - Connecting to Wifi: ");
    Serial.print(ssid);
    int wifiNotConnectedCounter = 0;
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
        wifiNotConnectedCounter++;
        if(wifiNotConnectedCounter == 10){
            ESP.restart();
        }
    }
    Serial.println();
    Serial.print("[info] - Connected, IP address: ");
    Serial.println(WiFi.localIP());
}


void mqttConnect(){
    // Loop until we're reconnected
    int counter = 0;
    while (!client.connected()) {
        counter++;
        Serial.print("[info] - Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(thingId)) {
            Serial.println("connected!");
            Serial.print("[info] - Subscribed on: ");
            Serial.print(inTopic);
            Serial.println();
            client.subscribe(inTopic);
        } else {
            //If it tries too many times, maybe wifi connection is gone. So restart the ESP
            if(counter >= 10){
                ESP.restart();
            }
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 2 seconds");
            delay(2000);
        }
    }
}

void sendHighTemperatureEvent(double temperature){
    DynamicJsonDocument doc(MSG_LENGTH);
    doc["thingId"] = thingId;
    //messageType
    doc["type"] = "event";
    //Sensor Data
    doc["temperature"] = temperature;
    char jsonChar[100];
    serializeJson(doc, jsonChar);
    Serial.println(jsonChar);
    client.publish(outTopic, jsonChar);
}

void readSensors(){
    DynamicJsonDocument doc(MSG_LENGTH);
    doc["thingId"] = thingId;
    doc["type"] = "update";
    double temperature = dht11Sensor->getTemperature();
    doc["temperature"] = temperature;
    if(temperature > TEMP_TRESHOLD){
        sendHighTemperatureEvent(temperature);
    }
    doc["humidity"] = dht11Sensor->getHumidity();
    doc["brightness"] = photoResistor->getBrightness();
    doc["light"] = led->getState();
    char jsonChar[150];
    serializeJson(doc, jsonChar);
    //Serial.println(jsonChar);
    client.publish(outTopic, jsonChar);
}

void setup() {
    Serial.begin(BAUD_RATE);
    client.setBufferSize(2048);
    Serial.println("Initializing board ...");
    initSensors();
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    wifiConnect();
    delay(100);

    Serial.println("[info] - Pinging Host.");
    if (Ping.ping(remote_ip)){
        Serial.println("[info] - Ping successfull.");
    } else {
        Serial.print("[error] - Host not reachable - Wifi Status: ");
        Serial.print(WiFi.status());
        Serial.println();
        state = S_CONNERROR;
    }
    delay(100);
    
    client.setServer(mqtt_server, 1883);
    delay(100);
    client.setClient(wifiClient);
    delay(100);
    client.setCallback(messageReceived);
    delay(100);

    Serial.println("[info] - Setup done. Starting MQTT Connectionloop.");
    ticker.attach(tickerPeriod, tickerINT); tickerFlag = false;
}

void waitForNextTick(){
    while(!tickerFlag) {}
    tickerFlag = false;
}

void step(){
    if(client.connected()){
      state = S_CONNOK;
    }
    else{
        state = S_CONNERROR;
    }
    switch(state){
        case S_CONNERROR:
            mqttConnect();
            break;
        case S_CONNOK:
            client.loop();
            readSensors();
            break;
    }
}

void loop() {
    waitForNextTick();
    step();
}

