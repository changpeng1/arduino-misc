#include "define.h"
void message_decoder(void)
{
    switch (msg_id)
    {
        //start move
        case MESSAGE_TYPE_START_MOVE:
            if (msg_length == 1 )
            {
            }
            break;
            //stop move
        case MESSAGE_TYPE_STOP_MOVE:
            if(msg_length == 1)
            {
            }
            break;
            //set speed
        case MESSAGE_TYPE_SET_SPEED:
            // set_speed(recv_buffer[0],recv_buffer[1],recv_buffer[2],recv_buffer[3]);
            
            if(direct_float_value)
            {
                float value,value1;
                byte *p = (byte*) &value;
                p[0] = recv_buffer[1];
                p[1] = recv_buffer[2];
                p[2] = recv_buffer[3];
                p[3] = recv_buffer[4];
                byte *q = (byte*) &value1;
                q[0] = recv_buffer[6];
                q[1] = recv_buffer[7];
                q[2] = recv_buffer[8];
                q[3] = recv_buffer[9];
                int left_rpm = value / (3.1415 * wheel_diameter) * radio * 11 / 20.0;
                int right_rpm = value1 / (3.1415 * wheel_diameter) * radio * 11 / 20.0;

                set_speed(recv_buffer[0],left_rpm,recv_buffer[5],right_rpm);
            }
            else
                set_speed(recv_buffer[0],recv_buffer[4],recv_buffer[5],recv_buffer[9]);
            break;
            //get speed
        case MESSAGE_TYPE_GET_SPEED:
            report_speed();
            break;
        default:
            break;

    }
    uart_new_msg = 0;
}
