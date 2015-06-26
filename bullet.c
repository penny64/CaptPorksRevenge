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

File: bullet.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions dealing with bullet objects - initialisation, movement,
   destruction etc.
 - the 'hurt_actor' and 'actor_die' functions.
 
*/

#include "allegro.h"
//#include "libnet.h"

#include <math.h>
#include <string.h>

#include "config.h"
#include "globvar.h"

#include "ai.h"
//#include "async.h"
#include "cloud.h"
#include "display.h"
#include "prand.h"
#include "sound.h"
#include "grid.h"
#include "move.h"
#include "score.h"
#include "effects.h"
#include "actor.h"
#include "pickup.h"

#include "light.h"

#define PREDRAWN_DIRT_CIRCLES 14
#define BLOOD_RED COLOUR_RED3

#ifndef PI
#define PI 3.14159265358979323846
#endif
// blood_red also defined in display.c

void destroy_bullet(int dbull);
void manage_bullet(int mbull);
int move_bullet(int mbull, int move_x2, int move_y2, char limited);

char bullet_impact(int ibull, char dir_x, char dir_y, int hit_what);

void place_explosion(int exp_x, int exp_y, int sx, int sy, int size, int mini_clouds);
void place_rocket_trail(int exp_x, int exp_y, int sp_x, int sp_y, int size, char accel, int rbull);
void bullet_bounce(int bbull, char bounce_x, char bounce_y);
void blast(int x_pos, int y_pos, int radius, int owner, int damage, int force, int dirt_dam, int shake, char tracer);
int blast_tracer(int x1, int y1, int x2, int y2);
void shrapnel_burst(int sbull, int amount, int damage, int range, int shrapnel_type);
void slam_actor(int sactor, int x_slam, int y_slam, int weight);
void play_bsound(int splay);
int hurt_actor(int hactor, int hurter, int harm, int shielded, int force, int async);
void actor_die(int dactor, int cause);
void destroy_grapple(int dbull);
void hurt_grid(int grx, int gry, int dir_x, int dir_y, int force);
void damage_grid(int grx, int gry, int force);
void check_grapples(int grx, int gry);
char actor_collision(int mbull);
void burn_dirt(int bx, int by, int brad);
void turn_tracker(int tbull, int tx, int ty, float turn_speed);
void turn_directly(int tbull, int tactor);
int hunter_acquire(int hx, int hy);
void blast_dirt(int bx, int by, int brad, char digging);
void dirt_splodge(int bx, int by, int brad, int colour);
void slow_bullet(int sbull, float slowed);
int bullet_rand(int rbull, int max);

int create_bullet(int bullet_type, int bullet_x, int bullet_y,
 int bullet_x_speed, int bullet_y_speed, unsigned char bullet_owner,
 int bullet_damage, int bullet_timer, int is_bullet, int bweight, float bangle,
 int facing, int status, unsigned char seed);

void blood_burst(int bactor, int blood_amount, int flesh_amount,
 int bx, int by, int bspeed_x, int bspeed_y);
void sparky(int x, int y, int xs, int ys, int scol, int amount);

void splatter(int sx, int sy, int amount, int colour);
char *user_name_plus(int ui);

int random_colour(void);

extern BITMAP *dirt_bmp;

BITMAP *circle_bmp [PREDRAWN_DIRT_CIRCLES];
BITMAP *circle_bmp_misc;

int team_colours(int which_team);


int team_colours(int which_team)
{
 switch(which_team)
 {
  default:
  case TEAM_NONE: return COLOUR_GREY8;//TEAM_NONE_COLOUR;
  case TEAM_RED: return COLOUR_RED4;//TEAM_RED_COLOUR;
  case TEAM_BLUE: return COLOUR_BLUE4;//TEAM_BLUE_COLOUR;
  case TEAM_GREEN: return COLOUR_GREEN4;//TEAM_GREEN_COLOUR;
  case TEAM_GOLD: return COLOUR_YELLOW4;//TEAM_GOLD_COLOUR;
  case TEAM_GREY: return COLOUR_GREY4;//TEAM_PURPLE_COLOUR;
  case TEAM_BROWN: return COLOUR_BROWN4;//TEAM_BROWN_COLOUR;
 }

 return TEAM_NONE_COLOUR;
}

void init_bullets(void)
{

 int ibull;

 for (ibull = 0; ibull < MAX_BULLETS; ibull++)
 {
  bullet[ibull].bullet_type = 0;
 }

 int circ;

 for (circ = 0; circ < PREDRAWN_DIRT_CIRCLES; circ ++)
 {
  if (circle_bmp [circ] != NULL)
   destroy_bitmap(circle_bmp [circ]);
  circle_bmp [circ] = create_bitmap(circ * 2 + 3, circ * 2 + 3);
  clear_bitmap(circle_bmp [circ]);
  circlefill(circle_bmp [circ], circ + 1, circ + 1, circ, 1);
 }

 if (circle_bmp_misc != NULL)
  destroy_bitmap(circle_bmp_misc);
 circle_bmp_misc = create_bitmap(70, 70);

 clear_bitmap(circle_bmp_misc);

}


int create_bullet(int bullet_type, int bullet_x, int bullet_y,
int bullet_x_speed, int bullet_y_speed, unsigned char bullet_owner,
int bullet_damage, int bullet_timer, int is_bullet, int bweight, float bangle,
int facing, int status, unsigned char seed)
{

   int bcounter = 0;

   for (bcounter = 1; bcounter < MAX_BULLETS; bcounter++)
   {
    if ((bullet_type == BULLET_SPARK || bullet_type == BULLET_SPECK)
         && bcounter > MAX_BULLETS / 2)
          return -1; // these bullets just for decoration
    if (bcounter == MAX_BULLETS - 1) return -1;
    if (bullet[bcounter].bullet_type == 0) break;
   }

   if (bullet_type == BULLET_GRAPPLE)
   {
    if (actor[bullet_owner].grapple_bullet != -1)
    {
      destroy_bullet(actor[bullet_owner].grapple_bullet);
    }
    actor[bullet_owner].grapple_x = bullet_x;
    actor[bullet_owner].grapple_y = bullet_y;
    actor[bullet_owner].grapple_anchored = 0;
    actor[bullet_owner].grapple_bullet = bcounter;
   }

   bullet[bcounter].bullet_type = bullet_type;
   bullet[bcounter].x = bullet_x;
   bullet[bcounter].y = bullet_y;
   bullet[bcounter].x_speed = bullet_x_speed;
   bullet[bcounter].y_speed = bullet_y_speed;
   bullet[bcounter].owner = bullet_owner;
   bullet[bcounter].seed = seed;
   bullet[bcounter].fuse = bullet_timer;
   bullet[bcounter].status = status; // must be before switch (for paintball)
   switch(bullet[bcounter].bullet_type)
   {
    case BULLET_GRAPPLE:
    bullet[bcounter].width = 0;
    bullet[bcounter].height = 0;
    // so it looks like it's lodged in wall.
    break;
    case BULLET_NAPALM:
    bullet[bcounter].width = 1;
    bullet[bcounter].height = 1;
    break;
    case BULLET_SQUIRM:
    bullet[bcounter].fuse = 90;
    bullet[bcounter].status2 = bullet_timer;
    break;
    case BULLET_GRAPESHOT:
    case BULLET_FAT_BULLET:
    case BULLET_BULLET:
    case BULLET_SHRAPNEL:
    case BULLET_AUTOCANNON:
    case BULLET_FLAK:
    if (game[0].bullets_explode == BULLETS_PAINTBALL)
     bullet[bcounter].status = random_colour();
      else
       bullet[bcounter].status = COLOUR_GREY8;
     default:
     bullet[bcounter].width = 1;
     bullet[bcounter].height = 1;
     break;
    case BULLET_GREN_SHRAPNEL:
    if (game[0].bullets_explode == BULLETS_PAINTBALL)
     bullet[bcounter].status = random_colour();
      else
       bullet[bcounter].status = COLOUR_GREEN4;
     bullet[bcounter].width = 1;
     bullet[bcounter].height = 1;
     break;
   }
   bullet[bcounter].damage = bullet_damage;
   bullet[bcounter].left_owner = 0;
   bullet[bcounter].time_since_cloud = 0;
   if (bullet_timer == 0) bullet[bcounter].fuse = 1000;
   bullet[bcounter].weight = bweight;
   bullet[bcounter].angle = bangle;
   bullet[bcounter].base_angle = (bangle + (PI / 2)) * (128 / PI);
//   bullet[bcounter].base_angle = (bangle) * (256 / PI);

//   actor[k].angle = ((float) actor[k].base_angle / 256 * PI) - (PI / 2);

   bullet[bcounter].facing = facing;
/*   if (bullet_type == BULLET_TRACKER)
   {
    bullet[bcounter].status = actor[bullet_owner].lock_on;
   } else*/
   bullet[bcounter].is_bullet = is_bullet;
//   if (bangle) bullet[bcounter].facing = -1;
//   bullet[bcounter].angle *= -1;

   return bcounter;

}

int random_colour(void)
{
 switch(prand(7))
 {
  case 0:
   return COLOUR_GREY4;
  case 1:
   return COLOUR_GREY8;
  case 2:
   return COLOUR_RED4;
  case 3:
   return COLOUR_BLUE4;
  case 4:
   return COLOUR_GREEN4;
  case 5:
   return COLOUR_YELLOW4;
  case 6:
   return COLOUR_BROWN4;
 }

 return COLOUR_GREY4;
}

void run_bullets(void)
{

 int rbull;

 for (rbull = 1; rbull < MAX_BULLETS; rbull++)
 {
  if (bullet[rbull].bullet_type != 0) manage_bullet(rbull);
 }

}


void manage_bullet(int mbull)
{

   int x_gain = 0;
   int y_gain = 0;
   char angle_change;

   bullet[mbull].time_since_cloud ++;
   if (bullet[mbull].left_owner < 100)
    bullet[mbull].left_owner ++;
   if (bullet[mbull].fuse > 0)
   {
    bullet[mbull].fuse --;
    if (bullet[mbull].fuse <= 0)
    {
     bullet_impact(mbull, 0, 0, -1);
     return;
    }
   }

   switch(bullet[mbull].bullet_type)
   {
    // case BULLET_BULLET: nothing special
   
    case BULLET_GRAPPLE:
    if (actor[bullet[mbull].owner].grapple_bullet != mbull)
    {
         destroy_bullet(mbull);
         return;
    }
    actor[bullet[mbull].owner].grapple_x = bullet[mbull].x;
    actor[bullet[mbull].owner].grapple_y = bullet[mbull].y;
    if (actor[bullet[mbull].owner].grapple_anchored == 1)
    {
     if (get_dirt(bullet[mbull].x / GRAIN, bullet[mbull].y / GRAIN) == 0)
     {
      actor[bullet[mbull].owner].grapple_anchored = 0;
//      exit(5);
     }
      else
       return;
    }
    bullet[mbull].y_speed += game[0].gravity;
    bullet[mbull].angle += 1;
    if (bullet[mbull].angle >= PI * 2 || bullet[mbull].angle <= PI * -2) bullet[mbull].angle = 0;
    break;
   
    case BULLET_GREN_LAUNCHER:
    bullet[mbull].y_speed += game[0].gravity;
//    slow_bullet(mbull, 0.995);
    if (prand(4) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(800), 20, 0, 0);
    }
    break;
    case BULLET_BOMB:
    bullet[mbull].y_speed += game[0].gravity;
    if (prand(25) > bullet[mbull].fuse)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(800), 20, 0, 0);
    }
    break;
    case BULLET_FIREBOMB:
    bullet[mbull].y_speed += game[0].gravity;
    if (prand(4) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(800), 10, 0, 0);
    } else
     if (prand(4) < bullet[mbull].time_since_cloud)
     {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_RED_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(800), 10, 0, 10);
     }

    break;
    case BULLET_NAPALM:
    bullet[mbull].y_speed += game[0].gravity / 2;
//    if (bullet[mbull].y_speed == 0) bullet[mbull].y -= 200;
//    if (bullet[mbull].y_speed == 0) bullet[mbull].y_speed = -200;
    if (prand(45) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(800), 10, 0, 0);
        if (prand(3) == 0)
         create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed - 80, prand(800), 10, 0, 20);
          else
           create_cloud(CLOUD_ORANGE_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed - 80, prand(800), 10, 0, 20);
        create_cloud(CLOUD_RED_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed - 80, prand(800), 10, 0, 10);
//        if (prand(5) == 0)
//         create_cloud(CLOUD_ORANGE_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(800), 20, 0);
    }
//    if (bullet[mbull].fuse > 50 && bullet[mbull].fuse % 5 == 0) bullet[mbull].height ++;
//     else if (bullet[mbull].fuse % 10 == 0) bullet[mbull].height --;
    if (bullet[mbull].left_owner > 3)
      blast(bullet[mbull].x, bullet[mbull].y, 2000, bullet[mbull].owner, 10, 0, 0, 0, 1);
//void blast(int x_pos, int y_pos, int radius, int owner, int damage, int force, int dirt_dam, int shake)
    break;
    
    case BULLET_FLAME:
    bullet[mbull].y_speed -= game[0].gravity / 3;
    slow_bullet(mbull, 0.98);
    if (prand(45) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(800), 10, 0, 0);
    }
    create_cloud(CLOUD_RED_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 100 + prand(400) + (400 - bullet[mbull].fuse * 19), 10, 0, 20);
    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 100 + prand(400) + (400 - bullet[mbull].fuse * 19), 10, 0, 20);
    if (bullet[mbull].fuse > 10) bullet[mbull].status ++;
     else if (bullet[mbull].fuse % 2 == 0)
     {
      bullet[mbull].status --;
      bullet[mbull].x_speed += bullet_rand(mbull, 200) - bullet_rand(mbull, 200);
      bullet[mbull].y_speed += bullet_rand(mbull, 200) - bullet_rand(mbull, 200);
     }
    break;
    case BULLET_ELECTRO:
//    if (arena[0].counter % 25 != 0)
//     break;
    // okay to use prand, not bullet_rand, 'cos this is cosmetic.
//    x_gain = (prand(40) - 19) * GRAIN;
//    y_gain = (prand(40) - 19) * GRAIN;
//    create_cloud(CLOUD_ELECTRO,
//     bullet[mbull].x + x_gain, //bullet[mbull].status,
////     bullet[mbull].y + y_gain, //bullet[mbull].status2,
//     bullet[mbull].x + bullet[mbull].x_speed + bullet[mbull].status,
//     bullet[mbull].y + bullet[mbull].y_speed + bullet[mbull].status2,
//     15, 1, 0, 0);
/*    create_cloud(CLOUD_ELECTRO,
//   put_x += ((bullet[dbull].seed * 3) % 40 - 19) * GRAIN;
//   put_y += ((bullet[dbull].seed * 7) % 40 - 19) * GRAIN;
     bullet[mbull].x + ((bullet[mbull].seed * 3) % 40 - 19) * GRAIN,
     bullet[mbull].y + ((bullet[mbull].seed * 7) % 40 - 19) * GRAIN,
     ((bullet[mbull].status * 3) % 40 - 19) * GRAIN - bullet[mbull].x_speed,
     ((bullet[mbull].status * 7) % 40 - 19) * GRAIN - bullet[mbull].y_speed,
     15, 1, 0, 0);*/


     bullet[mbull].status = bullet[mbull].x;// - bullet[mbull].x_speed;
     bullet[mbull].status2 = bullet[mbull].y;// - bullet[mbull].y_speed;
     if (bullet[mbull].left_owner > 2)
     {
      bullet[mbull].x += (bullet_rand(mbull, 20) - 9) * GRAIN; // needs to be * GRAIN
      bullet[mbull].y += (bullet_rand(mbull, 20) - 9) * GRAIN;
    create_cloud(CLOUD_ELECTRO,
     bullet[mbull].x, //bullet[mbull].status,
     bullet[mbull].y, //bullet[mbull].status2,
     bullet[mbull].status - bullet[mbull].x_speed,
     bullet[mbull].status2 - bullet[mbull].y_speed,
     9, 1, 0, 0);
    create_cloud(CLOUD_ELECTRO,
     bullet[mbull].x + ((arena[0].counter * 3) % 40 - 19) * GRAIN,
     bullet[mbull].y + ((arena[0].counter * 7) % 40 - 19) * GRAIN,
     bullet[mbull].status - bullet[mbull].x_speed + (((arena[0].counter - 1) * 3) % 40 - 19) * GRAIN,
     bullet[mbull].status2 - bullet[mbull].y_speed + (((arena[0].counter - 1) * 7) % 40 - 19) * GRAIN,
     9, 1, 0, 0);
     }
     if (bullet[mbull].left_owner == 2)
     {
      bullet[mbull].x += (bullet_rand(mbull, 20) - 9) * GRAIN; // needs to be * GRAIN
      bullet[mbull].y += (bullet_rand(mbull, 20) - 9) * GRAIN;
    create_cloud(CLOUD_ELECTRO,
     bullet[mbull].x, //bullet[mbull].status,
     bullet[mbull].y, //bullet[mbull].status2,
     bullet[mbull].status - bullet[mbull].x_speed,
     bullet[mbull].status2 - bullet[mbull].y_speed,
     9, 1, 0, 0);
    create_cloud(CLOUD_ELECTRO,
     bullet[mbull].x + ((arena[0].counter * 3) % 40 - 19) * GRAIN,
     bullet[mbull].y + ((arena[0].counter * 7) % 40 - 19) * GRAIN,
     bullet[mbull].status - bullet[mbull].x_speed,
     bullet[mbull].status2 - bullet[mbull].y_speed,
     9, 1, 0, 0);
     }
     bullet[mbull].status = bullet[mbull].x;// - bullet[mbull].x_speed;
     bullet[mbull].status2 = bullet[mbull].y;// - bullet[mbull].y_speed;
     
