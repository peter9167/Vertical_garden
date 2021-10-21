#include <TimerOne.h>
///////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>        // 블루투스 시리얼 통신 라이브러리 추가
#define BT_RXD 7
#define BT_TXD 8
SoftwareSerial bluetooth(BT_RXD, BT_TXD); // 블루투스 설정 BTSerial(Tx, Rx)
///////////////////////////////////////////////////////////////////////////
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11   // DHT22 (AM2302) 센서종류 설정
DHT dht(DHTPIN, DHTTYPE);
///////////////////////////////////////////////////////////////////////////
#include <Servo.h>    // 서보모터 라이브러리
Servo servo;    // 서보모터 사용을 위한 객체 생성
///////////////////////////////////////////////////////////////////////////
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define PIN       13 //NeoPixel LED 핀 번호
#define NUMPIXELS 65 //NeoPixel LED 개수
#define DELAYVAL 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
///////////////////////////////////////////////////////////////////////////
int light = A0; //조도 센서
int pir = A1; //인체 감지 센서
int fire = A2; //불꽃 감지 센서
int sound = A3; //소리 감지 센서
int motor = 6;

int pump = 11;
int fan = 12;

//-------타이머 인터럽트를 위한 타이머 카운트------------
int time_count = 0;
int hum_count = 0;
int temp_count = 0;
int pir_count = 0;
int fire_count = 0;
int sound_count = 0;
int light_count = 0;
int soil_count = 0;
int bt_count = 0;

//------모듈 사용---------
int led_count = 0;
int pump_count = 0;

int Pcount = 0;

void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600);
  dht.begin();
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  servo.attach(motor);  // 서보모터 연결
  servo.write(0);
  
  pinMode(pir, INPUT); //인체 감지 센서
  pinMode(fire, INPUT); //불꽃 감지 센서
  pinMode(sound, INPUT); //소리 감지 센서
  pinMode(light, INPUT); //조도 센서
  pinMode(pump, OUTPUT); //pump
  pinMode(fan, OUTPUT); //쿨링팬
  
  Timer1.initialize(100000); //1000000μs = 1s, 1000us = 0.001s, 1000us = 1ms
  Timer1.attachInterrupt(timerIsr); // run every 0.1 seconds

  ///////////////////////////////////////////////////////////////
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();  // Initialize all pixels to 'off'
  ///////////////////////////////////////////////////////////////
}
 
void loop(){
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int p = analogRead(pir);
  int f = analogRead(fire);
  int s = analogRead(sound);
  int l = analogRead(A0);
  int bt = (bluetooth.read());

  //------습도 값 읽기 카운터-------
  if(hum_count == 4){
    hum(h);
    hum_count = 0;
  }

  //------온도 값 읽기 카운터-------
  if(temp_count == 5){
    temp(t);
    temp_count = 0;
  }

  //------인체 감지 센서 값 읽기 카운터-------
  if(pir_count == 6){
    PIR(p);
    pir_count = 0;
  }

  //------불꽃 감지 센서 값 읽기 카운터-------
  if(fire_count == 7){
    FIRE(f);
    fire_count = 0;
  }

  //------소리(사운드) 감지 센서 값 읽기 카운터-------
  if(sound_count == 8){
    SOUND(s);
    sound_count = 0;
  }

  //------조도센서 값 읽기 카운터-------
  if(light_count == 9){
    LIGHT(l);
    light_count = 0;
  }
}

void timerIsr(){
  time_count += 1;
  hum_count += 1;
  temp_count += 1;
  pir_count += 1;
  fire_count += 1;
  sound_count += 1;
  light_count += 1;
}

void hum(int h){
  Serial.print("습도:");
  Serial.println(h); // 습도 값 출력
  if(h >= 70){
    digitalWrite(fan, HIGH);
  } else{ 
    digitalWrite(fan, LOW);
  }
}

void temp(int t){
  Serial.print("온도:");
  Serial.println(t); // 온도 값 출력
  if(t >= 26){
    digitalWrite(fan, HIGH);
  } else{
    digitalWrite(fan, LOW);
  }
}

void PIR(int p){
  Serial.print("인체 감지:");
  Serial.println(p); // 인체 감지 값 출력
  if(p == 1){
    for(int i = 20; i <= 120; i++){
      servo.write(i);
    }
  } else{
    servo.write(20);
  }
}

void FIRE(int f){
  Serial.print("불꽃:");
  Serial.println(f); // 불꽃 감지 값 출력
  if(f == 1){
    digitalWrite(pump, HIGH);
  } else{
    digitalWrite(pump, LOW);
  }
}

void SOUND(int s){
  Serial.print("소리:");
  Serial.println(s); // 소리 감지 값 출력
  if(s < 1200){
    servo.write(100);
  }
}

void LIGHT(int l){
  Serial.print("조도:");
  Serial.println(l);
  Serial.print("조도:");
  Serial.println(l);
  if(l >= 400){
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 150, 0));
      pixels.show();
      delay(DELAYVAL); // Pause before next pass through loop
    }
  } else{
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
}

void PUMP(){
  if(++Pcount >= 10 )
  {
    Pcount = 0;
    digitalWrite(13,HIGH);
  }
  else 
  {
    digitalWrite(13,LOW);
  }
}

void BT(int bt){
  if(bluetooth.available()){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
  }
}
