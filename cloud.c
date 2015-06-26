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

File: cloud.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions for clouds.
 - if you're looking for poison gas clouds, they're treated as bullets.

*/

#include "allegro.h"

#include "config.h"
#include "globvar.h"

#include "prand.h"
#include "cloud.h"
#include "light.h"


void destroy_cloud(int dcloud);
void manage_cloud(int mcloud);
void move_cloud(int mcloud, int move_x, int move_y);
int cull_clouds(void);
void slow_cloud(int mcloud, float slowed);




void init_clouds(void)
{

 int icloud;

 for (icloud = 0; icloud < MAX_CLOUDS; icloud++)
 {
  cloud[icloud].cloud_type = 0;
 }

}


int create_cloud(int cloud_type, int cloud_x, int cloud_y,
int cloud_x_speed, int cloud_y_speed, int cloud_size, int cloud_rate,
int cloud_state, int cloud_rate_change)
{

   if (cloud_x < 0 || cloud_y < 0 || cloud_x >= arena[0].max_x * GRAIN || cloud_y >= arena[0].max_y * GRAIN)
    return -1;

   int ccounter = 0;

   for (ccounter = 0; ccounter < MAX_CLOUDS; ccounter++)
   {
    if (ccounter == MAX_CLOUDS - 1)
    {
     ccounter = cull_clouds();
     break;
    }
    if (cloud[ccounter].cloud_type == 0) break;
   }

   cloud[ccounter].cloud_type = cloud_type;
   cloud[ccounter].x = cloud_x;
   cloud[ccounter].y = cloud_y;
   cloud[ccounter].x_speed = cloud_x_speed;
   cloud[ccounter].y_speed = cloud_y_speed;
   cloud[ccounter].size = cloud_size;
   cloud[ccounter].rate = cloud_rate;
   cloud[ccounter].state = cloud_state;
   cloud[ccounter].rate_change = cloud_rate_change;

   return ccounter;

}

/*
Looks through the clouds and removes the less important ones.
For when there're too many around.
*/
int cull_clouds(void)
{

 int ccloud;

 for (ccloud = 0; ccloud < MAX_CLOUDS; ccloud++)
 {
  if (cloud[ccloud].size <= 300)
  {
   destroy_cloud(ccloud);
   return ccloud;
  }
 }

 for (ccloud = 0; ccloud < MAX_CLOUDS; ccloud++)
 {
  if (cloud[ccloud].cloud_type == CLOUD_GREY_SMOKE
  || cloud[ccloud].cloud_type == CLOUD_BLACK_SMOKE)
  {
   destroy_cloud(ccloud);
   return ccloud;
  }
 }

 ccloud = prand(MAX_CLOUDS);
 destroy_cloud(ccloud);
 return ccloud;

}

void run_clouds(void)
{

 int rcloud;

 for (rcloud = 0; rcloud < MAX_CLOUDS; rcloud++)
 {
  if (cloud[rcloud].cloud_type != 0) manage_cloud(rcloud);
 }

}

void slow_cloud(int mcloud, float slowed)
{
        if (abs(cloud[mcloud].x_speed) != cloud[mcloud].x_speed)
        {
          cloud[mcloud].x_speed = abs(cloud[mcloud].x_speed);
          cloud[mcloud].x_speed *= slowed;
          cloud[mcloud].x_speed *= -1;
        }
            else
             cloud[mcloud].x_speed *= slowed;
             
        if (abs(cloud[mcloud].y_speed) != cloud[mcloud].y_speed)
        {
          cloud[mcloud].y_speed = abs(cloud[mcloud].y_speed);
          cloud[mcloud].y_speed *= slowed;
          cloud[mcloud].y_speed *= -1;
        }
            else
             cloud[mcloud].y_speed *= slowed;
}

