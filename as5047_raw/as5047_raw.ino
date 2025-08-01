#include <Arduino.h>
#include <SPI.h>

#define AS5047_CS PA4

void setup() {
  Serial.begin(115200);
  pinMode(AS5047_CS, OUTPUT);
  digitalWrite(AS5047_CS, HIGH); // 初始化为高电平
  SPI.begin();
}

uint16_t readAS5047() {
  digitalWrite(AS5047_CS, LOW);    // 拉低CS，启动通信
  delayMicroseconds(1);            // 等待芯片准备
  SPI.transfer16(0xFFFF);          // 发送读取角度指令
  uint16_t data = SPI.transfer16(0xFFFF); // 读取数据
  digitalWrite(AS5047_CS, HIGH);   // 拉高CS，结束通信
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
