# ESP32 MQTT Client
This project uses an ESP32 microcontroller to read temperature and humidity data from a DHT22 sensor, soil moisture data from an analog sensor, and light intensity from an LDR sensor. The data is published to an MQTT broker. Additionally, the ESP32 subscribes to an MQTT topic to receive commands and control three relays based on the received commands.

## Components Used
- ESP32
- DHT22 sensor
- Soil moisture sensor
- LDR sensor
- Three relays
- WiFi network

## Libraries Required
- WiFi.h
- WiFiMulti.h
- PubSubClient.h
- DHT.h

## Connections
- DHT22 sensor: Data pin to GPIO 4
- Soil moisture sensor: Analog output to GPIO 36 (VP Pin)
- LDR sensor: Analog output to GPIO 5
- Relays:
    Relay 1 control pin to GPIO 18
    Relay 2 control pin to GPIO 19
    Relay 3 control pin to GPIO 21

## MQTT Topics
- Publishing Topics: Temperature: Publishes temperature data in Celsius
- Relative: Publishes relative humidity data
- Luminosity: Publishes light intensity status ("Light" or "Dark")
- Humidity: Publishes soil moisture level in percentage

## Subscription Topic:
- commandTopic: Receives commands to control relays

### Commands
- Temperature Commands:
    HIGHTEMP: Turns on the relay associated with high temperature
    LOWTEMP: Turns off the relay associated with low temperature
    NORMALTEMP: Turns off the relay associated with normal temperature

- Humidity Commands:
    HIGHHUM: Turns off the relay associated with high humidity
    LOWHUM: Turns on the relay associated with low humidity
    NORMALHUM: Turns off the relay associated with normal humidity

- Light Commands:
    LIGHTON: Turns on the light relay
    LIGHTOFF: Turns off the light relay

## Code Explanation

### Setup

Connect to the WiFi network using the setup_wifi() function.
Initialize the MQTT client with the mqtt_server address and set the callback function.
Initialize the DHT22 sensor and set the pin modes for the LDR sensor, soil moisture sensor, and relay control pins.
Ensure all relays are initially turned off.

### Loop

Ensure the client is connected to the MQTT broker.
Publish sensor data to the MQTT broker every 30 seconds:
Read temperature and humidity from the DHT22 sensor.
Read light intensity from the LDR sensor.
Read soil moisture level from the soil moisture sensor.
Process received commands and update the relay states accordingly.

### Callback Function

Handle received commands from the MQTT broker and update the relay states based on the command received.