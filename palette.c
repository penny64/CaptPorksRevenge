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

    You should be able to reach Captain Pork by sending an email to
    captainpork@fastmail.fm or sending a letter to
    Captain Pork, 3/4 Kensington Rd, Rose Park, SA 5067 Australia
    but my lease is a short-term one so email is probably your best bet.

File: Palette.c
History:
1/5/03 - Started (Capt Pork)

This file contains:
The palette functions

*/

#include "allegro.h"


#include "config.h"


extern COLOR_MAP trans_table;
int blend_function(int x, int y, RGB *rgbl);
int limit_colour(int colour_input);
int average_colours(int colour1, int colour2);
extern RGB palet [256];
int adjust_lit_colour(int col, int lit);
int add_light_to_colour(int col, int lit);
void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal);
int get_lower_colour(int y);
int colour_illumination(int y);
int base_colour(int y);
int colour_brightness(int y);

void colour_table(const char *which_call);
//void colour_table(void);

RGB mono_palet [256];

/*
Colours:

0 - Black (trans)
1-8 - Greys
9-12 - Browns
13-16 - Yellows
17-20 - Reds
21-24 - Greens
25-28 - Blues
29-32 - Misc


+ 0 - Blue - colour 28
+ 1 - Yellow - 16
+ 2 - Red - 20
+ 3 - Green - 24
+ 4 - White - 8
+ 5 - Black (unlit)
+ 6 - Dimly lit
+ 7 - Originals

+ 0 - Blue - colour 28
+ 8 - Yellow -
+ 16 - Orange - 16
+ 24 - Red
+ 32 - Green - 24
+ 64 - White - 8
+ 96 - Black (unlit)
+ 128 - Dimly lit
+ 160 - Less Dim
+ 192 - Brightly lit (more so than 224+)
+ 224 - Normal



TO DO!

Black (0, 32, 64 etc) in every sub-palette should still be black!



*/

int base_palette [32] [3] =
{
{0, 0, 0},
{4, 4, 4},
{8, 8, 8},
{14, 14, 14},
{20, 20, 20},
{28, 28, 28},
{38, 38, 38},
{50, 50, 50},
{63, 63, 63}, // greys
{8, 5, 3},
{16, 10, 6}, // 10
{24, 15, 9},
{32, 20, 12}, // browns
{16, 16, 0},
{24, 24, 0},
{32, 32, 0},
{48, 48, 0}, // yellows
{16, 0, 0},
{32, 0, 0},
{48, 0, 0},
{63, 0, 0}, // Reds - 20
{0, 16, 0},
{0, 32, 0},
{0, 48, 0},
{0, 63, 0}, // Greens
{0, 0, 16},
{0, 0, 32},
{0, 0, 48},
{0, 0, 63}, // Blues
{48, 0, 48}, // Purple
{50, 50, 50}, // Writing
{0, 63, 0} // Green
//{0, 63, 0} // Green

};



