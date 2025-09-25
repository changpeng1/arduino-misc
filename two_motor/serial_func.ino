void serialEvent() {
  if(!use_simple_command)
  {
  while (Serial.available()) {
    // get the new byte:
    unsigned char data = Serial.read();
    //Serial.println(data);
    if (msg_count == 0) {
      if (data == 0xFE)  //Start Byte check
      {
        msg_count++;
      } else {
        ptr = 0;
        msg_count = 0;
        check_sum = 0;
      }
    } else if (msg_count == 1) {

      msg_id = data;
      check_sum = msg_id;
      msg_count++;
    } else if (msg_count == 2) {
      msg_length = data;
      check_sum = check_sum ^ msg_length;
      msg_count++;
    } else {
      if (msg_count > RECV_BUFFER_SIZE)  //Dump message due to error
      {
        ptr = 0;
        msg_count = 0;
        check_sum = 0;
      } else if (msg_count == msg_length + 2) {

        if (data != check_sum) {
          ptr = 0;
          msg_count = 0;
          check_sum = 0;
        } else {
          recv_buffer[ptr++] = data;
          msg_count++;
        }

      } else if (msg_count == msg_length + 3) {
        if (data == 0xFF)  //End Byte check
        {
          ptr = 0;
          msg_count = 0;
          check_sum = 0;
          uart_new_msg = 1;
        } else  //Dump message due to error
        {
          ptr = 0;
          msg_count = 0;
          check_sum = 0;
          //msg_error = 1;
        }
      } else {
        recv_buffer[ptr++] = data;
        msg_count++;
        check_sum = check_sum ^ data;
      }
    }
  }
  }
}
void calculate_enc() {
  /*
  float kk = micros()-last_time1;
    last_time1 = micros();

    double test,test1;
    //left1_speed
    {
        float aa = last_time1- last_left1_time;
        //int test;
        if(left1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_left1_time );

        //left1_speed = test*3.14*0.067*(1000.0/time_interval)/(56*11.0);//减速比56，转一圈11个脉冲，直径0.067m
        left_enc = test;
    }
    //right1_speed
    {
        float aa = last_time1- last_right1_time;
        if(right1_counter == 0 ) test = 0;
        else
            test = (kk+aa) /(temp_right1_time ); 
        //right1_speed = test*3.14*0.067*(1000.0/time_interval)/(56*11.0);//减速比56，转一圈11个脉冲，直径0.067m
        right_enc = test;
    }
    //Serial.println();
    */
  left_enc = left1_counter;
  right_enc = right1_counter;
  //left_Input1 = left_enc;
  //right_Input1 = right_enc;

  init_data();
}
void report_speed() {
  calculate_enc();
  unsigned long left_speed = left_enc;
  unsigned long right_speed = right_enc;
  unsigned char length;
  unsigned char check_sum;
  check_sum = 0;

  while (handle_request == 1) {
    delay(10);
  }

  handle_request = 1;
  unsigned char id = 0x19 | 0x80;

  Serial.write(id);
  check_sum = check_sum ^ id;
  length = 11;
  Serial.write(length);
  check_sum = check_sum ^ length;
  request_buffer[0] = left_speed >> 24;  //高8位
  request_buffer[1] = left_speed >> 16;
  request_buffer[2] = left_speed >> 8;
  request_buffer[3] = left_speed;  //低8位

  request_buffer[4] = last_left1_is_forward;



  request_buffer[5] = right_speed >> 24;
  request_buffer[6] = right_speed >> 16;
  request_buffer[7] = right_speed >> 8;
  request_buffer[8] = right_speed;

  request_buffer[9] = last_right1_is_forward;



  for (unsigned char i = 0; i < length - 1; i++) {
    Serial.write(request_buffer[i]);
    check_sum = check_sum ^ request_buffer[i];
    request_buffer[i] = '\0';
  }
  Serial.write(check_sum);

  handle_request = 0;
}
