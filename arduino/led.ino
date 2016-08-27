#include <Adafruit_NeoPixel.h> //https://github.com/adafruit/Adafruit_NeoPixel
#ifdef __AVR__
  #include <avr/power.h>
#endif


// Parameter 1 = number of pixels in _led_top
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel _led_top = Adafruit_NeoPixel(12+7, 2, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel _led_btm = Adafruit_NeoPixel(12  , 3, NEO_RGB  + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
typedef struct {
  uint8_t start_pos;
  uint8_t len;
  uint8_t index;
  uint8_t step;
  uint8_t time;
  uint8_t count;
  uint8_t target_count;
} Script_Info;

Script_Info _script[3];

void led_setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  _script[0].start_pos = 0;
  _script[0].len = 12;
  _script[0].target_count = 0;

  _script[1].start_pos = 12;
  _script[1].len = 7;
  _script[1].target_count = 0;

  _script[2].start_pos = 0;
  _script[2].len = 12;
  _script[2].target_count = 0;
  
  _led_top.begin();
  _led_top.show(); // Initialize all pixels to 'off'
  _led_btm.begin();
  _led_btm.show(); // Initialize all pixels to 'off'
}

static void _led_fill_top(uint8_t start, uint8_t len, uint32_t color)
{
  for (uint8_t i=start; i < start+len; i++) {
    _led_top.setPixelColor(i, color);    //turn every third pixel on
  }
}


static void _led_fill_btm(uint8_t start, uint8_t len, uint32_t color)
{
  for (uint8_t i=start; i < start+len; i++) {
    _led_btm.setPixelColor(i, color);    //turn every third pixel on
  }
}

static void _put_table_top(uint32_t * t, uint8_t start, uint8_t len, uint8_t offset)
{
  uint8_t index;
  for (uint8_t i=0; i < len; i++) {
    index = i + offset;
    if( index >= len) {
      index -= len;  
    }
    _led_top.setPixelColor(start+i, t[index]);    //turn every third pixel on
  }
}

static void _put_table_btm(uint32_t * t, uint8_t start, uint8_t len, uint8_t offset)
{
  uint8_t index;
  for (uint8_t i=0; i < len; i++) {
    index = i + offset;
    if( index >= len) {
      index -= len;  
    }
    _led_btm.setPixelColor(start+i, t[index]);    //turn every third pixel on
  }
}

static void _script_reset_top(uint8_t script_num)
{
  _led_fill_top(_script[script_num].start_pos,_script[script_num].len, 0);       
  _script[script_num].time = 0;
  _script[script_num].step = 1;
  _script[script_num].count = 0;
}

static void _script_reset_btm(uint8_t script_num)
{
  _led_fill_btm(_script[script_num].start_pos,_script[script_num].len, 0);       
  _script[script_num].time = 0;
  _script[script_num].step = 1;
  _script[script_num].count = 0;
}

static void _script_time_update(uint8_t script_num, uint8_t time_100ms)
{
  _script[script_num].time ++;
  if( _script[script_num].time >= time_100ms ) { // time
    _script[script_num].time = 0;  
    _script[script_num].step ++;
    if(_script[script_num].step >= (_script[script_num].len*2) ) { // string size
      _script[script_num].step = 1;  
      _script[script_num].count ++;        
    } 
  }   
}

// G R B W
static const uint32_t _color_pattern_A[12] = {
   _led_top.Color(0,100,0,0)
  ,_led_top.Color(100,0,0,0)
  ,_led_top.Color(0,0,100,0)
  ,_led_top.Color(0,0,0,100)
  ,_led_top.Color(0,0,0,20)
  
  ,_led_top.Color(0,0,0,15)
  ,_led_top.Color(0,0,0,15)
  ,_led_top.Color(0,0,0,10)
  ,_led_top.Color(0,0,0,10)
  ,_led_top.Color(0,0,0,5)

  ,_led_top.Color(0,0,0,5)
  ,_led_top.Color(0,0,0,5)
};

// G R B W
static const uint32_t _color_pattern_B[6] = {
   _led_top.Color(0,100,10,0)
  ,_led_top.Color(0,50,10,0)
  ,_led_top.Color(0,0,0,10)
  ,_led_top.Color(0,0,0,10)  
  ,_led_top.Color(0,0,0,10)
  
  ,_led_top.Color(0,0,0,10)
};

// R G B
static const uint32_t _color_pattern_B2[6] = {
   _led_top.Color(0,0,100)
  ,_led_top.Color(0,0,10)
  ,_led_top.Color(0,0,5)
  ,_led_top.Color(0,0,5)  
  ,_led_top.Color(0,0,5)
  
  ,_led_top.Color(0,0,5)
};

// G R B W
static const uint32_t _color_pattern_C[12] = {
   _led_top.Color(0,0,100,0)
  ,_led_top.Color(0,0,150,0)
  ,_led_top.Color(0,0,100,0)
  ,_led_top.Color(0,0,0,100)
  ,_led_top.Color(0,0,0,20)
  
  ,_led_top.Color(0,0,0,15)
  ,_led_top.Color(0,0,0,15)
  ,_led_top.Color(0,0,0,10)
  ,_led_top.Color(0,0,0,10)
  ,_led_top.Color(0,0,0,5)

  ,_led_top.Color(0,0,0,5)
  ,_led_top.Color(0,0,0,5)
};

// R G B
static const uint32_t _color_pattern_L_1[6] = {
   _led_top.Color(100,0,100)
  ,_led_top.Color(100,0,100)
  ,_led_top.Color(100,0,100)
  
  ,_led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)
};

