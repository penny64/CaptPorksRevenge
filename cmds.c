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

File: cmds.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - movement and actions of actors.
 - picking up pickups
 - intialising the command arrays from the config file

*/

#include <math.h>
#include <string.h>

#include "allegro.h"
//#include "libnet.h"

#include "config.h"

#include "globvar.h"

#include "cmds.h"
#include "base.h"
#include "move.h"
#include "bullet.h"
#include "cloud.h"
#include "prand.h"
#include "display.h"
#include "sound.h"
#include "pickup.h"
#include "ai.h"
#include "actor.h"
#include "objhead.h"
#include "pointhd.h"
#include "score.h"
#include "connect.h"
#include "async.h"

#include "acthead.h"

void get_keypress(void);
void init_cmds(void);
void clear_cmd_buffer(void);
void issue_command(int player_commanded, int command_given);
void enact_commands(void);
void get_movement(void);
void actor_change_weapon(int chactor, int slot);
void actor_replace_weapon(int ractor, int slot);
int actor_shoot(int sactor, char async, int seed, int lock);
int discharge_weapon(int weapon_type, int x_pos, int y_pos, int x_speed, int y_speed, int owner, int spread_mult, float angle, int seed, int async, int lock);
void actor_grapple(int sactor);
void weapon_sound(int wpn, int x, int y);
void remote_control(int ractor, int rcom);
void actor_tracer(int sactor);
void acquire_lock(int lactor);
void issue_user_command(int user_commanded, int command_given);
void respawn_command(int i, int k);
void toggle_jet(int jactor);
void shoot_porkball(int sactor);
int pickup_game_thing(int ractor, int pcount, int async);
int replacement_equipment(int x, int y);

//int collate_commands(void);
void clear_server_cmd_buffer(void);
void inc_server_cmds(void);
int get_server_state(int shift);
void user_to_server_cmds(int ui, int shift);

int user_cmd_buffer [NO_USERS] [NO_CMDS];

#define SERVER_CMD_BUFFER_SIZE 5

int server_cmd_buffer [SERVER_CMD_BUFFER_SIZE] [NO_USERS] [NO_CMDS];

extern int game_over;

int server_cmd_state;

//int collated;

void init_cmds2(void)
{
 server_cmd_state = 0;
// collated = 0;
 
}


int send_out_commands(void)
{
 int i; //, j;
// char call_server = -1;

// clear_server_cmd_buffer();

// clear_server_cmd_buffer();

// if (serial[0].game_type == SERIAL_LOCAL_ONLY)
 {
  for (i = 0; i < NO_USERS; i++)
  {
   if (user[i].active == 0) continue;
   switch(user[i].status)
   {
    case USTAT_AI:
    run_ai_user(i);
//    user_to_server_cmds(i, 0); // was ,1
    break;
//    case USTAT_LOCAL:
//    user_to_server_cmds(i, 0); // was ,1
//    break;
   }
  }
 }
/*
 if (serial[0].game_type == SERIAL_SERVER)
 {
  for (i = 0; i < NO_USERS; i++)
  {
   if (user[i].status == USTAT_NONE) continue;

   switch(user[i].status)
   {
/ *    case USTAT_AI:
    run_ai_user(i);
    send_remote_commands(user_cmd_buffer [i], i);
    break;* /
    case USTAT_LOCAL:
    user_to_server_cmds(i, 0); // was ,1
//    send_remote_commands(i, server_cmd_state);
    break;
    case USTAT_CLIENT:
    get_remote_cmds(i, get_server_state(0), USER_BUFFER);
    user_to_server_cmds(i, 0);
    //call_server = i;
    break;
   }
  }
 }
 
 if (serial[0].game_type == SERIAL_CLIENT)
 {
  for (i = 0; i < NO_USERS; i++)
  {
   if (user[i].status == USTAT_NONE) continue;

   wait_on_port();

   switch(user[i].status)
   {
    case USTAT_LOCAL:
//    send_remote_commands(user_cmd_buffer [i], i);
    textprintf(screen, font, 100, 190, 15, "Send: 1");
    send_remote_commands(i, get_server_state(0), USER_BUFFER);
    break;
    case USTAT_SERVER:
//    call_server = i;
    break;
/ *    case USTAT_AI:
    run_ai_user(i);
    send_remote_commands(user_cmd_buffer [i], i);
    break;

    Should turn this back on to allow distributed AI processing?
    
    * /
   }
  }
 }
*/
/*

 2nd one:


 if (serial[0].game_type == SERIAL_SERVER)
 {
  for (i = 0; i < NO_USERS; i++)
  {
   if (user[i].status == USTAT_NONE) continue;

   wait_on_port();

   switch(user[i].status)
   {
/ *    case USTAT_AI:
    run_ai_user(i);
    send_remote_commands(user_cmd_buffer [i], i);
    break;* /
    case USTAT_LOCAL:
    send_remote_commands(i, get_server_state(0), SERVER_BUFFER);
//    user_to_server_cmds(i, 0); // was ,1
//    send_remote_commands(i, server_cmd_state);
    break;
    case USTAT_CLIENT:
    send_remote_commands(i, get_server_state(0), SERVER_BUFFER);
//    get_remote_cmds(i, get_server_state(0), USER_BUFFER);
//    user_to_server_cmds(i, 0);
    //call_server = i;
    break;
   }
  }
 }
 
*/
 
 
// if (call_server != -1)
//  get_remote_cmds(call_server, get_server_state(0), USER_BUFFER);

 return 1;

}




void clear_cmd_buffer(void)
{

int i, j;

for (i = 0; i < NO_USERS; i++)
{

   if (user [i].status == USTAT_NONE) continue;
   for (j = 0; j < NO_CMDS; j ++)
   {
    user_cmd_buffer [i] [j] = 0;
   }
}

}
/*
void clear_server_cmd_buffer(void)
{

int i, j;

for (i = 0; i < NO_USERS; i++)
{

   if (user [i].status == USTAT_NONE) continue;
   for (j = 0; j < NO_CMDS; j ++)
   {
    server_cmd_buffer [server_cmd_state] [i] [j] = 0;
   }
}

}

void user_to_server_cmds(int ui, int shift)
{

 int j;

 for (j = 0; j < NO_CMDS; j ++)
 {
  server_cmd_buffer [get_server_state(shift)] [ui] [j] = user_cmd_buffer [ui] [j];
 }

}

void inc_server_cmds(void)
{
 server_cmd_state ++;
 if (server_cmd_state == SERVER_CMD_BUFFER_SIZE)
  server_cmd_state = 0;
}

int get_server_state(int shift)
{
 int retval = server_cmd_state + shift;
 if (retval > SERVER_CMD_BUFFER_SIZE)
  return retval - SERVER_CMD_BUFFER_SIZE;

  return retval;
}
*/
/*
Puts the command into the buffer to be enacted next turn
*/
void issue_command(int player_commanded, int command_given)
{

 user_cmd_buffer [player[player_commanded].user] [command_given] = 1;

//   textout(screen, font, " issued command.", 80, 30, palette_color[1]);
//   readkey();

}

void issue_user_command(int user_commanded, int command_given)
{

 user_cmd_buffer [user_commanded] [command_given] = 1;

}

