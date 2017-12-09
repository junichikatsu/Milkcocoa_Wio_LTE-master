#include <WioLTEforArduino.h>
#include <WioLTEClient.h>
#include <stdio.h>
#include <Milkcocoa.h>

#define APN               "soracom.io"
#define USERNAME          "sora"
#define PASSWORD          "sora"


/************************* Your Milkcocoa Setup *********************************/

#define MILKCOCOA_APP_ID      "...YOUR_MILKCOCOA_APP_ID..."
#define MILKCOCOA_DATASTORE   "Wio_LTE"
#define MILKCOCOA_API_KEY     "...YOUR_MILKCOCOA_API_KEY..."
#define MILKCOCOA_API_SECRET  "...YOUR_MILKCOCOA_API_SECRET..."

/************* Milkcocoa Setup (you don't need to change this!) ******************/

#define MILKCOCOA_SERVERPORT  1883

/************ Global State (you don't need to change this!) ******************/

WioLTE Wio;
WioLTEClient WioClient(&Wio);

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;

Milkcocoa* milkcocoa = Milkcocoa::createWithApiKey(&WioClient, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID, MILKCOCOA_API_KEY, MILKCOCOA_API_SECRET);

void onpush(DataElement *elem) {
  SerialUSB.println("onpush");
  SerialUSB.println(elem->getInt("v"));
};

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

  SerialUSB.println(F("Milkcocoa SDK demo"));

  setupLTE();

  SerialUSB.println( milkcocoa->on(MILKCOCOA_DATASTORE, "push", onpush) );
};

void loop() {
  milkcocoa->loop();

  DataElement elem = DataElement();
  elem.setValue("v", 1);

  milkcocoa->push(MILKCOCOA_DATASTORE, &elem);
  delay(7000);
};