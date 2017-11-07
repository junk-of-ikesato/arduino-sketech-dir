int freq[]={
    33,  35,  37,  39,  41,  44,  46,  49,  52,  55,  58,  62,
    65,  69,  73,  78,  82,  87,  93,  98, 104, 110, 117, 123,
   131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,
   262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
   523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
  1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,
  2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,
  4186,
};

int c=0;

void setup() {
}
 
void loop() {
    if (++c >= sizeof(freq)/sizeof(freq[0]))
        c=0;
    tone(12, freq[c], 100) ;
    delay(100) ;
    noTone(12) ;
    delay(100);
}


#define LEDC_CHANNEL_2     2
#define LEDC_TIMER_13_BIT  13
#define LEDC_BASE_FREQ     5000


void noTone(int pin)
{
  ledcWriteTone(LEDC_CHANNEL_2, 0.0) ;
}

void tone(int pin, int freq, int duration)
{
  ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT) ;
  ledcAttachPin(pin, LEDC_CHANNEL_2) ; // CH2をSOUNDERに
  ledcWriteTone(LEDC_CHANNEL_2, freq) ;
}

