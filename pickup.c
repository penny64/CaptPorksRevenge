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

File: pickup.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions controlling pickups
 
*/

#include "allegro.h"
#include "libnet.h"

#include "config.h"
#include "globvar.h"

#include "ai.h"
#include "cloud.h"
#include "display.h"
#include "grid.h"
#include "prand.h"
#include "bullet.h"
#include "objhead.h"
#include "async.h"
#include "light.h"

#define PICKUP_TYPES 4

void pickup_expire(int epick);
void destroy_pickup(int dpick);
void move_pickup(int mpick, int move_x2, int move_y2);
void manage_pickup(int mpick);
void move_pickup(int mpick, int move_x2, int move_y2);


int create_pickup(int pickup_type, int contains, int pickup_x, int pickup_y,
 int x_speed, int y_speed, int timeout, int force_index, int wait_clear);

//extern struct arena_struct arena [1];

char weapon_profile [400];
int wprof_num;

char equip_profile [50];
int eprof_num;

char pk_profile [50];
int pk_num;


// NOTE: assumes width & height == 4


void init_pickups(void)
{

 int ipick;

 game[0].weapon_status [WPN_LASER_TRACER] = 0;

 for (ipick = 0; ipick < MAX_PICKUPS; ipick++)
 {
  pickup[ipick].pickup_type = PICKUP_NONE;
  pickup[ipick].wait_clear = 0;
 }
// game[0].pk_time_between = 100;
// game[0].pk_number = 15;
 arena[0].pk_counter = game[0].pk_time_between;

 int epi = 0;
 int wpi = 0;
 int wcn = 2 + NO_EQUIP;
 wprof_num = 0;
 eprof_num = 0;
 pk_num = 0;

 do
 {
  if (game[0].weapon_status [wcn] < 2)
  {
   wcn ++;
   continue;
  }
  if (game[0].weapon_status [wcn] == 3 || game[0].weapon_status [wcn] == 5)
  {
   weapon_profile [wpi] = wcn;
   wprof_num ++;
   wpi ++;
  }
  if (game[0].weapon_status [wcn] == 2 || game[0].weapon_status [wcn] == 4)
  {
   weapon_profile [wpi] = wcn;
   wprof_num ++;
   wpi ++;
   weapon_profile [wpi] = wcn;
   wprof_num ++;
   wpi ++;
   weapon_profile [wpi] = wcn;
   wprof_num ++;
   wpi ++;
   weapon_profile [wpi] = wcn;
   wprof_num ++;
   wpi ++;
  }
  wcn ++;
 } while (wcn < NO_WEAPONS);

 wcn = 0;

 do
 {
  if (game[0].equip_status [wcn] == 0)
  {
   wcn ++;
   continue;
  }
  if (game[0].equip_status [wcn] == 3 || game[0].equip_status [wcn] == 5)
  {
   equip_profile [epi] = wcn;
   eprof_num ++;
   epi ++;
  }
  if (game[0].equip_status [wcn] == 2 || game[0].equip_status [wcn] == 4)
  {
   equip_profile [epi] = wcn;
   eprof_num ++;
   epi ++;
   equip_profile [epi] = wcn;
   eprof_num ++;
   epi ++;
   equip_profile [epi] = wcn;
   eprof_num ++;
   epi ++;
   equip_profile [epi] = wcn;
   eprof_num ++;
   epi ++;
  }
  wcn ++;
 } while (wcn < NO_EQUIP);

 wcn = 0;


  if (game[0].pk_health >= 1)
  {
   pk_profile [wcn] = PICKUP_POWER;
   pk_num ++;
   wcn ++;
  }
  if (game[0].pk_health >= 2)
  {
   pk_profile [wcn] = PICKUP_POWER;
   pk_num ++;
   wcn ++;
   pk_profile [wcn] = PICKUP_POWER;
   pk_num ++;
   wcn ++;
  }
  if (game[0].pk_health >= 3)
  {
   pk_profile [wcn] = PICKUP_POWER;
   pk_num ++;
   wcn ++;
   pk_profile [wcn] = PICKUP_POWER;
   pk_num ++;
   wcn ++;
  }

  if (wprof_num > 0)
  {
   if (game[0].pk_weapon >= 1)
   {
    pk_profile [wcn] = PICKUP_WEAPON;
    pk_num ++;
    wcn ++;
   }
   if (game[0].pk_weapon >= 2)
   {
    pk_profile [wcn] = PICKUP_WEAPON;
    pk_num ++;
    wcn ++;
    pk_profile [wcn] = PICKUP_WEAPON;
    pk_num ++;
     wcn ++;
   }
   if (game[0].pk_weapon >= 3)
   {
    pk_profile [wcn] = PICKUP_WEAPON;
    pk_num ++;
    wcn ++;
    pk_profile [wcn] = PICKUP_WEAPON;
    pk_num ++;
    wcn ++;
   }
  }

  if (wprof_num > 0)
  {
   if (game[0].pk_equip >= 1)
   {
    pk_profile [wcn] = PICKUP_EQUIP;
    pk_num ++;
    wcn ++;
   }
   if (game[0].pk_equip >= 2)
   {
    pk_profile [wcn] = PICKUP_EQUIP;
    pk_num ++;
    wcn ++;
    pk_profile [wcn] = PICKUP_EQUIP;
    pk_num ++;
     wcn ++;
   }
   if (game[0].pk_equip >= 3)
   {
    pk_profile [wcn] = PICKUP_EQUIP;
    pk_num ++;
    wcn ++;
    pk_profile [wcn] = PICKUP_EQUIP;
    pk_num ++;
    wcn ++;
   }
  }

 if (pk_num == 0) game[0].pk_number = 0;

}



