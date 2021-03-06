/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using SPI to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

bool alternator = false;
// int ledPin = 13; // choose the pin for the LED
int buttonPin = 8;   // choose the input pin (for a pushbutton)
int buttonVal = 0;     // variable for reading the pin status

int maxSleepMillis = 200;
int minSleepMillis = 150;
// stepper 1 Pins
int pinDir_1 = 30; // 2;
int pinStep_1 = 28; //3;
int pinSleep_1 = 26; //4;
int pinMS3_1 = 24; //5;
int pinMS2_1 = 22; // 6;
int pinMS1_1 = 20; // 7;

int pinDir_2 = 42; // 2;
int pinStep_2 = 40; //3;
int pinSleep_2 = 38; //4;
int pinMS3_2 = 36; //5;
int pinMS2_2 = 34; // 6;
int pinMS1_2 = 32; // 7;

// MS1, MS2, MS3 low = full speed
int stepperMS1 = 1;
int stepperMS2 = 1;
int stepperMS3 = 1;
int stepperSleep = 0;
int stepperDir = 0;
int stepperStep = 0;
#define ARRAYSIZE 2
String stepperDirections[ARRAYSIZE] = { "backward", "forward" };

void setup()   {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);    // declare pushbutton as input

  pinMode(pinMS1_1, OUTPUT);
  pinMode(pinMS2_1, OUTPUT);
  pinMode(pinMS3_1, OUTPUT);
  pinMode(pinStep_1, OUTPUT);
  pinMode(pinDir_1, OUTPUT);
  pinMode(pinSleep_1, OUTPUT);

  pinMode(pinMS1_2, OUTPUT);
  pinMode(pinMS2_2, OUTPUT);
  pinMode(pinMS3_2, OUTPUT);
  pinMode(pinSleep_2, OUTPUT);
  pinMode(pinStep_2, OUTPUT);
  pinMode(pinDir_2, OUTPUT);

  // initialize stepper 1
  digitalWrite(pinMS1_1, stepperMS1);
  digitalWrite(pinMS2_1, stepperMS2);
  digitalWrite(pinMS3_1, stepperMS3);
  digitalWrite(pinSleep_1, stepperSleep);
  digitalWrite(pinDir_1, stepperDir);
  digitalWrite(pinStep_1, stepperStep);

  // initialize stepper 2
  digitalWrite(pinMS1_2, stepperMS1);
  digitalWrite(pinMS2_2, stepperMS2);
  digitalWrite(pinMS3_2, stepperMS3);
  digitalWrite(pinSleep_2, stepperSleep);
  digitalWrite(pinDir_2, stepperDir);
  digitalWrite(pinStep_2, stepperStep);

  alternator = false;

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(250);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  // display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  //display.display();
  //delay(2000);
  // display.clearDisplay();

  testdrawchar();
  delay(250);
  initalizeTextDisplay();

}


void loop() {
  alternator = !alternator;
  buttonVal = digitalRead(buttonPin);
  if (buttonVal == HIGH)
  {
    if (alternator)
    {
      showOnDisplay( "press button..");
    }
    else
    {
      showOnDisplay( "press button...");
    }
    // send stepper to sleep - no current flow!
    digitalWrite(pinSleep_1, LOW);
    digitalWrite(pinSleep_2, LOW);
  }
  else
  {
    for (int outer = 0; outer < 3; outer++)
    {
      stepperDir = 1;
      showOnDisplay("Running: ", stepperDirections[stepperDir]);
      for (long n = 0; n < 10; n++)
      {
        unsigned long startTime = millis();
        RunTwoSteppers(200, stepperDir);
        // RunStepsStepper1(250, stepperDir);
        // RunStepsStepper2(250, stepperDir);
        stepperDir = !stepperDir;

        unsigned long endTime = millis();
        unsigned long elapsedTime = endTime - startTime;
        // showOnDisplay("Elapsed: ", String(elapsedTime));
        // delay(2000);
      }

      RunStepper2(100, 1);
      for (long n = 0; n < 10; n++)
      {
        unsigned long startTime = millis();
        RunStepper1(200, stepperDir);
        // RunStepsStepper1(250, stepperDir);
        // RunStepsStepper2(250, stepperDir);
        stepperDir = !stepperDir;

        unsigned long endTime = millis();
        unsigned long elapsedTime = endTime - startTime;
        // showOnDisplay("Elapsed: ", String(elapsedTime));
        // delay(2000);
      }
      RunStepper2(100, 0);

      RunStepper1(100, 1);
      for (long n = 0; n < 10; n++)
      {
        unsigned long startTime = millis();
        RunStepper2(200, stepperDir);
        // RunStepsStepper1(250, stepperDir);
        // RunStepsStepper2(250, stepperDir);
        stepperDir = !stepperDir;

        unsigned long endTime = millis();
        unsigned long elapsedTime = endTime - startTime;
        // showOnDisplay("Elapsed: ", String(elapsedTime));
        // delay(2000);
      }
      RunStepper1(100, 0);


      // maxSleepMillis = 500;
      // minSleepMillis = 500;
      // do so more tricks ...
      // RECTANGLE
      // first goto left, bottom

      showOnDisplay("Running Square");
      for (long k = 0; k < 6; k++)
      {
        stepperDir = 1;
        //showOnDisplay("right");
        RunStepper1(200, stepperDir);
        //delay(1000);
        // showOnDisplay("up");
        RunStepper2(200, stepperDir);
        //delay(1000);
        stepperDir = !stepperDir;
        //showOnDisplay("right");
        RunStepper1(200, stepperDir);
        //delay(1000);
        //showOnDisplay("down");
        RunStepper2(200, stepperDir);
        // delay(1000);
      }

      // schleife - 8-er-schleife
      for (int i = 0; i < 10; i++)
      {
        stepperDir = 1;
        RunStepper1(100, stepperDir);
        RunStepper2(100, stepperDir);
        RunStepper1(100, stepperDir);
        RunStepper2(100, stepperDir);
        stepperDir = !stepperDir;
        RunStepper1(100, stepperDir);
        RunStepper2(100, stepperDir);
        RunStepper1(100, stepperDir);
        RunStepper2(100, stepperDir);
      }
    }
  }
  digitalWrite(pinSleep_1, LOW);
  digitalWrite(pinSleep_2, LOW);
  delay(150);


}

