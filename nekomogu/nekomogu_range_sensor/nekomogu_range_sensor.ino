#include <Ticker.h>

//検知する距離
#define NEARBY_DISTANCE 10
//検知する時間
#define NEARBY_TIME 3

//ピン番号設定
#define LED_PIN   4
#define TRIG_PIN  5
#define ECHO_PIN  16

Ticker ticker;

unsigned int distance;
bool nearby = false;
unsigned int nearbyCount = 0;

void nearbyChecker() {
  //距離をコンソールに表示
  Serial.println("Distance(cm): " + String(distance));

  //対象物との距離が検知距離以下の場合はnearbyCountを加算、それ以上の場合は0にリセット
  if(distance < NEARBY_DISTANCE){
    nearbyCount ++;
  }else{
    nearbyCount = 0;
  }

  //検知時間以上の間、連続して対象物が検知距離に滞在する場合はnearbyフラグをたてる。
  if(nearbyCount >= NEARBY_TIME){
    if(!nearby){
      Serial.println("Detected!");
    }
    nearby = true;
  }else{
    nearby = false;
  }
}

unsigned int measureDistance() {
  //15msのパルスを発射
  digitalWrite(TRIG_PIN, HIGH);
  delay(15);
  digitalWrite(TRIG_PIN, LOW);

  //パルスを受信するまでの時間を計測
  int duration = pulseIn(ECHO_PIN, HIGH);

  //超音波は音速なので、反射して戻ってくるのにかかった時間を（ミリ秒）を58で割ると、センチメートルに変換できる
  return duration / 58;
}

void setup() {
  //初期設定
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //1秒ごとにnearbyChecker関数を実行して、接近状態にあるかどうかを判別
  ticker.attach(1, nearbyChecker);
}

void loop() {
  //対象物との距離を測定
  distance = measureDistance();
  
  //対象物を検知している間、LEDを点灯
  if(nearby){
    digitalWrite(LED_PIN, HIGH);
  }else{
    digitalWrite(LED_PIN, LOW);
  }
}
