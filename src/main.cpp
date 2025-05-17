#include <Arduino.h>
#include <Wire.h>
#include "BluetoothA2DPSink.h"

// === Pin Config ===
const uint8_t I2S_SCK   = 5;
const uint8_t I2S_WS    = 25;
const uint8_t I2S_SDOUT = 26;

const uint8_t I2C_SDA   = 21;
const uint8_t I2C_SCL   = 22;

const uint8_t TAS5805M_ADDR_FAULT = 27;
const uint8_t TAS5805M_PDN        = 14;

BluetoothA2DPSink a2dp_sink;

void initTAS5805M() {
    delay(5);  // Wait after PDN high (per datasheet)

    // Go to Page 0
    Wire.beginTransmission(0x58);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission();

    // Go to Book 0
    Wire.beginTransmission(0x58);
    Wire.write(0x7F);
    Wire.write(0x00);
    Wire.endTransmission();

    delay(5);

    // Set device to Hi-Z and enable DSP
    Wire.beginTransmission(0x58);
    Wire.write(0x03);
    Wire.write(0x10);  // Set D[1:0]=10 for Hi-Z (could vary based on your needs)
    Wire.endTransmission();

    delay(5);

    // Set DSP coefficients (example: set switching frequency, volume, etc.)
    Wire.beginTransmission(0x58);
    Wire.write(0x02);
    Wire.write(0x00); // 768kHz BD modulation
    Wire.endTransmission();

    Wire.beginTransmission(0x58);
    Wire.write(0x53);
    Wire.write(0x60); // Set loop bandwidth
    Wire.endTransmission();

    // Set to play mode
    Wire.beginTransmission(0x58);
    Wire.write(0x03);
    Wire.write(0x03); // Play mode
    Wire.endTransmission();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Booting...");

    Wire.begin(I2C_SDA, I2C_SCL);

    pinMode(TAS5805M_ADDR_FAULT, INPUT);
    pinMode(TAS5805M_PDN, OUTPUT);

    digitalWrite(TAS5805M_PDN, LOW); // Ensure power-down
    delay(100);
    digitalWrite(TAS5805M_PDN, HIGH); // Power on device
    delay(10); // Wait for internal LDOs to stabilize

    initTAS5805M(); // Initialize I2C and DSP

    // Setup I2S
    a2dp_sink.set_pin_config({
        .bck_io_num   = I2S_SCK,
        .ws_io_num    = I2S_WS,
        .data_out_num = I2S_SDOUT,
        .data_in_num  = I2S_PIN_NO_CHANGE
    });

    a2dp_sink.start("MyMusic");
    Serial.println("Bluetooth A2DP streaming started.");
}

void loop() {
    if (digitalRead(TAS5805M_ADDR_FAULT) == LOW) {
        Serial.println("⚠️ TAS5805M Fault Detected!");
    }
    delay(500);
}
