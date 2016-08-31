//Standard PWM DC control
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;    //M1 Direction Control
int M2 = 7;    //M1 Direction Control

///For previous Romeo, please use these pins.
//int E1 = 6;     //M1 Speed Control
//int E2 = 9;     //M2 Speed Control
//int M1 = 7;    //M1 Direction Control
//int M2 = 8;    //M1 Direction Control


void stop(void)                    //Stop
{
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);      
}   
void advance(char a,char b)          //Move forward
{
  analogWrite (E1,a);      //PWM Speed Control
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}  
void back_off (char a,char b)          //Move backward
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);   
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}
void spin_L (char a,char b)             //Turn Left
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}
void spin_R (char a,char b)             //Turn Right
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}

void reverseSpin() {
      if (direction) {
      spin_R(SPIN_SPEED, SPIN_SPEED);
      set_led_pattern(0,2);
      set_led_pattern(1,2);
      set_led_pattern(2,2);
//      Serial.println("_spinRight");
    } else {
      spin_L(SPIN_SPEED, SPIN_SPEED);
      set_led_pattern(0,1);
      set_led_pattern(1,1);
      set_led_pattern(2,1);
//      Serial.println("_spinLeft");
    }
    direction = !direction;
//    Serial.println("_reverseSpin");
}

void turnLeft() {
    advance(TURN_INNER_TIRE_SPEED, FORWARD_SPEED);
    delay(500);
    advance(FORWARD_SPEED, FORWARD_SPEED);
//    Serial.println("_turnLeft");
}

void turnRight() {
    advance(FORWARD_SPEED, TURN_INNER_TIRE_SPEED);
    delay(500);
    advance(FORWARD_SPEED, FORWARD_SPEED);
//    Serial.println("_turnRight");
}


