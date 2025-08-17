#include <Servo.h>
char buff[10];
Servo my_servo;
Servo my_servo1;
void setup() {
  // put your setup code here, to run once:
  my_servo.attach(PA0);
  my_servo1.attach(PA1);
 Serial.begin(115200);  
  while(Serial.read()>= 0){}//clear serialbuffer
}
int pos=10;
int pos1 = 10;
bool revert = false;
void loop() {
  // put your main code here, to run repeatedly:
  //my_servo.write(90);//50--115
  
     for(pos = 0; pos < 180; pos += 1){    // 舵机从0°转到180°，每次增加1°          
      my_servo.write(pos);           // 给舵机写入角度   
      delay(15);                    // 延时15ms让舵机转到指定位置
   }
   delay(500);
    for(pos = 180; pos>=0; pos-=1) {    // 舵机从180°转回到0°，每次减小1°                               
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
#if 0

#define SERVO_PIN PA0  // 舵机信号线接PA0

void setup() {
  pinMode(SERVO_PIN, OUTPUT);
  analogWriteFrequency( 50);  // 设置50Hz频率
  analogWriteResolution(12);            // 推荐12位分辨率(0-4095)
}

// 角度控制函数 (0°~180°)
void setServoAngle(int angle) {
  // 将角度转换为脉冲宽度(微秒)
  int pulseWidth = map(angle, 0, 180, 500, 2500);
  
  // 计算PWM值 (12位分辨率)
  // PWM值 = (脉冲宽度 / 周期) * 最大计数值
  int pwmValue = (pulseWidth * 4095) / 20000;  // 20000us=20ms周期 (pulseWidth/20000 * 4095)
  
  analogWrite(SERVO_PIN, pwmValue);
}

void loop() {
  setServoAngle(0);    // 0°位置
  delay(1000);
  setServoAngle(90);   // 90°位置
  delay(1000);
  setServoAngle(180);  // 180°位置
  delay(1000);
}
#endif
