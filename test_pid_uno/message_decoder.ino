#include "define.h"
void message_decoder(void)
{
    switch (msg_id)
    {


        //start move
        case 0x01:
            if (msg_length == 1 )
            {
                Serial.println("START MOVE");

            }
            break;
            //stop move
        case 0x02:
            if(msg_length == 1)
            {
                Serial.println("STOP MOVE");
            }
            break;
            //set speed
        case 0x03:
            //Serial.println("AAAAAAA");
            set_speed(recv_buffer[0],recv_buffer[1],recv_buffer[2],recv_buffer[3]);

             //while(Serial.read()>= 0){}
             //      Serial.println("SET SPEED");
             // Serial.println(recv_buffer[1]);
             // Serial.println(recv_buffer[3]);

            break;
            //get speed
        case 0x04:
            //Serial.println("GET SPEED");
            break;


    }
    uart_new_msg = 0;
}
