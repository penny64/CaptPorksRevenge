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

File: level.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions for generating the arena maps and loading in maps from disk.
 
*/

#include "allegro.h"

#include <string.h>

#include "config.h"
#include "globvar.h"
#include "grabhead.h"

#include "prand.h"

#include "level.h"
#include "bullet.h"
#include "misc.h"
#include "grid.h"

#include <math.h>

#define COL_CLASSIC_FRONT COLOUR_BROWN3
#define COL_CLASSIC_BACK COLOUR_GREY1
//#define COL_CLASSIC_FRONT 209
//#define COL_CLASSIC_BACK 217

//#define PIX_DIRT 100
//#define PIX_HARD 225

// File contains the only functions authorised to deal directly with the
//  dirt bitmaps outside of grid.c

extern BITMAP *dirt_bmp;
extern BITMAP *shadow_bmp;

extern BITMAP *brick_bmp [20] [5];

//BITMAP *put_dirt_to;
//BITMAP *put_back_to;

extern DATAFILE *datf;

void bricked(void);
//void dirt_with_circles(void);
void add_boils(int no_boils, int boil_size, int boil_colour, char allow_overlap);
char check_area(int x1, int y1, int x2, int y2);
void add_lichen(int no_boils, int boil_size, int boil_colour);
int suitable_boil_colour(void);
int suitable_dark_colour(void);
int suitable_back_colour(void);
int suitable_dirt_colour(void);
int nice_dirt_colour(void);
int nice_back_colour(void);
void make_dirt_background(int dirt_col);
void make_dirt_foreground(int dirt_col);
//void make_circle_foreground(int dirt_col);
void build_wall(BITMAP *wall_bmp, int brick_x, int brick_y, int wall_length, char platforms);

void hollow_out_dirt(void);
int load_level(void);

void create_bricked_level(void);
void create_bricked_level_bitmaps(int dirt_background);
void add_grass(BITMAP *bmp, int grass_colour [4], int x1, int y, int x2);
void arrange_lamps(void);
void put_lamps(int lnum, int lbright, int lcolour);
void place_lamp(int lx, int ly, int bright, int draw_lamp, int lamp_colour, int lamp_size);

//void apply_ambient_light(void);
int ambience(int input);
void distribute_lamps(int lbright, int lcolour);

int level_type_one(void);
void make_sky_platforms(int brick_type);
void make_bumpy_floor(int brick_type, int dirt_floor);
void drop_bricks(int x, int y, int brick_type, int dirt_floor);
char check_brick_area(int bx1, int bx2, int by1, int by2);
char is_solid_brick(int brick_type);
void make_shape(int x1, int y1, int x2, int y2, int substance);

void place_ivy(void);
void grow_ivy(int source_x, int source_y);
void grow_leaf(int source_x, int source_y, float stalk_angle);


void generate_level(void)
{

 if (strlen(arena[0].level_loaded) != 0)
 {
  if (load_level() == 0)
  {
   information_box("Failed to load map.", "Out of memory, bad format", "or file not found.", 1);
  }
   else
    return;
 }

 clear_bitmap(dirt_bmp);
 clear_bitmap(shadow_bmp);

 if (arena[0].level_style == LEVEL_CITY)
 {
  create_bricked_level();
  return;
 }

 bricked();

 if (arena[0].dirt_fill == 1
     || arena[0].dirt_fill == 2)
      hollow_out_dirt();
}

