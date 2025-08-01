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
            set_speed(recv_buffer[0],recv_buffer[1],recv_buffer[2],recv_buffer[3]);
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
