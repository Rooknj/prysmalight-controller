/*
  Light.cpp - Library for flashing Light code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Light.h"

//************************************************************************
// Constructor
//************************************************************************
CRGBPalette16 gPal; //for fire
Light::Light()
{
  // Set Serial Communication rate
  if (CONFIG_DEBUG)
  {
    // For some reason you can not call this inside LightController.ino or the FastLED DMA will not work
    // TODO: Figure out why
    Serial.begin(115200);
  }
  FastLED.addLeds<CONFIG_CHIPSET, CONFIG_DATA_PIN, CONFIG_COLOR_ORDER>(_leds, CONFIG_NUM_LEDS);
  _stateOn = false;
  _brightness = 100;
  _color = CRGB(255, 0, 0);
  _effect = NO_EFFECT;
  _effectSpeed = 4;
  gPal = HeatColors_p; //for FIRE
}

//************************************************************************
// Setters
//************************************************************************
void Light::setState(boolean stateOn)
{
  _stateOn = stateOn;
  if (stateOn)
  {
    changeColorTo(_color.r, _color.g, _color.b);
  }
  else
  {
    changeColorTo(0, 0, 0);
  }
}

void Light::setBrightness(uint8_t brightness)
{
  _brightness = brightness;
  changeBrightnessTo(brightness);
}

void Light::setColorRGB(uint8_t p_red, uint8_t p_green, uint8_t p_blue)
{
  _stateOn = true;
  _color = CRGB(p_red, p_green, p_blue);
  changeColorTo(p_red, p_green, p_blue); // You must call this before changing the effect for its logic to work
  _effect = NO_EFFECT;
}

void Light::setColorHSV(uint8_t p_hue, uint8_t p_saturation, uint8_t p_value)
{
  _stateOn = true;
  _color = CHSV(p_hue, p_saturation, p_value);
  changeColorTo(_color.r, _color.g, _color.b); // You must call this before changing the effect for its logic to work
  _effect = NO_EFFECT;
}

bool startFlash = false;
void Light::setEffect(String effect)
{
  _stateOn = true;
  _color = CRGB(255, 255, 255);
  _effect = effect;
  setRGB(0, 0, 0);
  if (effect == "Flash")
  {
    startFlash = true;
  }
}

void Light::setEffectSpeed(int effectSpeed)
{
  _effectSpeed = effectSpeed;
}

//************************************************************************
// Getters
//************************************************************************
boolean Light::getState()
{
  return _stateOn;
}

uint8_t Light::getBrightness()
{
  return _brightness;
}

CRGB Light::getColor()
{
  return _color;
}

String Light::getEffect()
{
  return _effect;
}

int Light::getEffectSpeed()
{
  return _effectSpeed;
}

char **Light::getEffects()
{
  return _effects;
}

int Light::getNumEffects()
{
  return _numEffects;
}

//************************************************************************
// Class Methods
//************************************************************************
void Light::loop(int packetSize, WiFiUDP port)
{
  // Handle color changes
  handleColorChange();

  // Handle Brightness changes
  handleBrightnessChange();

  // Handle the current effect
  if (_effect == NO_EFFECT || _stateOn == false)
  { // Change to add effect
    // do nothing
  }
  else if (_effect == "Flash")
  {
    handleFlash();
  }
  else if (_effect == "Fade")
  {
    handleFade();
  }
  else if (_effect == "Rainbow")
  {
    handleRainbow();
  }
  else if (_effect == "Confetti")
  {
    handleConfetti();
  }
  else if (_effect == "Cylon")
  {
    handleCylon();
  }
  else if (_effect == "Juggle")
  {
    handleJuggle();
  }
  else if (_effect == "BPM")
  {
    handleBPM();
  }
  else if (_effect == "Sinelon")
  {
    handleSinelon();
  }
  else if (_effect == "Visualize")
  {
    handleVisualize(packetSize, port);
  }
  else if (_effect == "Dots")
  {
    handleDots();
  }
  else if (_effect == "Fire")
  {
    handleFire();
  }
  else if (_effect == "Lightning")
  {
    handleLightning();
  }
  else if (_effect == "Noise")
  {
    handleNoise();
  }
  // ADD_EFFECTS
}

void Light::handleVisualize(int packetSize, WiFiUDP port)
{
  if (packetSize == sizeof(_leds))
  {
    port.read((char *)_leds, sizeof(_leds));
    FastLED.show();
  }
  else if (packetSize)
  {
    Serial.printf("Invalid packet size: %u (expected %u)\n", packetSize, sizeof(_leds));
    port.flush();
    return;
  }
}

//************************************************************************
// Light Effects
//************************************************************************
const int FRAMES_PER_SECOND = 60;                                   // The Frames Per Second of all animations
const int FLASH_SPEEDS[7] = {4000, 2000, 1000, 500, 350, 200, 100}; // In ms between color transitions
const int FADE_SPEEDS[7] = {200, 100, 50, 33, 20, 10, 4};           // In ms between changing the hue by 1 (hue is a number 0-255)
const int RAINBOW_SPEEDS[7] = {100, 50, 33, 17, 12, 10, 4};         // In ms between shifting the LED's and hue by 1
const int CONFETTI_SPEEDS[7] = {50, 33, 23, 17, 13, 10, 8};         // In ms between shifting the LED's and hue by 1
const int CYLON_SPEEDS[7] = {10, 25, 50, 75, 100, 150, 200};        // In percent of the strip to travel in a second

// Determines when to physically update the LED strip
long lastShow = 0;
bool shouldShow()
{
  int updateThreshold = 1000 / FRAMES_PER_SECOND;
  long now = millis();

  if (now - lastShow > updateThreshold)
  {
    lastShow = now;
    return true;
  }
  return false;
}

// Cycles through all hue values on loop
long lastHueCycle = 0;
byte gHue;
void Light::cycleHue()
{
  gHue++;
}

// Sets the light strip to an RGB color
void Light::setRGB(uint8_t p_red, uint8_t p_green, uint8_t p_blue)
{
  CRGB newColor = CRGB(p_red, p_green, p_blue);
  fill_solid(_leds, CONFIG_NUM_LEDS, newColor);
  FastLED.show();
}

// Sets the light strip to an HSV color
void Light::setHSV(uint8_t p_hue, uint8_t p_saturation, uint8_t p_value)
{
  CRGB newColor = CHSV(p_hue, p_saturation, p_value);
  fill_solid(_leds, CONFIG_NUM_LEDS, newColor);
  FastLED.show();
}

// Determines when the current effect should update
long lastUpdate = 0;
bool Light::shouldUpdate()
{
  long updateThreshold;
  if (_effect == "Flash")
  {
    updateThreshold = FLASH_SPEEDS[_effectSpeed - 1];
  }
  else if (_effect == "Fade")
  {
    updateThreshold = FADE_SPEEDS[_effectSpeed - 1];
  }
  else if (_effect == "Rainbow")
  {
    updateThreshold = RAINBOW_SPEEDS[_effectSpeed - 1];
  }
  else if (_effect == "Confetti")
  {
    updateThreshold = CONFETTI_SPEEDS[_effectSpeed - 1];
  }
  else if (_effect == "Cylon")
  {
    // calculates how many LEDs to move in a second in terms of the percentage of the strip
    updateThreshold = 1000 / (CONFIG_NUM_LEDS * CYLON_SPEEDS[_effectSpeed - 1] / 100);
    // calculates how many LEDs to move in a second in terms of the number of LEDs per second
    //updateThreshold = 1000 / CYLON_SPEEDS[_effectSpeed - 1];
  }
  else
  {
    updateThreshold = 17;
  }
  long now = millis();

  if (now - lastUpdate > updateThreshold)
  {
    lastUpdate = now;
    return true;
  }
  return false;
}

// Fire code
#define COOLING 55
#define SPARKING 120
bool gReverseDirection = false; // Change this to make fire run from the other end
void Light::Fire2012WithPalette()
{
  // Array of temperature readings at each simulation cell
  static byte heat[CONFIG_NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < CONFIG_NUM_LEDS; i++)
  {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / CONFIG_NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = CONFIG_NUM_LEDS - 1; k >= 2; k--)
  {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING)
  {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < CONFIG_NUM_LEDS; j++)
  {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(gPal, colorindex);
    int pixelnumber;
    if (gReverseDirection)
    {
      pixelnumber = (CONFIG_NUM_LEDS - 1) - j;
    }
    else
    {
      pixelnumber = j;
    }
    _leds[pixelnumber] = color;
  }
}

// Flash
byte flash_index = 0;
void Light::handleFlash()
{
  if (shouldUpdate() || startFlash)
  {
    startFlash = false;
    if (flash_index == 0)
    {
      setRGB(255, 0, 0);
      flash_index++;
    }
    else if (flash_index == 1)
    {
      setRGB(0, 255, 0);
      flash_index++;
    }
    else
    {
      setRGB(0, 0, 255);
      flash_index = 0;
    }
  }
}

// Fade
void Light::handleFade()
{
  if (shouldUpdate())
  {
    cycleHue();
    fill_solid(_leds, CONFIG_NUM_LEDS, CHSV(gHue, 255, 255));
  }
  if (shouldShow())
  {
    FastLED.show();
  }
}

// Rainbow
void Light::handleRainbow()
{
  if (shouldUpdate())
  {
    cycleHue();
    fill_rainbow(_leds, CONFIG_NUM_LEDS, gHue, 3); // The shorter the last number, the longer each color is on the rainbow
  }
  if (shouldShow())
  {
    FastLED.show();
  }
}

// Confetti
void Light::handleConfetti()
{
  if (shouldUpdate())
  {
    cycleHue();
    fadeToBlackBy(_leds, CONFIG_NUM_LEDS, 10);
    int pos = random16(CONFIG_NUM_LEDS);
    _leds[pos] += CHSV(gHue + random8(64), 200, 255);
  }
  if (shouldShow())
  {
    FastLED.show();
  }
}

// Cylon
int LED = 0;
bool forward = true;
void Light::fadeall()
{
  for (int i = 0; i < CONFIG_NUM_LEDS; i++)
  {
    _leds[i].nscale8(247);
  }
}
void Light::handleCylon()
{
  if (shouldUpdate())
  {
    cycleHue();
    fadeall();
    // First slide the led in one direction
    if (LED >= CONFIG_NUM_LEDS - 1)
    {
      forward = false;
    }
    else if (LED <= 0)
    {
      forward = true;
    }
    if (forward)
    {
      LED++;
    }
    else
    {
      LED--;
    }
    _leds[LED] = CHSV(gHue, 255, 255);
  }
  if (shouldShow())
  {
    FastLED.show();
  }
}

const int JUGGLE_BPMS_ADDER[7] = {1, 4, 7, 10, 13, 17, 20};
const int JUGGLE_FADE[7] = {20, 25, 30, 35, 40, 45, 50};
// Juggle
void Light::handleJuggle()
{
  if (shouldShow())
  {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(_leds, CONFIG_NUM_LEDS, JUGGLE_FADE[_effectSpeed - 1]);
    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
      _leds[beatsin16(i + JUGGLE_BPMS_ADDER[_effectSpeed - 1], 0, CONFIG_NUM_LEDS - 1)] |= CHSV(dothue, 200, 255); //TODO: Try changing i+7 (BPM) http://fastled.io/docs/3.1/group__lib8tion.html#gaa46e5de1c4c27833359e7a97a18c839b
      dothue += 32;
    }
    FastLED.show();
  }
}

// BPM
const int BPMS[7] = {15, 30, 50, 80, 100, 120, 150}; // 98 seems to be the max value
void Light::handleBPM()
{
  if (shouldShow())
  {
    cycleHue();
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = BPMS[_effectSpeed - 1];
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < CONFIG_NUM_LEDS; i++)
    { //9948
      _leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
    FastLED.show();
  }
}

// Sinelon
const int SINELON_BPMS[7] = {8, 12, 14, 18, 22, 26, 30};
void Light::handleSinelon()
{
  if (shouldShow())
  {
    cycleHue();
    fadeToBlackBy(_leds, CONFIG_NUM_LEDS, 20);
    int pos = beatsin16(SINELON_BPMS[_effectSpeed - 1], 0, CONFIG_NUM_LEDS - 1);
    _leds[pos] += CHSV(gHue, 255, 192);
    FastLED.show();
  }
}

// Dots
uint8_t count = 0; // Count up to 255 and then reverts to 0
uint8_t DOTS_BPM = 30;
const int DOTS_BPMS[7] = {8, 12, 14, 18, 22, 26, 30};
const int DOTS_FADES[7] = {20, 25, 30, 35, 40, 45, 50};
void Light::handleDots()
{
  if (shouldShow())
  {
    uint8_t inner = beatsin8(DOTS_BPMS[_effectSpeed - 1], CONFIG_NUM_LEDS / 4, CONFIG_NUM_LEDS / 4 * 3);
    uint8_t outer = beatsin8(DOTS_BPMS[_effectSpeed - 1], 0, CONFIG_NUM_LEDS - 1);
    uint8_t middle = beatsin8(DOTS_BPMS[_effectSpeed - 1], CONFIG_NUM_LEDS / 3, CONFIG_NUM_LEDS / 3 * 2);
    _leds[middle] = CRGB::Purple;
    _leds[inner] = CRGB::Blue;
    _leds[outer] = CRGB::Aqua;
    fadeToBlackBy(_leds, CONFIG_NUM_LEDS, DOTS_FADES[_effectSpeed - 1]);
    FastLED.show();
  }
}

// Fire
void Light::handleFire()
{
  random16_add_entropy(random8());
  if (shouldShow())
  {
    Fire2012WithPalette();
    FastLED.show();
  }
}

// Lightning
uint8_t frequency = 50; // controls the interval between strikes
uint8_t flashes = 8;    //the upper limit of flashes per strike
unsigned int dimmer = 1;
uint8_t ledstart; // Starting location of a flash
uint8_t ledlen;
long lastStrike = 0;
long nextStrike = 0;
bool shouldStrike()
{
  long now = millis();

  if (now - lastStrike > nextStrike)
  {
    lastStrike = now;
    return true;
  }
  return false;
}
void Light::handleLightning()
{
  random16_add_entropy(random8());
  if (shouldStrike())
  {
    ledstart = random8(CONFIG_NUM_LEDS);          // Determine starting location of flash
    ledlen = random8(CONFIG_NUM_LEDS - ledstart); // Determine length of flash (not to go beyond NUM_LEDS-1)
    for (int flashCounter = 0; flashCounter < random8(3, flashes); flashCounter++)
    {
      if (flashCounter == 0)
        dimmer = 5; // the brightness of the leader is scaled down by a factor of 5
      else
        dimmer = random8(1, 3); // return strokes are brighter than the leader
      fill_solid(_leds + ledstart, ledlen, CHSV(255, 0, 255 / dimmer));
      FastLED.show();                                        // Show a section of LED's
      delay(random8(4, 10));                                 // each flash only lasts 4-10 milliseconds
      fill_solid(_leds + ledstart, ledlen, CHSV(255, 0, 0)); // Clear the section of LED's
      FastLED.show();
      if (flashCounter == 0)
        delay(130);             // longer delay until next flash after the leader
      delay(50 + random8(100)); // shorter delay between strokes
    }
    nextStrike = random8(frequency) * 100; // delay between strikes
  }
}

// Noise
static uint16_t dist;         // A random number for our noise generator.
uint16_t scale = 30;          // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
uint8_t maxChanges = 48;      // Value for blending between palettes.
CRGBPalette16 targetPalette(OceanColors_p);
CRGBPalette16 currentPalette(CRGB::Black);
void Light::handleNoise()
{
  if (shouldShow())
  {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
    for (int i = 0; i < CONFIG_NUM_LEDS; i++)
    {                                                                      // Just onE loop to fill up the LED array as all of the pixels change.
      uint8_t index = inoise8(i * scale, dist + i * scale) % 255;          // Get a value from the noise function. I'm using both x and y axis.
      _leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    }
    dist += beatsin8(10, 1, 4); // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
    // In some sketches, I've used millis() instead of an incremented counter. Works a treat.
    FastLED.show();
  }
}

// ADD_EFFECTS

//************************************************************************
// Crossfade
//************************************************************************
int getStep(int start, int target, int numSteps)
{
  return (target - start) / numSteps;
}

int getRemainder(int start, int target, int numSteps)
{
  return (target - start) % numSteps;
}

int getChange(int stepAmount, int remainderAmount, int currentStep, int numSteps)
{
  int extra = 0;
  if (((currentStep * abs(remainderAmount)) / numSteps) >
      (((currentStep - 1) * abs(remainderAmount)) / numSteps))
  {
    if (stepAmount < 0 || remainderAmount < 0)
    {
      extra = -1;
    }
    else
    {
      extra = 1;
    }
  }

  return stepAmount + extra;
}

// Color takes 500ms to change no matter what
int colorTransitionTime = 500; // In ms, has to be a multiple of 255
int numColorSteps = 30;
boolean startFade = false;
boolean inFade = false;
uint8_t currentRed = 0; // Initialized as the initial color defined in the constructor
uint8_t currentGreen = 0;
uint8_t currentBlue = 0;
uint8_t targetRed = 0;
uint8_t targetGreen = 0;
uint8_t targetBlue = 0;
int stepRed = 0;
int stepGreen = 0;
int stepBlue = 0;
int remainderRed = 0;
int remainderGreen = 0;
int remainderBlue = 0;
int currentStep = 0;

void Light::changeColorTo(uint8_t red, uint8_t green, uint8_t blue)
{
  startFade = true;
  targetRed = red;
  targetGreen = green;
  targetBlue = blue;
}

unsigned long lastStepTime = 0;
void Light::handleColorChange()
{
  if (startFade)
  {
    // If we are playing an effect, do nothing
    if (_effect != NO_EFFECT && _stateOn)
    {
      return;
    }

    // If the transitions are off, or the light is set to an effect, dont do the transition
    if (colorTransitionTime == 0 || _effect != NO_EFFECT)
    {
      setRGB(targetRed, targetGreen, targetBlue);

      currentRed = targetRed;
      currentGreen = targetGreen;
      currentBlue = targetBlue;

      startFade = false;
    }
    // Start the transition
    else
    {
      startFade = false;
      inFade = true;
      currentStep = 1;

      // Calculate the step values
      stepRed = getStep(currentRed, targetRed, numColorSteps);
      stepGreen = getStep(currentGreen, targetGreen, numColorSteps);
      stepBlue = getStep(currentBlue, targetBlue, numColorSteps);
      remainderRed = getRemainder(currentRed, targetRed, numColorSteps);
      remainderGreen = getRemainder(currentGreen, targetGreen, numColorSteps);
      remainderBlue = getRemainder(currentBlue, targetBlue, numColorSteps);
    }
  }

  // If we are currently in the middle of a color change, keep doing the transition
  if (inFade)
  {
    int stepDuration = colorTransitionTime / numColorSteps;

    unsigned long now = millis();
    // If its time to take a step
    if (now - lastStepTime > stepDuration)
    {
      lastStepTime = now;

      // Calculate the next value to change to
      currentRed += getChange(stepRed, remainderRed, currentStep, numColorSteps);
      currentGreen += getChange(stepGreen, remainderGreen, currentStep, numColorSteps);
      currentBlue += getChange(stepBlue, remainderBlue, currentStep, numColorSteps);
      currentStep++;

      // Set the value and increment the step;
      setRGB(currentRed, currentGreen, currentBlue); // Write current values to LED pins
    }

    // If we have gone through all 255 steps, end the transition
    if (currentStep > numColorSteps)
    {
      inFade = false;
      //Serial.println("Ending Fade: ");
      //Serial.printf("Current Red: %i, Current Green: %i, Current Blue: %i\n", currentRed, currentGreen, currentBlue);
      //Serial.printf("target Red: %i, target Green: %i, target Blue: %i\n", targetRed, targetGreen, targetBlue);
    }
  }
}

// Brightness takes 1000ms to change from 0-100%, 500ms to change from 0-50, 250 ms to change from 0-25, etc.unsigned long lastBrightnessStepTime = 0;
const int maxBrightnessTransitionTime = 1000;
const int maxBrightnessSteps = 60;
const int maxBrightness = 100;
const int minBrightness = 0;
int brightnessTransitionTime = 1;
int numBrightnessSteps = 1;
boolean startBrightnessTransition = false;
boolean inBrightnessTransition = false;
uint8_t currentBrightness;
uint8_t targetBrightness;
int stepBrightness = 0;
int remainderBrightness = 0;
int currentBrightnessStep = 0;

void Light::changeBrightnessTo(uint8_t brightness)
{
  startBrightnessTransition = true;
  targetBrightness = brightness;
}

unsigned long lastBrightnessStepTime = 0;
void Light::handleBrightnessChange()
{
  if (startBrightnessTransition)
  {
    // If the lights are off, just set the brightness. Dont need to be fancy
    if (!_stateOn)
    {
      FastLED.setBrightness(map(targetBrightness, 0, 100, 0, CONFIG_MAX_BRIGHTNESS));
      currentBrightness = targetBrightness;

      startBrightnessTransition = false;
    }
    else
    {
      startBrightnessTransition = false;
      inBrightnessTransition = true;
      currentBrightnessStep = 1;
      brightnessTransitionTime = abs(targetBrightness - currentBrightness) * maxBrightnessTransitionTime / (maxBrightness - minBrightness);
      numBrightnessSteps = abs(targetBrightness - currentBrightness) * maxBrightnessSteps / (maxBrightness - minBrightness);
      if (numBrightnessSteps == 0)
      {
        numBrightnessSteps = 1;
      }
    }

    // Calculate the step values
    stepBrightness = getStep(currentBrightness, targetBrightness, numBrightnessSteps);
    remainderBrightness = getRemainder(currentBrightness, targetBrightness, numBrightnessSteps);
  }

  // If we are currently in the middle of a color change, keep doing the transition
  if (inBrightnessTransition)
  {
    int stepDuration = brightnessTransitionTime / numBrightnessSteps;

    unsigned long now = millis();
    // If its time to take a step
    if (now - lastBrightnessStepTime > stepDuration)
    {
      lastBrightnessStepTime = now;

      currentBrightness += getChange(stepBrightness, remainderBrightness, currentBrightnessStep, numBrightnessSteps);
      currentBrightnessStep++;

      // Set the value and increment the step;
      FastLED.setBrightness(map(currentBrightness, 0, 100, 0, CONFIG_MAX_BRIGHTNESS));
      FastLED.show();
    }

    // If we have gone through all 255 steps, end the transition
    if (currentBrightnessStep > numBrightnessSteps)
    {
      inBrightnessTransition = false;
      //Serial.println("Ending Brightness Transition: ");
      //Serial.printf("Current Brightness: %i\n", currentBrightness);
      //Serial.printf("target Brightness: %i\n", targetBrightness);
    }
  }
}
