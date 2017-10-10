#include <Keypad.h>

const byte ROWS = 6; //four rows
const byte COLS = 7; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','4','5','6','7'},
  {'8','9','0','A','B','C','D'},
  {'E','F','G','H','I','J','K'},
  {'L','M','N','O','P','Q','R'},
  {'S','T','U','V','W','X','Y'},
  {'Z','+','-','*','/','=','.'},
};
byte rowPins[ROWS] = {5, 6, 7, 8, 9, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 12, 13, A2, A3, A4, A5}; //connect to the column pinouts of the keypad


Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


#include <IRremote.h>
IRsend irsend;

void setup(){
  Serial.begin(115200);
  Serial.println("hello world");
}

void loop(){
  char key = keypad.getKey();

  if (key != NO_KEY){
    Serial.println(key);
  }
  if (key == 'Z') {
    irsend.sendPanasonic(0x55A, 0xF148688B); // This should turn your TV on and off
    Serial.println("aaaaaaaa");
  }
}

