#define TOUCH_PIN T4  // GPIO4, 也可以换成其他触摸引脚

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Touch sensor test");
}

void loop() {
  // 读取触摸值
  int touchValue = touchRead(TOUCH_PIN);

  Serial.println(touchValue);

  // 设置阈值（需要实验调节，比如 < 30 表示触摸）
  if (touchValue < 30) {
    Serial.println("Touched!");
  }

  delay(100);
}