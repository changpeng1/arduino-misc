#include <Arduino.h>
#include <SPI.h>

#define AS5047_CS 10

void setup() {
  Serial.begin(115200);
  pinMode(AS5047_CS, OUTPUT);
  
  SPI.begin();
  digitalWrite(AS5047_CS, HIGH); // 初始化为高电平
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
}
//能否读取到的关键是要在每次transfer之前和之后都拉低拉高cs
uint16_t readAS5047() {
  // SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  digitalWrite(AS5047_CS, LOW);    // 拉低CS，启动通信
  
  SPI.transfer16(0x3FFF);          // 发送读取角度指令
  digitalWrite(AS5047_CS, HIGH);   // 拉高CS，结束通信
  delayMicroseconds(1);            // 等待芯片准备
  digitalWrite(AS5047_CS, LOW);    // 拉低CS，启动通信
  uint16_t data = SPI.transfer16(0xFFFF); // 读取数据
  digitalWrite(AS5047_CS, HIGH);   // 拉高CS，结束通信
  // SPI.endTransaction();
  return data & 0x3FFF;            // 提取14位角度值
  
}

void loop() {
  uint16_t angle = readAS5047();
  float degrees = angle * 360.0 / 16384.0; // 转换为度数
  Serial.print("Angle: ");
  Serial.print(degrees);
  Serial.println("°");
  delay(100);
}
