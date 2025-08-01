#include "define.h"

void start_move(int wheel,bool is_forward)
{
    return;
}
void stop_move(int wheel)
{
    return;

}
void init_l298()
{
    digitalWrite(5,HIGH);
    digitalWrite(4,LOW);
    digitalWrite(6,HIGH);

}
void set_speed( unsigned char left1_is_forward,unsigned int l1_speed,unsigned char right1_is_forward,unsigned int r1_speed)
{

    if(left1_is_forward == 0)//反转
    {
        if(last_left1_is_forward!=0)
        {
            digitalWrite(3,LOW);
        }
        last_left1_is_forward = 0;


    }
    else 
    {
        if(last_left1_is_forward!=1)
        {

            digitalWrite(3,HIGH);
        }
        last_left1_is_forward = 1;
    }


    if(right1_is_forward ==0)
    {
        if(last_right1_is_forward!=0)
        {
            digitalWrite(7,LOW);
        }
        last_right1_is_forward = 0;
    }
    else
    {
        if(last_right1_is_forward!=1)
        {
            digitalWrite(7,HIGH);
        }
        last_right1_is_forward = 1;
    }
    left_duration = 1000000/2/l1_speed;//需要除以2，因为高低电平各占一半，这里计算出的值是高电平或低电平维持的时间，l1_speed的单位是步/秒
    right_duration = 1000000/2/r1_speed;
    //left_duration = l1_speed;
    //right_duration = r1_speed;
    if(l1_speed == 0) {
        should_left_stop = true;
        digitalWrite(4,HIGH);
    }
    else 
    {
        should_left_stop = false;
        digitalWrite(4,LOW);
    }

    if(r1_speed == 0) {
        should_right_stop = true;
        digitalWrite(8,HIGH);
    }
    else {
        should_right_stop = false;
        digitalWrite(8,LOW);
    }
}
void set_pwm( unsigned char l1_speed,unsigned char l2_speed,unsigned char r1_speed,unsigned char r2_speed)
{
    // 
    if(l1_speed < 0 ||l1_speed > 255 || r1_speed < 0 || r1_speed > 255 ) return;
    // if(l2_speed < 0 ||l2_speed > 255 || r2_speed < 0 || r2_speed > 255 ) return;
    analogWrite(6, l1_speed);
    //analogWrite(3, l2_speed);
    analogWrite(9, r1_speed);
    //analogWrite(9, r2_speed);
    //Serial.println(l1_speed);


}