int drop_pickups(int want_type, int want_contains)
{

 if (serial[0].game_type == SERIAL_CLIENT)
  return -1;

 int x_pos, y_pos;

 int size = 6;

 if (want_type == -1)
 {
  want_type = pk_profile [prand(pk_num)]; //prand(PICKUP_TYPES);
//  if (prand(3)) want_type = 2;
 }

 if (want_contains == -1)
 {
  switch(want_type)
  {
   case PICKUP_WEAPON: // weapons
   want_contains = weapon_profile [prand(wprof_num)];
   break;
   case PICKUP_EQUIP: // equipment
   want_contains = equip_profile [prand(eprof_num)];
   break;
   case PICKUP_FRUIT: // fruit
   want_contains = FRUITS_BLUEBERRY + prand(4);
   if (prand(5) == 0)
    want_contains = FRUITM_PEACH + prand(3);
   if (prand(25) == 0)
   {
    want_contains = FRUITL_DURIAN + prand(2);
    size = 8;
   }
   break;
  }
 }

 int tries = 0;
 char distant = 0;

 if (want_type == PICKUP_BASE || want_type == PICKUP_TH_BASE)
 {
   size = 18;
 }

 if ((want_type == PICKUP_BASE || want_type == PICKUP_TH_BASE) && arena[0].max_x > 400)
// don't bother making bases distant if max_x is too small
 {
   int i = 0;
   do
   {
    x_pos = prand(arena[0].max_x - 40) + 20;
    y_pos = prand(arena[0].max_y - 40) + 20;
    distant = 1;
    for (i = 0; i < NO_TEAMS; i ++)
    {
       if (team[i].active == 0 || i == want_contains) continue;
       if (x_pos * GRAIN < team[i].base_x + 40 * GRAIN && x_pos * GRAIN > team[i].base_x - 40 * GRAIN)
        distant = 0;
    }
    tries ++;
    if (tries > 10000) return -1;
   } while(!check_free_area(x_pos - size, y_pos - size, x_pos + size, y_pos + size)
            || distant == 0);
 }
  else
   do
   {
    x_pos = prand(arena[0].max_x - 40) + 20;
    y_pos = prand(arena[0].max_y - 40) + 20;
    tries ++;
    if (tries > 2000) return -1;
   } while(!check_free_area(x_pos - size, y_pos - size, x_pos + size, y_pos + size));

 x_pos *= GRAIN;
 y_pos *= GRAIN;

 int pku = -1;

 if ((pku = create_pickup(want_type, want_contains, x_pos, y_pos, 0, 0, 5000, -1, -1)) != -1)
 {
  if (pickup[pku].pickup_type == PICKUP_BASE || pickup[pku].pickup_type == PICKUP_TH_BASE)
   blast_dirt(x_pos, y_pos, 21, 1);
    else
     blast_dirt(x_pos, y_pos, pickup[pku].width * 2, 1);
//  blast_dirt(x_pos, y_pos + (4 * GRAIN), 10);
 }
// textprintf(screen, font, 300, 450, random() % 15 + 1, "%i, %i", x_pos, y_pos);

 return pku;

}



