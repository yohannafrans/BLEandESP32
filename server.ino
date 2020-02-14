#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <DHT.h>


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "62ac4df8-832c-4362-90c5-abeba580fb01"
BLECharacteristic temhumid (BLEUUID("33137a20-e523-487c-880a-56560add6b8f"), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
BLEDescriptor temhumidDescriptor(BLEUUID((uint16_t)0x2901));

bool clientConnected = false;

#define DHTPIN 23     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);
float kelembaban;
float suhu;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect (BLEServer * pServer) {
    clientConnected = true;
  };
  void onDisconnect (BLEServer * pServer) {
  clientConnected = false;
  }
};


void setup() {
  Serial.begin(115200); 
  Serial.println("Starting BLE work!");
  BLEDevice::init("ESP32 Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer-> createService(SERVICE_UUID);


  pServer->setCallbacks(new MyServerCallbacks());

  pService-> addCharacteristic(&temhumid);
  temhumidDescriptor.setValue("In Degree Celcius");
  temhumid.addDescriptor(&temhumidDescriptor);
  temhumid.addDescriptor(new BLE2902());

  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
  pService-> start(); // Start the sensor if you use DHT11 / DHT22
  pServer-> getAdvertising()->start ();// Starts the discovery of ESP32
  Serial.println("Waiting for Client...");
  dht.begin();
}

void loop () {
  delay(10000);
  if (clientConnected) {  
    suhu = dht.readTemperature();
    kelembaban = dht.readHumidity();
    char suhuconv [8];
    char humidconv [8];
    std::string suhuvalue = dtostrf(suhu, 3, 2, suhuconv);
    std::string kelembabanvalue = dtostrf(kelembaban, 3, 2, humidconv);
    std::string space = " ";
    temhumid.setValue(suhuvalue + space + kelembabanvalue);
    temhumid.notify(); //Send data to client
    if (isnan(suhu) || isnan(kelembaban)) {// isnan, digunakan untuk melakukan cek value pada variable.
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    }
    
  Serial.print(F("Temperature: "));
  Serial.print(suhu);
  Serial.print(F("°C || Humidity: "));
  Serial.print(kelembaban);
  Serial.print(F("%"));
  Serial.println();
  }
}
