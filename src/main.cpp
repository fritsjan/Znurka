/*  For Znurka gun, running on a seeed xiao samd21  */
/*  copyright (c) 2024 productbakery                */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SPI.h>
#include <Bounce2.h>

// PINS
#define LED_BUILTIN   13
#define SDA           4
#define SCL           5
#define LED_PIN       10
#define BTN1_PIN      9
#define BTN2_PIN      8
#define POTA_PIN     1
#define POTB_PIN     2
#define POTC_PIN     3
//#define POTD_PIN     6

// How many LEDS
#define LED_COUNT  16

// Segments of ledstrip
const uint8_t leds_SCORE[] = {0,1,2,3,4,5,6,7};
//const uint8_t leds_VU[] = {8,9,10,11,12,13,14,15};
const uint8_t leds_VU[] = {15,14,13,12,11,10,9,8};
const uint8_t leds_FIRE[] = {12,11,13,10,14,9,15,8};

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

// BUTTONS
Bounce btnA = Bounce();
Bounce btnB = Bounce();

// variables for potmeters
int potBrightness = 200;
int potSpeed = 0;
int potScream = 0;


// enum state
enum stateOptions {
  INIT,
  IDLE,
  VU_METER,
  FIRE,
  ADD_SCORE,
  REM_SCORE
};

uint8_t state = INIT;
uint8_t score_counter = 0;

struct colorObject{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t white;
};

const colorObject cGreen = {0,255,0,0};
const colorObject cYellow = {150,150,0,0};
const colorObject cOrange = {255,80,0,0};
const colorObject cRed = {255,0,0,0};
const colorObject cUV1 = {0,120,50,0};
const colorObject cUV2 = {0,120,80,0};
const colorObject cUV3 = {0,120,110,0};
const colorObject cWhite= {10,10,10,200};

//colorObject colorScore[8] = {cGreen, cGreen, cYellow, cYellow, cOrange, cOrange, cRed, cRed}; // score colors
// colorscore inverse order
colorObject colorScore[8] = {cRed, cRed, cOrange, cOrange, cYellow, cYellow, cGreen, cGreen}; // score colors
colorObject colorVU[8] = {cUV1,cUV1,cUV2,cUV2,cUV2,cUV2,cUV3,cUV3}; // vu meter colors
colorObject colorFire[8] = {cRed,cRed,cOrange,cOrange,cWhite,cWhite,cWhite,cWhite}; // fire colors

//array of 100 items imitating screaming sound in 0-8 range
const uint8_t soundLength = 100;
uint8_t screamArray[soundLength] = 
{0,1,2,3,4,5,6,7,8,7,6,5,4,4,4,5,5,6,7,8,8,8,5,5,4,3,2,1,3,4,5,6,7,7,8,8,7,6,5,4,4,6,7,8,8,8,6,5,4,2,1,0,2,3,4,5,6,7,8,8,7,6,5,6,7,8,7,6,5,4,5,6,7,8,8,7,6,5,4,3,4,5,6,7,8,8,6,4,5,6,7,8,8,7,6,5,4,3,2,1};
uint8_t soundArray[soundLength] =
{0,1,2,3,4,4,4,5,6,5,4,3,2,3,4,5,6,7,6,5,4,3,3,3,4,5,6,5,4,3,3,3,2,2,1,1,0,0,1,2,3,4,5,4,3,2,3,4,5,6,6,6,5,4,3,2,1,1,1,1,2,3,4,5,4,3,4,5,6,6,6,5,4,3,2,1,2,3,4,4,4,5,5,5,4,5,6,7,6,5,4,3,2,3,4,5,4,3,2,1};
uint8_t initArray[20] =
{0,1,2,3,4,5,6,7,8,8,8,7,6,5,4,3,2,1,0,0};
uint8_t fireArray[30] = 
{0,0,0,1,2,3,4,5,6,7,8,8,8,8,8,0,0,0,1,2,3,4,5,6,7,8,8,8,8,8};

uint8_t soundPos = 0;

void setScore(uint8_t score){
  if (score > 8){ score = 8; } 
  if (score < 0){ score = 0; }
  score_counter = score;

  for (int i = 0; i < 8; i++){
    if (i < score_counter){
      strip.setPixelColor(leds_SCORE[i], strip.Color(colorScore[i].red, colorScore[i].green,colorScore[i].blue,colorScore[i].white));
    } else {
      strip.setPixelColor(leds_SCORE[i], strip.Color(0, 0, 0, 0));
    }
  }
  strip.show();
}

