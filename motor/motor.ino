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
void init_data()
{
    last_left_counter = 0;
    left_counter = 0;
    last_right_counter = 0;
    right_counter = 0;
}
void report_speed(unsigned char left_speed,unsigned char right_speed)
{

    unsigned char length;
    unsigned char check_sum;
    check_sum = 0;

    while(handle_request==1)
    {
        delay(10);
    }

    handle_request=1;
    unsigned char id = 0x19 | 0x80;

    Serial.write(id);
    check_sum = check_sum ^ id;
    length =3;
    Serial.write(length);
    check_sum = check_sum ^ length;
    request_buffer[0]= left_speed;
    request_buffer[1]= right_speed;
    for(unsigned char i=0;i<length-1;i++)
    {
        Serial.write(request_buffer[i]);
        check_sum =check_sum ^ request_buffer[i];
        request_buffer[i]='\0';
    }
    Serial.write(check_sum);

    handle_request = 0;
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
void set_speed( unsigned char left1_is_forward,unsigned char l1_speed,unsigned char left2_is_forward,unsigned char l2_speed,unsigned char right1_is_forward,unsigned char r1_speed,unsigned char right2_is_forward,unsigned char r2_speed)
{
    if(l1_speed < 0 ||l1_speed > 255 || r1_speed < 0 || r1_speed > 255 ) return;
    if(l2_speed < 0 ||l2_speed > 255 || r2_speed < 0 || r2_speed > 255 ) return;
    init_data();
    if(left1_is_forward == 0)//反转
    {
        if(last_left1_is_forward!=0)
        {


            digitalWrite(24,HIGH);
            digitalWrite(25,LOW);
        }
        last_left1_is_forward = 0;

        analogWrite(5, l1_speed);
    }
    else 
    {
        if(last_left1_is_forward!=1)
        {

            digitalWrite(25,HIGH);
            digitalWrite(24,LOW);
        }
        last_left1_is_forward = 1;

        analogWrite(5, l1_speed);
    }
    if(left2_is_forward == 0)//反转
    {
        if(last_left2_is_forward!=0)
        {

            digitalWrite(22,HIGH);
            digitalWrite(23,LOW);

        }
        last_left2_is_forward = 0;

        analogWrite(3, l2_speed);
    }
    else 
    {
        if(last_left2_is_forward!=1)
        {

            digitalWrite(23,HIGH);
            digitalWrite(22,LOW);
        }
        last_left2_is_forward = 1;

        analogWrite(3, l2_speed);
    }
    if(right1_is_forward ==0)
    {
        if(last_right1_is_forward!=0)
        {


            digitalWrite(32,HIGH);
            digitalWrite(33,LOW);
        }
        last_right1_is_forward = 0;
        analogWrite(8, r1_speed);
    }
    else
    {
        if(last_right1_is_forward!=1)
        {

            digitalWrite(33,HIGH);
            digitalWrite(32,LOW);
        }
        last_right1_is_forward = 1;
        analogWrite(8, r1_speed);
    }
    if(right2_is_forward ==0)
    {
        if(last_right2_is_forward!=0)
        {
            digitalWrite(30,HIGH);
            digitalWrite(31,LOW);

        }
        last_right2_is_forward = 0;
        analogWrite(9, r2_speed);
    }
    else
    {
        if(last_right2_is_forward!=1)
        {
            digitalWrite(31,HIGH);
            digitalWrite(30,LOW);

        }
        last_right2_is_forward = 1;
        analogWrite(9, r2_speed);
    }

}
void start_move(int wheel,bool is_forward)
{
    switch(wheel)
    {
        case WHEEL_L:
            if(is_forward)
            {
                digitalWrite(24,HIGH);
                digitalWrite(25,LOW);
            }
            else
            {
                digitalWrite(25,HIGH);
                digitalWrite(24,LOW);

            }
            break;
        case WHEEL_R:
            if(is_forward)
            {
                digitalWrite(22,HIGH);
                digitalWrite(23,LOW);
            }
            else
            {
                digitalWrite(23,HIGH);
                digitalWrite(22,LOW);

            }
            break;
        default:
            break;
    }
}
void stop_move(int wheel)
{
    switch(wheel)
    {
        case WHEEL_L:
            digitalWrite(24,LOW);
            digitalWrite(25,LOW);
            break;
        case WHEEL_R:
            digitalWrite(22,LOW);
            digitalWrite(23,LOW);
            break;
        default:
            break;
    }

}
float temp_left_time,temp_right_time;
float last_left_time,last_right_time;
float last_time1;
void counter_timeout()//定时器中断处理函数
{
    float kk = micros()-last_time1;
    last_time1 = micros();
    float aa;
    double left_speed;
    double right_speed;
    double test,test1;
    {
        aa = last_time1- last_left_time;
        //int test;
        if(right_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_left_time );

        //if(test-40 > 2 &&test-40<20) left_pwm = left_pwm-(test-40)*0.4;
        //else if(40-test>2 && 40-test < 20) left_pwm = left_pwm +(40-test)*0.4; 
        //left_speed = test*3.14*0.07*(1000/timer_interval)/20;
        Serial.print("left_speed:");
        Serial.print(left_counter);
        Serial.print("right_speed:");
        Serial.print(right_counter);
        //下面是另一种测速方法，temp_left_time是两次脉冲的间隔
        /*
           double left_speed1 = 1000000*3.14*0.07/(temp_left_time *20);
           Serial.print("temp_speed:");
           Serial.println(left_speed1);
         */
    }

    Serial.println();

    //init_data();





}

/*
   void serialEvent() {
   while (Serial.available()) {
// get the new byte:
unsigned char data = Serial.read();
if(msg_count == 0)
{
if (data == 0xFE)	//Start Byte check
{
msg_count++;
}else
{
ptr = 0;
msg_count = 0;
check_sum = 0;
}
}else if(msg_count == 1)
{

msg_id = data;
check_sum = msg_id;
msg_count++;
}else if (msg_count == 2)
{
msg_length = data;
check_sum = check_sum^msg_length;
msg_count++;
}else
{	
if(msg_count > RECV_BUFFER_SIZE)	//Dump message due to error
{
ptr = 0;
msg_count = 0;
check_sum = 0;
}else if(msg_count == msg_length+2)
{

if(data != check_sum)
{
ptr = 0;
msg_count = 0;
check_sum = 0;
}
else
{
recv_buffer[ptr++]=data;
msg_count++;			
}

}
else if(msg_count == msg_length+3)
{
if(data == 0xFF)	//End Byte check
{
ptr = 0;
msg_count = 0;
check_sum = 0;
uart_new_msg = 1;
}
else    //Dump message due to error
{
ptr = 0;
msg_count = 0;
check_sum = 0;
//msg_error = 1;
}
}else
{
recv_buffer[ptr++]=data;
msg_count++;
check_sum = check_sum^data;
}
}
}
}
*/
const int TrigPin = A0;
const int EchoPin = 8;
float cm;
bool up_pressed = false;
bool down_pressed = false;
bool left_pressed = false;
bool right_pressed = false;


void onChange()
{
    //Serial.println("Found Controller, configured successful ");
    right_counter+=1;
    temp_right_time = micros()-last_right_time;
    last_right_time = micros();
}

void onChange1()
{
    //Serial.println("Found Controller, configured successful ");
    left_counter+=1;
    temp_left_time = micros()-last_left_time;
    last_left_time = micros();
}
void setup() {
    // put your setup code here, to run once:
    int tickEvent = t.every(1000, counter_timeout);
    Serial.begin(9600);
    while(Serial.read()>= 0){}
    init_gpio();
    // init_rf_receiver();

    //init radar speed mode
    pinMode(TrigPin,OUTPUT);//Trig
    pinMode(EchoPin,INPUT);//Echo
    attachInterrupt( 0, onChange, CHANGE);
    attachInterrupt( 5, onChange1, CHANGE);

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


void loop() {
    // put your main code here, to run repeatedly:

    t.update();
    /* 
       int left_value = digitalRead(12);
       if(left_value!=last_left_state)
       {
       left_counter+=1;
       }
       last_left_state = left_value;
       int right_value = digitalRead(13);
       if(right_value!=last_right_state)
       {
       right_counter+=1;
       }
       last_right_state = right_value;

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
     */
    /*
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
