#include "FastLED.h"

#define NUM_LEDS 10  // Кол-во светодиодов в адресной ленте
#define ALED_PIN 2   // Пин адресной ленты

CRGB leds[NUM_LEDS];

uint8_t sectors_num = NUM_LEDS * 2;
uint8_t sectors_ang = 360 / sectors_num;

void init_aled(){
  FastLED.addLeds<WS2811, ALED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(10);
  set_rgb(0,0,0);
}

void set_rgb(  uint8_t r, uint8_t g, uint8_t b){
  // Установка нужного цвета адресной ленты
  for (int i = 0; i < NUM_LEDS; i++ ) leds[i] = CRGB(r, g, b);
  FastLED.show();
}

void set_rgb(CRGB rgb){
  // Установка нужного цвета адресной ленты
  for (int i = 0; i < NUM_LEDS; i++ ) leds[i] = rgb;
  FastLED.show();
}

void set_rgb_sector(CRGB rgb_main, CRGB rgb_sector, int ang){
  // Показать навправление через адресную ленту
  for (int i = 0; i < NUM_LEDS; i++ ) leds[i] = rgb_main;

  uint8_t sector = (ang + 9) / sectors_ang;

  leds[sector / 2] = rgb_sector;
  if(sector % 2 == 0){
    if(sector == 0 or sector == sectors_num){
      leds[0] = rgb_sector;
      leds[NUM_LEDS-1] = rgb_sector;
    }
    else leds[sector / 2 - 1] = rgb_sector;
    
  }
  FastLED.show();
}

void setup() {
  Serial.begin(9600);
  init_aled();
}

int i = 255;
CHSV main_color{i, 255, 125};
CHSV sector_color{i + 127, 255, 255}; 
 
void loop() {
  for (int i = 0; i < 360; i += 1){
    Serial.println(i);
    set_rgb_sector(main_color, sector_color, i);
    delay(10);
  }
}
