#include <WiFi.h>
#include <WebServer.h>

// 你的手机热点账号
const char* ssid = "Jelly";
const char* password = "cld061129";
const int TOUCH_PIN = T0; // 触摸引脚GPIO4，和ex08共用同一引脚

WebServer server(80);

// 生成前端HTML页面，AJAX定时轮询拉取触摸数值
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>作业9 触摸传感器实时仪表盘</title>
  <style>
    body{font-family:Arial; text-align:center; margin-top:70px;}
    #sensorNum{font-size:90px; color:#0055dd; font-weight:bold; margin:40px 0;}
  </style>
</head>
<body>
  <h1>触摸传感器实时监控仪表盘</h1>
  <div id="sensorNum">--</div>
  <script>
    // AJAX异步请求函数，每100ms刷新一次数据
    async function fetchTouchData(){
      const res = await fetch("/touchData");
      const value = await res.text();
      document.getElementById("sensorNum").innerText = value;
    }
    // 定时器持续拉取数据，实现实时跳动
    setInterval(fetchTouchData, 100);
    fetchTouchData();
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// 首页路由，下发仪表盘HTML页面
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 数据上报接口：返回当前触摸传感器原始模拟量
void handleTouchData() {
  uint16_t touchValue = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(touchValue));
}

void setup() {
  Serial.begin(115200);

  // WiFi连接逻辑，带错误提示
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
  Serial.print("仪表盘访问地址：http://");
  Serial.println(WiFi.localIP());

  // 注册网页与数据接口路由
  server.on("/", handleRoot);
  server.on("/touchData", handleTouchData);
  server.begin();
}

void loop() {
  server.handleClient();
}