void RunTwoSteppers(long steps, int dir)
{
  // wake up stepper
  // initialize steppers
  digitalWrite(pinMS1_1, stepperMS1);
  digitalWrite(pinMS2_1, stepperMS2);
  digitalWrite(pinMS3_1, stepperMS3);
  digitalWrite(pinStep_1, LOW);
  digitalWrite(pinSleep_1, HIGH);
  digitalWrite(pinDir_1, dir);
  digitalWrite(pinMS1_2, stepperMS1);
  digitalWrite(pinMS2_2, stepperMS2);
  digitalWrite(pinMS3_2, stepperMS3);
  digitalWrite(pinStep_2, LOW);
  digitalWrite(pinSleep_2, HIGH);
  digitalWrite(pinDir_2, dir);

  int delayMS = maxSleepMillis;
  int k = 0;
  for (long i = 1; i < steps; i++)
  {
    digitalWrite(pinStep_1, HIGH);
    digitalWrite(pinStep_2, HIGH);
    delayMicroseconds(delayMS);        // pauses for 50 microseconds
    if (i % 100 == 0 && delayMS > minSleepMillis)
    {
      delayMS = delayMS - 4;
    }

    digitalWrite(pinStep_1, LOW);
    digitalWrite(pinStep_2, LOW);
  }
}

void RunStepper1(long steps, int dir)
{
  // wake up stepper
  // initialize stepper
  digitalWrite(pinMS1_1, stepperMS1);
  digitalWrite(pinMS2_1, stepperMS2);
  digitalWrite(pinMS3_1, stepperMS3);
  digitalWrite(pinStep_1, LOW);
  digitalWrite(pinSleep_1, HIGH);
  digitalWrite(pinDir_1, dir);

  int delayMS = maxSleepMillis;
  int k = 0;
  for (long i = 1; i < steps; i++)
  {
    digitalWrite(pinStep_1, HIGH);
    delayMicroseconds(delayMS);        // pauses for 50 microseconds
    if (i % 100 == 0 && delayMS > minSleepMillis)
    {
      delayMS = delayMS - 4;
    }

    digitalWrite(pinStep_1, LOW);
  }
}

void RunStepper2(long steps, int dir)
{
  // wake up stepper
  // initialize stepper
  digitalWrite(pinMS1_2, stepperMS1);
  digitalWrite(pinMS2_2, stepperMS2);
  digitalWrite(pinMS3_2, stepperMS3);
  digitalWrite(pinStep_2, LOW);
  digitalWrite(pinSleep_2, HIGH);
  digitalWrite(pinDir_2, dir);

  int delayMS = maxSleepMillis;
  int k = 0;
  for (long i = 1; i < steps; i++)
  {
    digitalWrite(pinStep_2, HIGH);
    delayMicroseconds(delayMS);        // pauses for 50 microseconds
    if (i % 100 == 0 && delayMS > minSleepMillis)
    {
      delayMS = delayMS - 4;
    }

    digitalWrite(pinStep_2, LOW);
  }
}

void initalizeTextDisplay(void)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("");
  display.display();
}

void showOnDisplay(String outPutText)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(outPutText);
  display.display();
}

void showOnDisplay(String outPutText1, String outPutText2)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(outPutText1);
  display.println(outPutText2);
  display.display();
}

void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
}











