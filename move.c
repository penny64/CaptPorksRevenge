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

File: move.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions for moving actors around and implementing some movement
   commands.
 
*/

#include "allegro.h"

#include <stdlib.h>
#include <math.h>

#include "config.h"

#include "globvar.h"
#include "grid.h"
#include "bullet.h"
#include "sound.h"
#include "prand.h"
#include "cloud.h"

#include "acthead.h"

void actually_move_actor(int mactor, int move_x, int move_y, char walking);
char actor_grounded(int gactor, char wide);
void gravitate(int gractor);
void actor_bounce(int bactor, int bounce_x, int bounce_y, char dipping);
void grapple_movement(int gactor);
void actor_jet(int jactor);
void actor_rocket(int ractor, int super);
int animate_legs(int dactor);
char bump_actor_up(int bactor);


// Passes the motion the actor should be experiencing to actually_move_actor,
// but does not check for obstacles
void move_actor(int mactor)
{

   if (actor[mactor].aclass == ACLASS_PORKBALL)
   {
      actor[mactor].y_speed += game[0].gravity;
      actually_move_actor(mactor, actor[mactor].x_speed, actor[mactor].y_speed, 1);
      return;
   }


   int move_x = 0, move_y = 0;
   char walking = 0;
   char grounded = actor_grounded(mactor, 1);

//   actor[mactor].x_speed = 100;

      if (actor [mactor].leg_action == LA_START_JUMP)
      {
         move_x = actor[mactor].x_speed;
         move_y = actor[mactor].jump_accel;
         if (actor_grounded(mactor, 0)) actor[mactor].y_speed = actor[mactor].jump_accel;
          else actor[mactor].y_speed -= 300; // hanging from grapple
         walking = 1;
         if (move_x > 0) actor[mactor].leg_action = LA_DRIFT_RIGHT;
         if (move_x < 0) actor[mactor].leg_action = LA_DRIFT_LEFT;
      } 

      if (actor [mactor].leg_action == LA_SLAM_JUMP)
      {
         move_x = actor[mactor].x_speed;
         move_y = -100;
         actor[mactor].y_speed = -100;
         walking = 1;
         if (move_x > 0) actor[mactor].leg_action = LA_DRIFT_RIGHT;
         if (move_x < 0) actor[mactor].leg_action = LA_DRIFT_LEFT;
      }

      gravitate(mactor);
      if (grounded)// || actor[mactor].leg_action == LA_CLIMB)
      {
/*       if (actor[mactor].leg_action == LA_CLIMB)
       {
         if (actor[mactor].y_speed > actor[mactor].walk_speed * -1)
         {
          actor[mactor].y_speed = actor[mactor].walk_speed * -1;
         }
         actor[mactor].leg_action = LA_STAND;
         if (actor[mactor].walking < 10)
          actor[mactor].walking ++;
       } else*/
       if (actor[mactor].leg_action == LA_WALK_LEFT)
       {
         if (actor[mactor].x_speed < actor[mactor].walk_speed * -1)
         {
          actor[mactor].x_speed = actor[mactor].walk_speed * -1;
         }
         if (actor[mactor].walking < 10)
          actor[mactor].walking ++;
       } else
       if (actor[mactor].leg_action == LA_WALK_RIGHT)
       {
         if (actor[mactor].x_speed > actor[mactor].walk_speed)
         {
          actor[mactor].x_speed = actor[mactor].walk_speed;
         }
         if (actor[mactor].walking < 10)
          actor[mactor].walking ++;
       }
        else
         actor[mactor].walking = 0;
      }
      
   if (actor[mactor].grapple_anchored == 1)
   {
    grapple_movement(mactor);
    if (grounded) walking = 1;
   }


      
      move_x = actor[mactor].x_speed;
      move_y = actor[mactor].y_speed;
      
      if (actor [mactor].leg_action == LA_DRIFT_LEFT || actor [mactor].leg_action == LA_WALK_LEFT)
      {
//         move_x -= 10;
//         actor[mactor].x_speed = -20;
         if (actor[mactor].walking >= 5 && actor[mactor].x_speed > 0 && actor [mactor].leg_action == LA_WALK_LEFT)
         {
          if (actor[mactor].x_speed > actor[mactor].walk_speed * -1)
          {
           actor[mactor].x_speed = actor[mactor].walk_speed * -1;
           move_x = actor[mactor].walk_speed * -1;
          }
         }// else
           if (move_x > 0)
           {
            move_x -= actor[mactor].walk_accel;
            actor[mactor].x_speed -= actor[mactor].walk_accel;
           }
              else
                if (move_x > actor[mactor].walk_speed * -1)
                {
                 actor[mactor].x_speed -= actor[mactor].walk_accel;
                 move_x = actor[mactor].x_speed;
                 if (actor[mactor].x_speed < actor[mactor].walk_speed * -1) actor[mactor].x_speed = actor[mactor].walk_speed * -1;
                 if (move_x < actor[mactor].walk_speed * -1) move_x = actor[mactor].walk_speed * -1;
                }

      } else
      if (actor [mactor].leg_action == LA_DRIFT_RIGHT || actor [mactor].leg_action == LA_WALK_RIGHT)
      {
//         move_x += 10;
//         actor[mactor].x_speed = 20;
         if (actor[mactor].walking >= 5 && actor[mactor].x_speed < 0 && actor [mactor].leg_action == LA_WALK_RIGHT)
         {
          if (actor[mactor].x_speed < actor[mactor].walk_speed)
          {
           actor[mactor].x_speed = actor[mactor].walk_speed;
           move_x = actor[mactor].walk_speed;
          }
         } //else
           if (move_x < 0)
           {
            move_x += actor[mactor].walk_accel;
            actor[mactor].x_speed += actor[mactor].walk_accel;
           }
              else
                if (move_x < actor[mactor].walk_speed)
                {
                 actor[mactor].x_speed += actor[mactor].walk_accel;
                 move_x = actor[mactor].x_speed;
                 if (actor[mactor].x_speed > actor[mactor].walk_speed) actor[mactor].x_speed = actor[mactor].walk_speed;
                 if (move_x > actor[mactor].walk_speed) move_x = actor[mactor].walk_speed;
                }

      } else
      {
       if (grounded) actor[mactor].x_speed = 0;
      }
      

  actually_move_actor(mactor, move_x, move_y, walking);

  actor[mactor].leg_frame = animate_legs(mactor);

  actor[mactor].total_dip = actor[mactor].walk_bounce [actor[mactor].leg_frame - P1_WALK_1];
  if (actor[mactor].dip > 3)
   actor[mactor].total_dip += 3;
     else
      actor[mactor].total_dip += actor[mactor].dip;
  
//  if (actor[mactor].leg_action == LA_CLIMB) actor[mactor].y_speed = 0;

}



