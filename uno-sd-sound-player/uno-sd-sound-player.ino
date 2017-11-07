// とりあえず、SDカードのWAVファイルを再生してみる。
// 16MHzのArduino で 256 clock毎に割り込むので 62.5kHz(8bit Monoral)が理想だが
// ボクの持っているソフトが対応していないので、32kHz(8bit Stereo)で代用
#include <SD.h>

#define BUF_SIZE 384          // バッファ・サイズ

volatile uint8_t              // グローバル変数
    buf[2][BUF_SIZE],           // バッファ
    buf_page,                   // バッファ・ページ
    buf_flg;                    // バッファ読み込みフラグ
volatile uint16_t buf_index;    // バッファ位置
volatile uint16_t read_size[2]; // バッファ読み込みサイズ

void setup() {
  Serial.begin(115200);
  Serial.println("aaaaaaaaaa0.0");
  pinMode(10, OUTPUT);                             // SDライブラリ使用時のお約束
  while( !SD.begin(10) );                          // ライブラリとSDカードを初期化
  Serial.println("aaaaaaaaaa0.1");
  // PWM初期化
  DDRD  |=  B00001000;                             // PD3 (OC2B) (Arduino D3)
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);  // 8bit高速PWM
  TCCR2B = _BV(CS20);                              // 分周なし
}

void loop() {
  File dataFile;
  Serial.println("aaaaaaaaaa0");
  if ( !(dataFile = SD.open( "o32.wav" )) )  return;   // error opening wavefile
  Serial.println("aaaaaaaaaa0.1");

  buf_index = 44;  buf_page = 0;  buf_flg = 1;    // パラメータ設定 (44byteはヘッダ)  
  read_size[buf_page] = dataFile.read( (uint8_t*)buf[buf_page], BUF_SIZE );
  TIMSK2 |= _BV(TOIE2);
  while(TIMSK2 & _BV(TOIE2)) {
    if(buf_flg) {     // データ読み込み指令のフラグが立ったら読み込む
      read_size[buf_page ^ 1] = dataFile.read( (uint8_t*)buf[buf_page ^ 1], BUF_SIZE );
      buf_flg = 0;    // 読み込んだらフラグを下ろす
      //Serial.println("aaaaaaaaaa1");
      #if 0
      {
          uint8_t *p = (uint8_t*)buf[buf_page ^ 1];
          int size = read_size[buf_page ^ 1];
          Serial.print("readed size");
          Serial.println(size);
          for (int i=0;i<size;i+=16){
              for (int j=0;j<16;j++){
                  Serial.print(" ");
                  Serial.print(p[i+j], HEX);
              }
              Serial.println();
          }
      }
      #endif

    }
  }
  Serial.println("aaaaaaaaaa1");
  OCR2B = 0;
  dataFile.close();
  delay(5000);
}

int flg = 0;
ISR(TIMER2_OVF_vect) {
  if (((++flg)&1) == 0) return;
  OCR2B = buf[buf_page][buf_index++];                  // データをPWMとして出力
  if(buf_index == read_size[buf_page]) {               // 現在のバッファの最後まで来たら...
    if(buf_index != BUF_SIZE)  TIMSK2 &= ~_BV(TOIE2);    // ファイルの最後なら,TOIE2をクリア
    buf_index = 0;  buf_page ^= 1;  buf_flg = 1;         // バッファを切り替え
  }
  //Serial.println("0,256");
  //Serial.println(OCR2B);
}