/*
     bullet[mbull].status = bullet[mbull].x;// - bullet[mbull].x_speed;
     bullet[mbull].status2 = bullet[mbull].y;// - bullet[mbull].y_speed;
     if (bullet[mbull].left_owner > 0)
     {
      bullet[mbull].x += (bullet_rand(mbull, 20) - 9) * GRAIN; // needs to be * GRAIN
      bullet[mbull].y += (bullet_rand(mbull, 20) - 9) * GRAIN;
     }
    create_cloud(CLOUD_ELECTRO,
     bullet[mbull].x + bullet[mbull].x_speed, //bullet[mbull].status,
     bullet[mbull].y + bullet[mbull].y_speed, //bullet[mbull].status2,
     bullet[mbull].status,
     bullet[mbull].status2,
     5, 1, 0, 0);
    create_cloud(CLOUD_ELECTRO,
     bullet[mbull].x + bullet[mbull].x_speed + ((arena[0].counter * 7) % 40 - 19) * GRAIN,
     bullet[mbull].y + bullet[mbull].y_speed + ((arena[0].counter * 7) % 40 - 19) * GRAIN,
     bullet[mbull].status + (((arena[0].counter - 1) * 7) % 40 - 19) * GRAIN,
     bullet[mbull].status2 + (((arena[0].counter - 1) * 7) % 40 - 19) * GRAIN,
     5, 1, 0, 0);
*/


     
//    bullet[mbull].status = bullet[mbull].seed;
//    bullet[mbull].seed = bullet_rand(mbull, 65535);
    
//    sparky(bullet[mbull].x, bullet[mbull].y, 0, 0, COLOUR_GREY8, 1 + prand(3));
//    bullet[dbull].status = bullet[dbull].x + x_gain;
//    bullet[dbull].status2 = bullet[dbull].y + y_gain;
    break;
    case BULLET_NUKE_EXP:
    if (bullet_rand(mbull, 3) < bullet[mbull].time_since_cloud)
    {
     if (bullet_rand(mbull, 5) == 0)
     {
        play_sound_pos(WAV_NUKE2, 1000, 250, bullet[mbull].x, bullet[mbull].y);
        blast(bullet[mbull].x, bullet[mbull].y, 5000, bullet[mbull].owner, 3000, 30, 20 + bullet_rand(mbull, 20), 10, 1);
     }
        bullet[mbull].time_since_cloud = 0;
        if (prand(2) == 0)
//         create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, 2000 + (5 * prand(200 - bullet[mbull].fuse)), 5, 0);
//          else
           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, 1000 + (5 * prand(200 - bullet[mbull].fuse)), 0, mbull);
    }
    break;
    case BULLET_RPG:
//    bullet[mbull].angle += (float) bullet_rand(mbull, 20) / 100;
//    bullet[mbull].angle -= 0.10;
    angle_change = bullet_rand(mbull, 20);
    angle_change -= 10;
//    bullet[mbull].base_angle += bullet_rand(mbull, 30);
//    bullet[mbull].base_angle -= 15;
    if (angle_change + bullet[mbull].base_angle < 0)
    {
     bullet[mbull].facing *= -1;
     bullet[mbull].base_angle = 0;
    }
     else
     {
      if (angle_change + bullet[mbull].base_angle > 255)
      {
       bullet[mbull].facing *= -1;
       bullet[mbull].base_angle = 255;
      }
       else bullet[mbull].base_angle += angle_change;
     }
    bullet[mbull].angle = ((float) bullet[mbull].base_angle / 128 * PI) - (PI / 2);
    case BULLET_LR_ROCKET:
    case BULLET_NUKE_M:
    x_gain = cos(bullet[mbull].angle) * 10 + (30 * bullet[mbull].bullet_type == BULLET_LR_ROCKET);
    y_gain = sin(bullet[mbull].angle) * 10 + (30 * bullet[mbull].bullet_type == BULLET_LR_ROCKET);
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed += x_gain;
    bullet[mbull].y_speed += y_gain;
    bullet[mbull].time_since_cloud = 0;
    place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), x_gain, y_gain, 400 + prand(400), 30, mbull);
    break;
    case BULLET_ROCKET:
    bullet[mbull].time_since_cloud = 0;
    place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 400 + prand(400), 0, mbull);
    break;
    case BULLET_SEEKER:
    if (bullet[mbull].status != -1)
     turn_tracker(mbull, actor[bullet[mbull].status].x, actor[bullet[mbull].status].y, 0.08);
    slow_bullet(mbull, 0.96);
    x_gain = cos(bullet[mbull].angle) * 30;// + (30 * bullet[mbull].bullet_type == BULLET_LR_ROCKET);
    y_gain = sin(bullet[mbull].angle) * 30;// + (30 * bullet[mbull].bullet_type == BULLET_LR_ROCKET);
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed += x_gain;
    bullet[mbull].y_speed += y_gain;
    // intentional fall-through
    bullet[mbull].time_since_cloud = 0;
    place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), x_gain, y_gain, 400 + prand(400), 20, mbull);
    case BULLET_REMOTE_ROCKET:
    slow_bullet(mbull, 0.98);
    x_gain = cos(bullet[mbull].angle) * 20;// + (30 * bullet[mbull].bullet_type == BULLET_LR_ROCKET);
    y_gain = sin(bullet[mbull].angle) * 20;// + (30 * bullet[mbull].bullet_type == BULLET_LR_ROCKET);
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed += x_gain;
    bullet[mbull].y_speed += y_gain;
    // intentional fall-through
//    if (prand(2) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
//        if (prand(3) != 0)
//         create_cloud(CLOUD_GREY_SMOKE, GRAIN * bullet[mbull].x - (GRAIN * 5) + ((prand(10))), GRAIN * bullet[mbull].y - (GRAIN * 5) + ((prand(10))), 0, 0, prand(800), 20, 0);
//          else
//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 400 + prand(400));
           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), x_gain, y_gain, 400 + prand(400), 20, mbull);

//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5), bullet[mbull].y - (GRAIN * 5), bullet[mbull].x_speed, bullet[mbull].y_speed, 400 + prand(400));
    }
//         create_cloud(CLOUD_GREY_SMOKE, GRAIN * bullet[mbull].x - (GRAIN * 5) + ((prand(10))), GRAIN * bullet[mbull].y - (GRAIN * 5) + ((prand(10))), 0, 0, prand(800), 20, 0);
    break;
//    break;
//    note: atan(sin(x) / cos(x)) = x;
//    case BULLET_HUNTER:
    case BULLET_TRACKER:
    if (bullet[mbull].status != -1)
     turn_tracker(mbull, actor[bullet[mbull].status].x, actor[bullet[mbull].status].y, 0.05);
    case BULLET_REMOTE_ROCKET_C:
    x_gain = cos(bullet[mbull].angle) * 300;
    y_gain = sin(bullet[mbull].angle) * 300;
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed = x_gain;
    bullet[mbull].y_speed = y_gain;
//    if (prand(8) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
//        if (prand(3) != 0)
           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 400 + prand(400), 0, mbull);
//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5), bullet[mbull].y - (GRAIN * 5), bullet[mbull].x_speed, bullet[mbull].y_speed, 400 + prand(400));
    }
    break;
    case BULLET_BOUNCY:
    if (bullet[mbull].status != -1)
    {
     turn_directly(mbull, bullet[mbull].status);
     x_gain = cos(bullet[mbull].angle) * 30;
     y_gain = sin(bullet[mbull].angle) * 30;
     if (bullet[mbull].facing == -1) x_gain *= -1;
     bullet[mbull].x_speed += x_gain;
     bullet[mbull].y_speed += y_gain;
    }
    bullet[mbull].y_speed += game[0].gravity;
    if (prand(6) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(800), 20, 0, 0);
    }
    break;
    case BULLET_SWARM:
    x_gain = cos(bullet[mbull].angle) * 40;
    y_gain = sin(bullet[mbull].angle) * 40;
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed += x_gain;
    bullet[mbull].y_speed += y_gain;
//    if (prand(3) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
//        if (prand(2) == 0)
//         create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(300), 20, 0);
//          else

//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 400);
           place_rocket_trail(bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), x_gain, y_gain, 350 + prand(200), 15, mbull);

//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 200 + prand(200));
    }
    break;
    case BULLET_SQUIRM:
//    if (bullet_rand(mbull, 3) != 0)
//    {
//     turn_tracker(mbull, bullet[mbull].status, bullet[mbull].status2, 0.08);
//    } else
    {
     angle_change = bullet_rand(mbull, 21);
     angle_change -= 10;
     if (angle_change + bullet[mbull].base_angle < 0)
     {
      bullet[mbull].facing *= -1;
      bullet[mbull].base_angle = 0;
     }
      else
      {
       if (angle_change + bullet[mbull].base_angle > 255)
       {
        bullet[mbull].facing *= -1;
        bullet[mbull].base_angle = 255;
       }
        else bullet[mbull].base_angle += angle_change;
      }
    }
    bullet[mbull].angle = ((float) bullet[mbull].base_angle / 128 * PI) - (PI / 2);
    x_gain = cos(bullet[mbull].angle) * 100;
    y_gain = sin(bullet[mbull].angle) * 100;
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed += x_gain;
    bullet[mbull].y_speed += y_gain;
    if (prand(3) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
//        if (prand(2) == 0)
//         create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(300), 20, 0);
//          else

//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 400);
           place_rocket_trail(bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), x_gain, y_gain, 300 + prand(120), 5, mbull);

//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, 200 + prand(200));
    }
    slow_bullet(mbull, 0.93);
    break;
    case BULLET_PLAS_R:
//    bullet[mbull].y_speed += game[0].gravity;
//    if (prand(3) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
//        if (prand(2) == 0)
//         create_cloud(CLOUD_BLUE_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(400), 10, 0, 5);
//         create_cloud(CLOUD_LBLUE_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].x_speed, bullet[mbull].y_speed, prand(400), 20, 0, 5);
         create_cloud(CLOUD_BLUE_EXPLOSION, bullet[mbull].x - (GRAIN * 4) + ((prand(8)) * GRAIN), bullet[mbull].y - (GRAIN * 4) + ((prand(8)) * GRAIN), 0, 0, prand(400), 10, 0, 5);
         create_cloud(CLOUD_LBLUE_EXPLOSION, bullet[mbull].x - (GRAIN * 4) + ((prand(8)) * GRAIN), bullet[mbull].y - (GRAIN * 4) + ((prand(8)) * GRAIN), 0, 0, prand(400), 20, 0, 5);
//          else
//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 400 + prand(400));
    }
    break;
    case BULLET_CUBE:
//    if (prand(3) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
         create_cloud(CLOUD_CUBE, bullet[mbull].x, bullet[mbull].y, 0, 0, prand(400), 10, bullet[mbull].status, 5);
    }
    break;
    case BULLET_NUKE_PLAS:
    bullet[mbull].y_speed += game[0].gravity;
    if (prand(4) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_GREY_SMOKE, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(1500), 20, 0, 0);
        create_cloud(CLOUD_RED_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, prand(500), 10, 0, 10);
        create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, prand(500), 20, 0, 30);
    }
    break;
    case BULLET_FIREBALL:
    bullet[mbull].y_speed += game[0].gravity;
    if (prand(50) > bullet[mbull].fuse)
     break;
    case BULLET_PLAS_C:
//    bullet[mbull].y_speed += game[0].gravity;
//    if (prand(4) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
//        if (prand(2) == 0)
/*         create_cloud(CLOUD_RED_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(500), 10, 0);
         create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(500), 20, 0);*/
         create_cloud(CLOUD_RED_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, prand(500), 10, 0, 10);
         create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, prand(500), 10, 0, 30);
//          else
//           place_rocket_trail(bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 400 + prand(400));
    }
    break;
    case BULLET_PLINKER:
//    if (prand(4) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
         create_cloud(CLOUD_COL_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, prand(500), 10, TRANS_BLUE, 10);
//         create_cloud(CLOUD_COL_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, prand(500), 20, 186, 10);
    }
    break;
    case BULLET_RECTIFIER:
    break;
    default:
    if (bullet[mbull].is_bullet && game[0].fast_bullets == 0)
    {
      bullet[mbull].y_speed += game[0].gravity / 5;
    } else
           bullet[mbull].y_speed += game[0].gravity;
    break;
    case BULLET_SMITELET:
    x_gain = 100 + bullet[mbull].fuse * 60;
       bullet[mbull].time_since_cloud = 0;
         create_cloud(CLOUD_RED_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, x_gain + prand(x_gain), 10, 0, 10);
         create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, x_gain + prand(x_gain), 10, 0, 30);
    break;
    case BULLET_SMITER:
    case BULLET_LASER_PULSE:
    case BULLET_LASER_BEAM:
    case BULLET_LASER_TRACER:
    move_bullet(mbull, bullet[mbull].x_speed, bullet[mbull].y_speed, 0);
    return;
    case BULLET_FUNKY_BOMB:
    bullet[mbull].y_speed += game[0].gravity;
    if (prand(3) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_COL_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(800), 10, trans_colours_array [prand(5)], 10);
    }
    break;
    case BULLET_FUNKY_BOMBLET:
    bullet[mbull].y_speed += game[0].gravity;
    if (prand(15) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        create_cloud(CLOUD_COL_EXPLOSION, bullet[mbull].x - (GRAIN * 5) + ((prand(10)) * GRAIN), bullet[mbull].y - (GRAIN * 5) + ((prand(10)) * GRAIN), 0, 0, prand(800), 10, trans_colours_array [prand(5)], 10);
    }
    break;
    case BULLET_HUNTER_BOMB:
    break;
    case BULLET_HUNTER:
    if (prand(300) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
        bullet[mbull].status = hunter_acquire(bullet[mbull].x, bullet[mbull].y);
        play_sound_pos(WAV_HUNTER, 1000, 250, bullet[mbull].x, bullet[mbull].y);
    }
    if (bullet[mbull].status != -1)
     turn_tracker(mbull, actor[bullet[mbull].status].x, actor[bullet[mbull].status].y, 0.1);
//    bullet[mbull].angle += (float) prand(14) / 20;
//    bullet[mbull].angle -= 0.35;
    angle_change = bullet_rand(mbull, 30);
    angle_change -= 15;
//    bullet[mbull].base_angle += bullet_rand(mbull, 30);
//    bullet[mbull].base_angle -= 15;
    if (angle_change + bullet[mbull].base_angle < 0)
    {
     bullet[mbull].facing *= -1;
     bullet[mbull].base_angle = 0;
    }
     else
     {
      if (angle_change + bullet[mbull].base_angle > 255)
      {
       bullet[mbull].facing *= -1;
       bullet[mbull].base_angle = 255;
      }
       else bullet[mbull].base_angle += angle_change;
     }
/*    bullet[mbull].base_angle += prand(30); //bullet_rand(mbull, 30);
// don't need to use bullet_rand as this weapon disabled in netgames
    bullet[mbull].base_angle -= 15;*/
    bullet[mbull].angle = ((float) bullet[mbull].base_angle / 128 * PI) - (PI / 2);
    x_gain = cos(bullet[mbull].angle) * 300;
    y_gain = sin(bullet[mbull].angle) * 300;
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed = x_gain;
    bullet[mbull].y_speed = y_gain;
/*    if (actor[bullet[mbull].owner].x < bullet[mbull].x) bullet[mbull].x_speed = -100;
     else bullet[mbull].x_speed = 100;
    if (actor[bullet[mbull].owner].y < bullet[mbull].y) bullet[mbull].y_speed = -100;
     else bullet[mbull].y_speed = 100;*/
    break;
    case BULLET_DISRUPTER_BANG:
    case BULLET_DISRUPTER:
    break;
    case BULLET_DISRUPTER_WAVE:
    if (bullet[mbull].x < 0 || bullet[mbull].x >= arena[0].max_x * GRAIN
     || bullet[mbull].y < 0 || bullet[mbull].y >= arena[0].max_y * GRAIN)
     {
             destroy_bullet(mbull);
             return;
     }
    if (bullet[mbull].left_owner > 3)
      blast(bullet[mbull].x, bullet[mbull].y, 4000, bullet[mbull].owner, 100, 0, 0, 0, 1);
    break;
    case BULLET_SPLINTER:
    bullet[mbull].y_speed += game[0].gravity;
    break;
    case BULLET_BLOOD:
    bullet[mbull].y_speed += game[0].gravity / 3;
    break;
    case BULLET_SPARK:
    bullet[mbull].y_speed += game[0].gravity;
    if (bullet[mbull].fuse < 15 && arena[0].counter % 6 == 0) // % 3 == 0)
     bullet[mbull].status --;
    break;
    case BULLET_FLESH:
    bullet[mbull].y_speed += game[0].gravity / 3;
    if (prand(10) == 0)
    {
     create_bullet(BULLET_BLOOD, bullet[mbull].x, bullet[mbull].y,
      bullet[mbull].x_speed + prand(300) - 150, bullet[mbull].y_speed + prand(300) - 300,
      bullet[mbull].owner, -1, 0, 0, 10, 0, 0, BLOOD_RED, 0);
    }
    break;
    case BULLET_TOXIN:
    break;
    case BULLET_SHREDDER:
    bullet[mbull].angle += bullet[mbull].facing;
    if (bullet[mbull].angle >= PI * 2) bullet[mbull].angle = 0;//PI * -2;
    if (bullet[mbull].angle <= PI * -2) bullet[mbull].angle = 0;//PI * 2;
    bullet[mbull].y_speed += game[0].gravity;
    slow_bullet(mbull, 0.995);
    break;
    case BULLET_PRONG:
    bullet[mbull].status += bullet_rand(mbull, 10);
    bullet[mbull].status -= 5;
    bullet[mbull].angle += (float) bullet[mbull].status / 5;