// Moves the actor on a dirt level, checking for obstacles
void actually_move_actor(int mactor, int move_x, int move_y, char walking)
{

char dir_x = 0;
char dir_y = 0;

int i;

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;

char ground = actor_grounded(mactor, 0);

while (move_x != 0 || move_y != 0)
{

 if (dir_y == 10)
 {
  for (i = (actor[mactor].x / GRAIN) - actor[mactor].width; i < (actor[mactor].x / GRAIN) + actor[mactor].width; i++)
  {
   if (get_dirt(i, (actor[mactor].y / GRAIN) + actor[mactor].height + 1))
   {
       move_y = 0;
       actor_bounce(mactor, 0, actor[mactor].y_speed, 1);
//       dirt_found++;
//       last_dirt = i;
   }
  }
/*   if (dirt_found == 1 && actor[mactor].leg_action != LA_START_JUMP)
   {
    change_dirt(NULL, last_dirt, (actor[mactor].y / GRAIN) + actor[mactor].height + 1, 0);
   }*/
 }

// dirt_found = 0;

 if (dir_y == -10)
 {
  for (i = (actor[mactor].x / GRAIN) - actor[mactor].width; i < (actor[mactor].x / GRAIN) + actor[mactor].width; i++)
  {
   if (get_dirt(i, (actor[mactor].y / GRAIN) - actor[mactor].height - 1))
   {
       move_y = 0;
       actor_bounce(mactor, 0, actor[mactor].y_speed, 0);
//       dirt_found++;
//       last_dirt = i;
   }
  }
/*   if (dirt_found == 1 && actor[mactor].leg_action != LA_START_JUMP)
   {
    change_dirt(NULL, last_dirt, (actor[mactor].y / GRAIN) - actor[mactor].height - 1, 0);
   }*/
 }

// dirt_found = 0;

 if (dir_x == -10)
 {
  if (ground == 1)
  {
   if (!get_dirt((actor[mactor].x / GRAIN) - actor[mactor].width - 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 6)
      && (get_dirt((actor[mactor].x / GRAIN) - actor[mactor].width - 2, (actor[mactor].y / GRAIN) + actor[mactor].height)
      || get_dirt((actor[mactor].x / GRAIN) - actor[mactor].width - 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 1)
      || get_dirt((actor[mactor].x / GRAIN) - actor[mactor].width - 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 2)
      || get_dirt((actor[mactor].x / GRAIN) - actor[mactor].width - 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 3)))
   {
//    if (!bump_actor_up(mactor) || actor[mactor].x_speed == 0)
    {
     actor[mactor].y_speed = -150;
     actor [mactor].leg_action = LA_START_JUMP;
     bump_actor_up(mactor);
    }
//    actor [mactor].leg_action = LA_CLIMB;
   }
  }

//  for (i = (actor[mactor].y / GRAIN) - actor[mactor].height - 1; i < (actor[mactor].y / GRAIN) + actor[mactor].height + 1; i++)
  for (i = (actor[mactor].y / GRAIN) - actor[mactor].height; i < (actor[mactor].y / GRAIN) + actor[mactor].height + 1; i++)
  {
   if (get_dirt((actor[mactor].x / GRAIN) - actor[mactor].width - 1, i))
   {
       move_x = 0;
       actor_bounce(mactor, actor[mactor].x_speed, 0, 0);
//       dirt_found++;
//       last_dirt = i;
   }
  }
/*   if (dirt_found == 1 && actor[mactor].leg_action != LA_START_JUMP)
   {
    change_dirt(NULL, (actor[mactor].x / GRAIN) - actor[mactor].width - 1, last_dirt, 0);
   }*/
 }

// dirt_found = 0;

 if (dir_x == 10)
 {
  if (ground == 1)
  {
   if (!get_dirt((actor[mactor].x / GRAIN) + actor[mactor].width + 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 6)
      && (get_dirt((actor[mactor].x / GRAIN) + actor[mactor].width + 2, (actor[mactor].y / GRAIN) + actor[mactor].height)
      || get_dirt((actor[mactor].x / GRAIN) + actor[mactor].width + 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 1)
      || get_dirt((actor[mactor].x / GRAIN) + actor[mactor].width + 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 2)
      || get_dirt((actor[mactor].x / GRAIN) + actor[mactor].width + 2, (actor[mactor].y / GRAIN) + actor[mactor].height - 3)))
   {
//    if (!bump_actor_up(mactor) || actor[mactor].x_speed == 0)
    {
     actor[mactor].y_speed = -150;
     actor [mactor].leg_action = LA_START_JUMP;
     bump_actor_up(mactor);
    }
//    actor [mactor].leg_action = LA_CLIMB;
   }
  }
  
//  for (i = (actor[mactor].y / GRAIN) - actor[mactor].height - 1; i < (actor[mactor].y / GRAIN) + actor[mactor].height + 1; i++)
  for (i = (actor[mactor].y / GRAIN) - actor[mactor].height; i < (actor[mactor].y / GRAIN) + actor[mactor].height + 1; i++)
  {
   if (get_dirt((actor[mactor].x / GRAIN) + actor[mactor].width + 1, i))
   {
       move_x = 0;
       actor_bounce(mactor, actor[mactor].x_speed, 0, 0);
//       dirt_found++;
//       last_dirt = i;
   }
  }
/*   if (dirt_found == 1 && actor[mactor].leg_action != LA_START_JUMP)
   {
    change_dirt(NULL, (actor[mactor].x / GRAIN) + actor[mactor].width + 1, last_dirt, 0);
   }*/
 }

 if (move_x >= GRAIN)
 {
  actor[mactor].x += GRAIN;
  move_x -= GRAIN;
 } else
 {
  if (move_x <= -GRAIN)
  {
    actor[mactor].x -= GRAIN;
    move_x += GRAIN;
  } else
   {
    actor[mactor].x += move_x;
    move_x = 0;
   }
 }

 if (move_y >= GRAIN)
 {
  actor[mactor].y += GRAIN;
  move_y -= GRAIN;
 } else
 {
  if (move_y <= -GRAIN)
  {
    actor[mactor].y -= GRAIN;
    move_y += GRAIN;
  } else
   {
    actor[mactor].y += move_y;
    move_y = 0;
   }
 }

}

}


