static int last_left_is_forward = -1;
static int last_right_is_forward = -1;
void set_speed( unsigned char left_is_forward,unsigned char l_speed,unsigned char right_is_forward,unsigned char r_speed)
{
    if(l_speed < 0 ||l_speed > 255 || r_speed < 0 || r_speed > 255 ) return;
    //init_data();
    if(left_is_forward == 0)//反转
    {
        if(last_left_is_forward!=0)
        {

            digitalWrite(5,HIGH);
            digitalWrite(4,LOW);
        }
        last_left_is_forward = 0;

        analogWrite(6, l_speed);
    }
    else 
    {
        if(last_left_is_forward!=1)
        {

            digitalWrite(4,HIGH);
            digitalWrite(5,LOW);
        }
        last_left_is_forward = 1;

        analogWrite(6, l_speed);
    }
    if(right_is_forward ==0)
    {
        if(last_right_is_forward!=0)
        {
            digitalWrite(7,HIGH);
            digitalWrite(8,LOW);
        }
        last_right_is_forward = 0;
        analogWrite(9, r_speed);
    }
    else
    {
        if(last_right_is_forward!=1)
        {
            digitalWrite(8,HIGH);
            digitalWrite(7,LOW);
        }
        last_right_is_forward = 1;
        analogWrite(9, r_speed);
    }

}

void stop_move()
{

            digitalWrite(4,LOW);
            digitalWrite(5,LOW);

            digitalWrite(8,LOW);
            digitalWrite(7,LOW);


}