void init_palette(void)
{

int i;


// colour_table("First");

 for (i = 0; i < 8; i ++)
 {
    // Blue
    palet[i].r = 0;
    palet[i].g = 0;
    palet[i].b = limit_colour(32 + i * 4);

    // Yellow
    palet[i + 8].r = limit_colour(48 + i * 3);
    palet[i + 8].g = limit_colour(32 + i * 4);
    palet[i + 8].b = limit_colour(i * 3 - 10);

    // Orange
    palet[i + 16].r = limit_colour(44 + i * 3);
    palet[i + 16].g = limit_colour(10 + i * 3);
    palet[i + 16].b = limit_colour(i * 3 - 10);

    // Red
    palet[i + 24].r = limit_colour(48 + i * 3);
    palet[i + 24].g = 0;
    palet[i + 24].b = 0;

    // Green
    palet[i + 32].r = 0;
    palet[i + 32].g = 0;
    palet[i + 32].b = 0;

    palet[i + 40].r = 0;
    palet[i + 40].g = i * 3;
    palet[i + 40].b = 0;

    palet[i + 48].r = 0;
    palet[i + 48].g = limit_colour(10 + i * 4);
    palet[i + 48].b = 0;
    
    palet[i + 56].r = 0;//limit_colour(i * 3);
    palet[i + 56].g = limit_colour(25 + i * 5);
    palet[i + 56].b = 0;

    // White
    palet[i + 64].r = 0;
    palet[i + 64].g = 0;
    palet[i + 64].b = 0;

    palet[i + 72].r = i * 3;
    palet[i + 72].g = i * 3;
    palet[i + 72].b = i * 3;

    palet[i + 80].r = limit_colour(10 + i * 5);
    palet[i + 80].g = limit_colour(10 + i * 5);
    palet[i + 80].b = limit_colour(10 + i * 5);
    

    palet[i + 88].r = limit_colour(25 + i * 10);
    palet[i + 88].g = limit_colour(25 + i * 10);
    palet[i + 88].b = limit_colour(25 + i * 10);
    
/*    k = base_palette [i] [0] + base_palette [i] [1] + base_palette [i] [2];
    k /= 2;
    if (k > 20) k = 20;
    if (k < 5) k = 5;
    k = 15;
//    palet[i + 64].r = limit_colour(base_palette [i] [0] + k);
//    palet[i + 64].g = limit_colour(base_palette [i] [1] + k);
//    palet[i + 64].b = limit_colour(base_palette [i] [2] + k);
    palet[i + 64].r = limit_colour((base_palette [i] [0] * 15) / 10);
    palet[i + 64].g = limit_colour((base_palette [i] [1] * 15) / 10);
    palet[i + 64].b = limit_colour((base_palette [i] [2] * 15) / 10);*/
    
  }

  palet[0].r = 0;
  palet[0].g = 0;
  palet[0].b = 0;
/*  palet[8].r = 0;
  palet[8].g = 0;
  palet[8].b = 0;
  palet[16].r = 0;
  palet[16].g = 0;
  palet[16].b = 0;
  palet[24].r = 0;
  palet[24].g = 0;
  palet[24].b = 0;*/
  
// colour_table("Second");


  for (i = 0; i < 32; i ++)
  {

/*    colour_1 [0] = &col1; // base
    colour_1 [0]->r = base_palette [i] [0];
    colour_1 [0]->g = base_palette [i] [1];
    colour_1 [0]->b = base_palette [i] [2];
    set_color(i + 224, colour_1 [0]);*/

//    colour_1 [0] = &col1; // base
    palet[i + 224].r = base_palette [i] [0];
    palet[i + 224].g = base_palette [i] [1];
    palet[i + 224].b = base_palette [i] [2];

    palet[i + 96].r = 0;
    palet[i + 96].g = 0;
    palet[i + 96].b = 0;

    palet[i + 128].r = limit_colour(base_palette [i] [0] / 8) * 3;
    palet[i + 128].g = limit_colour(base_palette [i] [1] / 8) * 3;
    palet[i + 128].b = limit_colour(base_palette [i] [2] / 8) * 3;

    palet[i + 160].r = limit_colour((base_palette [i] [0] / 4) * 3);
    palet[i + 160].g = limit_colour((base_palette [i] [1] / 4) * 3);
    palet[i + 160].b = limit_colour((base_palette [i] [2] / 4) * 3);

    palet[i + 192].r = limit_colour((base_palette [i] [0] * 7) / 3);
    palet[i + 192].g = limit_colour((base_palette [i] [1] * 7) / 3);
    palet[i + 192].b = limit_colour((base_palette [i] [2] * 7) / 3);


//    set_color(i + 224, &col1);

//    colour_1 [0] = &col1; // blues
//    set_color(i + 96, &col1);
    
//    colour_1 [0] = &col1; // green
//    palet[i + 32].r = limit_colour(base_palette [i] [0] - 10);
//    palet[i + 32].g = limit_colour(base_palette [i] [1] + 30);
//    palet[i + 32].b = limit_colour(base_palette [i] [2] - 10);
//    set_color(i + 128, &col1);
    
//    colour_1 [0] = &col1; // black
//    palet[i + 120].r = limit_colour(base_palette [i] [0] - 15);
//    palet[i + 160].g = limit_colour(base_palette [i] [1] - 15);
//    palet[i + 160].b = limit_colour(base_palette [i] [2] - 15);
//    set_color(i + 160, &col1);
    
//    colour_1 [0] = &col1; // white
/*
    colour_1 [0] = &col1; // blues
    colour_1 [0]->r = limit_colour(base_palette [i] [0] - 10);
    colour_1 [0]->g = limit_colour(base_palette [i] [1] - 10);
    colour_1 [0]->b = average_colours(base_palette [i] [2], 80);
    set_color(i, colour_1 [0]);
    
    colour_1 [0] = &col1; // yellows
    colour_1 [0]->r = average_colours(base_palette [i] [0], 80);
    colour_1 [0]->g = average_colours(base_palette [i] [1], 80);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] - 10);
    set_color(i + 32, colour_1 [0]);
    
    colour_1 [0] = &col1; // orange
    colour_1 [0]->r = average_colours(base_palette [i] [0], 80);
    colour_1 [0]->g = average_colours(base_palette [i] [1], 70);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] + 0);
    set_color(i + 64, colour_1 [0]);
    
    colour_1 [0] = &col1; // red
    colour_1 [0]->r = average_colours(base_palette [i] [0], 100);
    colour_1 [0]->g = average_colours(base_palette [i] [1], -10);
    colour_1 [0]->b = average_colours(base_palette [i] [2], -10);
    set_color(i + 96, colour_1 [0]);
    
    colour_1 [0] = &col1; // green
    colour_1 [0]->r = limit_colour(base_palette [i] [0] - 10);
    colour_1 [0]->g = limit_colour(base_palette [i] [1] + 30);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] - 10);
    set_color(i + 128, colour_1 [0]);
    
    colour_1 [0] = &col1; // black
    colour_1 [0]->r = limit_colour(base_palette [i] [0] - 15);
    colour_1 [0]->g = limit_colour(base_palette [i] [1] - 15);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] - 15);
    set_color(i + 160, colour_1 [0]);
    
    colour_1 [0] = &col1; // white*/
/*
    colour_1 [0]->r = limit_colour(average_colours(base_palette [i] [0], 55));
    colour_1 [0]->g = limit_colour(average_colours(base_palette [i] [1], 55));
    colour_1 [0]->b = limit_colour(average_colours(base_palette [i] [2], 55));*/
/*    k = base_palette [i] [0] + base_palette [i] [1] + base_palette [i] [2];
    k /= 2;
    if (k > 20) k = 20;
    if (k < 5) k = 5;
    k = 15;
//    palet[i + 64].r = limit_colour(base_palette [i] [0] + k);
//    palet[i + 64].g = limit_colour(base_palette [i] [1] + k);
//    palet[i + 64].b = limit_colour(base_palette [i] [2] + k);
    palet[i + 64].r = limit_colour((base_palette [i] [0] * 15) / 10);
    palet[i + 64].g = limit_colour((base_palette [i] [1] * 15) / 10);
    palet[i + 64].b = limit_colour((base_palette [i] [2] * 15) / 10);*/
//    set_color(i + 192, &col1);
    
  }

//  colour_1 [0] = &col1; // base
/* for (i = 0; i < 256; i += 32)
 {
  if (i > 224) break;
  palet[i].r = 0;
  palet[i].g = 0;
  palet[i].b = 0;
 }*/
 
/*
  set_color(0, &col1);
  set_color(32, colour_1 [0]);
  set_color(64, colour_1 [0]);
  set_color(96, colour_1 [0]);
  set_color(128, colour_1 [0]);
  set_color(160, colour_1 [0]);
  set_color(192, colour_1 [0]);
  set_color(224, colour_1 [0]);
*/
/*
  This code gives a palette where all transparencies are black

  for (i = 0; i < 224; i ++)
  {

    *colour_1 [0] = col1; // blues
    colour_1 [0]->r = 0;
    colour_1 [0]->g = 0;
    colour_1 [0]->b = 0;
    set_color(i, colour_1 [0]);
  }*/

/*

+ 1 - Blue
+ 2 - Yellow
+ 3 - Orange
+ 4 - Red
+ 5 - Green
+ 6 - Black
+ 7 - White

*/

// colour_table("Third - before set_palette");

   set_palette(palet);

// colour_table("Fourth - after set_palette");


/*  for (i = 0; i < 256; i ++)
  {

/ *    colour_1 [0] = &col1; // base
    colour_1 [0]->r = base_palette [i] [0];
    colour_1 [0]->g = base_palette [i] [1];
    colour_1 [0]->b = base_palette [i] [2];
    set_color(i + 224, colour_1 [0]);* /

//    colour_1 [0] = &col1; // base
    palet[i].r = i;
    palet[i].g = i;
    palet[i].b = i;
}*/

// colour_table("Fifth - after greys");
   set_palette(palet);
// colour_table("Sixth - after greys & set_palette");

//   int x, y;

//   create_trans_tab(&trans_table, palet, 128, 128, 128, NULL);

//   RGB rgb_thing;
//   RGB *rgb = &rgb_thing;

//   RGB *pal = palet;

//   int (*blend_func)(PALETTE pal, int x, int y, RGB *rgbl) = blend_function;
//   void (*blend_func)(palet, x, y, rgb_pt) = blend_function;

//   create_color_table(&trans_table, palet, (void *)blend_func(palet, x, y, rgb_pt), NULL);
//   pork_create_color_table(&trans_table, palet, (int *)blend_func, NULL);
   pork_create_color_table(&trans_table, palet);

   color_map = &trans_table;

   set_palette(palet);

/*   BITMAP *pbmp = create_bitmap(16, 16);

   for (x = 0; x < 16; x ++)
   {
    for (y = 0; y < 16; y ++)
    {
     putpixel(pbmp, y, x, y + (x * 16));
     if (y + (x * 16) < 224)
      putpixel(pbmp, y, x, 0);
    }
   }

   save_bitmap("palbmp3.pcx", pbmp, palet);
*/
 int j;

 for (i = 0; i < 256; i ++)
 {
  j = limit_colour((palet[i].r + palet[i].g + palet[i].b) / 3);
  mono_palet [i].r = j;
  mono_palet [i].g = j;
  mono_palet [i].b = j;
 }

 mono_palet [222].r = 60;
 mono_palet [222].g = 40;
 mono_palet [222].b = 0;
 mono_palet [223].r = 0;
 mono_palet [223].g = 60;
 mono_palet [223].b = 10; // these colours used for 'you win'/'game over'

}

