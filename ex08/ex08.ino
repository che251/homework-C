#include <WiFi.h>
#include <WebServer.h>

// 你的手机热点账号
const char* ssid = "Jelly";
const char* password = "cld061129";
const int LED_PIN = 2;    // 板载LED引脚
const int TOUCH_PIN = T0; // 触摸引脚GPIO4

WebServer server(80);
// 全局状态变量，记录安防系统状态
bool armState = false;    // false=未布防  true=已布防
bool alarmLock = false;  // true=报警锁定，松手也持续闪烁

// 生成网页页面
String makePage() {
  String statusText;
  if(!armState) statusText = "未布防";
  else if(alarmLock) statusText = "布防中，报警已锁定";
  else statusText = "布防待机（触摸引脚触发报警）";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>作业8 安防报警主机</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:60px;">
  <h1>物联网安防报警器</h1>
  <p style="font-size:22px;">当前系统状态：<b>)rawliteral" + statusText + R"rawliteral(</b></p>
  <br>
  <button onclick="fetch('/arm');location.reload()" style="padding:14px 32px; font-size:20px; margin:10px;">布防 Arm</button>
  <button onclick="fetch('/disarm');location.reload()" style="padding:14px 32px; font-size:20px; margin:10px;">撤防 Disarm</button>
</body>
</html>
)rawliteral";
  return html;
}

// 首页路由
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 布防接口
void handleArm() {
  armState = true;
  server.sendHeader("Location", "/");
  server.send(303);
}

// 撤防接口：清除报警锁定、熄灭LED、重置状态
void handleDisarm() {
  armState = false;
  alarmLock = false;
  digitalWrite(LED_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // WiFi连接逻辑，带错误打印
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(WiFi.status() == WL_CONNECT_FAILED){
      Serial.println("\nWiFi密码错误！");
    }else if(WiFi.status() == WL_NO_SSID_AVAIL){
      Serial.println("\n找不到热点 Jelly！");
    }
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("网页访问地址：http://");
  Serial.println(WiFi.localIP());

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();
  uint16_t touchValue = touchRead(TOUCH_PIN);

  // 仅布防且未报警时，触摸引脚触发锁定报警
  if(armState && !alarmLock && touchValue < 30){
    alarmLock = true;
  }

  // 报警锁定状态：LED高频闪烁，松手不停止
  if(alarmLock){
    digitalWrite(LED_PIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);
    delay(80);
  }
}