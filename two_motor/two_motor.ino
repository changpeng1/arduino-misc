#include "define.h"
#include <Timer.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <PID_v1.h>
bool use_ps2 = false;
bool use_joystick = false;
Timer t;

byte type = 0;
byte vibrate = 0;
bool use_new_pid = false;

#define LEFT_INT_PIN 2
#define RIGHT_INT_PIN 3

#define LEFT_IN1 4
#define LEFT_IN2 5
#define LEFT_PWM 6

#define RIGHT_IN1 7
#define RIGHT_IN2 8
#define RIGHT_PWM 9

#define CHANNEL4 11 //左右
#define CHANNEL2 12// 前进，后退
#define CHANNEL3 13 //油门

PID left_PID1(&left_Input1, &left_Output1, &left_Setpoint1, Kp, Ki, Kd, DIRECT);
PID left_PID2(&left_Input2, &left_Output2, &left_Setpoint2, Kp, Ki, Kd, DIRECT);
PID right_PID1(&right_Input1, &right_Output1, &right_Setpoint1, Kp, Ki, Kd, DIRECT);
PID right_PID2(&right_Input2, &right_Output2, &right_Setpoint2, Kp, Ki, Kd, DIRECT);
int pwm = 100;
void init_data()
{
    left1_counter = 0;
    right1_counter = 0;
}

void init_gpio()
{
    //left
    pinMode(LEFT_IN1,OUTPUT);
    pinMode(LEFT_IN2,OUTPUT);
    pinMode(LEFT_PWM,OUTPUT);

    //right
    pinMode(RIGHT_IN1,OUTPUT);
    pinMode(RIGHT_IN2,OUTPUT);
    pinMode(RIGHT_PWM,OUTPUT);

    if(use_joystick)
    {
        pinMode(CHANNEL2,INPUT);
        pinMode(CHANNEL3,INPUT);
        pinMode(CHANNEL4,INPUT);
    }
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

        //left1_speed = test*3.14*0.067*(1000.0/time_interval)/(56*11.0);//减速比56，转一圈11个脉冲，直径0.067m
        left1_speed = test;
        left_Input1 = left1_speed;
    }
    //right1_speed
    {
        float aa = last_time1- last_right1_time;
        if(right1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_right1_time ); 
        //right1_speed = test*3.14*0.067*(1000.0/time_interval)/(56*11.0);//减速比56，转一圈11个脉冲，直径0.067m
        right1_speed = test;
        right_Input1 = right1_speed;
    }
    /*
       Serial.print("left_speed:");
       Serial.print(left1_speed);
       Serial.print("right_speed:");
       Serial.print(right1_speed);
       Serial.println();
     */
    //report_speed();



    //init_data();




}


const int TrigPin = A0;
const int EchoPin = 8;
float cm;
bool up_pressed = false;
bool down_pressed = false;
bool left_pressed = false;
bool right_pressed = false;

void left_interrupt()
{
    left1_counter+=1;
    temp_left1_time = micros()-last_left1_time;
    last_left1_time = micros();
}

void right_interrupt()
{
    right1_counter+=1;
    temp_right1_time = micros()-last_right1_time;
    last_right1_time = micros();
}

void setup() {
    // put your setup code here, to run once:


    left_Setpoint1 = left_Input1 = 0;
    right_Setpoint1 = right_Input1 = 0;
    //turn the PID on
    left_PID1.SetMode(AUTOMATIC);
    right_PID1.SetMode(AUTOMATIC);

    int tickEvent = t.every(time_interval, counter_timeout);
    Serial.begin(115200);
    while(Serial.read()>= 0){}
    init_gpio();
    // init_rf_receiver();

    //init radar speed mode
    // pinMode(TrigPin,OUTPUT);//Trig
    // pinMode(EchoPin,INPUT);//Echo
    attachInterrupt( digitalPinToInterrupt(LEFT_INT_PIN), left_interrupt, FALLING);
    attachInterrupt( digitalPinToInterrupt(RIGHT_INT_PIN), right_interrupt, FALLING);


    //set_speed(1,0,1,0,1,0,1,0);
    //set_speed(1,100,1,100,1,50,1,100);

    //set_speed(0,50,1,0);
    //Serial.println(micros());
    //float speed = 0.1;
      //  int count = (int)(speed*56*11)/(1000/time_interval)/(3.14*0.067);
        //set_speed(1,count,1,count);

    if(use_ps2)
    {
        //setup PS2
        delay(300);
        init_ps2();
    }
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
        right_PID1.Compute();
    }
    if(left_Setpoint1 == 0)left_Output1 = 0;
    if(right_Setpoint1 == 0)right_Output1 = 0;
    //Serial.println(left_Output1);
    set_pwm((unsigned char)left_Output1,(unsigned char)right_Output1);
    //set_pwm(100,100,100,100); 
    //set_pwm(100,100);

    if(use_ps2)
    {
        process_ps2_command();
        delay(50);  
    }
    if(use_joystick)
    {
        process_joystick_command();
        delay(50);
    }

}
