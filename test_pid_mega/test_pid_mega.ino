#include "define.h"
#include <Timer.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <PID_v1.h>
Timer t;
PS2X ps2x; // create PS2 Controller Class
int error = 0;
byte type = 0;
byte vibrate = 0;


PID left_PID1(&left_Input1, &left_Output1, &left_Setpoint1, Kp, Ki, Kd, DIRECT);
PID left_PID2(&left_Input2, &left_Output2, &left_Setpoint2, Kp, Ki, Kd, DIRECT);
PID right_PID1(&right_Input1, &right_Output1, &right_Setpoint1, Kp, Ki, Kd, DIRECT);
PID right_PID2(&right_Input2, &right_Output2, &right_Setpoint2, Kp, Ki, Kd, DIRECT);
int pwm = 100;
void init_data()
{
    left1_counter = 0;
    left2_counter = 0;
    right1_counter = 0;
    right2_counter = 0;
}

void init_gpio()
{
    pinMode(3,OUTPUT);
    pinMode(22,OUTPUT);
    pinMode(23,OUTPUT);

    pinMode(5,OUTPUT);
    pinMode(24,OUTPUT);
    pinMode(25,OUTPUT);

    pinMode(8,OUTPUT);
    pinMode(33,OUTPUT);
    pinMode(32,OUTPUT);

    pinMode(9,OUTPUT);
    pinMode(30,OUTPUT);
    pinMode(31,OUTPUT);

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

        //left1_speed = test*3.14*0.067*(1000.0/time_interval)/(21.3*11.0);//减速比21.3，转一圈11个脉冲，直径0.067m
        left1_speed = test;
        //Serial.print("left1_speed:");
        //Serial.print(left1_speed);
    }
    //left2_speed
    {
        float aa = last_time1- last_left2_time;
        if(left2_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_left2_time ); 
        //left2_speed = test*3.14*0.067*(1000.0/time_interval)/(21.3*11.0);//减速比21.3，转一圈11个脉冲，直径0.067m
        left2_speed = test;
        //Serial.print("left2_speed:");
        //Serial.print(left2_speed);
    }
    //right1_speed
    {
        float aa = last_time1- last_right1_time;
        if(right1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_right1_time ); 
        //right1_speed = test*3.14*0.067*(1000.0/time_interval)/(21.3*11.0);//减速比21.3，转一圈11个脉冲，直径0.067m
        right1_speed = test;
        //Serial.print("right1_speed:");
        //Serial.print(right1_speed);
    }
    //right2_speed
    {
        float aa = last_time1- last_right2_time;
        if(right2_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_right2_time ); 
        //right2_speed = test*3.14*0.067*(1000.0/time_interval)/(21.3*11.0);//减速比21.3，转一圈11个脉冲，直径0.067m
        right2_speed = test;
         //Serial.print("right2_speed:");
         //Serial.println(right2_speed);
    }

    //Serial.println();

    report_speed(left1_speed,left2_speed,right1_speed,right2_speed);

    //report_speed(left_Output1,left_Output2,right_Output1,right_Output2);
    //report_speed(left_Setpoint1,left_Output1,left_Input1,right_Output2);
    left_Input1 = left1_speed;
    left_Input2 = left2_speed;
    right_Input1 = right1_speed;
    right_Input2 = right2_speed;

    init_data();

}
void left1_interrupt()
{
    left1_counter+=1;
    temp_left1_time = micros()-last_left1_time;
    last_left1_time = micros();
}
void left2_interrupt()
{
    left2_counter+=1;
    temp_left2_time = micros()-last_left2_time;
    last_left2_time = micros();
}
void right1_interrupt()
{
    right1_counter+=1;
    temp_right1_time = micros()-last_right1_time;
    last_right1_time = micros();
}

void right2_interrupt()
{
    right2_counter+=1;
    temp_right2_time = micros()-last_right2_time;
    last_right2_time = micros();
}
void setup() {
    // put your setup code here, to run once:


    left_Setpoint1 = left_Input1 = 0;
    left_Setpoint2 = left_Input2 = 0;
    right_Setpoint1 = right_Input1 = 0;
    right_Setpoint2 = right_Input2 = 0;
    
    //turn the PID on
    left_PID1.SetMode(AUTOMATIC);
    left_PID2.SetMode(AUTOMATIC);
    right_PID1.SetMode(AUTOMATIC);
    right_PID2.SetMode(AUTOMATIC);
   
    left_PID1.SetOutputLimits(50,100);
    left_PID2.SetOutputLimits(50,100);
    right_PID1.SetOutputLimits(50,100);
    right_PID2.SetOutputLimits(50,100);

    
    left_PID1.SetSampleTime(50);
    left_PID2.SetSampleTime(50);
    right_PID1.SetSampleTime(50);
    right_PID2.SetSampleTime(50);
    
    
    

    int tickEvent = t.every(time_interval, counter_timeout);
    Serial.begin(9600);
    while(Serial.read()>= 0){}
    init_gpio();
    attachInterrupt( 0, right1_interrupt, FALLING);//pin2
    attachInterrupt( 3, right2_interrupt, FALLING);//pin20
    attachInterrupt( 4, left2_interrupt, FALLING);//pin19
    attachInterrupt( 5, left1_interrupt, FALLING);//pin18


   // set_speed(1,0,1,0,1,0,1,0);

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
        left_PID2.Compute();
        right_PID1.Compute();
        right_PID2.Compute();
        /*
           Serial.print(left_Output1);
           Serial.print(":");
           Serial.print(left_Output2);
           Serial.print(":");
           Serial.println(right_Output1);
         */
    }
    if(left_Setpoint1 == 0)left_Output1 = 0;
    if(left_Setpoint2 == 0)left_Output2 = 0;
    if(right_Setpoint1 == 0)right_Output1 = 0;
    if(right_Setpoint2 == 0)right_Output2 = 0;
    set_pwm((unsigned char)left_Output1,(unsigned char)left_Output2,(unsigned char)right_Output1,(unsigned char)right_Output2);
    
    //set_pwm(100,100,100,100);
   
}
