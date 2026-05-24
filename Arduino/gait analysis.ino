#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// --- 1. CONFIGURATION ---
const int chipSelect = 10; 
const int numSensors = 4;
const int fsrPins[numSensors] = {A0, A1, A2, A3};
const unsigned long SAMPLING_INTERVAL = 50; // 50ms = 20Hz (Exact Target)

// --- ADXL345 ---
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


const float R_DIV = 10000.0; 
const float alpha = 0.5;      
float CALIBRATION_FACTOR = 1; 


float filteredValues[numSensors] = {0, 0, 0, 0};
float forceNewtons[numSensors] = {0, 0, 0, 0};
float sensorOffsets[numSensors] = {0, 0, 0, 0}; 
char fileName[13]; 
unsigned long lastSampleTime = 0; // For precise timing

void setup() {

  Serial.begin(115200); 
  Serial.println("System Starting... (Set Serial Monitor to 115200)");

  // --- ADXL345 ---
  if(!accel.begin()) {
    Serial.println("No ADXL345 detected!");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_16_G);

  // --- SD CARD ---
  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT); 
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed!");
    while (1); 
  }
  Serial.println("Card initialized.");

  strcpy(fileName, "DATA00.CSV");
  for (uint8_t i = 0; i < 100; i++) {
    fileName[4] = i/10 + '0';
    fileName[5] = i%10 + '0';
    if (!SD.exists(fileName)) {
      break; 
    }
  }
  Serial.print("Logging to: ");
  Serial.println(fileName);


  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time,S1_N,S2_N,S3_N,S4_N,Total_N,Accel_X,Accel_Y,Accel_Z,Real_Hz");
    dataFile.close();
  }


  Serial.println("CALIBRATING... STAY STILL (3 Seconds)");
  delay(1000); 
  for (int i = 0; i < 50; i++) {
    for (int s = 0; s < numSensors; s++) {
      int raw = analogRead(fsrPins[s]);
      float force = getForceInNewtons(raw);
      sensorOffsets[s] += force;
    }
    delay(20);
  }
  for (int s = 0; s < numSensors; s++) {
    sensorOffsets[s] /= 50.0;
  }
  Serial.println("READY! GO.");
  lastSampleTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSampleTime >= SAMPLING_INTERVAL) {
    
    unsigned long timeGap = currentMillis - lastSampleTime;
    float realHz = 1000.0 / timeGap; 
    
    lastSampleTime = currentMillis; 
    // --- 1. TIMESTAMP ---
    unsigned long minutes = (currentMillis / 60000);
    unsigned long seconds = (currentMillis / 1000) % 60;
    unsigned long rem_ms = currentMillis % 1000;
    char timeBuffer[12];
    sprintf(timeBuffer, "%02lu:%02lu.%03lu", minutes, seconds, rem_ms);

    // --- 2. ADXL345 ---
    sensors_event_t event; 
    accel.getEvent(&event);

    // --- 3. FSR SENSORS ---
    float totalForceN = 0.0;
    for (int i = 0; i < numSensors; i++) {
      int rawValue = analogRead(fsrPins[i]);
      filteredValues[i] = (alpha * rawValue) + ((1.0 - alpha) * filteredValues[i]);
      float currentForce = getForceInNewtons(filteredValues[i]);
      float cleanForce = currentForce - sensorOffsets[i];
      if (cleanForce < 0) cleanForce = 0;
      forceNewtons[i] = cleanForce * CALIBRATION_FACTOR;
      totalForceN += forceNewtons[i];
    }


    String dataString = "";
    dataString += String(timeBuffer) + ",";
    dataString += String(forceNewtons[0], 2) + ",";
    dataString += String(forceNewtons[1], 2) + ",";
    dataString += String(forceNewtons[2], 2) + ",";
    dataString += String(forceNewtons[3], 2) + ",";
    dataString += String(totalForceN, 2) + ",";
    dataString += String(event.acceleration.x, 2) + ",";
    dataString += String(event.acceleration.y, 2) + ",";
    dataString += String(event.acceleration.z, 2) + ",";
    dataString += String(realHz, 1);

    // --- 5. OUTPUT ---
    File dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println(dataString);
    }
  }
}

float getForceInNewtons(float rawReading) {
  if (rawReading < 10) return 0; 
  float voltage = rawReading * (5.0 / 1023.0);
  if (voltage < 0.001) return 0;
  float fsrResistance = (5.0 - voltage) * R_DIV / voltage;
  float conductance = 1000000.0 / fsrResistance;
  float force = 0;
  if (conductance <= 1000) force = conductance / 80.0;
  else force = conductance / 50.0;
  return force;
}