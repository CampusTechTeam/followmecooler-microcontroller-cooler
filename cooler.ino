#include <Wire.h>
#include <UnoWiFiDevEd.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(9, DHT11);
DHT dht2(8, DHT11);

unsigned long millisbefore = 0;
unsigned long millisbeforeshort = 0;


#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);


DeviceAddress insideThermometer;

#define CONNECTOR "mqtt"
#define TOPIC "followmecooler/cooler/setstate"

#define FAN 10
#define HEAT 11
#define COOL 12

void setup() {
  Ciao.begin();
  Serial.begin(9600);
  sensors.begin();
  sensors.getAddress(insideThermometer, 0);
  sensors.setResolution(insideThermometer, 9);
  pinMode(FAN, OUTPUT);
  pinMode(COOL, OUTPUT);
  pinMode(HEAT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //Wifi.begin();
  delay(500);
  //Wifi.print("Found Device: ");
  printAddress(insideThermometer);
  millisbefore = millis();
  millisbeforeshort = millis();
  dht.begin();
  dht2.begin();
}

void loop() {
  CiaoData data = Ciao.read(CONNECTOR, TOPIC);
  if (!data.isEmpty()) {
    const char* value = data.get(2);
    if (String(value) == "1") {
      digitalWrite(COOL, HIGH);
      digitalWrite(FAN, HIGH);
      digitalWrite(HEAT, LOW);
      //digitalWrite(LED_BUILTIN, HIGH);
      //Wifi.println("Set Cooler State to: COOL");
    }
    if (String(value) == "2") {
      digitalWrite(HEAT, HIGH);
      digitalWrite(FAN, HIGH);
      digitalWrite(COOL, LOW);
      //digitalWrite(LED_BUILTIN, HIGH);
      //Wifi.println("Set Cooler State to: HEAT");
    }
    if (String(value) == "0") {
      digitalWrite(HEAT, LOW);
      digitalWrite(COOL, LOW);
      digitalWrite(FAN, LOW);
      //digitalWrite(LED_BUILTIN, LOW);
      //Wifi.println("Set Cooler State to: OFF");
    }
    Serial.println(value);

  }
  if (millisbefore > millis()) {
    millisbefore = millis();
  }
  if (millisbeforeshort > millis()) {
    millisbeforeshort = millis();
  }
  if (millisbefore + 5000 < millis()) {
    //Serial.print(String(millisbefore+1000) + " " + String(millis()));
    sensors.requestTemperatures();
    Ciao.write(CONNECTOR, "followmecooler/cooler/intemp", String(int(sensors.getTempC(insideThermometer))));
    Serial.println(String(int(sensors.getTempC(insideThermometer))));

    Ciao.write(CONNECTOR, "followmecooler/cooler/hottemp", String(int(dht.readTemperature())));
    Serial.println(String(int(dht.readTemperature())));

    Ciao.write(CONNECTOR, "followmecooler/cooler/outtemp", String(int(dht2.readTemperature())));
    Serial.println(String(int(dht2.readTemperature())));

    millisbefore = millis();
  }
}





void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Wifi.print("0");
    Wifi.print(deviceAddress[i], HEX);
  }
}
