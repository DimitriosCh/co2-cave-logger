/*
 * CO2 Data Logger with Temperature Monitoring
 * 
 * This code implements a low-power CO2 data logging system using:
 * - Seeeduino (Arduino-compatible board optimized for low power consumption)
 * - COZIR CO2 sensor
 * - DS1337 Real-Time Clock (RTC)
 * - SD card storage
 * - Temperature sensor (analog)
 * 
 * The system logs CO2 concentration, temperature, date and time every 3 hours
 * to an SD card, then enters deep sleep mode to conserve battery power.
 * 
 * Author: D.Christaras, C. Pennos
 * Date: January 2016
 * Version: 1.0
 * License: MIT
 */

// Library includes
#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include "cozir.h"
#include <DS1337.h>
#include <Sleep_n0m1.h>

// ========== PIN DEFINITIONS ==========
#define LED_PIN 13          // Built-in LED for status indication
#define SENSOR_POWER_PIN 9  // Power control for sensors
#define SD_POWER_PIN 4      // Power control for SD card
#define SD_CS_PIN 10        // SD card chip select pin
#define COZIR_RX_PIN 7      // Arduino RX pin connected to COZIR TX
#define COZIR_TX_PIN 8      // Arduino TX pin connected to COZIR RX
#define TEMP_SENSOR_PIN 0   // Analog pin for temperature sensor

// ========== CONFIGURATION ==========
const unsigned long SLEEP_TIME_MS = 10800000; // 3 hours in milliseconds (10800000 ms)
const char* LOG_FILENAME = "retziki.txt";      // SD card log file name
const float REFERENCE_VOLTAGE = 3.3;          // Reference voltage for ADC
const int ADC_RESOLUTION = 1024;              // 10-bit ADC resolution

// ========== GLOBAL OBJECTS ==========
Sleep sleep;                                          // Sleep management object
SdFat sd;                                            // SD card file system object
SdFile logFile;                                      // Log file object
DS1337 RTC;                                          // Real-time clock object
SoftwareSerial cozirSerial(COZIR_RX_PIN, COZIR_TX_PIN); // Software serial for COZIR
COZIR cozirSensor(cozirSerial);                      // COZIR CO2 sensor object

/*
 * Initial setup function - runs once at startup
 */
void setup() {
  // Initialize LED pin for status indication
  pinMode(LED_PIN, OUTPUT);
  
  // Power up sensors and SD card
  powerUpPeripherals();
  
  // Initialize SD card
  initializeSDCard();
  
  // Signal successful initialization
  signalInitialization();
  
  // Initial delay to allow sensors to stabilize
  delay(6000);
}

/*
 * Main loop - runs continuously
 */
void loop() {
  // Visual indication of wake-up and data collection
  blinkStatusLED(4);
  
  // Power up peripherals
  powerUpPeripherals();
  
  // Allow sensors time to stabilize after power-up
  delay(6000);
  
  // Collect sensor data
  logSensorData();
  
  // Power down peripherals to save energy
  powerDownPeripherals();
  
  // Enter deep sleep mode
  enterSleepMode();
}

/*
 * Power up sensors and SD card
 */
void powerUpPeripherals() {
  digitalWrite(SENSOR_POWER_PIN, HIGH);  // Power up sensors
  digitalWrite(SD_POWER_PIN, LOW);       // Power up SD card (active low)
}

/*
 * Power down sensors and SD card to save energy
 */
void powerDownPeripherals() {
  digitalWrite(SENSOR_POWER_PIN, LOW);   // Power down sensors
  digitalWrite(SD_POWER_PIN, HIGH);      // Power down SD card (active low)
}

/*
 * Initialize SD card and check for errors
 */
void initializeSDCard() {
  if (!sd.begin(SD_CS_PIN, SPI_HALF_SPEED)) {
    sd.initErrorHalt();  // Halt if SD card initialization fails
  }
}

/*
 * Signal successful initialization with LED
 */
void signalInitialization() {
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
}

/*
 * Blink status LED specified number of times
 * @param count: Number of blinks
 */
void blinkStatusLED(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
  }
}

/*
 * Read temperature from analog sensor
 * Converts analog reading to temperature in Celsius
 * @return: Temperature in degrees Celsius
 */
float readTemperature() {
  int analogReading = analogRead(TEMP_SENSOR_PIN);
  float voltage = (analogReading * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
  
  // Convert voltage to temperature (assuming TMP36 or similar sensor)
  // TMP36: Vout = 0.5V at 0°C, 10mV/°C scale factor
  float temperatureC = (voltage - 0.5) * 100.0;
  
  return temperatureC;
}

/*
 * Read CO2 concentration from COZIR sensor
 * @return: CO2 concentration in ppm
 */
int readCO2() {
  // Optional: Set sensor to polling mode for single reading
  // cozirSensor.SetOperatingMode(CZR_POLLING);
  // delay(4000);
  
  return cozirSensor.CO2();
}

/*
 * Log sensor data to SD card with timestamp
 */
void logSensorData() {
  // Get current timestamp from RTC
  DateTime currentTime = RTC.now();
  
  // Read sensors
  float temperature = readTemperature();
  int co2Concentration = readCO2();
  
  // Open log file for writing (create if doesn't exist, append to end)
  if (!logFile.open(LOG_FILENAME, O_RDWR | O_CREAT | O_AT_END)) {
    // If file opening fails, we could add error handling here
    // For now, continue without logging this data point
    return;
  }
  
  // Write data to file in tab-separated format
  // Format: DD/MM/YYYY    HH:MM    Temperature    CO2
  logFile.print(currentTime.date(), DEC);
  logFile.print('/');
  logFile.print(currentTime.month(), DEC);
  logFile.print('/');
  logFile.print(currentTime.year(), DEC);
  logFile.print('\t');
  
  logFile.print(currentTime.hour(), DEC);
  logFile.print(':');
  if (currentTime.minute() < 10) {
    logFile.print('0'); // Add leading zero for minutes < 10
  }
  logFile.print(currentTime.minute(), DEC);
  logFile.print('\t');
  
  logFile.print(temperature, 2); // Temperature with 2 decimal places
  logFile.print('\t');
  logFile.println(co2Concentration);
  
  // Ensure data is written to SD card
  logFile.sync();
  
  // Close file to ensure data integrity
  logFile.close();
  
  // Small delay to ensure file operations complete
  delay(100);
}

/*
 * Enter deep sleep mode to conserve battery power
 */
void enterSleepMode() {
  delay(1000); // Brief delay before sleep
  
  sleep.pwrDownMode();           // Set power-down sleep mode
  sleep.sleepDelay(SLEEP_TIME_MS); // Sleep for specified duration
}

/*
 * File header format (for reference):
 * Date        Time    Temperature(°C)    CO2(ppm)
 * DD/MM/YYYY  HH:MM   XX.XX             XXXX
 * 
 * Example log entry:
 * 15/03/2024  14:30   22.56             410
 */
