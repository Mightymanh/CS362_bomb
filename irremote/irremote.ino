#include <IRremote.h>

const int RECV_PIN = 11;
IRrecv IR(11);
decode_results results;

void setup(){
  Serial.begin(9600);
  IR.enableIRIn();
}

void loop(){
  if(IR.decode()) {
    Serial.println(IR.decodedIRData.decodedRawData, HEX); 
    delay(1500);
    IR.resume(); 
  }
}