void respawn_command(int i, int k)
{

   if (user[i].penalty_time > 0) return;


   if (game[0].atypes_avail == 0 && user_cmd_buffer [i] [CMD_SPAWN_0])
   {
      user[i].soldier = SOLDIER_SOLDIER;
      actor[k].soldier = SOLDIER_SOLDIER;
      spawn_actor(k, 0);
   } else
    if (user_cmd_buffer [i] [CMD_SPAWN_0] && game[0].atypes_avail > 0)
    {
     user[i].soldier = SOLDIER_SOLDIER;
     actor[k].soldier = SOLDIER_SOLDIER;
     spawn_actor(k, 0);
    } else
     if (user_cmd_buffer [i] [CMD_SPAWN_1])// && game[0].atypes_avail > 0)
     {
      user[i].soldier = SOLDIER_SCOUT;
      actor[k].soldier = SOLDIER_SCOUT;
      spawn_actor(k, 0);
     } else
     if (user_cmd_buffer [i] [CMD_SPAWN_2])
     {
      user[i].soldier = SOLDIER_CYBORG;
      actor[k].soldier = SOLDIER_CYBORG;
      spawn_actor(k, 0);
     } else
      if (user_cmd_buffer [i] [CMD_SPAWN_3] && game[0].atypes_avail > 0)
      {
       user[i].soldier = SOLDIER_CRUSHER;
       actor[k].soldier = SOLDIER_CRUSHER;
       spawn_actor(k, 0);
      } else
       if (user_cmd_buffer [i] [CMD_SPAWN_4] && game[0].atypes_avail > 0)
       {
        user[i].soldier = SOLDIER_WALKER;
        actor[k].soldier = SOLDIER_WALKER;
        spawn_actor(k, 0);
       } else
         if (user_cmd_buffer [i] [CMD_SPAWN_5] && game[0].atypes_avail > 0)
         {
          user[i].soldier = SOLDIER_BUG;
          actor[k].soldier = SOLDIER_BUG;
          spawn_actor(k, 0);
         } else
         if (user_cmd_buffer [i] [CMD_SPAWN_6] && game[0].atypes_avail > 0)
         {
          user[i].soldier = SOLDIER_SKELETON;
          actor[k].soldier = SOLDIER_SKELETON;
          spawn_actor(k, 0);
         } else
         if (user_cmd_buffer [i] [CMD_SPAWN_7] && game[0].atypes_avail > 0)
         {
          user[i].soldier = SOLDIER_DEMON;
          actor[k].soldier = SOLDIER_DEMON;
          spawn_actor(k, 0);
         } else
         if (user_cmd_buffer [i] [CMD_SPAWN_8] && game[0].atypes_avail > 0)
         {
          user[i].soldier = SOLDIER_SNAIL;
          actor[k].soldier = SOLDIER_SNAIL;
          spawn_actor(k, 0);
         } /*else
         if (user_cmd_buffer [i] [CMD_SPAWN_9] && game[0].atypes_avail > 0)
         {
          user[i].soldier = SOLDIER_;
          actor[k].soldier = SOLDIER_CYBORG;
          spawn_actor(k, 0);
         }/ * else
        if (user_cmd_buffer [i] [CMD_SPAWN_4] && game[0].atypes_avail == 2)
        {
         user[i].atype = ATYPE_MEDIUM_J;
         spawn_actor(k, 0);
        } else
         if (user_cmd_buffer [i] [CMD_SPAWN_5] && game[0].atypes_avail == 2)
         {
          user[i].atype = ATYPE_HEAVY_J;
          spawn_actor(k, 0);
         } else
          if (user_cmd_buffer [i] [CMD_SPAWN_6] && game[0].atypes_avail == 2)
          {
           user[i].atype = ATYPE_LIGHT_R;
           spawn_actor(k, 0);
          } else
           if (user_cmd_buffer [i] [CMD_SPAWN_7] && game[0].atypes_avail == 2)
           {
            user[i].atype = ATYPE_MEDIUM_R;
            spawn_actor(k, 0);
           } else
            if (user_cmd_buffer [i] [CMD_SPAWN_8] && game[0].atypes_avail == 2)
            {
             user[i].atype = ATYPE_HEAVY_R;
             spawn_actor(k, 0);
            }
*/
/*   if (game[0].atypes_avail == 0 && user_cmd_buffer [i] [CMD_SPAWN_0])
   {
      user[i].atype = ATYPE_MEDIUM;
      spawn_actor(k, 0);
   } else
    if (user_cmd_buffer [i] [CMD_SPAWN_0] && game[0].atypes_avail > 0)
    {
     user[i].atype = ATYPE_LIGHT;
     spawn_actor(k, 0);
    } else
     if (user_cmd_buffer [i] [CMD_SPAWN_1])
     {
      user[i].atype = ATYPE_MEDIUM;
      spawn_actor(k, 0);
     } else
      if (user_cmd_buffer [i] [CMD_SPAWN_2] && game[0].atypes_avail > 0)
      {
       user[i].atype = ATYPE_HEAVY;
       spawn_actor(k, 0);
      } else
       if (user_cmd_buffer [i] [CMD_SPAWN_3] && game[0].atypes_avail == 2)
       {
        user[i].atype = ATYPE_LIGHT_J;
        spawn_actor(k, 0);
       } else
        if (user_cmd_buffer [i] [CMD_SPAWN_4] && game[0].atypes_avail == 2)
        {
         user[i].atype = ATYPE_MEDIUM_J;
         spawn_actor(k, 0);
        } else
         if (user_cmd_buffer [i] [CMD_SPAWN_5] && game[0].atypes_avail == 2)
         {
          user[i].atype = ATYPE_HEAVY_J;
          spawn_actor(k, 0);
         } else
          if (user_cmd_buffer [i] [CMD_SPAWN_6] && game[0].atypes_avail == 2)
          {
           user[i].atype = ATYPE_LIGHT_R;
           spawn_actor(k, 0);
          } else
           if (user_cmd_buffer [i] [CMD_SPAWN_7] && game[0].atypes_avail == 2)
           {
            user[i].atype = ATYPE_MEDIUM_R;
            spawn_actor(k, 0);
           } else
            if (user_cmd_buffer [i] [CMD_SPAWN_8] && game[0].atypes_avail == 2)
            {
             user[i].atype = ATYPE_HEAVY_R;
             spawn_actor(k, 0);
            }
*/
}


/*
Actually carries out the command.
Tests to make sure it's possible.
*/
void enact_commands(void)
{

int i, j;

int k;

for (i = 0; i < NO_USERS; i++)
{

//   if (user [i].status == USTAT_NONE) continue;
   if (user [i].status != USTAT_LOCAL
       && user [i].status != USTAT_AI) continue;
   if (user[i].out_of_lives == 1) continue;

   k = user[i].actor;

   if (actor[k].aclass == ACLASS_GHOST)
   {
    respawn_command(i, k);
   }

   switch(actor[k].firing_laser)
   {
    case LASER_PULSE:
    actor[k].laser_strength --;
    if (actor[k].laser_strength == 0)
    {
     actor[k].firing_laser = LASER_NONE;
     actor[k].recycle = 30;
    }
    break;
    case LASER_BEAM:
    actor[k].laser_strength --;
    if (actor[k].laser_strength == 0)
     actor[k].firing_laser = LASER_NONE;
    break;
    case LASER_TRACER:
    actor[k].laser_strength --;
    if (actor[k].laser_strength == 0)
    {
     actor[k].lock_on = -1;
     actor[k].firing_laser = LASER_NONE;
    }
    break;
   }
   if (actor[k].weapon [actor[k].current_weapon] == WPN_TRACKER
       || actor[k].weapon [actor[k].current_weapon] == WPN_SEEKER)
   {
//    actor_tracer(i);
      acquire_lock(k);
   } else actor[k].lock_on = -1;
   
   if (user_cmd_buffer [i] [CMD_LEFT] == 0 && user_cmd_buffer [i] [CMD_RIGHT] == 0)
    actor[k].leg_anim_state = 150;
   if (user_cmd_buffer [i] [CMD_UP] == 0 && user_cmd_buffer [i] [CMD_DOWN] == 0)
    actor[k].angle_increment = 0;
//   if (user_cmd_buffer [i] [CMD_JUMP] == 0) actor[k].jetting = 0;
   for (j = 0; j < NO_CMDS; j ++)
   {
      if (user_cmd_buffer [i] [j])
      {

       if (actor[k].remote_control != REMOTE_NONE)
       {
        switch(j)
        {
          case CMD_LEFT:
          remote_control(k, CMD_LEFT);
          break;
          case CMD_RIGHT:
          remote_control(k, CMD_RIGHT);
          break;
          case CMD_SHOOT:
          remote_control(k, CMD_SHOOT);
          break;
          case CMD_JUMP:
          remote_control(k, CMD_JUMP);
          break;
        }
       }
        else
         switch(j)
         {
          case CMD_UP:
//          actor [i].angle -= .05;
          if (actor [k].angle_increment > 0) actor [k].angle_increment = 0;
//          actor [k].angle_increment -= .010;
          actor [k].angle_increment -= 2;
//          if (actor [k].angle_increment <= -0.2) actor [k].angle_increment = -0.2;
          if (actor [k].angle_increment <= -30) actor [k].angle_increment = -30;
          
          if (actor[k].base_angle < (actor[k].angle_increment * -1))
           actor[k].base_angle = 0;
            else
             actor[k].base_angle += actor[k].angle_increment;
          
//          actor[k].angle += actor [k].angle_increment;
//          if (actor [k].angle < PI / -2)
//             actor [k].angle = PI / -2;
             // NOTE: have 2 angle vars: one int, for transmitting, and
             //  one float which is derived from the int one here.
             break;
          case CMD_DOWN:
          if (actor [k].angle_increment < 0) actor [k].angle_increment = 0;
          actor [k].angle_increment += 2;
          if (actor [k].angle_increment >= 30) actor [k].angle_increment = 30;
          if (255 - actor[k].base_angle < actor[k].angle_increment)
           actor[k].base_angle = 255;
            else
             actor[k].base_angle += actor[k].angle_increment;
             
//             actor[k].angle = ((float) actor[k].base_angle / 256 * PI) - (PI / 2);
//          actor [i].angle += .05;
/*
          if (actor [k].angle_increment < 0) actor [k].angle_increment = 0;
          actor [k].angle_increment += .010;
          if (actor [k].angle_increment >= 0.2) actor [k].angle_increment = 0.2;
          actor[k].angle += actor [k].angle_increment;
          if (actor [k].angle > PI / 2)
             actor [k].angle = PI / 2;
             */
             break;
          case CMD_LEFT:
           walk_actor(k, -1);
           break;
          case CMD_RIGHT:
           walk_actor(k, 1);
           break;
          case CMD_JUMP: if (actor[k].prevent_jump == 0)
          {
                    if (actor[k].jump_delay == 0 || actor[k].jet_on == 1) actor_jump(k, actor[k].grapple_anchored != 0);
                    destroy_grapple(actor[k].grapple_bullet);
          }
          break;
          case CMD_SHOOT: actor_shoot(k, WPN_NONE, arena[0].counter, actor[k].lock_on); break;
          case CMD_GRAPPLE:
//          switch(actor[k].propulsion)
//          {
//           case PROP_GRAPPLE:
    //         if (serial[0].game_type != SERIAL_LOCAL_ONLY)
    //         {
    //          async_user_shoots(actor[k].user, k, WPN_SHIELD);
    //         }
           actor_grapple(k); //break;
//           case PROP_JET: toggle_jet(k); break;
//           case PROP_ROCKETS: actor_rocket(k, 1); break;
//          }
          break;
          case CMD_DIG: actor_dig(k); break;
          case CMD_CHWEAP_0: actor_change_weapon(k, 0); break;
          case CMD_CHWEAP_1: actor_change_weapon(k, 1); break;
          case CMD_CHWEAP_2: actor_change_weapon(k, 2); break;
          case CMD_CHWEAP_3: actor_change_weapon(k, 3); break;
          case CMD_REWEAP_0: actor_replace_weapon(k, 0); break;
          case CMD_REWEAP_1: actor_replace_weapon(k, 1); break;
          case CMD_REWEAP_2: actor_replace_weapon(k, 2); break;
          case CMD_REWEAP_3: actor_replace_weapon(k, 3); break;
          // when adding new cmds may need to increase NO_CMDS
          
         }
      }
   }
   
   actor[k].angle = ((float) actor[k].base_angle / 256 * PI) - (PI / 2);

}


}


void toggle_jet(int jactor)
{

 return;
/*
 if (actor[jactor].jet_on == 1) actor[jactor].jet_on = 0;
  else actor[jactor].jet_on = 1;
 play_sound(WAV_SHORT_BLIP);
*/
}

void actor_change_weapon(int chactor, int slot)
{
 actor[chactor].current_weapon = slot;
 display_actor_ammunition(chactor);
 display_actor_clips(chactor);
}


