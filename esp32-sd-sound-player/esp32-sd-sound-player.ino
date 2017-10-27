
#define BTN_STOP_ALARM    0

hw_timer_t *timer = NULL;
uint32_t isrCounter = 0;

void IRAM_ATTR onTimer() {
    isrCounter++;
    // Print it
    if ((isrCounter % 16129) == 0) {
        Serial.print("aaa onTimer no. ");
        Serial.print(isrCounter);
        Serial.print(" at ");
        Serial.print(millis());
        Serial.println(" ms");
    }
}

void setup() {
    Serial.begin(115200);

    // Set BTN_STOP_ALARM to input mode
    pinMode(BTN_STOP_ALARM, INPUT_PULLUP  );

    // create 16kHz Timer 1000000/(16*1000) = 62.5
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 62, true);
    timerAlarmEnable(timer);
}

void loop() {
    if (digitalRead(BTN_STOP_ALARM) == LOW) {
        if (timer) {
            timerEnd(timer);
            timer = NULL;
        }
    }
}
