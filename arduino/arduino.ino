#include <Timer.h> //https://github.com/JChristensen/Timer

Timer timer;

const int buttonPin = 8;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

int bleSignal = 0;  // signal from bluetooth master
bool randomMove = true;

bool direction = false;   // true: left, false: right
bool spinStart = false;
bool goForward = false;
bool cupWaited = false;
unsigned long previousMillis = 0;   // for interval
long interval = 0;


void setup() {
  // put your setup code here, to run once:
  int i;
  for(i=4;i<=7;i++) {   // motor
    pinMode(i, OUTPUT);
  }
  pinMode(9, OUTPUT);   //buzzer
  pinMode(ledPin, OUTPUT);  //led indicator when singing a note
  Serial.begin(115200);      //Set Baud Rate
  Serial.println("App control initialized");

  led_setup();
  timer.every(10,event_10ms);
  button_setup();
  led_all_off();
}

void event_10ms()
{ 
  led_update_10ms();
}

void on_button_up()
{
  Serial.println("Up");
}

void on_button_down()
{
  Serial.println("Down");
}

void loop() {
  timer.update();

  buttonState = digitalRead(buttonPin);

  // put your main code here, to run repeatedly:
 if (Serial.available() > 0)  {
  bleSignal = Serial.read();

  switch(bleSignal) {
    case '1':   // start
    playSong(3);
//    set_led_pattern(0,1);
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
    led_all_off();
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
    set_led_pattern(0,1);
    break;
    
    case '8':
    set_led_pattern(0,2);
    break;
   
    case '&':
    set_led_pattern(1,1);
    break;
    
    case '*':
    set_led_pattern(1,2);
    break;

    case 'u':
    set_led_pattern(2,1);
    break;
    
    case 'i':
    set_led_pattern(2,2);
    break;
    
    case 'o':
    set_led_pattern(2,3);
    break;
    case 'x':
    led_all_off();
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
    stop();
    Serial.println("arrived");
    cupWaited = true;
  }

Serial.println(buttonState);
  if ( cupWaited == true && buttonState == HIGH ) {
    cupWaited = false;
    Serial.println("hold");
    led_all_off();
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