int create_pickup(int pickup_type, int contains, int pickup_x, int pickup_y,
 int x_speed, int y_speed, int timeout, int force_index, int wait_clear)
{

 if (serial[0].game_type == SERIAL_CLIENT && force_index == -1)
  return -1;

   if (game[0].pk_number == 0
   && pickup_type != PICKUP_BASE
   && pickup_type != PICKUP_TH_BASE
   && pickup_type != PICKUP_FRUIT
   && pickup_type != PICKUP_FLAG
   && pickup_type != PICKUP_GRAIL) return -1;

   int pcount = 0;
   int pc2 = 0;
   int j;

   if (force_index != -1)
   {
    pcount = force_index;
   }
    else
    {
     for (pcount = 1; pcount < MAX_PICKUPS; pcount++)
//   for (pcount = 1; pcount < 5; pcount++)
     {
      if (pickup_type != PICKUP_FRUIT
        && pickup_type != PICKUP_BASE
        && pickup_type != PICKUP_TH_BASE
        && pickup_type != PICKUP_GRAIL
        && pickup_type != PICKUP_FLAG
        && pc2 == game[0].pk_number - 1)
         return -1;
      if (pcount == MAX_PICKUPS)
        return -1;
      if (pickup[pcount].pickup_type == PICKUP_NONE) break;
      if (pickup[pcount].pickup_type != PICKUP_FRUIT
          && pickup[pcount].pickup_type != PICKUP_BASE
          && pickup[pcount].pickup_type != PICKUP_TH_BASE
          && pickup[pcount].pickup_type != PICKUP_FLAG)
//          && pickup[pcount].pickup_type != PICKUP_NONE)
       pc2 ++;
     }
   // assumes there's enough space in the pickup struct for all pickups
   //  + all fruit
    }

   if (wait_clear == -1)
   {
    pickup[pcount].wait_clear ++;
    if (pickup[pcount].wait_clear == 256)
     pickup[pcount].wait_clear = 0;
   }
    else
     pickup[pcount].wait_clear = wait_clear;

   if (serial[0].game_type == SERIAL_SERVER)
   {
    async_pickup(pcount, pickup_type, contains, pickup_x, pickup_y, timeout,
     pickup[pcount].wait_clear);
   }

   pickup[pcount].pickup_type = pickup_type;
   pickup[pcount].contains = contains;
   pickup[pcount].x = pickup_x;
   pickup[pcount].y = pickup_y;
   pickup[pcount].x_speed = 0;//x_speed;
   pickup[pcount].y_speed = 0;//y_speed;
   pickup[pcount].width = 4;
   pickup[pcount].height = 4;
   pickup[pcount].timeout = timeout;
   if (pickup_type == PICKUP_BASE)
   {
    pickup[pcount].width = 15;
    pickup[pcount].height = 15;
    pickup[pcount].timeout = 0;
//    pickup[pcount].wait_clear = 0;
    create_cloud(CLOUD_COL_EXPLOSION, pickup_x, pickup_y, 0, 0, 2000, 40, team_colours(contains), 10);
   }
   if (pickup_type == PICKUP_TH_BASE)
   {
    pickup[pcount].width = 15;
    pickup[pcount].height = 15;
    pickup[pcount].timeout = 0;
//    pickup[pcount].wait_clear = 0;
    create_cloud(CLOUD_COL_EXPLOSION, pickup_x, pickup_y, 0, 0, 2000, 40, 15, 10);
   }
   if (pickup_type == PICKUP_GRAIL)
   {
    pickup[pcount].timeout = 0;
   }
   if (pickup_type == PICKUP_FLAG)
   {
    if (serial[0].game_type == SERIAL_CLIENT)
    {
     for (j = 0; j < MAX_PICKUPS; j ++)
     {
      if (j == pcount) continue;
      if (pickup[j].pickup_type == PICKUP_FLAG
          && pickup[j].contains == contains)
           destroy_pickup(j);
      // sometimes there'll be extra flags laying around.
     }
    }
   }
   if (pickup_type == PICKUP_FRUIT)
   {
    switch(contains)
    {
     case FRUITS_CHERRY:
     case FRUITS_BLUEBERRY:
     case FRUITS_FIG:
     case FRUITS_STRAWBERRY:
      pickup[pcount].width = 4;
      pickup[pcount].height = 4;
      break;
     case FRUITM_PEACH:
     case FRUITM_RAPPLE:
     case FRUITM_PEAR:
      pickup[pcount].width = 6;
      pickup[pcount].height = 6;
      break;
     case FRUITL_DURIAN:
     case FRUITL_PINEAPPLE:
      pickup[pcount].width = 7;
      pickup[pcount].height = 7;
      break;
    }
   }

   if (pickup_type != PICKUP_BASE && pickup_type != PICKUP_TH_BASE)
   {
    create_cloud(CLOUD_LBLUE_EXPLOSION, pickup_x, pickup_y, 0, 0, 500, 3, 0, 10);
    create_cloud(CLOUD_LBLUE_EXPLOSION, pickup_x - 10 * GRAIN, pickup_y, 0, 0, 300, 5, 0, 10);
    create_cloud(CLOUD_LBLUE_EXPLOSION, pickup_x + 10 * GRAIN, pickup_y, 0, 0, 300, 5, 0, 10);
    create_cloud(CLOUD_LBLUE_EXPLOSION, pickup_x, pickup_y - 10 * GRAIN, 0, 0, 300, 5, 0, 10);
    create_cloud(CLOUD_LBLUE_EXPLOSION, pickup_x, pickup_y + 10 * GRAIN, 0, 0, 300, 5, 0, 10);
   }

   return pcount;

}




