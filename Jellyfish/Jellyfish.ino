#include <FastLED.h>

#define LED_PIN            6
#define NUM_LEDS           60
#define NUM_ACT_LEDS       18
#define BRIGHTNESS         128
#define LED_TYPE           WS2811
#define COLOR_ORDER        RGB
#define FRAMES_PER_SECOND  60
CRGB leds[NUM_LEDS];
CRGB setupLeds[NUM_LEDS];

boolean notzero = true;
uint8_t paletteIndex = 0;
uint8_t startIndex = 0;

const byte vibration = A0; // vibration sensor
const int tapLevel = 20;
unsigned long tapMillis;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
/* int beatMin = 50; */
/* int beatMax = NUM_LEDS; */
/* int beat = random(beatMin, beatMax); */
uint8_t beat = 30;
int previousPosition;
unsigned long pulsePause = 500;
unsigned long pulseTime = 0;
uint8_t beatCount = 0;

void setup() {
  Serial.begin(57600);
  Serial.println("Jellyfish Test");
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  tapMillis = millis();
  currentPalette = PartyColors_p;
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


  fadeToBlackBy(setupLeds, NUM_LEDS, 1);
  if(millis() > pulseTime)
  {
    ColorBreather();
    /* Serial.print(millis()); */
    /* Serial.print(">"); */
    /* Serial.println(pulseTime); */
  }
  else
  {
    Serial.println("Pause");
  }
  for(int i; i < NUM_LEDS; i++)
    {
      leds[i] = setupLeds[i];
    }

  if(millis()-tapMillis < 800)
    {
      addGlitter(255);
    }
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void ColorBreather() {

  uint8_t brightness = 255;
  /* fadeToBlackBy(setupLeds, NUM_LEDS, 1); */
  /* int position = beatsin8(beat, 0, NUM_LEDS); */
  int position = map(cubicwave8(beatCount), 0, 255, 0, NUM_LEDS);
  if(position == 0 && notzero)
    {
      paletteIndex += 8;
      notzero = false;
      pulsePause = random16(400,1200);
      pulseTime = millis() + pulsePause;
      beatCount = 0;
    }
  else if(position > 0)
    {
      notzero = true;
    }
  beatCount += 4;
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
  if(position >= 0)
  {
    setupLeds[position] = ColorFromPalette(currentPalette, paletteIndex, brightness, currentBlending);
    previousPosition = position;
  }
  if(prePixel >= 0 && prePixel < NUM_LEDS)
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
    leds[ random16(NUM_ACT_LEDS) ] += CRGB::White;
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void checkTaps()
{
  if(analogRead(vibration)<15)
    {
      Serial.print("Vibration Level: ");
      Serial.println(analogRead(vibration));
      addGlitter(80);
      tapMillis = millis();
    }
}
