#include <FS.h>
#include <SD.h>
#include <Arduino.h>
#include <driver/dac.h>

#define BTN_STOP_ALARM    0
//#define BUF_SIZE 384
#define BUF_SIZE 384
#define CH 0

hw_timer_t *timer = NULL;
uint32_t isrCounter = 0;


typedef struct {
    uint16_t readed;
    uint8_t buffer[BUF_SIZE];
} DobuleBuff;
volatile DobuleBuff dbuf[2];
volatile uint8_t curPage=0;
volatile int playPos;
volatile uint8_t buffFull;
uint32_t mainLoopCounter = 0;


void IRAM_ATTR onTimer() {
    isrCounter++;
    // Print it
    #if 0
    if ((isrCounter % 16129) == 0) {
        Serial.print("aaa onTimer no. ");
        Serial.print(isrCounter);
        Serial.print(" at ");
        Serial.print(millis());
        Serial.println(" ms");

        Serial.print(playPos);
        Serial.print(" ");
        Serial.print(curPage);
        Serial.print(" ");
        Serial.print(dbuf[curPage].readed);
        Serial.print(" ");
        Serial.print(buffFull);
        Serial.print(" ");
        Serial.print(dbuf[curPage].buffer[playPos]);
        Serial.print(" ");
        Serial.println((long)(&dbuf), HEX);
    }
    #endif
    uint8_t volume = dbuf[curPage].buffer[playPos++];
    //Serial.print(curPage);
    //Serial.print(",");
    //Serial.print(playPos);
    //Serial.print(",");
    //Serial.print(volume);
    //Serial.print(",");
    //Serial.print(dbuf[curPage ^ 1].readed);
    //Serial.println();
    if ((isrCounter % 10) == 0) {
        Serial.println(volume);
    }
    if (playPos >= dbuf[curPage].readed) {
        playPos = 0;
        curPage ^= 1;
        buffFull = 0;
    }

    // calculate duty, 8191 from 2 ^ 13 - 1
    //uint32_t duty = (8191 / 255) * min(volume, 255);
    //ledcWrite(CH, duty);
    dac_out_voltage(DAC_CHANNEL_2, volume);
}

void setup() {
    Serial.begin(115200);

    // Set BTN_STOP_ALARM to input mode
    pinMode(BTN_STOP_ALARM, INPUT_PULLUP  );


    // setup SD card
    if(!SD.begin()){
        Serial.println("Failed to initialize SD card");
        return;
    }
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
    #if 1
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    #endif

    // PWM
    //ledcSetup(CH, 5000, 13);
    //ledcAttachPin(22, CH);

    // setup buffer
    //memset(dbuf, 0, sizeof(dbuf));
    curPage = 0;
    playPos = 0;
    buffFull = 0;

    // create 16kHz Timer 1000000/(16*1000) = 62.5
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 62, true);
    timerAlarmEnable(timer);
}

void loop() {
    //Serial.println("aaaaaaaaaaaaaa 0");
    File file = SD.open("/sound.wav");
    //File file = SD.open("/foo.txt");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
    //Serial.println("aaaaaaaaaaaaaa 1");
    //Serial.println((long)(&dbuf), HEX);

    // Skip WAV file header
    file.seek(44);
    while (true) {
        //if ((mainLoopCounter++ % 1000000) == 0) {
        //    Serial.println("aaaaaaaaaaaaaa 1.0.0");
        //    Serial.println("aaaaaaaaaaaaaa 1.0");
        //    Serial.println(dbuf[curPage ^ 1].readed);
        //    Serial.print(playPos);
        //    Serial.print(" ");
        //    Serial.print(curPage);
        //    Serial.print(" ");
        //    Serial.print(dbuf[curPage].readed);
        //    Serial.print(" ");
        //    Serial.println(buffFull);
        //}
        if (!buffFull) {
            dbuf[curPage ^ 1].readed = file.read((uint8_t*)(dbuf[curPage ^ 1].buffer), BUF_SIZE);
            buffFull = 1;
            #if 0
            Serial.println(file.position());
            //Serial.print(curPage);
            {
                uint8_t *p = (uint8_t*)dbuf[curPage ^ 1].buffer;
                int size = dbuf[curPage ^ 1].readed;
                Serial.printf("readed size=%d\n",size);
                for (int i=0;i<size;i+=16){
                    for (int j=0;j<16;j++){
                        Serial.printf(" %02x", p[i+j]);
                    }
                    Serial.printf("\n");
                }
            }
            #endif
            if (dbuf[curPage ^ 1].readed != BUF_SIZE)
                break;
        }
    }
    file.close();

    if (digitalRead(BTN_STOP_ALARM) == LOW) {
        Serial.println("aaaaaaaaa");
        if (timer) {
            timerEnd(timer);
            timer = NULL;
        }
    }
    //Serial.println("aaaaaaaaaaaaaa 2");
}
