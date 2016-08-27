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
}

void loop() {
//  playSong(1);

  // put your main code here, to run repeatedly:
 if (Serial.available() > 0)  {
  bleSignal = Serial.read();
  switch(bleSignal) {
    case 49:  // '1' - start
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