//    bullet[mbull].angle -= 0.30;
    x_gain = cos(bullet[mbull].angle) * 70;
    y_gain = sin(bullet[mbull].angle) * 70;
    if (bullet[mbull].facing == -1) x_gain *= -1;
    bullet[mbull].x_speed += x_gain;
    bullet[mbull].y_speed += y_gain;
    break;
    case BULLET_GAS:
        if (abs(bullet[mbull].x_speed) != bullet[mbull].x_speed)
        {
          bullet[mbull].x_speed = abs(bullet[mbull].x_speed);
          bullet[mbull].x_speed *= 0.95;
          bullet[mbull].x_speed *= -1;
        }
            else
             bullet[mbull].x_speed *= 0.95;
             
        if (abs(bullet[mbull].y_speed) != bullet[mbull].y_speed)
        {
          bullet[mbull].y_speed = abs(bullet[mbull].y_speed);
          bullet[mbull].y_speed *= 0.95;
          bullet[mbull].y_speed *= -1;
        }
            else
             bullet[mbull].y_speed *= 0.95;
        bullet[mbull].y_speed -= game[0].gravity / 12;
        if (bullet[mbull].fuse > 10)
        {
         if (bullet[mbull].fuse < 150) bullet[mbull].fuse -= 2;
         if (bullet[mbull].fuse < 100) bullet[mbull].fuse -= 2;
         if (bullet[mbull].fuse < 50) bullet[mbull].fuse -= 2;
        }
    break;
    case BULLET_GAS_GREN:
    bullet[mbull].y_speed += game[0].gravity;
    if (bullet_rand(mbull, 4) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
      create_bullet(BULLET_GAS, bullet[mbull].x, bullet[mbull].y,
       bullet[mbull].x_speed, bullet[mbull].y_speed, bullet[mbull].owner, 5, 25 + bullet_rand(mbull, 150), 0, 10, 0, 0, 96 + arena[0].counter % 5, bullet[mbull].seed);
    }
    break;
    case BULLET_IMPLODER:
    if (prand(4) < bullet[mbull].time_since_cloud)
    {
        bullet[mbull].time_since_cloud = 0;
//        if (prand(2) == 0)
        create_cloud(CLOUD_COL_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, 200 + prand(500), 5, 0, 10);
//         else create_cloud(CLOUD_COL_EXPLOSION, bullet[mbull].x - (GRAIN * 2) + ((prand(4)) * GRAIN), bullet[mbull].y - (GRAIN * 2) + ((prand(4)) * GRAIN), 0, 0, 200 + prand(500), 5, 32 + prand(40));
    }
    break;
    case BULLET_IMPLOSION:
    bullet[mbull].x_speed = 0;
    bullet[mbull].y_speed = 0;
    play_sound_pos(WAV_IMPLOSION, 1000, 250, bullet[mbull].x, bullet[mbull].y);
    blast(bullet[mbull].x, bullet[mbull].y, 15000, bullet[mbull].owner, 5, -10, 0, 10, 1);
    if (bullet[mbull].fuse < bullet[mbull].status)
     bullet[mbull].status --;
      else
       if (bullet[mbull].status < 25 && bullet[mbull].status > 8)
       {
        bullet[mbull].status += (3);
        bullet[mbull].status --;
       }
    break;
   }
   // may not reach this point if grapple anchored or bullet destroyed


   if (move_bullet(mbull, bullet[mbull].x_speed, bullet[mbull].y_speed, 1) == 0)
    return;

/*
... now for bullet stuff that happens after movement:
*/
   switch(bullet[mbull].bullet_type)
   {
    case BULLET_SEEKER:
    case BULLET_TRACKER:
    place_light(bullet[mbull].x, bullet[mbull].y, 50);
    case BULLET_BOUNCY:
    //if (serial[0].game_type == SERIAL_SERVER)
    // async_smart(mbull);
    //break;
    case BULLET_REMOTE_ROCKET:
    case BULLET_REMOTE_ROCKET_C:
    place_light(bullet[mbull].x, bullet[mbull].y, 50);
 //   if ((serial[0].game_type == SERIAL_CLIENT
 //       && user[actor[bullet[mbull].owner].user].status == USTAT_LOCAL)
 //       || serial[0].game_type == SERIAL_SERVER)
 //        async_smart(mbull);
 //   break;
    case BULLET_NAPALM:
    place_light(bullet[mbull].x, bullet[mbull].y, 30 + prand(30));
    break;
    case BULLET_FIREBOMB:
    case BULLET_FLAME:
    place_light(bullet[mbull].x, bullet[mbull].y, 30 + prand(30));
    break;
    case BULLET_NUKE_EXP:
    place_light(bullet[mbull].x, bullet[mbull].y, bullet[mbull].fuse * 3);
    break;
    case BULLET_RPG:
    case BULLET_LR_ROCKET:
    case BULLET_NUKE_M:
    case BULLET_ROCKET:
    place_light(bullet[mbull].x, bullet[mbull].y, 50);
    break;
    case BULLET_SQUIRM:
    case BULLET_SWARM:
    place_light(bullet[mbull].x, bullet[mbull].y, 40);
    break;
    case BULLET_PRONG:
    case BULLET_PLAS_R:
    case BULLET_CUBE:
    place_light(bullet[mbull].x, bullet[mbull].y, 40);
    break;
    case BULLET_ELECTRO:
    place_light(bullet[mbull].x, bullet[mbull].y, 30 + prand(30));
    break;
    case BULLET_NUKE_PLAS:
    place_light(bullet[mbull].x, bullet[mbull].y, 70);
    break;
    case BULLET_FIREBALL:
    case BULLET_PLAS_C:
    place_light(bullet[mbull].x, bullet[mbull].y, 50);
    break;
    case BULLET_PLINKER:
    place_light(bullet[mbull].x, bullet[mbull].y, 50);
    break;
    case BULLET_RECTIFIER:
    place_light(bullet[mbull].x, bullet[mbull].y, 70);
    break;
    case BULLET_INCENDIARY:
    place_light(bullet[mbull].x, bullet[mbull].y, 25);
    break;
    case BULLET_IMPLOSION:
    place_dark(bullet[mbull].x, bullet[mbull].y, prand(5));
    break;
//    case BULLET_SPARK:
//    place_light(bullet[mbull].x, bullet[mbull].y, bullet[mbull].fuse);
//    break;


   }
   
}


int move_bullet(int mbull, int move_x2, int move_y2, char limited)
{


//if (arena[0].counter % 25 != 0)
// return 1;

if (move_x2 == 0 && move_y2 == 0) return 1;

char dir_x = 0, dir_y = 0;

int move_x = move_x2;
int move_y = move_y2;

int loop_count = 0;
int timeout = 0;

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;


int step_x = move_x;
int step_y = move_y;

int oldx = bullet[mbull].x;
int oldy = bullet[mbull].y;

/*
if (step_x == 0)
{
 if (step_y > 0)
  step_y = GRAIN;
   else step_y = -GRAIN;
}
 else
if (step_y == 0)
{
 if (step_x > 0)
  step_x = GRAIN;
   else step_x = -GRAIN;
}
 else
if (abs(step_x) >= abs(step_y))
{
 step_y = (step_y * GRAIN) / abs(step_x);
 step_x = (GRAIN * step_x) / abs(step_x);
} else
if (abs(step_y) > abs(step_x))
{
 step_x = (step_x * GRAIN) / abs(step_y);
 step_y = (GRAIN * step_y) / abs(step_y);
}
*/

if (step_x == 0)
{
 if (step_y > GRAIN)
  step_y = GRAIN;
 if (step_y < -GRAIN)
  step_y = -GRAIN;
}
 else
if (step_y == 0)
{
 if (step_x > GRAIN)
  step_x = GRAIN;
 if (step_x < -GRAIN)
  step_x = -GRAIN;
}
 else
 if (abs(step_x) > GRAIN || abs(step_y) > GRAIN)
 {
  if (abs(step_x) >= abs(step_y))
  {
   step_y = (step_y * GRAIN) / abs(step_x);
   step_x = (GRAIN * step_x) / abs(step_x);
  } else
  if (abs(step_y) > abs(step_x))
  {
   step_x = (step_x * GRAIN) / abs(step_y);
   step_y = (GRAIN * step_y) / abs(step_y);
  }
 }

 if (move_x == 10 && move_y == 0 && bullet[mbull].damage != -1)
 {
  if (actor_collision(mbull) == 1) return 0;
 }


while (move_x != 0 || move_y != 0)
{
 timeout ++;
 if (timeout > 1000)
  break;
 if (bullet[mbull].bullet_type == BULLET_GRAPPLE || bullet[mbull].bullet_type == BULLET_LASER_BEAM || bullet[mbull].bullet_type == BULLET_LASER_PULSE)
 {
  if (get_dirt(bullet [mbull].x / GRAIN, bullet [mbull].y / GRAIN))
  {
   bullet_impact(mbull, 1, 1, -1);
  }
 } else
 {
  if (bullet[mbull].bullet_type != BULLET_DISRUPTER_WAVE)
  {
  if (get_dirt((bullet [mbull].x) / GRAIN, (bullet [mbull].y + step_y) / GRAIN))
  {
          switch(bullet_impact(mbull, 0, 1, -1))
          {
           case 0: // bounced
           if (step_y > 0)
           {
            bullet[mbull].y_speed /= 2;
            bullet[mbull].y_speed *= -1;
            step_y /= 2;
            step_y *= -1;
            move_y /= 2;
            move_y *= -1;
            dir_y *= -1;
           } else
           {
            bullet[mbull].y_speed *= -1;
            bullet[mbull].y_speed /= 2;
            step_y *= -1;
            step_y /= 2;
            move_y *= -1;
            move_y /= 2;
            dir_y *= -1;
           }
 //          bullet[mbull].x_speed /= (float) -1;
 //          return;
           break;
           case 1: return 0; // exploded
           case 2:
 //          bullet[mbull].y_speed = 0;
 //          bullet[mbull].y_speed *= -1;
           bullet[mbull].y_speed /= -5;
           return 1;
           case 3: // elastic bounce
           bullet[mbull].y_speed *= -1;
           return 1;
           case 4:
           bullet[mbull].y_speed = 0;
           bullet[mbull].x_speed = 0;
 //          if (abs(bullet[mbull].y_speed) > 150)
 //           bullet[mbull].y_speed /= -3;
 //            else
 //             bullet[mbull].y_speed *= -1;
 //          if (abs(bullet[mbull].y_speed) < 50) bullet[mbull].y_speed *= 3;
           return 1;
          }
  } else
  if (get_dirt((bullet [mbull].x + step_x) / GRAIN, (bullet [mbull].y) / GRAIN))
  {
          switch(bullet_impact(mbull, 1, 0, -1))
          {
           case 0: // bounced
 //
           if (step_x > 0)
           {
            bullet[mbull].x_speed /= 2;
            bullet[mbull].x_speed *= -1;
            step_x /= 2;
            step_x *= -1;
            move_x /= 2;
            move_x *= -1;
            dir_x *= -1;
           } else
           {
            bullet[mbull].x_speed *= -1;
            bullet[mbull].x_speed /= 2;
            step_x *= -1;
            step_x /= 2;
            move_x *= -1;
            move_x /= 2;
            dir_x *= -1;
           }

 //          bullet[mbull].x_speed /= (float) -1;
           break;
           case 1: return 0; // exploded
           case 2:
 //          bullet[mbull].y_speed = 0;
 //          bullet[mbull].y_speed *= -1;
           bullet[mbull].x_speed /= -5;
           return 1;
           case 3: // elastic bounce
           bullet[mbull].x_speed *= -1;
           return 1;
           case 4:
 //          if (abs(bullet[mbull].y_speed) > 150)
 //           bullet[mbull].y_speed /= -3;
 //            else
 //             bullet[mbull].x_speed *= -1;
 //          if (abs(bullet[mbull].y_speed) < 50) bullet[mbull].y_speed *= 3;
           bullet[mbull].x_speed = 0;
           bullet[mbull].y_speed = 0;
           return 1;
          }
 //         break;
  } else
  if (get_dirt((bullet [mbull].x + step_x) / GRAIN, (bullet [mbull].y + step_y) / GRAIN))
  {
          switch(bullet_impact(mbull, 1, 1, -1))
          {
           case 0: // bounced
           if (step_y > 0)
           {
            bullet[mbull].y_speed /= 2;
            bullet[mbull].y_speed *= -1;
            step_y /= 2;
            step_y *= -1;
            move_y /= 2;
            move_y *= -1;
            dir_y *= -1;
           } else
           {
            bullet[mbull].y_speed *= -1;
            bullet[mbull].y_speed /= 2;
            step_y *= -1;
            step_y /= 2;
            move_y *= -1;
            move_y /= 2;
            dir_y *= -1;
           }
           if (step_x > 0)
           {
            bullet[mbull].x_speed /= 2;
            bullet[mbull].x_speed *= -1;
            step_x /= 2;
            step_x *= -1;
            move_x /= 2;
            move_x *= -1;
            dir_x *= -1;
           } else
           {
            bullet[mbull].x_speed *= -1;
            bullet[mbull].x_speed /= 2;
            step_x *= -1;
            step_x /= 2;
            move_x *= -1;
            move_x /= 2;
            dir_x *= -1;
           }

 //          bullet[mbull].x_speed /= (float) -1;
           break;
 //          return;
           case 1: return 0; // exploded
           case 2:
 //          bullet[mbull].y_speed = 0;
 //          bullet[mbull].y_speed *= -1;
           bullet[mbull].x_speed /= -5;
           bullet[mbull].y_speed /= -5;
           return 1;
           case 3: // elastic bounce
           bullet[mbull].x_speed *= -1;
           bullet[mbull].y_speed *= -1;
           return 1;
           case 4:
 //          if (abs(bullet[mbull].y_speed) > 150)
           bullet[mbull].y_speed = 0;
           bullet[mbull].x_speed = 0;
 //           bullet[mbull].y_speed /= -3;
 //            else
 //             bullet[mbull].x_speed *= -1;
 //          if (abs(bullet[mbull].y_speed) < 50) bullet[mbull].y_speed *= 3;
           return 1;
          }
 //   return;
   } // end of if !disrupter_wave
   } // end of if grapple... else
  }

 if (bullet[mbull].damage != -1 && loop_count % TARGET_THICKNESS == 0)
 {
  if (actor_collision(mbull) == 1) return 0;
 }






 if (step_x == 0 && step_y == 0) return 1;

 if (step_x == 0) move_x = 0;
 if (step_y == 0) move_y = 0;

//      create_cloud(CLOUD_DEBUG, bullet[mbull].x, bullet[mbull].y, 0, 0, 2000, 20, 0);


 if (move_y != 0 && (abs(move_y + step_y) == move_y + step_y) != (abs(step_y) == step_y))
 {
//  move_y = 0;
  step_y = move_y;
  if (move_x == 0) break;
 }
 
 if (move_x != 0 && (abs(move_x + step_x) == move_x + step_x) != (abs(step_x) == step_x))
 {
//  move_x = 0;
  step_x = move_x;
  if (move_y == 0) break;
 }
/* if (move_y != 0 && (abs(move_y) == move_y) != (abs(step_y) == step_y))
 {
  move_y = 0;
  step_y = 0;
  if (move_x == 0) break;
 }
 if (move_x != 0 && (abs(move_x) == move_x) != (abs(step_x) == step_x))
 {
  move_x = 0;
  step_x = 0;
  if (move_y == 0) break;
 }*/

 if ((move_x > 0) != (step_x > 0))// && (move_x < 0) != (step_x < 0))
 {
//  move_x = 0;
  step_x = move_x;
  if (move_y == 0) break;
 }

 if ((move_y > 0) != (step_y > 0))// && (move_y < 0) != (step_y < 0))
 {
//  move_y = 0;
  step_y = move_y;
  if (move_x == 0) break;
 }


 if (limited == 1)
 {
  move_x -= step_x;
  move_y -= step_y;
 }

 if (bullet[mbull].bullet_type != BULLET_DISRUPTER_WAVE)
 {
  if (bullet[mbull].bullet_type == BULLET_GRAPPLE || bullet[mbull].bullet_type == BULLET_LASER_BEAM || bullet[mbull].bullet_type == BULLET_LASER_PULSE)
  {
    if (get_dirt((bullet [mbull].x) / GRAIN, (bullet [mbull].y) / GRAIN))
     continue;
  }
   else
    {
     if (get_dirt((bullet [mbull].x + step_x) / GRAIN, (bullet [mbull].y + step_y) / GRAIN))
      continue;
    }
 }
 bullet[mbull].x += step_x;
 bullet[mbull].y += step_y;


// if (abs(move_x) <= GRAIN / 2) move_x = 0;

} // end of while loop



/*



 if (move_x >= GRAIN)
 {
  bullet[mbull].x += GRAIN;
  move_x -= GRAIN;
 } else
 {
  if (move_x <= -GRAIN)
  {
    bullet[mbull].x -= GRAIN;
    move_x += GRAIN;
  } else
   {
    bullet[mbull].x += move_x;
    move_x = 0;
   }
 }

 if (move_y >= GRAIN)
 {
  bullet[mbull].y += GRAIN;
  move_y -= GRAIN;
 } else
 {
  if (move_y <= -GRAIN)
  {
    bullet[mbull].y -= GRAIN;
    move_y += GRAIN;
  } else
   {
    bullet[mbull].y += move_y;
    move_y = 0;
   }
 }

 

 loop_count += GRAIN;
 

} // end of while loop

*/

if (oldx == bullet[mbull].x && oldy == bullet[mbull].y)
{
 bullet[mbull].x_speed = 0;
 bullet[mbull].y_speed = 0;
}



return 1;

}




void slow_bullet(int sbull, float slowed)
{
    if (abs(bullet[sbull].x_speed) != bullet[sbull].x_speed)
    {
       bullet[sbull].x_speed = abs(bullet[sbull].x_speed);
       bullet[sbull].x_speed *= slowed;
       bullet[sbull].x_speed *= -1;
    }
        else
         bullet[sbull].x_speed *= slowed;
             
    if (abs(bullet[sbull].y_speed) != bullet[sbull].y_speed)
    {
      bullet[sbull].y_speed = abs(bullet[sbull].y_speed);
      bullet[sbull].y_speed *= slowed;
      bullet[sbull].y_speed *= -1;
    }
        else
         bullet[sbull].y_speed *= slowed;


}