int limit_colour(int colour_input)
{

 if (colour_input < 0) return 0;
 if (colour_input > 63) return 63;
 return colour_input;

}

int average_colours(int colour1, int colour2)
{

 int avge = colour1 + colour2 / 2;
 if (avge < 0) return 0;
 if (avge > 63) return 63;
 return avge;

}

/*
+ 1 - Blue - colour 28
+ 2 - Yellow - 16
+ 3 - Orange - 30
+ 4 - Red - 20
+ 5 - Green - 24
+ 6 - Black - 1
+ 7 - White - 8
*/

int blend_function(int x, int y, RGB *rgbl)
{

 if (x % 32 == 0)
 {
  return y;
 }

 if (y == COLOUR_WRITING)
  return y;

 if (x >= 224)
 {
  if (y == 0)
   return 0;
  if (y < 32)
  {
   return get_lower_colour(x) + (y / 8) * 8;
  }
  if (y < 96)
  {
   return (y / 32) * 32 + x % 32;
  }
  if (y < 128)
  {
   return x % 32 + 96;
  }
  if (y < 160)
  {
   return x % 32 + 128;
  }
  if (y < 192)
  {
   return x % 32 + 160;
  }
  if (y < 224)
  {
   return x % 32 + 192;//(x % 32) + 192;
  }
  return x;
 }


/* if (x == COLOUR_BROWN4) // Grapple-cord colour
 {
  if (y < 224)
  {
   return y;
  }
  if (palet[y].r + palet[y].g + palet[y].b == 0)
  {
   return 0;
  }
  if (palet[y].r + palet[y].g + palet[y].b <= 10)
  {
   return COLOUR_BROWN1;
  }
  if (palet[y].r + palet[y].g + palet[y].b <= 20)
  {
   return COLOUR_BROWN2;
  }
  if (palet[y].r + palet[y].g + palet[y].b <= 36)
  {
   return COLOUR_BROWN3;
  }
   return COLOUR_BROWN4;
 }
*/
/* if (x >= LIGHT_1 && x <= LIGHT_4)
 {
  if (y >= 224)
  {
   lit = x - LIGHT_1 + 3;
   found_colour = adjust_lit_colour(y, lit);
//   if (found_colour < 224 && found_colour > 0)
//    exit(500);
   return found_colour;
  }
  if (y >= 192)
  {
   lit = x - LIGHT_1 + 3;
   found_colour = adjust_lit_colour(y, lit);
//   if (found_colour < 224 && found_colour > 0)
//    exit(500);
   return found_colour;
  }
  if (y >= 160)
  {
   lit = x - LIGHT_1 + 3;
   found_colour = adjust_lit_colour(y, lit);
//   if (found_colour < 224 && found_colour > 0)
//    exit(500);
   return found_colour;
  }
//  z = y;
  lit = x - LIGHT_1 + 1;
  found_colour = adjust_lit_colour(y % 32 + 224, lit);
  found_colour = (found_colour % 32) + y / 32;
  rgbl->r = palet[found_colour].r;
  rgbl->g = palet[found_colour].g;
  rgbl->b = palet[found_colour].b;
  rgbl->r = palet[y].r;
  rgbl->g = palet[y].g;
  rgbl->b = palet[y].b;
  return y;
//  return;
  
 }*/

/*
+ 96 - Black (unlit)
+ 128 - Dimly lit
+ 160 - Less Dim
+ 192 - Brightly lit (more so than 224+)
+ 224 - Normal
*/

 if (x == LIGHT_1)
 {
  if (y == PIX_PERM)
   return y; // so that LIGHT_x can be used on the shadow bmp in level
               // creation.
  if (y >= 96 && y < 128)
  {
   return y + 32;
  }
  return y;
 }

 if (x == LIGHT_2)
 {
  if (y == PIX_PERM)
   return y;
  if (y >= 96 && y < 128)
  {
   return y + 64;
  }
  if (y >= 128 && y < 160)
  {
   return y + 32;
  }
  return y;
 }

 if (x == LIGHT_3)
 {
  if (y == PIX_PERM)
   return y;
  if (y >= 96 && y < 128)
  {
   return y + 128;
  }
  if (y >= 128 && y < 160)
  {
   return y + 96;
  }
  if (y >= 160 && y < 192)
  {
   return y + 64;
  }
  return y;
 }

 if (x == LIGHT_4)
 {
  if (y == PIX_PERM)
   return y;
  if (y >= 96 && y < 128)
  {
   return y + 96;
  }
  if (y >= 128 && y < 160)
  {
   return y + 64;
  }
  if (y >= 160 && y < 192)
  {
   return y + 32;
  }
  if (y >= 192 && y < 224)
  {
   return y;
  }
  if (y >= 224)
  {
   return y - 32;
  }
  return y;
 }


 if (x == FORCE_LIGHT_1)
 {
  if (y == PIX_PERM)
   return y;
  if (y < 96)
   return y;
  return y % 32 + 96;
 }

 if (x == FORCE_LIGHT_2)
 {
  if (y == PIX_PERM)
   return y;
  if (y < 96)
   return y;
  return y % 32 + 128;
 }

 if (x == FORCE_LIGHT_3)
 {
  if (y == PIX_PERM)
   return y;
  if (y < 96)
   return y;
  return y % 32 + 160;
 }

 if (x == FORCE_LIGHT_4)
 {
  if (y == PIX_PERM)
   return y;
  if (y < 96)
   return y;
  return y % 32 + 224;
 }

 if (x == FORCE_LIGHT_5)
 {
  if (y == PIX_PERM)
   return y;
  if (y < 96)
   return y;
  return y % 32 + 192;
 }


 if (x == DARK_1)
 {
  if (y == PIX_PERM)
   return y; // so that LIGHT_x can be used on the shadow bmp in level
               // creation.
  if (y >= 192 && y < 224)
  {
   return y + 32;
  }
  return y;
 }

 if (x == DARK_2)
 {
  if (y == PIX_PERM)
   return y;
  if (y >= 192 && y < 224)
  {
   return y - 32;
  }
  if (y >= 224 && y < 256)
  {
   return y - 64;
  }
  return y;
 }

 if (x == DARK_3)
 {
  if (y == PIX_PERM)
   return y;
  if (y >= 192 && y < 224)
  {
   return y - 64;
  }
  if (y >= 224 && y < 256)
  {
   return y - 96;
  }
  if (y >= 160 && y < 192)
  {
   return y - 32;
  }
  return y;
 }

 if (x == DARK_4)
 {
  if (y == PIX_PERM)
   return y;
  if (y >= 96 && y < 128)
  {
   return y;
  }
  if (y >= 128 && y < 160)
  {
   return y - 32;
  }
  if (y >= 160 && y < 192)
  {
   return y - 64;
  }
  if (y >= 192 && y < 224)
  {
   return y - 64;
  }
  if (y >= 224)
  {
   return y - 128;
  }
  return y;
 }

 if (x == FADE_OUT)
 {
  if (y >= 96 && y < 128)
  {
   return 0;
  }
  if (y >= 128 && y < 160)
  {
   return y - 32;
  }
  if (y >= 160 && y < 192)
  {
   return y - 32;
  }
  if (y >= 192 && y < 224)
  {
   return y + 32;
  }
  if (y >= 224)
  {
   return y - 64;
  }
  return 0;
  if (y % 8 == 0)
   return y;
  return y - 1; //(y % 8 + (y / 8) * 8) - 1;
 }

 if (x != TRANS_BLUE
     && x != TRANS_YELLOW
     && x != TRANS_ORANGE
     && x != TRANS_RED
     && x != TRANS_GREEN
     && x != TRANS_BLACK
     && x != TRANS_WHITE)
      {
       rgbl->b = 63;
       rgbl->r = 0;
       rgbl->g = 0;
       return 8;
      }


 if (x == TRANS_BLUE)
 {
  return get_lower_colour(y);
/*  if (y % 32 == COLOUR_GREY8 % 32)
   return 7;
  
  if (y % 32 >= COLOUR_GREY1 % 32 && y % 32 <= COLOUR_GREY8 % 32)
   return (y % 32);
   
  if (y % 32 >= COLOUR_BROWN1 % 32 && y % 32 <= COLOUR_BLUE4 % 32)
   return ((((y % 32) - (COLOUR_BROWN1 % 32)) % 4 + 1) * 3) / 2;
*/
//  return y % 32;
 }

 if (x == TRANS_YELLOW)
 {
  if (y < 8)
  {
   return y;
  }
  if (y < 32)
  {
   return y % 8 + 8;
  }
  return get_lower_colour(y) + 8;
 }

 if (x == TRANS_ORANGE)
 {
  if (y < 16)
  {
   return y;
  }
  if (y < 32)
  {
   return y % 8 + 16;
  }
  return get_lower_colour(y) + 16;
 }

 if (x == TRANS_RED)
 {
  if (y < 24)
  {
   return y;
  }
  if (y < 32)
  {
   return y % 8 + 24;
  }
  return get_lower_colour(y) + 24;
 }

 if (y >= 96 && y < 128)
 {
     return 0;
 }


 if (x == TRANS_GREEN)
 {
  if (y < 64)
  {
   return y;
  }
  if (y < 128)
   return get_lower_colour(y) + 32;
  if (y < 160)
   return get_lower_colour(y) + 40;
  if (y < 192)
   return get_lower_colour(y) + 48;
  if (y < 224)
   return get_lower_colour(y) + 56;
   
  return get_lower_colour(y) + 48;
 }

 if (x == TRANS_WHITE)
 {
  if (y < 96)
  {
   return y;
  }
  if (y < 128)
   return get_lower_colour(y) + 64;
  if (y < 160)
   return get_lower_colour(y) + 72;
  if (y < 192)
   return get_lower_colour(y) + 80;
  if (y < 224)
   return get_lower_colour(y) + 88;
   
  return get_lower_colour(y) + 80;
 }

 return 0;

}



