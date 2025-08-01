//发射端程序
 
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
 
int value;
 
void setup()
{
  Serial.begin(9600);
  Mirf.spi = &MirfHardwareSpi;
   Mirf.cePin = 9;
  Mirf.csnPin = 10;
  Mirf.init();
  Mirf.setRADDR((byte *)"ABCDE"); //设置自己的地址（发送端地址），使用5个字符
  Mirf.payload = sizeof(value);
  Mirf.channel = 90;              //设置所用信道
  Mirf.config();
}
 
void loop()
{

  Mirf.setTADDR((byte *)"FGHIJ");           //设置接收端地址
  value = random(255);     
    Serial.print("send1:");
  Serial.println(value);//0-255的随机数
  Mirf.send((byte *)&value);                //发送指令，发送随机数value
  while(Mirf.isSending()) delay(1);  //直到发送成功，退出循环
  Serial.println("Send Done");
  delay(1000);
}
