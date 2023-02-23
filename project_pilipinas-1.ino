#include <NewPing.h>
#define L_InPin_1  2  //왼쪽모터 핀1
#define L_InPin_2  3  //왼쪽모터 핀2
#define R_InPin_1  7  //오른쪽모터 핀1
#define R_InPin_2  4  //오른쪽모터 핀2
#define L_en  5 //왼쪽모터 속도 제어 핀
#define R_en  6 //오른쪽모터 속도 제어 핀
#define F_Sonar  8  //정면부 초음파 센서 연결 핀
#define R_Sonar  10 //오른쪽 초음파 센서 연결 핀
#define L_Sonar  11 //왼쪽 초음파 센서 연결 핀
#define MaxDistance  400

float mirro_distance=800; //미로의 폭
float turn_distance=400; // 회전을 시작하는 거리
float timer1=0;
float timer2=0;
float timer3=0;
float timer4=0;
float timer5=0;

NewPing F_sensor(F_Sonar,F_Sonar,MaxDistance);
float F_distance; //정면 초음파 센서값을 저장할 변수 선언
float F_distance_old;
float F_Error;
NewPing L_sensor(L_Sonar,L_Sonar,MaxDistance);
float L_distance; //왼쪽 초음파 센서값을 저장할 변수 선언
float L_distance_old;
float L_Error;
NewPing R_sensor(R_Sonar,R_Sonar,MaxDistance);
float R_distance; //오른쪽 초음파 센서값을 저장할 변수 선언
float R_distance_old;
float R_Error;

void read_sonar_sensor(){ //초음파 센서 소숫점 단위로 측정함수
  F_distance=(float)(F_sensor.ping())*10/US_ROUNDTRIP_CM;
  R_distance=(float)(R_sensor.ping())*10/US_ROUNDTRIP_CM;
  L_distance=(float)(L_sensor.ping())*10/US_ROUNDTRIP_CM;
  if(F_distance==0){F_distance=MaxDistance*10;}
  if(L_distance==0){L_distance=MaxDistance*10;}
  if(R_distance==0){R_distance=MaxDistance*10;}
  }

void update_sonar_old(){ //초음파 센서의 옛날값 저장 함수
  F_distance_old=F_distance;
  L_distance_old=L_distance;
  R_distance_old=R_distance;
  }

void update_sonar_error(){ //초음파 센서의 현재값과 과거값의 차를 구하는 함수
  F_Error=F_distance - F_distance_old;
  R_Error=R_distance - R_distance_old;
  L_Error=L_distance - L_distance_old;
  }

void setup() {
  Serial.begin(115200);  //스크린프린트를 115200의 속도로 사용
  pinMode(L_InPin_1,OUTPUT); //왼쪽모터1 핀 모드를 아웃풋으로
  pinMode(L_InPin_2,OUTPUT); //왼쪽모터2 핀 모드를 아웃풋으로
  pinMode(R_InPin_1,OUTPUT); //오른쪽모터1 핀 모드를 아웃풋으로
  pinMode(R_InPin_2,OUTPUT); //오른쪽모터2 핀 모드를 아웃풋으로
  analogWrite(L_en,140);  //왼쪽모터 속도 조정
  analogWrite(R_en,140);  //오른쪽모터 속도 조정
}

void moter_control(int L_mode,int L_speed,int R_speed,int R_mode){//모터 컨트롤 함수 전진:1 후진:-1 정지:0
  switch(L_mode){
    case  1 : analogWrite(L_en,L_speed);
              digitalWrite(L_InPin_1,HIGH);
              digitalWrite(L_InPin_2,LOW);
              break;
    case  0 : analogWrite(L_en,L_speed);
              digitalWrite(L_InPin_1,LOW);
              digitalWrite(L_InPin_2,LOW);
              break;
    case -1 : analogWrite(L_en,L_speed);
              digitalWrite(L_InPin_1,LOW);
              digitalWrite(L_InPin_2,HIGH);
              break;
    }
    switch(R_mode){
    case  1 : analogWrite(R_en,R_speed);
              digitalWrite(R_InPin_1,HIGH);
              digitalWrite(R_InPin_2,LOW);
              break;
    case  0 : analogWrite(R_en,R_speed);
              digitalWrite(R_InPin_1,LOW);
              digitalWrite(R_InPin_2,LOW);
              break;
    case -1 : analogWrite(R_en,R_speed);
              digitalWrite(R_InPin_1,LOW);
              digitalWrite(R_InPin_2,HIGH);
              break;
    }
  }


