#include <driver/gpio.h>

#define BUTTON_PIN GPIO_NUM_1 // Physical header pin D0
#define LED_PIN    GPIO_NUM_2 // Physical header pin D1

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Configure BUTTON_PIN with an explicit hardware pull-down
  pinMode(BUTTON_PIN, INPUT);
  gpio_pulldown_en(BUTTON_PIN);  // Hardware force pull-down to GND (0V)
  gpio_pullup_dis(BUTTON_PIN);  // Ensure pull-up is OFF

  Serial.begin(115200);

  // Synchronize USB Serial for ESP32-S3 native USB
  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000));

  Serial.println("System Ready: Active Mode Test");
}

void loop() {
  // Read physical pin state
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("\nButton Pressed!");
    Serial.println("Status: LED ON - Active Load");
    
    digitalWrite(LED_PIN, HIGH); // Turn LED ON
    delay(5000);                 // Keep LED ON for 5 seconds
    
    digitalWrite(LED_PIN, LOW);  // Turn LED OFF
    Serial.println("Status: LED OFF - Idle Active Mode");
  }

  delay(50); // Small debounce delay
}