char actor_collision(int mbull)
{

  int actor_count;

  for (actor_count = 0; actor_count < NO_ACTORS; actor_count ++)
  {
   if (actor[actor_count].aclass == ACLASS_NONE
    || actor[actor_count].aclass == ACLASS_GHOST) continue;

   if (bullet[mbull].x >= actor[actor_count].x - (actor[actor_count].width * GRAIN)
       && bullet[mbull].x <= actor[actor_count].x + (actor[actor_count].width * GRAIN)
       && bullet[mbull].y <= actor[actor_count].y + ((actor[actor_count].height + 1) * GRAIN)
       // + 1 so that rolling bouncy bombs hit
       && bullet[mbull].y >= actor[actor_count].y - (actor[actor_count].height * GRAIN))
   {
       if (bullet[mbull].owner == actor_count && bullet[mbull].left_owner <= 10)
          continue;
       bullet_impact(mbull, 0, 0, actor_count);
       if (bullet[mbull].bullet_type != BULLET_NAPALM
        && bullet[mbull].bullet_type != BULLET_GAS)
       {
        if (serial[0].game_type != SERIAL_CLIENT
         || (bullet[mbull].bullet_type != BULLET_REMOTE_ROCKET
            && bullet[mbull].bullet_type != BULLET_REMOTE_ROCKET_C
            && bullet[mbull].bullet_type != BULLET_TRACKER
            && bullet[mbull].bullet_type != BULLET_SEEKER
            && bullet[mbull].bullet_type != BULLET_BOUNCY
            && bullet[mbull].bullet_type != BULLET_HUNTER))
             destroy_bullet(mbull);
             return 1;
       }
   }

  }

  return 0;

}


char bullet_impact(int ibull, char dir_x, char dir_y, int hit_what)
{

if (serial[0].game_type == SERIAL_CLIENT)
{
   switch(bullet[ibull].bullet_type)
   {
    case BULLET_REMOTE_ROCKET:
    case BULLET_REMOTE_ROCKET_C:
    case BULLET_TRACKER:
    case BULLET_SEEKER:
    if (bullet[ibull].fuse > 1)
     return 4;
    case BULLET_BOUNCY:
    if (bullet[ibull].fuse > 1)
     return 3;
     
   }
}

int ex = 0, ey = 0;
ex = bullet[ibull].x_speed;
if (dir_x != 0)
{
 ex *= -1;
}
ex /= 6;
ey = bullet[ibull].y_speed;
if (dir_y != 0)
{
 ey *= -1;
}
ey /= 6;
if (ex > 200) ex = 200;
if (ex < -200) ex = -200;
if (ey > 200) ey = 200;
if (ey < -200) ey = -200;


//void slam_actor(int sactor, int x_slam, int y_slam, int weight)
if (hit_what != -1 && bullet[ibull].bullet_type != BULLET_LASER_TRACER) // ie if it hit an actor rather than hit a wall/fuse expired
{
// create_cloud(CLOUD_RED_BLOOD, bullet[ibull].x, bullet[ibull].y, (bullet[ibull].x_speed + actor[hit_what].x_speed) / 10, (bullet[ibull].y_speed + actor[hit_what].y_speed) / 10, 600, 40, 0);
  if (bullet[ibull].damage > 50 && actor[hit_what].armour == 0)
   blood_burst(hit_what, 1, 0,
    bullet[ibull].x, bullet[ibull].y, actor[hit_what].x_speed, actor[hit_what].y_speed); // actor[hit_what].x_speed + bullet[ibull].x_speed / 3, actor[hit_what].y_speed + bullet[ibull].y_speed / 3);

 if (bullet[ibull].is_bullet)
 {
  if (game[0].fast_bullets == 1)
  {
   slam_actor(hit_what, (int) bullet[ibull].x_speed / 5, (int) bullet[ibull].y_speed / 5, bullet[ibull].weight);
  } else
    if (game[0].fast_bullets == 2)
    {
     slam_actor(hit_what, (int) bullet[ibull].x_speed / 100, (int) bullet[ibull].y_speed / 100, bullet[ibull].weight);
    }
     else
      slam_actor(hit_what, bullet[ibull].x_speed, bullet[ibull].y_speed, bullet[ibull].weight);
 }

 int how_hurt = hurt_actor(hit_what, bullet[ibull].owner, bullet[ibull].damage, 1, 0, 0);

 if (how_hurt == 1)
 {
  switch(user[actor[hit_what].user].soldier)
  {
   case SOLDIER_SCOUT:
   case SOLDIER_SOLDIER:
   create_cloud(CLOUD_RED_BLOOD, bullet[ibull].x, bullet[ibull].y, 0, 0, 300, 40, 0, 0);
   break;
   case SOLDIER_BUG:
   create_cloud(CLOUD_COL_CIRCLE, bullet[ibull].x, bullet[ibull].y, 0, 0, 300, 40, COLOUR_YELLOW4, 0);
   break;
   case SOLDIER_SNAIL:
   create_cloud(CLOUD_COL_CIRCLE, bullet[ibull].x, bullet[ibull].y, 0, 0, 300, 40, COLOUR_GREY4, 0);
   break;
   case SOLDIER_CYBORG:
   case SOLDIER_CRUSHER:
   case SOLDIER_WALKER:
   create_cloud(CLOUD_COL_CIRCLE, bullet[ibull].x, bullet[ibull].y, 0, 0, 340, 70, COLOUR_BLUE4, 10);
   break;
   case SOLDIER_DEMON:
   create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 10, 0, 10);
   create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 10, 0, 30);
   create_cloud(CLOUD_RISING_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 10, 0, 30);
   break;
  }
 }
 // doesn't make blood if shield blocked all damage
 if (how_hurt == 2)
 {
  switch(user[actor[hit_what].user].soldier)
  {
   case SOLDIER_WALKER:
   default:
   case SOLDIER_SOLDIER:
   create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 500, 40, COLOUR_BLUE4, 10);
   break;
  }
 }
}

int fcount = 0;

   switch(bullet[ibull].bullet_type)
   {
    case BULLET_GRAPPLE:
    if (actor[bullet[ibull].owner].grapple_anchored == 0)
     sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_YELLOW4, 1 + prand(5));
    bullet[ibull].x_speed = 0;
    bullet[ibull].y_speed = 0;
//    if (prand(2))
    actor[bullet[ibull].owner].grapple_anchored = 1;
    return 4;
    case BULLET_ELECTRO:
    play_sound_pos(WAV_LIGHTNING2, 1000, 250, bullet[ibull].x, bullet[ibull].y);
//     bullet[mbull].status = bullet[mbull].x;
//     bullet[mbull].status2 = bullet[mbull].y;

    create_cloud(CLOUD_ELECTRO,
     bullet[ibull].x, //bullet[mbull].status,
     bullet[ibull].y, //bullet[mbull].status2,
     bullet[ibull].status,
     bullet[ibull].status2,
     9, 1, 0, 0);
    create_cloud(CLOUD_ELECTRO,
     bullet[ibull].x,
     bullet[ibull].y,
     bullet[ibull].status + (((arena[0].counter) * 3) % 40 - 19) * GRAIN,
     bullet[ibull].status2 + (((arena[0].counter) * 7) % 40 - 19) * GRAIN,
     9, 1, 0, 0);
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_GREY8, 4 + prand(5));
    destroy_bullet(ibull);
    break;
    case BULLET_LASER_BEAM:
    actor[bullet[ibull].owner].laser_x = bullet[ibull].x;
    actor[bullet[ibull].owner].laser_y = bullet[ibull].y;
    actor[bullet[ibull].owner].firing_laser = LASER_BEAM;
    actor[bullet[ibull].owner].laser_strength = 2;
    if (game[0].soft_dirt)
    {
      destroy_dirt(NULL, bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN, 0);
    }
    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 300 + prand(210), 50, 0, 10);
    if (arena[0].counter % 3 == 0)
     sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_RED4, 1);
    place_light(bullet[ibull].x, bullet[ibull].y, 30 + prand(30));
    destroy_bullet(ibull);
    return 1;
    case BULLET_LASER_PULSE:
    actor[bullet[ibull].owner].laser_x = bullet[ibull].x;
    actor[bullet[ibull].owner].laser_y = bullet[ibull].y;
    actor[bullet[ibull].owner].firing_laser = LASER_PULSE;
    actor[bullet[ibull].owner].laser_strength = 2;
    if (game[0].soft_dirt)
    {
      destroy_dirt(NULL, bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN, 0);
    }
    create_cloud(CLOUD_LBLUE_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 300 + prand(210), 50, 0, 10);
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_BLUE4, 3 + prand(5));
    place_light(bullet[ibull].x, bullet[ibull].y, 30 + prand(30));
    destroy_bullet(ibull);
    return 1;
    case BULLET_LASER_TRACER:
    actor[bullet[ibull].owner].laser_x = bullet[ibull].x;
    actor[bullet[ibull].owner].laser_y = bullet[ibull].y;
    actor[bullet[ibull].owner].firing_laser = LASER_TRACER;
    actor[bullet[ibull].owner].lock_on = hit_what;
    actor[bullet[ibull].owner].laser_strength = 2;
    destroy_bullet(ibull);
    return 1;
    case BULLET_REMOTE_ROCKET:
    case BULLET_REMOTE_ROCKET_C:
    if (actor[bullet[ibull].owner].remote_thing == ibull)
     actor[bullet[ibull].owner].remote_control = REMOTE_NONE;
//    if (serial[0].
    case BULLET_TRACKER:
    case BULLET_SEEKER:
 //   if (serial[0].game_type == SERIAL_SERVER)
 //   {
 //    async_smart_destroy(ibull);
 //   }
    case BULLET_ROCKET:
    case BULLET_LR_ROCKET:
    case BULLET_GREN_LAUNCHER:
    play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
//    play_bsound(0);
    place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 2200, 1);
    shrapnel_burst(ibull, 9, 50, 0, BULLET_SHRAPNEL);
    blast(bullet[ibull].x, bullet[ibull].y, 6000, bullet[ibull].owner, 400, 30, 5, 5, 1);
/*    if (serial[0].game_type == SERIAL_SERVER
        || bullet[ibull].bullet_type == BULLET_ROCKET
        || bullet[ibull].bullet_type == BULLET_LR_ROCKET
        || bullet[ibull].bullet_type == BULLET_GREN_LAUNCHER)*/
    destroy_bullet(ibull);
//    hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 60);
    break;
    case BULLET_RPG:
    play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 1300, 1);
    shrapnel_burst(ibull, 4, 50, 0, BULLET_SHRAPNEL);
    blast(bullet[ibull].x, bullet[ibull].y, 4000, bullet[ibull].owner, 300, 20, 3, 5, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_FLAK:
    // has a larger explosion in the air than if it hits something
    play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    if (bullet[ibull].fuse == 0)
    {
     place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 2200, 1);
     shrapnel_burst(ibull, 10, 50, 0, BULLET_SHRAPNEL);
     blast(bullet[ibull].x, bullet[ibull].y, 6000, bullet[ibull].owner, 300, 30, 3, 3, 1);
    } else
    {
     place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 500, 1);
     shrapnel_burst(ibull, 7, 50, 0, BULLET_SHRAPNEL);
     blast(bullet[ibull].x, bullet[ibull].y, 2000, bullet[ibull].owner, 200, 30, 3, 3, 1);
    }
    destroy_bullet(ibull);
//    hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 40);
    break;
    case BULLET_AUTOCANNON:
    play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    if ((game[0].bullets_explode != BULLETS_NO_EXPLODE) && hit_what == -1)
    {
     place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 1500, 1);
     blast(bullet[ibull].x, bullet[ibull].y, 3000, bullet[ibull].owner, 300, 30, 2, 3, 1);
    } else
    {
     place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 500, 1);
     blast(bullet[ibull].x, bullet[ibull].y, 2000, bullet[ibull].owner, 300, 30, 2, 3, 1);
    }
    shrapnel_burst(ibull, 3, 50, 0, BULLET_SHRAPNEL);
    destroy_bullet(ibull);
//    hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 30);
    break;
    case BULLET_PRONG:
    play_sound_pos(WAV_POP, 1000, 250, bullet[ibull].x, bullet[ibull].y);
//    place_explosion(bullet[ibull].x, bullet[ibull].y, 1200, 5);
//     place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 800, 0);
     create_cloud(CLOUD_ORANGE_EXPLOSION, bullet[ibull].x, bullet[ibull].y, ex, ey, 800, -150, 0, 40);
     create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, ex, ey, 800, -150, 0, 60);
     create_cloud(CLOUD_RISING_LIGHT, bullet[ibull].x, bullet[ibull].y, ex, ey, 800, -150, 0, 40);
     sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_YELLOW4, 1 + prand(3));
     blast(bullet[ibull].x, bullet[ibull].y, 1500, bullet[ibull].owner, bullet[ibull].damage, 30, 2, 0, 1);
//    shrapnel_burst(ibull, 2, 50, 0, BULLET_SHRAPNEL);
//    blast(bullet[ibull].x, bullet[ibull].y, 3000, bullet[ibull].owner, 200, 12, 3, 4, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_SQUIRM:
    play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 900, 1);
//    shrapnel_burst(ibull, 2, 50, 0, BULLET_SHRAPNEL);
    // same blast as swarm, but no shrapnel.
    blast(bullet[ibull].x, bullet[ibull].y, 2000, bullet[ibull].owner, 250, 12, 3, 4, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_SWARM:
    play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 1200, 1);
    shrapnel_burst(ibull, 3, 50, 0, BULLET_SHRAPNEL);
    blast(bullet[ibull].x, bullet[ibull].y, 3000, bullet[ibull].owner, 250, 12, 3, 4, 1);
    destroy_bullet(ibull);
//    hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 30);
    break;
    case BULLET_NUKE_M:
    play_sound_pos(WAV_NUKE, 1000, 250, bullet[ibull].x, bullet[ibull].y);
//    place_explosion(bullet[ibull].x, bullet[ibull].y, 8000, 5);
    create_cloud(CLOUD_SHOCKWAVE, bullet[ibull].x, bullet[ibull].y, 0, 0, 17000, 400, 20000, 0);
    create_cloud(CLOUD_COL_CIRCLE, bullet[ibull].x, bullet[ibull].y, 0, 0, 8000, 10, COLOUR_GREY8, 10);
    place_explosion(bullet[ibull].x + prand(5000) - 2500, bullet[ibull].y + prand(5000) - 2500, 0, -200, 3000 + prand(2000), 1);
    place_explosion(bullet[ibull].x + prand(5000) - 2500, bullet[ibull].y + prand(5000) - 2500, 0, -200, 3000 + prand(2000), 1);
    place_explosion(bullet[ibull].x + prand(5000) - 2500, bullet[ibull].y + prand(5000) - 2500, 0, -200, 3000 + prand(2000), 1);
    blast(bullet[ibull].x, bullet[ibull].y, 15000, bullet[ibull].owner, 4000, 100, 33, 30, 1);
    create_bullet(BULLET_NUKE_EXP, bullet[ibull].x, bullet[ibull].y,
     0, -100, bullet[ibull].owner, 100, 100, 0, 100, 0, 0, 0, bullet[ibull].seed);
    for (fcount = 0; fcount < 5; fcount ++)
    {
     create_bullet(BULLET_NUKE_PLAS, bullet[ibull].x, bullet[ibull].y, bullet_rand(ibull, 2000) - 1000, bullet_rand(ibull, 2000) - 1000, bullet[ibull].owner, 1, 0, 0, 20, 0, 0, 0, bullet[ibull].seed);
    }
    destroy_bullet(ibull);
    nuke_flash();
//    hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 450);
    break;
    case BULLET_NUKE_EXP:
    if (dir_x == 0 && dir_y == 0)
    {
     create_cloud(CLOUD_GREY_SMOKE, bullet[ibull].x, bullet[ibull].y, 0, 0, 5000, 10, 0, 0);
     create_cloud(CLOUD_GREY_SMOKE, bullet[ibull].x + 3000 + prand(1000), bullet[ibull].y - 500 + prand(1000), 0, 0, 2000 + prand(1000), 10, 0, 0);
     create_cloud(CLOUD_GREY_SMOKE, bullet[ibull].x - 3000 - prand(1000), bullet[ibull].y - 500 + prand(1000), 0, 0, 2000 + prand(1000), 10, 0, 0);
     create_cloud(CLOUD_GREY_SMOKE, bullet[ibull].x + 4500 + prand(1500), bullet[ibull].y - 300 + prand(600), 0, 0, 2000 + prand(1000), 10, 0, 0);
     create_cloud(CLOUD_GREY_SMOKE, bullet[ibull].x - 4500 - prand(1500), bullet[ibull].y - 300 + prand(600), 0, 0, 2000 + prand(1000), 10, 0, 0);
     destroy_bullet(ibull);
    } else bullet[ibull].y_speed = 0;
//    hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 150);
    return 0;
    case BULLET_GRENADE:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     place_explosion(bullet[ibull].x, bullet[ibull].y, bullet[ibull].x_speed, bullet[ibull].y_speed, 2200, 1);
     shrapnel_burst(ibull, 20, 100, 0, BULLET_GREN_SHRAPNEL);
     blast(bullet[ibull].x, bullet[ibull].y, 6000, bullet[ibull].owner, 700, 30, 5, 5, 1);
     destroy_bullet(ibull);
//     hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 60);
     return 1;
    } /*else
    {
     bullet_bounce(ibull, dir_x, dir_y);
     return 0;
    }*/
    return 0;
    case BULLET_BOMB:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     place_explosion(bullet[ibull].x, bullet[ibull].y, bullet[ibull].x_speed, bullet[ibull].y_speed, 1300, 1);
     shrapnel_burst(ibull, 6, 50, 0, BULLET_SHRAPNEL);
     blast(bullet[ibull].x, bullet[ibull].y, 3000, bullet[ibull].owner, 350, 25, 3, 5, 1);
     destroy_bullet(ibull);
     return 1;
    }
    return 0;
    case BULLET_FRAG:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     place_explosion(bullet[ibull].x, bullet[ibull].y, bullet[ibull].x_speed, bullet[ibull].y_speed, 1300, 1);
     shrapnel_burst(ibull, 50, 100, 0, BULLET_GREN_SHRAPNEL);
     blast(bullet[ibull].x, bullet[ibull].y, 4000, bullet[ibull].owner, 300, 30, 3, 5, 1);
     destroy_bullet(ibull);
