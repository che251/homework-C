#define LED_PIN 2
unsigned long prevTime = 0;
const unsigned long interval = 500;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}
void loop() {
  unsigned long curTime = millis();
  if(curTime - prevTime >= interval){
    prevTime = curTime;
    int ledState = !digitalRead(LED_PIN);
    digitalWrite(LED_PIN, ledState);
    if(ledState) Serial.println("LED ON(millis)");
    else Serial.println("LED OFF(millis)");
  }
}