#include <Timer.h>
#include <PID_v1.h>
#include "define.h"

#include <PWM.h>

// Set pin 9's PWM frequency to 3906 Hz (31250/8 = 3906)
  
// Note that the base frequency for pins 3, 9, 10, and 11 is 31250 Hz
  
//setPwmFrequency(9, 8);
  
 
  
// Set pin 6's PWM frequency to 62500 Hz (62500/1 = 62500)
  
// Note that the base frequency for pins 5 and 6 is 62500 Hz
  
//setPwmFrequency(6, 1);
  
 
  
// Set pin 10's PWM frequency to 31 Hz (31250/1024 = 31)
  
//setPwmFrequency(10, 1024);
  
 
  
/**
  
 * Divides a given PWM pin frequency by a divisor.
  
 * 
  
 * The resulting frequency is equal to the base frequency divided by
  
 * the given divisor:
  
 *   - Base frequencies:
  
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
  
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
  
 *   - Divisors:
  
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
  
 *        256, and 1024.
  
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
  
 *        128, 256, and 1024.
  
 * 
  
 * PWM frequencies are tied together in pairs of pins. If one in a
  
 * pair is changed, the other is also changed to match:
  
 *   - Pins 5 and 6 are paired on timer0
  
 *   - Pins 9 and 10 are paired on timer1
  
 *   - Pins 3 and 11 are paired on timer2
  
 * 
  
 * Note that this function will have side effects on anything else
  
 * that uses timers:
  
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
  
 *     millis() functions to stop working. Other timing-related
  
 *     functions may also be affected.
  
 *   - Changes on pins 9 or 10 will cause the Servo library to function
  
 *     incorrectly.
  
 * 
  
 * Thanks to macegr of the Arduino forums for his documentation of the
  
 * PWM frequency divisors. His post can be viewed at:
  
 *   http://forum.arduino.cc/index.php?topic=16612#msg121031
  
 */
  
void setPwmFrequency(int pin, int divisor) {
  
  byte mode;
  
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
  
    switch(divisor) {
  
      case 1: mode = 0x01; break;
  
      case 8: mode = 0x02; break;
  
      case 64: mode = 0x03; break;
  
      case 256: mode = 0x04; break;
  
      case 1024: mode = 0x05; break;
  
      default: return;
  
    }
  
    if(pin == 5 || pin == 6) {
  
      TCCR0B = TCCR0B & 0b11111000 | mode;
  
    } else {
  
      TCCR1B = TCCR1B & 0b11111000 | mode;
  
    }
  
  } else if(pin == 3 || pin == 11) {
  
    switch(divisor) {
  
      case 1: mode = 0x01; break;
  
      case 8: mode = 0x02; break;
  
      case 32: mode = 0x03; break;
  
      case 64: mode = 0x04; break;
  
      case 128: mode = 0x05; break;
  
      case 256: mode = 0x06; break;
  
      case 1024: mode = 0x07; break;
  
      default: return;
  
    }
  
    TCCR2B = TCCR2B & 0b11111000 | mode;
  
  }
  
}




Timer t;

PID left_PID1(&left_Input1, &left_Output1, &left_Setpoint1, Kp, Ki, Kd, DIRECT);
//PID left_PID2(&left_Input2, &left_Output2, &left_Setpoint2, Kp, Ki, Kd, DIRECT);
PID right_PID1(&right_Input1, &right_Output1, &right_Setpoint1, Kp, Ki, Kd, DIRECT);
//PID right_PID2(&right_Input2, &right_Output2, &right_Setpoint2, Kp, Ki, Kd, DIRECT);
//int pwm = 180;
void init_data()
{
    left1_counter = 0;
    left2_counter = 0;
    right1_counter = 0;
    right2_counter = 0;
}

void init_gpio()
{
  pinMode(10,OUTPUT);//PWM速度控制
  pinMode(9,OUTPUT);//速度控制
  pinMode(12,OUTPUT);//正反控制
  pinMode(13,OUTPUT);//正反控制

  pinMode(5,OUTPUT);
  
  
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  
  digitalWrite(7,HIGH);
  digitalWrite(8,LOW);

}

static int last_left1_is_forward = -1;
static int last_right1_is_forward = -1;
static int last_left2_is_forward = -1;
static int last_right2_is_forward = -1;

