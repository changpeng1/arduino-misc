#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "define.h"
void init_rf_receiver()
{
    //Serial.begin(9600);
    Mirf.spi = &MirfHardwareSpi;
    Mirf.cePin = 48;
    Mirf.csnPin = 53;
    Mirf.init();

    Mirf.setRADDR((byte *)"FGHIJ"); //设置自己的地址（接收端地址），使用5个字符
    Mirf.payload = sizeof(rf_value);   
    Mirf.channel = 90;   //设置使用的信道
    Mirf.config(); 
    //Serial.println("Listening...");  //开始监听接收到的数据
}


void process_rf_command(int value)
{
    //Serial.println(value);
    if(value == 0)
    {
        set_speed(1,0,1,0); 
        //stop_move(); 
    }
    else if(value == 1)// 前进
    {
        //set_speed(1,70,1,64);
        set_speed(1,50,1,50);
    }
    else if(value == 2)//back
    {
        set_speed(0,50,0,50);
    }
    else if(value == 3)//left
    {
        //set_speed(1,155,1,155,0,155,0,155);
        set_speed(0,50,1,50);
    }
    else if(value == 4)//right
    {
        //set_speed(0,155,0,155,1,155,1,155);
        set_speed(1,50,0,50);
    }

    else if(value == 5) //left straight
    {
        set_speed(0,80,1,80);
    }
    else if(value == 6) //right straight
    {
        set_speed(1,80,0,80);
    }
}
