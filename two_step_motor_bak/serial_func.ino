
void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        unsigned char data = Serial.read();
        //Serial.println(data);
        if(msg_count == 0)
        {
            if (data == 0xFE)  //Start Byte check
            {
                msg_count++;
            }else
            {
                ptr = 0;
                msg_count = 0;
                check_sum = 0;
            }
        }else if(msg_count == 1)
        {

            msg_id = data;
            check_sum = msg_id;
            msg_count++;
        }else if (msg_count == 2)
        {
            msg_length = data;
            check_sum = check_sum^msg_length;
            msg_count++;
        }else
        { 
            if(msg_count > RECV_BUFFER_SIZE)  //Dump message due to error
            {
                ptr = 0;
                msg_count = 0;
                check_sum = 0;
            }else if(msg_count == msg_length+2)
            {

                if(data != check_sum)
                {
                    ptr = 0;
                    msg_count = 0;
                    check_sum = 0;
                }
                else
                {
                    recv_buffer[ptr++]=data;
                    msg_count++;      
                }

            }
            else if(msg_count == msg_length+3)
            {
                if(data == 0xFF)  //End Byte check
                {
                    ptr = 0;
                    msg_count = 0;
                    check_sum = 0;
                    uart_new_msg = 1;
                }
                else    //Dump message due to error
                {
                    ptr = 0;
                    msg_count = 0;
                    check_sum = 0;
                    //msg_error = 1;
                }
            }else
            {
                recv_buffer[ptr++]=data;
                msg_count++;
                check_sum = check_sum^data;
            }
        }
    }
}

void report_speed(unsigned int left1_speed,unsigned int left2_speed,unsigned int right1_speed,unsigned int right2_speed)
{

    unsigned char length;
    unsigned char check_sum;
    check_sum = 0;

    while(handle_request==1)
    {
        delay(10);
    }

    handle_request=1;
    unsigned char id = 0x19 | 0x80;

    Serial.write(id);
    check_sum = check_sum ^ id;
    length =11;
    Serial.write(length);
    check_sum = check_sum ^ length;
    request_buffer[0]= left1_speed>>24;//高8位
    request_buffer[1]= left1_speed>>16;
    request_buffer[2]= left1_speed>>8;
    request_buffer[3]= left1_speed;//低8位

    request_buffer[4]= last_left1_is_forward;



    request_buffer[5]= right1_speed>>24;
    request_buffer[6]= right1_speed>>16;
    request_buffer[7]= right1_speed>>8;
    request_buffer[8]= right1_speed;

    request_buffer[9]= last_right1_is_forward;



    for(unsigned char i=0;i<length-1;i++)
    {
        Serial.write(request_buffer[i]);
        check_sum =check_sum ^ request_buffer[i];
        request_buffer[i]='\0';
    }
    Serial.write(check_sum);

    handle_request = 0;
}