void actor_replace_weapon(int ractor, int slot)
{

 int pcount;
 int nearby_pickup [MAX_PICKUPS];
 int no_nearby = 0;
 int shortest = 10000;
 int shortest_distance = 10000;
 int last_found = 0;

 for (pcount = 0; pcount < MAX_PICKUPS; pcount ++)
 {
   nearby_pickup [pcount] = 0;
   if (pickup[pcount].pickup_type == PICKUP_NONE) continue;
   if (pickup[pcount].pickup_type == PICKUP_BASE
    || pickup[pcount].pickup_type == PICKUP_TH_BASE) continue;
   if ((pickup[pcount].x - 1600) + 2000 < (actor[ractor].x + actor[ractor].width) + 2000 && pickup[pcount].x + 1600 > actor[ractor].x - actor[ractor].width
   && (pickup[pcount].y - 1600) + 2000 < (actor[ractor].y + actor[ractor].height) + 2000 && pickup[pcount].y + 1600 > actor[ractor].y - actor[ractor].height)
   {
    nearby_pickup [pcount] = 1;
    no_nearby ++;
    last_found = pcount;
   }
 }

 if (no_nearby == 0) return;
 if (no_nearby > 1)
 {
  for (pcount = 0; pcount < MAX_PICKUPS; pcount ++)
  {
    if (nearby_pickup [pcount] == 0) continue;
    if (abs(actor[ractor].x - pickup[pcount].x) < shortest_distance)
    {
     shortest = pcount;
     shortest_distance = abs(actor[ractor].x - pickup[pcount].x);
    }
  }
  pcount = shortest;
 } else pcount = last_found;

// textprintf(screen, font, 100, 450, random() % 15 + 1, "%i at %i, %i with %1", pcount, pickup[pcount].x, pickup[pcount].y, pickup[pcount].contains);

 if (pickup_game_thing(ractor, pcount, 0))
  return;

 int find_duplicate = 0, find_replacement = 0;

 if (pickup[pcount].pickup_type == PICKUP_EQUIP)
 {
  do
  {
   if (actor[ractor].weapon [find_duplicate] == pickup[pcount].contains + 2)
   {
/*    if (actor[ractor].weapon [find_duplicate] == WPN_ARMOUR)
    {
     actor[ractor].armour = actor[ractor].max_armour;
     //async_take_pickup(ractor, pcount);
     destroy_pickup(pcount);
     display_actor_health(ractor, 0);
     actor[ractor].prevent_pickup = 1;
     play_sound_pos(WAV_PICKUP, 1000, 250, actor[ractor].x, actor[ractor].y);
    }
    // if armour, refill it*/
    return;
   }
   find_duplicate ++;
  } while(find_duplicate < actor[ractor].weapon_slots);

  do
  {
   if (replacement_equipment(actor[ractor].weapon [find_replacement], pickup[pcount].contains) == 1)
   {
    slot = find_replacement;
    break;
   }
   find_replacement ++;
  } while(find_replacement < actor[ractor].weapon_slots);


 // okay, from now on we're going to be replacing the old one, so:
 if (actor[ractor].weapon [slot] != 0)
 {
  switch(actor[ractor].weapon [slot])
  {
   case WPN_SHIELD:
   actor[ractor].shield = 0;
   actor[ractor].shield_visible = 0;
   break;
   case WPN_ARMOUR:
   actor[ractor].armour = 0;
//   display_actor_health(ractor, 0);
   break;
   case WPN_CLOAK:
   actor[ractor].cloak = 0;
   actor[ractor].uncloaked = 0;
   break;
   case WPN_JETPACK:
    if (actor[ractor].propulsion == PROP_JET) // may've been replaced already
     actor[ractor].propulsion = PROP_GRAPPLE;
   actor[ractor].jet_on = 0;
   actor[ractor].backpack = 0;
   break;
   case WPN_ROCKETPACK:
    if (actor[ractor].propulsion == PROP_ROCKETS) // may've been replaced already
     actor[ractor].propulsion = PROP_GRAPPLE;
   actor[ractor].backpack = 0;
   break;
   case WPN_SHADOW: actor[ractor].shadow = 0;
   break;
   case WPN_REGENERATOR: actor[ractor].regenerator = 0;
   break;
   case WPN_SPOTLIGHT: actor[ractor].spotlight = 0;
   break;
  }
  actor[ractor].weapon [slot] = 0;
 }

  // If this is changed, must also change start_equip in actor.c
  switch(pickup[pcount].contains)
  {
   case EQUIP_SHIELD: actor[ractor].shield = 1; actor[ractor].shield_visible = 5;
   play_sound_pos(WAV_S_PULSE, 1000, 250, actor[ractor].x, actor[ractor].y);
   break;
   case EQUIP_ARMOUR: actor[ractor].armour = 1; //actor[ractor].max_armour;
//   display_actor_health(ractor, 0);
   break;
   case EQUIP_CLOAK: actor[ractor].cloak = 1;
   break;
   case EQUIP_JETPACK: actor[ractor].propulsion = PROP_JET;
   actor[ractor].jet_on = 1;
   actor[ractor].backpack = 1;
   break;
   case EQUIP_ROCKETPACK: actor[ractor].propulsion = PROP_ROCKETS;
   actor[ractor].jet_on = 0;
   actor[ractor].backpack = 1;
   break;
   case EQUIP_SHADOW: actor[ractor].shadow = 1;
   break;
   case EQUIP_REGENERATOR: actor[ractor].regenerator = 1;
   break;
   case EQUIP_SPOTLIGHT: actor[ractor].spotlight = 1;
   break;
  }
  actor[ractor].weapon [slot] = pickup[pcount].contains + 2;
  actor[ractor].ammunition [slot] = 0;
  actor[ractor].clips [slot] = 0;
  actor[ractor].reload [slot] = 0;
  ////async_take_pickup(ractor, pcount);
  destroy_pickup(pcount);
  display_actor_ammunition(ractor);
  display_actor_clips(ractor);
  actor[ractor].prevent_pickup = 1;
  play_sound_pos(WAV_PICKUP, 1000, 250, actor[ractor].x, actor[ractor].y);
  return;
 }
 
 // okay, from now on we're going to be replacing the old one, so:
 if (actor[ractor].weapon [slot] != 0)
 {
  switch(actor[ractor].weapon [slot])
  {
   case WPN_SHIELD:
   actor[ractor].shield = 0;
   actor[ractor].shield_visible = 0;
   break;
   case WPN_ARMOUR:
   actor[ractor].armour = 0;
//   display_actor_health(ractor, 0);
   break;
   case WPN_CLOAK:
   actor[ractor].cloak = 0;
   actor[ractor].uncloaked = 0;
   break;
   case WPN_JETPACK:
    if (actor[ractor].propulsion == PROP_JET) // may've been replaced already
    {
     actor[ractor].propulsion = PROP_GRAPPLE;
    }
   actor[ractor].backpack = 0;
   actor[ractor].jet_on = 0;
   break;
   case WPN_ROCKETPACK:
    if (actor[ractor].propulsion == PROP_ROCKETS) // may've been replaced already
    {
     actor[ractor].propulsion = PROP_GRAPPLE;
    }
   actor[ractor].backpack = 0;
   break;
   case WPN_SHADOW: actor[ractor].shadow = 0;
   break;
   case WPN_REGENERATOR: actor[ractor].regenerator = 0;
   break;
   case WPN_SPOTLIGHT: actor[ractor].spotlight = 0;
   break;
  }
  actor[ractor].weapon [slot] = 0;
 }

 
 actor[ractor].weapon [slot] = pickup[pcount].contains;
 actor[ractor].ammunition [slot] = wlist[pickup[pcount].contains].ammunition;
 actor[ractor].clips [slot] = actor[ractor].max_clips;
 actor[ractor].reload [slot] = 0;
 ////async_take_pickup(ractor, pcount);
 destroy_pickup(pcount);

 display_actor_ammunition(ractor);
 display_actor_clips(ractor);
 actor[ractor].prevent_pickup = 1;
 play_sound_pos(WAV_PICKUP, 1000, 250, actor[ractor].x, actor[ractor].y);
}


int replacement_equipment(int x, int y)
{
 if (x == WPN_SHIELD && y == EQUIP_ARMOUR)
  return 1;
 if (x == WPN_ARMOUR && y == EQUIP_SHIELD)
  return 1;

 if (x == WPN_JETPACK && y == EQUIP_ROCKETPACK)
  return 1;
 if (x == WPN_ROCKETPACK && y == EQUIP_JETPACK)
  return 1;

 if (x == WPN_CLOAK && y == EQUIP_SHADOW)
  return 1;
 if (x == WPN_SHADOW && y == EQUIP_CLOAK)
  return 1;

 return 0;

}



