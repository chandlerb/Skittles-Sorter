

#include <Wire.h>
#include "Adafruit_TCS34725.h"
//for stepper 
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

// Example code for the Adafruit TCS34725 breakout library applied to Skittle colour classification.
// This example uses RGB vectors for classification.  It also converts the RGB vector to a HSB colourspace, 
// which is likely more robust for this classification, but does not implement the HSB classification.
// (If you change to HSB, you will likely only need hue and saturation, and not brightness). 

// More information:
// Breakout board: https://www.adafruit.com/product/1334
// Library: https://github.com/adafruit/Adafruit_TCS34725
// Installation instructions: https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use/how-to-install-a-library

   
// Initialise TCS24725 with specific int time and gain values 
// Note: 2-4 millisecond integration (sampling) times mean we can sample at about 250-500Hz
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);


// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 1);

/*
 * Global colour sensing variables
 */

#define NUM_COLORS  6

// Skittle colours to indices
#define COL_RED      0
#define COL_GREEN    1
#define COL_ORANGE   2
#define COL_YELLOW   3
#define COL_PURPLE   4
#define COL_NOTHING  5

// Names for colours
#define COLNAME_RED      "RED"
#define COLNAME_GREEN    "GREEN"
#define COLNAME_ORANGE   "ORANGE"
#define COLNAME_YELLOW   "YELLOW"
#define COLNAME_PURPLE   "PURPLE"
#define COLNAME_NOTHING  "NOTHING"

// RGB channels in the array
#define CHANNEL_R   0
#define CHANNEL_G   1
#define CHANNEL_B   2

// Training colours (populate these manually, but these vectors must be of unit length (i.e. length 1))
float trainingColors[3][NUM_COLORS];    // 3(rgb) x NUM_COLORS.

// Last read colour
float rNorm = 0.0f;
float gNorm = 0.0f;
float bNorm = 0.0f;
float hue = 0.0f;
float saturation = 0.0f;
float brightness = 0.0f;

// Last classified class
int lastClass = -1;
float lastCosine = 0;

/*
 * Colour sensing
 */
void initializeTrainingColors() {
  // Skittle: red
  trainingColors[CHANNEL_R][COL_RED] = 0.6967;
  trainingColors[CHANNEL_G][COL_RED] = 0.46215;
  trainingColors[CHANNEL_B][COL_RED] = 0.5461;

  // Skittle: green
  trainingColors[CHANNEL_R][COL_GREEN] = 0.449;
  trainingColors[CHANNEL_G][COL_GREEN] = 0.7436;
  trainingColors[CHANNEL_B][COL_GREEN] = 0.495;

  // Skittle: orange
  trainingColors[CHANNEL_R][COL_ORANGE] = 0.8339;
  trainingColors[CHANNEL_G][COL_ORANGE] = 0.40915;
  trainingColors[CHANNEL_B][COL_ORANGE] = 0.3668;
  
  // Skittle: yellow
  trainingColors[CHANNEL_R][COL_YELLOW] = 0.64516667;
  trainingColors[CHANNEL_G][COL_YELLOW] = 0.667666;
  trainingColors[CHANNEL_B][COL_YELLOW] = 0.36766667;

  // Skittle: purple
  trainingColors[CHANNEL_R][COL_PURPLE] = 0.488;
  trainingColors[CHANNEL_G][COL_PURPLE] = 0.5752;
  trainingColors[CHANNEL_B][COL_PURPLE] = 0.6536;

  // Nothing
  trainingColors[CHANNEL_R][COL_NOTHING] = 0.416;
  trainingColors[CHANNEL_G][COL_NOTHING] = 0.588;
  trainingColors[CHANNEL_B][COL_NOTHING] = 0.693;
}

void getNormalizedColor() {
  uint16_t r, g, b, c, colorTemp, lux;  
  tcs.getRawData(&r, &g, &b, &c);

  float lenVec = sqrt((float)r*(float)r + (float)g*(float)g + (float)b*(float)b);

  // Note: the Arduino only has 2k of RAM, so rNorm/gNorm/bNorm are global variables. 
  rNorm = (float)r/lenVec;
  gNorm = (float)g/lenVec;
  bNorm = (float)b/lenVec;

  // Also convert to HSB:
  RGBtoHSV(rNorm, gNorm, bNorm, &hue, &saturation, &brightness);
}


