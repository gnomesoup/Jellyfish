#include <FastLED.h>

#define LED_PIN            6
#define NUM_LEDS           60
#define BRIGHTNESS         64
#define LED_TYPE           WS2811
#define COLOR_ORDER        RGB
#define FRAMES_PER_SECOND  60
CRGB leds[NUM_LEDS];
CRGB setupLeds[NUM_LEDS];

boolean notzero = true;
uint8_t paletteIndex = 0;


/* typedef void (*SimplePatternList[])(); */
/* SimplePatternList gPatterns = { rainbow, rainbowWithGlitter }; */

const byte vibration = A0; // vibration sensor
const int tapLevel = 20;
unsigned long tapMillis;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void setup() {
  Serial.begin(57600);
  Serial.println("Jellyfish Test");
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  tapMillis = millis();
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  for(int i; i < NUM_LEDS; i++)
    {
      setupLeds[i] = ColorFromPalette(currentPalette, 0, 255, currentBlending);
    }
}

void loop()
{
  if(millis()-tapMillis > 2000)
    {
      checkTaps();
    }

  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;

  sinelon(startIndex);
  /* FillLEDsFromPaletteColors(paletteIndex); */

  if(millis()-tapMillis < 500)
    {
      addGlitter(160);
    }
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void sinelon(uint8_t colorIndex) {

  uint8_t brightness = 255;
  fadeToBlackBy(leds, NUM_LEDS, 1);
  int position = beatsin8(30, 0, NUM_LEDS);
  position -= 28;
  if(position > 0)
  {
    leds[position-1] = ColorFromPalette(currentPalette, paletteIndex, brightness, currentBlending);
  }
  if(position == 0 && notzero)
    {
      paletteIndex += 8;
      notzero = false;
    }
  else if(position > 0)
    {
      notzero = true;
    }
  Serial.print("position: ");
  Serial.print(position - 28);
  Serial.print("colorIndex: ");
  Serial.println(paletteIndex);

}
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  for(int i = NUM_LEDS-1; i >= 0; i--)
  {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending );
    colorIndex += 1;
  }
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 2);
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

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void checkTaps()
{
  if(analogRead(vibration)<1)
    {
      Serial.print("Vibration Level: ");
      Serial.println(analogRead(vibration));
      /* gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns); */
      /* rainbowWithGlitter(); */
      addGlitter(80);
      tapMillis = millis();
    }
}
