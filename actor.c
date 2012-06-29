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

File: actor.c
History:
Sometime in 2002 (or thereabouts) - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions for initialising the 'actors' (people and robots). This
   includes setting their game variables and assigning to them the
   appropriate bitmaps

*/

#include "allegro.h"

#include <libnet.h>

#include "config.h"
#include "globvar.h"

//#include "async.h"
#include "grid.h"
#include "prand.h"
#include "display.h"
#include "bullet.h"
#include "palette.h"
#include "cloud.h"

#include "acthead.h"

extern BITMAP *actor_bmp [NO_ACTORS] [NO_ACTOR_BITMAPS] [2] [2];
extern BITMAP *soldier_file [NO_SOLDIER_FILES];
/*extern DATAFILE *actor_file2;
extern DATAFILE *actor_file3;
extern DATAFILE *actor_file4;
extern DATAFILE *actor_file5;
extern DATAFILE *actor_file6;
extern DATAFILE *actor_file7;
extern DATAFILE *actor_file8;
extern DATAFILE *actor_file9;*/

void init_actor(int actor_number, int reset_player);

void start_equip(int ractor);


int replacement_equipment2(int x, int y); // in cmds.c


//int adjust_lit_colour(int col, int lit);


void spawn_actor(int sactor, char async)
{

//int i;

 if (async == 0 && serial[0].game_type == SERIAL_CLIENT
  && user[actor[sactor].user].player > 0)
 {
  //async_spawn(sactor, actor[sactor].soldier);
 }

 user[actor[sactor].user].resting_time = 329;

init_actor(sactor, 1);

/*    actor[sactor].firing_laser = LASER_NONE;
    actor[sactor].laser_strength = 0;
    actor[sactor].remote_control = REMOTE_NONE;
    actor[sactor].health = actor[sactor].max_health;
    actor[sactor].x_speed = 0;
    actor[sactor].y_speed = 0;

   actor [sactor].armour = 0;
   actor [sactor].shield = 0;
   actor [sactor].shield_visible = 0;
   actor [sactor].cloak = 0;
   actor [sactor].uncloaked = 0;
   actor [sactor].current_weapon = 0;
   for (i = 0; i < 4; i ++)
   {
    actor [sactor].weapon [i] = user[actor[sactor].user].weapon [i];
    actor [sactor].ammunition [i] = wlist[actor [sactor].weapon [i]].ammunition;
    if (actor[sactor].weapon [i] != WPN_NONE)
     actor [sactor].clips [i] = actor[sactor].max_clips;
      else
      {
       actor [sactor].clips [i] = 0;
       actor [sactor].ammunition [i] = 0;
      }
    actor [sactor].reload [i] = 0;
   }
*/

// start place_actor function

 int px = 0;
 int py = 0;

 int tries = 0;

 if (!async)
 {
  do
  {
   px = prand(arena[0].max_x - 40) + 10;
   py = prand(arena[0].max_y - 40) + 20;
   tries ++;
   if (tries == 50000)
   {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Can't find empty space in map. Aborting.");
        exit(1);
   }
  } while(!check_free_area(px - actor[sactor].width, py - actor[sactor].height, px + actor[sactor].width, py + actor[sactor].height));

  actor[sactor].x = px * GRAIN;
  actor[sactor].y = py * GRAIN;

  blast_dirt(actor[sactor].x, actor[sactor].y, 15, 1);
 }

      display_actor_health(sactor, 0);
      display_actor_ammunition(sactor);
      if (actor[sactor].player > 0)
       init_stat_disp2(actor[sactor].player);
      display_actor_clips(sactor);
      display_actor_armour(sactor);

 if (actor[sactor].incarnation == 0)
  actor[sactor].incarnation = 1;
   else
    actor[sactor].incarnation = 0;

//   textprintf(screen, large_font, 140, 190, arena[0].counter, "A %i H %i MH %i", sactor, actor[sactor].health, actor[sactor].max_health);

 create_cloud(CLOUD_SPAWN, actor[sactor].x, actor[sactor].y,
  0, 0, actor[sactor].circle_radius * 100, 1, actor[sactor].soldier, 3);
 create_cloud(CLOUD_BIGLIGHT, actor[sactor].x, actor[sactor].y,
  0, 0, actor[sactor].circle_radius * 100, 1, 0, 3);



}


