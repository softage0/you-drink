int bleSignal = 0;

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
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}  
void back_off (char a,char b)          //Move backward
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);   
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}
void turn_L (char a,char b)             //Turn Left
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}
void turn_R (char a,char b)             //Turn Right
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}


void setup() {
  // put your setup code here, to run once:
  int i;
  for(i=4;i<=7;i++)
    pinMode(i, OUTPUT);  
  Serial.begin(115200);      //Set Baud Rate
  Serial.println("App control initialized");
}

void loop() {
  // put your main code here, to run repeatedly:
 if (Serial.available() > 0)  {
  bleSignal = Serial.read();
  switch(bleSignal) {
    // start
    case 49:  // '1'
    Serial.println("proto start and spin by random seconds and go front");
    if (random(2) == 0) {
      turn_L(255, 255);
    } else {
      turn_R(255, 255);
    }

    delay(random(3000, 10000));
    advance (255,255);    
    break;
    // end
    case 50:  // '2'
    Serial.println("proto stop");
    stop ();
    break;
    // start
    case 51:  // '3'
    Serial.println("go forward");
    advance (255,255);
    break;
    // start
    case 52:   // '4'
    Serial.println("back off");
    back_off (255,255);
    break;
    default:
    Serial.println(bleSignal);
  }
 }
}