void run_pickups(void)
{


 if (arena[0].pk_counter > 0)
 {
  arena[0].pk_counter ++;
  if (arena[0].pk_counter >= game[0].pk_time_between)
  {
   drop_pickups(-1, -1);
   arena[0].pk_counter = 1;
  }
 } else return; // no pickups in game


 int rpick;

 for (rpick = 1; rpick < MAX_PICKUPS; rpick++)
 {
//  if (pickup[rpick].wait_clear > 0) pickup[rpick].wait_clear --;
  if (pickup[rpick].pickup_type != PICKUP_NONE) manage_pickup(rpick);
 }

 
}


void run_fruit(void)
{

 int no_fr = 0;
 int i;

 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (pickup[i].pickup_type == PICKUP_FRUIT)
   no_fr ++;
 }

 if (no_fr < game[0].fruit_no)
 {
  drop_pickups(PICKUP_FRUIT, -1);
 }

}


void pickup_expire(int epick)
{
 create_cloud(CLOUD_LBLUE_EXPLOSION, pickup [epick].x, pickup[epick].y, 0, 0, 500, 3, 0, 10);
 create_cloud(CLOUD_LBLUE_EXPLOSION, pickup [epick].x - 10 * GRAIN, pickup [epick].y, 0, 0, 300, 5, 0, 10);
 create_cloud(CLOUD_LBLUE_EXPLOSION, pickup [epick].x + 10 * GRAIN, pickup [epick].y, 0, 0, 300, 5, 0, 10);
 create_cloud(CLOUD_LBLUE_EXPLOSION, pickup [epick].x, pickup [epick].y - 10 * GRAIN, 0, 0, 300, 5, 0, 10);
 create_cloud(CLOUD_LBLUE_EXPLOSION, pickup [epick].x, pickup [epick].y + 10 * GRAIN, 0, 0, 300, 5, 0, 10);

 destroy_pickup(epick);
}

