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

#include <IRremote.h>

IRrecv irrecv(4);
IRsend irsend;
decode_results results;
uint32_t time;

#undef LED_BUILTIN
#define LED_BUILTIN A0
//#define LED_BUILTIN 3

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  //Serial.begin(9600);
  Serial.println("hello");

  uint8_t outputPins[] = {2, 3, 11, 12, 13, A0, A2, A3, A4, A5};
  uint8_t inputPins[] = {4, 5, 6, 7, 8, 9, 10};
  uint8_t interruptPins[] = {5, 6, 7, 8, 9, 10};
  uint8_t hzPins[] = {0, 1};
  uint8_t naPins[] = {A1};

  for (int i=0; i<sizeof(outputPins); i++) {
    pinMode(outputPins[i], OUTPUT);
  }
  for (int i=0; i<sizeof(inputPins); i++) {
    pinMode(inputPins[i], INPUT);
  }
//  for (int i=0; i<sizeof(hzPins); i++) {
//    pinMode(hzPins[i], INPUT);
//    digitalWrite(hzPins[i], HIGH);
//  }
  for (int i=0; i<sizeof(naPins); i++) {
    pinMode(naPins[i], INPUT);
    digitalWrite(naPins[i], HIGH);
  }
  digitalWrite(2, LOW);  // ENIRM
  digitalWrite(3, LOW);  // IRO
  digitalWrite(11, LOW); // KO1
  digitalWrite(12, LOW); // KO2
  digitalWrite(13, LOW); // KO3
  digitalWrite(A3, LOW); // KO4
  digitalWrite(A4, LOW); // KO5
  digitalWrite(A5, LOW); // KO6
  //wakeup();

  /*
   * Enable Pin Change Interrupts
   */
  PCICR = 0;                // Disable all Pin Change Interrupts
  uint8_t _PCICR = 0;
  for (int i=0; i<sizeof(interruptPins); i++) {
    _PCICR |= (1 << digitalPinToPCICRbit(interruptPins[i]));
    *digitalPinToPCMSK(interruptPins[i]) |= (1 << digitalPinToPCMSKbit(interruptPins[i]));
  }
  PCICR = _PCICR;


  irrecv.enableIRIn(); // Start the receiver
  time = millis();

  for (int i=0; i<10; i++) {
      digitalWrite(A0, HIGH); // LED
      delay(100);
      digitalWrite(A0, LOW); // LED
      delay(100);
  }
}



// the loop function runs over and over again forever
void loop() {
    for (int i=1; i<=3; i++) {
        digitalWrite(A0, HIGH); // LED
        delay(i*1000);
        digitalWrite(A0, LOW); // LED
        delay(1000);
    }
    //if (digitalRead(5) == 0) Serial.println("aaaaaa KI1");
    //if (digitalRead(6) == 0) Serial.println("aaaaaa KI2");
    //if (digitalRead(7) == 0) Serial.println("aaaaaa KI3");
    //if (digitalRead(8) == 0) Serial.println("aaaaaa KI4");
    //if (digitalRead(9) == 0) Serial.println("aaaaaa KI5");
    //if (digitalRead(10) == 0) Serial.println("aaaaaa KI6");

    if (irrecv.decode(&results)) {
        Serial.println(results.value, HEX);
        dumpIR(&results);
        irrecv.resume(); // Receive the next value
    }

    if (millis() - time > 3000) {
        goToSleep();
    }
}

void goToSleep() {
    Serial.println("going to sleep");

    digitalWrite(A0, LOW); // LED
    digitalWrite(2, LOW);  // ENIRM
    digitalWrite(3, LOW);  // IRO
    digitalWrite(11, LOW); // KO1
    digitalWrite(12, LOW); // KO2
    digitalWrite(13, LOW); // KO3
    digitalWrite(A3, LOW); // KO4
    digitalWrite(A4, LOW); // KO5
    digitalWrite(A5, LOW); // KO6

    // Disable ADC
    ADCSRA &= ~(1 << 7);

    // Enable Sleep
    SMCR |= (1 << 2); // power down mode
    SMCR |= 1; // enable sleep

    // Disable BOD
    MCUCR |= (3 << 5); // set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); // then set the BODS bit and clear the BODSE bit at the same time

    __asm__ __volatile__("sleep");
    wakeup();
}

void wakeup() {
    digitalWrite(A0, HIGH); // LED
    digitalWrite(2, HIGH);  // ENIRM
    time = millis();

    //irsend.sendSony(0xE730E11E, 32);
    irsend.sendNEC(0x404032CD, 32);
    Serial.println("wakeup");
}

ISR(PCINT0_vect)
{
  Serial.println("interrupt PCINT0_vect");
}

ISR(PCINT1_vect)
{
  Serial.println("interrupt PCINT1_vect");
}

ISR(PCINT2_vect)
{
  Serial.println("interrupt PCINT2_vect");
}


void dumpIR(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");

  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}
