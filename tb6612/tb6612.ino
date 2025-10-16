// ----------------------
// TB6612FNG 测试示例
// ----------------------

// 电机A
#define PWMA 3
#define AIN1 4
#define AIN2 5

// 电机B
#define PWMB 6
#define BIN1 7
#define BIN2 8

// STBY 引脚
#define STBY 9

// 初始化
void setup() {
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  // 启动芯片
  digitalWrite(STBY, HIGH);

  Serial.begin(9600);
  Serial.println("TB6612FNG 测试开始...");
}

// 控制函数
void motorA(int speed) {
  if (speed > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, speed);
  } else if (speed < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, -speed);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);
  }
}

void motorB(int speed) {
  if (speed > 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, speed);
  } else if (speed < 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, -speed);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, 0);
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
