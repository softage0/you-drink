#define PIN 8
static uint8_t _pin_state;
static uint8_t _db_count;

void button_setup()
{
  pinMode(PIN, INPUT_PULLUP);  
  _pin_state = digitalRead( PIN );
}

void button_update_10ms()
{
  uint8_t pin;
  pin = digitalRead( PIN );

  if( pin != _pin_state ) {
    _db_count = 5;  
  } else {
    if( _db_count ) {
      _db_count --;
      if( _db_count == 0 ) {
        if( pin == HIGH ) { on_button_up(); }
        else              { on_button_down(); }  
      }
    }  
  }
  _pin_state = pin;
  
}
