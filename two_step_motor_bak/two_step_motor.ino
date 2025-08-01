#include "define.h"
#include <Timer.h>
#include <PS2X_lib.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
Timer t;
PS2X ps2x; // create PS2 Controller Class
int error = 0;
byte type = 0;
byte vibrate = 0;
bool use_ps2 = false;

int pwm = 100;
void init_data()
{
    left_counter = 0;
    right_counter = 0;
}

void init_gpio()
{

    //left
    pinMode(2,OUTPUT);//PUL
    pinMode(3,OUTPUT);//DIR
    pinMode(4,OUTPUT);//ENA



    //right
    pinMode(6,OUTPUT);//PUL
    pinMode(7,OUTPUT);//DIR
    pinMode(8,OUTPUT);//ENA

    digitalWrite(3,LOW);
    digitalWrite(4,LOW);


    digitalWrite(7,LOW);
    digitalWrite(8,LOW);

}

static int last_left1_is_forward = -1;
static int last_right1_is_forward = -1;
static int last_left2_is_forward = -1;
static int last_right2_is_forward = -1;

//float temp_left1_time,temp_left2_time,temp_right1_time,temp_right2_time;
//float last_left1_time,last_left2_time,last_right1_time,last_right2_time;
//float last_time1;

bool up_pressed = false;
bool down_pressed = false;
bool left_pressed = false;
bool right_pressed = false;

void counter_timeout()//定时器中断处理函数
{
    //return;
    //Serial.print("left speed:");
    //float left_speed = (left1_counter/2/200.0)*(3.14 * 0.067)*1000.0/time_interval;
    //Serial.println(left_speed);
    //float right_speed = (right1_counter/2/200.0)*(3.14 * 0.067)*1000.0/time_interval;
    unsigned int left_count = left_counter/2;
    unsigned int right_count = right_counter/2;   
    report_speed(left_count,left_count,right_count,right_count);
    init_data();
}






void setup() {
    // put your setup code here, to run once:


    int tickEvent = t.every(time_interval, counter_timeout);
    Serial.begin(9600);
    while(Serial.read()>= 0){}
    init_gpio();

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

unsigned long current_micros = 0;
unsigned long last_left_micros = 0;
unsigned long last_right_micros = 0;

unsigned long ps2_duration = 50000;
unsigned long last_ps2_micros = 0;
byte left_pul = HIGH;
byte right_pul = HIGH;
bool first_run = true;

void move_left(byte pin_value)
{
    if(!should_left_stop)
    {
        digitalWrite(2,pin_value);
        left_counter +=1;
    }
}
void move_right(byte pin_value)
{
    if(!should_right_stop)
    {
        digitalWrite(6,pin_value);
        right_counter +=1;
    }
}
void loop() {
    // put your main code here, to run repeatedly:
    //set_speed(1,255,1,180,1,255,1,150);

    //process_rf_command(1);
    t.update();
    current_micros = micros();
    if(first_run)
    {
        first_run = false;
        last_left_micros = current_micros;
        last_right_micros = current_micros;
        last_ps2_micros = current_micros;
    }
    //Serial.println(left_duration);
    if(current_micros - last_left_micros >= left_duration)
    {
        if(left_pul == HIGH) left_pul = LOW;
        else left_pul = HIGH;
        last_left_micros = current_micros;// +=left_duration;
        move_left(left_pul );
    }
    if(current_micros - last_right_micros >= right_duration)
    {
        if(right_pul == HIGH) right_pul = LOW;
        else right_pul = HIGH;
        last_right_micros = current_micros;//+=right_duration;
        move_right(right_pul );
    }
    //move_left(left_pul );
    //move_right(right_pul );

    if(uart_new_msg)
    {
        message_decoder();
        for (unsigned int i=0;i<RECV_BUFFER_SIZE;i++)
        {
            recv_buffer[i]=0;
        }
    }
    if(use_ps2 && (current_micros - last_ps2_micros >=ps2_duration))
    {
        last_ps2_micros = current_micros;
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

        //delay(50);  
    }

}
