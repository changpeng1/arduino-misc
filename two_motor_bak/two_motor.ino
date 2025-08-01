#include "define.h"
#include <Timer.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <PID_v1.h>
bool use_ps2 = false;
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
    }
    //right1_speed
    {
        float aa = last_time1- last_right1_time;
        if(right1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_right1_time ); 
        //right1_speed = test*3.14*0.067*(1000.0/time_interval)/(56*11.0);//减速比56，转一圈11个脉冲，直径0.067m
        right1_speed = test;
    }
    //Serial.println();
    report_speed(left1_speed,right1_speed);
    left_Input1 = left1_speed;
    right_Input1 = right1_speed;

    init_data();




}


const int TrigPin = A0;
const int EchoPin = 8;
float cm;
bool up_pressed = false;
bool down_pressed = false;
bool left_pressed = false;
bool right_pressed = false;

void left1_interrupt()
{
    left1_counter+=1;
    temp_left1_time = micros()-last_left1_time;
    last_left1_time = micros();
}
/*
   void left2_interrupt()
   {
   left2_counter+=1;
   temp_left2_time = micros()-last_left2_time;
   last_left2_time = micros();
   }
 */
void right1_interrupt()
{
    right1_counter+=1;
    temp_right1_time = micros()-last_right1_time;
    last_right1_time = micros();
}
/*
   void right2_interrupt()
   {
   right2_counter+=1;
   temp_right2_time = micros()-last_right2_time;
   last_right2_time = micros();
   }
 */
void setup() {
    // put your setup code here, to run once:


    left_Setpoint1 = left_Input1 = 0;
    right_Setpoint1 = right_Input1 = 0;
    //turn the PID on
    left_PID1.SetMode(AUTOMATIC);
    //left_PID2.SetMode(AUTOMATIC);
    right_PID1.SetMode(AUTOMATIC);
    //right_PID2.SetMode(AUTOMATIC);

    int tickEvent = t.every(time_interval, counter_timeout);
    Serial.begin(9600);
    while(Serial.read()>= 0){}
    init_gpio();
    // init_rf_receiver();

    //init radar speed mode
    // pinMode(TrigPin,OUTPUT);//Trig
    // pinMode(EchoPin,INPUT);//Echo
    attachInterrupt( digitalPinToInterrupt(3), left1_interrupt, FALLING);
    attachInterrupt( digitalPinToInterrupt(2), right1_interrupt, FALLING);


    //set_speed(1,0,1,0,1,0,1,0);
    //set_speed(1,100,1,100,1,50,1,100);

    //Serial.println(micros());


    if(use_ps2)
    {
        //setup PS2
        delay(300);
        error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

        if(error == 0){
            Serial.println("Found Controller, configured successful ");
        }  
        else if(error == 1)
            Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

        else if(error == 2)
            Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

        else if(error == 3)
            Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

        //  Serial.print(ps2x.Analog(1), HEX);

        type = ps2x.readType(); 
        switch(type) {
            case 0:
                Serial.print("Unknown Controller type found ");
                break;
            case 1:
                Serial.print("DualShock Controller found ");
                break;
            case 2:
                Serial.print("GuitarHero Controller found ");
                break;
            case 3:
                Serial.print("Wireless Sony DualShock Controller found ");
                break;
        }
    }
}

//int aa = 100;

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
    set_pwm((unsigned char)left_Output1,(unsigned char)right_Output1);
    //set_pwm(100,100,100,100); 


    if(use_ps2)
    {
        if(error == 1) //skip loop if no controller found
            return; 
        if(type == 2){ //Guitar Hero Controller

        }
        else { //DualShock Controller
            ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

            if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
                Serial.println("Start is being held");
            if(ps2x.Button(PSB_SELECT))
                Serial.println("Select is being held");      

            if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
                //Serial.print("Up held this hard: ");
                //Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
                process_rf_command(1);
                up_pressed = true;
            }
            if(ps2x.Button(PSB_PAD_RIGHT)){
                //Serial.print("Right held this hard: ");
                //Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
                //set_speed(1,left_pwm,0,right_pwm);
                process_rf_command(4);
                right_pressed = true;
            }
            if(ps2x.Button(PSB_PAD_LEFT)){
                //Serial.print("LEFT held this hard: ");
                //Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
                process_rf_command(3);
                left_pressed = true;
            }
            if(ps2x.Button(PSB_PAD_DOWN)){
                //Serial.print("DOWN held this hard: ");
                //Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
                process_rf_command(2);
                down_pressed = true;
            }   
            if(!up_pressed && !down_pressed &&!left_pressed&&!right_pressed)
                //set_speed(1,0,1,0);
                process_rf_command(0);
            vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
            if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
                if(ps2x.Button(PSB_L3))
                    Serial.println("L3 pressed");
                if(ps2x.Button(PSB_R3))
                    Serial.println("R3 pressed");
                if(ps2x.Button(PSB_L2))
                    Serial.println("L2 pressed");
                if(ps2x.Button(PSB_R2))
                    Serial.println("R2 pressed");
                if(ps2x.Button(PSB_TRIANGLE))
                    Serial.println("Triangle pressed");        
            }

            if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
                Serial.println("Circle just pressed");
            if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
                Serial.println("X just changed");
            if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
                Serial.println("Square just released");     

            if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
                Serial.print("Stick Values:");
                Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
                Serial.print(",");
                Serial.print(ps2x.Analog(PSS_LX), DEC); 
                Serial.print(",");
                Serial.print(ps2x.Analog(PSS_RY), DEC); 
                Serial.print(",");
                Serial.println(ps2x.Analog(PSS_RX), DEC); 
            }
            up_pressed = false;
            down_pressed = false;
            left_pressed = false;
            right_pressed = false;
        }

        delay(50);  
    }

}
