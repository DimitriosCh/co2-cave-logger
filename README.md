# CO2 Cave Data Logger

A low-power Arduino-based data logging system for continuous CO2 concentration and temperature monitoring in cave environments. This project was developed for cave microclimate research and is optimized for extended battery-powered deployments in remote locations.

## Overview

This system uses a Seeeduino (Arduino-compatible board optimized for low power consumption) to log CO2 concentration, temperature, and timestamp data to an SD card. Between measurements, the system enters deep sleep mode to maximize battery life, making it suitable for long-term cave monitoring studies.

## Features

- **Low Power Design**: Deep sleep mode between measurements extends battery life
- **Real-time Logging**: Timestamps all measurements using DS1337 RTC
- **Configurable Sampling**: Adjustable measurement intervals (default: 3 hours)
- **Robust Storage**: Data logged to SD card in tab-separated format
- **Temperature Monitoring**: Analog temperature sensor integration
- **Status Indication**: LED feedback for system status
- **Extensible Platform**: Easy adaptation for additional sensors

## Hardware Requirements

### Core Components
- **Seeeduino** (or compatible Arduino board with low power consumption)
- **COZIR CO2 Sensor** - High-accuracy CO2 measurement
- **DS1337 Real-Time Clock Module** - Precise timestamping
- **SD Card Module** - Data storage
- **Analog Temperature Sensor** (e.g., TMP36)

### Supporting Components
- MicroSD card (FAT32 formatted)
- 3.3V/5V power supply or battery pack
- Connecting wires and breadboard/PCB
- Pull-up resistors for I2C communication (if not included on modules)

## Pin Connections

| Component | Arduino Pin | Notes |
|-----------|-------------|--------|
| COZIR RX | Pin 7 | Software Serial |
| COZIR TX | Pin 8 | Software Serial |
| Temperature Sensor | A0 | Analog input |
| SD Card CS | Pin 10 | SPI Chip Select |
| SD Card MOSI | Pin 11 | SPI |
| SD Card MISO | Pin 12 | SPI |
| SD Card SCK | Pin 13 | SPI |
| RTC SDA | A4 | I2C Data |
| RTC SCL | A5 | I2C Clock |
| Sensor Power | Pin 9 | Power control |
| SD Power | Pin 4 | Power control (active low) |
| Status LED | Pin 13 | Built-in LED |

## Required Libraries

Install the following libraries in your Arduino IDE:

- **SdFat** - SD card file system management
- **cozir** - COZIR CO2 sensor communication
- **DS1337** - Real-time clock functionality
- **Sleep_n0m1** - Power management and sleep modes
- **Wire** - I2C communication (built-in)
- **SPI** - SPI communication (built-in)
- **SoftwareSerial** - Software serial communication (built-in)

## Installation

1. **Hardware Setup**:
   - Connect all components according to the pin connections table
   - Format SD card as FAT32
   - Set the RTC to current time (refer to DS1337 library documentation)

2. **Software Setup**:
   - Install Arduino IDE
   - Install required libraries through Library Manager
   - Download and open `co2_logger.ino`
   - Verify pin connections match your hardware setup
   - Upload code to your Arduino

3. **Configuration**:
   - Adjust `SLEEP_TIME_MS` constant for desired measurement interval
   - Modify `LOG_FILENAME` if needed
   - Calibrate temperature sensor if using different model

## Usage

1. **Deploy**: Power on the system - LED will blink to indicate successful initialization
2. **Monitor**: System will blink LED 4 times before each measurement cycle
3. **Retrieve Data**: Remove SD card to access logged data
4. **Analyze**: Data is stored in tab-separated format for easy import to spreadsheet or analysis software

## Data Format

Data is logged to a tab-separated text file with the following format:

```
Date        Time    Temperature(°C)    CO2(ppm)
15/03/2024  14:30   22.56             410
15/03/2024  17:30   21.89             425
15/03/2024  20:30   20.12             390
```

### Columns:
- **Date**: DD/MM/YYYY format
- **Time**: HH:MM format (24-hour)
- **Temperature**: Degrees Celsius (2 decimal places)
- **CO2**: Parts per million (integer)

## Power Consumption

The system is optimized for low power consumption:
- **Active Mode**: ~50-100mA during measurement (6 seconds)
- **Sleep Mode**: <1mA between measurements
- **Estimated Battery Life**: Several months with appropriate battery pack (depends on measurement interval and battery capacity)

## Customization

### Changing Measurement Interval
Modify the `SLEEP_TIME_MS` constant:
```cpp
const unsigned long SLEEP_TIME_MS = 10800000; // 3 hours in milliseconds
```

### Adding Additional Sensors
The modular design allows easy integration of additional sensors:
1. Add sensor initialization in `setup()`
2. Create sensor reading function
3. Modify `logSensorData()` to include new measurements
4. Update data format documentation

## Troubleshooting

### Common Issues:
- **SD Card not detected**: Check connections, ensure FAT32 format
- **No timestamp**: Verify RTC connections and battery backup
- **High power consumption**: Ensure peripheral power management is working
- **Missing data**: Check SD card space and file system integrity

### LED Status Indicators:
- **Single long blink on startup**: System initialized successfully
- **4 blinks before measurement**: Normal operation cycle
- **No blinking**: Check power supply and connections

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## Citation

If you use this code in your research, please cite:

```
SpeleoCO: an open, low-cost Arduino CO₂ datalogger for cave microclimate monitoring 

Christos Pennos & Dimitrios Christaras 
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built using open-source Arduino libraries and community resources
- Developed for cave microclimate research applications
- Thanks to the Arduino and open-source hardware communities

## Contact

For questions or collaboration opportunities, please contact dimitrios@um.es.

---

**Keywords**: Arduino, CO2 monitoring, cave research, data logger, environmental monitoring, low power, microclimate
