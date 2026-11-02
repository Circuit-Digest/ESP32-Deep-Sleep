#include <driver/rtc_io.h> // Include RTC IO driver

#define BUTTON_PIN GPIO_NUM_1
#define LED_PIN    2

RTC_DATA_ATTR int wakeCount = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  delay(500);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    wakeCount++;
    Serial.println("\n--- ESP32 WOKE UP FROM DEEP SLEEP (BUTTON PRESSED) ---");
    Serial.print("Boot Count: ");
    Serial.println(wakeCount);
    Serial.println("Status: LED ON - Active Load");

    digitalWrite(LED_PIN, HIGH);
    delay(5000);
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println("\n--- ESP32 INITIAL POWER-ON ---");
    Serial.println("Status: System starting up directly into Deep Sleep...");
  }

  Serial.println("Status: Entering Deep Sleep Mode now...");
  Serial.flush();

  // Explicitly configure internal pull-down for RTC GPIO 33 during sleep
  rtc_gpio_pulldown_en(BUTTON_PIN);
  rtc_gpio_pullup_dis(BUTTON_PIN);

  // Enable ext0 wake-up on HIGH signal
  esp_sleep_enable_ext0_wakeup(BUTTON_PIN, 1);

  esp_deep_sleep_start();
}

void loop() {
}