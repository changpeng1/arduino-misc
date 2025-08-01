#include "define.h"

void stop_move()
{
    digitalWrite(5,HIGH);
    digitalWrite(4,LOW);
    //analogWrite(6, 100);
    digitalWrite(7,HIGH);
    digitalWrite(8,LOW);
    //analogWrite(9, 100);
    delay(100);
    analogWrite(6, 0);
    analogWrite(9, 0);
}
void set_speed( unsigned char left1_is_forward,unsigned char l1_speed,unsigned char right1_is_forward,unsigned char r1_speed)
{
    if(left1_is_forward == 0)//反转
    {
        if(last_left1_is_forward!=0)
        {
            digitalWrite(5,HIGH);
            digitalWrite(4,LOW);
        }
        last_left1_is_forward = 0;

        left_Setpoint1 = l1_speed;
    }
    else 
    {
        if(last_left1_is_forward!=1)
        {

            digitalWrite(4,HIGH);
            digitalWrite(5,LOW);
        }
        last_left1_is_forward = 1;

        left_Setpoint1 = l1_speed;
    }
    
    if(right1_is_forward ==0)
    {
        if(last_right1_is_forward!=0)
        {


            digitalWrite(7,HIGH);
            digitalWrite(8,LOW);
        }
        last_right1_is_forward = 0;
        right_Setpoint1 = r1_speed;
    }
    else
    {
        if(last_right1_is_forward!=1)
        {

            digitalWrite(8,HIGH);
            digitalWrite(7,LOW);
        }
        last_right1_is_forward = 1;
        right_Setpoint1 = r1_speed;
    }
    

}
void set_pwm( unsigned char l1_speed,unsigned char r1_speed)
{
    if(l1_speed < 0 ||l1_speed > 255 || r1_speed < 0 || r1_speed > 255 ) return;
    analogWrite(6, l1_speed);
    analogWrite(9, r1_speed);

}