// Doesn't really create the bitmaps as they're already created.
void create_bricked_level_bitmaps(int dirt_background)
{
// int ambient_light = 2;

 int i, j;
 int grass_colour [4] = {COLOUR_GREEN1, COLOUR_GREEN2, COLOUR_GREEN3, COLOUR_GREEN4};

 BITMAP *dirtblock1 = create_bitmap(30, 30); // foreground dirt
 BITMAP *dirtblock2 = create_bitmap(30, 30); // background dirt
 BITMAP *dirtblock3 = create_bitmap(30, 30); // foreground rock

 int dirt_col1 = COLOUR_BROWN3;
 int dirt_col2 = COLOUR_GREY1;
 int dirt_col3 = COLOUR_GREY5;


  if (arena[0].level_colours == COLOURS_CRAZY)
  {
   dirt_col1 = suitable_dirt_colour();
   dirt_col2 = suitable_back_colour();
   dirt_col3 = suitable_dirt_colour(); // may be same as dirt_col1
  }
  if (arena[0].level_colours == COLOURS_EARTH)
  {
   dirt_col1 = nice_dirt_colour();
   dirt_col2 = nice_back_colour();
   dirt_col3 = COLOUR_GREY5; //nice_dirt_colour();
  }
  if (arena[0].level_colours == COLOURS_HELL)
  {
   dirt_col1 = COLOUR_RED3;
   dirt_col2 = COLOUR_GREY1;
   dirt_col3 = COLOUR_YELLOW3; //nice_dirt_colour();
   grass_colour [0] = COLOUR_YELLOW1;
   grass_colour [1] = COLOUR_YELLOW2;
   grass_colour [2] = COLOUR_YELLOW3;
   grass_colour [3] = COLOUR_YELLOW4;
  }

 for (i = 0; i < 30; i ++)
 {
  for (j = 0; j < 30; j ++)
  {
   _putpixel(dirtblock1, i, j, ambience(dirt_col1 + prand(2)));
  }
 }


 for (i = 0; i < 30; i ++)
 {
  for (j = 0; j < 30; j ++)
  {
   _putpixel(dirtblock2, i, j, ambience(dirt_col2 + prand(2)));
  }
 }

 for (i = 0; i < 30; i ++)
 {
  for (j = 0; j < 30; j ++)
  {
   _putpixel(dirtblock3, i, j, ambience(dirt_col3 + prand(2)));
  }
 }


 for (i = 0; i < arena[0].max_x; i ++)
 {
  for (j = 0; j < arena[0].max_y; j ++)
  {
   switch(getpixel(arena[0].bricks, i, j))
   {
    case BRICKBACK_RED_BRICK:
     blit(brick_bmp [0] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, 0);
     break;
    case BRICKBACK_YELLOW_STONE:
     blit(brick_bmp [4] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, 0);
     break;
    case BRICKBACK_BLACK_TILE:
     blit(brick_bmp [5] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, 0);
     break;
    case BRICKBACK_BLUE_BRICK:
     blit(brick_bmp [8] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, 0);
     break;
    case BRICKBACK_GREEN_CIRCLE:
     blit(brick_bmp [10] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, 0);
     break;
    case BRICKBACK_DIRT:
     blit(dirtblock2, dirt_bmp, prand(30 - BRICK_X), prand(30 - BRICK_Y), i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, 0);
     break;
     
    case BRICK_RED_BRICK:
     blit(brick_bmp [1] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;
    case BRICK_YELLOW_STONE:
     blit(brick_bmp [2] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;
    case BRICK_YELLOW_CROSS:
     blit(brick_bmp [3] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;
    case BRICK_WHITE_TILE:
     blit(brick_bmp [6] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;
    case BRICK_BLUE_BRICK:
     blit(brick_bmp [7] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;
    case BRICK_GREEN_CIRCLE:
     blit(brick_bmp [9] [arena[0].ambient_light], dirt_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;

    case BRICK_DIRT:
     blit(dirtblock1, dirt_bmp, prand(30 - BRICK_X), prand(30 - BRICK_Y), i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     if (dirt_col1 == COLOUR_BROWN3
         && getpixel(arena[0].bricks, i, j - 1) < BRICK_SOLID)
      add_grass(dirt_bmp, grass_colour, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X);
     blit(dirtblock2, shadow_bmp, prand(30 - BRICK_X), prand(30 - BRICK_Y), i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     switch(dirt_background)
     {
      case BRICKBACK_RED_BRICK:
       blit(brick_bmp [0] [arena[0].ambient_light], shadow_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
       break;
      case BRICKBACK_YELLOW_STONE:
       blit(brick_bmp [4] [arena[0].ambient_light], shadow_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
       break;
      case BRICKBACK_BLACK_TILE:
       blit(brick_bmp [5] [arena[0].ambient_light], shadow_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
       break;
      case BRICKBACK_BLUE_BRICK:
       blit(brick_bmp [8] [arena[0].ambient_light], shadow_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
       break;
      case BRICKBACK_GREEN_CIRCLE:
       blit(brick_bmp [10] [arena[0].ambient_light], shadow_bmp, 0, 0, i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
       break;
      case BRICKBACK_DIRT:
       blit(dirtblock2, shadow_bmp, prand(30 - BRICK_X), prand(30 - BRICK_Y), i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
       break;
      }
//     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;
    case BRICK_STONE:
     blit(dirtblock3, dirt_bmp, prand(30 - BRICK_X), prand(30 - BRICK_Y), i * BRICK_X, j * BRICK_Y, BRICK_X, BRICK_Y);
     rectfill(shadow_bmp, i * BRICK_X, j * BRICK_Y, (i + 1) * BRICK_X, (j + 1) * BRICK_Y, PIX_PERM);
     break;

   }
  }
 }

//  blit(brick_bmp, g_to_bmp, brick_x, brick_y, i * BRICK_X + x_offset, j * BRICK_Y + y_offset, BRICK_X, BRICK_Y);



}

void add_grass(BITMAP *bmp, int grass_colour [4], int x1, int y, int x2)
{

 int i;

 for (i = x1; i < x2; i ++)
 {
  if (prand(3) != 0)
   putpixel(bmp, i, y, ambience(grass_colour [prand(4)]));
  if (prand(2) == 0)
   putpixel(bmp, i, y + 1, ambience(grass_colour [prand(4)]));
  if (prand(3) == 0)
   putpixel(bmp, i, y + 2, ambience(grass_colour [prand(4)]));

 }

}


void bricked(void)
{

 int brick_x = BRICK_X;
 int brick_y = BRICK_Y;

 int i, j;

// rectfill(dirt_bmp, 0, 0, game[0].max_x, game[0].max_y, 0);

 if (arena[0].level_style == LEVEL_CLASSIC)
 {
  if (arena[0].dirt_fill > 0)
  {
   make_dirt_foreground(-1);
   make_dirt_background(-1);
  }
   else
    make_dirt_foreground(-2);
  if (arena[0].solid_density > 0) add_boils(arena[0].max_x * arena[0].max_y / (85000 / arena[0].solid_density), 20 + prand(40), -1, 0);
  rect(dirt_bmp, 0, 0, arena[0].max_x - 1, arena[0].max_y - 1, COLOUR_GREY4);
  rect(shadow_bmp, 0, 0, arena[0].max_x - 1, arena[0].max_y - 1, PIX_PERM);
  arrange_lamps();
  return;
 }

 BITMAP *front_brick2 = brick_bmp [1] [arena[0].ambient_light];
 BITMAP *back_brick1 = brick_bmp [0] [arena[0].ambient_light];
// if (prand(2) == 0) back_brick1 = datf[MBRICK_SSTONE2].dat;

 switch(prand(5))
 {
  case 0:
   front_brick2 = brick_bmp [1] [arena[0].ambient_light];
   back_brick1 = brick_bmp [0] [arena[0].ambient_light];
  break;
  case 1:
   front_brick2 = brick_bmp [2] [arena[0].ambient_light];
   back_brick1 = brick_bmp [4] [arena[0].ambient_light];
  break;
  case 2:
   front_brick2 = brick_bmp [6] [arena[0].ambient_light];
   back_brick1 = brick_bmp [5] [arena[0].ambient_light];
  break;
 }

 if (arena[0].dirt_fill > 0)
 {
//  if (prand(3) != 0)
   make_dirt_foreground(-1);
/*    else
//     if (prand(2) == 0 || front_brick1 == NULL)
//      make_circle_foreground(-1);
//       else
        for (i = 0; i < arena[0].max_x / brick_x + 1; i ++)
         {
         for (j = 0; j < arena[0].max_y / brick_y + 1; j ++)
         {
          blit(front_brick1, dirt_bmp, 0, 0, i * brick_x, j * brick_y, brick_x, brick_y);
         }
        }*/


//  if (prand(3) != 0)
   make_dirt_background(-1);
/*    else
     for (i = 0; i < arena[0].max_x / brick_x + 1; i ++)
     {
      for (j = 0; j < arena[0].max_y / brick_y + 1; j ++)
      {
       blit(back_brick1, shadow_bmp, 0, 0, i * brick_x, j * brick_y, brick_x, brick_y);
      }
     }*/
 }
  else
  {
//   if (prand(3) != 0)
    make_dirt_foreground(-2);
/*     else
      for (i = 0; i < arena[0].max_x / brick_x + 1; i ++)
      {
       for (j = 0; j < arena[0].max_y / brick_y + 1; j ++)
       {
        blit(back_brick1, dirt_bmp, 0, 0, i * brick_x, j * brick_y, brick_x, brick_y);
       }
      }*/
  }
     
/*
    for (i = 0; i < arena[0].max_x / brick_x + 1; i ++)
    {
     for (j = 0; j < arena[0].max_y / brick_y + 1; j ++)
     {
      blit(back_brick1, dirt_bmp, 0, 0, i * brick_x, j * brick_y, brick_x, brick_y);
     }
    }

    clear_bitmap(shadow_bmp);*/

 if (arena[0].solid_density > 0)
 {

  if (prand(3) || arena[0].level_style == LEVEL_PLATFORM)
  {
   int wall_length = 2 + prand(7);
   j = 5 + arena[0].max_x * arena[0].max_y / (120000 / arena[0].solid_density);
   if (arena[0].level_style == LEVEL_PLATFORM)
   {
    j *= 2;
    wall_length = 5 + prand(4);
   }
   for (i = 0; i < j; i ++)
   {
    build_wall(front_brick2, brick_x, brick_y, wall_length, arena[0].level_style == LEVEL_PLATFORM);
   }
  }

//  if (prand(3) == 0 || solid_density == 1)
   add_boils(5 + arena[0].max_x * arena[0].max_y / (55000 / arena[0].solid_density), 20 + prand(40), -1, 0);
   
 }

 
 if (prand(3) == 0)
 {
  if (prand(8) != 0)
   add_lichen(1 + prand(80) + prand(arena[0].max_x * arena[0].max_y / 10000), 3 + prand(8), suitable_dark_colour());
    else
     add_lichen(1 + prand(80) + prand(arena[0].max_x * arena[0].max_y / 10000), 3 + prand(8), -1);
 }

// if (arena[0].dirt_fill == 0)
 {
  rect(dirt_bmp, 0, 0, arena[0].max_x - 1, arena[0].max_y - 1, COLOUR_GREY4);
  rect(shadow_bmp, 0, 0, arena[0].max_x - 1, arena[0].max_y - 1, PIX_PERM);
//  rectfill(dirt_bmp, 51, 51, arena[0].max_x - 52, arena[0].max_y - 52, COLOUR_GREY4);
//  rectfill(shadow_bmp, 51, 51, arena[0].max_x - 52, arena[0].max_y - 52, PIX_PERM);
 }

// apply_ambient_light();
 arrange_lamps();

}


void add_boils(int no_boils, int boil_size, int boil_colour, char allow_overlap)
{

 int i, j, k, l, m, n;
 int bcol = boil_colour;

 for (i = 0; i < no_boils; i ++)
 {
  j = prand(arena[0].max_x);
  k = prand(arena[0].max_y);
  l = 2 + prand(boil_size);
  if (allow_overlap == 0 && check_area(j - l - 2, k - l - 2, j + l + 2, k + l + 2) == 1)
  {
   i --;
   continue;
  }
 circlefill(shadow_bmp, j, k, l, PIX_PERM);
 if (boil_colour == -1) bcol = suitable_boil_colour();
 circlefill(dirt_bmp, j, k, l, ambience(bcol - 3));
 for (m = 10; m > 1; m --)
 {
  n = (m * 3) / 10;
  if (n < 0)
   n = 0;
  if (n > 3)
   n = 3;
  circlefill(dirt_bmp, j, k, l - (l / m), ambience(bcol - n));
 }
 }


}

char check_area(int x1, int y1, int x2, int y2)
{

 int i, j;

 for (i = x1; i < x2 + 1; i ++)
 {
  for (j = y1; j < y2 + 1; j ++)
  {
   if (getpixel(shadow_bmp, i, j) == PIX_PERM) return 1;
  }
 }

 return 0;

}

void add_lichen(int no_boils, int boil_size, int boil_colour)
{

 int i, j, k, l, m;
 int bcol = boil_colour;

 for (i = 0; i < no_boils; i ++)
 {
  j = prand(arena[0].max_x);
  k = prand(arena[0].max_y);
  l = 2 + prand(boil_size);
  if (check_area(j - l - 2, k - l - 2, j + l + 2, k + l + 2) == 1)
  {
   i --;
   continue;
  }
 if (boil_colour == -1) bcol = suitable_dark_colour();
 if (arena[0].dirt_fill > 0)
  circlefill(shadow_bmp, j, k, l, ambience(bcol));
   else
    circlefill(dirt_bmp, j, k, l, ambience(bcol));
 if (l >= 4)
 {
  for (m = 5 + l; m > 1; m --)
  {
   if (arena[0].dirt_fill > 0)
    circlefill(shadow_bmp, j - l + prand(l * 2 + 1), k - l + prand(l * 2 + 1), prand(3) + 1, ambience(bcol + prand(2)));
     else
      circlefill(dirt_bmp, j - l + prand(l * 2 + 1), k - l + prand(l * 2 + 1), prand(3) + 1, ambience(bcol + prand(2)));
  }
 }
 }


}
/*
void make_circle_foreground(int dirt_col)
{

  if (dirt_col == -1)
  {
   dirt_col = suitable_dirt_colour();
   if (arena[0].level_colours == COLOURS_EARTH) dirt_col = nice_dirt_colour();
   if (arena[0].level_colours == COLOURS_CLASSIC) dirt_col = COL_CLASSIC_FRONT;
  }
 int i;

 rectfill(dirt_bmp, 0, 0, arena[0].max_x, arena[0].max_y, dirt_col);

 for (i = 0; i < (arena[0].max_x * arena[0].max_y) / 400; i ++)
 {
  circlefill(dirt_bmp, prand(arena[0].max_x), prand(arena[0].max_y), prand(20), dirt_col + prand(5));
 }


}*/


void make_dirt_background(int dirt_col)
{

 if (dirt_col == -1)
 {
  dirt_col = suitable_dark_colour();
  if (arena[0].level_colours == COLOURS_CLASSIC) dirt_col = COL_CLASSIC_BACK;
  if (arena[0].level_colours == COLOURS_EARTH) dirt_col = nice_back_colour();
  if (arena[0].level_colours == COLOURS_HELL) dirt_col = COLOUR_GREY1;
 }

 int i, j;
 BITMAP *dirtblock = create_bitmap(30, 30);

 for (i = 0; i < 30; i ++)
 {
  for (j = 0; j < 30; j ++)
  {
   _putpixel(dirtblock, i, j, ambience(dirt_col + prand(2))); //dirt_col + prand(3));
  }
 }

 for (i = 0; i < arena[0].max_x + 31; i += 30)
 {
  for (j = 0; j < arena[0].max_y + 31; j += 30)
  {
   blit(dirtblock, shadow_bmp, 0, 0, i, j, 30, 30);
  }
 }

 for (i = 0; i < 20 + (arena[0].max_x * arena[0].max_y) / 10000; i ++)
 {
   blit(dirtblock, shadow_bmp, 0, 0, prand(arena[0].max_x), prand(arena[0].max_y), 30, 30);
 }

}

void make_dirt_foreground(int dirt_col)
{

  if (dirt_col == -1) // random
  {
   dirt_col = suitable_dirt_colour();
   if (arena[0].level_colours == COLOURS_EARTH) dirt_col = nice_dirt_colour();
   if (arena[0].level_colours == COLOURS_HELL) dirt_col = COLOUR_RED3;
   if (arena[0].level_colours == COLOURS_CLASSIC) dirt_col = COL_CLASSIC_FRONT;
  }
  
  if (dirt_col == -2) // random background colours if no dirt
  {
   dirt_col = suitable_dark_colour();
   if (arena[0].level_colours == COLOURS_EARTH) dirt_col = nice_back_colour();
   if (arena[0].level_colours == COLOURS_HELL) dirt_col = COLOUR_GREY1;
   if (arena[0].level_colours == COLOURS_CLASSIC) dirt_col = COL_CLASSIC_BACK;
  }
  
 int i, j;
 BITMAP *dirtblock = create_bitmap(30, 30);

 for (i = 0; i < 30; i ++)
 {
  for (j = 0; j < 30; j ++)
  {
   _putpixel(dirtblock, i, j, ambience(dirt_col + prand(2))); //dirt_col + prand(3));
  }
 }

 for (i = 0; i < arena[0].max_x + 31; i += 30)
 {
  for (j = 0; j < arena[0].max_y + 31; j += 30)
  {
   blit(dirtblock, dirt_bmp, 0, 0, i, j, 30, 30);
  }
 }

 for (i = 0; i < 80 + (arena[0].max_x * arena[0].max_y) / 10000; i ++)
 {
   blit(dirtblock, dirt_bmp, 0, 0, prand(arena[0].max_x), prand(arena[0].max_y), 30, 30);
 }

}

void build_wall(BITMAP *wall_bmp, int brick_x, int brick_y, int wall_length, char platforms)
{

 int count_out = 0;
 int x1, y1, x2, y2;

 int i, j;

 do
 {
  count_out ++;
  x1 = prand(arena[0].max_x / brick_x);
  y1 = prand(arena[0].max_y / brick_y);
  x2 = x1 + 1;
  y2 = y1 + 1;
  if (prand(2) || (platforms == 1 && prand(7) != 0))
   x2 = x1 + prand(wall_length) + 1;
    else
     y2 = y1 + prand(wall_length + 1);
  if (check_area(x1 * brick_x - 20, y1 * brick_y - 20, x2 * brick_x + 20, y2 * brick_y + 20) == 1) continue;
  for (i = x1; i < x2; i ++)
  {
   for (j = y1; j < y2; j ++)
   {
    blit(wall_bmp, dirt_bmp, 0, 0, i * brick_x, j * brick_y, brick_x, brick_y);
    rectfill(shadow_bmp, i * brick_x, j * brick_y, (i + 1) * brick_x - 1, (j + 1) * brick_y - 1, PIX_PERM);
   }
  }
 return;
 } while(count_out < 1000);

}


int suitable_dark_colour(void)
{
// int scol = 28 + prand(12) * 16;
// if (scol == 92) scol = 28;
// if (scol == 140) scol = 132;
// if (scol == 204) scol = 220;

 int scol = COLOUR_BROWN1;
 
 switch(prand(8))
 {
  case 0:
   scol = COLOUR_RED1; break;
  case 1:
   scol = COLOUR_YELLOW1; break;
  case 2:
   scol = COLOUR_BLUE1; break;
  case 3:
   scol = COLOUR_GREEN1; break;
  case 4:
   scol = COLOUR_BROWN1; break;
  default:
   scol = COLOUR_GREY1;
    break;
 }

 return scol;
}

int suitable_back_colour(void)
{
 int scol = COLOUR_BROWN1;
 
 switch(prand(8))
 {
  case 0:
   scol = COLOUR_RED1; break;
  case 1:
   scol = COLOUR_YELLOW1; break;
  case 2:
   scol = COLOUR_BLUE1; break;
  case 3:
   scol = COLOUR_GREEN1; break;
  case 4:
   scol = COLOUR_BROWN1; break;
  default:
   scol = COLOUR_GREY1;
    break;
 }

 return scol;
 
}

int suitable_dirt_colour(void)
{

 int scol = COLOUR_BROWN3;
 
 switch(prand(8))
 {
  case 0:
   scol = COLOUR_RED3; break;
  case 1:
   scol = COLOUR_YELLOW3; break;
  case 2:
   scol = COLOUR_BLUE3; break;
  case 3:
   scol = COLOUR_GREEN3; break;
  case 4:
   scol = COLOUR_BROWN3; break;
  default:
   scol = COLOUR_GREY5;
    break;
 }

 return scol;

}

int nice_dirt_colour(void)
{

 if (prand(3) == 0)
  return COLOUR_YELLOW3;
 return COLOUR_BROWN3;

}

int nice_back_colour(void)
{
 switch(prand(3))
 {
  default:
  case 0: return COLOUR_GREY1;
  case 2: return COLOUR_BROWN1;
 }

 return COLOUR_GREY1;
}

int suitable_boil_colour(void)
{
 int scol = COLOUR_GREY6;

 if (arena[0].level_colours == COLOURS_CLASSIC) return COLOUR_GREY6;
 if (arena[0].level_colours == COLOURS_HELL) return COLOUR_YELLOW4;
 if (arena[0].level_colours == COLOURS_EARTH)
 {
  if (prand(3) != 0) return COLOUR_GREY6;
  if (prand(2) != 0) return COLOUR_BROWN4;
  return COLOUR_YELLOW4;
 }
 switch(prand(8))
 {
  case 0:
   scol = COLOUR_RED4; break;
  case 1:
   scol = COLOUR_YELLOW4; break;
  case 2:
   scol = COLOUR_BLUE4; break;
  case 3:
   scol = COLOUR_GREEN4; break;
  case 4:
   scol = COLOUR_BROWN4; break;
  default:
   scol = COLOUR_GREY4;
    break;
 }
 return scol;
}


void hollow_out_dirt(void)
{

 int hole_size = 10;
 if (arena[0].dirt_fill == 1)
  hole_size = 20;

 int x, y;
// int x1, y1;
 int x_dir, y_dir;
 int tunnel_length = 20;

 int number = arena[0].max_x * arena[0].max_y / (85000 / 1) + 3;

 int i, j;

 for (i = 0; i < number; i ++)
 {
  tunnel_length = 50 + prand(30);
  x = prand(arena[0].max_x) * GRAIN;
  y = prand(arena[0].max_y) * GRAIN;
  if (prand(2) == 0) x_dir = 1 + prand(3);
   else x_dir = -1 - prand(3);
  if (prand(2) == 0) y_dir = 1 + prand(3);
   else y_dir = -1 - prand(3);
  for (j = 0; j < tunnel_length; j ++)
  {
//   if (x_dir >= 1)
    x += ((prand(hole_size) * GRAIN) * x_dir) / 2;
    y += ((prand(hole_size) * GRAIN) * y_dir) / 2;
//     else
//      x -= prand(hole_size * 2) * GRAIN;
//   if (y_dir == 1)
//    y += prand(hole_size * 2) * GRAIN;
//     else
//      y -= prand(hole_size * 2) * GRAIN;
//   x -= hole_size * GRAIN;
//   y += prand(hole_size * 2) * GRAIN;
//   y -= hole_size * GRAIN;
   if (x < 0 || x > arena[0].max_x * GRAIN)
    x_dir *= -1;
   if (y < 0 || y > arena[0].max_y * GRAIN)
    y_dir *= -1;
   blast_dirt(x, y, hole_size + prand(10), 0);
  }
 }


}



/*
Loads the level file bitmap then destroys it, to check that it's okay.
*/
int check_load_level(void)
{
 BITMAP *load_bmp;
 RGB temp_palette [256];

 char level_file [100];

 strcpy(level_file, "maps\\");
 strcat(level_file, arena[0].level_loaded);
 strcat(level_file, ".bmp");

 load_bmp = load_bitmap(level_file, temp_palette);

 if (load_bmp == NULL)
  return 0;

 int x, y;

 int i;

 for (i = 0; i < 5000; i ++)
 {
  if (getpixel(load_bmp, i, 0) == -1)
  {
   x = i;
   break;
  }
 }

 for (i = 0; i < 5000; i ++)
 {
  if (getpixel(load_bmp, 0, i) == -1)
  {
   y = i;
   break;
  }
 }

 if (x < 300 || y < 300 || x > 1200 || y > 1200)
 {
  destroy_bitmap(load_bmp);
  return 0;
 }

  destroy_bitmap(load_bmp);
  return 1;

}

/*
Shouldn't fail, as the file has already been checked and passed above.
But do some checks anyway.
*/
int load_level(void)
{

 BITMAP *load_bmp;
 RGB temp_palette [256];

 char level_file [100];

 strcpy(level_file, "maps\\");
 strcat(level_file, arena[0].level_loaded);
 strcat(level_file, ".bmp");

 load_bmp = load_bitmap(level_file, temp_palette);

 if (load_bmp == NULL)
  return 0;

 int x, y;

 int i, j;

 for (i = 0; i < 5000; i ++)
 {
  if (getpixel(load_bmp, i, 0) == -1)
  {
   x = i;
   break;
  }
 }

 for (i = 0; i < 5000; i ++)
 {
  if (getpixel(load_bmp, 0, i) == -1)
  {
   y = i;
   break;
  }
 }

 if (x < 100 || y < 100 || x > 1200 || y > 1200)
 {
  destroy_bitmap(load_bmp);
  return 0;
 }

// okay, it's loaded. Let's put it into the correct format:

 if (dirt_bmp != NULL)
  destroy_bitmap(dirt_bmp);
 if (shadow_bmp != NULL)
  destroy_bitmap(shadow_bmp);

 if ((dirt_bmp = create_bitmap(x, y / 2)) == NULL)
 {
   destroy_bitmap(load_bmp);
   return 0;
 }
 if ((shadow_bmp = create_bitmap(x, y / 2)) == NULL)
 {
   destroy_bitmap(load_bmp);
   destroy_bitmap(dirt_bmp);
   return 0;
 }
 
 for (i = 0; i < x; i ++)
 {
  for (j = 0; j < y; j ++)
  {
   if (getpixel(load_bmp, i, j) == 224)
    putpixel(load_bmp, i, j, 0);
  }
 }
 

 blit(load_bmp, dirt_bmp, 0, 0, 0, 0, x, y / 2);
 blit(load_bmp, shadow_bmp, 0, y / 2, 0, 0, x, y / 2);

 destroy_bitmap(load_bmp);

// int shadp = 0;

// no dirt in netgames!
//  but this is probably slow, so only do it if dirt_fill is zero
// if (serial[0].game_type != SERIAL_LOCAL_ONLY
//     && arena[0].dirt_fill == 0)
 if (arena[0].dirt_fill == 0)
 {
  for (i = 0; i < x; i ++)
  {
   for (j = 0; j < y; j ++)
   {
    destroy_dirt(dirt_bmp, i, j, 0);
   }
  }
 }


 arena[0].max_x = x;
 arena[0].max_y = y / 2;


 return 1;

}

void arrange_lamps(void)
{

 int lamp_no = 0;

 if (arena[0].lamp_amount == 0)
  return;

 lamp_no = arena[0].lamp_amount % 4;

 lamp_no *= arena[0].max_x * arena[0].max_y / 125000;

 if (lamp_no > 30)
  lamp_no = 30;

// lamp_no = 100;

 if (arena[0].lamp_amount < 4)
  put_lamps(lamp_no, -1, -1); // randomly
   else
    distribute_lamps(-1, -1); // regular pattern


}

// version of put_lamps which seeks to distribute the lamps in a squarish
//  pattern, meaning that much of the level will be illuminated but with
//  several dark spots. lnum currently unused.
void distribute_lamps(int lbright, int lcolour)
{
 int i, j, k, m, n, o;

 k = 300;

 if (arena[0].lamp_amount == 5)
  k = 180;
 if (arena[0].lamp_amount == 6)
  k = 100;


 for (i = 50; i < arena[0].max_x; i += k + prand(k))
 {
  for (j = 50; j < arena[0].max_y; j += k + prand(k))
  {
   if (check_area(i - 5, j - 5, i + 5, j + 5) == 1)
   {
    continue;
   }
   m = lbright;
   if (m == -1)
    m = prand(40) + 80;
   n = lcolour;
   if (n == -1)
   {
    switch(prand(5))
    {
     case 0: n = COLOUR_RED4; break;
     case 1: n = COLOUR_GREY7; break;
     case 2: n = COLOUR_BLUE4; break;
     case 3: n = COLOUR_GREEN4; break;
     case 4: n = COLOUR_YELLOW4; break;
    }
   }
   o = 3;
   if (m < 100)
    o = 2;
   if (m > 200)
    o = 4;
   if (m > 300)
    o = 5;
  place_lamp(i, j, m, 2, n, o);
  }
 }

}


void put_lamps(int lnum, int lbright, int lcolour)
{

 int i, j, k, m, n, o, timeout = 0;

 for (i = 0; i < lnum; i ++)
 {
  timeout ++;
  if (timeout > 15000)
   break;
  j = prand(arena[0].max_x);
  k = prand(arena[0].max_y);
  if (check_area(j - 3, k - 3, j + 3, k + 3) == 1)
  {
   i --;
   continue;
  }
  m = lbright;
  if (m == -1)
   m = prand(40) + 80;
  n = lcolour;
  if (n == -1)
  {
   switch(prand(5))
   {
    case 0: n = COLOUR_RED4; break;
    case 1: n = COLOUR_GREY7; break;
    case 2: n = COLOUR_BLUE4; break;
    case 3: n = COLOUR_GREEN4; break;
    case 4: n = COLOUR_YELLOW4; break;
   }
  }
  o = 3;
  if (m < 100)
   o = 2;
  if (m > 200)
   o = 4;
  if (m > 300)
   o = 5;
  place_lamp(j, k, m, 2, n, o);
 }
}



void place_lamp(int lx, int ly, int bright, int draw_lamp, int lamp_colour, int lamp_size)
{

 drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
 if (draw_lamp >= 1)
 {
//  circlefill(dirt_bmp, lx, ly, lamp_size + 2, lamp_colour - 1);
  circlefill(dirt_bmp, lx, ly, lamp_size + 1, lamp_colour);
  circlefill(dirt_bmp, lx, ly, lamp_size, COLOUR_GREY8);
 }
 if (draw_lamp == 2)
 {
//  circlefill(shadow_bmp, lx, ly, lamp_size + 2, lamp_colour - 1);
//  circlefill(shadow_bmp, lx, ly, lamp_size + 1, lamp_colour);
  circlefill(shadow_bmp, lx, ly, lamp_size, 0);
 }

 drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
 circlefill(dirt_bmp, lx, ly, bright, LIGHT_1);
 if (bright > 15)
  circlefill(dirt_bmp, lx, ly, bright - 10, LIGHT_2);
 if (bright > 40)
  circlefill(dirt_bmp, lx, ly, bright - 30, LIGHT_3);
 if (bright > 80)
  circlefill(dirt_bmp, lx, ly, bright - 70, LIGHT_4);

 circlefill(shadow_bmp, lx, ly, bright, LIGHT_1);
 if (bright > 15)
  circlefill(shadow_bmp, lx, ly, bright - 10, LIGHT_2);
 if (bright > 40)
  circlefill(shadow_bmp, lx, ly, bright - 30, LIGHT_3);
 if (bright > 80)
  circlefill(shadow_bmp, lx, ly, bright - 70, LIGHT_4);

 drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

}

int ambience(int input)
{
 switch(arena[0].ambient_light)
 {
  case 0:
   return input % 32 + 96;
  case 1:
   return input % 32 + 128;
  case 2:
   return input % 32 + 160;
  case 3:
   return input % 32 + 224;
  case 4:
   return input % 32 + 196;
 }

   return input % 32 + 224;
 
}

/*
void apply_ambient_light(void)
{
 drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
 rectfill(dirt_bmp, 0, 0, arena[0].max_x, arena[0].max_y, FORCE_LIGHT_2);
 rectfill(shadow_bmp, 0, 0, arena[0].max_x, arena[0].max_y, FORCE_LIGHT_2);
 drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

}
*/


void create_bricked_level(void)
{

// clear_to_color(arena[0].bricks, BRICKBACK_ROCK);
// rectfill(arena[0].bricks, 5, 5, 40, 40, BRICKBACK_ROCK);
// rectfill(arena[0].bricks, 15, 15, 20, 20, BRICK_ROCK);

// clear_to_color(arena[0].light, 0);
// clear_to_color(arena[0].base_light, 0);
  clear_bitmap(arena[0].bricks);
//  arena[0].light = create_bitmap(arena[0].bricks_x, arena[0].bricks_y);
//  clear_bitmap(arena[0].light);
//  arena[0].base_light = create_bitmap(arena[0].bricks_x, arena[0].bricks_y);
//  clear_bitmap(arena[0].base_light);


 int dirt_background = level_type_one();

 create_bricked_level_bitmaps(dirt_background);

  rect(dirt_bmp, 0, 0, arena[0].max_x - 1, arena[0].max_y - 1, COLOUR_GREY4);
  rect(shadow_bmp, 0, 0, arena[0].max_x - 1, arena[0].max_y - 1, PIX_PERM);

// apply_ambient_light();

// place_ivy();

 arrange_lamps();

}


int level_type_one(void)
{

 int brick_type1 = 0; // background
 int brick_type2 = 0; // ground
 int brick_type3 = 0; // platforms - if -1, each platform different
 int dirt_floor = 0;

// if (arena[0].level_colours == COLOURS_CLASSIC)
// {
  brick_type1 = BRICKBACK_DIRT;
  brick_type2 = BRICK_STONE;
  brick_type3 = BRICK_STONE;
// }
 if (arena[0].level_colours == COLOURS_HELL)
 {
  brick_type1 = BRICKBACK_DIRT;
  brick_type2 = BRICK_DIRT;
  brick_type3 = BRICK_STONE;
 }
 
 if (arena[0].level_colours == COLOURS_EARTH)
 {
  switch(prand(5))
  {
   case 0:
    brick_type1 = BRICKBACK_DIRT;
    brick_type2 = BRICK_STONE;
    brick_type3 = BRICK_STONE;
    break;
   case 1:
    brick_type1 = BRICKBACK_RED_BRICK;
    brick_type2 = BRICK_RED_BRICK;
    brick_type3 = BRICK_RED_BRICK;
    break;
   case 2:
    brick_type1 = BRICKBACK_YELLOW_STONE;
    brick_type2 = BRICK_YELLOW_STONE;
    brick_type3 = BRICK_YELLOW_STONE;
    if (prand(4) == 0)
     brick_type3 = BRICK_YELLOW_CROSS;
    break;
   case 3:
    brick_type1 = BRICKBACK_YELLOW_STONE;
    brick_type2 = BRICK_YELLOW_CROSS;
    brick_type3 = BRICK_YELLOW_CROSS;
    if (prand(4) == 0)
     brick_type3 = BRICK_YELLOW_STONE;
    break;
   case 4:
    brick_type1 = BRICKBACK_BLACK_TILE;
    brick_type2 = BRICK_WHITE_TILE;
    brick_type3 = BRICK_WHITE_TILE;
    break;

  }
  switch(prand(10))
  {
   case 0:
     brick_type1 = BRICKBACK_DIRT;
     break;
   case 1:
     brick_type1 = BRICKBACK_RED_BRICK;
     break;
   case 2:
     brick_type1 = BRICKBACK_BLACK_TILE;
     break;
   case 3:
     brick_type1 = BRICKBACK_YELLOW_STONE;
     break;
   default:
    break;
  }
  switch(prand(10))
  {
   case 0:
     brick_type3 = BRICK_YELLOW_STONE;
     break;
   case 1:
     brick_type3 = BRICK_RED_BRICK;
     break;
   case 2:
     brick_type3 = BRICK_WHITE_TILE;
     break;
   case 3:
     brick_type3 = BRICK_YELLOW_CROSS;
     break;
   case 4:
   case 5:
     brick_type3 = -1;
     break;
   default:
    break;
  }
 }
 
 if (arena[0].level_colours == COLOURS_CRAZY)
 {
  switch(prand(7))
  {
   case 0:
    brick_type1 = BRICKBACK_DIRT;
    brick_type2 = BRICK_STONE;
    brick_type3 = BRICK_STONE;
    break;
   case 1:
    brick_type1 = BRICKBACK_RED_BRICK;
    brick_type2 = BRICK_RED_BRICK;
    brick_type3 = BRICK_RED_BRICK;
    break;
   case 2:
    brick_type1 = BRICKBACK_YELLOW_STONE;
    brick_type2 = BRICK_YELLOW_STONE;
    brick_type3 = BRICK_YELLOW_STONE;
    if (prand(4) == 0)
     brick_type3 = BRICK_YELLOW_CROSS;
    break;
   case 3:
    brick_type1 = BRICKBACK_YELLOW_STONE;
    brick_type2 = BRICK_YELLOW_CROSS;
    brick_type3 = BRICK_YELLOW_CROSS;
    if (prand(4) == 0)
     brick_type3 = BRICK_YELLOW_STONE;
    break;
   case 4:
    brick_type1 = BRICKBACK_BLACK_TILE;
    brick_type2 = BRICK_WHITE_TILE;
    brick_type3 = BRICK_WHITE_TILE;
    break;
   case 5:
    brick_type1 = BRICKBACK_BLUE_BRICK;
    brick_type2 = BRICK_BLUE_BRICK;
    brick_type3 = BRICK_BLUE_BRICK;
    break;
   case 6:
    brick_type1 = BRICKBACK_GREEN_CIRCLE;
    brick_type2 = BRICK_GREEN_CIRCLE;
    brick_type3 = BRICK_GREEN_CIRCLE;
    break;

  }
  switch(prand(8))
  {
   case 0:
     brick_type1 = BRICKBACK_DIRT;
     break;
   case 1:
     brick_type1 = BRICKBACK_RED_BRICK;
     break;
   case 2:
     brick_type1 = BRICKBACK_BLACK_TILE;
     break;
   case 3:
     brick_type1 = BRICKBACK_YELLOW_STONE;
     break;
   case 4:
     brick_type1 = BRICKBACK_BLUE_BRICK;
     break;
   case 5:
     brick_type1 = BRICKBACK_GREEN_CIRCLE;
     break;
   default:
    break;
  }
  switch(prand(10))
  {
   case 0:
     brick_type3 = BRICK_YELLOW_STONE;
     break;
   case 1:
     brick_type3 = BRICK_RED_BRICK;
     break;
   case 2:
     brick_type3 = BRICK_WHITE_TILE;
     break;
   case 3:
     brick_type3 = BRICK_YELLOW_CROSS;
     break;
   case 4:
     brick_type3 = BRICK_BLUE_BRICK;
     break;
   case 5:
     brick_type3 = BRICK_GREEN_CIRCLE;
     break;
   default:
     brick_type3 = -1;
    break;
  }
 }

 if (arena[0].dirt_fill > 0)
 {
  if (arena[0].dirt_fill == 3)
   brick_type1 = BRICK_DIRT;
    else
    {
     if (prand(10) == 0)
      brick_type1 = BRICK_DIRT;
       else
        if (prand(10) == 0)
         brick_type2 = BRICK_DIRT;
          else
           if (prand(10) == 0)
           brick_type3 = BRICK_DIRT;
    if (prand(3) == 0)
     dirt_floor = 1 + prand(4);
    }

 }


 clear_to_color(arena[0].bricks, brick_type1);

 make_bumpy_floor(brick_type2, dirt_floor);

 if (arena[0].solid_density > 0)
  make_sky_platforms(brick_type3);

 return brick_type1;

}



void make_bumpy_floor(int brick_type, int dirt_floor)
{
 int i;

 int height = 0;
 int stepped = 0;
 int last_drop = 0;
 int drop = 0;

 int max_height = arena[0].bricks_y / 2;

 height = prand(max_height);

 for (i = 0; i < arena[0].bricks_x; i ++)
 {
  if (height < max_height)
  {
   height = max_height;
   stepped = 0;
  }
  if (height >= arena[0].bricks_y - 1)
  {
   height = arena[0].bricks_y - 3;
   stepped = 0;
  }

  drop_bricks(i, height, brick_type, dirt_floor);
  if (last_drop > 0)
   last_drop --;
  if (stepped > 0)
  {
   height ++;
   stepped --;
   continue;
  }
  if (stepped < 0)
  {
   height --;
   stepped ++;
   continue;
  }
 
  if (prand(4) == 0)
  {
   height += prand(3);
   height -= prand(3);
//   height -= 2;
   continue;
  }
  if (prand(10) == 0)
  {
   stepped += prand(10) + 1;
   continue;
  }
  if (prand(10) == 0)
  {
   stepped -= prand(10) + 1;
   continue;
  }
  if (last_drop == 0 && prand(10) == 0)
  {
   drop = -10 - prand(10);
   if (height + drop < max_height)
    continue;
   height += drop;
   putpixel(arena[0].bricks, i - 2, height - 4, brick_type);
   last_drop += 10;
   continue;
  }
  if (last_drop == 0 && prand(10) == 0)
  {
   drop = 10 + prand(10);
   if (height + drop > arena[0].bricks_y - 1)
    continue;
   putpixel(arena[0].bricks, i + 2, height - 4, brick_type);
   height += drop;
   last_drop += 10;
   continue;
  }

 }

}


void make_sky_platforms(int brick_type)
{

 int min_height = arena[0].bricks_y / 2 - 1;

 int btype = brick_type;

 int number = 15;
 int timeout = 0;
 int i = 0;
 int plat_x, plat_y, width, height;

 number = arena[0].max_x * arena[0].max_y / (125000 / arena[0].solid_density);

 if (number == 0)
  return;

 if (number > 30)
  number = 30;

 for (i = 0; i < number; i ++)
 {
  do
  {
   plat_x = prand(arena[0].bricks_x + 1);
   plat_y = prand(min_height + 1);
   height = 9 + prand(20);
   width = 9 + prand(20);
   timeout ++;
   if (timeout > 500)
    return;
  }
   while (check_brick_area(plat_x, plat_x + width, plat_y, plat_y + height) == 0);
   
  timeout = 0;

//  rectfill(arena[0].bricks, plat_x + 3, plat_y + 3, plat_x + width - 3, plat_y + height - 3, BRICK_ROCK);
  if (brick_type == -1)
  {
   if (arena[0].level_colours == COLOURS_CLASSIC)
    btype = BRICK_STONE;
   if (arena[0].level_colours == COLOURS_HELL)
    btype = BRICK_DIRT;
   if (arena[0].level_colours == COLOURS_EARTH)
   {
    switch(prand(5))
    {
     case 0: btype = BRICK_STONE; break;
     case 1: btype = BRICK_RED_BRICK; break;
     case 2: btype = BRICK_YELLOW_STONE; break;
     case 3: btype = BRICK_YELLOW_CROSS; break;
     case 4: btype = BRICK_WHITE_TILE; break;
    }
   }
   if (arena[0].level_colours == COLOURS_CRAZY)
   {
    switch(prand(9))
    {
     case 0: btype = BRICK_STONE; break;
     case 1: btype = BRICK_RED_BRICK; break;
     case 2: btype = BRICK_YELLOW_STONE; break;
     case 3: btype = BRICK_YELLOW_CROSS; break;
     case 4: btype = BRICK_WHITE_TILE; break;
     case 7:
     case 5: btype = BRICK_BLUE_BRICK; break;
     case 8:
     case 6: btype = BRICK_GREEN_CIRCLE; break;
    }
   }
  }
  make_shape(plat_x + 3, plat_y + 3, plat_x + width - 3, plat_y + height - 3, btype);

 }


}


// Returns 1 if empty, 0 if something solid present
char check_brick_area(int bx1, int bx2, int by1, int by2)
{

 int i, j;

 for (i = bx1; i < bx2 + 1; i ++)
 {
  for (j = by1; j < by2 + 1; j ++)
  {
   if (i <= 0 || i >= arena[0].bricks_x)
    return 0;
   if (j <= 0 || j >= arena[0].bricks_y)
    return 0;
   if (is_solid_brick(getpixel(arena[0].bricks, i, j)) == 1)
    return 0;
  }
 }

 return 1;

}


char is_solid_brick(int brick_type)
{
 if (brick_type < BRICK_SOLID || brick_type == BRICK_DIRT)
  return 0;
/* switch(brick_type)
 {
  case BRICK_ROCK:
   return 1;
  case BRICKBACK_DIRT:
   return 0;
 }*/

 return 1;

}


void drop_bricks(int x, int y, int brick_type, int dirt_floor)
{
 int i;

 for (i = y; i < arena[0].bricks_y + 1; i ++)
 {
  if (i < y + dirt_floor)
   putpixel(arena[0].bricks, x, i, BRICK_DIRT);
    else
     putpixel(arena[0].bricks, x, i, brick_type);
 }

}


void make_shape(int x1, int y1, int x2, int y2, int substance)
{

 int i, j;

 switch(prand(7))
 {
  default:
  case 0:
   rectfill(arena[0].bricks, x1, y1, x2, y2, substance);
   break;
  case 1:
   line(arena[0].bricks, x1, y2, x2, y2, substance);
   break;
  case 3:
   line(arena[0].bricks, x1, y2, x2, y2, substance);
   line(arena[0].bricks, x1, y1, x1, y2, substance);
   line(arena[0].bricks, x2, y1, x2, y2, substance);
   break;
  case 4:
   line(arena[0].bricks, x1, y1, x2, y1, substance);
   line(arena[0].bricks, x1, y1, x1, y2, substance);
   line(arena[0].bricks, x2, y1, x2, y2, substance);
   break;
  case 5:
   i = (x2 - x1) / 2;
   j = (y2 - y1) / 2;
   if (j < i) i = j;
   if (i < 1)
    break;
   circlefill(arena[0].bricks, x1 + i, y1 + i, i - 1, substance);
   break;
  case 6:
   i = (x2 - x1) / 2;
   triangle(arena[0].bricks, x1, y2, x2, y2, x1 + i, y1, substance);
   break;
 }

}


void place_ivy(void)
{

 int x, y, timeout = 0, i;

 do
 {
  x = 10 + prand(arena[0].max_x - 10);
  y = 10 + prand(arena[0].max_y - 10);
  timeout ++;
  if (timeout > 5000)
   return;
 } while (getpixel(shadow_bmp, x, y) != PIX_PERM);

 for (i = 0; i < 1 + prand(5); i ++)
 {
  grow_ivy(x * GRAIN, y * GRAIN);
 }


}


void grow_ivy(int source_x, int source_y)
{

 int x = source_x, y = source_y, i;
 int length = (150 + prand(200));
 float angle = (PI * prand(200)) / 100;
// float curve = 0;
// int curving = 0;
 int x_gain = 0, y_gain = 0;
 int leafy = 0;

 float base_angle = angle;

 for (i = 0; i < length; i ++)
 {
  x_gain = (float) cos(angle) * GRAIN;
  y_gain = (float) sin(angle) * GRAIN;
  x += x_gain;
  y += y_gain;
  if ((x + x_gain) / GRAIN >= arena[0].max_x || (y + y_gain) / GRAIN >= arena[0].max_y ||
   getpixel(shadow_bmp, (x + x_gain) / GRAIN, (y + y_gain) / GRAIN) != PIX_PERM)
  {
   base_angle += prand(PI * 60) / 60;
   base_angle -= prand(PI * 60) / 60;
   angle = base_angle;
   x -= x_gain;
   y -= y_gain;
   continue;
  }
  putpixel(dirt_bmp, x / GRAIN, y / GRAIN, ambience(COLOUR_BROWN4));
  if (prand(3) == 0)
  {
   if (angle < base_angle || prand(3) == 0)
    angle += (float) prand(PI * 30) / 60;
    
   if (angle > base_angle || prand(3) == 0)
    angle -= (float) prand(PI * 30) / 60;
//   continue;
  }
  leafy --;
  if (leafy <= 0)
  {
   grow_leaf(x, y, angle);
   leafy = 5 + prand(30);
  }
//  continue;
/*  if (curving > 0)
  {
   angle += curve;
   curving --;
   continue;
  }
  if (curving < 0)
  {
   angle -= curve;
   curving ++;
   continue;
  }
  if (angle < base_angle)
   curving = prand(10) + 5;
    else
     curving = prand(10) - 15;
  curve = 0;
//  if (prand(5) == 0)
//   continue;
  curve += (float) prand(PI * 30) / 100 + PI / 128;
//  curve -= (float) prand(PI * 30) / 80;*/
 }


}

void grow_leaf(int source_x, int source_y, float stalk_angle)
{

 int x = source_x, y = source_y, i;
 int length = 5 + prand(5);
 float angle = stalk_angle + PI / 2; //(PI * prand(200)) / 100;
 if (prand(2))
  angle = stalk_angle - PI / 2;
// float curve = 0;
// int curving = 0;
 int x_gain = 0, y_gain = 0;

// float base_angle = angle;

 for (i = 0; i < length; i ++)
 {
  x_gain = (float) cos(angle) * GRAIN;
  y_gain = (float) sin(angle) * GRAIN;
  x += x_gain;
  y += y_gain + (length * GRAIN / 5);
  if ((x + x_gain) / GRAIN >= arena[0].max_x || (y + y_gain) / GRAIN >= arena[0].max_y ||
   getpixel(shadow_bmp, (x + x_gain) / GRAIN, (y + y_gain) / GRAIN) != PIX_PERM)
  {
   return;
  }
  putpixel(dirt_bmp, x / GRAIN, y / GRAIN, ambience(COLOUR_GREEN4));
/*  if (prand(3) == 0)
  {
   if (angle < base_angle || prand(3) == 0)
    angle += (float) prand(PI * 30) / 60;
    
   if (angle > base_angle || prand(3) == 0)
    angle -= (float) prand(PI * 30) / 60;
//   continue;
  }*/
//  continue;
/*  if (curving > 0)
  {
   angle += curve;
   curving --;
   continue;
  }
  if (curving < 0)
  {
   angle -= curve;
   curving ++;
   continue;
  }
  if (angle < base_angle)
   curving = prand(10) + 5;
    else
     curving = prand(10) - 15;
  curve = 0;
//  if (prand(5) == 0)
//   continue;
  curve += (float) prand(PI * 30) / 100 + PI / 128;
//  curve -= (float) prand(PI * 30) / 80;*/
 }


}

/*
To Do:

int find_floor_area(int fx, int fy, int found [2]);

Function that looks at random locations. If location is clear, runs a line
directly down and sees if area of floor below with fx x fy free space above
it. If so, puts coordinates in found.

If it gives up, return 0.
*/
