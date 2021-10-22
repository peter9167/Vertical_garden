#include <TimerOne.h>
///////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>        // 블루투스 시리얼 통신 라이브러리 추가
#define BT_RXD 5
#define BT_TXD 6
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
#include <Stepper.h>
const int stepsPerRevolution = 64; // 모터별 스탭 수 설정 (28YBJ-48의 경우 64)
Stepper stepper(stepsPerRevolution, 8, 10, 9, 11);
///////////////////////////////////////////////////////////////////////////
int light = A0; //조도 센서
int fire = A2; //불꽃 감지 센서
int pir = 4; //인체 감지 센서

int pump = 11;
int fan = 12;

//-------타이머 인터럽트를 위한 타이머 카운트------------
int time_count = 0;
int hum_count = 0;
int temp_count = 0;
int pir_count = 0;
int fire_count = 0;
int light_count = 0;
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
  stepper.setSpeed(500); // 속도 설정
  
  pinMode(pir, INPUT); //인체 감지 센서
  pinMode(fire, INPUT); //불꽃 감지 센서
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
  int p = digitalRead(pir);
  int f = analogRead(fire);
  int l = analogRead(A0);
  char bt = (bluetooth.read());

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

  //------조도센서 값 읽기 카운터-------
  if(light_count == 9){
    LIGHT(l);
    light_count = 0;
  }

  //------블루투스 모듈 사용 카운터-------
  if(light_count == 10){
    BT(bt, h, t, p, f, l);
  }
}

void timerIsr(){
  time_count += 1;
  hum_count += 1;
  temp_count += 1;
  pir_count += 1;
  fire_count += 1;
  light_count += 1;
  bt_count += 1;
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
    stepper.step(5000);
  } else{
    stepper.step(-5000);
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
    digitalWrite(pump,HIGH);
  }
  else 
  {
    digitalWrite(pump,LOW);
  }
}

void BT(char bt, int h, int t, int p, int f, int l){
  bluetooth.write("온도 : ");
  bluetooth.write(t);
  bluetooth.write("'C");
  bluetooth.write("습도 : ");
  bluetooth.write(h);
  bluetooth.write("%");
  bluetooth.write("빛 밝기 : ");
  bluetooth.write(l);
  if(bluetooth.available()){
    switch(bt){
      case '1':
        for(int i=0; i<NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(255, 150, 0));
          pixels.show();
          delay(DELAYVAL); // Pause before next pass through loop
        }
        break;
      case '2':
        for(int i=0; i<NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
          pixels.show();
        }
        break;
      case '3':
        digitalWrite(pump, HIGH);
        break;
      case '4':
        digitalWrite(pump, LOW);
        break;
      case '5':
        stepper.step(5000);
        break;
      case '6':
        stepper.step(-5000);
        break;
    }
  }
}