char bump_actor_up(int bactor)
{

  int i;

  for (i = (actor[bactor].x / GRAIN) - actor[bactor].width; i < (actor[bactor].x / GRAIN) + actor[bactor].width; i++)
  {
   if (get_dirt(i, (actor[bactor].y / GRAIN) - actor[bactor].height - 1)
       || get_dirt(i, (actor[bactor].y / GRAIN) - actor[bactor].height - 2))
   {
       return 0;
   }
  }

  actor[bactor].y -= GRAIN * 2;
  return 1;
}


// bounce_x or y indicate the direction of the thing bactor hit
void actor_bounce(int bactor, int bounce_x, int bounce_y, char dipping)
{

 if (bounce_y > 300 && dipping == 1)// && actor[bactor].y_speed > 400)
 {
  actor[bactor].dip += bounce_y / 100;
  if (actor[bactor].dip > 15)
   actor[bactor].dip = 15;
 }

 bounce_x = abs(bounce_x);
 bounce_y = abs(bounce_y);


 if (actor[bactor].aclass != ACLASS_PORKBALL)
 {
  if (game[0].impact_damage > 0 && (user[actor[bactor].user].status != USTAT_AI || arena[0].ai_no_impact == 0))
  {
//  int hurt_speed = hypot(abs(bounce_x), abs(bounce_y));
// That's the proper way to do it. But currently can assume that one of the
//  two bounce_s will be 0:
   int hurt_speed = bounce_x + bounce_y;
   hurt_speed -= 800 - game[0].impact_damage * 10;
   if (hurt_speed > 0)
   {
    hurt_actor(bactor, bactor, hurt_speed * game[0].impact_damage / 50, 1, 0, 0);
    play_sound_pos(WAV_POP, 1000, 250, actor[bactor].x, actor[bactor].y);
//    play_sound(WAV_POP); // need a better sound
    create_cloud(CLOUD_RED_BLOOD, actor[bactor].x, actor[bactor].y, 0, 0, 300, 40, 0, 0);
   }
  }
 }
  else
  {
   if (bounce_x > 0)
   {
    if (bounce_x > 200) //play_sound(WAV_POP);
     play_sound_pos(WAV_POP, 1000, 250, actor[bactor].x, actor[bactor].y);

    if (bounce_x > 10) // change to x_accel
    {
      actor[bactor].x_speed *= -3; // bounces
      actor[bactor].x_speed /= 4;
    }
        else actor[bactor].x_speed = 0; // travelling too slowly; just stops
   }

   if (bounce_y > 0)
   {
    if (bounce_y > 200) //play_sound(WAV_POP);
     play_sound_pos(WAV_POP, 1000, 250, actor[bactor].x, actor[bactor].y);
    actor[bactor].y_speed *= -3;
    actor[bactor].y_speed /= 4;
   }
   return;
  }

 if (bounce_x > 0)
 {
  if (bounce_x > 800) //play_sound(WAV_POP);
    play_sound_pos(WAV_POP, 1000, 250, actor[bactor].x, actor[bactor].y);

  if (bounce_x > 400) // change to x_accel
    actor[bactor].x_speed /= -2; // bounces
      else actor[bactor].x_speed = 0; // travelling too slowly; just stops
  
 }

 if (bounce_y > 0)
 {
  if (bounce_y > 800) //play_sound(WAV_POP);
    play_sound_pos(WAV_POP, 1000, 250, actor[bactor].x, actor[bactor].y);
  actor[bactor].y_speed /= -2;
 }

}

