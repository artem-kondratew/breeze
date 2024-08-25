#pragma once
#ifndef TOOLS_H
#define TOOLS_H

#include "FastLED.h"
#define NUM_LEDS 10  // Кол-во светодиодов в адресной ленте
#define ALED_PIN 2   // Пин адресной ленты

namespace tools {
    void blink(size_t cnt, size_t dt);
    void ledOn();
    void ledOff();

    CRGB leds[NUM_LEDS];
    uint8_t sectors_num = NUM_LEDS * 2;
    uint8_t sectors_ang = 360 / tools::sectors_num;
    uint8_t sector = 0;

    CHSV main_colour{0, 255, 100};
    CHSV sector_colour{127, 255, 255};

    void init_aled();
    void set_sector(uint8_t s){ tools::sector = s;};
    void set_sector_ang(uint8_t ang){ tools::sector = (ang + tools::sectors_ang/2) / tools::sectors_ang;};
    void set_aled_colour(uint8_t H);
    void update_aled();
}

void tools::blink(size_t cnt, size_t dt) {
    for (size_t i = 0; i < cnt; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(dt);
        digitalWrite(LED_BUILTIN, LOW);
        delay(dt);
    }
}


void tools::ledOn() {
    digitalWrite(LED_BUILTIN, HIGH);
}

void tools::ledOff() {
    digitalWrite(LED_BUILTIN, LOW);
}

void tools::init_aled(){
  FastLED.addLeds<WS2811, ALED_PIN, GRB>(tools::leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(10);
  for (int i = 0; i < NUM_LEDS; i++ ) leds[i] = CRGB(0, 0, 0);
  FastLED.show();
}

void tools::set_aled_colour(uint8_t H){
  // Изменить основной и вспомогательный цвет индикации
  tools::main_colour   = CHSV(H, 255, 100);
  tools::sector_colour = CHSV(H + 127, 255, 255);
}

void tools::update_aled(){
  // Показать навправление через адресную ленту
  for (int i = 0; i < NUM_LEDS; i++ ) leds[i] = tools::main_colour;

  leds[tools::sector / 2] = tools::sector_colour;

  if ( tools::sector % 2 == 0 ){
    if( tools::sector == 0 or tools::sector == tools::sectors_num){
      leds[0] = tools::sector_colour;
      leds[NUM_LEDS-1] = tools::sector_colour;
    }
    else leds[sector / 2 - 1] = tools::sector_colour;
  }
  FastLED.show();
}

#endif // TOOLS_H
