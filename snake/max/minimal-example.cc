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
#include <iostream>
#include <ncurses.h>
#include <cstdlib> //includes rand
#include <ctime> //time library for seeding rand

//#include <windows.h>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using namespace std;

static void DrawOnCanvas(Canvas *canvas) {
  /*
   * Let's create a simple animation. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   */
  //canvas->Fill(0, 0, 255);

  int center_x = canvas->width() / 2;
  int center_y = canvas->height() / 2;

  int x = center_x;
  int y = center_y;
  int input;
  int next;

  int len = 5;
  int xArr[5] = {0};
  int yArr[5] = {0};

  while((next=getch()) != 27) {
    if(next != -1)
       input = next;

    canvas->SetPixel(xArr[len-1],yArr[len-1],0,0,0);

       for(int i = len - 2; i > -1; i--){
        yArr[i+1] = yArr[i];
  xArr[i+1] = xArr[i];
       }


    if(input == 119) {
        y--;
        if(y < 0)
          y = 31;
    } else if(input == 115) {
      y++;
      if(y > 31)
        y = 0;
    } else if(input ==97) {
      x--;
      if(x < 0)
        x = 31;
    } else if(input == 100) {
      x++;
      if(x > 31)
        x = 0;
    } else if(input == 101) {
      break;
    }
    yArr[0] = y;
    xArr[0] = x;
    canvas->SetPixel(x,y,255,0,0);  
    
    usleep(250000);
    
  }
}

//void drawPixel(int x, int y) {
//  canvas->SetPixel(x,y,255,0,0);
//}

static void EtchASketchDemo(int pixels[3][32][32][3]){
    /*start etch a sketch var declarations*/
  int etch_x = 0;
  int etch_y = 0;
  int etch_count = 0;
  int etch_rand = rand() % 32;
  int etch_direction = rand() % 4;
  //int r[5] = {0};
  /*end etch a sketch var declarations*/

/*Update etch-as-sketch*/
  if(pixels[0][etch_x][etch_y][0] == 255){ //if pixel is red, make blue
    pixels[0][etch_x][etch_y][0] = 0;
    pixels[0][etch_x][etch_y][1] = 255;
    pixels[0][etch_x][etch_y][2] = 0;
  }
  else if(pixels[0][etch_x][etch_y][1] == 255){ //if green, make blue
    pixels[0][etch_x][etch_y][0] = 0;
    pixels[0][etch_x][etch_y][1] = 0;
    pixels[0][etch_x][etch_y][2] = 255;
  }
  else{ //if blue or uncolored, make red
    pixels[0][etch_x][etch_y][0] = 255;
    pixels[0][etch_x][etch_y][1] = 0;
    pixels[0][etch_x][etch_y][2] = 0;
  }


  if(etch_rand == 0 || (etch_direction == 3 && etch_x >= 31) || (etch_direction == 0 && etch_y >= 31) || (etch_direction ==2 && etch_x <= 0) || (etch_direction == 1 && etch_y <= 0)){
    etch_direction = rand() % 4;
  }
  else{
    etch_rand--;
  }
  switch(etch_direction){
    case 0: //down
      etch_y++;
      break;
    case 1: //up
      etch_y--;
      break;
    case 2: //left
      etch_x--;
      break;
    case 3: //right
      etch_x++;
      break;
  }
  /*End etch-a-sketch*/
  //r[0] = etch_x;
  //r[1] = etch_y;
  //r[2] = pixels[0][etch_x][etch_y][0];
  //r[3] = pixels[0][etch_x][etch_y][1];
  //r[4] = pixels[0][etch_x][etch_y][2];
  //return r;
}

char MenuFunction(int currentScreen, Canvas *canvas){
  const int numScreens = 3;
  const int width = 32;
  const int length = 32;
  const int rgb = 3;
  int *returned;
  int pixels[3][32][32][3] = {0};
  int next;
  int currentWindow = 0;

  while((next=getch()) != 32); //if user presses spacebar, exit loop
  EtchASketchDemo(pixels); //update etchasketch panel (pixels[0])
  for(int i = 0; i < 32; i++){
    for(int j = 0; j < 32; j++){
      canvas->SetPixel(i, j, pixels[currentWindow][i][j][0], pixels[currentWindow][i][j][1], pixels[currentWindow][i][j][2]);
    }
  }
}




int main(int argc, char *argv[]) {
  char selection = 'E';
  /*
   * Set up GPIO pins. This fails when not running as root.
   */
  GPIO io;
  if (!io.Init())
    return 1;

  srand(time(0)); //seed random number generator. only needs to be called once per program
  
  initscr();
  noecho();
  raw();
  nodelay(stdscr, TRUE);
  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  int rows = 32;   // A 32x32 display. Use 16 when this is a 16x32 display.
  int chain = 1;   // Number of boards chained together.
  Canvas *canvas = new RGBMatrix(&io, rows, chain);

  selection = MenuFunction(selection, canvas);

  switch(selection){
    case 'E':
      //Etch-a-sketch
      break;
    case 'S':
      //Snake
      break;
    case 'B':
      //Battleship
      break;
    case 'P':
      //End program and shut down OS
      system("shutdown -h 00");
  }

  //DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  endwin();
  return 0;
}
