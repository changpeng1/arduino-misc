#include "define.h"
void message_decoder(void)
{
	int volume;
	switch (msg_id)
	{
		
		
		//start move
		case 0x01:
		if (msg_length == 1 )
		{

       set_speed(1,128,1,128);
		}
		break;
    //stop move
    case 0x02:
    if(msg_length == 1)
    {
      //Serial.println("STOP MOVE");
      //stop_move(WHEEL_L);
      //stop_move(WHEEL_R);
    }
    break;
    //set speed
    case 0x03:

      //set_speed(recv_buffer[0],recv_buffer[1],recv_buffer[2],recv_buffer[3]);
      set_servo(recv_buffer[0],recv_buffer[1]);

    // while(Serial.read()>= 0){}
     //       Serial.println("SET SPEED");
    //  Serial.println(recv_buffer[0]);
    //  Serial.println(recv_buffer[1]);

    break;
    //get speed
    case 0x04:
    //Serial.println("GET SPEED");
    break;
		
		
	}
	uart_new_msg = 0;
}
