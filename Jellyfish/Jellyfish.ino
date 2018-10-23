#include <FastLED.h>

#define LED_PIN            6
#define NUM_LEDS           60
#define NUM_ACT_LEDS       32
#define BRIGHTNESS         64
#define LED_TYPE           WS2811
#define COLOR_ORDER        RGB
#define FRAMES_PER_SECOND  60
CRGB leds[NUM_LEDS];
CRGB setupLeds[NUM_LEDS];

boolean notzero = true;
uint8_t paletteIndex = 0;

const byte vibration = A0; // vibration sensor
const int tapLevel = 20;
unsigned long tapMillis;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
/* int beatMin = 50; */
/* int beatMax = NUM_LEDS; */
/* int beat = random(beatMin, beatMax); */
int beat = 30;
int previousPosition;
int pulsePause = 127;

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
  if(millis()-tapMillis > 3000)
    {
      checkTaps();
    }

  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;

  if(millis()-pulsePause < )
  {
    ColorBreather(startIndex);
  }
  for(int i; i < NUM_LEDS; i++)
    {
      leds[i] = setupLeds[i];
    }

  if(millis()-tapMillis < 800)
    {
      addGlitter(200);
    }
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void ColorBreather(uint8_t colorIndex) {

  uint8_t brightness = 255;
  fadeToBlackBy(setupLeds, NUM_LEDS, 1);
  int position = beatsin8(beat, 0, NUM_LEDS);
  if(position == 0 && notzero)
    {
      paletteIndex += 8;
      notzero = false;
      /* beat = random(beatMin, beatMax); */
    }
  else if(position > 0)
    {
      notzero = true;
    }

  position -= (NUM_LEDS - NUM_ACT_LEDS) + 1;
  int prePixel;
  if(position > previousPosition)
    {
      prePixel = position - 1;
    }
  else
    {
      prePixel = position + 1;
    }
  if(position > 0)
  {
    setupLeds[position] = ColorFromPalette(currentPalette, paletteIndex, brightness, currentBlending);
    previousPosition = position;
  }
  if(prePixel > 0 && prePixel < NUM_LEDS)
    {
      setupLeds[prePixel] = ColorFromPalette(currentPalette, paletteIndex, brightness, currentBlending);
    }
  Serial.print("position: ");
  Serial.print(position);
  Serial.print(" beat: ");
  Serial.print(beat);
  Serial.print(" previousPosition: ");
  Serial.println(previousPosition);

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
      addGlitter(80);
      tapMillis = millis();
    }
}