//     hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 20);
     return 1;
    }
    return 0;
    case BULLET_GRAPESHOT:
    case BULLET_FAT_BULLET:
    case BULLET_BULLET:
    case BULLET_SHRAPNEL:
    case BULLET_GREN_SHRAPNEL:
    if (prand(15) == 0)
     play_sound_pos(WAV_RICOCHET, 1000, 250, bullet[ibull].x, bullet[ibull].y);
      else
       play_sound_pos(WAV_POP, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    if (hit_what == -1)
    {
     if (game[0].bullets_explode == BULLETS_EXPLODE)
     {
      place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 500, 1);
      blast(bullet[ibull].x, bullet[ibull].y, 1500, bullet[ibull].owner, bullet[ibull].damage, 2, 2, 0, 1);
     }
     if (game[0].bullets_explode == BULLETS_NO_EXPLODE)
     {
      create_cloud(CLOUD_GREY_SMOKE, bullet[ibull].x, bullet[ibull].y, 0, 0, 300, 10, 0, 0);
      destroy_dirt(NULL, bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN, 0);
      destroy_dirt(NULL, bullet[ibull].x / GRAIN - 1, bullet[ibull].y / GRAIN, 0);
      destroy_dirt(NULL, bullet[ibull].x / GRAIN + 1, bullet[ibull].y / GRAIN, 0);
      destroy_dirt(NULL, bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN + 1, 0);
      destroy_dirt(NULL, bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN - 1, 0);
      sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_YELLOW4, prand(3));
     }
     if (game[0].bullets_explode == BULLETS_PAINTBALL)
     {
      splatter(bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN, 0, bullet[ibull].status);
     }
    }
    destroy_bullet(ibull);
//    hurt_grid(bullet[ibull].x, bullet[ibull].y, dir_x, dir_y, 5);
    break;
    case BULLET_PLAS_R:
    play_sound_pos(WAV_PLASR_BANG, 1000, 250, bullet[ibull].x, bullet[ibull].y);
//    create_cloud(CLOUD_LBLUE_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 900, 30, 0);
    create_cloud(CLOUD_BLUE_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 900, -100, 0, 20);
    create_cloud(CLOUD_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 900, -100, 0, 20);
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_BLUE4, 1 + prand(5));
    blast(bullet[ibull].x, bullet[ibull].y, 2000, bullet[ibull].owner, 300, 30, 2, 2, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_PLAS_C:
    play_sound_pos(WAV_BANG2, 1000, 250, bullet[ibull].x, bullet[ibull].y);
//    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 20, 0, 10);
//    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 30, 0, 30);
    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, 0, 10);
    create_cloud(CLOUD_ORANGE_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, 0, 20);
    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, 0, 30);
    create_cloud(CLOUD_RISING_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, 0, 10);
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_RED4, 1 + prand(4));
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_YELLOW4, 1 + prand(4));
    blast(bullet[ibull].x, bullet[ibull].y, 4000, bullet[ibull].owner, 500, 30, 3, 3, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_FIREBALL:
    play_sound_pos(WAV_POP, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    fcount = bullet[ibull].fuse + 10;
//    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 20, 0, 10);
//    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 30, 0, 30);
    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, ex, ey, fcount * 10, -100, 0, 10);
    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, ex, ey, fcount * 10, -100, 0, 30);
    create_cloud(CLOUD_RISING_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, fcount * 10, -100, 0, 10);
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_RED4, 1 + prand(4));
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_YELLOW4, 1 + prand(4));
    blast(bullet[ibull].x, bullet[ibull].y, fcount * 30, bullet[ibull].owner, fcount * 5, 30, 3, 3, 1);
    if (bullet[ibull].fuse < 25 || (dir_x == 0 && dir_y == 0))
    {
     destroy_bullet(ibull);
     return 1;
    }
    bullet[ibull].fuse -= 20;
    return 0;
    case BULLET_CUBE:
    play_sound_pos(WAV_CUBE, 900 + prand(200), 250, bullet[ibull].x, bullet[ibull].y);
    create_cloud(CLOUD_CUBE, bullet[ibull].x, bullet[ibull].y, 0, 0, 900, -100, bullet[ibull].status, 20);
    create_cloud(CLOUD_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 900, -100, 0, 20);
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_GREY8, 1 + prand(5));
    blast(bullet[ibull].x, bullet[ibull].y, 2000, bullet[ibull].owner, 300, 30, 2, 2, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_SMITELET:
    if (dir_x == 0 && dir_y == 0)
    {
     destroy_bullet(ibull);
     return 1;
    }
    return 0;
    case BULLET_SMITER:
    play_sound_pos(WAV_BANG2, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 20, 0, 10);
    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 30, 0, 30);
    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, 0, 10);
//    create_cloud(CLOUD_ORANGE_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, 0, 20);
//    create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, TRANS_BLUE, 20);
//    create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, TRANS_GREEN, 10);
    create_cloud(CLOUD_RISING_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 1500, -100, 0, 10);
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_RED4, 1 + prand(4));
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_YELLOW4, 1 + prand(4));
    blast(bullet[ibull].x, bullet[ibull].y, 4000, bullet[ibull].owner, 500, 30, 3, 3, 1);
    for (fcount = 0; fcount < 5; fcount ++)
    {
     create_bullet(BULLET_SMITELET, bullet[ibull].x, bullet[ibull].y, bullet_rand(ibull, 1000) - 500, bullet_rand(ibull, 1000) - 500, bullet[ibull].owner, 50, 15, 0, 8, 0, 0, 0, bullet[ibull].seed);
    }
    destroy_bullet(ibull);
    break;
    case BULLET_NUKE_PLAS:
    play_sound_pos(WAV_BANG2, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    create_cloud(CLOUD_RED_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, -100, 0, 10);
    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, -100, 0, 30);
    create_cloud(CLOUD_RISING_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, -100, 0, 10);
    blast(bullet[ibull].x, bullet[ibull].y, 4000, bullet[ibull].owner, 1500, 30, 10, 3, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_PLINKER:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_BANG2, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1300,-100, TRANS_BLUE, 10);
     create_cloud(CLOUD_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 1300, -100, 0, 10);
//     create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1300, 30, 170, 30);
     blast(bullet[ibull].x, bullet[ibull].y, 3000, bullet[ibull].owner, 500, 30, 3, 3, 1);
     destroy_bullet(ibull);
     return 1;
    }
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_BLUE4, 1 + prand(4));
    return 3;
    case BULLET_SHREDDER:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_POP, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 20, TRANS_WHITE, 10);
     sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_GREY8, 1 + prand(4));
     destroy_bullet(ibull);
     return 1;
    }
//    if (prand(2))
//     sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_YELLOW4, 1);
    if (abs(bullet[ibull].x_speed) < GRAIN && abs(bullet[ibull].y_speed) < GRAIN)
    {
     bullet[ibull].x_speed = 0;
     bullet[ibull].y_speed = 0;
    }
    if (abs(bullet[ibull].y_speed) + abs(bullet[ibull].x_speed) > 500)
       play_sound_pos(WAV_POP, 1000, 250, bullet[ibull].x, bullet[ibull].y);

    return 0;
    case BULLET_RECTIFIER:
    play_sound_pos(WAV_RECTBANG2, 1000, 250, bullet[ibull].x, bullet[ibull].y);
//    create_cloud(CLOUD_LGREEN_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 4000, 10, 0);
//    create_cloud(CLOUD_LGREEN_ELLIPSE, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 10, 4000);
//    create_cloud(CLOUD_YELLOW_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 1300, 20, 0);
//    create_cloud(CLOUD_GREEN_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 4000, 20, 0, 10);
//    create_cloud(CLOUD_GREEN_ELLIPSE, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, 20, 4000, 10);
    create_cloud(CLOUD_GREEN_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 3500, -100, 0, 15);
    create_cloud(CLOUD_GREEN_ELLIPSE, bullet[ibull].x, bullet[ibull].y, 0, 0, 1700, -100, 4000, 15);
    create_cloud(CLOUD_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 3500, -100, 0, 15);
    blast(bullet[ibull].x, bullet[ibull].y, 15000, bullet[ibull].owner, 2000, 100, 13, 10, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_FIREBOMB:
    for (fcount = 0; fcount < 8; fcount ++)
    {
     create_bullet(BULLET_NAPALM, bullet[ibull].x, bullet[ibull].y,
      bullet_rand(ibull, 400) - 200, bullet_rand(ibull, 400) - 200, bullet[ibull].owner, 0, 150 + bullet_rand(ibull, 55), 0, 10, 0, 0, 0, bullet[ibull].seed);
    }
    play_sound_pos(WAV_POP, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    destroy_bullet(ibull);
    break;
    case BULLET_INCENDIARY:
     create_bullet(BULLET_NAPALM, bullet[ibull].x, bullet[ibull].y,
      bullet_rand(ibull, 400) - 200, bullet_rand(ibull, 400) - 200, bullet[ibull].owner, 0, 50 + bullet_rand(ibull, 45), 0, 10, 0, 0, 0, bullet[ibull].seed);
    destroy_bullet(ibull);
    break;
    case BULLET_NAPALM:
    if (bullet[ibull].left_owner <= 2)
     return 3;
/*    if (game[0].soft_dirt)
    {

    } else
    {*/
//     if (!prand(3)) destroy_dirt(NULL, (bullet[ibull].x / GRAIN) + prand(3) - 1, (bullet[ibull].y / GRAIN) + prand(3) - 1, 0);
//     if (!prand(10)) destroy_dirt(NULL, (bullet[ibull].x / GRAIN), (bullet[ibull].y / GRAIN), 0);
//     bullet[ibull].x -= GRAIN; prand(50);// - prand(99);
//     bullet[ibull].y -= GRAIN;//prand(50);// - prand(99);
//     destroy_dirt(NULL, (bullet[ibull].x / GRAIN) + prand(3) - 1, (bullet[ibull].y / GRAIN) + prand(3) - 1, 0);
//   l  destroy_dirt(NULL, (bullet[ibull].x / GRAIN) + prand(3) - 1, (bullet[ibull].y / GRAIN) + prand(3) - 1, 0);
//    }
//    if (dir_x == 0 && dir_y == 0)
//    {
     if (bullet[ibull].fuse <= 0)
      destroy_bullet(ibull);
       else
       {
        if (bullet_rand(ibull, 20) == 0)
         splatter(bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN, 0, COLOUR_GREY1);
        return 4;
       }
//    }
    return 1;
/*    if (dir_x == 0 && dir_y == 0 && bullet[ibull].fuse <= 0)
     destroy_bullet(ibull);
      else
       return 4;
    return 1;*/
    case BULLET_FLAME:
    if (dir_x == 0 && dir_y == 0)
     destroy_bullet(ibull);
      else
      {
       splatter(bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN, 0, COLOUR_GREY1);
       return 2;
      }
    return 1;
    case BULLET_FUNKY_BOMB:
    play_sound_pos(WAV_FUNKY, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2300, -100, trans_colours_array [prand(5)], 15);
    create_cloud(CLOUD_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 2300, -100, 0, 15);
//    shrapnel_burst(ibull, 9, 1, 0, BULLET_FUNKY_BOMBLET);
    blast(bullet[ibull].x, bullet[ibull].y, 5000, bullet[ibull].owner, 100, 30, 5, 5, 1);
    for (fcount = 0; fcount < 9; fcount ++)
    {
     create_bullet(BULLET_FUNKY_BOMBLET, bullet[ibull].x, bullet[ibull].y, bullet_rand(ibull, 1000) - 500, bullet_rand(ibull, 1000) - 500, bullet[ibull].owner, 1, 0, 0, 8, 0, 0, 0, bullet[ibull].seed);
    }
    destroy_bullet(ibull);
    break;
    case BULLET_FUNKY_BOMBLET:
    if ((dir_x != 0 || dir_y != 0) && bullet[ibull].left_owner <= 2)
     return 3;
    play_sound_pos(WAV_FUNKY, 100 + prand(2000), 250, bullet[ibull].x, bullet[ibull].y);
    create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, -100, trans_colours_array [prand(5)], 15);
    create_cloud(CLOUD_RISING_LIGHT, bullet[ibull].x, bullet[ibull].y, 0, 0, 2000, -100, 0, 15);
    blast(bullet[ibull].x, bullet[ibull].y, 4000, bullet[ibull].owner, 400, 30, 5, 5, 1);
    destroy_bullet(ibull);
    break;
    case BULLET_HUNTER_BOMB:
    sparky(bullet[ibull].x, bullet[ibull].y, 0, 0, COLOUR_GREEN4, 1 + prand(5));
    fcount = hunter_acquire(bullet[ibull].x, bullet[ibull].y);
//    create_bullet(BULLET_HUNTER, bullet[ibull].x, bullet[ibull].y, bullet[ibull].x_speed * -1, bullet[ibull].y_speed * -1, bullet[ibull].owner, 350, 200, 0, 8, 0, fcount);

    create_bullet(BULLET_HUNTER, bullet[ibull].x, bullet[ibull].y, prand(200) - 100, prand(200) - 100, bullet[ibull].owner, 350, 200 + prand(100), 8, 8, 0, 1, fcount, bullet[ibull].seed);
    create_bullet(BULLET_HUNTER, bullet[ibull].x, bullet[ibull].y, prand(200) - 100, prand(200) - 100, bullet[ibull].owner, 350, 200 + prand(100), 8, 8, 0, 1, fcount, bullet[ibull].seed + 1);
    create_bullet(BULLET_HUNTER, bullet[ibull].x, bullet[ibull].y, prand(200) - 100, prand(200) - 100, bullet[ibull].owner, 350, 200 + prand(100), 8, 8, 0, 1, fcount, bullet[ibull].seed + 2);
    create_bullet(BULLET_HUNTER, bullet[ibull].x, bullet[ibull].y, prand(200) - 100, prand(200) - 100, bullet[ibull].owner, 350, 200 + prand(100), 8, 8, 0, 1, fcount, bullet[ibull].seed + 3);
    destroy_bullet(ibull);
    break;
    case BULLET_HUNTER:
    if (bullet[ibull].fuse <= 0) //dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_HUNTER, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 10, 96, 10);
     destroy_bullet(ibull);
     return 1;
    }
//    if (game[0].soft_dirt)
//    {
      destroy_dirt(NULL, bullet[ibull].x / GRAIN + prand(3) - 1, bullet[ibull].y / GRAIN + prand(3) - 1, 0);
//    }
    return 3;
    case BULLET_DISRUPTER:
    play_sound_pos(WAV_RECTBANG, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    blast(bullet[ibull].x, bullet[ibull].y, 8000, bullet[ibull].owner, 500, 80, 0, 20, 0);
    create_bullet(BULLET_DISRUPTER_BANG, bullet[ibull].x, bullet[ibull].y, 0, 0, bullet[ibull].owner, -1, 30, 0, 8, 0, 1, 0, bullet[ibull].seed);
    destroy_bullet(ibull);
    break;
    case BULLET_DISRUPTER_BANG:
    if (dir_x == 0 && dir_y == 0)
     destroy_bullet(ibull);
      else
       return 2;
    return 1;
    case BULLET_SPLINTER:
    destroy_bullet(ibull);
    return 1;
    case BULLET_FLESH:
    case BULLET_BLOOD:
//    if (dir_x == 0 && dir_y == 0)
    splatter(bullet[ibull].x / GRAIN, bullet[ibull].y / GRAIN, 0, bullet[ibull].status);
     destroy_bullet(ibull);
//      else
//       return 2;
    return 1;
    case BULLET_SPARK:
    if (dir_x == 0 && dir_y == 0)
    {
     destroy_bullet(ibull);
     return 1;
    }
    return 0;
    case BULLET_SPECK:
    if ((dir_x == 0 && dir_y == 0) || prand(3) == 0)
    {
     destroy_bullet(ibull);
     return 1;
    }
    return 0;
    case BULLET_TOX_BOMB:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_FUNKY, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     for (fcount = 0; fcount < 30; fcount ++)
     {
//      create_bullet(BULLET_TOXIN, bullet[ibull].x + prand(10 * GRAIN) - (5 * GRAIN), bullet[ibull].y + prand(10 * GRAIN) - (5 * GRAIN),
      create_bullet(BULLET_TOXIN, bullet[ibull].x, bullet[ibull].y,
       bullet_rand(ibull, 400) - 200, bullet_rand(ibull, 400) - 200, bullet[ibull].owner, 70, 150 + bullet_rand(ibull, 55), 0, 10, 0, 0, 0, bullet[ibull].seed);
     }
//     shrapnel_burst(ibull, 40, 50, 100, BULLET_TOXIN);
     create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 10, TRANS_YELLOW, 10);
     destroy_bullet(ibull);
     return 1;
    }
    return 0;
    case BULLET_TOXIN:
    if (dir_x == 0 && dir_y == 0)
    {
     destroy_bullet(ibull);
     return 1;
    }
    return 3;
    case BULLET_BOUNCY:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_BANG1, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     place_explosion(bullet[ibull].x, bullet[ibull].y, ex, ey, 1600, 1);
     shrapnel_burst(ibull, 10, 50, 0, BULLET_SHRAPNEL);
     blast(bullet[ibull].x, bullet[ibull].y, 4000, bullet[ibull].owner, 400, 30, 5, 5, 1);
     destroy_bullet(ibull);
  //   if (serial[0].game_type == SERIAL_SERVER)
  //   {
  //    async_smart_destroy(ibull);
  //   }
     return 1;
    }
    if (serial[0].game_type != SERIAL_CLIENT)
    {
     bullet[ibull].status = hunter_acquire(bullet[ibull].x, bullet[ibull].y);
    }
    return 3;
    case BULLET_GAS_GREN:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_GAS_GREN, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     for (fcount = 0; fcount < 30; fcount ++)
     {
//      fcount = 75 + prand(20);
      create_bullet(BULLET_GAS, bullet[ibull].x, bullet[ibull].y,
       bullet_rand(ibull, 600) - 300, bullet_rand(ibull, 600) - 300, bullet[ibull].owner, 5, 150 + bullet_rand(ibull, 55), 0, 10, 0, 0, 96 + fcount % 5, bullet[ibull].seed);
       // remember: these are also created just from the grenade
     }
     destroy_bullet(ibull);
     return 1;
    }
    return 0;
    case BULLET_GAS:
    if (bullet[ibull].fuse <= 0)
    {
     destroy_bullet(ibull);
     return 1;
    }
    return 3;
    case BULLET_DIRTBOMB:
    if (dir_x == 0 && dir_y == 0)
    {
     play_sound_pos(WAV_FUNKY, 1000, 250, bullet[ibull].x, bullet[ibull].y);
     create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 10, 47, 10);