void walk_actor(int wactor, char wdir)
{

   actor[wactor].facing = wdir;

   if (actor_grounded(wactor, 1))
   {
//     if (actor[wactor].leg_action != LA_CLIMB)
//     {
      if (wdir == 1)
       actor [wactor].leg_action = LA_WALK_RIGHT;
            else actor [wactor].leg_action = LA_WALK_LEFT;
//     }

            actor[wactor].leg_anim_state += actor[wactor].leg_anim_speed;
            if (actor[wactor].leg_anim_state > 99) actor [wactor].leg_anim_state = 0;
      return;
   }

//     if (actor[wactor].leg_action != LA_CLIMB)
//     {
      if (wdir == 1) actor [wactor].leg_action = LA_DRIFT_RIGHT;
       else actor [wactor].leg_action = LA_DRIFT_LEFT;
//     }
            
   return;

}


char actor_jump(int jactor, char force_jump)
{
//   int grounded = actor_grounded(jactor, 1);
   int grounded = actor_grounded(jactor, 0);
   if (actor[jactor].prevent_jump == 1) return 0;
   if (grounded == 0)
   {
    if (actor[jactor].jet_on) actor_jet(jactor);
    if (actor[jactor].propulsion == PROP_ROCKETS)
    {
     if (actor[jactor].dip != 0) actor[jactor].dip = 1;
     actor_rocket(jactor, 0);
     return 1;
    }
    if (force_jump == 0) return 0;
   }

     actor[jactor].leg_action = LA_START_JUMP;
     play_sound_pos(WAV_JUMP, 1000, 250, actor[jactor].x, actor[jactor].y);
//     play_sound(WAV_JUMP);
     actor[jactor].jump_delay = 20;
     if (actor[jactor].dip != 0) actor[jactor].dip = 1;

     return 1;

}


