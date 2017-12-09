Milkcocoa Wio LTE SDK
=====

[Milkcocoa](https://mlkcca.com/) SDK working on [Arduino core for Wio LTE](https://github.com/SeeedJP/WioLTEforArduino).

Works with the Wio LTE Arduino platforms, and anything that supports Arduino's Client interface.


## How To Use

Include library(`#include <Milkcocoa.h>`), and write a code like below.

```
// 'client' is Ethernet/WiFi Client
Milkcocoa milkcocoa = Milkcocoa(&client, "milkcocoa_app_id.mlkcca.com", 1883, "milkcocoa_app_id", "mqtt_client_id");

void setup() {
 	//"on" API was able to call in setup
	milkcocoa.on("milkcocoa_datastore_name", "push", onpush);
}

void loop() {
	//milkcocoa.loop must be called in loop()
	milkcocoa.loop();

	//push
	DataElement elem = DataElement();
	elem.setValue("name", "Milk");
	elem.setValue("age", 35);
	milkcocoa.push("milkcocoa_datastore_name", &elem);

	delay(10000);
}

void onpush(DataElement *elem) {
  Serial.println(elem->getString("name"));
  Serial.println(elem->getInt("age"));
  // Output:
  // Milk
  // 35
};
```

### Using API Key

If you use Milkcocoa API Key Authantication, please use `createWithApiKey`.

```
Milkcocoa *milkcocoa = Milkcocoa::createWithApiKey(&client, "milkcocoa_app_id.mlkcca.com", 1883, "milkcocoa_app_id", "mqtt_client_id", "API_KEY", "API_SECRET");
```

### onメソッドでイベント監視だけ行う場合

接続を維持するために、10秒から20秒間隔でpingを送信する必要があります。

```
void setup() {
 	//"on" API was able to call in setup
	milkcocoa.on("milkcocoa_datastore_name", "push", onpush);
}

void loop() {
	//milkcocoa.loop must be called in loop()
	milkcocoa.loop();
	milkcocoa.ping()
	delay(10000);
}

void onpush(DataElement *elem) {
  Serial.println(elem->getString("name"));
  Serial.println(elem->getInt("age"));
  // Output:
  // Milk
  // 35
};

```

## Examples

- [Simple Wio LTE Example](https://github.com/junichikatsu/Milkcocoa_Wio_LTE-master/blob/master/examples/milkcocoa_wio_lte/milkcocoa_wio_lte.ino): simple test of `push()` and `on("push")`.
- [Wio LTE API Key Authantication Example](https://github.com/junichikatsu/Milkcocoa_Wio_LTE-master/blob/master/examples/milkcocoa_wio_lte_apikey_auth/milkcocoa_wio_lte_apikey_auth.ino): auth with Milkcocoa API Key


## LICENSE

MIT



以下はCopyright (c) 2015 Technical Rockstars.

- Milkcocoa.h
- Milkcocoa.cpp

### Using

- https://github.com/adafruit/Adafruit_MQTT_Library
- https://github.com/interactive-matter/aJson

