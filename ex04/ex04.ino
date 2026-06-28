#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 500
const unsigned long DEBOUNCE_COOL = 300;

bool ledState = false;
int i = 0;
volatile bool touchFlag = false;
unsigned long lastTouchTime = 0;

void gotTouch() {
  touchFlag = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  touchAttachInterrupt(TOUCH_PIN, gotTouch, THRESHOLD);
}

void loop() {
  if (touchFlag)
  {
    unsigned long now = millis();
    if (now - lastTouchTime > DEBOUNCE_COOL)
    {
      if(i % 2 == 0)
      {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
      }
      Serial.println(i++);
      lastTouchTime = now;
    }
    touchFlag = false;
  }
  delay(100);
}