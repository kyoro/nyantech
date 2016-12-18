//ねこが来たとみなす距離（センチ）
#define NEARBY_DISTANCE 20
//ねこが来たと判定する検出回数（多くすると安定するが、検知しにくくなる）
#define NEARBY_TIME 15

//ピン番号設定
#define LED_PIN   4
#define TRIG_PIN  5
#define ECHO_PIN  16


//距離測定
unsigned int measureDistance() {
  //パルスを発射
  digitalWrite(TRIG_PIN, HIGH);
  delay(15);
  digitalWrite(TRIG_PIN, LOW);

  //パルスを受信するまでの時間を計測
  int duration = pulseIn(ECHO_PIN, HIGH);

    //超音波は音速なので、反射して戻ってくるのにかかった時間から距離を計測
  return ((duration * 340 * 100) / 1000000) / 2;
}


bool nearby = false;
bool lastStatus;
unsigned int stateCount = 0;


//ねこ検知
void nearbyCheck() {
  int distance = measureDistance();
  
  //距離をシリアルに表示
  Serial.println("Distance(cm): " + String(distance));

  //計測距離から、検知範囲に対象がいるかどうかを判定
  bool detected;
  if(distance > 0 && distance < NEARBY_DISTANCE){
    detected = true;
  }else{
    detected = false;
  }

  //検出状態と判定結果が違い、なおかつ前回の判定結果と同様の場合はカウンタを加算
  if(nearby != detected && detected == lastStatus){
    stateCount ++;
  }else{
    stateCount = 0;
  }
  lastStatus = detected;

  //連続して同様の判定結果が出た場合、検知状態を切り替える
  if(stateCount >= NEARBY_TIME && nearby != detected){
    Serial.println("Nearby state was changed");
    nearby = detected;

    //対象物を検知している間、LEDを点灯
    digitalWrite(LED_PIN, nearby ? HIGH : LOW);
  }
}


void setup() {
  //初期設定
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}


void loop() {
  //対象物が近くにいるか検知
  nearbyCheck();
    
  delay(200);
}
