int pinBuzzer = 3; //管脚D3连接到蜂鸣器元件的基极
 
void setup() {
  pinMode(pinBuzzer, OUTPUT); //设置pinBuzzer脚为输出状态
}
 
void loop() { 
   digitalWrite(pinBuzzer, HIGH);//输出HIGH电平,停止发声
   delay(50); //等待3000毫秒
 
   digitalWrite(pinBuzzer, LOW);//输出LOW电平,发声
   delay(50); //等待1000毫秒
}
