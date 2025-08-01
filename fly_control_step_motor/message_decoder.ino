#include "define.h"
void message_decoder(void)
{
  //Serial.println(msg_id);
  //Serial.println("GET SPEED");
  if(msg_id == 4)
    report_speed();
    switch (msg_id)
    {
        //start move
        case 0x01:
            if (msg_length == 1 )
            {
                //Serial.println("START MOVE");
                //start_move(WHEEL_L,true);

                //start_move(WHEEL_R,true);
                //set_speed(1,128,1,128);
            }
            break;
            //stop move
        case 0x02:
            if(msg_length == 1)
            {
               // Serial.println("STOP MOVE");
                //stop_move(WHEEL_L);
                //stop_move(WHEEL_R);
            }
            break;
            //set speed
        case 3:
            int left_speed = recv_buffer[1]<<24|recv_buffer[2]<<16|recv_buffer[3]<<8|recv_buffer[4];
            int right_speed = recv_buffer[6]<<24|recv_buffer[7]<<16|recv_buffer[8]<<8|recv_buffer[9];
            set_speed(recv_buffer[0],left_speed,recv_buffer[5],right_speed);

            break;
            //get speed
        case 4:
            //Serial.println("GET SPEED1");
            report_speed();
            break;
        defualt:
        break;

    }
    uart_new_msg = 0;
}