int getColorClass() {
  float distances[NUM_COLORS] = {0.0f};

  // Step 1: Compute the cosine similarity between the query vector and all the training colours. 
  for (int i=0; i<NUM_COLORS; i++) {
    // For normalized (unit length) vectors, the cosine similarity is the same as the dot product of the two vectors.
    float cosineSimilarity = rNorm*trainingColors[CHANNEL_R][i] + gNorm*trainingColors[CHANNEL_G][i] + bNorm*trainingColors[CHANNEL_B][i];
    distances[i] = cosineSimilarity;

    // DEBUG: Output cosines
    //Serial.print("   C"); Serial.print(i); Serial.print(": "); Serial.println(cosineSimilarity, 3);
  }

  // Step 2: Find the vector with the highest cosine (meaning, the closest to the training color)
  float maxVal = distances[0];
  int maxIdx = 0;
  for (int i=0; i<NUM_COLORS; i++) {
    if (distances[i] > maxVal) {
      maxVal = distances[i];
      maxIdx = i;
    }
  }

  // Step 3: Return the index of the minimum color
  lastCosine = maxVal;
  lastClass = maxIdx;
  return maxIdx;
}


// Convert from colour index to colour name.
void printColourName(int colIdx) {
  /*
  switch (colIdx) {
    case COL_RED:
      Serial.print(COLNAME_RED);
      break;
    case COL_GREEN:
      Serial.print(COLNAME_GREEN);
      break;
    case COL_ORANGE:
      Serial.print(COLNAME_ORANGE);
      break;
    case COL_YELLOW:
      Serial.print(COLNAME_YELLOW);
      break;
    case COL_PURPLE:
      Serial.print(COLNAME_PURPLE);
      break;
    case COL_NOTHING:
      Serial.print(COLNAME_NOTHING);
      break;
    default:
      Serial.print("ERROR");
      break;
  }
  */
}

/*
 * Colour converstion
 */

// RGB to HSV.  From https://www.cs.rit.edu/~ncs/color/t_convert.html . 
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v ) {  
  float minVal = min(min(r, g), b);
  float maxVal = max(max(r, g), b);
  *v = maxVal;       // v
  float delta = maxVal - minVal;
  if( maxVal != 0 )
    *s = delta / maxVal;   // s
  else {
    // r = g = b = 0    // s = 0, v is undefined
    *s = 0;
    *h = -1;
    return;
  }
  if( r == maxVal )
    *h = ( g - b ) / delta;   // between yellow & magenta
  else if( g == maxVal )
    *h = 2 + ( b - r ) / delta; // between cyan & yellow
  else
    *h = 4 + ( r - g ) / delta; // between magenta & cyan
  *h *= 60;       // degrees
  if( *h < 0 )
    *h += 360;
}

void color_detection() {
  // Step 1: Get normalized colour vector
  getNormalizedColor();
  int colClass = getColorClass(); 
  
  if (colClass == 0){
    myservo.write(135);
    delay(100);
  }
  else if (colClass == 1) {
    myservo.write(110);
    delay(100);
  }
  else if (colClass == 2) {
    myservo.write(80);
    delay(100);
  }
  else if (colClass == 3){
    myservo.write(60);
    delay(100);
  }
  else if (colClass == 4 || colClass == 5) {
    myservo.write(35);
    delay(125);
  }
    

  
  /*
  // Step 2: Output colour
  Serial.print("R: "); Serial.print(rNorm, 3); Serial.print("  ");
  Serial.print("G: "); Serial.print(gNorm, 3); Serial.print("  ");
  Serial.print("B: "); Serial.print(bNorm, 3); Serial.print("  ");  
  Serial.print("H: "); Serial.print(hue, 3); Serial.print("  ");
  Serial.print("S: "); Serial.print(saturation, 3); Serial.print("  ");
  Serial.print("B: "); Serial.print(brightness, 3); Serial.print("  ");

  printColourName(colClass);  
  Serial.print(" (cos: "); Serial.print(lastCosine); Serial.print(") ");
  Serial.println("");
  */
}

/*
 * Main Arduino functions
 */
 
 
void setup(void) {
  // Populate array of training colours for classification. 
  initializeTrainingColors();

  AFMS.begin(1000);  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  myMotor->setSpeed(35);  // 20 rpm 
  delay(5000);

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(125);
  // Now we're ready to get readings!
}

void loop(void) {
  
  myMotor->step(33, BACKWARD, SINGLE);
  color_detection();
  delay(50);
  myMotor->step(34, BACKWARD, SINGLE);
  color_detection();
  delay(50);
  myMotor->step(33, BACKWARD, SINGLE);
  color_detection();
  delay(50);
  
  
  
  /*
  delay(1000);
  myMotor->step(50, BACKWARD, SINGLE); 
  color_detection();
  delay(1000);
  myMotor->step(47, BACKWARD, SINGLE); 
  color_detection();
  delay(1000);
  myMotor->step(52, BACKWARD, SINGLE);
  color_detection(); 
  delay(1000);
  */
  
 
  
}




