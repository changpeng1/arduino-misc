#include <SPI.h>
#include <Arduino.h>

// 硬件SPI引脚配置 (使用默认HSPI引脚)
#define SPI_MISO 13
#define SPI_MOSI 11
#define SPI_SCLK 12
#define SPI_CS   10   // 片选引脚



// SPI设置
const uint32_t SPI_FREQ = 10000000;  // 10MHz SPI时钟
const uint8_t SPI_MODE = SPI_MODE1;  // CPOL=0, CPHA=1

void setup() {
  Serial.begin(115200);
  while (!Serial);  // 等待串口连接
  
  Serial.println("===============================\n");
  
  // 初始化SPI
  SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS);
  SPI.setFrequency(SPI_FREQ);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE);
  
  // 配置片选引脚
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);  // 初始取消选中

}
uint8_t i = 0;
void loop() {
  static uint32_t lastUpdate = 0;
  
  // 每10ms发送一次
  if (millis() - lastUpdate >= 10) {
    lastUpdate = millis();
    

    digitalWrite(SPI_CS, LOW); // 选中设备
  // 发送命令并接收响应
  
  uint8_t response = SPI.transfer(i++);
  
  digitalWrite(SPI_CS, HIGH); // 取消选中
   Serial.printf("response data is:%d",response);
   Serial.println();
    
  }
}


