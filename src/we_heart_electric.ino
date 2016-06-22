#include "FastLED.h"
#include "ADC.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    23
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
// Leds + 1, last led is used as phanthom for the hearts array.
#define NUM_LEDS    39
CRGBArray<NUM_LEDS> leds;

CRGBSet heartsArray[] {CRGBSet(leds, 38, 38), CRGBSet(leds, 30, 37), CRGBSet(leds, 16, 29), CRGBSet(leds, 0, 15)};

ADC *adc = new ADC();

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  Serial.begin(9600);
  delay(3000); // 3 second delay for recovery
  adc->setReference(ADC_REF_1V2);
  adc->setResolution(16);
  adc->setAveraging(8);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
  heartGrow,
  rainbow,
  rainbowWithGlitter,
  confetti,
  sinelon,
  juggle,
  rainbowHeart,
  rainbowHeartWithGlitter,
  heartMove,
  mirrorHeart,
};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
  int anVal = adc->analogRead(A2);
  random16_add_entropy(anVal);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 10 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 90 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define CLEAR_PIXELS memset8(leds, 0, NUM_LEDS * sizeof(CRGB))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  /* gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns); */
  // Get random pattern
  gCurrentPatternNumber = (gCurrentPatternNumber + random16(ARRAY_SIZE(gPatterns) / 2)) % ARRAY_SIZE(gPatterns);
  /* gCurrentPatternNumber = random16(ARRAY_SIZE(gPatterns)); */
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void rainbowHeart() {
  // Heart that has a rainbow effect.
  CLEAR_PIXELS;
  CHSV hsv;
  hsv.hue = gHue;
  hsv.val = 240;
  hsv.sat = 255;
  for(int pos = 0; pos <= 3; pos++) {
    for(CRGB & pixel : heartsArray[pos]) { pixel = hsv; }
    hsv.hue += 60;
  }
}

void rainbowHeartWithGlitter() {
  rainbowHeart();
  addGlitter(80);
}

void heartMove() {
  // A Heart that moves from small to large and back.
  CLEAR_PIXELS;
  int pos = scale8(triwave8(beat8(25)), 2) + 1;
  CHSV hsv = CHSV( gHue, 240, 255);
  for(CRGB & pixel : heartsArray[pos]) { pixel = hsv; }
};


void heartGrow() {
  // Growing heart with 16th of a fraction.
  CLEAR_PIXELS;
  uint8_t pos16 = scale8(triwave8(beat8(25)), 4 * 16);
  uint8_t pos = pos16 / 16;
  uint8_t frac = pos16 & 0X0F;
  uint8_t mainPixelBrightness = frac * 16;
  uint8_t bright;
  for(int i = 0; i <= 3; i++) {
    if(i == pos) {
      bright = mainPixelBrightness;
    } else if (i < pos) {
      bright = 255;
    } else {
      bright = 0;
    }
    heartsArray[i] = CHSV(gHue, 240, bright);
  }
}

void beatDetected() {
  static uint16_t avgVolume = 0;
  // FF Transform
}

void mirrorHeart() {
  // Herats growing in size in mirror from bottom.
  CLEAR_PIXELS;
  uint8_t beat = 10;
  uint8_t posHeart16 = scale16(beat16(beat), 3 * 24);
  uint8_t posHeart = posHeart16 / 24 + 1;
  if(posHeart > ARRAY_SIZE(heartsArray) - 1) {
    posHeart = ARRAY_SIZE(heartsArray) - 1;
  }
  CRGBSet heart = heartsArray[posHeart];
  uint8_t fracHeart = (posHeart16 & 0X0F) * 16;
  // 22 leds to turn on, we want a fraction again so 22 * 16)
  uint16_t pos16 = scale8(fracHeart, (heart.size() / 2 + 1) * 16);
  // The last led to turn on.
  uint8_t pos = pos16 / 16;
  // Now get which led this is in the heart matrix.
  uint8_t frac = pos16 & 0X0F;
  uint8_t mainPixelBrightness = frac * 16;
  uint8_t bright;
  for(int i = 0; i < heart.size() / 2 + 1; i++) {
    if(i < pos) {
      bright = 255;
    } else if(i == pos) {
      bright = mainPixelBrightness;
    } else {
      bright = 0;
    }
    heart[i] = CHSV(gHue, 240, bright);
    uint8_t op = heart.size() - i;
    if((op < heart.size()) && (op > heart.size() / 2)) {
      heart[op] = heart[i];
    }
  }
}
