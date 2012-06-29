/*
Captain Pork's Revenge
Copyright (C) 2003 Captain Pork

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public Licence as published by
    the Free Software Foundation; either version 2 of the Licence, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public Licence for more details.

    You should have received a copy of the GNU General Public Licence
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    The GPL version 2 is included in this distribution in a file called
    LICENCE.TXT. Use any text editor or the TYPE command to read it.

File: grid.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - most of the game's functions for dealing with the arena map bitmaps.
   Eg changing their contents, detecting what's at a particular location,
   etc.
 - map generation is in level.c.
 - it's called 'grid.c' because, before the game included dirt, the level
   map was in the form of a grid with 30x20 pixel squares rather than
   bitmaps.
   
*/

#include "allegro.h"

#include "config.h"

#include "prand.h"
#include "globvar.h"

#include "bullet.h"

extern unsigned char grid [GRID_X] [GRID_Y];
extern unsigned char underlying_grid [GRID_X] [GRID_Y];
extern int grid_strength [GRID_X] [GRID_Y];


extern BITMAP *dirt_bmp;
extern BITMAP *shadow_bmp;

unsigned char get_dirt(int dirt_x, int dirt_y);


int get_brick(int brx, int bry)
{

 if (brx < 0 || brx >= arena[0].bricks_x || bry < 0 || bry >= arena[0].bricks_y)
  return BRICK_STONE; // arena[0].default_brick;

 return getpixel(arena[0].bricks, brx, bry);

}

unsigned char get_dirt(int dirt_x, int dirt_y)
{

// if (dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x && dirt_y < arena[0].max_y)
  return getpixel(shadow_bmp, dirt_x, dirt_y);

//  return 1;
}
// should add clipping so it can be changed to _getpixel for speed

unsigned char get_dirt_colour(int dirt_x, int dirt_y)
{

// if (dirt_x >= 0 && dirt_y >= 0 && dirt_x <= arena[0].max_x && dirt_y < arena[0].max_y)
  return getpixel(dirt_bmp, dirt_x, dirt_y);

//  return 1;
}
// should add clipping so it can be changed to _getpixel for speed

void change_dirt(int dirt_x, int dirt_y, int colour)
{
// int shadp = getpixel(shadow_bmp, dirt_x, dirt_y);
 int wasp;

// if (dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x - 1 && dirt_y < arena[0].max_y - 1 && shadp != 0 && shadp != PIX_PERM)
// if ( < arena[0].max_x - 1 && dirt_y < arena[0].max_y - 1)
 {
  wasp = ((getpixel(dirt_bmp, dirt_x, dirt_y) / 32) * 32) + colour % 32;
  _putpixel(dirt_bmp, dirt_x, dirt_y, wasp);
  if (prand(50) == 0
  && dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x - 1 && dirt_y < arena[0].max_y - 1)
   create_bullet(BULLET_SPECK, dirt_x * GRAIN, dirt_y * GRAIN, prand(300) - 150, prand(450),
    0, -1, 10 + prand(40), 0, 10, 0, 0, wasp, 0);
 }
// {
//  _putpixel(dirt_bmp, dirt_x, dirt_y, colour);
  // doesn't think of shadow_bmp yet
  // change so it doesn't destroy certain things
// }
}

// Is able to change the edges of the dirt bitmap, so use only if they're
//  being replaced by more dirt (not empty space)
void change_any_dirt(int dirt_x, int dirt_y, int colour)
{

// int wasp;

// if (dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x && dirt_y < arena[0].max_y)
 {
//  wasp = ((_getpixel(dirt_bmp, dirt_x, dirt_y) / 32) * 32) + colour % 32;
//  _putpixel(dirt_bmp, dirt_x, dirt_y, wasp);
//  if (prand(50) == 0)
//   create_bullet(BULLET_SPECK, dirt_x * GRAIN, dirt_y * GRAIN, prand(300) - 150, prand(450),
//    0, -1, 10 + prand(40), 0, 10, 0, 0, wasp, 0);
  putpixel(dirt_bmp, dirt_x, dirt_y, colour);
  // doesn't think of shadow_bmp yet
  // change so it doesn't destroy certain things
 }
}

void destroy_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int nothing)
{
 // note: not _getpixel because there's no clipping yet
 int shadp = getpixel(shadow_bmp, dirt_x, dirt_y);

 if (dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x - 1 && dirt_y < arena[0].max_y - 1 && shadp != 0 && shadp != PIX_PERM)
 {
  _putpixel(dirt_bmp, dirt_x, dirt_y, getpixel(shadow_bmp, dirt_x, dirt_y));
  _putpixel(shadow_bmp, dirt_x, dirt_y, 0);
  // change so it doesn't destroy certain things
 }
}

