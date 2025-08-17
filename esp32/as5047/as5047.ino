#include <SPI.h>
#include <Arduino.h>

// 硬件SPI引脚配置 (使用默认HSPI引脚)
#define SPI_MISO 13
#define SPI_MOSI 11
#define SPI_SCLK 12
#define SPI_CS   10   // 片选引脚

// AS5047P寄存器地址
#define AS5047_ANGLE_REG    0x3FFF  // 角度读取寄存器
#define AS5047_ERROR_REG    0x0001  // 错误状态寄存器
#define AS5047_SETTINGS_REG 0x0018  // 芯片设置寄存器

// 错误标志位
#define AS5047_ERROR_PARITY    (1 << 0)  // 奇偶校验错误
#define AS5047_ERROR_COMMAND   (1 << 1)  // 无效命令
#define AS5047_ERROR_FRAMING   (1 << 2)  // 帧错误
#define AS5047_ERROR_MAGNET    (1 << 3)  // 磁铁问题

// AS5047P SPI设置
const uint32_t SPI_FREQ = 10000000;  // 10MHz SPI时钟
const uint8_t SPI_MODE = SPI_MODE1;  // CPOL=0, CPHA=1

void setup() {
  Serial.begin(115200);
  while (!Serial);  // 等待串口连接
  
  Serial.println("\nAS5047P Magnetic Encoder Reader");
  Serial.println("===============================\n");
  
  // 初始化SPI
  SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS);
  SPI.setFrequency(SPI_FREQ);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE);
  
  // 配置片选引脚
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);  // 初始取消选中

  /*
  // 检测AS5047P是否存在
  if (checkAS5047Presence()) {
    Serial.println("AS5047P detected successfully!");
    
    // 读取并打印芯片信息
    uint16_t settings = readRegister(AS5047_SETTINGS_REG);
    Serial.printf("Chip Settings: 0x%04X\n", settings);
    Serial.printf("  DAEC: %s\n", (settings & 0x0100) ? "Enabled" : "Disabled");
    Serial.printf("  ABIB: %s\n", (settings & 0x0200) ? "Enabled" : "Disabled");
    Serial.printf("  DIR:  %s\n", (settings & 0x0400) ? "CCW" : "CW");
    Serial.printf("  UVW:  %s\n", (settings & 0x0800) ? "Enabled" : "Disabled");
    Serial.printf("  PWM:  %s\n", (settings & 0x1000) ? "Enabled" : "Disabled");
    Serial.println();
  } else {
    Serial.println("Error: AS5047P not detected! Check connections.");
    while(1);  // 停止执行
  }
  */
}

void loop() {
  static uint32_t lastUpdate = 0;
  
  // 每50ms读取一次角度
  if (millis() - lastUpdate >= 50) {
    lastUpdate = millis();
    
    // 读取角度值
    uint16_t rawAngle = readAngle();
    
    {
      // 转换为角度值 (0-360度)
      float angle = (rawAngle & 0x3FFF) * 360.0 / 16384.0;
      
      // 读取磁铁状态
      uint16_t errorStatus = readRegister(AS5047_ERROR_REG);
      
      // 打印结果
      Serial.printf("Angle: %6.2f° | Raw: 0x%04X | ", angle, rawAngle);
      Serial.printf("Magnet: %s | ", (errorStatus & AS5047_ERROR_MAGNET) ? "Weak/None" : "OK");
      Serial.printf("Errors: %s%s%s\n", 
                   (errorStatus & AS5047_ERROR_PARITY) ? "Parity " : "",
                   (errorStatus & AS5047_ERROR_COMMAND) ? "Command " : "",
                   (errorStatus & AS5047_ERROR_FRAMING) ? "Framing " : "");
    }
  }
}

// 读取AS5047P角度值
uint16_t readAngle() {
  return readRegister(AS5047_ANGLE_REG);
}

// 读取AS5047P寄存器
uint16_t readRegister(uint16_t regAddress) {
  // AS5047P命令格式：最高位=1(读), 14位地址, 最低位=奇偶校验
  uint16_t command = 0x4000 | (regAddress & 0x3FFF); // 设置读位
  command |= calculateParity(command) ? 0 : 1; // 添加偶校验位
  
  digitalWrite(SPI_CS, LOW); // 选中设备
  
  // 发送命令并接收响应
  SPI.transfer16(command);
  
  digitalWrite(SPI_CS, HIGH); // 取消选中
  
    digitalWrite(SPI_CS, LOW); // 选中设备
  
  // 发送命令并接收响应
  uint16_t response = SPI.transfer16(0xFFFF);
  
  digitalWrite(SPI_CS, HIGH); // 取消选中
  // 返回的数据是16位，包含14位数据+2位状态
  return response;
}

// 计算偶校验位
bool calculateParity(uint16_t value) {
  // 计算value中1的个数
  uint8_t count = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (value & (1 << i)) count++;
  }
  return (count % 2) == 0;
}

// 检测AS5047P是否存在
bool checkAS5047Presence() {
  // 尝试读取角度寄存器
  uint16_t response = readRegister(AS5047_ANGLE_REG);
  
  // 检查响应是否有效
  if (response == 0xFFFF || response == 0x0000) {
    return false; // 全0或全1通常表示无连接
  }
  
  // 检查奇偶校验（可选）
  // ...
  
  return true;
}

