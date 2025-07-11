
/*
  RespirAI - Respiratory Monitoring System
  Arduino Uno based respiratory anomaly detection using MLX90614 and BMP280
  Detects apnea, hypopnea, and shallow breathing
  Uses rule-based algorithms (Arduino Uno has limited ML capabilities)

  Hardware connections:
  MLX90614 Thermal Sensor: SDA->A4, SCL->A5, VDD->3.3V, GND->GND
  BMP280 Pressure Sensor: SDA->A4, SCL->A5, VDD->3.3V, GND->GND
  Red LED (Apnea): Pin 7 with 220Ω resistor
  Yellow LED (Hypopnea): Pin 8 with 220Ω resistor  
  Green LED (Normal): Pin 9 with 220Ω resistor
  Blue LED (Shallow): Pin 10 with 220Ω resistor
  Buzzer: Pin 6

  Author: RespirAI Team
  Date: 2025
*/

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_BMP280.h>

// Initialize sensors
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_BMP280 bmp; // I2C interface

// Pin definitions
const int RED_LED = 7;      // Apnea indicator
const int YELLOW_LED = 8;   // Hypopnea indicator
const int GREEN_LED = 9;    // Normal breathing
const int BLUE_LED = 10;    // Shallow breathing
const int BUZZER = 6;       // Audio alert

// Breathing detection parameters
const int SAMPLE_SIZE = 50;           // Number of samples for analysis
const int SAMPLING_INTERVAL = 200;    // 200ms between samples (5Hz)
const float TEMP_THRESHOLD = 0.5;     // Temperature change threshold (°C)
const float PRESSURE_THRESHOLD = 0.3; // Pressure change threshold (hPa)
const int APNEA_THRESHOLD = 20;       // Seconds without breathing
const int HYPOPNEA_THRESHOLD = 50;    // 50% reduction in breathing amplitude
const float SHALLOW_THRESHOLD = 0.3;  // Shallow breathing threshold

// Data storage arrays
float tempReadings[SAMPLE_SIZE];
float pressureReadings[SAMPLE_SIZE];
unsigned long timestamps[SAMPLE_SIZE];
int currentIndex = 0;
bool bufferFull = false;

// Breathing analysis variables
float baselineTemp = 0;
float baselinePressure = 0;
unsigned long lastBreathTime = 0;
unsigned long currentTime = 0;
int breathingRate = 0;
float breathingAmplitude = 0;
bool breathingDetected = false;

// Alert state variables
bool apneaAlert = false;
bool hypopneaAlert = false;
bool shallowAlert = false;
unsigned long lastAlertTime = 0;
const unsigned long ALERT_INTERVAL = 5000; // 5 seconds between alerts

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  Serial.println("RespirAI - Respiratory Monitoring System");
  Serial.println("Initializing sensors...");

  // Initialize MLX90614
  if (!mlx.begin()) {
    Serial.println("Error: Could not find MLX90614 sensor!");
    while (1);
  }

  // Initialize BMP280
  if (!bmp.begin(0x76)) {  // Try address 0x76 first
    if (!bmp.begin(0x77)) { // Then try 0x77
      Serial.println("Error: Could not find BMP280 sensor!");
      while (1);
    }
  }

  // Configure BMP280 for respiratory monitoring
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode
                  Adafruit_BMP280::SAMPLING_X2,     // Temperature oversampling
                  Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,      // Filtering
                  Adafruit_BMP280::STANDBY_MS_500); // Standby time

  // Initialize LED pins
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Turn off all LEDs initially
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  // Startup sequence
  startupSequence();

  // Initialize baseline readings
  calibrateSensors();

  Serial.println("System ready. Starting respiratory monitoring...");
  Serial.println("Temp(°C),Pressure(hPa),Rate(BPM),Amplitude,Status");
}