int get_lower_colour(int y)
{

  if (y % 32 == COLOUR_GREY8 % 32)
   return 7;
  
  if (y % 32 >= COLOUR_BLACK % 32 && y % 32 <= COLOUR_GREY8 % 32)
   return (y % 32);
   
  if (y % 32 >= COLOUR_BROWN1 % 32 && y % 32 <= COLOUR_BLUE4 % 32)
   return ((((y % 32) - (COLOUR_BROWN1 % 32)) % 4 + 1) * 3) / 2;

 return 7;

}


// Takes a non-trans colour at any illumination and converts it to one of the
//  224+ 'normal' colours for the purposes of transparency
int colour_illumination(int y)
{

 int illum = 3; // 224+
 if (y < 224) illum = 4;
 if (y < 192) illum = 2;
 if (y < 160) illum = 1;
 if (y < 128) illum = 0;

 if (illum == 0)
  return 96 + y % 32;
 if (illum == 3)
  return y;
  
 int bright = colour_brightness(y);
 if (illum == 1)
  bright /= 2;
 if (illum == 2)
 {
  bright *= 2;
  bright /= 3;
 }
 if (illum == 4)
 {
  bright *= 3;
  bright /= 2;
  if (bright > 8)
   bright = 8;
 }

 if (y % 32 > COLOUR_GREY8 % 32)
  bright /= 2;

 return base_colour(y) + bright;
 

}


