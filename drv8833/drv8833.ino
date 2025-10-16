 // ==========================
// DRV8833 双路电机驱动示例
// ==========================

// 电机 A 引脚
#define AIN1 4
#define AIN2 5

// 电机 B 引脚
#define BIN1 6
#define BIN2 7

// PWM 频率使用 Arduino 默认 PWM 输出
// 可用 analogWrite(pin, value) 控制占空比 (0-255)

// 初始化
void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  Serial.begin(9600);
  Serial.println("DRV8833 测试开始...");
}

// 控制函数
void motorA(int speed) {
  if (speed > 0) {
    analogWrite(AIN1, speed);
    digitalWrite(AIN2, LOW);
  } else if (speed < 0) {
    analogWrite(AIN2, -speed);
    digitalWrite(AIN1, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }
}

void motorB(int speed) {
  if (speed > 0) {
    analogWrite(BIN1, speed);
    digitalWrite(BIN2, LOW);
  } else if (speed < 0) {
    analogWrite(BIN2, -speed);
    digitalWrite(BIN1, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }
}

void loop() {
  Serial.println("前进");
  motorA(200);
  motorB(200);
  delay(2000);

  Serial.println("后退");
  motorA(-200);
  motorB(-200);
  delay(2000);

  Serial.println("左转");
  motorA(-200);
  motorB(200);
  delay(1500);

  Serial.println("右转");
  motorA(200);
  motorB(-200);
  delay(1500);

  Serial.println("停止");
  motorA(0);
  motorB(0);
  delay(2000);
}
