#include <Arduino.h>
#include <Wire.h>
#include "BluetoothA2DPSink.h"

// === Pin Config ===
const uint8_t I2S_SCK  = 5;
const uint8_t I2S_WS   = 25;
const uint8_t I2S_SDOUT = 26;

const uint8_t I2C_SDA = 21;
const uint8_t I2C_SCL = 22;

const uint8_t TAS5805M_ADDR_FAULT = 27;
const uint8_t TAS5805M_PDN = 14;

BluetoothA2DPSink a2dp_sink;

void initTAS5805M() {
    // Optional I2C init stub for TAS5805M config
    Wire.beginTransmission(0x58); // TAS5805M default I2C address (check your datasheet)
    Wire.write(0x00); // Example register
    Wire.write(0x01); // Example value
    Wire.endTransmission();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Booting...");

    // I2C init
    Wire.begin(I2C_SDA, I2C_SCL);

    // Control pin setup
    pinMode(TAS5805M_ADDR_FAULT, INPUT);
    pinMode(TAS5805M_PDN, OUTPUT);
    digitalWrite(TAS5805M_PDN, HIGH); // Power on TAS5805M

    // Optional: init I2C configuration of TAS5805M
    initTAS5805M();

    // I2S pin config for ESP32 audio output
    a2dp_sink.set_pin_config({
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_SDOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    });

    // Start A2DP sink
    a2dp_sink.start("MyMusic");
    Serial.println("Bluetooth A2DP streaming started.");
}

void loop() {
    if (digitalRead(TAS5805M_ADDR_FAULT) == LOW) {
        Serial.println("⚠️ TAS5805M Fault Detected!");
    }
    delay(500);
}
