#include <Servo.h>
#define CHANNEL2 12// 前进，后退
#define CHANNEL3 13 //油门
#define CHANNEL4 2 //左右
Servo AIL;
void setup() {
  // put your setup code here, to run once:
  pinMode(CHANNEL2,0);
  pinMode(CHANNEL3,0);
  pinMode(CHANNEL4,0);
  
  AIL.attach(9);
  Serial.begin(9600);
  while(Serial.read()>= 0){}
}

void loop() {
  // put your main code here, to run repeatedly:


  
  unsigned long channel2_value,channel3_value,channel4_value;
  channel2_value = pulseIn(CHANNEL2,1);//1180-1440-1680 down-middle-up
  channel3_value = pulseIn(CHANNEL3,1);//870-1860
  channel4_value = pulseIn(CHANNEL4,1);//1280-1540-1780  left-middle-right
  
  Serial.print("CHANNEL2=");
  Serial.print(channel2_value);
    Serial.print("      CHANNEL3=");
  Serial.print(channel3_value);
  Serial.print("      CHANNEL4=");
  Serial.println(channel4_value);
  //AIL.writeMicroseconds(in_value);
  delay(5);
   
}