//     dirt_splodge(bullet[ibull].x + prand(15 * GRAIN) - 15 * GRAIN, bullet[ibull].y + prand(15 * GRAIN) - 15 * GRAIN, 40 + prand(10), -1);
//     dirt_splodge(bullet[ibull].x + prand(15 * GRAIN) - 15 * GRAIN, bullet[ibull].y + prand(15 * GRAIN) - 15 * GRAIN, 40 + prand(10), -1);
//     dirt_splodge(bullet[ibull].x + prand(15 * GRAIN) - 15 * GRAIN, bullet[ibull].y + prand(15 * GRAIN) - 15 * GRAIN, 40 + prand(10), -1);
     dirt_splodge(bullet[ibull].x + bullet_rand(ibull, 16 * GRAIN) - 8 * GRAIN, bullet[ibull].y + bullet_rand(ibull, 16 * GRAIN) - 8 * GRAIN, 10 + bullet_rand(ibull, 10), -1);
/*     dirt_splodge(bullet[ibull].x + prand(15 * GRAIN) - 15 * GRAIN, bullet[ibull].y + prand(15 * GRAIN) - 15 * GRAIN, 20 + prand(10), -1);
     dirt_splodge(bullet[ibull].x + prand(15 * GRAIN) - 15 * GRAIN, bullet[ibull].y + prand(15 * GRAIN) - 15 * GRAIN, 20 + prand(10), -1);
     */
     destroy_bullet(ibull);
     return 1;
    }
    return 0;
    case BULLET_DIRTCLOD:
    play_sound_pos(WAV_FUNKY, 1000, 250, bullet[ibull].x, bullet[ibull].y);
    create_cloud(CLOUD_COL_EXPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, 400, 10, 47, 10);
    dirt_splodge(bullet[ibull].x, bullet[ibull].y, 5 + bullet_rand(ibull, 4), -1);
    destroy_bullet(ibull);
    return 1;
    case BULLET_IMPLODER:
//    play_sound(WAV_RECTBANG);
    blast(bullet[ibull].x, bullet[ibull].y, 1000, bullet[ibull].owner, 20, -30, 10, 20, 0);
    create_bullet(BULLET_IMPLOSION, bullet[ibull].x, bullet[ibull].y, 0, 0, bullet[ibull].owner, -1, 100, 0, 8, 0, 1, 15, bullet[ibull].seed);
    destroy_bullet(ibull);
    break;
    case BULLET_IMPLOSION:
    if (dir_x == 0 && dir_y == 0)
     destroy_bullet(ibull);
      else
       return 2;
    return 1;
    case BULLET_DISRUPTER_WAVE:
    if (dir_x == 0 && dir_y == 0)
    {
     destroy_bullet(ibull);
     return 1;
    }
    return 0;

   }

   return 1; // bullet destroyed

}

int bullet_rand(int rbull, int max)
{

 init_prand(bullet[rbull].seed);
 unsigned int retval = prand(65535);
 bullet[rbull].seed = retval + 1;
 if (max > 0)
  retval %= max;
   else
    return 0;
 return retval;

}


void play_bsound(int splay)
{

 switch(splay)
 {
  case 0: play_sound(WAV_BANG1); break;
  case 1: play_sound(WAV_BANG2); break;
  case 2: play_sound(WAV_POP); break;
  case 3: play_sound(WAV_RICOCHET); break;
  case 4: play_sound(WAV_RECTBANG2); break;
  case 5: play_sound(WAV_PLASR_BANG); break;

 }


}

void blast_dirt(int bx, int by, int brad, char digging)
{

//  if (serial[0].game_type != SERIAL_LOCAL_ONLY)
//   async_dirt(bx, by, brad, 0);


if (brad >= 30) // this way is faster, even if called 3 times
// but it leaves spots, which makes it inappropriate for eg tunnelling
{
  int i;
  for (i = 0; i < brad; i ++)
  {
   do_circle(dirt_bmp, bx / GRAIN, by / GRAIN, i, 0, destroy_dirt);
  }
  return;
}

 int di, dj;

 if (brad < PREDRAWN_DIRT_CIRCLES)
 {
  for (di = 0; di < (brad * 2) + 3; di ++)
  {
   for (dj = 0; dj < (brad * 2) + 3; dj ++)
   {
    if (digging)
    {
       if (getpixel(circle_bmp [brad], di, dj)) // change to _getpixel
        dig_dirt(dirt_bmp, (bx / GRAIN) - brad + di - 1, (by / GRAIN) - brad + dj - 1, 0);
    }
     else
      {
       if (getpixel(circle_bmp [brad], di, dj)) // change to _getpixel
        destroy_dirt(dirt_bmp, (bx / GRAIN) - brad + di - 1, (by / GRAIN) - brad + dj - 1, 0);
      }
   }
  }
  return;
 }

 circlefill(circle_bmp_misc, brad + 1, brad + 1, brad, 1);
 
 for (di = 0; di < (brad * 2) + 3; di ++)
 {
  for (dj = 0; dj < (brad * 2) + 3; dj ++)
  {
   if (digging)
   {
    if (getpixel(circle_bmp_misc, di, dj)) // change to _getpixel
     dig_dirt(dirt_bmp, (bx / GRAIN) - brad + di - 1, (by / GRAIN) - brad + dj - 1, 0);
    putpixel(circle_bmp_misc, di, dj, 0); // clear it afterwards
   } else
    {
     if (getpixel(circle_bmp_misc, di, dj)) // change to _getpixel
      destroy_dirt(dirt_bmp, (bx / GRAIN) - brad + di - 1, (by / GRAIN) - brad + dj - 1, 0);
     putpixel(circle_bmp_misc, di, dj, 0); // clear it afterwards
    }
  }
 }

 clear_bitmap(circle_bmp_misc);

}


void dirt_splodge(int bx, int by, int brad, int colour)
{

int dcol = 0;

//  if (serial[0].game_type != SERIAL_LOCAL_ONLY)
//   async_dirt(bx, by, brad, colour);


if (brad >= 30) // this way is faster, even if called 3 times
// but it leaves spots, which makes it inappropriate for eg tunnelling
{
  int i;
  dcol = colour;
  if (colour == -1)
   dcol = COLOUR_BROWN3 + prand(2);
  for (i = 0; i < brad; i ++)
  {
   do_circle(dirt_bmp, bx / GRAIN, by / GRAIN, i, dcol, add_dirt);
  }

  return;
}

 int di, dj;

 if (brad < PREDRAWN_DIRT_CIRCLES)
 {
  for (di = 0; di < (brad * 2) + 3; di ++)
  {
   for (dj = 0; dj < (brad * 2) + 3; dj ++)
   {
    if (colour == -1) dcol = COLOUR_BROWN3 + prand(2);
   
    if (getpixel(circle_bmp [brad], di, dj)) // change to _getpixel
     add_dirt(dirt_bmp, (bx / GRAIN) - brad + di - 1, (by / GRAIN) - brad + dj - 1, dcol);
   }
  }
  return;
 }

 circlefill(circle_bmp_misc, brad + 1, brad + 1, brad, 1);
 
 for (di = 0; di < (brad * 2) + 3; di ++)
 {
  for (dj = 0; dj < (brad * 2) + 3; dj ++)
  {
   if (colour == -1) dcol = COLOUR_BROWN3 + prand(2);
   if (getpixel(circle_bmp_misc, di, dj)) // change to _getpixel
    add_dirt(dirt_bmp, (bx / GRAIN) - brad + di - 1, (by / GRAIN) - brad + dj - 1, dcol);
   putpixel(circle_bmp_misc, di, dj, 0); // clear it afterwards
  }
 }



}

// Too slow... doesn't look so great, either
void burn_dirt(int bx, int by, int brad)
{
int i;
 for (i = 0; i < brad; i ++)
 {
  do_circle(dirt_bmp, bx / GRAIN, by / GRAIN, i, 50, blacken_dirt);
 }
}


// Quick & dirty; relies on it being quite difficult for a player to tell
//  just how close they were to an explosion. To increase this confusion,
//  all blast radii should be larger than their visible explosion radius
// The real problem is that all blasts are square. - MAY NOT BE TRUE
void blast(int x_pos, int y_pos, int radius, int owner, int damage, int force, int dirt_dam, int shake, char tracer)
{

int actor_count;

int damage1 = damage;

if (arena[0].dirt_possible > 0 && dirt_dam > 0)
{
 if (game[0].soft_dirt)
 {
  dirt_dam *= 3;
 }
 if (dirt_dam >= 30)
 {
  blast_dirt(x_pos, y_pos, dirt_dam, 0);
  blast_dirt(x_pos + 2 * GRAIN, y_pos, dirt_dam, 0);
  blast_dirt(x_pos, y_pos + 2 * GRAIN, dirt_dam, 0);
 } else blast_dirt(x_pos, y_pos, dirt_dam, 0);
// burn_dirt(x_pos, y_pos, dirt_dam * 4);
}

  shake_arena(shake);

  for (actor_count = 0; actor_count < NO_ACTORS; actor_count ++)
  {
   if (actor[actor_count].aclass == ACLASS_GHOST
    || actor[actor_count].aclass == ACLASS_NONE) continue;

/*   if (actor[actor_count].x >= x_pos - radius
   && actor[actor_count].x <= x_pos + radius
   && actor[actor_count].y >= y_pos - radius
   && actor[actor_count].y <= y_pos + radius)*/
   {
    char dir_x = 1;
    char dir_y = 1;
    int x_dist = abs(actor[actor_count].x - x_pos);
    int y_dist = abs(actor[actor_count].y - y_pos);
    int distance = hypot(x_dist, y_dist);

    if (distance > radius) continue;
    if (distance < actor[actor_count].circle_radius * GRAIN)
     {
      distance = 5 * GRAIN;//actor[actor_count].circle_radius * GRAIN;
     }
      else
      {
       distance -= actor[actor_count].circle_radius * GRAIN;
      }
    if (distance < 5 * GRAIN) distance = 5 * GRAIN;
    if (tracer == 1 && blast_tracer(actor[actor_count].x, actor[actor_count].y, x_pos, y_pos) == 0) continue;
    
    float x_component = (float) x_dist / distance;
    float y_component = (float) y_dist / distance;
    int x_accel = x_component * force;
    int y_accel = y_component * force;

//  textprintf(screen, font, 80, 410, palette_color[random() % 200], "%i, %i", x_dist, y_dist);


    int dam_percent = (distance * 100) / radius;//distance / radius;
    dam_percent = 100 - dam_percent;

    damage = damage1;

    damage *= dam_percent;
    damage /= 100;
//    damage /= distance;
//  textprintf(screen, font, 80, 430, palette_color[random() % 200], "%f, %f", x_component, y_component);
    int how_hurt = hurt_actor(actor_count, owner, damage, 1, 0, 0);
    if (damage > 50 && how_hurt == 1)
     blood_burst(actor_count, 1, 0,
      actor[actor_count].x, actor[actor_count].y, actor[actor_count].x_speed, actor[actor_count].y_speed);

    if (x_pos > actor[actor_count].x) dir_x = -1;
    if (y_pos > actor[actor_count].y) dir_y = -1;
    int x_glob = x_accel * dir_x;
    int y_glob = y_accel * dir_y;
//  textprintf(screen, font, 80, 430, palette_color[random() % 200], "%i, %i", x_glob, y_glob);

    slam_actor(actor_count, x_glob, y_glob, 2000);

   }

  }

 if (game[0].blast_bullets)
 {
  int bc;
 
  for (bc = 0; bc < MAX_BULLETS; bc ++)
  {
   if (bullet[bc].bullet_type == 0) continue;

/*   if (actor[actor_count].x >= x_pos - radius
   && actor[actor_count].x <= x_pos + radius
   && actor[actor_count].y >= y_pos - radius
   && actor[actor_count].y <= y_pos + radius)*/
   {
    char dir_x = 1;
    char dir_y = 1;
    int x_dist = abs(bullet[bc].x - x_pos);
    int y_dist = abs(bullet[bc].y - y_pos);
    int distance = hypot(x_dist, y_dist);

    if (distance > radius) continue;
    if (distance < 5 * GRAIN) distance = 5 * GRAIN;
    if (tracer == 1 && blast_tracer(bullet[bc].x, bullet[bc].y, x_pos, y_pos) == 0) continue;
    
    float x_component = (float) x_dist / distance;
    float y_component = (float) y_dist / distance;
    int x_accel = x_component * force;
    int y_accel = y_component * force;

    int dam_percent = (distance * 100) / radius;//distance / radius;
    dam_percent = 100 - dam_percent;

    damage = damage1;

    damage *= dam_percent;
    damage /= 100;
//    damage /= distance;
//  textprintf(screen, font, 80, 430, palette_color[random() % 200], "%f, %f", x_component, y_component);
//    int how_hurt = hurt_actor(actor_count, owner, damage, 1, 0, 0);
//    if (damage > 50 && how_hurt == 1)
//     blood_burst(actor_count, 1, 0,
//      actor[actor_count].x, actor[actor_count].y, actor[actor_count].x_speed, actor[actor_count].y_speed);

    if (damage >= 20)
    {
     switch(bullet[bc].bullet_type)
     {
      case BULLET_GREN_LAUNCHER:
      case BULLET_ROCKET:
      case BULLET_GRENADE:
      case BULLET_LR_ROCKET:
      case BULLET_SWARM:
      case BULLET_SQUIRM:
      case BULLET_NUKE_M:
      case BULLET_FLAK:
      case BULLET_FRAG:
      case BULLET_FIREBOMB:
      case BULLET_REMOTE_ROCKET:
      case BULLET_REMOTE_ROCKET_C:
      case BULLET_TRACKER:
      case BULLET_HUNTER_BOMB:
      case BULLET_HUNTER:
      case BULLET_FUNKY_BOMB:
      case BULLET_TOX_BOMB:
      case BULLET_BOMB:
      case BULLET_RPG:
      case BULLET_BOUNCY:
      case BULLET_GAS_GREN:
      case BULLET_DIRTCLOD:
      case BULLET_DIRTBOMB:
      case BULLET_SEEKER:
      bullet[bc].fuse = 1;
      break;
     }
    }

    if (x_pos > bullet[bc].x) dir_x = -1;
    if (y_pos > bullet[bc].y) dir_y = -1;
    if (bullet[bc].weight > 0)
    {
     x_accel /= bullet[bc].weight;
     y_accel /= bullet[bc].weight;
    }
    int x_glob = x_accel * dir_x;
    int y_glob = y_accel * dir_y;
//  textprintf(screen, font, 80, 430, palette_color[random() % 200], "%i, %i", x_glob, y_glob);

    if (x_glob > 5000) x_glob = 5000;
    if (x_glob < -5000) x_glob = -5000;
    if (y_glob > 5000) y_glob = 5000;
    if (y_glob < -5000) y_glob = -5000;


//    slam_actor(actor_count, x_glob, y_glob, 2000);
    bullet[bc].x_speed += x_glob * 60;
    bullet[bc].y_speed += y_glob * 60;

   }

  }



 }

}


void check_grapples(int grx, int gry)
{
 int ca = 0;

 for (ca = 0; ca < NO_ACTORS; ca ++)
 {
  if (actor[ca].grapple_anchored == 0) continue;
  if (actor[ca].grapple_x / GRAIN == grx * GRID_WIDTH_X && actor[ca].grapple_y / GRAIN >= (gry - 1) * GRID_WIDTH_Y && actor[ca].grapple_y / GRAIN <= (gry + 0) * GRID_WIDTH_Y)
  actor[ca].grapple_anchored = 0;
//    destroy_grapple(actor[ca].grapple_bullet);
  if (actor[ca].grapple_x / GRAIN == (grx - 1) * GRID_WIDTH_X - 1 && actor[ca].grapple_y / GRAIN >= (gry - 1) * GRID_WIDTH_Y && actor[ca].grapple_y / GRAIN <= (gry + 0) * GRID_WIDTH_Y)
  actor[ca].grapple_anchored = 0;
//    destroy_grapple(actor[ca].grapple_bullet);
  if (actor[ca].grapple_y / GRAIN == gry * GRID_WIDTH_Y && actor[ca].grapple_x / GRAIN >= (grx - 1) * GRID_WIDTH_X && actor[ca].grapple_x / GRAIN <= (grx + 0) * GRID_WIDTH_X)
  actor[ca].grapple_anchored = 0;
//    destroy_grapple(actor[ca].grapple_bullet);
  if (actor[ca].grapple_y / GRAIN == (gry - 1) * GRID_WIDTH_Y - 1 && actor[ca].grapple_x / GRAIN >= (grx - 1) * GRID_WIDTH_X && actor[ca].grapple_x / GRAIN <= (grx + 0) * GRID_WIDTH_X)
  actor[ca].grapple_anchored = 0;
//    destroy_grapple(actor[ca].grapple_bullet);

    
//  if (actor[ca].grapple_x / GRAIN == grx * GRID_WIDTH_X && actor[ca].grapple_y / GRAIN >= (gry + 0) * GRID_WIDTH_Y && actor[ca].grapple_y / GRAIN <= (gry - 1) * GRID_WIDTH_Y)
//    destroy_grapple(actor[ca].grapple_bullet);

/*  if (actor[ca].grapple_x / GRAIN == grx * GRID_WIDTH_X && actor[ca].grapple_y / GRAIN >= gry * GRID_WIDTH_Y && actor[ca].grapple_y / GRAIN >= (gry + 1) * GRID_WIDTH_Y)
    exit(1);
  if (actor[ca].grapple_x / GRAIN == grx * GRID_WIDTH_X && actor[ca].grapple_y / GRAIN >= gry * GRID_WIDTH_Y && actor[ca].grapple_y / GRAIN >= (gry + 1) * GRID_WIDTH_Y)
    exit(1);*/
 }
 
}






