/*
 * Measuring Noise Levels with Arduino Using the KE-ZS-BZ-TTL-05 Sensor
 * Author: Phan Khac Hai (2023 Oct,05) https://khphan.com/
 * Read data from sensor by using AltSoftSerial Pin 8 and 9
 * Wiring: 
 * Sensor     ----- Arduino
 * Rx         ----- 9 
 * Tx         ----- 8 
 * +5V        ----- 5V
 * GND        ----- GND
 * The getNoiseLevel function is responsible for communicating with the sound sensor. 
 * It sends the read command, receives and processes the sensor data, and calculates 
 * the noise level in decibels (dB).
 * Hex command format: 0x01 0x03 0x00 0x00 0x00 0x01 0x84 0x0A
 * Data receive format: 0x01 0x03 0x02 0x02 0xC9 0x79 0x72
 * where 0x02 0xC9 is the reading value of noise
 * 02C9 = 713 = 71.3 dB
 */
 
#include "AltSoftSerial.h"
AltSoftSerial SerialVW; // Pin 8 and 9

const byte COMMAND_READ_NOISE[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};

void setup() {
  Serial.begin(9600);
  SerialVW.begin(9600);
}

void loop() {
  float noise_dB = getNoiseLevel();
  if (noise_dB >= 0) {
    Serial.print("Noise Level: ");
    Serial.println(noise_dB, 1); // Display with one decimal place
    Serial.println(" dB");
  }
  delay(500);// Set the delay to match the slowest or highest response time (1.5 and 0.5 seconds)
}

float getNoiseLevel() {
  uint8_t input[200];
  uint8_t output[2];
  int inputCount = 0;
  int outputCount = 0;
  bool inputComplete = false;
  bool dataStart = false;
  bool dataComplete = false;
  uint16_t decValue = 0;

  SerialVW.write(COMMAND_READ_NOISE, sizeof(COMMAND_READ_NOISE));

  while (SerialVW.available() > 0) {
    input[inputCount] = SerialVW.read();
    if (inputCount == 2 && !inputComplete) {
      dataStart = true;
      dataComplete = false;
    }
    if (dataStart && !dataComplete && inputCount > 2) {
      output[outputCount] = input[inputCount];
      outputCount++;
      if (outputCount >= 2) {
        dataStart = false;
        dataComplete = true;
      }
    }
    if (inputCount >= 6) {
      inputComplete = true;
    }
    inputCount++;
  }

  if (inputComplete && dataComplete) {
    decValue = (output[0] << 8) | output[1];
    float noise_dB = static_cast<float>(decValue) / 10.0;
    return noise_dB;
  }
  return -1; // Data is incomplete or incorrect
}
