#include "define.h"
/*
void set_speed( unsigned char left1_is_forward,unsigned char l1_speed,unsigned char left2_is_forward,unsigned char l2_speed,unsigned char right1_is_forward,unsigned char r1_speed,unsigned char right2_is_forward,unsigned char r2_speed)
{
    if(l1_speed < 0 ||l1_speed > 255 || r1_speed < 0 || r1_speed > 255 ) return;
    if(l2_speed < 0 ||l2_speed > 255 || r2_speed < 0 || r2_speed > 255 ) return;
    //init_data();
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
*/
void start_move(int wheel,bool is_forward)
{
  return;
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
  return;
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
void set_speed( unsigned char left1_is_forward,unsigned char l1_speed,unsigned char left2_is_forward,unsigned char l2_speed,unsigned char right1_is_forward,unsigned char r1_speed,unsigned char right2_is_forward,unsigned char r2_speed)
{
    //if(l1_speed < 0 ||l1_speed > 255 || r1_speed < 0 || r1_speed > 255 ) return;
    //if(l2_speed < 0 ||l2_speed > 255 || r2_speed < 0 || r2_speed > 255 ) return;
    //init_data();
    if(left1_is_forward == 0)//反转
    {
        if(last_left1_is_forward!=0)
        {
            digitalWrite(12,LOW);
        }
        last_left1_is_forward = 0;

        //analogWrite(5, l1_speed);
        left_Setpoint1 = l1_speed;
        //if(l1_speed == 0) left_Input1 = 0;
    }
    else 
    {
        if(last_left1_is_forward!=1)
        {
            digitalWrite(12,HIGH);
        }
        last_left1_is_forward = 1;

        //analogWrite(5, l1_speed);
        left_Setpoint1 = l1_speed;
    }
    if(left2_is_forward == 0)//反转
    {
        if(last_left2_is_forward!=0)
        {

            //digitalWrite(22,HIGH);
            //digitalWrite(23,LOW);

        }
        last_left2_is_forward = 0;

        //analogWrite(3, l2_speed);
        left_Setpoint2 = l2_speed;
        
    }
    else 
    {
        if(last_left2_is_forward!=1)
        {

           // digitalWrite(23,HIGH);
           // digitalWrite(22,LOW);
        }
        last_left2_is_forward = 1;

        //analogWrite(3, l2_speed);
        left_Setpoint2 = l2_speed;
    }
    if(right1_is_forward ==0)
    {
        if(last_right1_is_forward!=0)
        {


            digitalWrite(13,HIGH);
        }
        last_right1_is_forward = 0;
        //analogWrite(8, r1_speed);
        right_Setpoint1 = r1_speed;
    }
    else
    {
        if(last_right1_is_forward!=1)
        {

            //digitalWrite(33,HIGH);
            digitalWrite(13,LOW);
        }
        last_right1_is_forward = 1;
        //analogWrite(8, r1_speed);
        right_Setpoint1 = r1_speed;
    }
    if(right2_is_forward ==0)
    {
        if(last_right2_is_forward!=0)
        {
            //digitalWrite(30,HIGH);
            //digitalWrite(31,LOW);

        }
        last_right2_is_forward = 0;
        //analogWrite(9, r2_speed);
        right_Setpoint2 = r2_speed;
    }
    else
    {
        if(last_right2_is_forward!=1)
        {
            //digitalWrite(31,HIGH);
            //digitalWrite(30,LOW);

        }
        last_right2_is_forward = 1;
        //analogWrite(9, r2_speed);
        right_Setpoint2 = r2_speed;
    }

}
void set_pwm( unsigned char l1_speed,unsigned char l2_speed,unsigned char r1_speed,unsigned char r2_speed)
{
 // 
    if(l1_speed < 0 ||l1_speed > 255 || r1_speed < 0 || r1_speed > 255 ) return;
    //if(l2_speed < 0 ||l2_speed > 255 || r2_speed < 0 || r2_speed > 255 ) return;
    //if(l1_speed!=0)
    //  Serial.println(l1_speed);
    //analogWrite(11, l1_speed);
    pwmWrite(10,l1_speed);
    //analogWrite(3, l2_speed);
    pwmWrite(9, r1_speed);
    //analogWrite(9, r2_speed);
    //Serial.println(l1_speed);
    
    
}
