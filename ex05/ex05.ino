#define TOUCH_PIN 4
#define LED_PWM_PIN 5
#define THRESHOLD 500
// 触摸防抖冷却时间
const unsigned long DEBOUNCE_COOL = 300;

// 呼吸灯档位：1/2/3档
int speedGear = 1;
// 调整后：1档适中慢、2档中等、3档极速，差距明显，一档不拖沓
int gearDelay[4] = {0, 40, 15, 2};
int brightness = 0;
int step = 1;

// 触摸中断防抖相关
volatile bool touchFlag = false;
unsigned long lastTouchTime = 0;

void gotTouch() {
  touchFlag = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PWM_PIN, OUTPUT);
  touchAttachInterrupt(TOUCH_PIN, gotTouch, THRESHOLD);
}

void loop() {
  // 触摸档位切换逻辑（带软件防抖）
  if (touchFlag)
  {
    unsigned long now = millis();
    if (now - lastTouchTime > DEBOUNCE_COOL)
    {
      // 档位循环切换 1→2→3→1
      speedGear++;
      if (speedGear > 3) speedGear = 1;
      Serial.print("当前呼吸档位：");
      Serial.println(speedGear);
      lastTouchTime = now;
    }
    touchFlag = false;
  }

  // PWM呼吸渐变逻辑
  analogWrite(LED_PWM_PIN, brightness);
  brightness += step;
  // 到达亮度边界反向
  if (brightness <= 0 || brightness >= 255)
  {
    step = -step;
  }
  // 根据当前档位控制呼吸速度
  delay(gearDelay[speedGear]);
}