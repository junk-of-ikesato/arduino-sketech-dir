// PWM でもよかったんだけど、逆位相も試したかったからタイマーを使う

hw_timer_t *timer = NULL;

int flag = 0;
void IRAM_ATTR onTimer() {
    digitalWrite(25, flag);
    digitalWrite(26, flag ^ 1);
    flag ^= 1;
    Serial.println(flag);
}

void setup() {
    Serial.begin(115200);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    //timerAlarmWrite(timer, 13337, true); // 150Hz Timer 1000000/(150/2) = 13337
    timerAlarmWrite(timer, 1000000, true); // 1Hz
    timerAlarmEnable(timer);
}

void loop() {
}