int base_colour(int y)
{
 switch(y % 32)
 {
  case COLOUR_GREY1:
  case COLOUR_GREY2:
  case COLOUR_GREY3:
  case COLOUR_GREY4:
  case COLOUR_GREY5:
  case COLOUR_GREY6:
  case COLOUR_GREY7:
  case COLOUR_GREY8:
   return COLOUR_GREY1;
  case COLOUR_BROWN1:
  case COLOUR_BROWN2:
  case COLOUR_BROWN3:
  case COLOUR_BROWN4:
   return COLOUR_BROWN1;
  case COLOUR_BLUE1:
  case COLOUR_BLUE2:
  case COLOUR_BLUE3:
  case COLOUR_BLUE4:
   return COLOUR_BLUE1;
  case COLOUR_RED1:
  case COLOUR_RED2:
  case COLOUR_RED3:
  case COLOUR_RED4:
   return COLOUR_RED1;
  case COLOUR_YELLOW1:
  case COLOUR_YELLOW2:
  case COLOUR_YELLOW3:
  case COLOUR_YELLOW4:
   return COLOUR_YELLOW1;
  case COLOUR_GREEN1:
  case COLOUR_GREEN2:
  case COLOUR_GREEN3:
  case COLOUR_GREEN4:
   return COLOUR_GREEN1;
 }

 return COLOUR_BLACK;

}