void actor_jet(int jactor)
{

 if (actor[jactor].aclass != ACLASS_PLAYER)
  return;

 int jpower = actor[jactor].jet_power * 2;

 if (actor[jactor].jet_heat > 3000) return;
 actor[jactor].jet_heat += 10;// - actor[jactor].jet_speed;
// if (actor[jactor].jet_heat > 500) jpower -= 5;
// if (actor[jactor].jet_heat > 1000) jpower -= 10;
 if (actor[jactor].jet_heat > 2200) jpower -= 10;

 actor[jactor].jet_spin += actor[jactor].jet_speed;
 if (actor[jactor].jet_spin > actor[jactor].jet_power * 2)
  actor[jactor].jet_spin = actor[jactor].jet_power * 2;
 
 if (jpower > actor[jactor].jet_spin) jpower = actor[jactor].jet_spin;

 jpower /= 2;

 actor[jactor].y_speed -= jpower;

 if (arena[0].counter % 6 == 0)
//  play_sound2(WAV_JET, 100 + (actor[jactor].jet_spin * 5) + rand() % 20, 250, 127);
  play_sound_pos(WAV_JET, 100 + (actor[jactor].jet_spin * 5) + rand() % 20, 250, actor[jactor].x, actor[jactor].y);

// actor [actor_number].jet_power = 30;
// actor [actor_number].jet_heat = 0;
// actor [actor_number].jet_speed = 0;
/*   if (prand(2) == 0)
    create_cloud(CLOUD_COL_SMOKE, actor[jactor].x + (-4 * GRAIN * actor[jactor].facing), actor[jactor].y + 4 * GRAIN,
     actor[jactor].x_speed, actor[jactor].y_speed + jpower * 20, 200 + prand(500), 20,
     17 + actor[jactor].jet_heat / 230);*/

   if (prand(2) == 0)
    create_cloud(CLOUD_GREY_SMOKE, actor[jactor].x + (-4 * GRAIN * actor[jactor].facing), actor[jactor].y + 4 * GRAIN,
     actor[jactor].x_speed, actor[jactor].y_speed + jpower * 20, 100 + prand(200), -100,
     TRANS_WHITE, 20);
 actor[jactor].jetting = 1;


}