// same as destroy_dirt, but can create dirt particles
void dig_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int nothing)
{
 // note: not _getpixel because there's no clipping yet
 int shadp = getpixel(shadow_bmp, dirt_x, dirt_y);
 int wasp;

 if (dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x - 1 && dirt_y < arena[0].max_y - 1 && shadp != 0 && shadp != PIX_PERM)
 {
  wasp = (_getpixel(dirt_bmp, dirt_x, dirt_y) % 32) + 224;
  _putpixel(dirt_bmp, dirt_x, dirt_y, getpixel(shadow_bmp, dirt_x, dirt_y));
  _putpixel(shadow_bmp, dirt_x, dirt_y, 0);
  if (prand(50) == 0)
   create_bullet(BULLET_SPECK, dirt_x * GRAIN, dirt_y * GRAIN, prand(300) - 150, prand(450),
    0, -1, 10 + prand(40), 0, 10, 0, 0, wasp, 0);
 }
}

void add_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int added)
{
 // note: not _getpixel because there's no clipping yet
 int shadp = getpixel(shadow_bmp, dirt_x, dirt_y);

 int light_level;

 if (shadp == 0 && dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x - 1 && dirt_y < arena[0].max_y - 1)
 {
  light_level = _getpixel(dirt_bmp, dirt_x, dirt_y) / 32;
  added = (added % 32) + (light_level * 32);
  _putpixel(shadow_bmp, dirt_x, dirt_y, getpixel(dirt_bmp, dirt_x, dirt_y));
  _putpixel(dirt_bmp, dirt_x, dirt_y, added);
  // change so it doesn't destroy certain things
 }
}

// is the area completely empty?
int check_empty_area(int x1, int y1, int x2, int y2)
{
 int i, j;

 for (i = x1; i < x2; i ++)
 {
  for (j = y1; j < y2; j ++)
  {
   if (get_dirt(i, j)) return 0;
  }
 }

 return 1;

}

// is the area empty of non-diggable pixels?
int check_free_area(int x1, int y1, int x2, int y2)
{

 int i, j;

 for (i = x1; i < x2; i ++)
 {
  for (j = y1; j < y2; j ++)
  {
   if (get_dirt(i, j) == PIX_PERM) return 0;
  }
 }

 return 1;

}



void blacken_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int chance)
{
 if (prand(100) > chance) return;

 if (dirt_x > 0 && dirt_y > 0 && dirt_x < arena[0].max_x && dirt_y < arena[0].max_y)
 {
  if (_getpixel(dirt_bmp, dirt_x, dirt_y))
   _putpixel(dirt_bmp, dirt_x, dirt_y, 28 + prand(4));
  // change so it doesn't destroy certain things
 }
}


unsigned char get_grid(int grid_x, int grid_y)
{

if (grid_x > 0 && grid_y > 0 && grid_x < GRID_X && grid_y < GRID_Y)
   return (grid [grid_x] [grid_y] >= BRICK_SOLID);

   return 1;

}

unsigned char get_specific_grid(int grid_x, int grid_y)
{

if (grid_x > 0 && grid_y > 0 && grid_x < GRID_X && grid_y < GRID_Y)
   return grid [grid_x] [grid_y];

   if ((grid_x + grid_y) % 2 == 0)
    return GRID_REDBRICK1;
     else
      return GRID_REDBRICK2;

}

void set_grid(int grid_x, int grid_y, int to_grid)
{

 if (grid_x > 0 && grid_y > 0 && grid_x < GRID_X && grid_y < GRID_Y)
  grid [grid_x] [grid_y] = to_grid;

}

void set_underlying_grid(int grid_x, int grid_y, int to_grid)
{

 if (grid_x > 0 && grid_y > 0 && grid_x < GRID_X && grid_y < GRID_Y)
  underlying_grid [grid_x] [grid_y] = to_grid;

}

unsigned char get_underlying_grid(int grid_x, int grid_y)
{

if (grid_x > 0 && grid_y > 0 && grid_x < GRID_X && grid_y < GRID_Y)
   return underlying_grid [grid_x] [grid_y];

   if ((grid_x + grid_y) % 2 == 0)
    return GRID_REDBRICK1;
     else
      return GRID_REDBRICK2;


}


int get_grid_strength(int grid_x, int grid_y)
{

if (grid_x > 0 && grid_y > 0 && grid_x < GRID_X && grid_y < GRID_Y)
   return grid_strength [grid_x] [grid_y];

   return 1000;

}

void set_grid_strength(int grid_x, int grid_y, int str_to)
{

if (grid_x > 0 && grid_y > 0 && grid_x < GRID_X && grid_y < GRID_Y)
   grid_strength [grid_x] [grid_y] = str_to;
    else
     return;


}