int pickup_game_thing(int ractor, int pcount, int async)
{

 if (pickup[pcount].pickup_type == PICKUP_FRUIT)
 {
  if (serial[0].game_type == SERIAL_LOCAL_ONLY)
  {
   eat_fruit(ractor, pcount);
   destroy_pickup(pcount);
   actor[ractor].prevent_pickup = 1;
  }
   else
   {
    if (serial[0].game_type == SERIAL_SERVER)
    {
     eat_fruit(ractor, pcount);
     destroy_pickup(pcount);
     actor[ractor].prevent_pickup = 1;
    }
    if (async == 0)
    {
     ////async_take_pickup(ractor, pcount);
     destroy_pickup(pcount);
     actor[ractor].prevent_pickup = 1;
    }
   }
  return 1;
 }


 if (pickup[pcount].pickup_type == PICKUP_POWER)
 {
  if (serial[0].game_type != SERIAL_CLIENT)
  {
   actor[ractor].health += actor[ractor].max_health / 3 + prand(1 + actor[ractor].max_health / 2);
   if (actor[ractor].health >= actor[ractor].max_health)
    actor[ractor].health = actor[ractor].max_health;
   display_actor_health(ractor, 0);
  }
  if (async == 0)
   ////async_take_pickup(ractor, pcount);
  destroy_pickup(pcount);
  actor[ractor].prevent_pickup = 1;
  play_sound_pos(WAV_HEALTH, 1000, 250, actor[ractor].x, actor[ractor].y);
  return 1;
 }

 if (pickup[pcount].pickup_type == PICKUP_GRAIL)
 {
  if (serial[0].game_type != SERIAL_CLIENT)
  {
   actor[ractor].has_grail = 1;
  }
  if (async == 0)
   ////async_take_pickup(ractor, pcount);
  destroy_pickup(pcount);
  actor[ractor].prevent_pickup = 1;
  return 1;
 }

 if (pickup[pcount].pickup_type == PICKUP_FLAG)
 {
  if (serial[0].game_type != SERIAL_CLIENT)
  {
   if (pickup[pcount].contains == actor[ractor].team)
   {
    pickup[pcount].x = team[actor[ractor].team].base_x;
    pickup[pcount].y = team[actor[ractor].team].base_y;
    if (team[actor[ractor].team].flag_at_base == 0)
    {
     score_message(SMESSAGE_FLAG_RETURN, actor[ractor].user, 0, 0);
    }
    team[actor[ractor].team].flag_at_base = 1;
    return 1;
   }
   if (actor[ractor].has_flag == -1)
   {
    if (async == 0)
     ////async_take_pickup(ractor, pcount);
    actor[ractor].has_flag = pickup[pcount].contains;
    team[actor[ractor].has_flag].flag_at_base = 0;
    score_message(SMESSAGE_HAS_FLAG, actor[ractor].user, actor[ractor].has_flag, 0);
    destroy_pickup(pcount);
    actor[ractor].prevent_pickup = 1;
    return 1;
   }
  } // end if ... != SERIAL_CLIENT
  if (async == 0)
   ////async_take_pickup(ractor, pcount);
//  if (serial[0].game_type == SERIAL_CLIENT
//      && )
//  destroy_pickup(pcount);
//  actor[ractor].prevent_pickup = 1;
  return 1;
 }

 return 0;

}

void pickup_game_thing_effect(int ractor, int pcount)
{


}

void shoot_porkball(int sactor)
{

 int porkb = actor[sactor].has_porkball;

 float angle_used = actor[sactor].angle;

// if (actor[sactor].facing == -1) angle_used += PI;

 int bull_x = cos(angle_used) * 1000;
 int bull_y = sin(angle_used) * 1000;

 bull_x *= actor[sactor].facing;

// int b2_x = bull_x * -1;

// if (actor[sactor].facing == -1) bull_x *= -1;

 bull_x += actor[sactor].x_speed;
 bull_y += actor[sactor].y_speed;

 actor[porkb].x_speed = bull_x;
 actor[porkb].y_speed = bull_y;
 actor[porkb].jump_delay = 10;

 actor[sactor].has_porkball = -1;
 actor[sactor].recycle = 20;
 arena[0].who_has_porkball = -1;

// actor[porkb].x_speed = -2000;

}


/*
Any changes to this function may need to be mirrored in ai.c
*/
int actor_shoot(int sactor, char async, int seed, int lock)
{

 if (actor[sactor].aclass != ACLASS_PLAYER)
  return -1;

 if (actor[sactor].has_porkball != -1)
 {
  shoot_porkball(sactor);
  return -1;
 }

 init_prand(seed);

 int spread_mult = 50;

 int fired = 0;

 if (async == WPN_NONE)
 {
  if (actor[sactor].recycle > 0) return -1;
  if (actor[sactor].reload [actor[sactor].current_weapon] > 0) return -1;
  if (actor[sactor].ammunition [actor[sactor].current_weapon] == 0) return -1;
  fired = actor[sactor].weapon [actor[sactor].current_weapon];
 }
  else fired = async;


 float angle_used = actor[sactor].angle;
// float scattered = 0;

 if (wlist[fired].scatter > 0)
 {
  int sc_used = wlist[fired].scatter;
  if (game[0].bullet_speed == 0 && wlist[fired].is_bullet)
   sc_used /= 3;
  angle_used += (float) prand(sc_used) / 800;
  angle_used -= (float) prand(sc_used) / 800;
 }

// angle_used += scatter / 10;

 int bull_x = cos(angle_used) * 500;
 bull_x *= actor [sactor].facing;
 int bull_y = sin(angle_used) * 500;

/* int bull_x2 = cos(angle_used - PI / 4) * 300;
 bull_x2 *= actor [sactor].facing;
 int bull_y2 = sin(angle_used - PI / 4) * 300;

 int bull_x3 = cos(angle_used + PI / 4) * 300;
 bull_x3 *= actor [sactor].facing;
 int bull_y3 = sin(angle_used + PI / 4) * 300;*/

 int bull_x2 = cos(angle_used - PI / 2) * 300;
 bull_x2 *= actor [sactor].facing;
 int bull_y2 = sin(angle_used - PI / 2) * 300;

 int bull_x3 = cos(angle_used + PI / 2) * 300;
 bull_x3 *= actor [sactor].facing;
 int bull_y3 = sin(angle_used + PI / 2) * 300;
 
 int l_speed = wlist[fired].launch_speed;
 // remember, if changed also change in ai.c
 if (wlist[fired].is_bullet && game[0].fast_bullets == 0)
 {
  spread_mult = 100;
/*
  if (l_speed >= 70)
   l_speed /= 6;
   else
    if (l_speed >= 60)
     l_speed /= 5;
      else
       if (l_speed >= 40)
        l_speed /= 3;
         else
          l_speed /= 2;*/
          // NOTE: if changed, also change in ai.c for ai tracers
  l_speed = wlist[fired].slow_speed;
 }
 if (wlist[fired].is_bullet && game[0].fast_bullets == 2)
 {
  spread_mult = 10000;
  l_speed *= 100;
 }
 
 int bull_x_speed = bull_x * l_speed;
 bull_x_speed /= 10;
 int bull_y_speed = bull_y * l_speed;
 bull_y_speed /= 10;

 float angle_passed = angle_used;
 if (angle_passed < 0) angle_used = (PI * 2) + angle_passed;
 if (actor[sactor].facing == -1) angle_used *= -1;

 //+= PI;// / 2;

 int total_x_speed = bull_x_speed;
 int total_y_speed = bull_y_speed;
 
 if (fired != WPN_REMOTE_ROCKET_C && fired != WPN_LASER_BEAM && fired != WPN_LASER_PULSE)
 {
  total_x_speed += actor[sactor].x_speed;
  total_y_speed += actor[sactor].y_speed;
 }


 int success = -1;
 char smart = 0;

  if (async == WPN_NONE && serial[0].game_type == SERIAL_CLIENT)
  {
   if (fired == WPN_REMOTE_ROCKET
    || fired == WPN_REMOTE_ROCKET_C
    || fired == WPN_BOUNCY
    || fired == WPN_SEEKER
    || fired == WPN_TRACKER)
    {
//     async_user_shoots(actor[sactor].user, sactor, fired);
     success = 0;
     smart = 1;
//     return -1; // these are fired when the client gets a
      // smart_weapon_init chunk from the server.
    }
  }

 if (success == -1)
  success = discharge_weapon(fired, actor[sactor].x + bull_x + (actor[sactor].fire_x * actor[sactor].facing * GRAIN), actor[sactor].y + (actor[sactor].total_dip * GRAIN) + bull_y + (actor[sactor].fire_y * GRAIN), total_x_speed, total_y_speed, sactor, spread_mult, angle_passed, seed, 0, lock);

 if (success != -1)
 {
 //   if (async == WPN_NONE && serial[0].game_type != SERIAL_LOCAL_ONLY)
 //   {
 //    async_user_shoots(actor[sactor].user, sactor, fired);
 //   }


/*    if (async != WPN_NONE)
    {
     textprintf(screen, small_font, 50, 165, 15, "x %i y %i xs %i ys %i ba %i      ",
     actor[sactor].x, actor[sactor].y, actor[sactor].x_speed, actor[sactor].y_speed, actor[sactor].base_angle);
    }*/

    int bspeed_x = bull_x;
    int bspeed_y = bull_y;

    bull_x *= actor[sactor].firing_distance;
    bull_x /= 10;
    bull_y *= actor[sactor].firing_distance;
    bull_y /= 10;

    muzzle_flash(sactor, fired, actor[sactor].x + actor[sactor].fire_x * GRAIN * actor[sactor].facing, actor[sactor].y + actor[sactor].fire_y * GRAIN, bull_x, bull_y, bull_x2, bull_y2, bull_x3, bull_y3, bspeed_x, bspeed_y);
    actor[sactor].recycle += wlist[fired].recycle_time;
     actor[sactor].ammunition [actor[sactor].current_weapon] --;
    display_actor_ammunition(sactor);
//    display_actor_clips(sactor);
    // Note: tracker has already played its sound
    weapon_sound(fired, actor[sactor].x, actor[sactor].y);
    if (wlist[fired].recoil > 0)
     slam_actor(sactor, bull_x * -1, bull_y * -1, wlist[fired].recoil);
    if (actor[sactor].cloak == 1)
     actor[sactor].uncloaked = 40;

   if (fired == WPN_REMOTE_ROCKET
    || fired == WPN_REMOTE_ROCKET_C
    || fired == WPN_BOUNCY
    || fired == WPN_SEEKER
    || fired == WPN_TRACKER)
    {
  //   if (async == WPN_NONE && serial[0].game_type == SERIAL_SERVER)
  //   {
  //    async_smart_init(success, -1, fired);
  //   }
    }
      // display call assumes can't fire while change button pressed
 }

  if (actor[sactor].ammunition [actor[sactor].current_weapon] == 0
  && actor[sactor].clips [actor[sactor].current_weapon] > 0)
  {
   actor[sactor].ammunition [actor[sactor].current_weapon] = wlist[actor[sactor].weapon [actor[sactor].current_weapon]].ammunition;
   if (game[0].unlimited_clips == 0
    && (user[actor[sactor].user].status != USTAT_AI || arena[0].ai_unlimited_clips == 0))
     actor[sactor].clips [actor[sactor].current_weapon] --;
   actor[sactor].reload [actor[sactor].current_weapon] = wlist[actor[sactor].weapon [actor[sactor].current_weapon]].reload_time * game[0].reload_time;
   actor[sactor].reload [actor[sactor].current_weapon] /= 10;
   display_actor_clips(sactor);
   display_actor_ammunition(sactor);
  }

  return success;

}



