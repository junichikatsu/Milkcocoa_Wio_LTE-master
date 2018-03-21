#include <WioLTEforArduino.h>
#include <WioLTEClient.h>
#include <stdio.h>
#include <Milkcocoa.h>

#define APN               "soracom.io"
#define USERNAME          "sora"
#define PASSWORD          "sora"

#define SENSOR_PIN        (WIOLTE_D38)

#define ID                "12345678"    // ブラウザで情報を見るためのID

/************************* Your Milkcocoa Setup *********************************/

#define MILKCOCOA_APP_ID      "...YOUR_MILKCOCOA_APP_ID..."
#define MILKCOCOA_DATASTORE   "environment"

/************* Milkcocoa Setup (you don't need to change this!) ******************/

#define MILKCOCOA_SERVERPORT  1883

/************ Global State (you don't need to change this!) ******************/

WioLTE Wio;
WioLTEClient WioClient(&Wio);

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;

Milkcocoa milkcocoa = Milkcocoa(&WioClient, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID);

void setupLTE() {
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");

  SerialUSB.println("### I/O Initialize.");
  Wio.Init();

  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(5000);

  SerialUSB.println("### Turn on or reset.");
  if (!Wio.TurnOnOrReset()) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  SerialUSB.println("### Connecting to \""APN"\".");
  delay(5000);
  if (!Wio.Activate(APN, USERNAME, PASSWORD)) {
    SerialUSB.println("### ERROR! ###");
    return;
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  setupLTE();

  TemperatureAndHumidityBegin(SENSOR_PIN);
}

void loop() {
  float temp;
  float humi;

  if (!TemperatureAndHumidityRead(&temp, &humi)) {
    SerialUSB.println("ERROR!");
  }
  else
  {
    SerialUSB.print("Current humidity = ");
    SerialUSB.print(humi);
    SerialUSB.print("%  ");
    SerialUSB.print("temperature = ");
    SerialUSB.print(temp);
    SerialUSB.println("C");

    milkcocoa.loop();

    DataElement elem = DataElement();

    elem.setValue("temperature", temp);
    elem.setValue("humidity", humi);
    elem.setValue("id", ID);

    milkcocoa.push(MILKCOCOA_DATASTORE, &elem);
  }

  delay(7000);
}

////////////////////////////////////////////////////////////////////////////////////////
//

int TemperatureAndHumidityPin;

void TemperatureAndHumidityBegin(int pin)
{
  TemperatureAndHumidityPin = pin;
  DHT11Init(TemperatureAndHumidityPin);
}

bool TemperatureAndHumidityRead(float* temperature, float* humidity)
{
  byte data[5];

  DHT11Start(TemperatureAndHumidityPin);
  for (int i = 0; i < 5; i++) data[i] = DHT11ReadByte(TemperatureAndHumidityPin);
  DHT11Finish(TemperatureAndHumidityPin);

  if(!DHT11Check(data, sizeof (data))) return false;
  if (data[1] >= 10) return false;
  if (data[3] >= 10) return false;

  *humidity = (float)data[0] + (float)data[1] / 10.0f;
  *temperature = (float)data[2] + (float)data[3] / 10.0f;

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////
//

void DHT11Init(int pin)
{
  digitalWrite(pin, HIGH);
  pinMode(pin, OUTPUT);
}

void DHT11Start(int pin)
{
  // Host the start of signal
  digitalWrite(pin, LOW);
  delay(18);

  // Pulled up to wait for
  pinMode(pin, INPUT);
  while (!digitalRead(pin)) ;

  // Response signal
  while (digitalRead(pin)) ;

  // Pulled ready to output
  while (!digitalRead(pin)) ;
}

byte DHT11ReadByte(int pin)
{
  byte data = 0;

  for (int i = 0; i < 8; i++) {
    while (digitalRead(pin)) ;

    while (!digitalRead(pin)) ;
    unsigned long start = micros();

    while (digitalRead(pin)) ;
    unsigned long finish = micros();

    if ((unsigned long)(finish - start) > 50) data |= 1 << (7 - i);
  }

  return data;
}

void DHT11Finish(int pin)
{
  // Releases the bus
  while (!digitalRead(pin)) ;
  digitalWrite(pin, HIGH);
  pinMode(pin, OUTPUT);
}

bool DHT11Check(const byte* data, int dataSize)
{
  if (dataSize != 5) return false;

  byte sum = 0;
  for (int i = 0; i < dataSize - 1; i++) {
    sum += data[i];
  }

  return data[dataSize - 1] == sum;
}

////////////////////////////////////////////////////////////////////////////////////////