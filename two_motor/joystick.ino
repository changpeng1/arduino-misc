void process_joystick_command() {
  long channel2_value, channel3_value, channel4_value;
  channel2_value = pulseIn(CHANNEL2, 1);  //1180-1440-1680 down-middle-forward
  //channel3_value = pulseIn(CHANNEL3,1);//870-1860
  channel4_value = pulseIn(CHANNEL4, 1);  //1280-1540-1780  left-middle-right
  if (channel2_value - 1440 < -100)       //back
  {
    process_rf_command(2);
    down_pressed = true;
  }
  if (channel2_value - 1480 > 100)  //forward
  {
    process_rf_command(1);
    up_pressed = true;
  }
  if (channel4_value - 1420 < -100)  //left
  {
    process_rf_command(3);
    left_pressed = true;
  }
  if (channel4_value - 1420 > 200)  //right
  {
    process_rf_command(4);
    right_pressed = true;
  }
  if (!up_pressed && !down_pressed && !left_pressed && !right_pressed)
    process_rf_command(0);
  up_pressed = false;
  down_pressed = false;
  left_pressed = false;
  right_pressed = false;
  /*
           Serial.print("CHANNEL2=");
           Serial.print(channel2_value);

           Serial.print("      CHANNEL4=");
           Serial.println(channel4_value);
         */
}