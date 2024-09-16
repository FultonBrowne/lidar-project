#include <TFMPlus.h> // Include TFMini Plus Library
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "dca96e5a-fe28-4697-9c1a-d67181d8fa8b"
#define CHARACTERISTIC_UUID "77eed7e7-4bf1-478d-a432-440428ed4acf"

#define RXD2 26
#define TXD2 25

TFMPlus tfmP;
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  tfmP.begin(&Serial2);

  // Initialize BLE
  BLEDevice::init("DistanceSensor");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();
}

// Initialize variables
int16_t tfDist = 0;  // Distance to object in centimeters
int16_t tfFlux = 0;  // Strength or quality of return signal
int16_t tfTemp = 0;  // Internal temperature of Lidar sensor chip

void loop() {
    delay(50);  // Loop delay to match the 20Hz data frame rate

  if (tfmP.getData(tfDist, tfFlux, tfTemp)) {
    String jsonData = "{";
    jsonData += "\"distance\":" + String(tfDist) + ",";
    jsonData += "\"flux\":" + String(tfFlux) + ",";
    jsonData += "\"temperature\":" + String(tfTemp) + "}";
    
    // Send data over BLE
    pCharacteristic->setValue(jsonData.c_str());
    pCharacteristic->notify();
  }
  delay(50);
}