#include <Arduino.h>
#include <RCSwitch.h>
#include <ESP8266WiFi.h>
#include<PubSubClient.h>
#include <WiFiManager.h>
RCSwitch mySwitch = RCSwitch();

const char *mqtt_server = "192.168.1.140";
const int mqtt_port = 1883;

const char *state_topic = "rf_sniffer/device1/state";

WiFiClient espClient;
PubSubClient client(espClient);

void pulse(int N){
  for (int i = 0; i < N; i++){
    digitalWrite(2, LOW); // Toggle the onboard LED  if serial is available - Optional

    delay(300);

    digitalWrite(2, HIGH);

    delay(300);
  }
}

void connectToMQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    // Attempt to connect
    if (client.connect("ESP8266Client2")) {
      Serial.println("Connected to MQTT");
      // Subscribe to the command topic
      
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  mySwitch.enableReceive(0);  // Receiver input on interrupt 0 (D2)
  pinMode(2, OUTPUT); // D13 as output- Optional
  digitalWrite(2, HIGH); 
  WiFiManager wifiManager;
  wifiManager.autoConnect("RF Sniffer");

  client.setServer(mqtt_server, mqtt_port);
  connectToMQTT();
  pulse(10);
  
}

void loop() {

  if (mySwitch.available()) {
    Serial.print("Received ");
    unsigned long value = mySwitch.getReceivedValue();
    Serial.print( value );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("Bit ");
    Serial.print(" // ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );
    pulse(1);
    String payload = String(value);
    client.publish(state_topic, payload.c_str());
    mySwitch.resetAvailable();
  }
  client.loop();
}