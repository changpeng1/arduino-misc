#include <Servo.h>
char buff[10];
Servo my_servo;
Servo my_servo1;
void setup() {
  // put your setup code here, to run once:
  my_servo.attach(9);
  my_servo1.attach(10);
 Serial.begin(9600);  
  while(Serial.read()>= 0){}//clear serialbuffer
}
int pos=10;
int pos1 = 10;
bool revert = false;
void loop() {
  // put your main code here, to run repeatedly:
  //my_servo.write(90);//50--115
  
     for(pos = 50; pos < 165; pos += 1){    // 舵机从0°转到180°，每次增加1°          
      my_servo.write(pos);           // 给舵机写入角度   
      delay(15);                    // 延时15ms让舵机转到指定位置
   }
   delay(500);
    for(pos = 165; pos>=50; pos-=1) {    // 舵机从180°转回到0°，每次减小1°                               
       my_servo.write(pos);        // 写角度到舵机     
       delay(15);                 // 延时15ms让舵机转到指定位置
    } 
    delay(500);

  /*
   if (Serial.available() > 0) {  
    //delay(100); // 等待数据传完  
    char data = Serial.read();
    if(data>'0'&&data<='9')    
    { 
      if(pos == 160) revert = true;
      else if(pos == 10) revert = false;
      if(revert )pos -=1;
      else pos+=1;
      my_servo.write(pos);
      Serial.print("Serial.readBytes = :");
      Serial.println(pos);
   //   delay(15);
    }  
  //  Serial.print("Serial.readBytes = :");  
  //  Serial.println(data);  
  }  
  */
  
}