void actor_rocket(int ractor, int super)
{

 if (actor[ractor].aclass != ACLASS_PLAYER)
  return;

 if (actor[ractor].rocket_charge != 0) return;

 int rpower = actor[ractor].rocket_power;
 if (super == 1) rpower *= 3;

 actor[ractor].y_speed -= rpower;
 play_sound_pos(WAV_PROPROCK, 1000, 250, actor[ractor].x, actor[ractor].y);

 int xsize = 400;

 create_cloud(CLOUD_GREY_SMOKE, actor[ractor].x + (-4 * GRAIN * actor[ractor].facing), actor[ractor].y,
   actor[ractor].x_speed, actor[ractor].y_speed + rpower * 1, xsize, 20, 0, 0);
 create_cloud(CLOUD_RED_EXPLOSION, actor[ractor].x + (-4 * GRAIN * actor[ractor].facing), actor[ractor].y,
   actor[ractor].x_speed, actor[ractor].y_speed + rpower * 1, xsize, 20, 0, 10);
 create_cloud(CLOUD_ORANGE_EXPLOSION, actor[ractor].x + (-4 * GRAIN * actor[ractor].facing), actor[ractor].y,
   actor[ractor].x_speed, actor[ractor].y_speed + rpower * 1, xsize, 25, 0, 20);
 create_cloud(CLOUD_YELLOW_EXPLOSION, actor[ractor].x + (-4 * GRAIN * actor[ractor].facing), actor[ractor].y,
   actor[ractor].x_speed, actor[ractor].y_speed + rpower * 1, xsize, 30, 0, 30);
 create_cloud(CLOUD_RISING_BIGLIGHT, actor[ractor].x + (-4 * GRAIN * actor[ractor].facing), actor[ractor].y,
   actor[ractor].x_speed, actor[ractor].y_speed + rpower * 1, xsize * 3, 20, 0, 10);


 actor[ractor].rocket_charge = 40;
 if (super == 1)
  actor[ractor].rocket_charge = 120;

 actor[ractor].rocketing = 1;

}




