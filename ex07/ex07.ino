#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Jelly";
const char* password = "cld061129";
const int LED_PIN = 2;

WebServer server(80);
uint8_t brightness = 0;

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>作业7 无极调光</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:50px;">
  <h1>LED无极亮度调节 0~255</h1>
  <input type="range" id="slider" min="0" max="255" value=")rawliteral" + String(brightness) + R"rawliteral(" style="width:80%;height:30px;">
  <p>当前亮度值：<span id="val">)rawliteral" + String(brightness) + R"rawliteral(</span></p>
  <script>
    const slider = document.getElementById("slider");
    const valText = document.getElementById("val");
    slider.addEventListener("input", async ()=>{
      let v = slider.value;
      valText.innerText = v;
      await fetch(`/set?b=${v}`);
    })
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleSetBright() {
  if(server.hasArg("b")){
    brightness = server.arg("b").toInt();
    analogWrite(LED_PIN, brightness); // 替换ledcWrite，通用PWM
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // 删除全部ledc相关初始化代码，不再报错
  analogWrite(LED_PIN, 0); // 上电默认熄灭

  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(WiFi.status() == WL_CONNECT_FAILED){
      Serial.println("\nWiFi密码错误！");
    }else if(WiFi.status() == WL_NO_SSID_AVAIL){
      Serial.println("\n找不到热点 Jelly！");
    }
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleSetBright);
  server.begin();
}

void loop() {
  server.handleClient();
}