// G R B W
static const uint32_t _color_pattern_L_2[6] = {
   _led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)
  
  ,_led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)  
  ,_led_top.Color(0,0,100)
};

// G R B W
static const uint32_t _color_pattern_R_1[6] = { 
   _led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)  
  ,_led_top.Color(0,0,100)
  
  ,_led_top.Color(100,0,100)
  ,_led_top.Color(100,0,100)
  ,_led_top.Color(100,0,100)
};

// G R B W
static const uint32_t _color_pattern_R_2[6] = {
   _led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)  
  ,_led_top.Color(0,0,100)
  
  ,_led_top.Color(0,0,100)
  ,_led_top.Color(0,0,100)  
  ,_led_top.Color(0,0,100)
};

void _pattern_0_1_update() // left
{
  if( _script[0].step == 0 ) { _script_reset_btm(0); return; }

  if( _script[0].step % 2 ) {
    if( (_script[0].step/2) % 2 ) {
      _put_table_btm( _color_pattern_L_1, _script[0].start_pos+6, 6, 0);
    } else {
      _put_table_btm( _color_pattern_L_2, _script[0].start_pos+6, 6, 0);
    }
    _put_table_btm( _color_pattern_B2, _script[0].start_pos, 6, (_script[0].step-1)/2);
    _led_btm.show();
    _script[0].step++;
  } else {
    _script_time_update(0, 7);
  }
}

void _pattern_0_2_update() // left
{
  if( _script[0].step == 0 ) { _script_reset_btm(0); return; }

  if( _script[0].step % 2 ) {
    if( (_script[0].step/2) % 2 ) {
      _put_table_btm( _color_pattern_R_1, _script[0].start_pos+6, 6, 0);
    } else {
      _put_table_btm( _color_pattern_R_2, _script[0].start_pos+6, 6, 0);
    }
    _put_table_btm( _color_pattern_B2, _script[0].start_pos, 6, 6- ((_script[0].step-1)/2));
    _led_btm.show();
    _script[0].step++;
  } else {
    _script_time_update(0, 7);
  }
}

void _pattern_1_1_update()
{
  if( _script[1].step == 0 ) { _script_reset_top(1); return; }

  if( _script[1].step % 2 ) {
    _led_top.setPixelColor(_script[1].start_pos, _led_top.Color(0,_script[1].step*10,0,0));
    _put_table_top( _color_pattern_B, _script[1].start_pos+1, _script[1].len-1, (_script[1].step-1)/2);
    _led_top.show();
    _script[1].step++;
  } else {
    _script_time_update(1,10); 
  }  
}

