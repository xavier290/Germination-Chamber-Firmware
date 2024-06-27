//ESP32 MQTT Client
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti WiFiMulti;
 
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4 // Define the pin for DHT11 sensor
#define DHTTYPE DHT22 // Define the type of DHT sensor

#define RELAY_PIN_1 18
#define RELAY_PIN_2 19
#define RELAY_PIN_3 21

DHT dht(DHTPIN, DHTTYPE);                                                                                                           

#define SOIL_MOISTURE_PIN 36
#define LDRPIN 5 // Define the pin for the LDR module
 
const char* ssid = "TIGO-55F4";
const char* password = "Coding1725";
const char* mqtt_server = "192.168.0.9"; //Change this to the IP address of your PC on your internal network (IPConfig will tell you this)
const char* command_topic = "commandTopic";
 
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


// Relay state variables
bool relayState1 = true;
bool relayState2 = true;
bool relayState3 = true;
 
void setup_wifi() {
 
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  randomSeed(micros());
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message;

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }

  Serial.println();

  // Handle command received from the server
  if (String(topic) == command_topic) {
    Serial.print("Command received: ");
    Serial.println(message); // Add this line to print the received command

    if (message == "HIGHTEMP") {
      // Handle high temperature command
      Serial.println("Received High Temperature Command");
      relayState1 = true;
    } 
    else if (message == "LOWTEMP") {
      // Handle low temperature command
      Serial.println("Received Low Temperature Command");
      relayState1 = false;
    } 
    else if (message == "NORMALTEMP") {
      // Handle normal temperature command
      Serial.println("Received Normal Temperature Command");
      relayState1 = false;
    }
    else if (message == "HIGHHUM") {
      // Handle high humidity command
      Serial.println("Received High Humidity Command");
      relayState2 = false;
    }
    else if (message == "LOWHUM") {
      // Handle low humidity command
      Serial.println("Received Low Hum Command");
      relayState2 = true;
    }
    else if (message == "NORMALHUM") {
      // Handle normal humidity command
      Serial.println("Received Normal Hum Command");
      relayState2 = false;
    }
    else if (message == "LIGHTON") {
      // Handle light on command
      Serial.println("Received Light On Command");
      relayState3 = true;
    }
    else if (message == "LIGHTOFF") {
      // Handle light off command
      Serial.println("Received Light Off Command");
      relayState3 = false;
    }
    else {
      Serial.println("Unknown Command");
    }

    // Update relay states
    digitalWrite(RELAY_PIN_1, relayState1 ? LOW : HIGH);
    digitalWrite(RELAY_PIN_2, relayState2 ? LOW : HIGH);
    digitalWrite(RELAY_PIN_3, relayState3 ? LOW : HIGH);
  }
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(command_topic);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.begin();
  pinMode(LDRPIN, INPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);

  // Set relay pins as output
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);

  // Ensure all relays are initially turned off
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
  digitalWrite(RELAY_PIN_3, HIGH);
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  unsigned long now = millis();
  if (now - lastMsg > 30000) {
    lastMsg = now;

    // Read temperature from DHT22
    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
    } 
    else {
      snprintf(msg, MSG_BUFFER_SIZE, "%.2f", temperature);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("Temperature", msg);
    }

    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
      Serial.println("Failed to read humidity from DHT sensor!");
    }
    else {
      snprintf(msg, MSG_BUFFER_SIZE, "%.2f", humidity);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("Relative", msg);
    }

    // Read LDR status
    int ldrStatus = digitalRead(LDRPIN);
    if (ldrStatus == HIGH) {
      snprintf(msg, MSG_BUFFER_SIZE, "Dark");
    }
    else {
      snprintf(msg, MSG_BUFFER_SIZE, "Light");
    }
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Luminosity", msg);

    // Read soil moisture level
    int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
    float soilMoisturePercent = map(soilMoistureValue, 0, 4095, 100, 0); // Convert analog value to percentage
    snprintf(msg, MSG_BUFFER_SIZE, "%.2f", soilMoisturePercent);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Humidity", msg);
  }
}