void manage_cloud(int mcloud)
{

int bright;

switch(cloud[mcloud].cloud_type)
{
   case CLOUD_ELECTRO:
   cloud[mcloud].size -= cloud[mcloud].rate;
   if (cloud[mcloud].size < 1) destroy_cloud(mcloud);
   return;
   default:
   case CLOUD_COL_FLASH:
   case CLOUD_COL_CIRCLE:
   case CLOUD_YELLOW_FLASH:
   case CLOUD_YELLOW2_FLASH:
   case CLOUD_RED_FLASH:
   case CLOUD_GREEN_FLASH:
   case CLOUD_BLUE_FLASH:
   case CLOUD_RED_BLOOD:
   case CLOUD_CUBE:
   case CLOUD_SPAWN:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        break;
   case CLOUD_RISING_LIGHT:
        cloud[mcloud].y_speed -= game[0].gravity / 3;
   case CLOUD_LIGHT:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        bright = cloud[mcloud].state;
        if (bright != 0)
         place_light(cloud[mcloud].x, cloud[mcloud].y, bright);
          else
          {
/*            bright = 1;
           if (cloud[mcloud].size > 500)
            bright = 2;
           if (cloud[mcloud].size > 1000)
            bright = 3;
           if (cloud[mcloud].size > 1500)
            bright = 4;
           if (cloud[mcloud].size > 2000)
            bright = 5;
           if (cloud[mcloud].size > 2500)
            bright = 6;*/
           place_light(cloud[mcloud].x, cloud[mcloud].y, (cloud[mcloud].size * 3) / GRAIN);
          }
        break;
   case CLOUD_RISING_BIGLIGHT:
        cloud[mcloud].y_speed -= game[0].gravity / 3;
   case CLOUD_BIGLIGHT:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        bright = cloud[mcloud].state;
        if (bright != 0)
         place_light(cloud[mcloud].x, cloud[mcloud].y, bright);
          else
          {
           place_light(cloud[mcloud].x, cloud[mcloud].y, (cloud[mcloud].size * 8) / GRAIN);
          }
        break;
   case CLOUD_GREY_SMOKE:
   case CLOUD_BLACK_SMOKE:
   case CLOUD_COL_SMOKE:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        if (cloud[mcloud].rate >= 20)
         cloud[mcloud].rate_change = 0;

        slow_cloud(mcloud, 0.9);

/*
        
        if (abs(cloud[mcloud].x_speed) != cloud[mcloud].x_speed)
           cloud[mcloud].x_speed += 50;
            else
//            exit(0);
             cloud[mcloud].x_speed -= 50;
*/
//         cloud[mcloud].x_speed *= (float) 0.9;
//         cloud[mcloud].x_speed /= (float) 1.1;
//          else
         //          else
//          {
//           cloud[mcloud].x_speed += 1;
//          }

          
//         cloud[mcloud].y_speed *= (float) 0.9;
//         else
//         {
//          cloud[mcloud].x_speed *= -0.9;
//         }
//        cloud[mcloud].x_speed /= 10;
/*        cloud[mcloud].y_speed *= 9;
        cloud[mcloud].y_speed /= 10;*/
        cloud[mcloud].y_speed -= game[0].gravity / 6;
        break;
   case CLOUD_COL_BURST:
   case CLOUD_RED_EXPLOSION:
   case CLOUD_BLUE_EXPLOSION:
        cloud[mcloud].y_speed -= game[0].gravity / 3;
//        slow_cloud(mcloud, 0.9);
   case CLOUD_LBLUE_EXPLOSION:
   case CLOUD_LGREEN_EXPLOSION:
   case CLOUD_GREEN_EXPLOSION:
   case CLOUD_COL_EXPLOSION:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        break;
   case CLOUD_ORANGE_EXPLOSION:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        cloud[mcloud].y_speed -= game[0].gravity / 3;
//        slow_cloud(mcloud, 0.9);
        break;
   case CLOUD_YELLOW_EXPLOSION:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        cloud[mcloud].y_speed -= game[0].gravity / 3;
//        slow_cloud(mcloud, 0.9);
        break;
   case CLOUD_DEBUG:
   case CLOUD_NUMBER:
        cloud[mcloud].size -= cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        break;
   case CLOUD_GREEN_ELLIPSE:
   case CLOUD_LGREEN_ELLIPSE:
        cloud[mcloud].size -= cloud[mcloud].rate;
//        if (cloud[mcloud].size <= 1000) cloud[mcloud].state -= cloud[mcloud].rate;
        cloud[mcloud].state += cloud[mcloud].rate;
        cloud[mcloud].rate += cloud[mcloud].rate_change;
        break;

}
   move_cloud(mcloud, cloud[mcloud].x_speed, cloud[mcloud].y_speed);
   if (cloud[mcloud].size <= 1) destroy_cloud(mcloud);
   // note: CLOUD_ELECTRO doesn't get this far.
}


void move_cloud(int mcloud, int move_x, int move_y)
{

//char dir_x = 0, dir_y = 0;

//int loop_count = 0;
//char actor_count = 0;


cloud[mcloud].x += move_x;
cloud[mcloud].y += move_y;

}


void destroy_cloud(int dcloud)
{

   cloud[dcloud].cloud_type = 0;

}