void init_actor(int actor_number, int reset_player)
{

   if (reset_player) actor [actor_number].aclass = ACLASS_PLAYER;
/*   if (actor[actor_number].player >= 0)
    actor [actor_number].atype = player[actor[actor_number].player].atype;
     else
      actor[actor_number].atype = ATYPE_MEDIUM;*/
// need to move atype to the user struct? Or something
//   actor [actor_number].atype = ATYPE_LIGHT;

   actor[actor_number].atype = user[actor[actor_number].user].atype;

//   actor [actor_number].x = 3000;
//   actor [actor_number].y = 3000;
   actor [actor_number].x_speed = 0;
   actor [actor_number].y_speed = 0;
   actor [actor_number].facing = 1;
   actor [actor_number].leg_action = 0;
   actor [actor_number].walking = 0;
   actor [actor_number].climbing = 0;
   actor [actor_number].angle = 0;
   actor [actor_number].base_angle = 128;
   actor [actor_number].recycle = 0;
   actor [actor_number].shield = 0;
   actor [actor_number].shield_visible = 0;
   actor [actor_number].cloak = 0;
   actor [actor_number].uncloaked = 0;
   actor [actor_number].shadow = 0;
   actor [actor_number].spotlight = 0;
   actor [actor_number].backpack = 0;
   actor [actor_number].regenerator = 0;
   actor [actor_number].armour = 0;
   actor [actor_number].armour_pulse = 0;
   actor [actor_number].time_since_hurt = 0;
   actor [actor_number].grapple_anchored = 0;
   actor [actor_number].grapple_bullet = -1;
   actor [actor_number].grapple_x = 0;
   actor [actor_number].grapple_y = 0;
   actor [actor_number].grapple_recycle = 0;
   actor [actor_number].remote_control = REMOTE_NONE;
   actor [actor_number].remote_thing = -1;
   actor [actor_number].firing_laser = LASER_NONE;
   actor [actor_number].lock_on = -1;
   actor [actor_number].prevent_jump = 0;
   actor [actor_number].angle_increment = 0;
   actor [actor_number].jump_delay = 0;
   actor [actor_number].prevent_pickup = 0;
   actor [actor_number].current_weapon = 0;
   actor [actor_number].leg_action = LA_WALK_RIGHT;
   actor [actor_number].leg_frame = 0;
   actor [actor_number].dip = 0;

   actor [actor_number].weapon [0] = user[actor[actor_number].user].weapon [0];
   actor [actor_number].weapon [1] = user[actor[actor_number].user].weapon [1];
   actor [actor_number].weapon [2] = user[actor[actor_number].user].weapon [2];
   actor [actor_number].weapon [3] = user[actor[actor_number].user].weapon [3];

   if (game[0].starting_weapons == 3)
   {
    actor [actor_number].weapon [2] = WPN_NONE;
    actor [actor_number].weapon [3] = WPN_NONE;
    switch(actor[actor_number].soldier)
    {
     case SOLDIER_SOLDIER:
     actor [actor_number].weapon [0] = WPN_LMG;
     actor [actor_number].weapon [1] = WPN_GREN;
     break;
     case SOLDIER_SCOUT:
     actor [actor_number].weapon [0] = WPN_LMG;
     actor [actor_number].weapon [1] = WPN_ROCKETPACK;
     break;
     case SOLDIER_CYBORG:
     actor [actor_number].weapon [0] = WPN_HMG;
     actor [actor_number].weapon [1] = WPN_FRAG;
     break;
     case SOLDIER_CRUSHER:
     actor [actor_number].weapon [0] = WPN_HMG;
     actor [actor_number].weapon [1] = WPN_GR_L;
     break;
     case SOLDIER_WALKER:
     actor [actor_number].weapon [0] = WPN_AUTOCANNON;
     actor [actor_number].weapon [1] = WPN_SQUIRM;
     break;
     case SOLDIER_BUG:
     actor [actor_number].weapon [0] = WPN_SWARM;
     actor [actor_number].weapon [1] = WPN_JETPACK;
     break;
     case SOLDIER_DEMON:
     actor [actor_number].weapon [0] = WPN_PLAS_C;
     actor [actor_number].weapon [1] = WPN_FIREBALL;
     break;
     case SOLDIER_SKELETON:
     actor [actor_number].weapon [0] = WPN_SGUN;
     actor [actor_number].weapon [1] = WPN_SHREDDER;
     break;
     case SOLDIER_SNAIL:
     actor [actor_number].weapon [0] = WPN_PLAS_R;
     actor [actor_number].weapon [1] = WPN_GAS_GREN;
     break;

    }
   }
   
/*   actor [actor_number].weapon [0] = WP_LMG;
   actor [actor_number].weapon [1] = WPN_NONE;
   actor [actor_number].weapon [2] = WPN_NONE;
   actor [actor_number].weapon [3] = WPN_NONE;*/
   int i, j;
   for (i = 0; i < 4; i ++)
   {
    for (j = 0; j < 4; j ++)
    {
     if (i == j)
      continue;
     if (actor [actor_number].weapon [i] < WPN_LMG
         && actor [actor_number].weapon [i] != WPN_NONE) // ie it's equipment
     {
      if (actor [actor_number].weapon [i] == actor [actor_number].weapon [j])
       actor [actor_number].weapon [j] = WPN_NONE;
      if (replacement_equipment2(actor [actor_number].weapon [i], actor [actor_number].weapon [j]))
       actor [actor_number].weapon [j] = WPN_NONE;
     }
    }
   }
   actor [actor_number].ammunition [0] = wlist[actor [actor_number].weapon [0]].ammunition;
   actor [actor_number].ammunition [1] = wlist[actor [actor_number].weapon [1]].ammunition;
   actor [actor_number].ammunition [2] = wlist[actor [actor_number].weapon [2]].ammunition;
   actor [actor_number].ammunition [3] = wlist[actor [actor_number].weapon [3]].ammunition;
   actor [actor_number].reload [0] = 0;
   actor [actor_number].reload [1] = 0;
   actor [actor_number].reload [2] = 0;
   actor [actor_number].reload [3] = 0;
   actor [actor_number].leg_anim_state = 0;

//   actor [actor_number].colour_1_r = user[actor[actor_number].user].colour_r;
//   actor [actor_number].colour_1_g = user[actor[actor_number].user].colour_g;
//   actor [actor_number].colour_1_b = user[actor[actor_number].user].colour_b;
   actor [actor_number].colour1 = user[actor[actor_number].user].colour1;
   actor [actor_number].colour2 = user[actor[actor_number].user].colour2;
   actor [actor_number].team = user[actor[actor_number].user].team;
   if (arena[0].teams == 0)
    actor [actor_number].team = TEAM_NONE;
     else
     {
      actor [actor_number].colour1 = team_colours(actor[actor_number].team);
     }
   
/*   actor [actor_number].colour_2_r = usayer[player_number].colour_r;
   actor [actor_number].colour_2_g = usayer[player_number].colour_g;
   actor [actor_number].colour_2_b = usayer[player_number].colour_b;*/

   actor [actor_number].propulsion = PROP_NONE;
   actor [actor_number].rocket_power = 0;
   actor [actor_number].rocket_charge = 0;
   actor [actor_number].jet_on = 0;
   actor [actor_number].backpack = 0;

   actor [actor_number].has_grail = 0;
   actor [actor_number].has_flag = -1;
   actor [actor_number].has_porkball = -1;

   actor [actor_number].async_last_timestamp = 255;
   actor [actor_number].lit = 1;

//   actor[actor_number].health = 588;

int bi, bk;//, bj;//, bz;

BITMAP *datafi = soldier_file [actor[actor_number].soldier];
switch(actor[actor_number].soldier)
{
 default:
 case SOLDIER_SOLDIER:
// soldier_location = 4;
 actor [actor_number].width = 8;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 11;
 actor [actor_number].sprite_height = 11;
 actor [actor_number].circle_radius = 13;
 actor [actor_number].inertia = 250;
 actor [actor_number].grapple_inertia = 150;
 actor [actor_number].max_health = 10 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 2;
 actor [actor_number].walk_speed = 200;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 25;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 4;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 3;
 actor [actor_number].jet_power = 36;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 4;
 actor [actor_number].rocket_power = 1100;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 26;
 actor [actor_number].fire_x = 0;
 actor [actor_number].fire_y = 0;
 actor [actor_number].firing_distance = 10;
 break;
 case SOLDIER_WALKER:
// soldier_location = 4;
 actor [actor_number].width = 11;
 actor [actor_number].sprite_width = 11;
 actor [actor_number].height = 12;
 actor [actor_number].sprite_height = 12;
 actor [actor_number].circle_radius = 15;
 actor [actor_number].inertia = 1000;
 actor [actor_number].grapple_inertia = 400;
 actor [actor_number].max_health = 30 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 4;
 actor [actor_number].walk_speed = 100;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 4;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 4;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 4;
 actor [actor_number].jet_power = 31;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 4;
 actor [actor_number].rocket_power = 850;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 18;
 actor [actor_number].fire_x = -3;
 actor [actor_number].fire_y = -7;
 actor [actor_number].firing_distance = 2;
 break;
 case SOLDIER_SNAIL:
// soldier_location = 4;
 actor [actor_number].width = 11;
 actor [actor_number].sprite_width = 11;
 actor [actor_number].height = 11;
 actor [actor_number].sprite_height = 11;
 actor [actor_number].circle_radius = 15;
 actor [actor_number].inertia = 750;
 actor [actor_number].grapple_inertia = 300;
 actor [actor_number].max_health = 22 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 4;
 actor [actor_number].walk_speed = 130;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 25; // gastropods are slow but have traction.
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 4;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 4;
 actor [actor_number].jet_power = 32;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 4;
 actor [actor_number].rocket_power = 900;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 21;
 actor [actor_number].fire_x = -4;
 actor [actor_number].fire_y = 1;
 actor [actor_number].firing_distance = 7;
 break;
 case SOLDIER_SKELETON:
// soldier_location = 4;
 actor [actor_number].width = 6;
 actor [actor_number].sprite_width = 7;
 actor [actor_number].height = 10;
 actor [actor_number].sprite_height = 10;
 actor [actor_number].circle_radius = 12;
 actor [actor_number].inertia = 100;
 actor [actor_number].grapple_inertia = 120;
 actor [actor_number].max_health = 6 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 1;
 actor [actor_number].walk_speed = 320;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 35;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 2;
// actor [actor_number].propulsion = PROP_JET;
// actor [actor_number].jet_on = 1;
// actor [actor_number].jet_power = 36;
// actor [actor_number].jet_heat = 0;
// actor [actor_number].jet_speed = 4;
 actor [actor_number].jet_power = 38;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 6;
 actor [actor_number].rocket_power = 1200;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 33;
 actor [actor_number].fire_x = 0;
 actor [actor_number].fire_y = 0;
 actor [actor_number].firing_distance = 10;
 break;
 case SOLDIER_CYBORG:
// soldier_location = 4;
 actor [actor_number].width = 8;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 11;
 actor [actor_number].circle_radius = 14;
 actor [actor_number].sprite_height = 11;
 actor [actor_number].inertia = 400;
 actor [actor_number].grapple_inertia = 180;
 actor [actor_number].max_health = 14 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 3;
 actor [actor_number].walk_speed = 160;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 12;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 4;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 3;
 actor [actor_number].jet_power = 33;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 4;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 24;
 actor [actor_number].fire_x = 0;
 actor [actor_number].fire_y = 0;
 actor [actor_number].firing_distance = 10;
 actor [actor_number].rocket_power = 1000;
 break;
 case SOLDIER_CRUSHER:
// soldier_location = 4;
 actor [actor_number].width = 11;
 actor [actor_number].sprite_width = 11;
 actor [actor_number].height = 12;
 actor [actor_number].circle_radius = 15;
 actor [actor_number].sprite_height = 12;
 actor [actor_number].inertia = 600;
 actor [actor_number].grapple_inertia = 250;
 actor [actor_number].max_health = 17 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 4;
 actor [actor_number].walk_speed = 140;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 8;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 3;
 actor [actor_number].jet_power = 32;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 4;
 actor [actor_number].rocket_power = 950;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 22;
 actor [actor_number].fire_x = 0;
 actor [actor_number].fire_y = 0;
 actor [actor_number].firing_distance = 10;
 break;
 case SOLDIER_SCOUT:
// soldier_location = 4;
 actor [actor_number].width = 7;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 10;
 actor [actor_number].circle_radius = 12;
 actor [actor_number].sprite_height = 10;
 actor [actor_number].inertia = 170;
 actor [actor_number].grapple_inertia = 140;
 actor [actor_number].max_health = 8 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 2;
 actor [actor_number].walk_speed = 250;
 actor [actor_number].walk_accel = 35;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 2;
 actor [actor_number].jet_power = 38;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 5;
 actor [actor_number].rocket_power = 1200;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 26;
 actor [actor_number].fire_x = 0;
 actor [actor_number].fire_y = 0;
 actor [actor_number].firing_distance = 10;
 break;
 case SOLDIER_DEMON:
// soldier_location = 4;
 actor [actor_number].width = 7;
 actor [actor_number].sprite_width = 7;
 actor [actor_number].height = 10;
 actor [actor_number].sprite_height = 10;
 actor [actor_number].circle_radius = 12;
 actor [actor_number].inertia = 130;
 actor [actor_number].grapple_inertia = 170;
 actor [actor_number].max_health = 7 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 1;
 actor [actor_number].walk_speed = 300;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 35;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 2;
// actor [actor_number].propulsion = PROP_JET;
// actor [actor_number].jet_on = 1;
// actor [actor_number].jet_power = 36;
// actor [actor_number].jet_heat = 0;
// actor [actor_number].jet_speed = 4;
 actor [actor_number].jet_power = 38;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 5;
 actor [actor_number].rocket_power = 1200;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 26;
 actor [actor_number].fire_x = 0;
 actor [actor_number].fire_y = 0;
 actor [actor_number].firing_distance = 10;
 break;
 case SOLDIER_BUG:
// soldier_location = 4;
 actor [actor_number].width = 12;
 actor [actor_number].sprite_width = 12;
 actor [actor_number].height = 10;
 actor [actor_number].sprite_height = 10;
 actor [actor_number].circle_radius = 13;
 actor [actor_number].inertia = 200;
 actor [actor_number].grapple_inertia = 150;
 actor [actor_number].max_health = 7 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 1;
 actor [actor_number].walk_speed = 280;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 50;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 3;
// actor [actor_number].propulsion = PROP_JET;
// actor [actor_number].jet_on = 1;
// actor [actor_number].jet_power = 36;
// actor [actor_number].jet_heat = 0;
// actor [actor_number].jet_speed = 4;
 actor [actor_number].jet_power = 38;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 5;
 actor [actor_number].rocket_power = 1200;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 26;
 actor [actor_number].fire_x = 0;
 actor [actor_number].fire_y = 0;
 actor [actor_number].firing_distance = 10;
 break;
/* case ATYPE_LIGHT:
 soldier_location = 0;
 actor [actor_number].width = 5;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 7;
 actor [actor_number].sprite_height = 7;
 actor [actor_number].inertia = 100;
 actor [actor_number].grapple_inertia = 100;
 actor [actor_number].max_health = (float) 7.5 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 600;
 actor [actor_number].max_clips = 1;
 actor [actor_number].walk_speed = 300;
// actor [actor_number].drift_speed = 200;
 actor [actor_number].walk_accel = 30;
// actor [actor_number].drift_accel = 100;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].weapon_slots = 2;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 32;
 break;
 case ATYPE_MEDIUM:
 soldier_location = 3;
 actor [actor_number].width = 6;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 8;
 actor [actor_number].sprite_height = 8;
 actor [actor_number].inertia = 250;
 actor [actor_number].grapple_inertia = 150;
 actor [actor_number].max_health = 10 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 2;
 actor [actor_number].walk_speed = 200;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 25;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 4;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 3;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 26;
 break;
 case ATYPE_HEAVY:
 soldier_location = 6;
 actor [actor_number].width = 7;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 8;
 actor [actor_number].sprite_height = 8;
 actor [actor_number].inertia = 400;
 actor [actor_number].grapple_inertia = 200;
 actor [actor_number].max_health = 15 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1800;
 actor [actor_number].max_clips = 3;
 actor [actor_number].walk_speed = 140;
// actor [actor_number].drift_speed = 100;
 actor [actor_number].walk_accel = 20;
// actor [actor_number].drift_accel = 30;
 actor [actor_number].leg_anim_speed = 3;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 4;
 actor [actor_number].propulsion = PROP_GRAPPLE;
 actor [actor_number].grapple_power = 22;
 break;
 case ATYPE_LIGHT_J:
 soldier_location = 1;
 actor [actor_number].width = 5;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 7;
 actor [actor_number].sprite_height = 7;
 actor [actor_number].inertia = 100;
 actor [actor_number].grapple_inertia = 100;
 actor [actor_number].max_health = (float) 7.5 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 750;
 actor [actor_number].max_clips = 1;
 actor [actor_number].walk_speed = 300;
// actor [actor_number].drift_speed = 200;
 actor [actor_number].walk_accel = 30;
// actor [actor_number].drift_accel = 100;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].weapon_slots = 2;
 actor [actor_number].propulsion = PROP_JET;
 actor [actor_number].jet_on = 1;
 actor [actor_number].jet_power = 38;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 5;
 break;
 case ATYPE_MEDIUM_J:
 soldier_location = 4;
 actor [actor_number].width = 11;
 actor [actor_number].sprite_width = 11;
 actor [actor_number].height = 12;
 actor [actor_number].sprite_height = 12;
 actor [actor_number].inertia = 250;
 actor [actor_number].grapple_inertia = 150;
 actor [actor_number].max_health = 10 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 2;
 actor [actor_number].walk_speed = 200;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 25;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 4;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 3;
 actor [actor_number].propulsion = PROP_JET;
 actor [actor_number].jet_on = 1;
 actor [actor_number].jet_power = 36;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 4;
 break;
 case ATYPE_HEAVY_J:
 soldier_location = 7;
 actor [actor_number].width = 7;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 8;
 actor [actor_number].sprite_height = 8;
 actor [actor_number].inertia = 400;
 actor [actor_number].grapple_inertia = 200;
 actor [actor_number].max_health = 15 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1500;
 actor [actor_number].max_clips = 3;
 actor [actor_number].walk_speed = 140;
// actor [actor_number].drift_speed = 100;
 actor [actor_number].walk_accel = 20;
// actor [actor_number].drift_accel = 30;
 actor [actor_number].leg_anim_speed = 3;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 4;
 actor [actor_number].propulsion = PROP_JET;
 actor [actor_number].jet_on = 1;
 actor [actor_number].jet_power = 32;
 actor [actor_number].jet_heat = 0;
 actor [actor_number].jet_speed = 4;
 break;
 case ATYPE_LIGHT_R:
 soldier_location = 2;
 actor [actor_number].width = 5;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 7;
 actor [actor_number].sprite_height = 7;
 actor [actor_number].inertia = 100;
 actor [actor_number].grapple_inertia = 100;
 actor [actor_number].max_health = (float) 7.5 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 750;
 actor [actor_number].max_clips = 1;
 actor [actor_number].walk_speed = 300;
// actor [actor_number].drift_speed = 200;
 actor [actor_number].walk_accel = 30;
// actor [actor_number].drift_accel = 100;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].leg_anim_speed = 5;
 actor [actor_number].weapon_slots = 2;
 actor [actor_number].propulsion = PROP_ROCKETS;
 actor [actor_number].rocket_power = 1200;
 actor [actor_number].rocket_charge = 0;
 break;
 case ATYPE_MEDIUM_R:
 soldier_location = 5;
 actor [actor_number].width = 6;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 8;
 actor [actor_number].sprite_height = 8;
 actor [actor_number].inertia = 250;
 actor [actor_number].grapple_inertia = 150;
 actor [actor_number].max_health = 10 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1000;
 actor [actor_number].max_clips = 2;
 actor [actor_number].walk_speed = 200;
// actor [actor_number].drift_speed = 150;
 actor [actor_number].walk_accel = 25;
// actor [actor_number].walk_accel = 200;
 // actor [actor_number].drift_accel = 70;
 actor [actor_number].leg_anim_speed = 4;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 3;
 actor [actor_number].propulsion = PROP_ROCKETS;
 actor [actor_number].rocket_power = 1100;
 actor [actor_number].rocket_charge = 0;
 break;
 case ATYPE_HEAVY_R:
 soldier_location = 8;
 actor [actor_number].width = 7;
 actor [actor_number].sprite_width = 8;
 actor [actor_number].height = 8;
 actor [actor_number].sprite_height = 8;
 actor [actor_number].inertia = 400;
 actor [actor_number].grapple_inertia = 200;
 actor [actor_number].max_health = 15 * game[0].health_amount;
 actor [actor_number].max_health *= user[actor[actor_number].user].handicap;
 actor [actor_number].max_health /= 100;
 actor [actor_number].health = actor [actor_number].max_health;
 actor [actor_number].max_armour = 1500;
 actor [actor_number].max_clips = 3;
 actor [actor_number].walk_speed = 140;
// actor [actor_number].drift_speed = 100;
 actor [actor_number].walk_accel = 20;
// actor [actor_number].drift_accel = 30;
 actor [actor_number].leg_anim_speed = 3;
 actor [actor_number].jump_accel = -420;
 actor [actor_number].weapon_slots = 4;
 actor [actor_number].propulsion = PROP_ROCKETS;
 actor [actor_number].rocket_power = 1000;
 actor [actor_number].rocket_charge = 0;
 break;
*/
}

start_equip(actor_number);


  for (bi = 0; bi < 4; bi ++)
  {
   if (actor [actor_number].weapon [bi] != WPN_NONE)
    actor [actor_number].clips [bi] = actor[actor_number].max_clips;
     else
     {
      actor [actor_number].clips [bi] = 0;
      actor [actor_number].ammunition [bi] = 0;
     }
  }

    int offset = 0;
    int bz, bn, bop;

    for (bi = 0; bi < NO_ACTOR_BITMAPS; bi ++)
    {
//     for (bj = 0; bj < 2; bj ++)
//     {
    
      if (actor_bmp [actor_number] [bi] [0] [0] != NULL)
       destroy_bitmap(actor_bmp [actor_number] [bi] [0] [0]);
      actor_bmp [actor_number] [bi] [0] [0] = create_bitmap(actor[actor_number].sprite_width * 2 + 1, actor[actor_number].sprite_height * 2 + 1);
      clear_bitmap(actor_bmp [actor_number] [bi] [0] [0]);

      if (actor_bmp [actor_number] [bi] [0] [1] != NULL)
       destroy_bitmap(actor_bmp [actor_number] [bi] [0] [1]);
      actor_bmp [actor_number] [bi] [0] [1] = create_bitmap(actor[actor_number].sprite_width * 2 + 1, actor[actor_number].sprite_height * 2 + 1);
      clear_bitmap(actor_bmp [actor_number] [bi] [0] [1]);
      bk = bi;
//     blit(datafi, actor_bmp [actor_number] [bi], P1_UNDER * 24 + 2, 1, 1, 0, actor[actor_number].sprite_width * 2 + 1, actor[actor_number].sprite_height * 2 + 1);
      switch(actor[actor_number].soldier)
      {
       case SOLDIER_DEMON:
       case SOLDIER_SKELETON:
        offset = -2;
        break;
       case SOLDIER_CRUSHER:
        if (bi >= P1_UNDER)
         offset = 2;
        break;
      }
      blit(datafi, actor_bmp [actor_number] [bi] [0] [0], bi * 24 + 2, 1, 1 + offset, 0, 23, actor[actor_number].sprite_height * 2 + 1);
      if (bi < P1_UNDER && actor[actor_number].soldier != SOLDIER_SNAIL
       && actor[actor_number].soldier != SOLDIER_BUG)
        blit(datafi, actor_bmp [actor_number] [bi] [0] [1], P1_BACKPACK * 24 + 2, 1, 1 + offset, 0, 23, actor[actor_number].sprite_height * 2 + 1);
      masked_blit(datafi, actor_bmp [actor_number] [bi] [0] [1], bi * 24 + 2, 1, 1 + offset, 0, 23, actor[actor_number].sprite_height * 2 + 1);
      if (bi < P1_UNDER && actor[actor_number].soldier == SOLDIER_SNAIL)
       masked_blit(datafi, actor_bmp [actor_number] [bi] [0] [1], P1_BACKPACK * 24 + 2, 1, 1 + offset, 0, 23, actor[actor_number].sprite_height * 2 + 1);
      if (bi >= P1_UNDER && actor[actor_number].soldier == SOLDIER_BUG)
        masked_blit(datafi, actor_bmp [actor_number] [bi] [0] [1], P1_BACKPACK * 24 + 2, 1, 1 + offset, 0, 23, actor[actor_number].sprite_height * 2 + 1);

      if (bi >= P1_WALK_1 && bi <= P1_WALK_10)
      {
       actor[actor_number].walk_bounce [bi - P1_WALK_1] = 0;
       if (getpixel(datafi, bi * 24 + 1, 0) == COLOUR_BLUE4)
        actor[actor_number].walk_bounce [bi - P1_WALK_1] = -1;
       if (getpixel(datafi, bi * 24 + 1, 0) == COLOUR_RED4)
        actor[actor_number].walk_bounce [bi - P1_WALK_1] = 1;
      }

     int got_pixel = 0;

     int col1_pixel = getpixel(datafi, 0, 1);
     int col2_pixel = getpixel(datafi, 0, 2);

//     actor[actor_number].colour1 = COLOUR_RED4;
//     actor[actor_number].colour2 = COLOUR_YELLOW4;

     for (bz = 0; bz < 24; bz ++)
     {
      for (bop = 0; bop < 2; bop ++)
      {
       for (bn = 0; bn < actor[actor_number].sprite_height * 2 + 1; bn ++)
       {
        got_pixel = getpixel(actor_bmp [actor_number] [bi] [0] [bop], bz, bn);
        if (got_pixel <= 224)
        {
         putpixel(actor_bmp [actor_number] [bi] [0] [bop], bz, bn, 0);
         continue;
        }
        if (got_pixel == col1_pixel) got_pixel = actor[actor_number].colour1 - 3;
        if (got_pixel == col1_pixel + 1) got_pixel = actor[actor_number].colour1 - 2;
        if (got_pixel == col1_pixel + 2) got_pixel = actor[actor_number].colour1 - 1;
        if (got_pixel == col1_pixel + 3) got_pixel = actor[actor_number].colour1;
        if (got_pixel == col2_pixel) got_pixel = actor[actor_number].colour2 - 3;
        if (got_pixel == col2_pixel + 1) got_pixel = actor[actor_number].colour2 - 2;
        if (got_pixel == col2_pixel + 2) got_pixel = actor[actor_number].colour2 - 1;
        if (got_pixel == col2_pixel + 3) got_pixel = actor[actor_number].colour2;
//       got_pixel = adjust_lit_colour(got_pixel, bj);
        putpixel(actor_bmp [actor_number] [bi] [0] [bop], bz, bn, got_pixel);
//       if (got_pixel == 122) putpixel(actor_bmp [actor_number] [bi], bz, bn, 31);
//       if (got_pixel == 232) putpixel(actor_bmp [actor_number] [bi], bz, bn, 224 + actor_number);
       }
      }
     }
      
      
/*     int team_colour = 0;

     switch(actor[actor_number].team)
     {
      case TEAM_NONE: team_colour = TEAM_NONE_COLOUR; break;
      case TEAM_RED: team_colour = TEAM_RED_COLOUR; break;
      case TEAM_BLUE: team_colour = TEAM_BLUE_COLOUR + 2; break;
      case TEAM_GREEN: team_colour = TEAM_GREEN_COLOUR + 1; break;
      case TEAM_GOLD: team_colour = TEAM_GOLD_COLOUR + 4; break;
      case TEAM_PURPLE: team_colour = TEAM_PURPLE_COLOUR + 4; break;
      case TEAM_BROWN: team_colour = TEAM_BROWN_COLOUR; break;
     }

     int got_pixel = 0;

     int team_pixel = getpixel(datafi, 0, 0);
     
     for (bz = 0; bz < 17; bz ++)
     {
      for (bj = 0; bj < 17; bj ++)
      {
       got_pixel = getpixel(actor_bmp [actor_number] [bi], bz, bj);
       if (got_pixel == 122) putpixel(actor_bmp [actor_number] [bi], bz, bj, 31);
       if (got_pixel == 232) putpixel(actor_bmp [actor_number] [bi], bz, bj, 224 + actor_number);
       if (arena[0].teams)
        if (got_pixel == team_pixel) putpixel(actor_bmp [actor_number] [bi], bz, bj, team_colour);
      }
     }
*/
//    }
//   }
//    }
   }




   for (bop = 0; bop < 2; bop ++)
   {
    for (bi = 0; bi < NO_ACTOR_BITMAPS; bi ++)
    {
      if (actor_bmp [actor_number] [bi] [1] [bop] != NULL)
       destroy_bitmap(actor_bmp [actor_number] [bi] [1] [bop]);
      actor_bmp [actor_number] [bi] [1] [bop] = create_bitmap(actor[actor_number].sprite_width * 2 + 1, actor[actor_number].sprite_height * 2 + 1);
      clear_bitmap(actor_bmp [actor_number] [bi] [1] [bop]);

      draw_sprite_h_flip(actor_bmp [actor_number] [bi] [1] [bop], actor_bmp [actor_number] [bi] [0] [bop], 0, 0);
    }
   }

/*
//   actor[actor_number].max_health = 955;

    for (bi = 0; bi < NO_ACTOR_BITMAPS; bi ++)
    {
     if (actor_bmp [actor_number] [bi] != NULL)
      destroy_bitmap(actor_bmp [actor_number] [bi]);
     actor_bmp [actor_number] [bi] = create_bitmap(actor[actor_number].sprite_width * 2 + 1, actor[actor_number].sprite_height * 2 + 1);
     clear_bitmap(actor_bmp [actor_number] [bi]);
//     circlefill(actor_bmp [actor_number] [bi], 8, 8, random() % 7, 1);
     bk = bi;
     blit(datafi, actor_bmp [actor_number] [bi], P1_UNDER * 18 + 2, soldier_location * 18, 1, 0, actor[actor_number].sprite_width * 2 + 1, actor[actor_number].sprite_height * 2 + 1);
     masked_blit(datafi, actor_bmp [actor_number] [bi], bi * 18 + 2, soldier_location * 18, 1, 0, 17, 17);

     int team_colour = 0;

     switch(actor[actor_number].team)
     {
      case TEAM_NONE: team_colour = TEAM_NONE_COLOUR; break;
      case TEAM_RED: team_colour = TEAM_RED_COLOUR; break;
      case TEAM_BLUE: team_colour = TEAM_BLUE_COLOUR + 2; break;
      case TEAM_GREEN: team_colour = TEAM_GREEN_COLOUR + 1; break;
      case TEAM_GOLD: team_colour = TEAM_GOLD_COLOUR + 4; break;
      case TEAM_PURPLE: team_colour = TEAM_PURPLE_COLOUR + 4; break;
      case TEAM_BROWN: team_colour = TEAM_BROWN_COLOUR; break;
     }

     int got_pixel = 0;

     int team_pixel = getpixel(datafi, 0, 0);
     
     for (bz = 0; bz < 17; bz ++)
     {
      for (bj = 0; bj < 17; bj ++)
      {
       got_pixel = getpixel(actor_bmp [actor_number] [bi], bz, bj);
//       if (got_pixel == 224) putpixel(actor_bmp [actor_number] [bi], bz, bj, 0);
       if (got_pixel == 122) putpixel(actor_bmp [actor_number] [bi], bz, bj, 31);
       if (got_pixel == 232) putpixel(actor_bmp [actor_number] [bi], bz, bj, 224 + actor_number);
       if (arena[0].teams)
        if (got_pixel == team_pixel) putpixel(actor_bmp [actor_number] [bi], bz, bj, team_colour);
//   textprintf(screen, font, 10 + bi * 25, 10 + bj * 15, palette_color[100], "%i", getpixel(actor_bmp [bz], bi, bj));
      }
     }
     
//    }
//   }
   
   }
*/
   
}


