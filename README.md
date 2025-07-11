# RespirAI - A Standalone Wearable with On-Device Display for Early Detection of Respiratory Disorders

## Summary
**RespirAI** is a portable, standalone wearable device designed for real-time respiratory pattern monitoring and early detection of respiratory disorders including apnea, hypopnea, and shallow breathing. Developed as part of the TRL-8 sensor development program, RespirAI addresses the critical gap in affordable respiratory monitoring technology, particularly for rural and underserved populations where traditional healthcare access is limited.

The system operates independently without requiring internet connectivity or mobile device pairing, making it ideal for home use, remote healthcare monitoring, and emergency response scenarios. With a target cost of less than ₹2500 ($30), RespirAI democratizes access to continuous respiratory health monitoring.

## 1. Integrated Biosensing Modalities
RespirAI combines dual-sensor fusion technology to achieve superior accuracy and reliability in respiratory pattern detection:

| Sensor             | Type              | Purpose                           | Interface | Power | Pins                    |
|--------------------|-------------------|-----------------------------------|-----------|-------|-------------------------|
| **MLX90614**       | Thermal IR        | Airflow temperature monitoring    | I²C       | 3.3V  | SDA → A4, SCL → A5     |
| **BMP280**         | Pressure/Temp     | Breathing pressure changes        | I²C       | 3.3V  | SDA → A4, SCL → A5     |
| **Status LEDs**    | Visual Indicators | Multi-colored breathing alerts    | Digital   | 5V    | D7, D8, D9, D10        |
| **Buzzer**         | Audio Alert       | Audible anomaly notifications     | PWM       | 5V    | D6                     |

### MLX90614 Thermal IR Sensor
A non-contact infrared temperature sensor that measures airflow temperature variations during breathing cycles. The sensor detects the temperature difference between inhaled cool air and exhaled warm air, providing:
   * Respiratory rate calculation (12-30 breaths/min)
   * Breathing pattern analysis
   * Airflow temperature monitoring (±0.5°C accuracy)

### BMP280 Pressure Sensor
A high-precision atmospheric pressure sensor that detects minute pressure variations associated with breathing cycles. This sensor provides:
   * Pressure-based breath detection
   * Breathing depth analysis
   * Atmospheric pressure compensation
   * Dual-sensor validation for enhanced accuracy

### Multi-Modal Status System
**Visual Indicators:**
- 🔴 **Red LED (D7)**: Apnea alert (>20 seconds no breathing)
- 🟡 **Yellow LED (D8)**: Hypopnea alert (reduced breathing amplitude)
- 🟢 **Green LED (D9)**: Normal breathing pattern
- 🔵 **Blue LED (D10)**: Shallow breathing detection

**Audio Alerts:**
- **Buzzer (D6)**: Variable tone patterns for different respiratory conditions

## 2. System Architecture

The sensors interface with an Arduino Uno microcontroller that performs the following tasks:

* **Real-time Signal Processing**: Acquires and filters respiratory signals at 10Hz sampling rate
* **Dual-Sensor Fusion**: Combines thermal and pressure data for enhanced accuracy
* **Anomaly Detection**: Implements rule-based algorithms for apnea, hypopnea, and shallow breathing detection
* **Immediate Alerts**: Triggers visual and audio notifications for respiratory anomalies
* **Standalone Operation**: Functions independently without external connectivity requirements

### Core Detection Algorithms

**Apnea Detection:**
- Monitors breathing cessation for >20 seconds
- Uses combined thermal and pressure signal analysis
- Triggers critical alert with red LED and buzzer

**Hypopnea Detection:**
- Detects 50% reduction in breathing amplitude
- Compares against personalized baseline
- Provides early warning with yellow LED

**Shallow Breathing Detection:**
- Analyzes breath depth using sensor fusion
- Adaptive threshold based on individual patterns
- Blue LED indication for shallow breathing patterns

## 3. Motivation & Technical Differentiator

RespirAI addresses critical gaps in respiratory healthcare access:

**Global Health Impact:**
- 3+ million annual deaths from respiratory diseases (WHO)
- 80% of sleep apnea cases remain undiagnosed
- Limited access to respiratory monitoring in rural areas