void loop() {
  currentTime = millis();

  // Read sensors
  float objectTemp = mlx.readObjectTempC();
  float pressure = bmp.readPressure() / 100.0F; // Convert to hPa

  // Store readings in circular buffer
  tempReadings[currentIndex] = objectTemp;
  pressureReadings[currentIndex] = pressure;
  timestamps[currentIndex] = currentTime;

  currentIndex = (currentIndex + 1) % SAMPLE_SIZE;
  if (currentIndex == 0) {
    bufferFull = true;
  }

  // Analyze breathing pattern if we have enough data
  if (bufferFull || currentIndex > 10) {
    analyzeBreathingPattern();
    detectAnomalies();
    updateAlerts();
  }

  // Output data for monitoring
  Serial.print(objectTemp, 2);
  Serial.print(",");
  Serial.print(pressure, 2);
  Serial.print(",");
  Serial.print(breathingRate);
  Serial.print(",");
  Serial.print(breathingAmplitude, 2);
  Serial.print(",");
  Serial.println(getStatusString());

  delay(SAMPLING_INTERVAL);
}

void calibrateSensors() {
  Serial.println("Calibrating sensors... Please breathe normally for 10 seconds.");

  float tempSum = 0;
  float pressureSum = 0;
  int samples = 0;

  unsigned long calibrationStart = millis();

  while (millis() - calibrationStart < 10000) { // 10 seconds calibration
    float temp = mlx.readObjectTempC();
    float pressure = bmp.readPressure() / 100.0F;

    tempSum += temp;
    pressureSum += pressure;
    samples++;

    delay(200);
  }

  baselineTemp = tempSum / samples;
  baselinePressure = pressureSum / samples;

  Serial.print("Baseline Temperature: ");
  Serial.print(baselineTemp, 2);
  Serial.println(" °C");
  Serial.print("Baseline Pressure: ");
  Serial.print(baselinePressure, 2);
  Serial.println(" hPa");
}

void analyzeBreathingPattern() {
  // Calculate breathing rate using peak detection
  int peaks = countPeaks();
  unsigned long timeWindow = getTimeWindow();

  if (timeWindow > 0) {
    breathingRate = (peaks * 60000) / timeWindow; // Convert to breaths per minute
  }

  // Calculate breathing amplitude
  breathingAmplitude = calculateAmplitude();

  // Check if breathing is detected
  breathingDetected = (peaks > 0) && (breathingAmplitude > SHALLOW_THRESHOLD);

  if (breathingDetected) {
    lastBreathTime = currentTime;
  }
}

int countPeaks() {
  if (!bufferFull && currentIndex < 10) return 0;

  int peaks = 0;
  int dataSize = bufferFull ? SAMPLE_SIZE : currentIndex;

  // Combine temperature and pressure signals for better peak detection
  for (int i = 2; i < dataSize - 2; i++) {
    int idx = (currentIndex - dataSize + i + SAMPLE_SIZE) % SAMPLE_SIZE;
    int prev2 = (idx - 2 + SAMPLE_SIZE) % SAMPLE_SIZE;
    int prev1 = (idx - 1 + SAMPLE_SIZE) % SAMPLE_SIZE;
    int next1 = (idx + 1) % SAMPLE_SIZE;
    int next2 = (idx + 2) % SAMPLE_SIZE;

    // Normalized temperature and pressure changes
    float tempChange = abs(tempReadings[idx] - baselineTemp);
    float pressureChange = abs(pressureReadings[idx] - baselinePressure);

    // Combined signal strength
    float signal = tempChange + (pressureChange * 2); // Weight pressure more heavily

    // Peak detection: current value higher than neighbors
    if (signal > TEMP_THRESHOLD && 
        signal > (abs(tempReadings[prev1] - baselineTemp) + abs(pressureReadings[prev1] - baselinePressure) * 2) &&
        signal > (abs(tempReadings[next1] - baselineTemp) + abs(pressureReadings[next1] - baselinePressure) * 2)) {
      peaks++;
    }
  }

  return peaks;
}