void blinkScore(uint8_t score, uint8_t times, uint8_t wait){
  //blink last pixel
  for (int i = 0; i < times; i++){
    strip.setPixelColor(leds_SCORE[score-1], strip.Color(0, 0, 0, 0));
    strip.show();
    delay(wait);
    strip.setPixelColor(leds_SCORE[score-1], strip.Color(colorScore[score-1].red, colorScore[score-1].green,colorScore[score-1].blue,colorScore[score-1].white));
    strip.show();
    delay(wait);
  }

}

void setVU(uint8_t vu){
  if (vu > 8){ vu = 8; }
  if (vu < 0){ vu = 0; }
  for (int i = 0; i < 8; i++){
    if (i < vu){
      strip.setPixelColor(leds_VU[i], strip.Color(colorVU[i].red, colorVU[i].green,colorVU[i].blue,colorVU[i].white));
    } else {
      strip.setPixelColor(leds_VU[i], strip.Color(0, 0, 0, 0));
    }
  }
  strip.show();
}

void setFire(uint8_t vu){
  if (vu > 8){ vu = 8; }
  if (vu < 0){ vu = 0; }
  for (int i = 0; i < 8; i++){
    if (i < vu){
      strip.setPixelColor(leds_FIRE[i], strip.Color(colorFire[i].red, colorFire[i].green,colorFire[i].blue,colorFire[i].white));
    } else {
      strip.setPixelColor(leds_FIRE[i], strip.Color(0, 0, 0, 0));
    }
  }
  strip.show();
}

void setup() {
  strip.begin();
  strip.clear();
  strip.show();

  // read brightness potmeter
  potBrightness = map(analogRead(POTA_PIN), 0, 1023, 10, 255);
  strip.setBrightness(potBrightness);

  // setup buttons
  btnA.attach(BTN1_PIN, INPUT_PULLUP);
  btnA.interval(50); // debounce interval in ms
  btnB.attach(BTN2_PIN, INPUT_PULLUP);
  btnB.interval(50); // debounce interval in ms
}

unsigned long prevMillis = millis();

void loop() {

  unsigned long currentMillis = millis();  //  Update current time

  // Button action
  btnA.update(); //check button
  if ( btnA.changed() ) {
    int buttonState = btnA.read();
    if( buttonState == LOW ) { //button pressed
      if (state == IDLE) {
        soundPos = 0;
        state = VU_METER;
      } else if (state == VU_METER) {
        soundPos = 0;
        state = FIRE;
      }
    }
  }

  btnB.update(); //check button
  if ( btnB.changed() ) {
    int buttonState = btnB.read();
    if( buttonState == LOW ) { //button pressed
      state = REM_SCORE;
    }
  }
    
  // read potmeter and map to speed of leds
  potSpeed = map(analogRead(POTB_PIN), 0, 1000, 2, 200);
  potScream = map(analogRead(POTC_PIN), 0, 1000, 2, 200);
  
  switch(state) {
    case INIT:
      if(currentMillis - prevMillis > 100) {
        prevMillis = currentMillis;
        setVU(initArray[soundPos]);
        setScore(initArray[soundPos]);  
        soundPos ++;
        if (soundPos > 19){
          soundPos = 0;
          score_counter = 1;
          setScore(score_counter);
          state = IDLE;
        }
      }
      break;
    case IDLE:
      // ambient sound
      if(currentMillis - prevMillis > potSpeed) {
        prevMillis = currentMillis;
        setVU(soundArray[soundPos]);
        soundPos ++;
        if (soundPos > soundLength-1){
          soundPos = 0;
        }
      }

      break;
    case VU_METER: 
      //scream
      if(currentMillis - prevMillis > potScream) {
        prevMillis = currentMillis;
        setVU(screamArray[soundPos]);
        soundPos ++;
        if (soundPos > soundLength-1){
          soundPos = 0;
        }
      }
      break;
    case FIRE:
      if(currentMillis - prevMillis > potScream) {
          prevMillis = currentMillis;
          setFire(fireArray[soundPos]);
          soundPos ++;
          if (soundPos > 29){
            state = ADD_SCORE;
          }
        }    
      break;
    case ADD_SCORE: 
      //blink score
      score_counter++;
      setScore(score_counter);
      blinkScore(score_counter, 5, 100);
      state = IDLE;     
      break;
    case REM_SCORE:
      score_counter--;
      setScore(score_counter);
      state = IDLE;
      break;
    default:
      break;
  }

}