// Tests to see if there's a clear path between points 1 and 2.
//  Only blocked by dirt
// See comments for move_bullet
// except that it matters less how accurate this function is.
int blast_tracer(int x1, int y1, int x2, int y2)
{

char dir_x = 0, dir_y = 0;

int move_x = (x2 - x1);
int move_y = (y2 - y1);
if (abs(move_x) <= 5 && abs(move_y) <= 5) return 1;
 // don't bother with very short distances

int loop_count = 0;
//int actor_count = 0;

int step_x = move_x;
int step_y = move_y;

if (step_x == 0)
{
 if (step_y > 0)
  step_y = GRAIN;
   else step_y = -GRAIN;
}
 else
if (step_y == 0)
{
 if (step_x > 0)
  step_x = GRAIN;
   else step_x = -GRAIN;
}
 else
if (abs(step_x) >= abs(step_y))
{
 step_y = (step_y * GRAIN) / abs(step_x);
 step_x = GRAIN * (step_x / abs(step_x));
} else
if (abs(step_y) > abs(step_x))
{
 step_x = (step_x * GRAIN) / abs(step_y);
 step_y = GRAIN * (step_y / abs(step_y));
}

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;

int x = x1;
int y = y1;

while (move_x != 0 || move_y != 0)
{

  if (get_dirt(x / GRAIN, y / GRAIN)) return 0;

//      create_cloud(CLOUD_DEBUG, x, y, 0, 0, 2000, 20, 0);

 x += step_x;
 move_x -= step_x;
 y += step_y;
 move_y -= step_y;

 if (abs(move_y) <= GRAIN / 2) move_y = 0;
 if (abs(move_x) <= GRAIN / 2) move_x = 0;

 loop_count += GRAIN;

} // end of while loop

return 1;

}









int hurt_actor(int hactor, int hurter, int harm, int shielded, int force,
                   int async)
{


actor[hactor].time_since_hurt = 0;
int was_health = actor[hactor].health;

int absorbed = 0;
//int armour = 0;

if (actor[hactor].aclass == ACLASS_PORKBALL) return 0;

if (hactor != hurter && user[actor[hactor].user].status == USTAT_AI)
 annoy_ai(hurter, hactor);

if (shielded)
{
 if (actor[hactor].shield)
 {
  actor[hactor].shield_visible += 3 + harm / 80;
  if (actor[hactor].shield_visible > 20) actor[hactor].shield_visible = 20;
  harm -= prand(50);
  harm -= prand(50);
  harm -= prand(50);
//  harm -= prand(50);
//  harm -= prand(50);
//  harm -= prand(50);
  play_sound_pos(WAV_S_PULSE, 1000, 250, actor[hactor].x, actor[hactor].y);
  if (harm <= 0) return 0;
 }
 if (actor[hactor].armour)
 {
  absorbed = harm / 2;
  harm -= absorbed;
  actor[hactor].armour_pulse = actor[hactor].circle_radius + 1;
  if (harm <= 0) return 0;
 }
}

 //if (serial[0].game_type == SERIAL_CLIENT)
 //{
 // if ((serial[0].who_decides_damage == DAM_SUFFERER
 //       && actor[hactor].player != 0)
 //     || (serial[0].who_decides_damage == DAM_INFLICTOR
 //         && actor[hurter].player != 0))
 // {
 //  async_hurt(hactor, hurter, harm);
 // }
 // return 0;
 //}
 
/*
 if (serial[0].game_type == SERIAL_SERVER)
 {
  if (serial[0].who_decides_damage == DAM_SERVER
   || (serial[0].who_decides_damage == DAM_SUFFERER
       && actor[hactor].player != 0)
   || (serial[0].who_decides_damage == DAM_INFLICTOR
       && actor[hurter].player != 0))
  {
   async_hurt(hactor, hurter, harm);
  }
 }*/
 
 if (serial[0].game_type == SERIAL_SERVER)
 {
  if (serial[0].who_decides_damage != DAM_SERVER
   && async == 0
   && (serial[0].who_decides_damage != DAM_SUFFERER
       || actor[hactor].player == 0)
   && (serial[0].who_decides_damage != DAM_INFLICTOR
       || actor[hurter].player == 0))
  {
   return 0;
//   async_hurt(hactor, hurter, harm);
  }
 }


   if (harm >= actor[hactor].health)
   {
      actor[hactor].health = 0;
      display_actor_health(hactor, was_health);
      actor_die(hactor, hurter);
   }
      else
      {
       display_actor_health(hactor, harm);
       actor[hactor].health -= harm;
      }

   if (options[0].display_damage == 1 && harm >= 10)
    create_cloud(CLOUD_NUMBER, actor[hactor].x, actor[hactor].y - 50, 0, -40, 25, 1, (harm / 10), 0);

      if (actor[hactor].armour != 0)
       return 2;
       
      return 1;

}

void sparky(int x, int y, int xs, int ys, int scol, int amount)
{

 int sc = scol;

 int i;

 for (i = 0; i < amount; i ++)
 {

  if (scol == -1)
  {
   switch(prand(5))
   {
    case 0: sc = COLOUR_GREY8; break;
    case 1: sc = COLOUR_RED4; break;
    case 2: sc = COLOUR_YELLOW4; break;
    case 3: sc = COLOUR_GREEN4; break;
    case 4: sc = COLOUR_BLUE4; break;
   }
  }
  create_bullet(BULLET_SPARK, x, y, xs + prand(300) - 150, ys - prand(450),
   0, -1, 10 + prand(40), 0, 10, 0, 0, sc, 0);
 }

}

void blood_burst(int bactor, int blood_amount, int flesh_amount, int bx, int by, int bspeed_x, int bspeed_y)
{

 int i;
 //int j;

 switch(user[actor[bactor].user].soldier)
 {
  case SOLDIER_WALKER:
  case SOLDIER_CYBORG:
  case SOLDIER_CRUSHER:
   sparky(bx, by, bspeed_x, bspeed_y, COLOUR_BLUE4, blood_amount + flesh_amount * 2 + prand(3));
   return;
  case SOLDIER_SKELETON:
   for (i = 0; i < blood_amount; i ++)
   {
    create_bullet(BULLET_SPLINTER, bx, by, bspeed_x + prand(300) - 150, bspeed_y - prand(450) + 150,
    bactor, -1, 0, 0, 10, 0, 0, COLOUR_GREY8, 0);
   }
   return;
  case SOLDIER_SNAIL:
   for (i = 0; i < blood_amount; i ++)
   {
    create_bullet(BULLET_BLOOD, bx, by, bspeed_x + prand(200) - 100, bspeed_y - prand(350) + 100,
    bactor, -1, 0, 0, 10, 0, 0, COLOUR_GREY4, 0);
   }
//   for (i = 0; i < blood_amount; i ++)
//   {
//    create_bullet(BULLET_BLOOD, bx, by, bspeed_x + prand(300) - 150, bspeed_y - prand(450) + 150,
//    bactor, -1, 0, 0, 10, 0, 0, COLOUR_GREY8, 0);
//   }
   return;
  case SOLDIER_BUG:
   for (i = 0; i < blood_amount; i ++)
   {
    create_bullet(BULLET_BLOOD, bx, by, bspeed_x + prand(300) - 150, bspeed_y - prand(450) + 150,
    bactor, -1, 0, 0, 10, 0, 0, COLOUR_YELLOW4, 0);
   }
   return;
  case SOLDIER_SOLDIER:
  case SOLDIER_SCOUT:
   for (i = 0; i < blood_amount; i ++)
   {
    create_bullet(BULLET_BLOOD, bx, by, bspeed_x + prand(300) - 150, bspeed_y - prand(450) + 150,
    bactor, -1, 0, 0, 10, 0, 0, BLOOD_RED, 0);
   }

   for (i = 0; i < flesh_amount; i ++)
   {
    create_bullet(BULLET_FLESH, bx, by, bspeed_x + prand(300) - 150, bspeed_y - prand(450) + 150,
     bactor, -1, 0, 0, 20, 0, 0, BLOOD_RED, 0);
   }
   return;
  case SOLDIER_DEMON:
      create_cloud(CLOUD_RED_EXPLOSION, bx, by, bspeed_x, bspeed_y, 400 + (blood_amount + flesh_amount * 2) * 250, 10, 0, 10);
      create_cloud(CLOUD_YELLOW_EXPLOSION, bx, by, bspeed_x, bspeed_y, 400 + (blood_amount + flesh_amount * 2) * 250, 10, 0, 30);
      create_cloud(CLOUD_LIGHT, bx, by, bspeed_x, bspeed_y, 400 + (blood_amount + flesh_amount * 2) * 250, 10, 0, 30);
   return;
 }


}

void splatter(int sx, int sy, int amount, int colour)
{

 int i, j, k, l, m;

 for (i = -2; i < 3; i ++)
 {
  for (j = -2; j < 3; j ++)
  {
   if (abs(i) + abs(j) == 4) continue;
   k = get_dirt(i + sx, j + sy);
   if (prand(4) != 0 && k != 0)
   {
    l = get_dirt_colour(i + sx, j + sy);
//    l = COLOUR_GREY2;
    m = ((l / 32) * 32) + (colour % 32);
    change_any_dirt(i + sx, j + sy, m);
   }
  }
 }
/*
   if (get_dirt(i, j))
    change_dirt(i, j, colour);
   if (get_dirt(i + 1, j))
    change_dirt(i + 1, j, colour);
   if (get_dirt(i - 1, j))
    change_dirt(i - 1, j, colour);
   if (get_dirt(i, j + 1))
    change_dirt(i, j + 1, colour);
   if (get_dirt(i, j - 1))
    change_dirt(i, j - 1, colour);*/

}


void shrapnel_burst(int sbull, int amount, int damage, int range, int shrapnel_type)
{

int shraps = 0;
int srange = 0;

 for (shraps = 0; shraps < amount; shraps ++)
 {
   if (range != 0) srange = range + bullet_rand(sbull, 30);
   // need to adjust for is_bullet?
   create_bullet(shrapnel_type, bullet[sbull].x, bullet[sbull].y, bullet_rand(sbull, 3000) - 1500, bullet_rand(sbull, 3000) - 1500, bullet[sbull].owner, damage, srange, 0, 15, 0, 0, 0, bullet[sbull].seed);
 }

}


void bullet_bounce(int bbull, char bounce_x, char bounce_y)
{

 if (bounce_x != 0)
 {

//  if (abs(bounce_x) > 400)
    bullet[bbull].x_speed *= -1; ///= -1; // bounces
//    bullet[bbull].x += bounce_x * GRAIN;
//      else bullet[bbull].x_speed = 0; // travelling too slowly; just stops
  
 }
 
 if (bounce_y != 0)
 {
  bullet[bbull].y_speed *= -1;
//  bullet[bbull].y += bounce_y * GRAIN;
//  textprintf(screen, font, 80, 440, palette_color[random() % 200], "Bounce!");
 }

}

//void place_blood()
// give blood velocity of bullet & player summed?

// This function assumes that clouds will be created in the correct order
void place_explosion(int exp_x, int exp_y, int sx, int sy, int size, int mini_clouds)
{
/*   while (mini_clouds > 0)
   {
        create_cloud(CLOUD_GREY_SMOKE, exp_x - (GRAIN * 10) + ((random() % 20) * GRAIN), exp_y - (GRAIN * 10) + ((random() % 20) * GRAIN), 0, 0, random() % 900, 5);
        mini_clouds --;
   }*/

   create_cloud(CLOUD_GREY_SMOKE, exp_x, exp_y, sx * 2, sy * 2, size, 0, 0, 1);
   create_cloud(CLOUD_RED_EXPLOSION, exp_x, exp_y, sx, sy, size, -150, 0, 30);
   if (mini_clouds == 1)
    create_cloud(CLOUD_ORANGE_EXPLOSION, exp_x, exp_y, sx, sy, size, -150, 0, 60);
   create_cloud(CLOUD_YELLOW_EXPLOSION, exp_x, exp_y, sx, sy, size, -150, 0, 90);
   if (mini_clouds == 1)
    create_cloud(CLOUD_RISING_LIGHT, exp_x, exp_y, sx, sy, size, -150, 0, 30);
}

void place_rocket_trail(int exp_x, int exp_y, int sp_x, int sp_y, int size, char accel, int rbull)
{
/*   while (mini_clouds > 0)
   {
        create_cloud(CLOUD_GREY_SMOKE, exp_x - (GRAIN * 10) + ((random() % 20) * GRAIN), exp_y - (GRAIN * 10) + ((random() % 20) * GRAIN), 0, 0, random() % 900, 5);
        mini_clouds --;
   }*/
   if (accel > 0)
   {
    if (sp_x > 0)
    {
     sp_x *= (accel * -1);
    } else
     {
      sp_x = abs(sp_x) * accel;
     }
    if (sp_y > 0)
    {
     sp_y *= (accel * -1);
    } else
     {
      sp_y = abs(sp_y) * accel;
     }
     sp_x += bullet[rbull].x_speed;
     sp_y += bullet[rbull].y_speed;
   }
    else
    {
     if (sp_x > 0)
     {
      sp_x /= 3;
      sp_x *= -1;
     } else
      {
       sp_x = abs(sp_x) / 3;
      }
     if (sp_y > 0)
     {
      sp_y /= 3;
      sp_y *= -1;
     } else
      {
       sp_y = abs(sp_y) / 3;
      }
    }
//   sp_x /= 3;
//   sp_y /= 3;
   create_cloud(CLOUD_GREY_SMOKE, exp_x, exp_y, sp_x, sp_y - 30, size, 0, 0, 1);
   create_cloud(CLOUD_RED_EXPLOSION, exp_x, exp_y, sp_x, sp_y, size, 20, 0, 10);
   create_cloud(CLOUD_YELLOW_EXPLOSION, exp_x, exp_y, sp_x, sp_y, size, 20, 0, 30);
//   create_cloud(CLOUD_RISING_LIGHT, exp_x, exp_y, sp_x, sp_x, size, 20, 0, 50);
}



void destroy_bullet(int dbull)
{
   if (bullet[dbull].bullet_type == BULLET_GRAPPLE)
   {
    actor[bullet[dbull].owner].grapple_x = 0;
    actor[bullet[dbull].owner].grapple_y = 0;
    actor[bullet[dbull].owner].grapple_bullet = -1;
    actor[bullet[dbull].owner].grapple_anchored = 0;
   }
   bullet[dbull].bullet_type = 0;

}

void destroy_grapple(int dbull)
{
   if (bullet[dbull].bullet_type != BULLET_GRAPPLE) return;
   bullet[dbull].bullet_type = 0;
   if (actor[bullet[dbull].owner].grapple_bullet != dbull)
    return; // Bug!
   actor[bullet[dbull].owner].grapple_x = 0;
   actor[bullet[dbull].owner].grapple_y = 0;
   actor[bullet[dbull].owner].grapple_bullet = -1;
   actor[bullet[dbull].owner].grapple_anchored = 0;
}


void muzzle_flash(int mactor, int shot_type, int centre_x, int centre_y,
     int bull_x, int bull_y, int bull_x2, int bull_y2, int bull_x3, int bull_y3,
     int bspeed_x, int bspeed_y)
{

   int speed_x = bspeed_x / 3 + actor[mactor].x_speed;
   int speed_y = bspeed_y / 3 + actor[mactor].y_speed;
   int speed_x2, speed_y2;

   int fsize = 0, colour = 0;

   switch(shot_type)
   {
   case WPN_NEEDLER:
   case WPN_LMG:
   case WPN_GRAPESHOT:
   case WPN_INCENDIARIES:
//    create_cloud(CLOUD_GREY_SMOKE, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 400, 20, 0, 0);
//    create_cloud(CLOUD_YELLOW2_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 10 + prand(15), 10, 2 + prand(4), 0);
//    create_cloud(CLOUD_YELLOW_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 10 + prand(15), 10, 2 + prand(2), 0);
    create_cloud(CLOUD_GREY_SMOKE, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 800, 20, 0, 0);
//    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 200 + prand(250), -150, TRANS_ORANGE, 90);
    fsize = 300 + prand(300);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -100, TRANS_YELLOW, 150);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -100, 0, 100);
   break;
   case WPN_HMG:
    fsize = 200 + prand(100);
    speed_x2 = bull_x2 + actor[mactor].x_speed;
    speed_y2 = bull_y2 + actor[mactor].y_speed;
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, TRANS_YELLOW, 150);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, 0, 150);
    speed_x2 = bull_x3 + actor[mactor].x_speed;
    speed_y2 = bull_y3 + actor[mactor].y_speed;
    fsize = 200 + prand(100);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, TRANS_YELLOW, 150);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, 0, 150);
    // intentional fall-through
   case WPN_SCATTER:
   create_cloud(CLOUD_GREY_SMOKE, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 800, 20, 0, 0);
   case WPN_PRONGTHROWER:
    fsize = 300 + prand(300);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_YELLOW, 150);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 150);
   break;
   case WPN_NIBBLER:
    fsize = 200 + prand(100);
    speed_x2 = bull_x2 + actor[mactor].x_speed;
    speed_y2 = bull_y2 + actor[mactor].y_speed;
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, TRANS_YELLOW, 150);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, 0, 150);
    speed_x2 = bull_x3 + actor[mactor].x_speed;
    speed_y2 = bull_y3 + actor[mactor].y_speed;
    fsize = 200 + prand(100);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, TRANS_YELLOW, 150);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x2 + bull_x, centre_y + bull_y2 + bull_y, speed_x2, speed_y2, fsize, -100, 0, 150);
    create_cloud(CLOUD_GREY_SMOKE, centre_x + bull_x, centre_y + bull_y * 2, speed_x, speed_y, 300, 20, 0, 0);
//    create_cloud(CLOUD_YELLOW2_FLASH, centre_x + bull_x2 * 2, centre_y + bull_y2 * 2, speed_x, speed_y, 5 + prand(10), 30, prand(3), 0);
//    create_cloud(CLOUD_YELLOW_FLASH, centre_x + bull_x2 * 2, centre_y + bull_y2 * 2, speed_x, speed_y, 5 + prand(10), 30, prand(3), 0);
//    create_cloud(CLOUD_YELLOW2_FLASH, centre_x + bull_x3 * 2, centre_y + bull_y3 * 2, speed_x, speed_y, 5 + prand(10), 30, prand(3), 0);
//    create_cloud(CLOUD_YELLOW_FLASH, centre_x + bull_x3 * 2, centre_y + bull_y3 * 2, speed_x, speed_y, 5 + prand(10), 30, prand(3), 0);
   break;
   case WPN_SGUN:
   case WPN_SLUG:
   case WPN_AUTOCANNON:
   case WPN_FLAKKER:
   case WPN_GR_L:
   case WPN_GREN:
   case WPN_FRAG:
   case WPN_FIREBOMB:
   case WPN_SEMI_AUTO:
   case WPN_BOMB:
   case WPN_TOXIN:
   case WPN_BOUNCY:
    fsize = 400 + prand(400);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_YELLOW, 100);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 100);
   break;
   case WPN_BLUNDER:
