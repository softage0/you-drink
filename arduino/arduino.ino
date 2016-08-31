#include <Timer.h> //https://github.com/JChristensen/Timer

#define MOTOR_PIN_FIRST 4
#define MOTOR_PIN_LAST 7
#define BUTTON_PIN 8    // the number of the cup-pressed button pin
#define BUZZER_PIN 9
#define LED_PIN 13      // the number of the LED pin

#define SPIN_SPEED 120
#define FORWARD_SPEED 120
#define TURN_INNER_TIRE_SPEED 60
#define BACKOFF_SPEED 80

int bleSignal = 0;  // signal from bluetooth master
bool randomMove = false;

bool direction = false;   // true: left, false: right
bool spinStart = false;
bool goForward = false;
bool cupWaited = false;
unsigned long previousMillis = 0;   // for interval
long interval = 0;

Timer timer;


void setup() {
  // put your setup code here, to run once:
  int i;
  for (i = MOTOR_PIN_FIRST; i <= MOTOR_PIN_LAST; i++) { // motor
    pinMode(i, OUTPUT);
  }
  pinMode(BUZZER_PIN, OUTPUT);   //buzzer
  pinMode(LED_PIN, OUTPUT);  //led indicator when singing a note
  Serial.begin(115200);      //Set Baud Rate
  Serial.println("App control initialized");

  led_setup();
  timer.every(10, event_10ms);
  button_setup();
  led_all_off();
}

void event_10ms()
{
  led_update_10ms();
  button_update_10ms();
}

void on_button_up()
{
//  Serial.println("Up");
  if ( cupWaited == true ) {
    cupWaited = false;
    led_all_off();
    Serial.println("hold");
  }
}

void on_button_down()
{
//  Serial.println("Down");
}

void loop() {
  timer.update();

  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)  {
    bleSignal = Serial.read();

    switch (bleSignal) {
      case '1':   // start
        if (cupWaited) break;
        playSong(3);
        //    set_led_pattern(0,1);
        direction = random(2);
        if (direction) {
          spin_L(SPIN_SPEED, SPIN_SPEED);
          set_led_pattern(0, 1);
          set_led_pattern(1, 1);
          set_led_pattern(2, 1);
          //      Serial.println("_spinLeft");
        } else {
          spin_R(SPIN_SPEED, SPIN_SPEED);
          set_led_pattern(0, 2);
          set_led_pattern(1, 2);
          set_led_pattern(1, 2);
          //      Serial.println("_spinRight");
        }

        interval = random(3000, 10000);
        previousMillis = millis();
        spinStart = true;
        Serial.println("startSpin");
        break;

      case '2':   // stop
        spinStart = false;
        goForward = false;
        cupWaited = false;
        stop();
        led_all_off();
        Serial.println("gameStop");
        break;

      case '3':   // reverse during spinning
        if (cupWaited) break;
        reverseSpin();
        break;

      case '4':   // turn left when going forward
        if (cupWaited) break;
        turnLeft();
        break;

      case '5':   // turn right when going forward
        if (cupWaited) break;
        turnRight();
        break;

      case '7':
        set_led_pattern(0, 1);
        break;

      case '8':
        set_led_pattern(0, 2);
        break;

      case '9':
        set_led_pattern(0, 3);
        break;

      case 'a':
        randomMove = true;
        break;

      case 'b':
        randomMove = false;
        break;

      case '&':
        set_led_pattern(1, 1);
        break;

      case '*':
        set_led_pattern(1, 2);
        break;

      case '(':
        set_led_pattern(1, 3);
        break;

      case 'u':
        set_led_pattern(2, 1);
        break;

      case 'i':
        set_led_pattern(2, 2);
        break;

      case 'o':
        set_led_pattern(2, 3);
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
  if (spinStart == true && millis() - previousMillis > interval) {
    advance(FORWARD_SPEED, FORWARD_SPEED);
    spinStart = false;

    interval = 3000;
    previousMillis = millis();
    goForward = true;
    Serial.println("goForward");
    set_led_pattern(0, 3);
    set_led_pattern(1, 3);
    set_led_pattern(2, 3);

  }

  if (goForward == true && millis() - previousMillis > interval) {
    goForward = false;
    stop();
    set_led_pattern(0, 1);
    set_led_pattern(1, 2);
    set_led_pattern(2, 1);
    Serial.println("arrived");
    cupWaited = true;
  }

  //
  // random move functions
  //
  if (randomMove) {
    if (spinStart && !((millis() - previousMillis) % 1000) && !random(3)) {
      reverseSpin();
    }
    if (goForward && !((millis() - previousMillis) % 200) && !random(3)) {
      switch (random(3)) {
        case 0:
          turnLeft();
          break;
        case 1:
          turnRight();
          break;
        case 2:
          back_off(BACKOFF_SPEED, BACKOFF_SPEED);
          delay(500);
          advance(FORWARD_SPEED, FORWARD_SPEED);
          break;
      }
    }
  }
}

