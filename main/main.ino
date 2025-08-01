#include "define.h"
#include <Timer.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <PS2X_lib.h>
const int TrigPin = A0;
const int EchoPin = 10;
#define CHANNEL2 12
#define CHANNEL3 13
//TODO:收到新的按键指令后，应该停止调速，否则响应按键会延迟
Timer t;
PS2X ps2x; // create PS2 Controller Class

bool use_ps2 = false;
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
float get_distance()
{
  digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
  delayMicroseconds(2);       // delayMicroseconds在更小的时间内延时准确
  digitalWrite(TrigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TrigPin, LOW); //通过这里控制超声波的发射
  
  cm = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
   cm = (long(cm * 100.0)) / 100.0; //保留两位小数 
   /*
  Serial.print("Distance:"); 
  Serial.print(cm); 
  Serial.print("cm"); 
  Serial.println();
  */
  return cm;
  }
void setup() {
    // put your setup code here, to run once:

    int tickEvent = t.every(timer_interval, counter_timeout);
    //pinMode(3, INPUT);//左侧测速接口
    //pinMode(2, INPUT);//右侧测速接口
    //pinMode(CHANNEL2,0);
    //pinMode(CHANNEL3,0);

    Serial.begin(9600);
    while(Serial.read()>= 0){}



    init_gpio();
      pinMode(TrigPin,OUTPUT);//Trig
  pinMode(EchoPin,INPUT);//Echo
    //set_speed(1,100,1,0);

    //init_rf_receiver();

    //init radar speed mode
    //pinMode(TrigPin,OUTPUT);//Trig
    //pinMode(EchoPin,INPUT);//Echo

    // attachInterrupt( digitalPinToInterrupt(2), onChange, FALLING);
    // attachInterrupt( digitalPinToInterrupt(3), onChange1, FALLING);
    last_left_time = last_right_time = micros();
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
void turn_left()
{
  set_speed(0,left_pwm,1,right_pwm);
  Serial.println("move left");
  }
  void move_forward()
  {
    set_speed(1,left_pwm,1,right_pwm);
    Serial.println("move forward");
    }
int count,count1;
void loop() {
    // put your main code here, to run repeatedly:

    // t.update();
    
 float distance = get_distance();
 if(distance > 0 && distance < 15)
 {
  count +=1;
  if(count >=10)
  {
    count = 0;
    //stop_move();
    delay(100);
    turn_left();
    count1 = 0;
    }
  }
  else
  {
    count1 += 1;
    if(count1 >=10)
    {
      count1 = 0;
      move_forward();
      count = 0;
    }
    
    
    }
    
//set_speed(1,left_pwm,0,right_pwm);

    /*
       if(uart_new_msg)
       {
       message_decoder();
       for (unsigned int i=0;i<RECV_BUFFER_SIZE;i++)
       {
       recv_buffer[i]=0;
       }
       }
     */
    /*
       if(Mirf.dataReady()) 
       {  //当接收到程序，便从串口输出接收到的数据
       Mirf.getData((byte *) &rf_value);
       process_rf_command(rf_value);
       Serial.print("Got data: ");
       Serial.println(rf_value);
       }
       digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
       delayMicroseconds(2);       // delayMicroseconds在更小的时间内延时准确
       digitalWrite(TrigPin, HIGH); 
       delayMicroseconds(10); 
       digitalWrite(TrigPin, LOW); //通过这里控制超声波的发射

       cm = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
       cm = (int(cm * 100.0)) / 100.0; //保留两位小数 
       if(cm < 20.0 && cm > 0.0)
       {
       if(rf_value !=2)//如果距离小于20cm，且不是后退命令，则停止小车前进
       process_rf_command(0);
       }
     */
    /*
       Serial.print("Distance:"); 
       Serial.print(cm); 
       Serial.print("cm"); 
       Serial.println();
     */ 

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
            Serial.print("Up held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
            set_speed(1,left_pwm,1,right_pwm);
            up_pressed = true;
        }
        if(ps2x.Button(PSB_PAD_RIGHT)){
            Serial.print("Right held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
            set_speed(0,left_pwm,1,right_pwm);
            right_pressed = true;
        }
        if(ps2x.Button(PSB_PAD_LEFT)){
            Serial.print("LEFT held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
            set_speed(1,left_pwm,0,right_pwm);
            left_pressed = true;
        }
        if(ps2x.Button(PSB_PAD_DOWN)){
            Serial.print("DOWN held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
            set_speed(0,left_pwm,0,right_pwm);
            down_pressed = true;
        }   
        if(!up_pressed && !down_pressed &&!left_pressed&&!right_pressed)
            set_speed(1,0,1,0);
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
    /*

       unsigned long channel2_value,channel3_value;
       channel2_value = pulseIn(CHANNEL2,1);//860-1366-1847
       channel3_value = pulseIn(CHANNEL3,1);//980-1980
       Serial.print("CHANNEL2=");
       Serial.print(channel2_value);
       Serial.print("      CHANNEL3=");
       Serial.println(channel3_value);
    //AIL.writeMicroseconds(in_value);
    if(channel2_value < 1166)
    {
    //move left
    set_speed(1,left_pwm,0,right_pwm);
    }
    else if(channel2_value > 1566)
    {
    //move right
    set_speed(0,left_pwm,1,right_pwm);
    }
    else
    {
    // move straight
    if(channel3_value > 1680 && channel3_value < 2000)
    {
    //move forward
    set_speed(1,left_pwm,1,right_pwm);
    }
    else if(channel3_value < 1280 && channel3_value > 1000)
    {
    //move back
    set_speed(0,left_pwm,0,right_pwm);
    }
    else
    {
    //stop
    set_speed(1,0,1,0);
    }
    }
    delay(5);
     */

}
