#include "define.h"
int FirstShotX , FirstShotY;
bool check_valid(int value)
{
    bool ret = false;
    if(value> valid_value || value < -valid_value)
    {
        return true;
    }
    return ret;
}
void setup() {
    // put your setup code here, to run once:

    for(int i=0; i<19; i++)
    {
        pinMode(i, INPUT);
        digitalWrite(i, 1);
    }

    Serial.begin(9600);
    FirstShotX = 0;
    FirstShotY = 0;
    init_rf_sender();
}

void loop() {
    // put your main code here, to run repeatedly:
    int i, someInt, flag = 0;
    someInt = 0;
    for(i=2; i<9; i++)
    {
        someInt = digitalRead(i);
        if(someInt == 0)
        {
            flag =1;
            break;
        }
    }
    if(flag == 1)
    {
        switch(i)
        {

            case 2: Serial.println("--------> Button A"); break;
            case 3: Serial.println("--------> Button B");send_rf_command(6); break;
            case 4: Serial.println("--------> Button C"); break;
            case 5: Serial.println("--------> Button D"); send_rf_command(5);break;
            case 6: Serial.println("--------> Button E"); break;
            case 7: Serial.println("--------> Button F"); break;
            case 8: Serial.println("--------> Button KEY"); break;
            default: break;
        }
        flag=0;
    }
    else
    {
    X_Value = Y_Value = 0;
    int sensorValue = analogRead(A0);
    if(FirstShotX == 0)
    {
        FirstShotX = sensorValue;
        Serial.print("FirstShotX = ");
        Serial.println(FirstShotX);
    }
    X_Value = sensorValue - FirstShotX;
    if(check_valid(X_Value))
    {
      is_valid_value = 1;
        Serial.print("X = ");
        Serial.println(X_Value);
    }
    sensorValue = analogRead(A1);
    if(FirstShotY == 0)
    {
        FirstShotY = sensorValue;
        Serial.print("FirstShotY = ");
        Serial.println(FirstShotY);
    }
    Y_Value = sensorValue - FirstShotY;
    if(check_valid(Y_Value))
    {
      is_valid_value = 1;
        Serial.print("Y = ");
        Serial.println(Y_Value);
    }
    if(is_valid_value == 1)
    {
      is_valid_value = 0;
      if(check_valid(X_Value))
      {
      if(X_Value>0)//left
        send_rf_command(3);
        else if(X_Value <0)//right
        send_rf_command(4);
      }
      else if(check_valid(Y_Value))
      {
      if(Y_Value>0)//up
      {
        send_rf_command(1);
        }
        else if(Y_Value <0)//down
        {
          send_rf_command(2);
          }
      }
      
      }
      else
      {
        send_rf_command(0);
        }
    }
    delay(200);
}