void destroy_pickup(int dpick)
{
 pickup[dpick].pickup_type = PICKUP_NONE;
 tell_ai_pickup_gone(dpick);
}


void manage_pickup(int mpick)
{
   pickup[mpick].y_speed += game[0].gravity;
   if (pickup[mpick].timeout > 0)
   {
    pickup[mpick].timeout --;
    if (pickup[mpick].timeout == 0)
    {
     pickup_expire(mpick);
     return;
    }
   }
//   if (pickup[mpick].y_speed < -2000)
//    pickup[mpick].y_speed = 0;
   if (pickup[mpick].y_speed != 0 || pickup[mpick].x_speed != 0)
    move_pickup(mpick, pickup[mpick].x_speed, pickup[mpick].y_speed);
   switch(pickup[mpick].pickup_type)
   {
    case PICKUP_GRAIL:
     if (prand(20) == 0)
      create_cloud(CLOUD_YELLOW_EXPLOSION, pickup[mpick].x - (6 * GRAIN) + (prand(7) * GRAIN), pickup[mpick].y - (6 * GRAIN) + (prand(7) * GRAIN), 0, 0, prand(500) + 200, 5, 15, 10);
    place_light(pickup[mpick].x, pickup[mpick].y, 50);
    break;
    case PICKUP_FLAG:
    if (team[pickup[mpick].contains].flag_at_base == 1)
    {
     pickup[mpick].x = team[pickup[mpick].contains].base_x;
     pickup[mpick].y = team[pickup[mpick].contains].base_y;
     pickup[mpick].x_speed = 0;
     pickup[mpick].y_speed = 0;
    }
    break;
   }
    
}







void move_pickup(int mpick, int move_x2, int move_y2)
{

//if (pickup [mpick].y_speed < -2000)
// pickup[mpick].y_speed = -2000;

char dir_x = 0, dir_y = 0;

int move_x = move_x2;
int move_y = move_y2;

int loop_count = 0;

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;

int i;


while (move_x != 0 || move_y != 0)
{

 if (dir_y == 10)
 {
  for (i = (pickup[mpick].x / GRAIN) - pickup[mpick].width; i < (pickup[mpick].x / GRAIN) + pickup[mpick].width; i++)
  {
   if (get_dirt(i, (pickup[mpick].y / GRAIN) + pickup[mpick].height + 1))
   {
          pickup[mpick].y_speed /= (float) -2;
          if (pickup[mpick].y_speed <= 200) pickup[mpick].y_speed = 0;
          return;
   }
  }
 }

 if (dir_y == -10)
 {
  for (i = (pickup[mpick].x / GRAIN) - pickup[mpick].width; i < (pickup[mpick].x / GRAIN) + pickup[mpick].width; i++)
  {
   if (get_dirt(i, (pickup[mpick].y / GRAIN) - pickup[mpick].height - 1))
   {
          pickup[mpick].y_speed = 0; ///= (float) -2;
//          if (pickup[mpick].y_speed <= 200) pickup[mpick].y_speed = 0;
          return;
   }
  }
 }

 if (move_y >= GRAIN)
 {
  pickup[mpick].y += GRAIN;
  move_y -= GRAIN;
 } else
 {
  if (move_y <= -GRAIN)
  {
    pickup[mpick].y -= GRAIN;
    move_y += GRAIN;
  } else
   {
    pickup[mpick].y += move_y;
    move_y = 0;
   }
 }

 

 loop_count += GRAIN;
 

} // end of while loop

 if (pickup[mpick].pickup_type == PICKUP_BASE)
 {
  team[pickup[mpick].contains].base_x = pickup[mpick].x;
  team[pickup[mpick].contains].base_y = pickup[mpick].y;
 }


}


