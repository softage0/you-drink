#include <Timer.h> //https://github.com/JChristensen/Timer

Timer timer;

int bleSignal = 0;  // signal from bluetooth master
bool randomMove = true;

bool direction = false;   // true: left, false: right
bool spinStart = false;
bool goForward = false;
unsigned long previousMillis = 0;   // for interval
long interval = 0;


void setup() {
  // put your setup code here, to run once:
  int i;
  for(i=4;i<=7;i++) {   // motor
    pinMode(i, OUTPUT);
  }
  pinMode(3, OUTPUT);   //buzzer
  pinMode(13, OUTPUT);  //led indicator when singing a note

  Serial.begin(115200);      //Set Baud Rate
  Serial.println("App control initialized");

  led_setup();
  timer.every(10,event_10ms);
}

void event_10ms()
{ 
  led_update_10ms();
}

void loop() {
  timer.update();

  // put your main code here, to run repeatedly:
 if (Serial.available() > 0)  {
  bleSignal = Serial.read();
  switch(bleSignal) {
    case '1':   // start
    playSong(3);
    set_led_pattern(0,1);
    direction = random(2);
    if (direction) {
      turn_L(255, 255);
//      Serial.println("_spinLeft");
    } else {
      turn_R(255, 255);
//      Serial.println("_spinRight");
    }

    interval = random(3000, 10000);
    previousMillis = millis();
    spinStart = true;
    Serial.println("startSpin");
    break;

    case '2':   // stop
    stop();
    Serial.println("gameStop");
    break;

    case '3':   // reverse during spinning
    reverseSpin();
    break;

    case '4':   // turn left when going forward
    turnLeft();
    break;

    case '5':   // turn right when going forward
    turnRight();
    break;

    case '7':
    Serial.println('7!');
    set_led_pattern(0,1);
    break;
    
    case '8':
    set_led_pattern(0,2);
    break;
    
    case '9':
    set_led_pattern(0,3);
    break;

    case '&':
    set_led_pattern(1,1);
    break;
    
    case '*':
    set_led_pattern(1,2);
    break;
    
    default:
    Serial.println(bleSignal);
  }
 }

  //
  // interval functions
  //
  if ( spinStart == true && millis() - previousMillis > interval ) {
    advance(100, 100);
    spinStart = false;
  
    interval = 10000;
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

