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
#include <vector>
#include <cstdlib>

//#include <windows.h>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using namespace std;

enum Direction{
   LEFT,
   RIGHT,
   UP,
   DOWN
};

class Snake{
private:

 struct node{
   int x;
   int y;
 };

 vector<node> body; 
 vector<node> food;

 void  fin();                         //End Game

public:

 void birth(int x, int y);      //Makes a new snake
 void move(Direction dir);      //Moves the snake one pixel in
                                //desginated direction
 void grow();                   //Adds a segment
 bool check_collision();        //checks for end game
                                //and grow events
                                //returns true for end game event
 void update(Canvas *canvas);   //updates the Led panel
 void gen_food();               //places "food" on a random pixel

};

static void DrawOnCanvas(Canvas *canvas) {
  /*
   * Let's create a simple Snake Game!. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   */

  int center_x = canvas->width() / 2;
  int center_y = canvas->height() / 2;

  int foodTimer = 0;
  int input;
  int next;

  Snake snake;
  snake.birth(center_x, center_y);// make this snake spawn


  while((next=getch()) != 27) {
    if(next != -1)
       input = next;


    if(input == 119) {
      snake.move(UP);
    } else if(input == 115) {
      snake.move(DOWN);
    } else if(input ==97) {
      snake.move(LEFT);
    } else if(input == 100) {
      snake.move(RIGHT);
    } else if(input == 101) {
      break;
    }


    if(snake.check_collision()) //if you died reset the snake
      snake.birth(center_x, center_y);

   
   if((foodTimer++) % 10 == 0)
     snake.gen_food();

    snake.update(canvas);


    usleep(250000);
    
  }
}

//void drawPixel(int x, int y) {
//  canvas->SetPixel(x,y,255,0,0);
//}

int main(int argc, char *argv[]) {
  /*
   * Set up GPIO pins. This fails when not running as root.
   */
  GPIO io;
  if (!io.Init())
    return 1;
  
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

  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  endwin();
  return 0;
}


void Snake::gen_food(){

 int xRan, yRan;
 bool conflict;

 do{
  conflict = false;

  xRan = rand() % 32;
  yRan = rand() % 32;

  for(int i = 0; i < body.size(); i++)
    if(body[i].x == xRan && body[i].y == yRan)
      conflict = true;

 }while(conflict);

 node temp;
 temp.x = xRan;
 temp.y = yRan;

 food.push_back(temp);

}

void Snake::birth(int x, int y){

 node temp;

 for(int i = 0; i < 5; i++){
   temp.x = x;
   temp.y = y + i;

   if(temp.y > 31) //wraparound
     temp.y = 0;

   body.push_back(temp);
 }

}

void Snake::move(Direction dir){

 vector<node>::iterator iter;

 node swap;
 node dest = *body.begin();

 switch(dir){
  case UP:    swap.y--;
              if(swap.y < 0)
                swap.y = 31;
  break;
  case DOWN:  swap.y = (swap.y + 1) % 31;
  break;
  case LEFT:  swap.x--;
              if(swap.x < 0)
                swap.x = 31;
  break;  
  case RIGHT: swap.x = (swap.x + 1) % 31;
  break;
 }
 
 for(; iter != body.end(); iter++){
   swap = *iter;
   *iter = dest;
   dest = swap;
 }
}


void Snake::update(Canvas *canvas){

 canvas->Fill(0, 0, 0);

 for(int i = 0; i < body.size(); i++){

    canvas->SetPixel(body[i].x,body[i].y,255,0,0);  

 }

 for(int i = 0; i < food.size(); i++){

    canvas->SetPixel(food[i].x,food[i].y,0,0,255);  

 }

}

void Snake::grow(){

 node temp;

 body.push_back(temp);

}


void Snake::fin(){

  //TODO output the score and reset play

  cin.ignore();

}


bool Snake::check_collision(){
  vector<node>::iterator iter;

  //check for head to body collision
  for(iter = body.begin(); iter != body.end(); iter++){

     if(body.begin()->x == iter->x &&
        body.begin()->y == iter->y  ){

       fin();

       return true;
     }
  }

  //Check for food collision
  for(iter = food.begin(); iter != food.end(); iter++){

     if(body.begin()->x == iter->x &&
        body.begin()->y == iter->y  ){

       grow();
     }

  }
    
 return false;

}
