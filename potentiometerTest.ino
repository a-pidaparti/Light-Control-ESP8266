#include <SPI.h>  
const int red = 10;
const int blue = 9;
const int green  = 8;
int PotWiperVoltage = 1;
int RawVoltage = 0;
float Voltage = 0;
 
void setup() {
  pinMode (red, OUTPUT);
  pinMode (blue, OUTPUT);
  pinMode (green, OUTPUT);   
  Serial.begin(9600);
  SPI.begin();
  digitalWrite(red, HIGH);
  digitalWrite(green, HIGH);
  digitalWrite(blue, HIGH);
  MCP41010Write(255, blue);
  MCP41010Write(255, green);    
}
 
void loop() {
//  // move the potentiometer in one direction
//  for (int level = 200; level > 0; level--)
//  {
//    MCP41010Write(level, red);
//    delay(20);
//    
//  }  // wait a couple seconds
//  delay(100);
//  // Now mover potentiometer in other directions
//  
//  for (int level = 0; level < 200; level++) 
//  {
//    MCP41010Write(level, red);
//    delay(20);
//  }
//
//     for (int level = 200; level > 0; level--) 
//  {
//    MCP41010Write(level, blue);
//    delay(20);
//  }  // wait a couple seconds
//  delay(100);
//  // Now mover potentiometer in other directions
//  
//  for (int level = 0; level < 200; level++) 
//  {
//    MCP41010Write(level, blue);
//    delay(20);
//  }
//
//     for (int level = 200; level > 0; level--) 
//  {
//    MCP41010Write(level, green);
//    delay(20); 
//  } // wait a couple seconds
//  delay(100);
//  // Now mover potentiometer in other directions
//  
//  for (int level = 0; level < 200; level++) 
//  {
//    MCP41010Write(level, green);
//    delay(20);
//  }

  for(int i = 0; i < 256; i ++){
    wheel(i);
  }
}

void writeColor(int r, int g, int b){
  MCP41010Write(r, red);
  MCP41010Write(g, green);
  MCP41010Write(b, blue);
}
 
void MCP41010Write(byte value, const int pin) 
{
  // Note that the integer vale passed to this subroutine
  // is cast to a byte
  
  digitalWrite(pin,LOW);
  SPI.transfer(B00010001); // This tells the chip to set the pot
  SPI.transfer(value);     // This tells it the pot position
  digitalWrite(pin,HIGH); 
}

void wheel(int pos){
  if (pos < 85){
    writeColor(255-pos*3,0,pos*3);
  }
  else if(pos < 170){
    pos -= 85;
    writeColor(0, pos * 3, 255 - pos * 3);
  }
  else{
    pos -= 170;
    writeColor(pos * 3, 255 - pos * 3, 0);

  }
}
