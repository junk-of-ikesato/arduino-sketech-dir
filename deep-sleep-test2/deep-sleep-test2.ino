/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald

  modified 2 Sep 2016
  by Arturo Guadalupi

  modified 8 Sep 2016
  by Colby Newman
*/

#include <avr/sleep.h>


#undef LED_BUILTIN
#define LED_BUILTIN 4


// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    Serial.println("\nhello world");
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);

  for (int i=0; i<20; i++) {
    pinMode(i, OUTPUT);
  }
}

// the loop function runs over and over again forever
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                       // wait for a second
    goToSleep();
}



void goToSleep() {
//    Serial.println("going to sleep");
//    delay(100);
//
//    // Disable ADC
//    ADCSRA &= ~(1 << 7);
//
//    // Enable Sleep
//    SMCR |= (1 << 2); // power down mode
//    SMCR |= 1; // enable sleep
//
//    // Disable BOD
//    MCUCR |= (3 << 5); // set both BODS and BODSE at the same time
//    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); // then set the BODS bit and clear the BODSE bit at the same time
//
//    __asm__ __volatile__("sleep");

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    //cli();
    sleep_enable();
    //sleep_bod_disable();
    //sei();
    //sleep_cpu();
    sleep_mode();            // here the device is actually put to sleep!!

    /* wake up here */
    sleep_disable();

    wakeup();
}

void wakeup() {
    Serial.println("wakeup");
}

