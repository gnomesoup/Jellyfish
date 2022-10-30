#include "FastLED.h"
#include "pixeltypes.h"

#define LED1_PIN  6
#define LED2_PIN  9
#define LED3_PIN  10
#define SOUND_PIN A5

#define NUM_LEDS      32
#define TOP_LEDS      8
#define BRIGHTNESS    128
#define LED_TYPE      WS2811
#define COLOR_ORDER   RGB
#define SOUND_READ_INTERVAL 50

CRGB leds[NUM_LEDS];
CRGB setupLeds[NUM_LEDS];
CRGB currentColor = CRGB::Red;
uint8_t beatCount = 0;
uint8_t beatAdvance = 8;
uint8_t lastPosition = 0;
uint8_t colorIndex = 0;
unsigned long pulseTime = 0;
unsigned long nextShowTime = 0;
unsigned long soundReadTime = 50;
float soundLevelMax = 0;
float sampleMin = 1024;
float sampleMax = 0;

CRGBPalette16 JellyColors(
  CRGB::Lime,
  CRGB::SpringGreen,
  CRGB::DeepSkyBlue,
  CRGB::Blue,

  CRGB::Fuchsia,
  CRGB::MediumOrchid,
  CRGB::Aqua,
  CRGB::Aquamarine,

  CRGB::Lime,
  CRGB::SpringGreen,
  CRGB::DeepSkyBlue,
  CRGB::Blue,

  CRGB::Fuchsia,
  CRGB::MediumOrchid,
  CRGB::Aqua,
  CRGB::Aquamarine
);

void setup() {
  Serial.begin(115200);
  delay( 3000 );
  pinMode(SOUND_PIN, INPUT);
  Serial.println("Setup");
  FastLED.setMaxRefreshRate(120);
  FastLED.addLeds<LED_TYPE, LED1_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED2_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED3_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  for(int i; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  delay(1000);
  currentColor = JellyColors[colorIndex];
  for(int i; i < NUM_LEDS; i++)
  {
    setupLeds[i] = currentColor;
    leds[i] = currentColor;
  }
  FastLED.show();
  
  Serial.println("Setup Complete");
}

void loop() {
  fadeToBlackBy(leds, NUM_LEDS, 32);
  int sample = analogRead(SOUND_PIN);
  if(sample < sampleMin)
  {
    sampleMin = sample;
  }
  if(sample > sampleMax)
  {
    sampleMax = sample;
  }

  if(millis() > soundReadTime)
  {
    int peak = sampleMax - sampleMin;
    float soundLevel = (peak * 3.3) / 1024;
    // Serial.print("Peak = ");
    // Serial.print(peak);
    if (soundLevel > 0.25)
    {
      Serial.print("BOOM! ");
      Serial.println(soundLevel);
    }
    else
    {
      Serial.println(" ");      
    }
    sampleMin = 1024;
    sampleMax = 0;
    soundReadTime = millis() + SOUND_READ_INTERVAL;
  }

  if(millis() > pulseTime)
  {
    int position = map(cubicwave8(beatCount), 0, 255, 0, NUM_LEDS);
    // Serial.print("position=");
    // Serial.print(position);
    // Serial.print(" lastPosition=");
    // Serial.print(lastPosition);
    int difference = position - lastPosition;
    // Serial.print(" difference = ");
    // Serial.println(difference);
    for(int i = abs(difference); i > 0; i--)
    {
      if(difference > 0)
      {
        setupLeds[position - i] = currentColor;
      }
      else
      {
        setupLeds[position + i] = currentColor;
      }
    }
      beatCount += beatAdvance;
    if(position == 0 && position != lastPosition)
    {
      // Serial.println("Blue");
      colorIndex = (colorIndex + 1) % 16;
      currentColor = JellyColors[colorIndex];
      beatAdvance = 6;
      pulseTime = millis() + 500;
    }
    if(position == NUM_LEDS && position != lastPosition)
    {
      // Serial.println("Red");
      colorIndex = (colorIndex + 1) % 16;
      currentColor = JellyColors[colorIndex];
      beatAdvance = 16;
    }
    lastPosition = position;
  }


  // for(int i; i < NUM_LEDS; i++)
  // {
  //   leds[i] = setupLeds[i];
  // }  
  
  if (millis() > nextShowTime)
  {
    Serial.println("show");
    FastLED.show();
    nextShowTime = millis() + 100;
  }
}
