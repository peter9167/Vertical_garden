#include <TimerOne.h>

#include <SoftwareSerial.h>        // 블루투스 시리얼 통신 라이브러리 추가
#define BT_RXD 8
#define BT_TXD 7
SoftwareSerial bluetooth(BT_RXD, BT_TXD);        // 블루투스 설정 BTSerial(Tx, Rx)

#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int pir = 3; //인체 감지 센서
int fire = 4; //불꽃 감지 센서
int sound = 5; //소리 감지 센서
 
int light = A0; //조도 센서
int soil = A1; //토양 수분 센서

//-------타이머 인터럽트를 위한 타이머 카운트------------
int time_count = 0;
int hum_count = 0;
int temp_count = 0;
int light_count = 0;
int soil_count = 0;

//------모듈 사용---------
int 

void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  pinMode(pir, INPUT);
  pinMode(fire, INPUT);
  pinMode(sound, INPUT);
  pinMode(light, INPUT);
  pinMode(soil, INPUT);
  Timer1.initialize(100000); //1000000μs = 1s, 1000us = 0.001s, 1000us = 1ms
  Timer1.attachInterrupt(timerIsr); // run every 0.1 seconds
}
 
void loop(){
  int h = dht.readHumidity(); //습도 값 저장 변수 선언
  int t = dht.readTemperature(); //온도 값 저장 변수 선언
  int l = analogRead(A0);

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

  //------조도센서 값 읽기 카운터-------
  if(light_count == 6){
    light(l);
    light_count = 0;
  }
}

void timerIsr(){
  time_count += 1;
  hum_count += 1;
  temp_count += 1;
  light_count += 1;
}
