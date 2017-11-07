#include <zlib.h>



void setup() {
  // put your setup code here, to run once:
  z_stream z;
  inflateInit2(&z, -MAX_WBITS);
}

void loop() {
  // put your main code here, to run repeatedly:

}
