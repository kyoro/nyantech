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
}

unsigned long last_detected = 0;  
void loop() {
  uint32_t data = 0;
  int count = recv_data(&data);
  if ((count == 39) && ((data >> 16) == 0xAB8C)) {
    if((millis() - last_detected) > DETECT_INTERVAL_MS){
      float weight = (float)(data & 0x7ff) / 10.0f;
      String result = String(weight, 1) + String(" kg");
      Serial.println(result);
      Serial.println("----");
    }
    last_detected = millis();    
  }
}
