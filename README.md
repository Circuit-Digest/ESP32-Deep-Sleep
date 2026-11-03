# ESP32-S3 Deep Sleep & Power Optimization Guide

[![Board](https://img.shields.io/badge/Board-XIAO%20ESP32--S3-blue)](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20IDE-green)](https://www.arduino.cc/)
[![Power Savings](https://img.shields.io/badge/Power%20Reduction-99.6%25-orange)](#power-consumption-benchmarks)

A practical hands-on guide and reference implementation for configuring **Deep Sleep** on the Seeed Studio **XIAO ESP32-S3** using an external hardware interrupt (`ext0`). This project demonstrates how to slash idle power consumption by **99.6%**, extending battery life from a couple of days to multiple years.

---

## 📸 Real-World Power Benchmarks

Measurements recorded using a precision multimeter at **5V DC input**:

| Operating Mode | System State | Measured Current | Description |
| :--- | :--- | :--- | :--- |
| **Active Mode** | LED ON | **~40.40 mA** | CPU active ($240\text{ MHz}$) powering the load |
| **Active Mode** | LED OFF | **~31.21 mA** | CPU active, continuously running `loop()` polling |
| **Deep Sleep Mode** | LED ON | **~40.40 mA** | Woken up via `ext0`, executing active payload |
| **Deep Sleep Mode** | LED OFF | **0.13 mA** ($130\ \mu\text{A}$) | CPU, RAM, and digital peripherals completely OFF |

> 💡 **Key Takeaway:** Deep Sleep reduces idle current draw from **31.21 mA** down to **0.13 mA**—a massive **99.6% energy savings**.

---

## 🛠 Hardware Required

* **Microcontroller:** Seeed Studio XIAO ESP32-S3
* **Push Button:** Standard 2-pin tactile switch
* **LED:** Standard 5mm LED (Load indicator)
* **Resistors:** $220\ \Omega$ current-limiting resistor for LED
* **Power & Measurement:** Bench Power Supply (UNI-T UTP1306) & Multimeter (CHNT ZTW890D Pro)
* **Breadboard & Jumper Wires**

---

## ⚡ Circuit Schematic & Wiring

| Component | XIAO ESP32-S3 Pin | Function / Description |
| :--- | :--- | :--- |
| **Push Button** | `GPIO 1` (`D0`) | External `ext0` wake-up trigger (HIGH signal) |
| **LED Load** | `GPIO 2` (`D1`) | Output load indicator |
| **Power Input** | `5V` & `GND` | Power rail connected to bench power supply |

---

## 🚀 How It Works

1. **Boot Reason Detection:** Upon waking, the chip calls `esp_sleep_get_wakeup_cause()`.
2. **Event Handling:** If booted via `ESP_SLEEP_WAKEUP_EXT0` (button press), it increments the persistent `wakeCount` variable stored in **RTC RAM** (`RTC_DATA_ATTR`) and turns on the LED for 5 seconds.
3. **RTC Pin Configuration:** Before sleeping, `rtc_gpio_pulldown_en()` configures the pin's internal pull-down resistor to prevent floating signals.
4. **Sleep Activation:** `esp_sleep_enable_ext0_wakeup(GPIO_NUM_1, 1)` prepares the RTC controller to monitor `GPIO 1` for a `HIGH` signal, and `esp_deep_sleep_start()` turns off the CPU.

---

## 💻 Source Code

```cpp
#include <driver/rtc_io.h>

#define BUTTON_PIN GPIO_NUM_1
#define LED_PIN    2

// Variable stored in RTC memory to survive deep sleep reboots
RTC_DATA_ATTR int wakeCount = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  delay(500);

  // Check what woke up the ESP32
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    wakeCount++;
    Serial.println("\n--- ESP32 WOKE UP FROM DEEP SLEEP (BUTTON PRESSED) ---");
    Serial.print("Boot Count: ");
    Serial.println(wakeCount);
    Serial.println("Status: LED ON - Active Load");

    digitalWrite(LED_PIN, HIGH);
    delay(5000); // Simulate workload for 5 seconds
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println("\n--- ESP32 INITIAL POWER-ON ---");
    Serial.println("Status: System starting up directly into Deep Sleep...");
  }

  Serial.println("Status: Entering Deep Sleep Mode now...");
  Serial.flush();

  // Configure internal pull-down for RTC GPIO during sleep to keep signal stable at 0V
  rtc_gpio_pulldown_en(BUTTON_PIN);
  rtc_gpio_pullup_dis(BUTTON_PIN);

  // Enable ext0 wake-up on HIGH signal (3.3V)
  esp_sleep_enable_ext0_wakeup(BUTTON_PIN, 1);

  // Enter deep sleep mode
  esp_deep_sleep_start();
}

void loop() {
  // Never reached — execution restarts from setup() upon wake-up
}
