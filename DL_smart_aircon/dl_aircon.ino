#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define IR_PIN 14
#define LED_GREEN 12
#define LED_YELLOW 13
#define LED_RED 4
#define LED_BLUE 5

const char* wifi_ssid = "";
const char* wifi_pass = "";

const char* mqtt_server = "";
const char* mqtt_user = "";
const char* mqtt_pass = "";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (32)
char msg[MSG_BUFFER_SIZE];
int value = 0;

IRsend irsend(IR_PIN);

unsigned long millis_last = 0;
unsigned long default_signal = 0x12110010;


void setup_wifi() {

  delay(25);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_RED, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(LED_RED, HIGH);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  digitalWrite(LED_GREEN, HIGH);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
}


void callback(char* topic, byte* payload, unsigned int length) {
  char* command_hex = "";
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    command_hex[i] = (char)payload[i];
  }
  Serial.println();

  int command = (int)strtol(command_hex, NULL, 0);
  
  irsend.sendNEC((unsigned long)command, 32);
  
  digitalWrite(LED_BLUE, HIGH);
  delay(200);
  digitalWrite(LED_BLUE, LOW);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    digitalWrite(LED_YELLOW, HIGH);
    delay(200);
    digitalWrite(LED_YELLOW, LOW);
    Serial.print("Attempting MQTT connection...");
    String clientId = "DL_AIRCON_N82";
    // Attempt to connect
    //if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass, "unexpected exit", 1, true)) {
    if (client.connect(clientId.c_str())){
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("aircon/stat", "Ready.");
      // ... and resubscribe
      client.subscribe("aircon/cmnd");
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);
      delay(200);
      digitalWrite(LED_GREEN, LOW);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
      digitalWrite(LED_YELLOW, LOW);
      delay(4500);
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(IR_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  digitalWrite(LED_RED, HIGH);
  setup_wifi();
  digitalWrite(LED_RED, LOW);
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
  
  irsend.begin();
  Serial.println("Ready.");
  
  delay(200);
  digitalWrite(LED_GREEN, HIGH);
  delay(200);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(2, HIGH);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}