void actor_tracer(int sactor)
{

 int fired = WPN_LASER_TRACER; //actor[sactor].weapon [actor[sactor].current_weapon];

// switch(fired)
// {
//  case WPN_TRACKER:
// case WPN_SQUIRM:
//  case
//  fired = WPN_LASER_TRACER;
//  break;
// }

 float angle_used = actor[sactor].angle;

 int bull_x = cos(angle_used) * 500;
 bull_x *= actor [sactor].facing;
 int bull_y = sin(angle_used) * 500;

 float angle_passed = angle_used;
 if (angle_passed < 0) angle_used = (PI * 2) + angle_passed;
 if (actor[sactor].facing == -1) angle_used *= -1;

 int total_x_speed = bull_x;
 int total_y_speed = bull_y;
 
 discharge_weapon(fired, actor[sactor].x + bull_x + (actor[sactor].fire_x * actor[sactor].facing * GRAIN), actor[sactor].y + bull_y + (actor[sactor].total_dip * GRAIN) + (actor[sactor].fire_y * GRAIN), total_x_speed, total_y_speed, sactor, 0, angle_passed, 0, 0, 0);

}

void acquire_lock(int lactor)
{
 int i = 0;
 int closest = -1;
 int distance = 50000;
 int curr_dist = 0;
 int ix = actor[lactor].x;
 int iy = actor[lactor].y;
// int cross_x, cross_y;

 if (actor[lactor].weapon [actor[lactor].current_weapon] == WPN_TRACKER
     || actor[lactor].weapon [actor[lactor].current_weapon] == WPN_SEEKER)
 {
  ix = cos(actor[lactor].angle) * 80 * GRAIN;

  ix *= actor [lactor].facing;
 
  iy = sin(actor[lactor].angle) * 80 * GRAIN;

  ix += actor [lactor].x;
  iy += actor [lactor].y;

 }

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (i == lactor) continue;
  if (actor[i].aclass != ACLASS_PLAYER) continue;

//  if (actor[i].x < ix && actor[lactor].facing == 1) continue;
//  if (actor[i].x > ix && actor[lactor].facing == -1) continue;
  curr_dist = (abs(actor[i].x - ix) / GRAIN) + (abs(actor[i].y - iy) / GRAIN);
  if (curr_dist < distance)
  {
   distance = curr_dist;
   closest = i;
  }
 }

 actor[lactor].lock_on = closest;

}


void weapon_sound(int wpn, int x, int y)
{

 switch(wpn)
 {
  case WPN_ELECTRO:
     play_sound_pos(WAV_LIGHTNING1, 1000, 250, x, y);
     break;
  case WPN_INCENDIARIES:
  case WPN_SEMI_AUTO:
  case WPN_LMG:
     play_sound_pos(WAV_LMGSHOT, 1000, 250, x, y);
     break;
  case WPN_HMG:
     play_sound_pos(WAV_HMGSHOT, 1000, 250, x, y);
     break;
  case WPN_BLUNDER:
  case WPN_SGUN:
     play_sound_pos(WAV_SHOTGUN, 1000, 250, x, y);
     break;
  case WPN_BOMB:
  case WPN_FUNKY_BOMB:
  case WPN_TOXIN:
  case WPN_FRAG:
  case WPN_FIREBOMB:
  case WPN_GREN:
  case WPN_BOUNCY:
  case WPN_GAS_GREN:
  case WPN_DIRTBOMB:
  case WPN_GR_L:
     play_sound_pos(WAV_GR_L, 1000, 250, x, y);
     break;
  case WPN_RPG:
  case WPN_NUKE_M:
  case WPN_LR_R:
  case WPN_REMOTE_ROCKET:
  case WPN_REMOTE_ROCKET_C:
  case WPN_R_L:
     play_sound_pos(WAV_ROCKET, 1000, 250, x, y);
     break;
  // Note: tracker plays sound in discharge_weapon
  case WPN_NAPALM:
  case WPN_FTHROWER:
        play_sound_pos(WAV_ROCKET, 600, 250, x, y);
        break;
//void play_sound2(int sample, int frq, int vol, int pan)
  case WPN_SQUIRM:
  case WPN_SWARM:
     play_sound_pos(WAV_SWARM, 1000, 250, x, y);
     break;
  case WPN_CUBE:
     play_sound_pos(WAV_CUBE2, 1000, 250, x, y);
     break;
  case WPN_PLAS_R:
     play_sound_pos(WAV_PLASR, 1000, 250, x, y);
     break;
  case WPN_FIREBALL:
  case WPN_PLAS_C:
     play_sound_pos(WAV_PLASC, 1000, 250, x, y);
     break;
  case WPN_GRAPESHOT:
  case WPN_SCATTER:
     play_sound_pos(WAV_SCATTER, 1000, 250, x, y);
     break;
  case WPN_RECTIFIER:
     play_sound_pos(WAV_RECT, 1000, 250, x, y);
     break;
  case WPN_CLOD_OF_DIRT:
  case WPN_FLAKKER:
     play_sound_pos(WAV_AUTOCANNON, 1000, 250, x, y);
     break;
  case WPN_SLUG:
  case WPN_AUTOCANNON:
     play_sound_pos(WAV_FLAK, 1000, 250, x, y);
     break;
  case WPN_NEEDLER:
     play_sound_pos(WAV_NEEDLER, 1000, 250, x, y);
     break;
  case WPN_NIBBLER:
     play_sound_pos(WAV_NIBBLER, 1000, 250, x, y);
     break;
  case WPN_PLINKER:
     play_sound_pos(WAV_PLINKER, 1000, 250, x, y);
     break;
  case WPN_HUNTER:
     play_sound_pos(WAV_HUNTER, 1000, 250, x, y);
     break;
  case WPN_DISRUPTER_WAVE:
  case WPN_DISRUPTER:
     play_sound_pos(WAV_DISRUPTER, 1000, 250, x, y);
     break;
  case WPN_SHREDDER:
  case WPN_PRONGTHROWER:
     play_sound_pos(WAV_PRONG, 1000, 250, x, y);
     break;
  case WPN_FUMIGATOR:
     play_sound_pos(WAV_GAS_GREN, 1000, 250, x, y);
     break;
  default: return;
 }
}

/*
enum
{
BULLET_NONE,
BULLET_GRAPPLE,
BULLET_GREN_LAUNCHER,
BULLET_BULLET,
BULLET_ROCKET,
BULLET_GRENADE
};

enum
{
WPN_NULL,
WPN_NONE,
WPN_LMG,
WPN_HMG,
WPN_SGUN,
WPN_SLUG,
WPN_R_L,
WPN_GR_L,
WPN_GREN

};
*/