void init_porkball(int actor_number)
{

   actor [actor_number].aclass = ACLASS_PORKBALL;
   actor[actor_number].atype = ACLASS_NONE;

   actor [actor_number].x = 3000;
   actor [actor_number].y = 3000;
   actor [actor_number].x_speed = 0;
   actor [actor_number].y_speed = 0;
   actor [actor_number].facing = 1;
   actor [actor_number].leg_action = 0;
   actor [actor_number].walking = 0;
   actor [actor_number].angle = 0;
   actor [actor_number].recycle = 0;
   actor [actor_number].shield = 0;
   actor [actor_number].shield_visible = 0;
   actor [actor_number].armour = 0;
   actor [actor_number].time_since_hurt = 0;
   actor [actor_number].grapple_anchored = 0;
   actor [actor_number].grapple_bullet = -1;
   actor [actor_number].grapple_x = 0;
   actor [actor_number].grapple_y = 0;
   actor [actor_number].grapple_recycle = 0;
   actor [actor_number].remote_control = REMOTE_NONE;
   actor [actor_number].remote_thing = -1;
   actor [actor_number].firing_laser = LASER_NONE;
   actor [actor_number].lock_on = -1;
   actor [actor_number].prevent_jump = 0;
   actor [actor_number].angle_increment = 0;
   actor [actor_number].jump_delay = 0;
   actor [actor_number].prevent_pickup = 0;
   actor [actor_number].current_weapon = 0;
   actor [actor_number].leg_action = LA_WALK_RIGHT;
   actor [actor_number].weapon [0] = WPN_NONE;
   actor [actor_number].weapon [1] = WPN_NONE;
   actor [actor_number].weapon [2] = WPN_NONE;
   actor [actor_number].weapon [3] = WPN_NONE;
   actor [actor_number].leg_anim_state = 0;

   actor [actor_number].colour1 = COLOUR_RED3;
   actor [actor_number].colour2 = COLOUR_RED3;
//   actor [actor_number].colour_1_b = 40;
   actor [actor_number].team = TEAM_NONE;

   actor [actor_number].propulsion = PROP_NONE;
   actor [actor_number].rocket_power = 0;
   actor [actor_number].rocket_charge = 0;
   actor [actor_number].jet_on = 0;

   actor [actor_number].has_grail = 0;
   actor [actor_number].has_flag = -1;
   actor [actor_number].has_porkball = -1;

 actor [actor_number].width = 3;
 actor [actor_number].sprite_width = 3;
 actor [actor_number].height = 3;
 actor [actor_number].sprite_height = 3;
 actor [actor_number].inertia = 100;
 actor [actor_number].grapple_inertia = 100;
 actor [actor_number].max_health = 1;
 actor [actor_number].health = 1;
 actor [actor_number].max_armour = 1;
 actor [actor_number].max_clips = 1;
 actor [actor_number].walk_speed = 0;
 actor [actor_number].walk_accel = 0;
 actor [actor_number].jump_accel = 0;
 actor [actor_number].leg_anim_speed = 0;
 actor [actor_number].weapon_slots = 0;
 actor [actor_number].propulsion = PROP_NONE;
 actor [actor_number].grapple_power = 0;

 actor [actor_number].jetting = 0;

 arena[0].porkball_state = -1;

}

