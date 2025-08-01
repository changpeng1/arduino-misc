#include "define.h"
#include <Timer.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <PS2X_lib.h>

#include <Servo.h>
const int TrigPin = A0;
const int EchoPin = 10;
#define CHANNEL2 12
#define CHANNEL3 13
Servo left_hip_servo;
Servo left_knee_servo;
Servo left_feet_servo;
Servo right_hip_servo;
Servo right_knee_servo;
Servo right_feet_servo;
//TODO:收到新的按键指令后，应该停止调速，否则响应按键会延迟
Timer t;
PS2X ps2x; // create PS2 Controller Class

bool use_ps2 = true;
int error = 0;
byte type = 0;
byte vibrate = 0;

int left_pwm = 100;
double right_pwm = 80;
int timer_interval = 100;//定时器的间隔，单位ms
void init_data()
{
    last_left_counter = 0;
    left_counter = 0;
    last_right_counter = 0;
    right_counter = 0;
}

void init_gpio()
{
    pinMode(9,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(8,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);
    pinMode(7,OUTPUT);

}


float temp_left_time,temp_right_time;
float last_left_time,last_right_time;
float last_time1;
void counter_timeout()//定时器中断处理函数
{
}


float cm;
bool up_pressed = false;
bool down_pressed = false;
bool left_pressed = false;
bool right_pressed = false;


void onChange()
{
    right_counter+=1;
    temp_right_time = micros()-last_right_time;
    last_right_time = micros();
}

void onChange1()
{
    left_counter+=1;
    temp_left_time = micros()-last_left_time;
    last_left_time = micros();
}

void setup() {
    // put your setup code here, to run once:

    int tickEvent = t.every(timer_interval, counter_timeout);

  left_hip_servo.attach(9);
  left_knee_servo.attach(10);
  left_feet_servo.attach(11);
  right_hip_servo.attach(3);
  right_knee_servo.attach(5);
  right_feet_servo.attach(6);
    Serial.begin(9600);
    while(Serial.read()>= 0){}
left_hip_servo.write(90);
left_knee_servo.write(90);
left_feet_servo.write(90);
right_hip_servo.write(90);
right_knee_servo.write(90);
right_feet_servo.write(90);

}

void loop() {
    // put your main code here, to run repeatedly:

    // t.update();
//left_hip_servo.write(90);
//left_knee_servo.write(90);
    
       if(uart_new_msg)
       {
       message_decoder();
       for (unsigned int i=0;i<RECV_BUFFER_SIZE;i++)
       {
       recv_buffer[i]=0;
       }
       }

}
