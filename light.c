#include "allegro.h"

#include "config.h"

#include "grid.h"

#include "globvar.h"

// all light_struct information (inc NO_LIGHTS) also declared in display.c.
#define NO_LIGHTS 100


struct light_struct
{
 int x;
 int y;
 int level;
 int size;
 char light_or_dark;
};

struct light_struct light [NO_LIGHTS];


//void add_light(int lx, int ly, int bright);
void add_light(int x, int y, int bright, int size, char light_or_dark);
void light_circle1(int lx, int ly, int bright, char corners);
void light_circle2(int lx, int ly, int bright, char corners);
void light_circle3(int lx, int ly, int bright, char corners);
int brick_opaque(int btype);
void cascade_light(void);

int get_light_level(int lx, int ly);


void add_light(int x, int y, int bright, int size, char light_or_dark)
{

 int i;

 for (i = 0; i < NO_LIGHTS; i ++)
 {
  if (i == NO_LIGHTS - 1)
   return; // too many.
  if (light[i].level == 0)
   break;
 }

 light[i].x = x;
 light[i].y = y;
 if (bright < 0) bright = 0;
 if (bright > 4) bright = 4;
 light[i].level = bright;
 light[i].size = size;
 light[i].light_or_dark = light_or_dark;
 
 light[i + 1].level = 0;

}


void init_lightmap(void)
{

// clear_to_color(arena[0].light, 0);
// clear_to_color(arena[0].base_light, 0);
// cascade_light();

 int i;

 for (i = 0; i < NO_LIGHTS; i ++)
 {
  light[i].level = 0;
 }

}

void cascade_light(void)
{
// int i, j, started;

 return;
/*
 for (i = 0; i < arena[0].bricks_x; i ++)
 {
  for (j = 0; j < arena[0].bricks_y; j ++)
  {
   if (brick_opaque(get_brick(i, j)) == 0)
    started = 1;
   putpixel(arena[0].base_light, i, j, 20); // arena[0].ambient_light
   if (brick_opaque(get_brick(i, j)) == 0)
   {
    if (getpixel(arena[0].base_light, i + 1, j) < 30 / 2)
     putpixel(arena[0].base_light, i + 1, j, 30 / 2);
    if (getpixel(arena[0].base_light, i - 1, j) < 30 / 2)
     putpixel(arena[0].base_light, i - 1, j, 30 / 2);
   }
   if (started == 1 && brick_opaque(get_brick(i, j)) == 1)
    break;
  }
  started = 0;
 }*/

}

int brick_opaque(int btype)
{

/*
 switch(btype)
 {
  case BRICK_ROCK: return 1;
  case BRICKBACK_ROCK: return 0;
 }*/

 return 1;
}

void turn_light(void)
{

 light[0].level = 0;

// clear_to_color(arena[0].base_light, 10);
/* if (arena[0].nuke_flash > 0 && arena[0].nuke_flash <= 2)
  clear_to_color(arena[0].light, 50);
   else
    blit(arena[0].base_light, arena[0].light, 0, 0, 0, 0, arena[0].bricks_x, arena[0].bricks_y);
    */
// clear_bitmap(arena[0].light);
}


int get_light_level(int lx, int ly)
{
 if (lx < 0 || lx >= arena[0].bricks_x || ly < 0 || ly >= arena[0].bricks_y)
  return 0; // arena[0].ambient_light;

  return 1; //getpixel(arena[0].light, lx, ly) / 10;

}

int get_light_level_pixel(int lx, int ly)
{
 if (lx < 0 || lx >= arena[0].max_x * GRAIN || ly < 0 || ly >= arena[0].max_y * GRAIN)
  return 0; // arena[0].ambient_light;

  return 1; //getpixel(arena[0].light, (lx / GRAIN) / BRICK_X, (ly / GRAIN) / BRICK_Y) / 10;

}

int place_dark(int lx, int ly, int bright)
{
 if (bright <= 0)
  return 0;

 add_light(lx, ly, bright, bright, 0);
 return 1;
}