// Different to the cmds.c version because here y contains WPN_ values,
//  not EQUIP_ values.
int replacement_equipment2(int x, int y)
{
 if (x == WPN_SHIELD && y == WPN_ARMOUR)
  return 1;
 if (x == WPN_ARMOUR && y == WPN_SHIELD)
  return 1;

 if (x == WPN_JETPACK && y == WPN_ROCKETPACK)
  return 1;
 if (x == WPN_ROCKETPACK && y == WPN_JETPACK)
  return 1;

 if (x == WPN_CLOAK && y == WPN_SHADOW)
  return 1;
 if (x == WPN_SHADOW && y == WPN_CLOAK)
  return 1;

 return 0;

}


void start_equip(int ractor)
{
 int i = 4;


 for (i = 0; i < 4; i ++)
 {
  if (i >= actor[ractor].weapon_slots)
   break;
  // If this is changed, must also change version in cmds.c:
  switch(actor[ractor].weapon [i])
  {
   case WPN_SHIELD: actor[ractor].shield = 1; actor[ractor].shield_visible = 5;
//   play_sound_pos(WAV_S_PULSE, 1000, 250, actor[ractor].x, actor[ractor].y);
   break;
   case WPN_ARMOUR: actor[ractor].armour = 1;
   break;
   case WPN_CLOAK: actor[ractor].cloak = 1;
   break;
   case WPN_JETPACK: actor[ractor].propulsion = PROP_JET;
    actor[ractor].jet_on = 1;
    actor[ractor].backpack = 1;
   break;
   case WPN_ROCKETPACK: actor[ractor].propulsion = PROP_ROCKETS;
    actor[ractor].jet_on = 0;
    actor[ractor].backpack = 1;
   break;
   case WPN_SHADOW: actor[ractor].shadow = 1;
   break;
   case WPN_REGENERATOR: actor[ractor].regenerator = 1;
   break;
   case WPN_SPOTLIGHT: actor[ractor].spotlight = 1;
   break;
  }
 }

}

/*void splatter_actor(int sactor, int sbull, int colour)
{


}*/