void _pattern_1_2_update()
{
  if( _script[1].step == 0 ) { _script_reset_top(1); return; }

  if( _script[1].step % 2 ) {
    _led_top.setPixelColor(_script[1].start_pos, _led_top.Color(0,_script[1].step*10,0,0));
    _put_table_top( _color_pattern_B, _script[1].start_pos+1, _script[1].len-1, (_script[1].len-1)-((_script[1].step-1)/2));
    _led_top.show();
    _script[1].step++;
  } else {
    _script_time_update(1,10);
  }  
}

void _pattern_2_1_update()
{
  if( _script[2].step == 0 ) { _script_reset_top(2); return; }

  if( _script[2].step % 2 ) {
    _put_table_top( _color_pattern_A, _script[2].start_pos, _script[2].len, (_script[2].step-1)/2);
    _led_top.show();
    _script[2].step++;
  } else {
    _script_time_update(2, 1);
  }
}

void _pattern_2_2_update()
{
  if( _script[2].step == 0 ) { _script_reset_top(2); return; }

  if( _script[0].step % 2 ) {
    _put_table_top( _color_pattern_A, _script[2].start_pos, _script[2].len, 12- ((_script[2].step-1)/2));
    _led_top.show();
    _script[2].step++;
  } else {
    _script_time_update(2,1); 
  }  
}

static uint8_t _color_buf[12];
static uint8_t _dir_buf[12];
void _pattern_2_3_update()
{
  uint8_t i;
  uint8_t sum=0;
  if( _script[2].step == 0 ) { 
    _script_reset_top(2); 
    for(i=0; i<_script[2].len; i++) {
      _color_buf[i] = sum - (sum%10); 
      sum += i; 
      _dir_buf[i] = 1;
      _led_top.setPixelColor(i,_led_top.Color(0,_color_buf[i],_color_buf[i]));
    }
    _led_top.show();
    return; 
  }

  if( _script[2].step % 2 ) {
    for(i=0; i<_script[2].len; i++) {
      if(_dir_buf[i]) {
        if( _color_buf[i] < 120 ) { _color_buf[i] += 10; }
        else                      { _color_buf[i] -= 10; _dir_buf[i] = 0; }
      } else {
        if( _color_buf[i] > 10 ) { _color_buf[i] -= 10; }
        else                     { _color_buf[i] += 10; _dir_buf[i] = 1; }
      }
      _led_top.setPixelColor(i,_led_top.Color(0,_color_buf[i],_color_buf[i]));
    }
    _led_top.show();
    _script[2].step++;
  } else {
    _script_time_update(2,3); 
  }  
}



void led_update_10ms() 
{
  switch( _script[0].index ) {
    case 0 :
      break;
    case 1 :       
      _pattern_0_1_update();
      break;
    case 2:
      _pattern_0_2_update();
      break;
    default:
      break;  
  }
  switch( _script[1].index ) {
    case 0 :
      break;
    case 1 :       
      _pattern_1_1_update();
      break;
    case 2:
      _pattern_1_2_update();
      break;
    default:
      break;  
  }   
  switch( _script[2].index ) {
    case 0 :
      break;
    case 1 :       
      _pattern_2_1_update();
      break;
    case 2:
      _pattern_2_2_update();
      break;
    case 3:
      _pattern_2_3_update();
      break;
    default:
      break;  
  }   
}

void set_led_pattern(uint8_t block_num, uint8_t index)
{
  _script[block_num].index = index;
  _script[block_num].step = 0;
  _script[block_num].time = 0;
  _script[block_num].count = 0;  
}

void led_all_off()
{
  _led_fill_top(0,_led_top.numPixels(), 0);
  _led_fill_btm(0,_led_btm.numPixels(), 0);
  _led_top.show();
  _led_btm.show();
  _script[0].index = 0;
  _script[1].index = 0;
  _script[2].index = 0;
  _script[3].index = 0;  
}

