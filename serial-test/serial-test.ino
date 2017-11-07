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

#undef LED_BUILTIN
#define LED_BUILTIN 4

long time = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("hello");
  Serial1.println("hello1");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {

  int input;
  input = Serial.read();
  
  if (input == '1') {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("HIGH");
    Serial1.println("HIGH1");
  }
  if (input == '0') {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LOW");
    Serial1.println("LOW1");
  }

  if (millis() - time > 1000) {
      Serial.println("hello world");
      Serial1.println("hello world1");
      time = millis();
  }
}
