#include "define.h"

void stop_move()
{
    digitalWrite(LEFT_IN2,HIGH);
    digitalWrite(LEFT_IN1,LOW);
    //analogWrite(6, 100);
    digitalWrite(RIGHT_IN1,HIGH);
    digitalWrite(RIGHT_IN2,LOW);
    //analogWrite(9, 100);
    delay(100);
    analogWrite(LEFT_PWM, 0);
    analogWrite(RIGHT_PWM, 0);
}
void set_speed( unsigned char left1_is_forward,unsigned char l1_speed,unsigned char right1_is_forward,unsigned char r1_speed)
{
    if(left1_is_forward == 0)//反转
    {
        if(last_left1_is_forward!=0)
        {
            digitalWrite(LEFT_IN2,HIGH);
            digitalWrite(LEFT_IN1,LOW);
        }
        last_left1_is_forward = 0;

        left_Setpoint1 = l1_speed;
    }
    else 
    {
        if(last_left1_is_forward!=1)
        {

            digitalWrite(LEFT_IN1,HIGH);
            digitalWrite(LEFT_IN2,LOW);
        }
        last_left1_is_forward = 1;

        left_Setpoint1 = l1_speed;
    }
    
    if(right1_is_forward ==0)
    {
        if(last_right1_is_forward!=0)
        {
            digitalWrite(RIGHT_IN1,HIGH);
            digitalWrite(RIGHT_IN2,LOW);
        }
        last_right1_is_forward = 0;
        right_Setpoint1 = r1_speed;
    }
    else
    {
        if(last_right1_is_forward!=1)
        {
            digitalWrite(RIGHT_IN2,HIGH);
            digitalWrite(RIGHT_IN1,LOW);
        }
        last_right1_is_forward = 1;
        right_Setpoint1 = r1_speed;
    }
    

}
void set_pwm( unsigned char l1_speed,unsigned char r1_speed)
{
    if(l1_speed < 0 ||l1_speed > 255 || r1_speed < 0 || r1_speed > 255 ) return;
    analogWrite(LEFT_PWM, l1_speed);
    analogWrite(RIGHT_PWM, r1_speed);

}
