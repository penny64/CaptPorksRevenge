/*
This file doesn't do anything. It's here because I'm lazy.
*/

#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvar.h"

/*
void draw_direction_bullet(int put_x, int put_y, int dbull, BITMAP *bmp, BITMAP *forwards, BITMAP *u1, BITMAP *u2, BITMAP *u3, BITMAP *u4, BITMAP *u5, BITMAP *u6)
{
//  int bullet_stat = abs((bullet[dbull].angle * 12) / PI);
//  if (bullet[dbull].angle > PI) // ie if it's downwards
  BITMAP *to_draw;
//     if (bullet_state >= 21
  switch(abs((bullet[dbull].angle * 12) / PI))
  {
   case 0: to_draw = forwards; break;
   case 1: to_draw = u1; break;
   case 2: to_draw = u2; break;
   case 3: to_draw = u3; break;
   case 4: to_draw = u4; break;
   case 5: to_draw = u5; break;
   case 6: to_draw = u6; break;
   case 21: to_draw = u6; break;
   case 22: to_draw = u6; break;
   case 23: to_draw = u6; break;
   case 24: to_draw = u6; break;
   case 25: to_draw = u6; break;
   case 26: to_draw = u6; break;
   case 27: to_draw = u6; break;
  }

//draw_sprite(bmp, to_draw, put_x - 4, put_y - 4);

}
  */
