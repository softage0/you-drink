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

int bleSignal = 0;  // signal from bluetooth master
bool randomMove = true;

bool direction = false;   // true: left, false: right
bool spinStart = false;
bool goForward = false;
unsigned long previousMillis = 0;   // for interval
long interval = 0; 


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

void reverseSpin() {
      if (direction) {
      turn_R(255, 255);
      Serial.println("_spinRight");
    } else {
      turn_L(255, 255);
      Serial.println("_spinLeft");
    }
    direction = !direction;
    Serial.println("_reverseSpin");
}

void turnLeft() {
    advance(50, 100);
    delay(500);
    advance(50, 50);
    Serial.println("_turnLeft");
}

void turnRight() {
    advance(100, 50);
    delay(100);
    advance(50, 50);
    Serial.println("_turnRight");
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
    case 49:  // '1' - start
    direction = random(2);
    if (direction) {
      turn_L(255, 255);
      Serial.println("_spinLeft");
    } else {
      turn_R(255, 255);
      Serial.println("_spinRight");
    }

    interval = random(3000, 10000);
    previousMillis = millis();
    spinStart = true;
    Serial.println("spinStart");
    break;

    case 50:  // '2' - stop
    stop();
    Serial.println("gameStop");
    break;

    case 51:  // '3' - reverse during spinning
    reverseSpin();
    break;

    case 52:   // '4' - turn left when going forward
    turnLeft();
    break;

    case 53:   // '5' - turn right when going forward
    turnRight();
    break;

    default:
    Serial.println(bleSignal);
  }
 }

  //
  // interval functions
  //
  if ( spinStart == true && millis() - previousMillis > interval ) {
    advance(50, 50);
    spinStart = false;
  
    interval = 3000;
    previousMillis = millis();
    goForward = true;
    Serial.println("goForward");
  }

  if ( goForward == true && millis() - previousMillis > interval ) {
    goForward = false;
    Serial.println("arrived");
  }

  //
  // random move functions
  //
  if ( randomMove ) {
    if ( spinStart == true && !(millis() - previousMillis) % 500 && !random(3) ) {
      reverseSpin();
    }
    if ( goForward == true && !(millis() - previousMillis) % 100 && !random(3) ) {
      if (random(2)) {
        turnLeft();
      } else {
        turnRight();
      }
    } 
  }
}