**Technical Advantages:**
- **Standalone Operation**: No smartphone or internet dependency
- **Dual-Sensor Fusion**: Higher reliability than single-sensor systems
- **Cost-Effective**: <₹2500 target (90% cost reduction vs. existing solutions)
- **Medical-Grade Accuracy**: Thermal + pressure sensor combination
- **Real-Time Processing**: Immediate on-device analysis and alerts

**Target Applications:**
- Home-based respiratory monitoring
- Rural healthcare deployment
- Elderly care facilities
- Sleep disorder screening
- Emergency response systems

## Setup Steps: Getting Started with RespirAI

Follow these steps to set up your development environment for the *RespirAI* project using *Arduino IDE* and the Arduino Uno microcontroller.

### 1. Install Required Software

#### Arduino IDE
Download and install the Arduino IDE from the official website:
- Download from: [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
- Install for your OS (Windows/macOS/Linux)

#### Required Libraries
Install the following libraries through Arduino IDE Library Manager:
- **Adafruit MLX90614 Library**
- **Adafruit BMP280 Library**

### 2. Hardware Assembly

#### Component List
| Component                    | Quantity | Purpose                    |
|------------------------------|----------|----------------------------|
| Arduino Uno R3               | 1        | Main microcontroller       |
| MLX90614 IR Temperature      | 1        | Airflow temperature sensor |
| BMP280 Pressure Sensor       | 1        | Breathing pressure sensor  |
| LEDs (Red, Yellow, Green, Blue) | 4     | Status indicators          |
| Piezo Buzzer                 | 1        | Audio alerts               |
| 220Ω Resistors               | 4        | LED current limiting       |
| 4.7kΩ Resistors              | 2        | I2C pull-up resistors      |
| Breadboard                   | 1        | Prototyping platform       |
| Jumper Wires                 | 20+      | Connections                |

#### Circuit Connections

**I2C Bus (Shared by all sensors):**
```
Arduino Pin A4 (SDA) → MLX90614 SDA, BMP280 SDA, OLED SDA
Arduino Pin A5 (SCL) → MLX90614 SCL, BMP280 SCL, OLED SCL
3.3V → All sensor VCC pins
GND → All sensor GND pins
4.7kΩ pull-up resistors: SDA to 3.3V, SCL to 3.3V
````

**Status LEDs:**
```
Pin D7 → 220Ω → Red LED → GND (Apnea Alert)
Pin D8 → 220Ω → Yellow LED → GND (Hypopnea Alert)
Pin D9 → 220Ω → Green LED → GND (Normal Breathing)
Pin D10 → 220Ω → Blue LED → GND (Shallow Breathing)
```

**Audio Alert:**
```
Pin D6 → Buzzer Positive
GND → Buzzer Negative
```

### 3. Software Configuration

#### I2C Address Configuration
- **MLX90614**: 0x5A (default)
- **BMP280**: 0x76 or 0x77 (depends on module)
- **SSD1306**: 0x3C (default)

#### Upload Project Files
1. Download the RespirAI firmware from the repository
2. Open `RespirAI_Main.ino` in Arduino IDE
3. Select **Arduino Uno** as the board
4. Select the appropriate COM port
5. Click **Upload** to flash the firmware

### 4. Calibration and Testing

#### Initial Calibration (10 seconds)
1. Power on the device
2. Breathe normally for 10 seconds during calibration
3. System will establish personal baseline thresholds
4. Green LED will indicate successful calibration

#### Sensor Verification
Use the built-in I2C scanner to verify sensor connections:
- Expected addresses: 0x3C (OLED), 0x5A (MLX90614), 0x76/0x77 (BMP280)
- All sensors should be detected before normal operation

## TRL-8 Goals
RespirAI is designed to meet TRL-8 readiness, demonstrating reliability, stability, and real-world deployability. The following characterization steps are planned:

### 1. **Requirements Freeze & CTQ Table**
- Define Critical-to-Quality parameters for respiratory rate accuracy (±2 breaths/min)
- Establish temperature measurement precision (±0.5°C)
- Set pressure sensitivity thresholds (±1 hPa)

### 2. **Bench Accuracy & Linearity Testing**
- Compare respiratory rate measurements with reference spirometry equipment
- Validate temperature readings against calibrated thermometers
- Test pressure sensor accuracy across operational range

### 3. **24-Hour Continuous Logging**
- Perform extended operation testing with continuous data logging
- Export timestamped CSV files for analysis
- Evaluate long-term stability and baseline drift

### 4. **Environmental Robustness Testing**
- Temperature drift evaluation (-10°C to +50°C)
- Humidity resistance testing (10-90% RH)
- Electromagnetic interference immunity testing

### 5. **Motion Artifact & Noise Analysis**
- Evaluate signal quality during user movement
- Test effectiveness of digital filtering algorithms
- Assess false alarm rates under various conditions

### 6. **Clinical Validation Studies**
- Controlled breathing pattern tests with healthy volunteers
- Correlation analysis with gold-standard respiratory monitoring
- Sensitivity and specificity analysis for anomaly detection

### 7. **Power Consumption & Battery Life**
- Optimize power management for extended operation
- Validate battery life under typical usage patterns
- Test low-power modes and sleep functionality

### 8. **Manufacturing Readiness**
- Finalize Bill of Materials (BOM) for cost optimization
- Establish supply chain for Indian manufacturing
- Design for testability and quality assurance

### 9. **User Interface & Experience Testing**
- Evaluate OLED display readability under various lighting conditions
- Test audio alert effectiveness and user response times
- Validate intuitive operation for elderly users

### 10. **Documentation & Compliance**
- Complete technical documentation package
- Prepare regulatory submission materials
- Establish quality management system documentation

## Target Specifications

### Performance Metrics
- **Respiratory Rate Range**: 5-60 breaths/min
- **Accuracy**: ±2 breaths/min (vs. reference standard)
- **Response Time**: <10 seconds for anomaly detection
- **Apnea Detection**: >20 seconds breathing cessation
- **Hypopnea Sensitivity**: 50% amplitude reduction detection

### Technical Specifications
- **Operating Voltage**: 5V (USB powered)
- **Power Consumption**: <200mA typical
- **Operating Temperature**: 0°C to 50°C
- **Display**: 128x64 OLED, white-on-black
- **Audio Output**: 85dB @ 10cm
- **Connectivity**: USB serial for data logging

### Regulatory Compliance
- **Safety**: IEC 60601-1 (medical electrical equipment)
- **EMC**: IEC 60601-1-2 (electromagnetic compatibility)
- **Software**: IEC 62304 (medical device software)
- **Risk Management**: ISO 14971 (medical device risk management)

## Repository Structure

```
RespirAI/
├── README.md                 # This file
├── LICENSE                   # MIT License
├── /firmware/               # Arduino source code
│   ├── RespirAI_Main.ino    # Main application
│   ├── sensor_drivers/      # Sensor libraries
│   └── examples/            # Test utilities
├── /hardware/               # Circuit diagrams & BOM
│   ├── schematics/          # KiCad/Fritzing files
│   ├── PCB_design/          # PCB layout files
│   └── BOM.txt              # Bill of Materials
├── /testlog_data/           # Validation data
    ├── bench_tests/         # Laboratory validation
    ├── clinical_data/       # User study results
    └── environmental/       # Environmental testing

```

## Contributing

We welcome contributions to the RespirAI project! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on:
- Code style and standards
- Testing procedures
- Documentation requirements
- Issue reporting
- Pull request process

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **WHO Global Health Observatory** for respiratory disease statistics
- **Arduino Community** for open-source hardware support
- **Adafruit** for sensor libraries and documentation
- **TRL-8 Sensor Development Program** for funding and support

## Contact

For technical questions, collaboration opportunities, or support:
- **Project Lead**: Mukesh
- **Email**: mukul12150@gmail.com
- **GitHub**: [https://github.com/Mukesh0035/RespirAI](https://github.com/Mukesh0035/RespirAI)

---

**⚠️ Medical Disclaimer**: RespirAI is designed for educational and research purposes only. It is not FDA-approved or intended for medical diagnosis. Always consult healthcare professionals for any respiratory concerns. This device should be used as a learning tool to understand respiratory monitoring principles, not as a replacement for professional medical devices.

**🔬 Research Use**: This system is suitable for academic research, prototype development, and educational applications in biomedical engineering and respiratory physiology.