float temp_left1_time,temp_left2_time,temp_right1_time,temp_right2_time;
float last_left1_time,last_left2_time,last_right1_time,last_right2_time;
float last_time1;
void counter_timeout()//定时器中断处理函数
{
    float kk = micros()-last_time1;
    last_time1 = micros();
//Serial.println(last_time1);
//return;
    double left1_speed,left2_speed;
    double right1_speed,right2_speed;
    double test,test1;
    //left1_speed
    {
        float aa = last_time1- last_left1_time;
        //int test;
        if(left1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_left1_time );

        //left1_speed = test*3.14*0.067*(1000.0/time_interval)/(50.0*6.0);//减速比50，转一圈6个脉冲，直径0.067m
        left1_speed = test;//*600/(300);
        if(DEBUG)
        {
          Serial.print("left1_speed:");
          Serial.print(left1_speed);
        }
        
    }
       //right1_speed
    {
        float aa = last_time1- last_right1_time;
        if(right1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_right1_time ); 
        //right1_speed = test*3.14*0.067*(1000.0/time_interval)/(21.3*11.0);//减速比21.3，转一圈11个脉冲，直径0.067m
        right1_speed = test;//*600/(300);
        
        // Serial.print("right1_speed:");
        //Serial.print(right1_counter);
        //Serial.print("====:");
        //Serial.print(right1_speed);
        
    }
    if(DEBUG)
    {
      Serial.println();
    }


    report_speed(left1_speed,left1_speed,right1_speed,right1_speed);

    left_Input1 = left1_speed;
    //left_Input2 = left2_speed;
    right_Input1 = right1_speed;
    //right_Input2 = right2_speed;

    init_data();




}


void left1_interrupt()
{
    //Serial.println(left1_counter);
    left1_counter+=1;
    temp_left1_time = micros()-last_left1_time;
    last_left1_time = micros();
}
void right1_interrupt()
{
    //Serial.println("Found Controller, configured successful ");
    right1_counter+=1;
    temp_right1_time = micros()-last_right1_time;
    last_right1_time = micros();
}
int pwm = 280;
void setup() {
  // put your setup code here, to run once:

     //int fff = 2;  // 可以是 1, 2, 3, 4, 5
      //TCCR0B = TCCR0B & 0xF8 | fff;

//setPwmFrequency(11, 8);//3k


  

      left_Setpoint1 = left_Input1 = 0;
    left_Setpoint2 = left_Input2 = 0;
    right_Setpoint1 = right_Input1 = 0;
    right_Setpoint2 = right_Input2 = 0;
        //turn the PID on
    left_PID1.SetMode(AUTOMATIC);
    //left_PID2.SetMode(AUTOMATIC);
    right_PID1.SetMode(AUTOMATIC);
    //right_PID2.SetMode(AUTOMATIC);


      int tickEvent = t.every(time_interval, counter_timeout);
    Serial.begin(9600);
    while(Serial.read()>= 0){}


InitTimersSafe();
  bool success ;//= SetPinFrequencySafe(10, 10000);
  if(DEBUG)
  {
    if(success)
    {
      Serial.println("AAAAAAAA");
    }
    else
    {
      Serial.println("BBBBBBB");
    }
  }
    init_gpio();
    attachInterrupt( digitalPinToInterrupt(2), left1_interrupt, FALLING);
    attachInterrupt( digitalPinToInterrupt(3), right1_interrupt, FALLING);
    set_speed(1,pwm,1,0,1,pwm,1,0);
    analogWrite(5, 255);//控制L298N输出电压
}

void loop() {
  // put your main code here, to run repeatedly:
t.update();

 if(uart_new_msg)
    {
        message_decoder();
        for (unsigned int i=0;i<RECV_BUFFER_SIZE;i++)
        {
            recv_buffer[i]=0;
        }
    }

    //if(left_Input1-left_Setpoint1 >3 || left_Input1-left_Setpoint1 < -3)
    {
        left_PID1.Compute();
        //left_PID2.Compute();
        right_PID1.Compute();
        //right_PID2.Compute();
                
           //Serial.println(left_Output1);
           //Serial.print(":");
           //Serial.print(left_Output2);
           //Serial.print(":");
           //Serial.println(right_Output1);
         

    }
    if(left_Setpoint1 == 0)left_Output1 = 0;
    if(left_Setpoint2 == 0)left_Output2 = 0;
    if(right_Setpoint1 == 0)right_Output1 = 0;
    if(right_Setpoint2 == 0)right_Output2 = 0;
    set_pwm((unsigned char)left_Output1,(unsigned char)left_Output2,(unsigned char)right_Output1,(unsigned char)right_Output2);

//set_pwm(100,100,100,100);
/*
if (Serial.available() > 0) {
  char data = Serial.read();
  Serial.print("===");
  Serial.println(data);
  //if(data == '0') digitalWrite(2,LOW);
  //else if(data == '1')digitalWrite(2,HIGH);
  //else if(data == '3')digitalWrite(5,0);
  if(data == '1') pwm +=1;
  else pwm -=1;
    set_pwm(pwm,100,100,100);
}
*/
}
