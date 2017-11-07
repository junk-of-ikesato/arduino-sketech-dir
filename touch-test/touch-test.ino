#include <WiFi.h>

int prevt = 0;
int t = 0;
int threshold = 5;

void setup()
{
  Serial.begin(115200);
  pinMode(25,OUTPUT);
  pinMode(26,INPUT);
  pinMode(15,OUTPUT);

  //WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  //WiFi.forceSleepBegin();
}

int counter = 0;

void loop()
{
  t = micros();

  // パルスの立ち上げ
  digitalWrite(25, HIGH);

//Serial.println("aaaa 0");
  // 立ち上がりまでの時間計測
  while (digitalRead(26)!=HIGH) ;

  t = micros() - t;
//Serial.println("aaaa 1");

  // 放電するまで待つ
  digitalWrite(25, LOW);
  delay(50);

  // ローパスフィルタ
  //t = 0.999 * prevt + 0.001 * t;
  t = 0.9 * prevt + 0.1 * t;
  //t = 0.8 * prevt + 0.2 * t;
  prevt = t;

  if ((counter++) % 1 == 0) {
      Serial.print("3000,0,");
      Serial.println(t);
  }

  // LED点灯
  if( t > threshold ){
    digitalWrite(15, HIGH);
  } else {
    digitalWrite(15, LOW);
  }
}
