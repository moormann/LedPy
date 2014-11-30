#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <cstdlib>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using namespace std;

const int MAX_APPLES = 5;

enum Direction{
  LEFT,
  RIGHT,
  UP,
  DOWN
};

class Snake {

  private:
    int disco;
    int r,g,b;
    int time;
    bool dead;
    int num_apples;

    struct pixel{
      int  x;
      int  y;
      bool spec;
    };

    vector<pixel> snake1; 
    vector<pixel> snake2;
    vector<pixel> food;
   
    Canvas *canvas;

    void fin();                    //End Game

  public:
    Snake(Canvas *canvas);
    void Begin_Game();
    bool discoAintDead();          //Time to boogie
    void birth(int x, int y);      //Makes a new snake
    void move(Direction dir);      //Moves the snake one pixel in desginated direction
    void grow();                   //Adds a segment
    bool check_collision();        //Checks for end game and grow events
                                   //returns true for end game event
    void update();                 //Updates the Led panel
    void gen_food();               //Places "food" on a random pixel

    int score;
};

static int DrawOnCanvas(Canvas *canvas) {
  /*
   * Let's create a simple Snake Game!. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   */
  Snake snake(canvas);

  snake.Begin_Game();

  return snake.score;
}

int main(int argc, char *argv[]) {
  bool again = true;
  int score;

  while(again) {

    // Set up GPIO pins. This fails when not running as root.
    GPIO io;
    if (!io.Init())
      return 1;
    
    initscr();
    noecho();
    raw();
    nodelay(stdscr, TRUE);
    
    // Set up the RGBMatrix. It implements a 'Canvas' interface.
    int rows = 32;   // A 32x32 display. 
    int chain = 1;   // Number of boards chained together.
    Canvas *canvas = new RGBMatrix(&io, rows, chain);

    score = DrawOnCanvas(canvas);    // Using the canvas.

    // Animation finished. Shut down the RGB matrix.
    initscr();
    canvas->Clear();
    delete canvas;

    cout << "Score: " << score << endl << endl << "Play again? (y/n)";
    int input = 0;
    while((input=getch()) != 27) {
      if(input == 121) {
        again = true;
        break;
      } else if(input == 110) {
        again = false;
        break;
      }
    }
  }

  endwin();

  return 0;
}

Snake::Snake(Canvas *can){
  canvas = can;
  disco = 0;
  r = 255;
  b = 0;
  g = 0;
  time = 250000;
  num_apples = 0;
  score = 0;
}

void Snake::Begin_Game(){

  int center_x = canvas->width() / 2;
  int center_y = canvas->height() / 2;

  int foodTimer = 0;
  int input = 97;
  int next = 100;
  int last;

  bool sngPlay = true;

  if(sngPlay){
    birth(center_x, center_y); // make this snake spawn
  } else {
  
  }

  while((next=getch()) != 27) {
    fflush(stdin);

    if(next != -1) {
      last = input;   
      input = next;
    }

    if(input == 119) {
      if(last != 115)
        move(UP);
      else {
        move(DOWN);
        input = 115;
      }

    } else if(input == 115) {
      if(last != 119)
        move(DOWN);
      else {
        move(UP);
        input = 119;
      }

    } else if(input == 97) {
      if(last != 100)
        move(LEFT);
      else {
        move(RIGHT);
        input = 100;
      }

    } else if(input == 100) {
      if(last != 97)
        move(RIGHT);
      else {
        move(LEFT);
        input = 97;
      }

    } else if(input == 101) {
      break;
    } else if(input == -1){
      //do nothing
    } else {
      //cin.ignore(); //pause
    }

    if(check_collision()) //if you died reset the snake
      break;
   
    if(((foodTimer++) % 20 == 0) && (num_apples < MAX_APPLES))
      gen_food();

    update();

    if(discoAintDead()) {
      usleep(time / 3); 
    } else {
      usleep(time);
    }
  }
}