// This function doesn't do scatter
int discharge_weapon(int weapon_type, int x_pos, int y_pos, int x_speed, int y_speed, int owner, int spread_mult, float angle, int seed, int async, int lock)
{

int success = 0;
int dcount = 0;
//int bspeed = 10;
//int spread_mult = 100;
if (game[0].fast_bullets == 0) spread_mult = 45;

switch(weapon_type)
{
 case WPN_NULL:
 case WPN_NONE: return -1;
 case WPN_NEEDLER:
 case WPN_LMG:
 case WPN_SEMI_AUTO:
 case WPN_HMG:
 case WPN_NIBBLER:
// bspeed = game[game_played].bullet_speed;
 success = create_bullet(BULLET_BULLET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_SLUG:
// bspeed = game[game_played].bullet_speed;
 success = create_bullet(BULLET_FAT_BULLET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_SGUN:
// bspeed = game[game_played].bullet_speed;
 success = create_bullet(BULLET_BULLET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 spread_mult *= 3;
 for (dcount = 0; dcount < 10; dcount ++)
 {
//  create_bullet(BULLET_BULLET, x_pos + prand(500) - prand(500), y_pos + prand(500) - prand(500), x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0);
  create_bullet(BULLET_BULLET, x_pos, y_pos, x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 }
 break;
 case WPN_SCATTER:
// bspeed = game[game_played].bullet_speed;
 success = create_bullet(BULLET_BULLET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 spread_mult *= 3;
 for (dcount = 0; dcount < 4; dcount ++)
 {
  create_bullet(BULLET_BULLET, x_pos, y_pos, x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
//  create_bullet(BULLET_BULLET, x_pos + prand(500) - prand(500), y_pos + prand(500) - prand(500), x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0);
 }
 break;
 case WPN_BLUNDER:
// bspeed = game[game_played].bullet_speed;
 success = create_bullet(BULLET_BULLET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 spread_mult *= 5;
 for (dcount = 0; dcount < 20; dcount ++)
 {
//  create_bullet(BULLET_BULLET, x_pos + prand(500) - prand(500), y_pos + prand(500) - prand(500), x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0);
  create_bullet(BULLET_BULLET, x_pos, y_pos, x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 }
 break;
 case WPN_GR_L:
 success = create_bullet(BULLET_GREN_LAUNCHER, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_GREN:
 success = create_bullet(BULLET_GRENADE, x_pos, y_pos, x_speed, y_speed, owner, -1, 40, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_R_L:
 success = create_bullet(BULLET_ROCKET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 90, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_LR_R:
 success = create_bullet(BULLET_LR_ROCKET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 90, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_SWARM:
 success = create_bullet(BULLET_SWARM, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 90, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_SQUIRM:
// actor_tracer(owner);
 success = create_bullet(BULLET_SQUIRM, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, actor[owner].laser_y, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, actor[owner].laser_x, seed);
 break;
 case WPN_PLAS_R:
 success = create_bullet(BULLET_PLAS_R, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_PLAS_C:
 success = create_bullet(BULLET_PLAS_C, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_CUBE:
 success = create_bullet(BULLET_CUBE, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, trans_colours_array [prand(6)], seed);
 break;
 case WPN_ELECTRO:
 success = create_bullet(BULLET_ELECTRO, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, trans_colours_array [prand(6)], seed);
 break;
 case WPN_RECTIFIER:
 success = create_bullet(BULLET_RECTIFIER, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_NUKE_M:
 success = create_bullet(BULLET_NUKE_M, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_FRAG:
 success = create_bullet(BULLET_FRAG, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 60, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_FLAKKER:
// if (game[0].fast_bullets < 2)
//  success = create_bullet(BULLET_FLAK, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 7 + prand(3) + (game[0].fast_bullets == 0) * 20, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
//   else
//    success = create_bullet(BULLET_FLAK, x_pos, y_pos, x_speed / 100, y_speed / 100, owner, wlist[weapon_type].damage, 7 + prand(3) + (game[0].fast_bullets == 0) * 20, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 if (game[0].fast_bullets == 0)
  success = create_bullet(BULLET_FLAK, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 23 + prand(3), wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 if (game[0].fast_bullets == 1)
  success = create_bullet(BULLET_FLAK, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 8 + prand(3), wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 if (game[0].fast_bullets == 2)
  success = create_bullet(BULLET_FLAK, x_pos, y_pos, x_speed / 10, y_speed / 10, owner, wlist[weapon_type].damage, 2, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_AUTOCANNON:
// bspeed = game[game_played].bullet_speed;
 success = create_bullet(BULLET_AUTOCANNON, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_FTHROWER:
 success = create_bullet(BULLET_FLAME, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 20, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_FIREBOMB:
 success = create_bullet(BULLET_FIREBOMB, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 80, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_PLINKER:
 success = create_bullet(BULLET_PLINKER, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 90, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_REMOTE_ROCKET:
 success = create_bullet(BULLET_REMOTE_ROCKET, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 if (success != -1)// && async == 0)
 {
  actor[owner].remote_control = REMOTE_ROCKET;
  actor[owner].remote_thing = success;
 }
 break;
 case WPN_TRACKER:
 success = create_bullet(BULLET_TRACKER, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, lock, seed);
 if (lock == -1)
  play_sound_pos(WAV_ROCKET, 1000, 250, x_pos, y_pos);
   else play_sound_pos(WAV_TRACKER, 1000, 250, x_pos, y_pos);
 break;
 case WPN_SEEKER:
 success = create_bullet(BULLET_SEEKER, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, lock, seed);
 if (lock == -1)
  play_sound_pos(WAV_ROCKET, 1000, 250, x_pos, y_pos);
   else play_sound_pos(WAV_TRACKER, 1000, 250, x_pos, y_pos);
 break;
 case WPN_REMOTE_ROCKET_C:
 success = create_bullet(BULLET_REMOTE_ROCKET_C, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 if (success != -1) // && async == 0)
 {
  actor[owner].remote_control = REMOTE_ROCKET_C;
  actor[owner].remote_thing = success;
 }
 break;
 case WPN_LASER_BEAM:
 success = create_bullet(BULLET_LASER_BEAM, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_LASER_TRACER:
 success = create_bullet(BULLET_LASER_TRACER, x_pos, y_pos, x_speed, y_speed, owner, 0, 0, 0, 0, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_LASER_PULSE:
 success = create_bullet(BULLET_LASER_PULSE, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
   actor[owner].laser_strength = 2;
/* if (success != -1)
 {
  if (actor[owner].laser_strength == 0)
   actor[owner].laser_strength = 16;
    else
     actor[owner].laser_strength --;
 }*/
 break;
 case WPN_FUNKY_BOMB:
 success = create_bullet(BULLET_FUNKY_BOMB, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 break;
 case WPN_HUNTER:
 success = create_bullet(BULLET_HUNTER_BOMB, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_DISRUPTER:
 success = create_bullet(BULLET_DISRUPTER, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_TOXIN:
 success = create_bullet(BULLET_TOX_BOMB, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 90, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_RPG:
 success = create_bullet(BULLET_RPG, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 90, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_BOMB:
 success = create_bullet(BULLET_BOMB, x_pos, y_pos, x_speed, y_speed, owner, -1, 30 + prand(40), wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_GRAPESHOT:
 success = create_bullet(BULLET_GRAPESHOT, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 spread_mult *= 3;
 for (dcount = 0; dcount < 8; dcount ++)
 {
//  create_bullet(BULLET_GRAPESHOT, x_pos + prand(500) - prand(500), y_pos + prand(500) - prand(500), x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0);
  create_bullet(BULLET_GRAPESHOT, x_pos, y_pos, x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 }
 break;
 case WPN_BOUNCY:
 success = create_bullet(BULLET_BOUNCY, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 150, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, -1, seed);
 break;
 case WPN_PRONGTHROWER:
 success = create_bullet(BULLET_PRONG, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_GAS_GREN:
 success = create_bullet(BULLET_GAS_GREN, x_pos, y_pos, x_speed, y_speed, owner, -1, 50, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_DIRTBOMB:
 success = create_bullet(BULLET_DIRTBOMB, x_pos, y_pos, x_speed, y_speed, owner, -1, 50, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_CLOD_OF_DIRT:
 success = create_bullet(BULLET_DIRTCLOD, x_pos, y_pos, x_speed, y_speed, owner, 0, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_IMPLODER:
 success = create_bullet(BULLET_IMPLODER, x_pos, y_pos, x_speed, y_speed, owner, 0, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_FIREBALL:
 success = create_bullet(BULLET_FIREBALL, x_pos, y_pos, x_speed, y_speed, owner, 0, 150, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_INCENDIARIES:
 success = create_bullet(BULLET_INCENDIARY, x_pos, y_pos, x_speed, y_speed, owner, 0, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_FUMIGATOR:
 success = create_bullet(BULLET_GAS, x_pos, y_pos, x_speed, y_speed, owner, 5, 200 + prand(50), wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_NAPALM:
 success = create_bullet(BULLET_NAPALM, x_pos, y_pos, x_speed, y_speed, owner, 0, 100 + prand(50), wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_SHREDDER:
 spread_mult *= 3;
 for (dcount = 0; dcount < 4; dcount ++)
 {
  create_bullet(BULLET_SHREDDER, x_pos, y_pos, x_speed + prand(spread_mult) - prand(spread_mult), y_speed + prand(spread_mult) - prand(spread_mult), owner, wlist[weapon_type].damage, 150 + prand(50), wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 }
 break;
 case WPN_DISRUPTER_WAVE:
 success = create_bullet(BULLET_DISRUPTER_WAVE, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 25, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;
 case WPN_SMITER:
 success = create_bullet(BULLET_SMITER, x_pos, y_pos, x_speed, y_speed, owner, wlist[weapon_type].damage, 0, wlist[weapon_type].is_bullet, wlist[weapon_type].bullet_weight, angle, actor[owner].facing, 0, seed);
 break;


}


// create_bullet(type, x_pos, y_pos, x_speed, y_speed, owner, damage, fuse);

return success;

}


void actor_grapple(int sactor)
{

// if (actor[sactor].recycle > 0) return;
 actor[sactor].prevent_jump = 1;

 if (actor[sactor].grapple_recycle > 0) return;

 int bull_x = cos(actor[sactor].angle) * 500;
 bull_x *= actor [sactor].facing;
 int bull_y = sin(actor[sactor].angle) * 500;

 int bull_x_speed = bull_x * actor[sactor].grapple_power;
 bull_x_speed /= 10;
 int bull_y_speed = bull_y * actor[sactor].grapple_power;
 bull_y_speed /= 10;

 int grapple = create_bullet(BULLET_GRAPPLE, actor[sactor].x + bull_x, actor[sactor].y + bull_y, bull_x_speed + actor[sactor].x_speed, bull_y_speed + actor[sactor].y_speed, sactor, -1, 0, 0, 30, actor[sactor].angle, actor[sactor].facing, 0, 0);

 if (grapple != -1)
 {
  actor[sactor].grapple_x = bullet[grapple].x;
  actor[sactor].grapple_y = bullet[grapple].y;
  actor[sactor].grapple_bullet = grapple;
  actor[sactor].grapple_recycle += 20;
 }
 
// if (create_bullet(BULLET_GRAPPLE, actor[sactor].x + bull_x, actor[sactor].y + bull_y, bull_x + actor[sactor].x_speed, bull_y + actor[sactor].y_speed, sactor) != -1)
// {
//    actor[sactor].recycle += 30;
// }

 play_sound_pos(WAV_GRAPPLE, 1000, 250, actor[sactor].x, actor[sactor].y);
}

/*
void command_buffer(void)
{

int i, j;

for (i = 0; i < NO_CMDS; i ++)
{
 for (j = 0; j < NO_ACTORS; j ++)
 {
  if (cmd_buffer [j] [i]) issue_command(j, i);
 }
}

}
*/

void get_movement(void)
{







   char was_changing_weapon [3] = {0, 0, 0};
   char replace_weapon [3] = {0, 0, 0};

   if (player [1].changing_weapon == 1)
      was_changing_weapon [1] = 1;
   if (player [2].changing_weapon == 1)
      was_changing_weapon [2] = 1;

   if (player [1].change_delay > 0)
    player [1].change_delay --;
   if (player [2].change_delay > 0)
    player [2].change_delay --;

   player [1].changing_weapon = 0;
   player [2].changing_weapon = 0;

   if (keypress [CMD_KEY_SCORE1] == 1)
   {
    player[1].show_scores = 1;
   } else player[1].show_scores = 0;
   
   if (keypress [CMD_KEY_SCORE2] == 1)
   {
    player[2].show_scores = 1;
   } else player[2].show_scores = 0;

   if (keypress [CMD_KEY_NAMES1] == 1)
   {
    if (player[1].show_names == 1)
     player[1].show_names = -30;
      else
       {
        if (player[1].show_names >= 0)
         player[1].show_names = 30;
       }
   }
   
   if (keypress [CMD_KEY_NAMES2] == 1)
   {
    if (player[2].show_names == 1)
     player[2].show_names = -30;
      else
       {
        if (player[2].show_names >= 0)
         player[2].show_names = 30;
       }
   }
   

//  if (actor[player[1].actor_controlled].aclass != ACLASS_GHOST)
if (user[player[1].user].out_of_lives == 0)
{
 if (actor[player[1].actor_controlled].aclass == ACLASS_GHOST && game_over == 0)
 {
//   if (player [1].change_delay > 0)
//    player [1].change_delay --;
  if (keypress [CMD_KEY_UP1] == 1 && player[1].change_delay == 0)
  {
   player[1].ingame_slot --;
   if (player[1].ingame_slot < 0)
    player[1].ingame_slot = arena[0].ingame_slots - 1;
   player[1].change_delay = 5;
   play_sound(WAV_SHORT_BLIP);
  }
  if (keypress [CMD_KEY_DOWN1] == 1 && player[1].change_delay == 0)
  {
   player[1].ingame_slot ++;
   if (player[1].ingame_slot >= arena[0].ingame_slots)
    player[1].ingame_slot = 0;
   player[1].change_delay = 5;
   play_sound(WAV_SHORT_BLIP);
  }
  if (keypress [CMD_KEY_DOWN1] == 0 && keypress [CMD_KEY_UP1] == 0)
   player[1].change_delay = 0;
  if (keypress [CMD_KEY_CHANGE1] && keypress [CMD_KEY_SHOOT1])
   issue_command(1, CMD_SPAWN_0 + player[1].ingame_slot);
  if (user[player[1].user].resting_time == 0)
   issue_command(1, CMD_SPAWN_0 + player[1].ingame_slot);
//      spawn_actor(player[1].actor_controlled);
 }
   else
  {
   if (keypress [CMD_KEY_JUMP1])
   {
    if (keypress [CMD_KEY_CHANGE1])
    {
     issue_command(1, CMD_GRAPPLE);
    } else
    {
     issue_command(1, CMD_JUMP);
     actor[player[1].actor_controlled].prevent_pickup = 0;
    }
   } else
   {
      actor[player[1].actor_controlled].prevent_jump = 0;
      if (keypress [CMD_KEY_CHANGE1])
      {
       player [1].changing_weapon = 1;
//       display_player_ammunition(1);
//       display_player_clips(1);
       if (keypress [CMD_KEY_SHOOT1] && actor[player[1].actor_controlled].prevent_pickup == 0)
          switch(player [1].slot_selected)
          {
           case 0: issue_command(1, CMD_REWEAP_0); break;
           case 1: issue_command(1, CMD_REWEAP_1); break;
           case 2: issue_command(1, CMD_REWEAP_2); break;
           case 3: issue_command(1, CMD_REWEAP_3); break;
          }
       
      } else actor[player[1].actor_controlled].prevent_pickup = 0;
   }
   if (keypress [CMD_KEY_SHOOT1] && keypress [CMD_KEY_CHANGE1] == 0) issue_command(1, CMD_SHOOT);
   if (keypress [CMD_KEY_GRAPPLE1]) issue_command(1, CMD_GRAPPLE);
//   if (keypress [CMD_KEY_SHOOT1] && replace_weapon [1] == 0) issue_command(1, CMD_SHOOT);
   if (keypress [CMD_KEY_UP1]) issue_command(1, CMD_UP);
   if (keypress [CMD_KEY_DOWN1]) issue_command(1, CMD_DOWN);
   // left & right must come after change
   if (keypress [CMD_KEY_LEFT1] && !keypress [CMD_KEY_RIGHT1])
   {
    if (player [1].changing_weapon == 1)
    {
     if (player [1].change_delay == 0)
     {
      player [1].slot_selected --;
      if (player [1].slot_selected == -1) player [1].slot_selected = actor[player [1].actor_controlled].weapon_slots - 1;
      player [1].change_delay = 20;
      display_player_ammunition(1);
      display_player_clips(1);
     }
    } else
          {
             issue_command(1, CMD_LEFT);
             player[1].moving = -1;
          }
   }
   
   if (keypress [CMD_KEY_RIGHT1])
   {
    if (player [1].changing_weapon == 1)
    {
     if (player [1].change_delay == 0)
     {
      player [1].slot_selected ++;
      if (player [1].slot_selected == actor[player [1].actor_controlled].weapon_slots) player [1].slot_selected = 0;
      player [1].change_delay = 20;
      display_player_ammunition(1);
      display_player_clips(1);
     }
    } else
          {
           if (player[1].moving == 1 && keypress [CMD_KEY_LEFT1])
           {
             if (player[1].has_dug == 0) issue_command(1, CMD_DIG);
             issue_command(1, CMD_RIGHT);
             player[1].has_dug = 1;
             player[1].moving = 1;
           } else
           if (player[1].moving == -1 && keypress [CMD_KEY_LEFT1])
           {
             if (player[1].has_dug == 0) issue_command(1, CMD_DIG);
             issue_command(1, CMD_LEFT);
             player[1].moving = -1;
             player[1].has_dug = 1;
           } else
                 {
                  issue_command(1, CMD_RIGHT); // finally!
                  player[1].moving = 1;
                 }
          }
   } else if (keypress [CMD_KEY_LEFT1] == 0) player [1].change_delay = 0; // if neither direction is being pressed, reset the change delay

   if (!keypress [CMD_KEY_LEFT1] || !keypress [CMD_KEY_RIGHT1]) player[1].has_dug = 0;
 } // end else
} // end if not out of lives

 if (no_players == 2) // && actor[player[2].actor_controlled].aclass != ACLASS_GHOST)
 {
 if (user[player[2].user].out_of_lives == 0)
 {
 if (actor[player[2].actor_controlled].aclass == ACLASS_GHOST && game_over == 0)
 {
//   if (player [1].change_delay > 0)
//    player [1].change_delay --;
  if (keypress [CMD_KEY_UP2] == 1 && player[2].change_delay == 0)
  {
   player[2].ingame_slot --;
   if (player[2].ingame_slot < 0)
    player[2].ingame_slot = arena[0].ingame_slots - 1;
   player[2].change_delay = 5;
   play_sound(WAV_SHORT_BLIP);
  }
  if (keypress [CMD_KEY_DOWN2] == 1 && player[2].change_delay == 0)
  {
   player[2].ingame_slot ++;
   if (player[2].ingame_slot >= arena[0].ingame_slots)
    player[2].ingame_slot = 0;
   player[2].change_delay = 5;
   play_sound(WAV_SHORT_BLIP);
  }
  if (keypress [CMD_KEY_DOWN2] == 0 && keypress [CMD_KEY_UP2] == 0)
   player[2].change_delay = 0;
  if (keypress [CMD_KEY_CHANGE2] && keypress [CMD_KEY_SHOOT2])
   issue_command(2, CMD_SPAWN_0 + player[2].ingame_slot);
  if (user[player[2].user].resting_time == 0)
   issue_command(2, CMD_SPAWN_0 + player[2].ingame_slot);
//      spawn_actor(player[1].actor_controlled);
 }
   else
  {
   if (keypress [CMD_KEY_JUMP2])
   {
    if (keypress [CMD_KEY_CHANGE2])
    {
     issue_command(2, CMD_GRAPPLE);
    } else
    {
     issue_command(2, CMD_JUMP);
     actor[player[2].actor_controlled].prevent_pickup = 0;
    }
   } else
   {
      actor[player[2].actor_controlled].prevent_jump = 0;
      if (keypress [CMD_KEY_CHANGE2])
      {
       player [2].changing_weapon = 1;
       display_player_ammunition(2);
       display_player_clips(2);
       if (keypress [CMD_KEY_SHOOT2] && actor[player[2].actor_controlled].prevent_pickup == 0)
          switch(player [2].slot_selected)
          {
           case 0: issue_command(2, CMD_REWEAP_0); break;
           case 1: issue_command(2, CMD_REWEAP_1); break;
           case 2: issue_command(2, CMD_REWEAP_2); break;
           case 3: issue_command(2, CMD_REWEAP_3); break;
          }
       
      } else actor[player[2].actor_controlled].prevent_pickup = 0;
   }
   if (keypress [CMD_KEY_SHOOT2] && keypress [CMD_KEY_CHANGE2] == 0) issue_command(2, CMD_SHOOT);
//   if (keypress [CMD_KEY_SHOOT2] && replace_weapon [2] == 0) issue_command(2, CMD_SHOOT);
   if (keypress [CMD_KEY_GRAPPLE2]) issue_command(2, CMD_GRAPPLE);
   if (keypress [CMD_KEY_UP2]) issue_command(2, CMD_UP);
   if (keypress [CMD_KEY_DOWN2]) issue_command(2, CMD_DOWN);
   // left & right must come after change
   if (keypress [CMD_KEY_LEFT2] && !keypress [CMD_KEY_RIGHT2])
   {
    if (player [2].changing_weapon == 1)
    {
     if (player [2].change_delay == 0)
     {
      player [2].slot_selected --;
      if (player [2].slot_selected == -1) player [2].slot_selected = actor[player [2].actor_controlled].weapon_slots - 1;
      player [2].change_delay = 20;
      display_player_ammunition(2);
      display_player_clips(2);
     }
    } else
          {
             issue_command(2, CMD_LEFT);
             player[2].moving = -1;
          }
   }
   
   if (keypress [CMD_KEY_RIGHT2])
   {
    if (player [2].changing_weapon == 1)
    {
     if (player [2].change_delay == 0)
     {
      player [2].slot_selected ++;
      if (player [2].slot_selected == actor[player [2].actor_controlled].weapon_slots) player [2].slot_selected = 0;
      player [2].change_delay = 20;
      display_player_ammunition(2);
      display_player_clips(2);
     }
    } else
          {
           if (player[2].moving == 1 && keypress [CMD_KEY_LEFT2])
           {
             if (player[2].has_dug == 0) issue_command(2, CMD_DIG);
             issue_command(2, CMD_RIGHT);
             player[2].has_dug = 1;
             player[2].moving = 1;
           } else
           if (player[2].moving == -1 && keypress [CMD_KEY_LEFT2])
           {
             if (player[2].has_dug == 0) issue_command(2, CMD_DIG);
             issue_command(2, CMD_LEFT);
             player[2].has_dug = 1;
             player[2].moving = -1;
           } else
                 {
                   issue_command(2, CMD_RIGHT); // finally!
                   player[2].moving = 1;
                 }
          }
   } else
           if (keypress [CMD_KEY_LEFT2] == 0) player [2].change_delay = 0; // if neither direction is being pressed, reset the change delay

   if (!keypress [CMD_KEY_LEFT2] || !keypress [CMD_KEY_RIGHT2]) player[2].has_dug = 0;


  } // end respawn input else
  } // end if not out of lives
  } // end if (no_players == 2)




  /*
   
   if (keypress [CMD_KEY_JUMP2])
   {
    if (keypress [CMD_KEY_CHANGE2])
    {
     issue_command(2, CMD_GRAPPLE);
    } else issue_command(2, CMD_JUMP);
   } else
   {
      actor[2].prevent_jump = 0;
      if (keypress [CMD_KEY_CHANGE2])
      {
       player [2].changing_weapon = 1;
       if (keypress [CMD_KEY_SHOOT2]) replace_weapon [2] = 1;
      }
   }
   if (keypress [CMD_KEY_SHOOT2] && replace_weapon [2] == 0) issue_command(2, CMD_SHOOT);
   if (keypress [CMD_KEY_UP2]) issue_command(2, CMD_UP);
   if (keypress [CMD_KEY_DOWN2]) issue_command(2, CMD_DOWN);
   if (keypress [CMD_KEY_LEFT2])
   {
    if (player [2].changing_weapon == 1)
    {
     if (player [2].change_delay == 0)
     {
      player [2].slot_selected --;
      if (player [2].slot_selected == -1) player [2].slot_selected = actor[player [2].actor_controlled].weapon_slots - 1;
      player [2].change_delay = 20;
      display_player_ammunition(2);
      display_player_clips(2);
     }
    } else issue_command(2, CMD_LEFT);
   }
   
   if (keypress [CMD_KEY_RIGHT2])
   {
    if (player [2].changing_weapon == 1)
    {
     if (player [2].change_delay == 0)
     {
      player [2].slot_selected ++;
      if (player [2].slot_selected == actor[player [2].actor_controlled].weapon_slots) player [2].slot_selected = 0;
      player [2].change_delay = 20;
      display_player_ammunition(2);
      display_player_clips(2);
     }
    } else issue_command(2, CMD_RIGHT);
   } else if (keypress [CMD_KEY_LEFT2] == 0) player [2].change_delay = 0; // if neither direction is being pressed, reset the change delay
*/
   if (was_changing_weapon [1] == 1 && player [1].changing_weapon == 0)
   {
    switch(player [1].slot_selected)
    {
        case 0:
        if (replace_weapon [1] == 1) issue_command(1, CMD_REWEAP_0);
        issue_command(1, CMD_CHWEAP_0); break;
        case 1:
        if (replace_weapon [1] == 1) issue_command(1, CMD_REWEAP_1);
        issue_command(1, CMD_CHWEAP_1); break;
        case 2:
        if (replace_weapon [1] == 1) issue_command(1, CMD_REWEAP_2);
        issue_command(1, CMD_CHWEAP_2); break;
        case 3:
        if (replace_weapon [1] == 1) issue_command(1, CMD_REWEAP_3);
        issue_command(1, CMD_CHWEAP_3); break;
    }
   }

   if (no_players == 2 && was_changing_weapon [2] == 1 && player [2].changing_weapon == 0)
   {
    switch(player [2].slot_selected)
    {
        case 0:
        if (replace_weapon [2] == 1) issue_command(2, CMD_REWEAP_0);
        issue_command(2, CMD_CHWEAP_0); break;
        case 1:
        if (replace_weapon [2] == 1) issue_command(2, CMD_REWEAP_1);
        issue_command(2, CMD_CHWEAP_1); break;
        case 2:
        if (replace_weapon [2] == 1) issue_command(2, CMD_REWEAP_2);
        issue_command(2, CMD_CHWEAP_2); break;
        case 3:
        if (replace_weapon [2] == 1) issue_command(2, CMD_REWEAP_3);
        issue_command(2, CMD_CHWEAP_3); break;
    }
   }

   
}

// Gets all relevant keys being pressed.
// Doesn't clear the keypress array, so it can be called multiple times
// to build up
void get_keypress(void)
{

int i;

for (i = 0; i < NO_CMDS; i ++)
{
   if (key [(int) cmd_key [i]]) keypress [i] = 1;
}

}


void remote_control(int ractor, int rcom)
{
int thc;

switch(actor[ractor].remote_control)
{
 case REMOTE_ROCKET_C:
 case REMOTE_ROCKET:
 thc = actor[ractor].remote_thing;
 switch(rcom)
 {
  case CMD_LEFT:
  if (bullet[thc].facing == 1)
  {
   bullet[thc].angle -= .1;
   if (bullet[thc].angle < PI / -2)
   {
    bullet[thc].facing = -1;
    bullet[thc].angle = (PI / -2);// + bullet[thc].angle + PI / 2;
   }
  } else
  {
   bullet[thc].angle += .1;
   if (bullet[thc].angle > PI / 2)
   {
    bullet[thc].facing = 1;
    bullet[thc].angle = (PI / 2);// + bullet[thc].angle + PI / 2;
   }
  }
  break;
  case CMD_RIGHT:
  if (bullet[thc].facing == -1)
  {
   bullet[thc].angle -= .1;
   if (bullet[thc].angle < PI / -2)
   {
    bullet[thc].facing = 1;
    bullet[thc].angle = (PI / -2);// + bullet[thc].angle + PI / 2;
   }
  } else
  {
   bullet[thc].angle += .1;
   if (bullet[thc].angle > PI / 2)
   {
    bullet[thc].facing = -1;
    bullet[thc].angle = (PI / 2);// + bullet[thc].angle + PI / 2;
   }
  }
  break;
  case CMD_SHOOT:
  if (actor[ractor].recycle == 0)
  {
   bullet[thc].fuse = 1;
   actor[ractor].recycle = 20;
   //async_smart_destroy(thc);
  }
  break;
  case CMD_JUMP:
  actor[ractor].remote_control = REMOTE_NONE;
  actor[ractor].remote_thing = -1;
  break;
 }
 break;

}

// if (bullet[thc].facing == 1)
  bullet[thc].base_angle = (bullet[thc].angle + (PI / 2)) * (128 / PI);
//   else
//    bullet[thc].base_angle = 256 - (bullet[thc].angle + (PI / 2)) * (128 / PI);
 bullet[thc].angle = ((float) bullet[thc].base_angle / 128 * PI) - (PI / 2);
// bullet[thc].base_angle = (bullet[thc].angle) * (256 / PI);
// bullet[thc].angle = ((float) bullet[thc].base_angle / 256 * PI);// - (PI / 2);

// text_mode(0);
//  textprintf(screen, large_font, 150, 460, 15, "Angle : %f %i  ", bullet[thc].angle, bullet[thc].base_angle);


}




void init_cmds(void)
{

/*  set_config_int("Key Definitions", "CMD_KEY_UP1", cmd_key [CMD_KEY_UP1]);
 get_config_int("Key Definitions", "CMD_KEY_UP1", KEY_8_PAD);*/

 cmd_key [CMD_KEY_UP1] = get_config_int("KeyDef", "CMD_KEY_UP1", KEY_8_PAD);
 cmd_key [CMD_KEY_DOWN1] = get_config_int("KeyDef", "CMD_KEY_DOWN1", KEY_2_PAD);
 cmd_key [CMD_KEY_LEFT1] = get_config_int("KeyDef", "CMD_KEY_LEFT1", KEY_4_PAD);
 cmd_key [CMD_KEY_RIGHT1] = get_config_int("KeyDef", "CMD_KEY_RIGHT1", KEY_6_PAD);
 cmd_key [CMD_KEY_JUMP1] = get_config_int("KeyDef", "CMD_KEY_JUMP1", KEY_ENTER_PAD);;
 cmd_key [CMD_KEY_SHOOT1] = get_config_int("KeyDef", "CMD_KEY_SHOOT1", KEY_0_PAD);
 cmd_key [CMD_KEY_CHANGE1] = get_config_int("KeyDef", "CMD_KEY_CHANGE1", KEY_PLUS_PAD);
 cmd_key [CMD_KEY_SCORE1] = get_config_int("KeyDef", "CMD_KEY_SCORE1", KEY_SLASH_PAD);
 cmd_key [CMD_KEY_NAMES1] = get_config_int("KeyDef", "CMD_KEY_NAMES1", KEY_ASTERISK);
 cmd_key [CMD_KEY_GRAPPLE1] = get_config_int("KeyDef", "CMD_KEY_GRAPPLE1", KEY_5_PAD);
 cmd_key [CMD_KEY_UP2] = get_config_int("KeyDef", "CMD_KEY_UP2", KEY_W);
 cmd_key [CMD_KEY_DOWN2] = get_config_int("KeyDef", "CMD_KEY_DOWN2", KEY_X);
 cmd_key [CMD_KEY_LEFT2] = get_config_int("KeyDef", "CMD_KEY_LEFT2", KEY_A);
 cmd_key [CMD_KEY_RIGHT2] = get_config_int("KeyDef", "CMD_KEY_RIGHT2", KEY_D);
 cmd_key [CMD_KEY_JUMP2] = get_config_int("KeyDef", "CMD_KEY_JUMP2", KEY_TAB);;
 cmd_key [CMD_KEY_SHOOT2] = get_config_int("KeyDef", "CMD_KEY_SHOOT2", KEY_S);
 cmd_key [CMD_KEY_CHANGE2] = get_config_int("KeyDef", "CMD_KEY_CHANGE2", KEY_TILDE);
 cmd_key [CMD_KEY_SCORE2] = get_config_int("KeyDef", "CMD_KEY_SCORE2", KEY_Q);
 cmd_key [CMD_KEY_NAMES2] = get_config_int("KeyDef", "CMD_KEY_NAMES2", KEY_LSHIFT);
 cmd_key [CMD_KEY_GRAPPLE2] = get_config_int("KeyDef", "CMD_KEY_GRAPPLE2", KEY_E);
 
}


