// 定义步进电机的控制引脚
int motorPin1 = 8;  // IN1
int motorPin2 = 9;  // IN2
int motorPin3 = 10; // IN3
int motorPin4 = 11; // IN4

void setup() {
  // 设置引脚为输出
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
}

void loop() {
  // 顺时针旋转
  for (int i = 0; i < 512; i++) {  // 步进电机步数
    stepMotor(i % 4);
    delay(5);  // 适当延时调整速度
  }

  // delay(1000);  // 停顿1秒

  // // 逆时针旋转
  // for (int i = 0; i < 512; i++) {
  //   stepMotor(3 - i % 4);  // 逆时针旋转时调整控制顺序
  //   delay(10);
  // }

  // delay(1000);
}

void stepMotor(int step) {
  switch(step) {
    case 0:
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
      break;
    case 1:
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
      break;
    case 2:
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, HIGH);
      digitalWrite(motorPin4, LOW);
      break;
    case 3:
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, HIGH);
      break;
  }
}

