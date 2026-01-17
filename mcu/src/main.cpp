#include <LiquidCrystal.h> 
#include <Arduino.h>
//TODO improve button reading logic to avoid multiple detections on single press
//TODO change print and remove delay to quicken serial monitor
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 
int BUTTONPIN = A0;
int LEFT = 480;
int RIGHT = 0;
int UP = 131;
int DOWN = 307;
void setup(){
 lcd.begin(16,2);
Serial.begin(9600);

}                        
void loop(){
lcd.setCursor(0,0);
int pressed = analogRead(BUTTONPIN);

if(pressed == LEFT){
  lcd.print("LEFT WON");
  Serial.println(1);
  
}
if(pressed == RIGHT){
  lcd.print("RIGHT WON");
  Serial.println(2);
}
if (pressed == UP){
  lcd.print("UP WON");
  Serial.println(3);
}
if (pressed == DOWN){
  lcd.print("DOWN WON");
  Serial.println(4);
}

delay(10);
lcd.clear();
}