// Returns how bright a colour looks at normal illumination
int colour_brightness(int y)
{
 int i = 0;

 switch(y % 32)
 {
  case COLOUR_GREY1:
   i = 1;
   break;
  case COLOUR_GREY2:
  case COLOUR_BROWN1:
  case COLOUR_RED1:
  case COLOUR_YELLOW1:
  case COLOUR_GREEN1:
  case COLOUR_BLUE1:
   i = 2;
   break;
  case COLOUR_GREY3:
   i = 3;
   break;
  case COLOUR_GREY4:
  case COLOUR_BROWN2:
  case COLOUR_RED2:
  case COLOUR_YELLOW2:
  case COLOUR_GREEN2:
  case COLOUR_BLUE2:
   i = 4;
   break;
  case COLOUR_GREY5:
   i = 5;
   break;
  case COLOUR_GREY6:
  case COLOUR_BROWN3:
  case COLOUR_RED3:
  case COLOUR_YELLOW3:
  case COLOUR_GREEN3:
  case COLOUR_BLUE3:
   i = 6;
   break;
  case COLOUR_GREY7:
   i = 7;
   break;
  case COLOUR_GREY8:
  case COLOUR_BROWN4:
  case COLOUR_RED4:
  case COLOUR_YELLOW4:
  case COLOUR_GREEN4:
  case COLOUR_BLUE4:
   i = 8;
   break;
  default:
   i = 0;
   break;
 }

 return i;

}


/*
This function had to be modified from the allegro create_color_table
because the allegro version used bestfit_color, whereas we need
specific color values (eg so that there can be multiple blacks to carry
information in areas of the screen that haven't been lightsourced yet)
*/
void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal)
//void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal, (int *)(blend)(AL_CONST PALETTE pal, int x, int y, RGB *rgb), void (*callback)(int pos))
{
   int x, y, z;
   RGB c;

   for (x=0; x<PAL_SIZE; x++) {
      for (y=0; y<PAL_SIZE; y++) {
   z = blend_function(x, y, &c);

//   if (rgb_map)
      table->data[x][y] = z;//rgb_map->data[c.r>>1][c.g>>1][c.b>>1];
//   else
//      table->data[x][y] = bestfit_color(pal, c.r, c.g, c.b);
      }

   }
}



//void create_color_table(COLOR_MAP *table, const PALETTE pal, void (*blend)(PALETTE pal, int x, int y, RGB *rgb), void (*callback)(int pos));
/*     Fills the specified color mapping table with lookup data for doing
     customised effects with the specified palette, calling the blend
     function to determine the results of each color combination. Your
     blend routine will be passed a pointer to the palette and the two
     colors which are to be combined, and should fill in the RGB
     structure with the desired result in 0-63 format. Allegro will
     then search the palette for the closest match to the RGB color
     that you requested, so it doesn't matter if the palette has no
     exact match for this color. If the callback function is not NULL,
     it will be called 256 times during the calculation, allowing you
     to display a progress indicator.*/






