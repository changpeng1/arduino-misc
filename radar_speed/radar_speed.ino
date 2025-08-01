const int TrigPin = 12;
const int EchoPin = 2;
float cm;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(TrigPin,OUTPUT);//Trig
  pinMode(EchoPin,INPUT);//Echo
}

void loop() {
  digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
  delayMicroseconds(2);       // delayMicroseconds在更小的时间内延时准确
  digitalWrite(TrigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TrigPin, LOW); //通过这里控制超声波的发射
  
  cm = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
  cm = (long(cm * 100.0)) / 100.0; //保留两位小数 
  Serial.print("Distance:"); 
  Serial.print(cm); 
  Serial.print("cm"); 
  Serial.println(); 
  delay(200); 
}
