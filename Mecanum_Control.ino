#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Initialising the pins for Arduino Mega
int speedPinFL = 4;
int dir1FL = 22;
int dir2FL = 24;
int speedPinFR = 5;
int dir1FR = 28;
int dir2FR = 26;
int speedPinRL = 3;
int dir1RL = 30;
int dir2RL = 32;
int speedPinRR = 2;
int dir1RR = 34;
int dir2RR = 36;

//Defining the variables
int Chassis_Vx = 0;
int Chassis_Vy = 0;
int Chassis_Wz = 0;

float Wheel_Speed_FL_Calc;
float Wheel_Speed_FR_Calc;
float Wheel_Speed_RL_Calc;
float Wheel_Speed_RR_Calc;
float Wheel_Speed_FL_Output;
float Wheel_Speed_FR_Output;
float Wheel_Speed_RL_Output;
float Wheel_Speed_RR_Output;

//Measured width and length from car center to a single wheel's center
float Wheel_To_Center_Width = 0.07; //m
float Wheel_To_Center_Length = 0.06; //m

//Initializing the radio frequency communication
RF24 radio(6, 7); //CE, CSN pins

const byte address[6] = "00001";

//Joystick data structure
struct JoyStick_Data_t {
  int Left_VRx = 0;
  int Left_VRy = 0;
  int Right_VRx = 0;
  int Right_VRy = 0;
  byte Right_SW = 0;
};

JoyStick_Data_t JoyStick_Data;

void setup() {
  // put your setup code here, to run once:
  pinMode(speedPinFL,OUTPUT);
  pinMode(dir1FL,OUTPUT);
  pinMode(dir2FL,OUTPUT);
  pinMode(speedPinFR,OUTPUT);
  pinMode(dir1FR,OUTPUT);
  pinMode(dir2FR,OUTPUT);
  pinMode(speedPinRL,OUTPUT);
  pinMode(dir1RL,OUTPUT);
  pinMode(dir2RL,OUTPUT);
  pinMode(speedPinRR,OUTPUT);
  pinMode(dir1RR,OUTPUT);
  pinMode(dir2RR,OUTPUT);

  radio.begin();
  
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  
  radio.startListening(); //Receiving signal
  
  Serial.begin(9600);
}

void loop() {
  if (radio.available()){
    radio.read(&JoyStick_Data,sizeof(JoyStick_Data_t));
  }

  //The spinning function
  if(JoyStick_Data.Right_SW){
    Chassis_Vx = 0;
    Chassis_Vy = 0;
    Chassis_Wz = 1960; 
  }
  
  else{
    Chassis_Vx = JoyStick_Data.Left_VRx;
    Chassis_Vy = JoyStick_Data.Left_VRy;
    Chassis_Wz = JoyStick_Data.Right_VRx;
  }

  //Inverse kinematic calculation for four wheel's speed
  Wheel_Speed_FL_Calc = (Chassis_Vy + Chassis_Vx + (Wheel_To_Center_Width + Wheel_To_Center_Length)*Chassis_Wz);
  Wheel_Speed_FR_Calc = (Chassis_Vy - Chassis_Vx - (Wheel_To_Center_Width + Wheel_To_Center_Length)*Chassis_Wz);
  Wheel_Speed_RL_Calc = (Chassis_Vy - Chassis_Vx + (Wheel_To_Center_Width + Wheel_To_Center_Length)*Chassis_Wz);
  Wheel_Speed_RR_Calc = (Chassis_Vy + Chassis_Vx - (Wheel_To_Center_Width + Wheel_To_Center_Length)*Chassis_Wz);

  //Change wheel's direction based on calculated speed
  if(Wheel_Speed_FL_Calc >= 0){
    digitalWrite(dir1FL,HIGH);
    digitalWrite(dir2FL,LOW);
  }
  else{
    digitalWrite(dir1FL,LOW);
    digitalWrite(dir2FL,HIGH);
  }
  
  if(Wheel_Speed_FR_Calc >= 0){
    digitalWrite(dir1FR,HIGH);
    digitalWrite(dir2FR,LOW);
  }
  else{
    digitalWrite(dir1FR,LOW);
    digitalWrite(dir2FR,HIGH);
  }
  
  if(Wheel_Speed_RL_Calc >= 0){
    digitalWrite(dir1RL,HIGH);
    digitalWrite(dir2RL,LOW);
  }
  else{
    digitalWrite(dir1RL,LOW);
    digitalWrite(dir2RL,HIGH);
  }
  
  if(Wheel_Speed_RR_Calc >= 0){
    digitalWrite(dir1RR,HIGH);
    digitalWrite(dir2RR,LOW);
  }
  else{
    digitalWrite(dir1RR,LOW);
    digitalWrite(dir2RR,HIGH);
  }

  //Minimum activation speed setting
  if(abs(Wheel_Speed_FL_Calc) <= 100){
    Wheel_Speed_FL_Calc = 0;
  }
  if(abs(Wheel_Speed_FR_Calc) <= 100){
    Wheel_Speed_FR_Calc = 0;
  }
  if(abs(Wheel_Speed_RL_Calc) <= 100){
    Wheel_Speed_RL_Calc = 0;
  }
  if(abs(Wheel_Speed_RR_Calc) <= 100){
    Wheel_Speed_RR_Calc = 0;
  }

  //Speed output
  analogWrite(speedPinFL,constrain(abs(Wheel_Speed_FL_Calc),0,255));
  analogWrite(speedPinFR,constrain(abs(Wheel_Speed_FR_Calc),0,255));
  analogWrite(speedPinRL,constrain(abs(Wheel_Speed_RL_Calc),0,255));
  analogWrite(speedPinRR,constrain(abs(Wheel_Speed_RR_Calc),0,255));
}
