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

File: effects.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - A few special effects - the nuclear explosion flash and
   earthquakes, currently
 
*/

#include "allegro.h"

#include "config.h"
#include "globvar.h"

#include "bullet.h"
#include "sound.h"
#include "prand.h"
#include "cloud.h"

extern RGB *palet;
//extern PALETTE white_palette;
//extern PALETTE *wh_pal;

void shake_arena(int how_much)
{

   arena[0].shake_time += how_much;
   if (arena[0].shake_time >= 30) arena[0].shake_time = 30;

//   arena [0].shake_x += prand(how_much) - prand(how_much);
//   arena [0].shake_y += prand(how_much) - prand(how_much);


}

void nuke_flash(void)
{
 arena[0].nuke_flash = 5;
// fade_from(*palet, white_palette, 1);
}


void run_effects(void)
{

 if (arena[0].shake_time > 0)
 {
//  arena[0].shake_x += prand(3) - 1;
//  arena[0].shake_y += prand(3) - 1;
  arena[0].shake_x += prand(5) - 2;
  arena[0].shake_y += prand(5) - 2;
  arena[0].shake_time --;
 } else
  {
   if (arena[0].shake_x > 0) arena[0].shake_x --;
   if (arena[0].shake_x < 0) arena[0].shake_x ++;
   if (arena[0].shake_y > 0) arena[0].shake_y --;
   if (arena[0].shake_y < 0) arena[0].shake_y ++;
  }

 if (arena[0].nuke_flash > 0)
 {
  arena[0].nuke_flash --;
//  if (arena[0].nuke_flash == 30)
//  {
//   fade_from(white_palette, *palet, 32);
//   arena[0].nuke_flash = 0;
//  }
 }

 if (arena[0].replace_dirt > 0)
 {
  arena[0].dirt_count -= arena[0].replace_dirt * arena[0].replace_dirt;
  if (arena[0].dirt_count < 20)
  {
   arena[0].dirt_storm = 5 + prand(25);
   arena[0].dirt_count = 2000;
  }
 }

 int ex, ey, erad;

 if (arena[0].dirt_storm > 0 && prand(5) == 0)
 {
  ex = (prand(arena[0].max_x - 40) + 10) * GRAIN;
  ey = (prand(arena[0].max_y - 40) + 20) * GRAIN;
  erad = 5 + prand(8);
//  create_cloud(CLOUD_SHOCKWAVE, ex, ey, 0, 0, erad * 100, 20, 0, 0);
//  create_cloud(CLOUD_SHOCKWAVE, ex, ey, 0, 0, 1700, 800, 2000, 0);
  dirt_splodge(ex, ey, erad, -1);
  arena[0].dirt_storm --;
  shake_arena(2);
  play_sound(WAV_QUAKE);
 }
 

}
