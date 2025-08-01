static int last_left_is_forward = -1;
static int last_right_is_forward = -1;

void set_servo(unsigned char index,unsigned char degree)
{
  //0---180,中间位置为90
  switch(index)
  {
    case LEFT_HIP:
    {
//      if(left_hip_value > degree)
//      {
//        while(left_hip_value -=5 > degree);
//          left_hip_servo.write(left_hip_value);
//      }
//      if(left_hip_value < degree)
//      {
//        while(left_hip_value +=5 < degree);
//          left_hip_servo.write(left_hip_value);
//        }
//      left_hip_value = degree;
      left_hip_servo.write(degree);
    }
    break;
    case LEFT_KNEE:left_knee_servo.write(degree);break;
    case LEFT_FEET:left_feet_servo.write(degree);break;
    case RIGHT_HIP:right_hip_servo.write(degree);break;
    case RIGHT_KNEE:right_knee_servo.write(degree);break;
    case RIGHT_FEET:right_feet_servo.write(degree);break;
    default:break;
    }
  }
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
