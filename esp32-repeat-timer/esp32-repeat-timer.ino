/*
 Repeat timer example

 This example shows how to use hardware timer in ESP32. The timer calls onTimer
 function every second. The timer can be stopped with button attached to PIN 0
 (IO0).

 This example code is in the public domain.
 */

// Stop button is attached to PIN 0 (IO0)
#define BTN_STOP_ALARM    0

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;
uint32_t hogeCounter = 0;
uint32_t fugaCounter = 0;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  hogeCounter++;


  if ((isrCounter % 16129) == 0) {
  Serial.print("aaa1 onTimer no. ");
  Serial.print(isrCounter);
  Serial.print(" at ");
  Serial.print(lastIsrAt);
  Serial.println(" ms");
  Serial.print(hogeCounter);
  Serial.print(" ");
  Serial.println(fugaCounter);
  }

  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

void setup() {
  Serial.begin(115200);

  // Set BTN_STOP_ALARM to input mode
  pinMode(BTN_STOP_ALARM, INPUT_PULLUP  );

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  //timerAlarmWrite(timer, 1000000, true);
  timerAlarmWrite(timer, 62, true);

  // Start an alarm
  timerAlarmEnable(timer);
}

void loop() {
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    if ((fugaCounter % 10000) == 0) {
    Serial.print("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa0 onTimer no. ");
    Serial.print(isrCount);
    Serial.print(" at ");
    Serial.print(isrTime);
    Serial.println(" ms");
    Serial.print(hogeCounter);
    Serial.print(" ");
    Serial.println(fugaCounter);
    }
    portEXIT_CRITICAL(&timerMux);
    // Print it
    fugaCounter++;
  }
  // If button is pressed
  if (digitalRead(BTN_STOP_ALARM) == LOW) {
    Serial.println("aaaaaaaaaaaaaa");
    // If timer is still running
    if (timer) {
      // Stop and free timer
      timerEnd(timer);
      timer = NULL;
    }
  }
}