void Snake::gen_food(){

  int xRan, yRan;
  bool conflict;

  do {
    conflict = false;

    xRan = rand() % 32;
    yRan = rand() % 32;

    //Check for food on snake1 conflict
    for(unsigned int i = 0; i < snake1.size(); i++) {
      if(snake1[i].x == xRan && snake1[i].y == yRan)
        conflict = true;
    }

    //Check for food on food conflict
    for(unsigned int i = 0; i < food.size(); i++) {
      if(food[i].x == xRan && food[i].y == yRan)
        conflict = true;
    }

  } while(conflict);

  num_apples++;

  pixel temp;
  temp.x = xRan;
  temp.y = yRan;
  temp.spec = (rand() % 100 < 10)? true : false;

  food.push_back(temp);
}

void Snake::birth(int x, int y) {

  pixel temp;
  snake1.clear();
  dead = false;

  for(int i = 0; i < 5; i++) {
    temp.x = x;
    temp.y = y + i;

    if(temp.y > 31) //wraparound
      temp.y = 0;

    snake1.push_back(temp);
  }
}

void Snake::move(Direction dir){

  vector<pixel>::iterator iter;

  pixel swap;
  pixel dest  = *snake1.begin();

  switch(dir){
    case UP:
      dest.y--;
      if(dest.y < 0){
        dest.y = 31;
        //dest.x = rand() % 32;
      }
    break;
  
    case DOWN:  
      dest.y++;
      if(dest.y > 31){  
        dest.y = 0;
        //dest.x = rand() % 32;
      }
    break;
  
    case LEFT:  
      dest.x--;
      if(dest.x < 0){
        //dest.y = rand() % 32;
        dest.x = 31;
      }
    break;  
  
    case RIGHT: 
      dest.x++;
      if(dest.x > 31){  
        dest.x = 0;
        //dest.y = rand() % 32;
      }
    break;
  } 
 
  for(iter = snake1.begin(); iter != snake1.end(); iter++){
    swap  = *iter;
    *iter = dest;
    dest  = swap;
  }
}

void Snake::update(){

  canvas->Clear();
  static bool hes = false; 

  if(disco != 0) {
    if(hes) {
      r = rand() % 256;
      g = rand() % 256;
      b = rand() % 256;
    }

    hes = (disco % 10)? true : false;
    canvas->Fill(255 - r, 255 - g, 255 - b);
    disco--;
  
  } else if(!dead) {
    r = 255;
    g = 0;
    b = 0;
  }

  for(unsigned int i = 0; i < snake1.size(); i++) {
    canvas->SetPixel(snake1[i].x,snake1[i].y,r,g,b);  
  }

  for(unsigned int i = 0; i < food.size(); i++) {
    if(food[i].spec)
      canvas->SetPixel(food[i].x,food[i].y, rand() % 256, rand() % 256, rand() % 256);
    else
      canvas->SetPixel(food[i].x,food[i].y,0,255,0);  
  }
}

void Snake::grow(){
  pixel temp;
  snake1.push_back(temp);

  if(time > 50000)
    time -= 10000;
}

void Snake::fin(){
  //TODO output the score and reset play  

  disco = 0;
  dead = true;

  while(r != 0) {
    r--;
    update();
    usleep(5000);
  }

  food.clear();
  cin.ignore();
}

bool Snake::check_collision(){
  vector<pixel>::iterator iter;

  //check for head to snake1 collision
  for(iter = snake1.begin() + 1; iter != snake1.end(); iter++){

    if(snake1.begin()->x == iter->x && snake1.begin()->y == iter->y) {
      fin();

      return true;
    }
  }

  //Check for food collision
  for(iter = food.begin(); iter != food.end(); iter++){

    if(snake1.begin()->x == iter->x && snake1.begin()->y == iter->y) {

      if(iter->spec)
        disco += 81;

      food.erase(iter);
      grow();
      num_apples--;
      score++;
      break;
    }
  }
    
  return false;
}

bool Snake::discoAintDead() {

  if(disco != 0)
    return true;
  else
    return false;
}
