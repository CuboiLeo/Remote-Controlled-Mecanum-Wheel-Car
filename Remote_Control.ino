#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 10);  // CE, CSN

//Initializing pins
const int Left_VRx_Pin = A1;
const int Left_VRy_Pin = A0;
const int Right_VRx_Pin = A3;
const int Right_VRy_Pin = A2;
const int Right_SW_Pin = 8;

//address through which two modules communicate.
const byte address[6] = "00001";

//Defining joystick structure
struct JoyStick_Data_t {
  int Left_VRx = 0;
  int Left_VRy = 0;
  int Right_VRx = 0;
  int Right_VRy = 0;
  byte Right_SW = 0;
};

JoyStick_Data_t JoyStick_Data;

void setup()
{
  radio.begin();
  
  //set the address
  radio.openWritingPipe(address);

  radio.setPALevel(RF24_PA_MIN);
  
  //Set module as transmitter
  radio.stopListening();

  pinMode(Right_SW_Pin, INPUT);
  digitalWrite(Right_SW_Pin, HIGH);
  Serial.begin(9600);
}
void loop()
{
  //Send message to receiver, map readings to sensible speed values
  JoyStick_Data.Left_VRx = map(analogRead(Left_VRx_Pin),0,1023,240,-240);
  JoyStick_Data.Left_VRy = map(analogRead(Left_VRy_Pin),0,1023,240,-240);
  JoyStick_Data.Right_VRx = map(analogRead(Right_VRx_Pin),0,1023,1600,-1600);
  JoyStick_Data.Right_VRy = map(analogRead(Right_VRy_Pin),0,1023,1600,-1600);
  JoyStick_Data.Right_SW = map(digitalRead(Right_SW_Pin),0,1,1,0);

  radio.write(&JoyStick_Data, sizeof(JoyStick_Data_t));
}