//int create_cloud(int cloud_type, int cloud_x, int cloud_y,
//int cloud_x_speed, int cloud_y_speed, int cloud_size, int cloud_rate,
//int cloud_state, int cloud_rate_change)
    fsize = 500 + prand(500);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_YELLOW, 100);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 100);
//    create_cloud(CLOUD_GREY_SMOKE, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 800, 20, 0, 0);
//    create_cloud(CLOUD_ORANGE_EXPLOSION, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 500 + prand(500), -150, 0, 100);
//    create_cloud(CLOUD_YELLOW_EXPLOSION, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 500 + prand(400), -150, 0, 100);
//    create_cloud(CLOUD_GREY_SMOKE, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 800, 20, 0, 0);
//    create_cloud(CLOUD_YELLOW2_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 15 + prand(20), 10, 5 + prand(6), 0);
//    create_cloud(CLOUD_YELLOW_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 15 + prand(20), 10, 4 + prand(3), 0);
   break;
   case WPN_SMITER:
   case WPN_PLAS_C:
   case WPN_FIREBALL:
//    create_cloud(CLOUD_RED_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 800 + prand(30), 30, 0);
//    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 20 + prand(30), 10, TRANS_RED, 0);
//    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 20 + prand(30), 10, TRANS_YELLOW, 0);
//    create_cloud(CLOUD_YELLOW2_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 15 + prand(20), 10, 5 + prand(6));
    fsize = 500 + prand(500);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_RED, 80);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_YELLOW, 110);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 80);
   break;
   case WPN_PLAS_R:
//    create_cloud(CLOUD_LBLUE_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 800 + prand(30), 30, 0);
    fsize = 400 + prand(400);
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_BLUE, 110);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 110);
//    create_cloud(CLOUD_BLUE_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 800 + prand(30), 40, 0);
   break;
   case WPN_RECTIFIER:
//    create_cloud(CLOUD_LGREEN_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 1100 + prand(30), 30, 0);
    fsize = 1000;
    create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_GREEN, 110);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 110);
//    create_cloud(CLOUD_GREEN_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 40 + prand(40), 10, 5 + prand(6), 0);
//    create_cloud(CLOUD_GREEN_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 1100 + prand(30), 40, 0);
   break;
   case WPN_PLINKER:
   fsize = 400 + prand(500);
   create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_BLUE, 110);
   create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 110);
   break;
   case WPN_CUBE:
//    create_cloud(CLOUD_LBLUE_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 800 + prand(30), 30, 0);
    fsize = 400 + prand(400);
    colour = trans_colours_array [prand(6)];
    create_cloud(CLOUD_CUBE, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, colour, 110);
    create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 110);
//    create_cloud(CLOUD_BLUE_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, 0, 0, 800 + prand(30), 40, 0);
   break;
   case WPN_LASER_BEAM:
//   create_cloud(CLOUD_RED_FLASH, centre_x + bull_x, centre_y + bull_y, 0, 0, 10 + prand(20), 10, 3 + prand(3));
   place_light(centre_x + bull_x * 2, centre_y + bull_y * 2, 30 + prand(20));
   break;
   case WPN_LASER_PULSE:
   create_cloud(CLOUD_BLUE_FLASH, centre_x + bull_x, centre_y + bull_y, 0, 0, 10 + prand(20), 10, 3 + prand(3), 0);
   place_light(centre_x + bull_x * 2, centre_y + bull_y * 2, 30 + prand(20));
   break;
   case WPN_ELECTRO:
//   fsize = 400 + prand(400);
//   create_cloud(CLOUD_COL_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, TRANS_BLUE, 110);
//   create_cloud(CLOUD_BIGLIGHT, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, fsize, -150, 0, 110);
   sparky(centre_x + bull_x * 2, centre_y + bull_y, 0, 0, COLOUR_GREY8, 2 + prand(5));
   break;
   }



//   create_cloud(CLOUD_YELLOW_FLASH, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 50, 20, 2 + prand(4));
//   create_cloud(CLOUD_RED_EXPLOSION, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 500, 20, 0);
//   create_cloud(CLOUD_YELLOW_EXPLOSION, centre_x + bull_x * 2, centre_y + bull_y * 2, speed_x, speed_y, 300, 10, 0);


//   create_cloud(CLOUD_RED_EXPLOSION, centre_x + bull_x, centre_y + bull_y, 0, 0, 500, 20);
   
}

void turn_directly(int tbull, int tactor)
{

 if (actor[tactor].x > bullet[tbull].x) bullet[tbull].facing = 1;
  else
   bullet[tbull].facing = -1;

 float angle_wanted = 0;
// char turning = 0;

 if (actor[tactor].x == bullet[tbull].x)
  angle_wanted = PI / 2;
   else
    angle_wanted = atan((float) ((float) actor[tactor].y - bullet[tbull].y) / ((float) actor[tactor].x - bullet[tbull].x));

 if (bullet[tbull].facing == -1)
  angle_wanted *= -1;
  
// bullet[tbull].angle = angle_wanted;
 bullet[tbull].base_angle = (angle_wanted + (PI / 2)) * (128 / PI);
 bullet[tbull].angle = ((float) bullet[tbull].base_angle / 128 * PI) - (PI / 2);
 

}


void turn_tracker(int tbull, int tx, int ty, float turn_speed)
{

 float angle_wanted = 0;
// float bullet_angle = 0;
// char facing_wanted = 0;
 char turning = 0;

 if (tx == bullet[tbull].x)
  angle_wanted = PI / 2;
   else
    angle_wanted = atan((float) ((float) ty - bullet[tbull].y) / ((float) tx - bullet[tbull].x));

 if (bullet[tbull].facing == 1)
 {
  if (tx >= bullet[tbull].x)
  {
   if (angle_wanted > bullet[tbull].angle) turning = 1;
    else turning = -1;
  } else
        {
         if (angle_wanted < bullet[tbull].angle) turning = 1;
           else turning = -1;
        }

 } // end facing == 1
  else
 {
  angle_wanted *= -1;
 
  if (tx <= bullet[tbull].x)
  {
   if (angle_wanted > bullet[tbull].angle) turning = 1;
    else turning = -1;
//     return;
  } else
        {
         if (angle_wanted < bullet[tbull].angle) turning = 1;
           else turning = -1;
        }

 }

 if (turning > 0)
 {
    bullet[tbull].angle += turn_speed;
    if (bullet[tbull].angle > PI / 2)
    {
     bullet[tbull].facing *= -1;
     bullet[tbull].angle = (PI / 2);// + bullet[tbull].angle + PI / 2;
    }
 } else
 {
    bullet[tbull].angle -= turn_speed;
    if (bullet[tbull].angle < PI / -2)
    {
     bullet[tbull].facing *= -1;
     bullet[tbull].angle = (PI / -2);// + bullet[tbull].angle + PI / 2;
    }
 }

 bullet[tbull].base_angle = (bullet[tbull].angle + (PI / 2)) * (128 / PI);
 bullet[tbull].angle = ((float) bullet[tbull].base_angle / 128 * PI) - (PI / 2);
 

}


int hunter_acquire(int hx, int hy)
{
  // Quite happy to go for whoever fired them
  // note: cloak offers no protection

 int i = 0;
 int closest = -1;
 int distance = 50000;
 int curr_dist = 0;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].aclass != ACLASS_PLAYER) continue;
  
  curr_dist = (abs(actor[i].x - hx) / GRAIN) + (abs(actor[i].y - hy) / GRAIN);
  if (curr_dist < distance)// && blast_tracer(hx, hy, actor[i].x, actor[i].y))
  {
   distance = curr_dist;
   closest = i;
  }
 }
 
 return closest;

}


// x/y_slam are speeds of the bullet/force of the blast, so we can't
//  apply them directly
void slam_actor(int sactor, int x_slam, int y_slam, int weight)
{
 if (actor[sactor].aclass == ACLASS_NONE
  || actor[sactor].aclass == ACLASS_GHOST) return;

 char grounded = actor_grounded(sactor, 1);

 long x_accel = x_slam * weight;
 long y_accel = y_slam * weight;

 x_accel /= actor[sactor].inertia;
 y_accel /= actor[sactor].inertia;

 if (grounded)
 {
         if (abs(x_accel) + abs(y_accel) < 50)
          return;
         actor[sactor].leg_action = LA_SLAM_JUMP;
//         move_x = actor[sactor].x_speed;
//         move_y = -1000;
         actor[sactor].y_speed = -100;
//         if (actor_grounded(mactor)) actor[mactor].y_speed = -520;
//          else actor[mactor].y_speed -= 400;
//         walking = 1;
 }
 
 actor[sactor].x_speed += x_accel;
 actor[sactor].y_speed += y_accel;


 
// if (

}


void actor_die(int dactor, int cause)
{
    int wt;
    char kill_msg [30];
//    char itstring [10];

    strcpy(kill_msg, "");

    if (cause >= 0) // -ve means natural causes
    {
     if (cause == dactor) score_event(dactor, SCORE_SUICIDE, 0);
      else score_event(cause, SCORE_KILL, dactor);
    }

 //   if (serial[0].game_type == SERIAL_SERVER)
 //   {
 //    async_kill(dactor, cause);
 //   }

//    if (game[0].lives_each > 0)
//    {
     lose_life(dactor);
//     user[actor[dactor].user].lives --;
//     update_score();
     // need to make a lives_lost func in score.c so it can use game_over
//    }
    if (cause == dactor)
    {
     strcpy(kill_msg, user_name_plus(actor[dactor].user));
     strcat(kill_msg, " commits suicide.");
     if (arena[0].teams == 1 && user[actor[dactor].user].team != TEAM_NONE)
      message(-1, kill_msg, team_colours(user[actor[dactor].user].team));
       else
        message(-1, kill_msg, COLOUR_GREY8);
    } else
    if (cause >= 0) // -ve means natural causes
    {
     strcpy(kill_msg, user_name_plus(actor[cause].user));
     strcat(kill_msg, " kills ");
     strcat(kill_msg, user_name_plus(actor[dactor].user));
     strcat(kill_msg, ".");
     if (arena[0].teams == 1 && user[actor[cause].user].team != TEAM_NONE)
      message(-1, kill_msg, team_colours(user[actor[cause].user].team));
       else
        message(-1, kill_msg, COLOUR_GREY8);
    } else
    {
     strcpy(kill_msg, user_name_plus(actor[dactor].user));
     strcat(kill_msg, " dies.");
     if (arena[0].teams == 1 && user[actor[dactor].user].team != TEAM_NONE)
      message(-1, kill_msg, team_colours(user[actor[dactor].user].team));
       else
        message(-1, kill_msg, COLOUR_GREY8);
    }

    int i = 0;

    for (i = 0; i < MAX_BULLETS; i ++)
    {
     if (bullet[i].bullet_type == BULLET_GRAPPLE
         && bullet[i].owner == dactor)
         {
          destroy_grapple(i);
         }
     if (bullet[i].bullet_type != BULLET_HUNTER
      && bullet[i].bullet_type != BULLET_TRACKER
      && bullet[i].bullet_type != BULLET_SEEKER)
       continue;
     if (bullet[i].status == dactor)
      bullet[i].status = -1;
    }

//    place_explosion(actor[dactor].x, actor[dactor].y, 1500, 5);
    switch(user[actor[dactor].user].soldier)
    {
     case SOLDIER_BUG:
     create_cloud(CLOUD_BLOOD, actor[dactor].x, actor[dactor].y, 0, 0, 1500, 20, COLOUR_YELLOW4, 3);
     break;
     case SOLDIER_SNAIL:
     create_cloud(CLOUD_BLOOD, actor[dactor].x, actor[dactor].y, 0, 0, 2500, 20, COLOUR_GREY4, 10);
     break;
     case SOLDIER_SCOUT:
     case SOLDIER_SOLDIER:
     create_cloud(CLOUD_RED_BLOOD, actor[dactor].x, actor[dactor].y, 0, 0, 800, 20, COLOUR_RED4, 3);
     break;
     case SOLDIER_WALKER:
     place_explosion(actor[dactor].x, actor[dactor].y, 0, 0, 2500, 5);
     create_cloud(CLOUD_COL_EXPLOSION, actor[dactor].x + prand(20 * GRAIN) - (10 * GRAIN), actor[dactor].y + prand(40 * GRAIN) - (20 * GRAIN), prand(6 * GRAIN) - 3 * GRAIN, prand(6 * GRAIN) - 3 * GRAIN, 600 + prand(550), 5, TRANS_BLUE, 10);
     create_cloud(CLOUD_COL_EXPLOSION, actor[dactor].x + prand(20 * GRAIN) - (10 * GRAIN), actor[dactor].y + prand(40 * GRAIN) - (20 * GRAIN), prand(6 * GRAIN) - 3 * GRAIN, prand(6 * GRAIN) - 3 * GRAIN, 600 + prand(550), 5, TRANS_BLUE, 10);
     create_cloud(CLOUD_COL_EXPLOSION, actor[dactor].x + prand(20 * GRAIN) - (10 * GRAIN), actor[dactor].y + prand(40 * GRAIN) - (20 * GRAIN), prand(6 * GRAIN) - 3 * GRAIN, prand(6 * GRAIN) - 3 * GRAIN, 600 + prand(550), 5, TRANS_BLUE, 10);
     break;
     case SOLDIER_CRUSHER:
     place_explosion(actor[dactor].x, actor[dactor].y, 0, 0, 2000, 5);
     create_cloud(CLOUD_COL_EXPLOSION, actor[dactor].x + prand(20 * GRAIN) - (10 * GRAIN), actor[dactor].y + prand(40 * GRAIN) - (20 * GRAIN), prand(6 * GRAIN) - 3 * GRAIN, prand(6 * GRAIN) - 3 * GRAIN, 600 + prand(550), 5, TRANS_BLUE, 10);
     create_cloud(CLOUD_COL_EXPLOSION, actor[dactor].x + prand(20 * GRAIN) - (10 * GRAIN), actor[dactor].y + prand(40 * GRAIN) - (20 * GRAIN), prand(6 * GRAIN) - 3 * GRAIN, prand(6 * GRAIN) - 3 * GRAIN, 600 + prand(550), 5, TRANS_BLUE, 10);
     break;
     case SOLDIER_CYBORG:
     place_explosion(actor[dactor].x, actor[dactor].y, 0, 0, 1500, 5);
     create_cloud(CLOUD_COL_EXPLOSION, actor[dactor].x + prand(20 * GRAIN) - (10 * GRAIN), actor[dactor].y + prand(40 * GRAIN) - (20 * GRAIN), prand(6 * GRAIN) - 3 * GRAIN, prand(6 * GRAIN) - 3 * GRAIN, 600 + prand(550), 5, TRANS_BLUE, 10);
     break;
     case SOLDIER_DEMON:
     create_cloud(CLOUD_RED_EXPLOSION, actor[dactor].x, actor[dactor].y, 0, 0, 1500, -100, TRANS_RED, 10);
     create_cloud(CLOUD_ORANGE_EXPLOSION, actor[dactor].x, actor[dactor].y, 0, 0, 1500, -100, TRANS_ORANGE, 20);
     create_cloud(CLOUD_YELLOW_EXPLOSION, actor[dactor].x, actor[dactor].y, 0, 0, 1500, -100, TRANS_YELLOW, 30);
     create_cloud(CLOUD_RISING_LIGHT, actor[dactor].x, actor[dactor].y, 0, 0, 1500, -100, 0, 10);
     break;
    }

    blood_burst(dactor, 10, 4, actor[dactor].x, actor[dactor].y, actor[dactor].x_speed, actor[dactor].y_speed);
    play_sound_pos(WAV_BANG1, 1000, 250, actor[dactor].x, actor[dactor].y);
//    play_sound(WAV_DIE);

    destroy_grapple(actor[dactor].grapple_bullet);

    if (actor[dactor].has_grail == 1)
    {
     create_pickup(PICKUP_GRAIL, 0, actor[dactor].x, actor[dactor].y,
      0, 0, 0, -1, -1);
    }

    if (actor[dactor].has_flag != -1)
    {
     create_pickup(PICKUP_FLAG, actor[dactor].has_flag,
     actor[dactor].x, actor[dactor].y,
      0, 0, 0, -1, -1);
    }

    if (actor[dactor].has_porkball != -1)
    {
     arena[0].who_has_porkball = -1;
    }

    if (user[actor[dactor].user].status == USTAT_AI)
     reset_dead_ai(dactor);

    tell_ai_someone_died(dactor);

    if (player[1].actor_controlled == dactor)
    {
     player[1].slot_selected = 0;
     wt = 1;
    }
    if (player[2].actor_controlled == dactor)
    {
     player[2].slot_selected = 0;
     wt = 2;
    }

    actor[dactor].aclass = ACLASS_GHOST;
    actor[dactor].atype = ATYPE_NONE;
    user[actor[dactor].user].penalty_time = game[0].penalty * 33 - 1;
    user[actor[dactor].user].resting_time = 329;
//    spawn_actor(dactor);

}

/*
Does little because it looked ugly. The score/life thing was added before
 the show_scores key was enabled, and it's pretty much redundant now.
*/
char *user_name_plus(int ui)
{
 static char ustring [40];
// char itstring [10];

 strcpy(ustring, user[ui].uname);
/* strcat(ustring, " (");
 if (arena[0].teams == 1)
  strcat(ustring, itoa(team[user[ui].team].score, itstring, 10));
   else
    strcat(ustring, itoa(user[ui].score, itstring, 10));
 if (game[0].lives_each > 0)
 {
  strcat(ustring, "/");
  strcat(ustring, itoa(user[ui].lives, itstring, 10));
 }
 strcat(ustring, ")");*/

 return ustring;
}


/*
struct bullet_list *bullet;


char create_bullet(int bullet_type, int bullet_x, int bullet_y,
int bullet_x_speed, int bullet_y_speed, unsigned char bullet_owner)
{

   struct bullet_list *newbullet = malloc(sizeof(bullet_list));

   struct bullet_list *bullet_counter;
   bullet_counter->next = bullet;

   while(bullet_counter->next != NULL)
   {
      bullet_counter = bullet_counter->next;

   }
}


char destroy_bullet(bullet_list *bullet)
{

   bullet->prev

} */