void init_palette2_not_working(void)
{

int i;

int k;

//  RGB *colour_1 [2];

  RGB col1;

  for (i = 0; i < 32; i ++)
  {

/*    colour_1 [0] = &col1; // base
    colour_1 [0]->r = base_palette [i] [0];
    colour_1 [0]->g = base_palette [i] [1];
    colour_1 [0]->b = base_palette [i] [2];
    set_color(i + 224, colour_1 [0]);*/

//    colour_1 [0] = &col1; // base
    col1.r = base_palette [i] [0];
    col1.g = base_palette [i] [1];
    col1.b = base_palette [i] [2];
    set_color(i + 224, &col1);

//    colour_1 [0] = &col1; // blues
    col1.r = limit_colour(base_palette [i] [0] - 10);
    col1.g = limit_colour(base_palette [i] [1] - 10);
    col1.b = average_colours(base_palette [i] [2], 80);
    set_color(i, &col1);
    
//    colour_1 [0] = &col1; // yellows
    col1.r = average_colours(base_palette [i] [0], 80);
    col1.g = average_colours(base_palette [i] [1], 80);
    col1.b = limit_colour(base_palette [i] [2] - 10);
    set_color(i + 32, &col1);
    
//    colour_1 [0] = &col1; // orange
    col1.r = average_colours(base_palette [i] [0], 80);
    col1.g = average_colours(base_palette [i] [1], 70);
    col1.b = limit_colour(base_palette [i] [2] + 0);
    set_color(i + 64, &col1);
    
//    colour_1 [0] = &col1; // red
    col1.r = average_colours(base_palette [i] [0], 100);
    col1.g = average_colours(base_palette [i] [1], -10);
    col1.b = average_colours(base_palette [i] [2], -10);
    set_color(i + 96, &col1);
    
//    colour_1 [0] = &col1; // green
    col1.r = limit_colour(base_palette [i] [0] - 10);
    col1.g = limit_colour(base_palette [i] [1] + 30);
    col1.b = limit_colour(base_palette [i] [2] - 10);
    set_color(i + 128, &col1);
    
//    colour_1 [0] = &col1; // black
    col1.r = limit_colour(base_palette [i] [0] - 15);
    col1.g = limit_colour(base_palette [i] [1] - 15);
    col1.b = limit_colour(base_palette [i] [2] - 15);
    set_color(i + 160, &col1);
    
//    colour_1 [0] = &col1; // white
/*
    colour_1 [0] = &col1; // blues
    colour_1 [0]->r = limit_colour(base_palette [i] [0] - 10);
    colour_1 [0]->g = limit_colour(base_palette [i] [1] - 10);
    colour_1 [0]->b = average_colours(base_palette [i] [2], 80);
    set_color(i, colour_1 [0]);
    
    colour_1 [0] = &col1; // yellows
    colour_1 [0]->r = average_colours(base_palette [i] [0], 80);
    colour_1 [0]->g = average_colours(base_palette [i] [1], 80);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] - 10);
    set_color(i + 32, colour_1 [0]);
    
    colour_1 [0] = &col1; // orange
    colour_1 [0]->r = average_colours(base_palette [i] [0], 80);
    colour_1 [0]->g = average_colours(base_palette [i] [1], 70);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] + 0);
    set_color(i + 64, colour_1 [0]);
    
    colour_1 [0] = &col1; // red
    colour_1 [0]->r = average_colours(base_palette [i] [0], 100);
    colour_1 [0]->g = average_colours(base_palette [i] [1], -10);
    colour_1 [0]->b = average_colours(base_palette [i] [2], -10);
    set_color(i + 96, colour_1 [0]);
    
    colour_1 [0] = &col1; // green
    colour_1 [0]->r = limit_colour(base_palette [i] [0] - 10);
    colour_1 [0]->g = limit_colour(base_palette [i] [1] + 30);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] - 10);
    set_color(i + 128, colour_1 [0]);
    
    colour_1 [0] = &col1; // black
    colour_1 [0]->r = limit_colour(base_palette [i] [0] - 15);
    colour_1 [0]->g = limit_colour(base_palette [i] [1] - 15);
    colour_1 [0]->b = limit_colour(base_palette [i] [2] - 15);
    set_color(i + 160, colour_1 [0]);
    
    colour_1 [0] = &col1; // white*/
/*
    colour_1 [0]->r = limit_colour(average_colours(base_palette [i] [0], 55));
    colour_1 [0]->g = limit_colour(average_colours(base_palette [i] [1], 55));
    colour_1 [0]->b = limit_colour(average_colours(base_palette [i] [2], 55));*/
    k = base_palette [i] [0] + base_palette [i] [1] + base_palette [i] [2];
    if (k > 20) k = 20;
    col1.r = limit_colour(base_palette [i] [0] + k);
    col1.g = limit_colour(base_palette [i] [1] + k);
    col1.b = limit_colour(base_palette [i] [2] + k);
    set_color(i + 192, &col1);
    
  }

//  colour_1 [0] = &col1; // base
  col1.r = 0;
  col1.g = 0;
  col1.b = 0;
  set_color(0, &col1);
  set_color(32, &col1);
  set_color(64, &col1);
  set_color(96, &col1);
  set_color(128, &col1);
  set_color(160, &col1);
  set_color(192, &col1);
  set_color(224, &col1);
/*
  set_color(0, &col1);
  set_color(32, colour_1 [0]);
  set_color(64, colour_1 [0]);
  set_color(96, colour_1 [0]);
  set_color(128, colour_1 [0]);
  set_color(160, colour_1 [0]);
  set_color(192, colour_1 [0]);
  set_color(224, colour_1 [0]);
*/
/*
  This code gives a palette where all transparencies are black

  for (i = 0; i < 224; i ++)
  {

    *colour_1 [0] = col1; // blues
    colour_1 [0]->r = 0;
    colour_1 [0]->g = 0;
    colour_1 [0]->b = 0;
    set_color(i, colour_1 [0]);
  }*/

/*

+ 1 - Blue
+ 2 - Yellow
+ 3 - Orange
+ 4 - Red
+ 5 - Green
+ 6 - Black
+ 7 - White

*/

   set_palette(palet);

//   int x, y;

//   create_trans_tab(&trans_table, palet, 128, 128, 128, NULL);

//   RGB rgb_thing;
//   RGB *rgb = &rgb_thing;

//   RGB *pal = palet;

//   void (*blend_func)(PALETTE pal, int x, int y, RGB *rgbl) = blend_function;
//   void (*blend_func)(palet, x, y, rgb_pt) = blend_function;

//   create_color_table(&trans_table, palet, (void *)blend_func(palet, x, y, rgb_pt), NULL);
//   create_color_table(&trans_table, palet, (void *)blend_func, NULL);

   color_map = &trans_table;


/*   BITMAP *pbmp = create_bitmap(16, 16);

   for (x = 0; x < 16; x ++)
   {
    for (y = 0; y < 16; y ++)
    {
     putpixel(pbmp, y, x, y + (x * 16));
     if (y + (x * 16) < 224)
      putpixel(pbmp, y, x, 0);
    }
   }

   save_bitmap("palbmp3.pcx", pbmp, palet);
*/
}

