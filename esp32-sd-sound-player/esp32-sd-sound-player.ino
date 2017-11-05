#include <FS.h>
#include <SD.h>
#include <Arduino.h>
#include <driver/dac.h>

#define BTN_STOP_ALARM    0
//#define BUF_SIZE 384
#define BUF_SIZE 384+32*5
#define CH 0

#define USE_PWM 1

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
File file;

void IRAM_ATTR onTimer() {
    //isrCounter++;
#if 1
    uint8_t volume = dbuf[curPage].buffer[playPos++];
    //uint8_t volume = dbuf[curPage].buffer[playPos];
    if (playPos >= dbuf[curPage].readed) {
        playPos = 0;
        curPage ^= 1;
        buffFull = 0;
    }
#endif

    #if USE_PWM == 1
    // calculate duty, 8191 from 2 ^ 13 - 1
    //uint32_t duty = (8191 / 255) * min(volume, 255);
    ledcWrite(CH, volume);
    #else
    dac_out_voltage(DAC_CHANNEL_2, volume*0.2);
    #endif

#if 0
    {// for debug
        static uint8_t v = 128;
        //v = (v==128 ? 127 : 128);
        //v = 0;
        //v=255;
        v = 128;
        dac_out_voltage(DAC_CHANNEL_2, v);
    }
#endif
}

SPIClass aSPI(HSPI);

void setup() {
    Serial.begin(115200);

    // Set BTN_STOP_ALARM to input mode
    pinMode(BTN_STOP_ALARM, INPUT_PULLUP);


    // setup SD card
    if(!SD.begin()){
    //if(!SD.begin(15, aSPI)){
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

    #if USE_PWM == 1
    // PWM
    ledcSetup(CH, 100*1000*1000, 8);
    ledcAttachPin(26, CH);
    #endif

    // setup buffer
    //memset(dbuf, 0, sizeof(dbuf));
    curPage = 0;
    playPos = 0;
    buffFull = 0;


    //file = SD.open("/a44.wav");
    file = SD.open("/o32.wav");
    //file = SD.open("/sound.wav");
    //File file = SD.open("/foo.txt");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
    // Skip WAV file header
    file.seek(44);
    uint8_t page = curPage;
    dbuf[page].readed = file.read((uint8_t*)(dbuf[page].buffer), BUF_SIZE);


    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 31, true); // 32kHz Timer 1000000/(32*1000) = 31.25
    //timerAlarmWrite(timer, 62, true); // 16kHz Timer 1000000/(16*1000) = 62.5
    //timerAlarmWrite(timer, 22, true); // 16kHz Timer 1000000/(44*1000) = 22.72
    timerAlarmEnable(timer);
}

void loop() {
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
#if 1
    //Serial.println(file.position());

    if (!buffFull) {
        uint8_t page = curPage ^ 1;
        dbuf[page].readed = file.read((uint8_t*)(dbuf[page].buffer), BUF_SIZE);
        buffFull = 1;
        //Serial.println(file.position());

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
        if (dbuf[page].readed != BUF_SIZE) {
            Serial.println("aaaaaaaaaaaaaaa8");
            file.seek(44);
        }
    }
    //file.close();
#endif

    if (digitalRead(BTN_STOP_ALARM) == LOW) {
        Serial.println("aaaaaaaaa");
        if (timer) {
            timerEnd(timer);
            timer = NULL;
        }
    }
    // Serial.println("aaaaaaaaaaaaaa 2");
}