void grapple_movement(int gactor)
{
 int x1 = actor[gactor].x;
 int y1 = actor[gactor].y;
 int x2 = bullet[actor[gactor].grapple_bullet].x;
 int y2 = bullet[actor[gactor].grapple_bullet].y;

 int min_length = 1000;// - actor[gactor].inertia * 5;

 int x_length = abs(x1 - x2);
 int y_length = abs(y1 - y2);

 float x_accel = 0;
 float y_accel = 0;

 char x_dir = 0, y_dir = 0;

 if (x1 > x2) x_dir = -1;
 if (x1 < x2) x_dir = 1;
 if (y1 > y2) y_dir = -1;
 if (y1 < y2) y_dir = 1;

 float length = hypot(x_length, y_length);

 if (length < min_length) return;

 if (length > 5000) length = 5000;

 x_accel = (20 + (length - min_length) * (x_length / (length + 1)) / 200) * x_dir;
// if (x_accel < GRAIN / 100) x_accel = 0;
 if (x_accel > GRAIN * 20) x_accel = GRAIN * 20;
 y_accel = (20 + (length - min_length) * (y_length / (length + 1)) / 200) * y_dir;
// if (y_accel < GRAIN / 50) y_accel = 0;
 if (y_accel > GRAIN * 20) y_accel = GRAIN * 20;

 x_accel *= 300;
 x_accel /= 100 + actor[gactor].grapple_inertia;
 y_accel *= 300;
 y_accel /= 100 + actor[gactor].grapple_inertia;
 
 actor[gactor].x_speed += x_accel;
 actor[gactor].y_speed += y_accel;

}


void actor_dig(int dactor)
{
 int dig_x = cos(actor[dactor].angle) * 700;
 dig_x *= actor [dactor].facing;
 int dig_y = sin(actor[dactor].angle) * 1000;

// play_sound(WAV_RICOCHET);
 blast_dirt(actor[dactor].x, actor[dactor].y, actor[dactor].circle_radius + 2, 1);
 blast_dirt(actor[dactor].x + dig_x, actor[dactor].y + dig_y, actor[dactor].circle_radius + 4, 1);
// blast_dirt(actor[dactor].x + dig_x - 200 + prand(400), actor[dactor].y + dig_y - 200 + prand(400), 10);
// blast_dirt(actor[dactor].x + dig_x - 200 + prand(400), actor[dactor].y + dig_y - 200 + prand(400), 12);
}


/*int place_actor(int pactor)
{

// blast_dirt(actor[pactor].x + (3 * GRAIN), actor[pactor].y, 15);
// blast_dirt(actor[pactor].x, actor[pactor].y + (3 * GRAIN), 15);

 return 1;

}*/



// is actor gripping to a surface below it (ie standing)?
char actor_grounded(int gactor, char wide)
{

// if leg_action is stunned, whatever - return 0
int i;

  for (i = (actor[gactor].x / GRAIN) - actor[gactor].width - wide; i < (actor[gactor].x / GRAIN) + actor[gactor].width + wide; i++)
  {
   if (get_dirt(i, (actor[gactor].y / GRAIN) + actor[gactor].height + 1))
   {
    return 1;
   }
  }


  return 0;

}



void gravitate(int gractor)
{

// leave gravity on, otherwise just moving up and down ignores grounding

   if (user[actor[gractor].user].status != USTAT_LOCAL
       && user[actor[gractor].user].status != USTAT_AI
       && actor[gractor].jetting == 1) return;

   actor [gractor].y_speed += game[0].gravity;

//   if (actor [gractor].y_speed > 10) actor [gractor].y_speed = 10;

}


int animate_legs(int dactor)
{

// if (actor[dactor].leg_action == LA_DRIFT_LEFT
//     || actor[dactor].leg_action == LA_DRIFT_RIGHT)
   if (actor_grounded(dactor, 1) == 0)
      return P1_JUMP;

 switch(actor[dactor].leg_anim_state / 10)
 {
  case 0: return P1_WALK_1;
  case 1: return P1_WALK_2;
  case 2: return P1_WALK_3;
  case 3: return P1_WALK_4;
  case 4: return P1_WALK_5;
  case 5: return P1_WALK_6;
  case 6: return P1_WALK_7;
  case 7: return P1_WALK_8;
  case 8: return P1_WALK_9;
  case 9: return P1_WALK_10;
//  case 10: return P1_WALK_11;
//  case 5: return 5;
//  case 6: return 7;
  default: return P1_UNDER;
 }
}



