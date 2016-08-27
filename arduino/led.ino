#include <Adafruit_NeoPixel.h> //https://github.com/adafruit/Adafruit_NeoPixel
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2

// Parameter 1 = number of pixels in _strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel _strip = Adafruit_NeoPixel(12+7, PIN, NEO_RGBW + NEO_KHZ800);

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

Script_Info _script[4];

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
  
  _strip.begin();
  _strip.show(); // Initialize all pixels to 'off'
}

static void _led_fill(uint8_t start, uint8_t len, uint32_t color)
{
  for (uint8_t i=start; i < start+len; i++) {
    _strip.setPixelColor(i, color);    //turn every third pixel on
  }
}

static void _put_table(uint32_t * t, uint8_t start, uint8_t len, uint8_t offset)
{
  uint8_t index;
  for (uint8_t i=0; i < len; i++) {
    index = i + offset;
    if( index >= len) {
      index -= len;  
    }
    _strip.setPixelColor(start+i, t[index]);    //turn every third pixel on
  }
}

static void _script_reset(uint8_t script_num)
{
  _led_fill(_script[script_num].start_pos,_script[script_num].len, 0);       
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
   _strip.Color(0,100,0,0)
  ,_strip.Color(100,0,0,0)
  ,_strip.Color(0,0,100,0)
  ,_strip.Color(0,0,0,100)
  ,_strip.Color(0,0,0,20)
  
  ,_strip.Color(0,0,0,15)
  ,_strip.Color(0,0,0,15)
  ,_strip.Color(0,0,0,10)
  ,_strip.Color(0,0,0,10)
  ,_strip.Color(0,0,0,5)

  ,_strip.Color(0,0,0,5)
  ,_strip.Color(0,0,0,5)
};

// G R B W
static const uint32_t _color_pattern_B[6] = {
   _strip.Color(0,100,10,0)
  ,_strip.Color(0,50,10,0)
  ,_strip.Color(0,0,0,10)
  ,_strip.Color(0,0,0,10)  
  ,_strip.Color(0,0,0,10)
  
  ,_strip.Color(0,0,0,10)
};

void _pattern_0_1_update()
{
  if( _script[0].step == 0 ) { _script_reset(0); return; }

  if( _script[0].step % 2 ) {
    _put_table( _color_pattern_A, _script[0].start_pos, _script[0].len, (_script[0].step-1)/2);
    _strip.show();
    _script[0].step++;
  } else {
    _script_time_update(0, 1);
  }
}

void _pattern_0_2_update()
{
  if( _script[0].step == 0 ) { _script_reset(0); return; }

  if( _script[0].step % 2 ) {
    _put_table( _color_pattern_A, _script[0].start_pos, _script[0].len, 12- ((_script[0].step-1)/2));
    _strip.show();
    _script[0].step++;
  } else {
    _script_time_update(0,1); 
  }  
}

static uint8_t _color_buf[12];
static uint8_t _dir_buf[12];
void _pattern_0_3_update()
{
  uint8_t i;
  uint8_t sum=0;
  if( _script[0].step == 0 ) { 
    _script_reset(0); 
    for(i=0; i<_script[0].len; i++) {
      _color_buf[i] = sum - (sum%10); 
      sum += i; 
      _dir_buf[i] = 1;
      _strip.setPixelColor(i,_strip.Color(0,_color_buf[i],_color_buf[i]));
    }
    _strip.show();
    return; 
  }

  if( _script[0].step % 2 ) {
    for(i=0; i<_script[0].len; i++) {
      if(_dir_buf[i]) {
        if( _color_buf[i] < 120 ) { _color_buf[i] += 10; }
        else                      { _color_buf[i] -= 10; _dir_buf[i] = 0; }
      } else {
        if( _color_buf[i] > 10 ) { _color_buf[i] -= 10; }
        else                     { _color_buf[i] += 10; _dir_buf[i] = 1; }
      }
      _strip.setPixelColor(i,_strip.Color(0,_color_buf[i],_color_buf[i]));
    }
    _strip.show();
    _script[0].step++;
  } else {
    _script_time_update(0,3); 
  }  
}

void _pattern_1_1_update()
{
  if( _script[1].step == 0 ) { _script_reset(1); return; }

  if( _script[1].step % 2 ) {
    _strip.setPixelColor(_script[1].start_pos, _strip.Color(0,_script[1].step*10,0,0));
    _put_table( _color_pattern_B, _script[1].start_pos+1, _script[1].len-1, (_script[1].step-1)/2);
    _strip.show();
    _script[1].step++;
  } else {
    _script_time_update(1,10); 
  }  
}

void _pattern_1_2_update()
{
  if( _script[1].step == 0 ) { _script_reset(1); return; }

  if( _script[1].step % 2 ) {
    _strip.setPixelColor(_script[1].start_pos, _strip.Color(0,_script[1].step*10,0,0));
    _put_table( _color_pattern_B, _script[1].start_pos+1, _script[1].len-1, (_script[1].len-1)-((_script[1].step-1)/2));
    _strip.show();
    _script[1].step++;
  } else {
    _script_time_update(1,10);
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
    case 3:
      _pattern_0_3_update();
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
  _led_fill(0,_strip.numPixels(), 0);
  _strip.show();
  _script[0].index = 0;
  _script[1].index = 0;
  _script[2].index = 0;
  _script[3].index = 0;
  
}

