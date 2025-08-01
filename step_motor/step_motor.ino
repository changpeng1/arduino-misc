#include <Timer.h>
#include "define.h"
bool debug = false;
const int TrigPin = A0;
const int EchoPin = 8;
unsigned int cm;
Timer t;

static int last_left1_is_forward = -1;
static int last_right1_is_forward = -1;
static int last_left2_is_forward = -1;
static int last_right2_is_forward = -1;

bool first_run = true;
unsigned char first_distance()//最开始运行时先获取一次距离
{
  digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
  delayMicroseconds(2);       // delayMicroseconds在更小的时间内延时准确
  digitalWrite(TrigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TrigPin, LOW); //通过这里控制超声波的发射
  
  int distance = pulseIn(EchoPin, HIGH) / 58.3; //将回波时间换算成cm 
  if(distance <0 || distance > 255) distance = 255;//如果为负值，返回一个极大值
  
  return (unsigned char)distance;
  }
void counter_timeout()//定时器中断处理函数
{
    digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
  delayMicroseconds(2);       // delayMicroseconds在更小的时间内延时准确
  digitalWrite(TrigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TrigPin, LOW); //通过这里控制超声波的发射
  
  float value = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
  //cm = (int(cm * 100.0)) / 100.0; //保留两位小数 
  if(value < 0 || value > 255) value = 255;
  cm = (unsigned char)value;
  if(debug)
  {
    Serial.print("Distance:"); 
    Serial.print(cm); 
    Serial.print("cm"); 
    Serial.println();
  }
  report_value();
}
bool clock_wise = true;
void setup() {
  // put your setup code here, to run once:
    pinMode(2,OUTPUT);//PUL
    pinMode(3,OUTPUT);//DIR
    pinMode(4,OUTPUT);//ENA


    pinMode(6,OUTPUT);//PUL
    pinMode(7,OUTPUT);//DIR
    pinMode(8,OUTPUT);//ENA

    digitalWrite(3,LOW);
     digitalWrite(7,LOW);
    
    digitalWrite(4,LOW);//HIGH 不使能步进电机
    digitalWrite(8,LOW);

  //if(debug)
  {
    Serial.begin(9600);
    while(Serial.read()>= 0){}
  }
  //pinMode(TrigPin,OUTPUT);//Trig
  //pinMode(EchoPin,INPUT);//Echo
  int tickEvent = t.every(time_interval, counter_timeout);
}
unsigned long int counter = 0; 
unsigned char index = 0;//0-99
void loop() {
  // put your main code here, to run repeatedly:
  /*
  if(first_run)
  {
    first_run = false;
    cm = first_distance();
    index = 0;
    request_buffer[index] = cm;
    }
  
t.update();
if(counter == 3200)//180度
{
  counter = 0;
  delay(1000);
  if(clock_wise)
  {
    clock_wise = false;
  digitalWrite(3,HIGH);
  index = 99;
  }
  else
  {
    clock_wise = true;
  digitalWrite(3,LOW);
  index = 0;
    }
    
  }
  else
{
  digitalWrite(2,HIGH);
  delayMicroseconds(500);
  digitalWrite(2,LOW);
  delayMicroseconds(500);
  counter+=1;

}
  if(counter %32 == 0)
  {
    if(index < 0) index = 0;
    request_buffer[index] = cm;
    if(clock_wise) index++;
    else index--;
    }
//delay(10);
*/

for(int i = 0;i<6400;i++)
{
digitalWrite(2,HIGH);
// digitalWrite(6,HIGH);
  delayMicroseconds(100);
  digitalWrite(2,LOW);
  // digitalWrite(6,LOW);
  delayMicroseconds(100);
}
delay(2000);

}