int adjust_lit_colour(int col, int lit)
{
 int col2;

 if (lit <= 0)
  return col;

 if (lit == 0) return 0;
// if (lit > 4)
//  lit = 4;
 if (col >= 160 && col < 224)
 {
  return col + 32;
 }

 if (col <= 224) return 0;

 lit ++;

 if (col >= COLOUR_BROWN1 && col <= COLOUR_BLUE4)
 {
  col2 = col - 4 + lit;
 }
 if (col >= COLOUR_GREY1 && col <= COLOUR_GREY8)
 {
  col2 = col - 8 + lit * 2;
 }

  switch(col)
  {
   case COLOUR_BROWN1: case COLOUR_BROWN2: case COLOUR_BROWN3: case COLOUR_BROWN4:
//    col2 = col;
    if (col2 < COLOUR_BROWN1) col2 = COLOUR_BROWN1;
    if (col2 > COLOUR_BROWN4) col2 = COLOUR_BROWN4;
   break;
   case COLOUR_YELLOW1: case COLOUR_YELLOW2: case COLOUR_YELLOW3: case COLOUR_YELLOW4:
//    col2 = col;
    if (col2 < COLOUR_YELLOW1) col2 = COLOUR_YELLOW1;
    if (col2 > COLOUR_YELLOW4) col2 = COLOUR_YELLOW4;
   break;
   case COLOUR_GREEN1: case COLOUR_GREEN2: case COLOUR_GREEN3: case COLOUR_GREEN4:
//    col2 = col;
    if (col2 < COLOUR_GREEN1) col2 = COLOUR_GREEN1;
    if (col2 > COLOUR_GREEN4) col2 = COLOUR_GREEN4;
   break;
   case COLOUR_RED1: case COLOUR_RED2: case COLOUR_RED3: case COLOUR_RED4:
//    col2 = col;
    if (col2 < COLOUR_RED1) col2 = COLOUR_RED1;
    if (col2 > COLOUR_RED4) col2 = COLOUR_RED4;
//    col2 = COLOUR_PURPLE;
   break;
   case COLOUR_BLUE1: case COLOUR_BLUE2: case COLOUR_BLUE3: case COLOUR_BLUE4:
//    col2 = col;
    if (col2 < COLOUR_BLUE1) col2 = COLOUR_BLUE1;
    if (col2 > COLOUR_BLUE4) col2 = COLOUR_BLUE4;
   break;
   case COLOUR_GREY1: case COLOUR_GREY2: case COLOUR_GREY3: case COLOUR_GREY4:
   case COLOUR_GREY5: case COLOUR_GREY6: case COLOUR_GREY7: case COLOUR_GREY8:
//    col2 = col;
    if (col2 < COLOUR_GREY1) col2 = COLOUR_GREY1;
    if (col2 > COLOUR_GREY8) col2 = COLOUR_GREY8;
   break;
  }
  
 return col2;

}



