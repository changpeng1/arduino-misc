#include "define.h"
#include <Timer.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <PID_v1.h>
#include <util/atomic.h>
Timer t;
PS2X ps2x; // create PS2 Controller Class
int error = 0;
byte type = 0;
byte vibrate = 0;
bool debug = true;


PID left_PID1(&left_Input1, &left_Output1, &left_Setpoint1, Kp, Ki, Kd, DIRECT);
PID right_PID1(&right_Input1, &right_Output1, &right_Setpoint1, Kp, Ki, Kd, DIRECT);
int pwm = 100;
void init_data()
{
    left1_counter = 0;
    right1_counter = 0;
}

void init_gpio()
{
    //left
    pinMode(6,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(5,OUTPUT);
    //right
    pinMode(9,OUTPUT);
    pinMode(7,OUTPUT);
    pinMode(8,OUTPUT);


}

static int last_left1_is_forward = -1;
static int last_right1_is_forward = -1;

float temp_left1_time,temp_right1_time;
float last_left1_time,last_right1_time;
float last_time1;
float v1Filt = 0;
float v1Prev = 0;

void counter_timeout()//定时器中断处理函数
{
    float kk = micros()-last_time1;
    last_time1 = micros();

    double left1_speed;
    double right1_speed;
    double test,test1;
    //left1_speed
    {
        float aa = last_time1- last_left1_time;
        //int test;
        if(left1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_left1_time );

        float readable_speed = test*3.14*0.067*(1000.0/time_interval)/(56*11.0);//减速比21.3，转一圈11个脉冲，直径0.067m
        //left_Input1 = readable_speed;
        left1_speed = test;
        left_Input1 = left1_speed;

    }

    //right1_speed
    {
        float aa = last_time1- last_right1_time;
        if(right1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_right1_time ); 

        v1Filt = 0.854*v1Filt + 0.0728*test + 0.0728*v1Prev;
        v1Prev = test;
        float readable_speed = test*3.14*0.067*(1000.0/time_interval)/(56*11.0);//减速比21.3，转一圈11个脉冲，直径0.067m//50
        if(readable_speed > 2.0) return;
        //right_Input1 = readable_speed;
        right1_speed = test;
        if(debug)
        {
            Serial.print(30);
            Serial.print(" ");
            Serial.println(readable_speed*100);
        }
    }

    right_Input1 = right1_speed;

    init_data();

}

void left_interrupt()
{
    left1_counter+=1;
    temp_left1_time = micros()-last_left1_time;
    last_left1_time = micros();
    //left_Input1 = (1000000/temp_left1_time/(56.0*11))*3.14*0.067;


}
float velocity_i = 0;
void right_interrupt()
{
    right1_counter+=1;
    long curT = micros();
    temp_right1_time = ((float)(curT-last_right1_time))/1.0e6;
    velocity_i = 1/temp_right1_time;
    last_right1_time = curT;
}
float speed = 0.1;
void setup() {
    // put your setup code here, to run once:


    left_Setpoint1 = left_Input1 = 0;
    right_Setpoint1 = right_Input1 = 0;

    //turn the PID on
    left_PID1.SetMode(AUTOMATIC);
    right_PID1.SetMode(AUTOMATIC);

    //    left_PID1.SetOutputLimits(50,250);
    //    right_PID1.SetOutputLimits(50,250);
    //
    //    
    //    left_PID1.SetSampleTime(50);
    //    right_PID1.SetSampleTime(50);




    //int tickEvent = t.every(time_interval, counter_timeout);
    Serial.begin(9600);
    while(Serial.read()>= 0){}
    init_gpio();
    attachInterrupt( digitalPinToInterrupt(2), left_interrupt, FALLING);
    attachInterrupt( digitalPinToInterrupt(3), right_interrupt, FALLING);


    //int count = (int)(speed*56*11)/(1000/time_interval)/(3.14*0.067);
    int count = speed/(3.14*0.067)*60;
    set_speed(1,count,1,count);

}
float v2Filt = 0;
float v2Prev = 0;

void loop() {
    // put your main code here, to run repeatedly:

    //t.update();
    if(uart_new_msg)
    {
        message_decoder();
        for (unsigned int i=0;i<RECV_BUFFER_SIZE;i++)
        {
            recv_buffer[i]=0;
        }
    }


    {
        float velocity2 = 0;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
            velocity2 = velocity_i;

        }
        float v2 = velocity2/(56*11)*60.0;
        v2Filt = 0.854*v2Filt + 0.0728*v2 + 0.0728*v2Prev;
        v2Prev = v2;
        right_Input1 = v2Filt;
        float vt = speed/(3.14*0.067)*60;
        Serial.print(vt);
        Serial.print(" ");
        Serial.print(v2);
        Serial.print(" ");
        Serial.print(v2Filt);
        Serial.println();
    }
    left_PID1.Compute();
    right_PID1.Compute();
    if(left_Setpoint1 == 0)left_Output1 = 0;
    if(right_Setpoint1 == 0)right_Output1 = 0;
    set_pwm((unsigned char)left_Output1,(unsigned char)right_Output1);

}
