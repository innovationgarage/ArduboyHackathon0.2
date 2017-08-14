#include "graphics.h"
#include "tune.h"
#include "Star.h"
#include <Arduboy2.h>
#include <ArduboyPlaytune.h>

Arduboy2 arduboy;
ArduboyPlaytune tunes(arduboy.audio.enabled);

#define SCREEN_WIDTH WIDTH          // Screen width in pixels.
#define SCREEN_HEIGHT HEIGHT         // Screen height in pixels.

// Program constants.
#define STAR_COUNT 30            // Number of stars on the screen. Arduino UNO maxes out around 250.
#define BACKGROUND_COLOR BLACK   // Background color in hex. 0x0000 is black.
#define STAR_SPEED_MIN 1          // Minimum movement in pixels per update. (value is inclusive)
#define STAR_SPEED_MAX 50         // Maximum movement in pixels per update. (value is inclusive)
#define STAR_COLOR WHITE         // Star color in hex. 0xffff is white.

// Global variables.
Star stars[STAR_COUNT];                                             // An array of star instances.
float travelx = 0;
float travely = 0;

int curSpeed = 25;
int minSpeed = 1;
long actualTime = 0;
long speedMills;
int starCount = STAR_COUNT;
bool showInfo = false;

void setup(void)
{
  // initiate arduboy instance
  arduboy.begin();

  arduboy.setFrameRate(60);
  arduboy.clear();

  starsSetup();
  arduboy.initRandomSeed();

  // audio setup
  tunes.initChannel(PIN_SPEAKER_1);
#ifndef AB_DEVKIT
  // if not a DevKit
  tunes.initChannel(PIN_SPEAKER_2);
#else
  // if it's a DevKit
  tunes.initChannel(PIN_SPEAKER_1); // use the same pin for both channels
  tunes.toneMutesScore(true);       // mute the score when a tone is sounding
#endif

}

void starsSetup()
{
  // Loop through each star.
  for (int i = 0; i < STAR_COUNT; i++)
  {
    // Randomize its position and speed.
    stars[i].randomize(-200, 200, -200, 200, 200, 200, STAR_SPEED_MIN, STAR_SPEED_MAX);
  }
}

void handleButtons() {
  arduboy.pollButtons();
  if (arduboy.pressed(UP_BUTTON) == true ) {
    travely = 0.03;
  }
  if (arduboy.pressed(DOWN_BUTTON) == true ) {
    travely = -0.03;
  }
  if (arduboy.pressed(LEFT_BUTTON) == true )
    travelx = -0.05;
  if (arduboy.pressed(RIGHT_BUTTON) == true )
    travelx = 0.05;
  if (arduboy.justPressed(A_BUTTON) == true ) {
    showInfo = false;
    //faster = true;
  } if (arduboy.justReleased(A_BUTTON) == true ) {
    //faster = false;
  }
  if (arduboy.justPressed(B_BUTTON) == true ) {
    showInfo = true;
    //slower = true;
  } if (arduboy.justReleased(B_BUTTON) == true ) {
    //slower = false;
  }
  if (arduboy.pressed(A_BUTTON + B_BUTTON) && arduboy.justPressed(UP_BUTTON) ) {
    travelx = 0;
  }
  if (arduboy.pressed(A_BUTTON + B_BUTTON) && arduboy.justPressed(DOWN_BUTTON) ) {
    travely = 0;
  }
  if (arduboy.justReleased(UP_BUTTON) == true ) {
    travely = 0;
  }
  if (arduboy.justReleased(DOWN_BUTTON) == true ) {
    travely = 0;
  }
  if (arduboy.justReleased(LEFT_BUTTON) == true ) {
    travelx = 0;
  }
  if (arduboy.justReleased(RIGHT_BUTTON) == true ) {
    travelx = 0;
  }
}

void loop(void)
{
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  if (!tunes.playing())
    tunes.playScore(score);

  arduboy.clear();
  handleButtons();
  starsDraw(travelx, travely);

  // logo
  if(showInfo)
  {
    const int minx = 9, maxx =10;
    arduboy.setCursor(20,3);
    arduboy.println("CONGRATULATIONS");
    arduboy.setCursor(3,15);
    arduboy.println("On your epic victory!");
    arduboy.setCursor(2,15+9);
    arduboy.println("what the heck is this");
    arduboy.setCursor(7,15+18);
    arduboy.println("thing that you won?");

    arduboy.setCursor(20,54-9);
    arduboy.println("Find out here:");
    arduboy.setCursor(9,54);
    arduboy.println("dualoghackaton.com");
    arduboy.invert(true);
  }
  else
  {
    arduboy.invert(false);
    arduboy.drawBitmap(0, 0, Hackaton0, 128, 64);
  }

  arduboy.display();
}

void starsDraw(float travelx, float travely)
{
  // Loop through each star.
  for (int i = 0; i < starCount; i++)
  {
    stars[i].z = stars[i].z - curSpeed;

    if (travely != 0 ) {
      float temp_y = stars[i].y;
      float temp_z = stars[i].z;
      stars[i].y = temp_y * cos(travely) - temp_z * sin(travely);
      stars[i].z = temp_z * cos(travely) + temp_y * sin(travely);
    }
    if (travelx != 0 ) {
      float temp_x = stars[i].x;
      float temp_y = stars[i].y;
      stars[i].x = temp_x * cos(travelx) - temp_y * sin(travelx);
      stars[i].y = temp_y * cos(travelx) + temp_x * sin(travelx);
    }
    //caclulate screen position from 3D space
    stars[i].screen_x = stars[i].x / stars[i].z * 100 + WIDTH / 2;
    stars[i].screen_y = stars[i].y / stars[i].z * 100 + HEIGHT / 2;

    actualTime = millis();
    /*if (faster == true && slower == false && curSpeed < STAR_SPEED_MAX && actualTime - speedMills >= 100) {
      curSpeed = curSpeed + 1;
      minSpeed = STAR_SPEED_MIN;
      speedMills = actualTime;

    }
    if (slower == true && faster == false && curSpeed != STAR_SPEED_MIN && actualTime - speedMills >= 100) {
      curSpeed = curSpeed - 1;
      minSpeed = STAR_SPEED_MIN;
      speedMills = actualTime;
    }*/

    //If the stars go off the screen remove them and re-draw. if the stars hang out in the center remove them also
    if (stars[i].screen_x > WIDTH || stars[i].screen_x < 0 || stars[i].screen_y > HEIGHT || stars[i].screen_y < 0 || (stars[i].screen_x == WIDTH / 2 && stars[i].screen_y == HEIGHT / 2) ) {
      stars[i].randomize(-500, 1000, -500, 1000, 100, 2000, STAR_SPEED_MIN, STAR_SPEED_MAX);

      stars[i].screen_x = stars[i].x / stars[i].z * 100 + WIDTH / 2;
      stars[i].screen_y = stars[i].y / stars[i].z * 100 + HEIGHT / 2;
      stars[i].old_screen_x = stars[i].screen_x;
      stars[i].old_screen_y = stars[i].screen_y;
    }
    
    // Draw the star at its new coordinate.
    arduboy.drawLine(stars[i].screen_x, stars[i].screen_y, stars[i].old_screen_x, stars[i].old_screen_y, STAR_COLOR);

    //keep track of the old spot
    stars[i].old_screen_x = stars[i].screen_x;
    stars[i].old_screen_y = stars[i].screen_y;
  }
}
