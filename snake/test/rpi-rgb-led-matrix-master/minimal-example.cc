// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

static void DrawOnCanvas(Canvas *canvas) {
  /*
   * Let's create a simple animation. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   */
  canvas->Fill(0, 0, 0);

  /*int center_x = canvas->width() / 2;
  int center_y = canvas->height() / 2;
  float radius_max = canvas->width() / 2;
  float angle_step = 1.0 / 360;
  for (float a = 0, r = 0; r < radius_max; a += angle_step, r += angle_step) {
    float dot_x = cos(a * 2 * M_PI) * r;
    float dot_y = sin(a * 2 * M_PI) * r;
    canvas->SetPixel(center_x + dot_x, center_y + dot_y,
                     255, 0, 0);
    usleep(1 * 1000);  // wait a little to slow down things.
  }*/

  const int stepSize = 10;
  int numWhitePixels = 0;
  const int maxValue = 100; //limit how bright the screen gets
  const int matrixWidth = canvas->width();
  const int matrixHeight = canvas->height();
  //const float maxRadius = canvas->width() / 2; 
  //short int pixels [32][32] = {{0}};
  short int colorPixels[32][32][3] = {{{0}}};
  int rand_x;
  int rand_y;
  int rand_rgb;
  //bool everyOther = false;
  int radius;

  /*
  Randomly increase brightness of each pixel until all are white
  */
  /*while(numWhitePixels < (matrixWidth * matrixHeight)){
    rand_x = rand() % matrixWidth;
    rand_y = rand() % matrixHeight;

    if(pixels[rand_x][rand_y] < maxValue){
      pixels[rand_x][rand_y] += stepSize;
      canvas->SetPixel(rand_x, rand_y, pixels[rand_x][rand_y], pixels[rand_x][rand_y], pixels[rand_x][rand_y]);
      if(pixels[rand_x][rand_y] >= maxValue)
        numWhitePixels += 1;
    }
    usleep(1000 / (int) pow(1.25, numWhitePixels));
  }*/
  //end while

  /*
  Randomly add to each color until all pixels white
  */
  /*while(numWhitePixels < (matrixWidth * matrixHeight)){
    if(everyOther){
      rand_x = (rand() % (matrixWidth / 2)) + 8;
      rand_y = (rand() % (16 - (rand_x - 8))) + 8;
    } else{
      rand_x = rand() % matrixWidth;
      rand_y = rand() % matrixHeight;
      usleep(100 / (int) pow(10, numWhitePixels));
      rand_rgb = rand() % 3; //0 = r, 1 = g, 2 = b
    }
    if(colorPixels[rand_x][rand_y][rand_rgb] < maxValue){
      colorPixels[rand_x][rand_y][rand_rgb] += stepSize;
      canvas->SetPixel(rand_x, rand_y, colorPixels[rand_x][rand_y][0], colorPixels[rand_x][rand_y][1], colorPixels[rand_x][rand_y][2]);
      if((colorPixels[rand_x][rand_y][0] >= maxValue) && (colorPixels[rand_x][rand_y][1] >= maxValue) && (colorPixels[rand_x][rand_y][2] >= maxValue))
        numWhitePixels += 1;
    }
    //usleep(100 / (int) pow(10, numWhitePixels));
    everyOther = !everyOther;
  }*/
  //end while

  /* Dim all pixels back to black
  for(int Iwidth = 0; Iwidth < matrixWidth; Iwidth++){
    for(int Jheight = 0; Jheight < matrixHeight; Jheight++){
      pixels[Iwidth][Jheight] -= stepSize;
      canvas->SetPixel(Iwidth, Jheight, pixels[Iwidth][Jheight], pixels[Iwidth][Jheight], pixels[Iwidth][Jheight]);
    }
    usleep(100);
  }*/

  /*
  Increment random color until all pixels white. Higher probability of center circle being incremented.
  */
  int everySix = 0;
  int stepMultiplier;
  while(numWhitePixels < (matrixWidth * matrixHeight)){
    everySix += 1;
    if((everySix % 3) == 0){
      radius = rand() % ((int) ((matrixWidth / 2) * (2/3)) + 1);
      stepMultiplier = 2;
      if(radius > 0)
        rand_x = rand() % radius;
      else
        rand_x = 0;
      rand_y = sqrt(pow(radius, 2) - pow(rand_x, 2)); //Formula for circle
      rand_x += 16; //Center of display is (16,16)
      if((everySix % 6) == 0)
        rand_y *= -1;
    } else{
      rand_x = rand() % matrixWidth;
      rand_y = rand() % matrixHeight;
      stepMultiplier = 1;
    }

    usleep(100 / (int) pow(10, numWhitePixels));
    rand_rgb = rand() % 3; //0 = r, 1 = g, 2 = b

    if(colorPixels[rand_x][rand_y][rand_rgb] < maxValue){
      colorPixels[rand_x][rand_y][rand_rgb] += (stepSize * stepMultiplier);
      canvas->SetPixel(rand_x, rand_y, colorPixels[rand_x][rand_y][0], colorPixels[rand_x][rand_y][1], colorPixels[rand_x][rand_y][2]);
      if((colorPixels[rand_x][rand_y][0] >= maxValue) && (colorPixels[rand_x][rand_y][1] >= maxValue) && (colorPixels[rand_x][rand_y][2] >= maxValue))
        numWhitePixels += 1;
    }
    //usleep(100 / (int) pow(10, numWhitePixels));
    std::cout << "(" << rand_x << ", " << rand_y << ")" << std::endl;
  }
  //end while

}

int main(int argc, char *argv[]) {
  /*
   * Set up GPIO pins. This fails when not running as root.
   */
  GPIO io;
  if (!io.Init())
    return 1;
    
  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  int rows = 32;   // A 32x32 display. Use 16 when this is a 16x32 display.
  int chain = 1;   // Number of boards chained together.
  Canvas *canvas = new RGBMatrix(&io, rows, chain);

  srand(time(0));

  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
