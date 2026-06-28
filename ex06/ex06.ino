// ex06 双通道反向PWM警车渐变双闪
// 引脚定义：GPIO2红色LED，GPIO5黄色LED
const int ledRed = 2;
const int ledYellow = 5;

// PWM参数
const int freq = 5000;
const int resolution = 8; // 亮度范围 0~255

void setup() {
  Serial.begin(115200);
  // 直接绑定引脚+PWM参数，无通道报错
  ledcAttach(ledRed, freq, resolution);
  ledcAttach(ledYellow, freq, resolution);
  // 上电初始熄灭
  ledcWrite(ledRed, 0);
  ledcWrite(ledYellow, 0);
  Serial.println("互补反向呼吸初始化完成");
}

void loop() {
  // 红灯渐亮，黄灯同步渐暗
  Serial.println("红灯渐亮，黄灯渐暗");
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(ledRed, duty);
    ledcWrite(ledYellow, 255 - duty);
    delay(10);
  }

  // 红灯渐暗，黄灯同步渐亮
  Serial.println("红灯渐暗，黄灯渐亮");
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(ledRed, duty);
    ledcWrite(ledYellow, 255 - duty);
    delay(10);
  }

  Serial.println("一轮互补呼吸完成\n------------------------");
}