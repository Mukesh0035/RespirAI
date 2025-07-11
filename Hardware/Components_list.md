# 📦 Project BOM (Bill of Materials)

This project involves building a sensor-based indicator system using Arduino. Below is the complete Bill of Materials (BOM), including part numbers and estimated costs. This setup is ideal for prototyping temperature and pressure sensing with visual and audio feedback.

---

## 🧾 Components List

| No. | Component                              | Quantity | Example Part Number        | Estimated Cost (Rupee)|
|-----|----------------------------------------|----------|-----------------------------|----------------------|
| 1   | Arduino Uno R3                         | 1        | A000066 (Official Arduino)  | 210.00               |
| 2   | MLX90614 Infrared Temperature Sensor   | 1        | MLX90614ESF-AAA             | 1000.00              |
| 3   | BMP280 Pressure/Temperature Sensor     | 1        | GY-BMP280                   | 220.00               |
| 4   | LEDs (Red, Yellow, Green, Blue)        | 4        | Generic 5mm LEDs            | 20(5 each)           |
| 5   |  Resistors box                         | 4        | CF1/4W-220R-JT              | 80.00                |
| 6   | Piezo Buzzer                           | 1        | PS1240P02BT                 | 20.00                |
| 7   | Breadboard                             | 1        | MB-102                      | 60.00                |
| 8   | Jumper Wires(Male-Male & male-female)  |          | Generic                     | 120.00               |
| 9   | USB cable (Type-B)                     | 1        | Generic                     | 40.00                |
| 10  | Battery                                | 1        | HYW                         | 20                   |
| 11  | Battery Connector                      | 1        | Generic                     | 10                   |
---

## 📌 Notes

- Ensure both the **MLX90614** and **BMP280** sensors use I2C communication (check breakout board specs).
- **Pull-up resistors (10kΩ)** might be necessary for I2C lines (SDA and SCL) depending on sensor configuration.
- Each **LED** should be connected in series with a **220Ω resistor** to limit current and prevent damage.
- The **buzzer** can be used for sound notifications triggered by sensor thresholds or events.

---

## 🛠️ Tools used

- USB cable (Type-B) for Arduino Uno programming
- Power supply (5V via USB or external)
- Multimeter (for voltage/resistance checks)
- Soldering iron and solder 

## 📂 License

This BOM and project documentation are provided under the [MIT License](https://opensource.org/licenses/MIT).

---