float calculateAmplitude() {
  if (!bufferFull && currentIndex < 5) return 0;

  float minVal = 999, maxVal = -999;
  int dataSize = bufferFull ? SAMPLE_SIZE : currentIndex;

  // Find min and max combined signal values
  for (int i = 0; i < dataSize; i++) {
    int idx = (currentIndex - dataSize + i + SAMPLE_SIZE) % SAMPLE_SIZE;
    float combined = abs(tempReadings[idx] - baselineTemp) + 
                    abs(pressureReadings[idx] - baselinePressure) * 2;

    if (combined < minVal) minVal = combined;
    if (combined > maxVal) maxVal = combined;
  }

  return maxVal - minVal;
}

unsigned long getTimeWindow() {
  if (!bufferFull && currentIndex < 2) return 0;

  int dataSize = bufferFull ? SAMPLE_SIZE : currentIndex;
  int firstIdx = (currentIndex - dataSize + SAMPLE_SIZE) % SAMPLE_SIZE;
  int lastIdx = (currentIndex - 1 + SAMPLE_SIZE) % SAMPLE_SIZE;

  return timestamps[lastIdx] - timestamps[firstIdx];
}

void detectAnomalies() {
  // Reset all alerts
  apneaAlert = false;
  hypopneaAlert = false;
  shallowAlert = false;

  // Check for apnea (no breathing for extended period)
  unsigned long timeSinceLastBreath = currentTime - lastBreathTime;
  if (timeSinceLastBreath > (APNEA_THRESHOLD * 1000)) {
    apneaAlert = true;
  }

  // Check for hypopnea (reduced breathing amplitude)
  if (breathingDetected && breathingAmplitude > 0) {
    float normalAmplitude = 1.0; // Expected normal amplitude
    float reductionPercentage = ((normalAmplitude - breathingAmplitude) / normalAmplitude) * 100;

    if (reductionPercentage >= HYPOPNEA_THRESHOLD) {
      hypopneaAlert = true;
    }
  }

  // Check for shallow breathing
  if (breathingDetected && breathingAmplitude < SHALLOW_THRESHOLD) {
    shallowAlert = true;
  }

  // Check breathing rate anomalies
  if (breathingRate > 0) {
    if (breathingRate < 8 || breathingRate > 25) { // Abnormal rates
      if (breathingRate < 8) {
        hypopneaAlert = true; // Very slow breathing
      }
    }
  }
}

void updateAlerts() {
  // Turn off all LEDs first
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  bool alertActive = false;

  if (apneaAlert) {
    digitalWrite(RED_LED, HIGH);
    alertActive = true;
  } else if (hypopneaAlert) {
    digitalWrite(YELLOW_LED, HIGH);
    alertActive = true;
  } else if (shallowAlert) {
    digitalWrite(BLUE_LED, HIGH);
    alertActive = true;
  } else if (breathingDetected) {
    digitalWrite(GREEN_LED, HIGH); // Normal breathing
  }

  // Sound buzzer for critical alerts with rate limiting
  if (alertActive && (currentTime - lastAlertTime > ALERT_INTERVAL)) {
    if (apneaAlert) {
      // Continuous alarm for apnea
      tone(BUZZER, 1000, 1000);
    } else if (hypopneaAlert) {
      // Two beeps for hypopnea
      tone(BUZZER, 800, 200);
      delay(100);
      tone(BUZZER, 800, 200);
    } else if (shallowAlert) {
      // Single beep for shallow breathing
      tone(BUZZER, 600, 300);
    }
    lastAlertTime = currentTime;
  }
}

String getStatusString() {
  if (apneaAlert) return "APNEA";
  if (hypopneaAlert) return "HYPOPNEA";
  if (shallowAlert) return "SHALLOW";
  if (breathingDetected) return "NORMAL";
  return "INITIALIZING";
}

void startupSequence() {
  Serial.println("System startup...");

  // LED test sequence
  digitalWrite(RED_LED, HIGH);
  delay(200);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  delay(200);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(200);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  delay(200);
  digitalWrite(BLUE_LED, LOW);

  // Buzzer test
  tone(BUZZER, 1000, 100);
  delay(200);
  tone(BUZZER, 1500, 100);
  delay(200);

  Serial.println("Hardware test complete.");
}

