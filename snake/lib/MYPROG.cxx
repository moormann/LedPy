#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>


using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

static void DrawOnCanvas(Canvas *canvas) {
  
 int cursorPosX = 0, cursorPosY = 0;
 int command = 0;

 canvas->Fill(0,0,255);

 int color = 255;

 while(command != KEY_BACKSPACE){

   command = getch();

   canvas->SetPixel(cursorPosX, cursorPosY, color, 0, 0);

   if(color == 255){
     
     color = 0;

   }else{

     color = 255;

   }


   usleep(1000);

 }
 

/*
   * Let's create a simple animation. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   *
  canvas->Fill(0, 0, 255);

  int center_x = canvas->width() / 2;
  int center_y = canvas->height() / 2;
  float radius_max = canvas->width() / 2;
  float angle_step = 1.0 / 360;
  for (float a = 0, r = 0; r < radius_max; a += angle_step, r += angle_step) {
    float dot_x = cos(a * 2 * M_PI) * r;
    float dot_y = sin(a * 2 * M_PI) * r;
    canvas->SetPixel(center_x + dot_x, center_y + dot_y,
		     255, 0, 0);
    usleep(1 * 1000);  // wait a little to slow down things.
  }
*/
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

  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
