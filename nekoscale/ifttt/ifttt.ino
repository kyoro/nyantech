//-------------------------
// IFTTTへのデータ送信
//-------------------------

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//WiFi設定
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

//IFTTTのWebHook Key
//参照: https://ifttt.com/maker_webhooks のDocumentation
const char* iftttKey = "YOUR_IFTTT_WEBHOOK_KEY";
const char* iftttEvent = "YOUR_IFTTT_EVENT_NAME";

//WiFiへの接続
void wifiConnect() {
  Serial.print("Connecting to " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());
}

//IFTTTへのデータ送信関数
void sendDataToIFTTT(String weight) {
  const char* host = "maker.ifttt.com";
  WiFiClientSecure client;
  Serial.println("Try");
  //IFTTTのサーバに接続
  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected");
  //リクエストを送信(送信内容はJSON形式)
  String query = String("") + "{\"value1\":\"" + weight + "\"}";
  String request = String("") +
               "POST /trigger/" + iftttEvent + "/with/key/" + iftttKey + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Length: " + String(query.length()) +  "\r\n" + 
               "Content-Type: application/json\r\n\r\n" +
                query + "\r\n";
  client.print(request);
  //受信終了まで待つ
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
}

//-------------------------
// 体重計からのデータ取得
//-------------------------

#define RECV_PIN 5
#define TIMEOUT_US 30000
#define DETECT_INTERVAL_MS 10000

int recv_data(uint32_t *data) {
  int count = 0;
  while (true) {
    unsigned long pulse = pulseIn(RECV_PIN, HIGH, TIMEOUT_US);
    if (pulse == 0){
      break;
    }
    uint32_t bit = pulse < 750 ? 1 : 0;
    if (count < 32) {
      bitWrite(*data, 31 - count, bit);
    }
    count++;
  }
  return count;
}

void setup() {
  pinMode(RECV_PIN, INPUT);
  Serial.begin(115200);
  Serial.println("Neko Scale with Spreadsheet");
  //WiFi接続
  wifiConnect();
}

unsigned long last_detected = 0;  
void loop() {
  uint32_t data = 0;
  int count = recv_data(&data);
  if ((count == 39) && ((data >> 16) == 0xAB8C)) {
    if((millis() - last_detected) > DETECT_INTERVAL_MS){
      float weight = (float)(data & 0x7ff) / 10.0f;
      String weightStr = String(weight, 1);
      //測定データの送信
      sendDataToIFTTT(weightStr);
      Serial.println(weightStr + " kg ... Sent!");
    }
    last_detected = millis();    
  }
}