int place_light(int lx, int ly, int bright)
{
 if (game[0].lightsourcing == 0)
  return 0;

 if (bright <= 0)
  return 0;

 add_light(lx, ly, bright, bright, 1);

 if (arena[0].need_actor_lit == 1)
 {
  int i;
  for (i = 0; i < NO_ACTORS; i ++)
  {
   if (actor[i].aclass == ACLASS_PLAYER
       && actor[i].lit == 0)
   {
    if (actor[i].x > lx - (bright * GRAIN)
        && actor[i].x < lx + (bright * GRAIN)
        && actor[i].y > ly - (bright * GRAIN)
        && actor[i].y < ly + (bright * GRAIN))
         actor[i].lit = 10;
   }
  }

 }
 
 return 1;

}

/*void light_circle1(int lx, int ly, int bright, char corners)
{
  add_light(lx + 1, ly, bright);
  add_light(lx - 1, ly, bright);
  add_light(lx, ly + 1, bright);
  add_light(lx, ly - 1, bright);
  if (!corners) return;
  
  add_light(lx - 1, ly + 1, bright);
  add_light(lx - 1, ly - 1, bright);
  add_light(lx + 1, ly + 1, bright);
  add_light(lx + 1, ly - 1, bright);
}

void light_circle2(int lx, int ly, int bright, char corners)
{

  add_light(lx + 2, ly - 1, bright);
  add_light(lx + 2, ly, bright);
  add_light(lx + 2, ly + 1, bright);
  add_light(lx - 2, ly - 1, bright);
  add_light(lx - 2, ly, bright);
  add_light(lx - 2, ly + 1, bright);
  add_light(lx - 1, ly - 2, bright);
  add_light(lx, ly - 2, bright);
  add_light(lx + 1, ly - 2, bright);
  add_light(lx - 1, ly + 2, bright);
  add_light(lx, ly + 2, bright);
  add_light(lx + 1, ly + 2, bright);
  if (!corners) return;
  
  add_light(lx + 2, ly + 2, bright);
  add_light(lx + 2, ly - 2, bright);
  add_light(lx - 2, ly + 2, bright);
  add_light(lx - 2, ly - 2, bright);

}

void light_circle3(int lx, int ly, int bright, char corners)
{

  add_light(lx + 3, ly - 2, bright);
  add_light(lx + 3, ly - 1, bright);
  add_light(lx + 3, ly, bright);
  add_light(lx + 3, ly + 1, bright);
  add_light(lx + 3, ly + 2, bright);
  add_light(lx - 3, ly - 2, bright);
  add_light(lx - 3, ly - 1, bright);
  add_light(lx - 3, ly, bright);
  add_light(lx - 3, ly + 1, bright);
  add_light(lx - 3, ly + 2, bright);
  add_light(lx - 2, ly - 3, bright);
  add_light(lx - 1, ly - 3, bright);
  add_light(lx, ly - 3, bright);
  add_light(lx + 1, ly - 3, bright);
  add_light(lx + 2, ly - 3, bright);
  add_light(lx - 2, ly + 3, bright);
  add_light(lx - 1, ly + 3, bright);
  add_light(lx, ly + 3, bright);
  add_light(lx + 1, ly + 3, bright);
  add_light(lx + 2, ly + 3, bright);
  if (!corners) return;
  
  add_light(lx + 3, ly + 3, bright);
  add_light(lx + 3, ly - 3, bright);
  add_light(lx - 3, ly + 3, bright);
  add_light(lx - 3, ly - 3, bright);

}*/

/*
void add_light(int lx, int ly, int bright)
{

 if (lx < 0 || lx >= arena[0].bricks_x || ly < 0 || ly >= arena[0].bricks_y)
  return;

 int got = getpixel(arena[0].light, lx, ly);

/ * if (got < bright)
  got = bright + got / 3;
   else
   {
    got = got + bright / 3;
   }* /
 got += bright;

 if (got >= 60) got = 60;

 putpixel(arena[0].light, lx, ly, got);

}
*/

