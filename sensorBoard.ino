#include <TFMPlus.h>  // Include TFMini Plus Library v1.5.0
#include "BluetoothSerial.h"

// Check if Bluetooth is enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` and enable it.
#endif

#define RXD2 26
#define TXD2 25

TFMPlus tfmP;         // Create a TFMini Plus object
BluetoothSerial SerialBT;

void setup() {
    Serial.begin(115200);  // Initialize terminal serial port
    delay(20);             // Give port time to initialize
    Serial.println("\r\nTFMPlus Library Example - 10SEP2021");

    // Initialize TFMini Plus device serial port
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(20); 
    tfmP.begin(&Serial2);  // Initialize device library object

    // Initialize Bluetooth with device name
    SerialBT.begin("DistanceSensor");

    // Get firmware version
    Serial.print("Firmware version: ");
    if (tfmP.sendCommand(GET_FIRMWARE_VERSION, 0)) {
        Serial.printf("%1u.%1u.%1u\r\n", tfmP.version[0], tfmP.version[1], tfmP.version[2]);
    } else {
        tfmP.printReply();
    }

    // Set data frame-rate to 20Hz
    Serial.print("Data-Frame rate: ");
    if (tfmP.sendCommand(SET_FRAME_RATE, FRAME_20)) {
        Serial.printf("%2uHz.\r\n", FRAME_20);
    } else {
        tfmP.printReply();
    }

    delay(500);  // Wait for half a second
}

// Initialize variables
int16_t tfDist = 0;  // Distance to object in centimeters
int16_t tfFlux = 0;  // Strength or quality of return signal
int16_t tfTemp = 0;  // Internal temperature of Lidar sensor chip

// Use the 'getData' function to pass back device data.
void loop() {
    delay(50);  // Loop delay to match the 20Hz data frame rate
    if (tfmP.getData(tfDist, tfFlux, tfTemp)) {  // Get data from the device
        // Display data on the serial monitor
        Serial.printf("Dist:%04icm Flux:%05i Temp:%2iC\r\n", tfDist, tfFlux, tfTemp);

        // Create JSON object to send via Bluetooth
        String jsonData = "{";
        jsonData += "\"distance\":" + String(tfDist) + ",";
        jsonData += "\"flux\":" + String(tfFlux) + ",";
        jsonData += "\"temperature\":" + String(tfTemp) + "}";
        
        // Send JSON data over Bluetooth
        SerialBT.println(jsonData);
    } else {
        // If the command fails, display the error and HEX data
        tfmP.printFrame();
    }
}