int wall_fillowing_one_upgreat(){ //상황에 맞춰 한쪽 벽을 타는 함수
  update_sonar_error();
  if(F_distance>turn_distance && (R_distance*0.96 <= L_distance && R_distance*1.04 >= L_distance)){
    int R_speed=150-L_Error*5;
    int L_speed=150+L_Error*5;
    if(R_speed >= 240){ R_speed=240;}
    if(R_speed <= 100){ R_speed=100;}
    if(L_speed >= 240){ L_speed=240;}
    if(L_speed <= 100){ L_speed=100;}
    moter_control(1,L_speed,R_speed,1);
    update_sonar_old();
    delay(10);
    timer4=millis();
    return 1;
    }
  else if(F_distance>turn_distance && (R_distance >= (mirro_distance/3)*2)){
    int R_speed=150-L_Error*5;
    int L_speed=150+L_Error*5;
    if(R_speed >= 240){ R_speed=240;}
    if(R_speed <= 100){ R_speed=100;}
    if(L_speed >= 240){ L_speed=240;}
    if(L_speed <= 100){ L_speed=100;}
    moter_control(1,L_speed,R_speed,1);
    update_sonar_old();
    delay(10);
    return 1;
    }
  else if(F_distance>turn_distance && (L_distance >= (mirro_distance/3)*2)){
    int R_speed=150+R_Error*5;
    int L_speed=150-R_Error*5;
    if(R_speed >= 240){ R_speed=240;}
    if(R_speed <= 100){ R_speed=100;}
    if(L_speed >= 240){ L_speed=240;}
    if(L_speed <= 100){ L_speed=100;}
    moter_control(1,L_speed,R_speed,1);
    update_sonar_old();
    delay(10);
    return 1;
    }
   else {
    moter_control(1,150,150,1);
    timer4=millis();
    }
   return 0;
  }

void wall_fillowing_two_upgreat(){ //한쪽 벽을 타지 않을시 양쪽 벽을 타는 함수(조건 벽이 2개 모두 존재하며 가운데로 0.04% 오차 내에 존재하지 않을 경우 작동
  if((wall_fillowing_one_upgreat()) == 0){
    int R_speed=150-(R_distance-L_distance)*2;
    int L_speed=150+(R_distance-L_distance)*2;
    if(R_speed >= 240){ R_speed=240;}
    if(R_speed <= 100){ R_speed=100;}
    if(L_speed >= 240){ L_speed=240;}
    if(L_speed <= 100){ L_speed=100;}
    moter_control(1,L_speed,R_speed,1);
    }
  }

int turn_right(){ // 턴을 해야할 시 오른쪽로 꺽는 함수
  if(R_distance >= (mirro_distance/5)*4){
    float min_distance=F_distance;
    moter_control(1,150,150,-1);
    delay(300);
    while(L_distance > min_distance+40){
       read_sonar_sensor();
       moter_control(1,150,150,-1);
      }
      return 1;
    }
    return 0;
  }

int turn_left(){ // 턴을 해야할 시 왼쪽로 꺽는 함수
  if(L_distance >= (mirro_distance/5)*4){
    timer3=millis();
    float min_distance=F_distance;
    moter_control(-1,150,150,1);
    delay(300);
    while(R_distance > min_distance+40){
       read_sonar_sensor();
       moter_control(-1,150,150,1);
      }
      timer2=millis()-timer3;
      return 1;
    }
    return 0;
  }

void turning_left(){ //중간에 왼쪽으로 구멍이 있을때 들어가는 함수
  if( L_distance >= mirro_distance){
    if(millis()-timer1 >= timer5 && timer1 != 0){
      moter_control(-1,150,150,1);
      delay(timer2);
      while(1){
        read_sonar_sensor();
        moter_control(1,150,150,1);
        if(R_distance+L_distance <= mirro_distance || F_distance <= turn_distance){
          break;
          }
        }
      }
    }
  else{
    timer1=millis();
    }
  }

void turn_all(){ //돌아가는것과 관련된 모든 함수
  if(F_distance <= turn_distance){
    timer5=millis()-timer4;
    if((turn_left())==0){
       turn_right();
      }
    }
  if(F_distance >= mirro_distance){
    turning_left();
    }
  }


void all_stop(){ //두개의 벽이 모두 없을시 정지하는 함수
  if(R_distance > 1500 && L_distance > 1500){
     moter_control(0,0,0,0);
    }
  }

void seiral_sonar(){
  Serial.print("F_distance : ");
  Serial.print(F_distance);
  Serial.print("mm   maze_distance : ");
  Serial.print(L_distance+R_distance);
  Serial.println("mm");
  }

void loop() {
  read_sonar_sensor();
  seiral_sonar();
  turn_all();
  wall_fillowing_two_upgreat();
  all_stop();
}
