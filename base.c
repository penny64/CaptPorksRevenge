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

File: base.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions for placing and maintaining team bases and T&H bases
 - various functions for scoring and events in the more complicated
   game types (Capture the Flag etc)
 
*/

#include <math.h>
#include <string.h>

#include "allegro.h"

#include "config.h"
#include "globvar.h"

#include "actor.h"
#include "prand.h"
#include "bullet.h"
#include "pickup.h"
#include "objhead.h"
#include "score.h"
#include "display.h"
#include "grid.h"
#include "cloud.h"
#include "sound.h"
#include "score.h"

char *team_name(int team);
void drop_porkball(int pactor);
void teleport_team_base(int bteam);
void teleport_th_base(int th);

void init_bases(void)
{
 int i;

 for (i = 0; i < NO_TEAMS; i ++)
 {
   team[i].base_x = 0;
   team[i].base_y = 0;
   team[i].flag_x = 0;
   team[i].flag_x = 0;
   team[i].flag_at_base = 1;
 }

 for (i = 0; i < 5; i ++)
 {
  arena[0].th_base_x [i] = 0;
  arena[0].th_base_y [i] = 0;
  arena[0].th_base_taken [i] = TEAM_NONE;
  arena[0].th_base_counter [i] = 0;
  arena[0].th_base_move [i] = 0;
  arena[0].th_base_pickup [i] = -1;
  arena[0].th_base_direction [i] = 1;
  arena[0].th_base_angle [i] = 128;
 }
 
}


void prepare_bases(void)
{

 int i;
 int bpick = 0;

 if (serial[0].game_type == SERIAL_CLIENT) return;

 switch(game[0].game_type)
 {
  default: return; // no bases in this game

  case GAME_GRAIL:
  drop_pickups(PICKUP_GRAIL, 0);
  for (i = 0; i < NO_TEAMS; i ++)
  {
//   textprintf(screen, large_font, 100, 100 + i * 15, 15, "Team %i Active %i Size %i", i, team[i].active, team[i].size);
   if (team[i].active == 1)
   {
    bpick = drop_pickups(PICKUP_BASE, i);
    team[i].base_x = pickup[bpick].x;
    team[i].base_y = pickup[bpick].x;
   }
  }
  rest(2000);
  break;
  case GAME_CAP_FLAG:
  for (i = 0; i < NO_TEAMS; i ++)
  {
   if (team[i].active == 1)
   {
    bpick = drop_pickups(PICKUP_BASE, i);
    team[i].base_x = pickup[bpick].x;
    team[i].base_y = pickup[bpick].y;
    create_pickup(PICKUP_FLAG, i, team[i].base_x, team[i].base_y, 0, 0, 0, -1, -1);
   }
  }
  break;
  case GAME_PORKBALL:
  for (i = 0; i < NO_TEAMS; i ++)
  {
   if (team[i].active == 1)
   {
    bpick = drop_pickups(PICKUP_BASE, i);
    team[i].base_x = pickup[bpick].x;
    team[i].base_y = pickup[bpick].x;
   }
  }
  init_porkball(NO_ACTORS - 1);
  drop_porkball(NO_ACTORS - 1);
  break;
  case GAME_TAKEHOLD:
  for (i = 0; i < game[0].th_base_no; i ++)
  {
    bpick = drop_pickups(PICKUP_TH_BASE, 0);
    arena[0].th_base_x [i] = pickup[bpick].x;
    arena[0].th_base_y [i] = pickup[bpick].y;
    arena[0].th_base_pickup [i] = bpick;
    pickup[bpick].th_index = i;
  }
  break;
  
 }


}


void near_base(int bactor)
{

   int i;
   int j;
   char occupied = 0;

 switch(game[0].game_type)
 {
  default: return;
  case GAME_GRAIL:
  if (actor[bactor].has_grail == 0 || serial[0].game_type == SERIAL_CLIENT)
   return;
  break;
  case GAME_CAP_FLAG:
  if (actor[bactor].has_flag == -1 || serial[0].game_type == SERIAL_CLIENT)
   return;
  break;
  case GAME_TAKEHOLD:
  if (serial[0].game_type == SERIAL_CLIENT) return;
  for (i = 0; i < game[0].th_base_no; i ++)
  {
   occupied = 0;
   if (arena[0].th_base_taken [i] == actor[bactor].team) continue;
   if (actor[bactor].x > arena[0].th_base_x [i] - (15 * GRAIN)
    && actor[bactor].x < arena[0].th_base_x [i] + (15 * GRAIN)
    && actor[bactor].y > arena[0].th_base_y [i] - (15 * GRAIN)
    && actor[bactor].y < arena[0].th_base_y [i] + (15 * GRAIN))
   {
    if (arena[0].th_base_taken [i] != TEAM_NONE)
    {
     for (j = 0; j < NO_ACTORS; j ++)
     {
      if (actor[j].aclass != ACLASS_PLAYER) continue;
      if (actor[j].team == actor[bactor].team) continue;
      if (actor[j].x > arena[0].th_base_x [i] - (15 * GRAIN)
       && actor[j].x < arena[0].th_base_x [i] + (15 * GRAIN)
       && actor[j].y > arena[0].th_base_y [i] - (15 * GRAIN)
       && actor[j].y < arena[0].th_base_y [i] + (15 * GRAIN))
      {
       occupied = 1;
      }
     }
     if (occupied == 0)
     {
       arena[0].th_base_taken [i] = actor[bactor].team;
       arena[0].th_base_counter [i] = 900;
       arena[0].th_base_speed [i] = 0;
       arena[0].th_base_direction [i] *= -1;
       score_message(SMESSAGE_CAPTURE, actor[bactor].user, actor[bactor].team, 0);
     }
    }
     else
      {
       arena[0].th_base_taken [i] = actor[bactor].team;
       arena[0].th_base_counter [i] = 900;
       arena[0].th_base_speed [i] = 0;
       arena[0].th_base_direction [i] *= -1;
       score_message(SMESSAGE_CAPTURE, actor[bactor].user, actor[bactor].team, 0);
//       arena[0].th_
      }
   }
  }
  return;
 }

 if (actor[bactor].x > team[actor[bactor].team].base_x - (15 * GRAIN)
  && actor[bactor].x < team[actor[bactor].team].base_x + (15 * GRAIN)
  && actor[bactor].y > team[actor[bactor].team].base_y - (15 * GRAIN)
  && actor[bactor].y < team[actor[bactor].team].base_y + (15 * GRAIN))
 {
  switch(game[0].game_type)
  {
   default: return;
   case GAME_GRAIL:
   score_message(SMESSAGE_GRAIL, actor[bactor].user, actor[bactor].team, 0);
   score_event(bactor, SCORE_GRAIL, 0);
   actor[bactor].has_grail = 0;
   drop_pickups(PICKUP_GRAIL, 0);
   team[actor[bactor].team].base_counter ++;
   if (team[actor[bactor].team].base_counter >= 5)
   {
    teleport_team_base(actor[bactor].team);
    team[actor[bactor].team].base_counter = 0;
   }
//   play_sound(WAV_SCORE);
   break;
   case GAME_CAP_FLAG:
   if (actor[bactor].has_flag == -1
       || team[actor[bactor].team].flag_at_base == 0) return;
   score_event(actor[bactor].team, SCORE_FLAG, actor[bactor].has_flag);
   score_message(SMESSAGE_FLAG, actor[bactor].user, actor[bactor].has_flag, 0);

   team[actor[bactor].team].base_counter ++;
   if (team[actor[bactor].team].base_counter >= 5)
   {
    teleport_team_base(actor[bactor].team);
    team[actor[bactor].team].base_counter = 0;
   }
   create_pickup(PICKUP_FLAG, actor[bactor].has_flag,
     team[actor[bactor].has_flag].base_x, team[actor[bactor].has_flag].base_y,
     0, 0, 0, -1, -1);
   team[actor[bactor].has_flag].flag_at_base = 1;
   actor[bactor].has_flag = -1;
//   play_sound(WAV_SCORE);
   break;
  }
 }
}

void teleport_team_base(int bteam)
{

 int i = 0;
 int bpick;


 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (pickup[i].pickup_type == PICKUP_BASE
   && pickup[i].contains == bteam)
   {
    bpick = i;
    break;
   }
 }

 create_cloud(CLOUD_COL_EXPLOSION, pickup[bpick].x, pickup[bpick].y, 0, 0, 2000, 40, team_colours(bteam), 10);

 destroy_pickup(bpick);

 bpick = drop_pickups(PICKUP_BASE, bteam);
 team[bteam].base_x = pickup[bpick].x;
 team[bteam].base_y = pickup[bpick].x;

}

void teleport_th_base(int th)
{

// int i = 0;
 int bpick = arena[0].th_base_pickup [th];

 create_cloud(CLOUD_COL_EXPLOSION, pickup[bpick].x, pickup[bpick].y, 0, 0, 2000, 40, 15, 10);

 destroy_pickup(bpick);

 bpick = drop_pickups(PICKUP_TH_BASE, 0);
 arena[0].th_base_x [th] = pickup[bpick].x;
 arena[0].th_base_y [th] = pickup[bpick].y;
 arena[0].th_base_pickup [th] = bpick;
 pickup[bpick].th_index = th;

}


void actor_porkball(int pactor)
{
 int ball_x = cos(actor[pactor].angle) * 7 * GRAIN;
 ball_x *= actor[pactor].facing;
 int ball_y = sin(actor[pactor].angle) * 7 * GRAIN;

 ball_x += actor[pactor].x;
 ball_y += actor[pactor].y - 2 * GRAIN;

 arena[0].porkball_x = ball_x;
 arena[0].porkball_y = ball_y;
 arena[0].porkball_state = actor[pactor].team;

 if (ball_x < 5 * GRAIN) ball_x = 5 * GRAIN;
 if (ball_x > arena[0].max_x * GRAIN - 5 * GRAIN) ball_x = arena[0].max_x * GRAIN - 5 * GRAIN;
 if (ball_y < 5 * GRAIN) ball_y = 5 * GRAIN;
 if (ball_y > arena[0].max_y * GRAIN - 5 * GRAIN) ball_y = arena[0].max_y * GRAIN - 5 * GRAIN;

 actor[(int) actor[pactor].has_porkball].x = ball_x;
 actor[(int) actor[pactor].has_porkball].y = ball_y;

 actor[(int) actor[pactor].has_porkball].x_speed = actor[pactor].x_speed;
 actor[(int) actor[pactor].has_porkball].y_speed = actor[pactor].y_speed;
 
}

/*
There's usually no collision detection between actors or between actors
and pickups (ie bases). So the porkball needs it specially:
*/
void run_porkball(int pactor)
{

 int i;

 if (arena[0].who_has_porkball == -1 && actor[pactor].jump_delay == 0)
 {
  for (i = 0; i < NO_ACTORS; i ++)
  {
   if (actor[i].aclass == ACLASS_NONE
    || actor[i].aclass == ACLASS_GHOST
    || actor[i].aclass == ACLASS_PORKBALL) continue;

   if (actor[pactor].x + actor[pactor].width * GRAIN>= actor[i].x - (actor[i].width * GRAIN)
       && actor[pactor].x - actor[pactor].width * GRAIN <= actor[i].x + (actor[i].width * GRAIN)
       && actor[pactor].y - actor[pactor].height * GRAIN <= actor[i].y + (actor[i].height * GRAIN)
       && actor[pactor].y + actor[pactor].height * GRAIN >= actor[i].y - (actor[i].height * GRAIN))
   {
    arena[0].who_has_porkball = i;
    actor[i].has_porkball = pactor;
    break;
   }
  }
 }


 for (i = 0; i < NO_TEAMS; i ++)
 {
  if (team[i].active == 0) continue;

  if (i == arena[0].porkball_state) continue;
  
  if (actor[pactor].x > team[i].base_x - (15 * GRAIN)
   && actor[pactor].x < team[i].base_x + (15 * GRAIN)
   && actor[pactor].y > team[i].base_y - (15 * GRAIN)
   && actor[pactor].y < team[i].base_y + (15 * GRAIN))
  {
   if (arena[0].porkball_state != -1)
   {
    score_event(arena[0].porkball_state, SCORE_GOAL, i);
    score_message(SMESSAGE_GOAL, arena[0].porkball_state, i, 0);
/*    team[arena[0].porkball_state].base_counter ++;
   if (team[arena[0].porkball_state].base_counter >= 5)
   {
    teleport_team_base(arena[0].porkball_state);
    team[arena[0].porkball_state].base_counter = 0;
   }*/
    team[i].base_counter ++;
   if (team[i].base_counter >= 5)
   {
    teleport_team_base(i);
    team[i].base_counter = 0;
   }
   }
   drop_porkball(pactor);
   break;
  }
 }


// now see if it should be rolling:

 if (abs(actor[pactor].x_speed) < 500)
 {

  char dirt_left = 0, dirt_right = 0, dirt_centre = 0;

  for (i = (actor[pactor].x / GRAIN) - actor[pactor].width; i < (actor[pactor].x / GRAIN) + actor[pactor].width; i++)
  {
   if (get_dirt(i, (actor[pactor].y / GRAIN) + actor[pactor].height + 1))
   {
    if (i < (actor[pactor].x / GRAIN)) dirt_left = 1;
     else
      if (i > (actor[pactor].x / GRAIN)) dirt_right = 1;
       else
        if (i == (actor[pactor].x / GRAIN)) dirt_centre = 1;
   }
  }

  if (dirt_left == 1 && dirt_right == 0)
   actor[pactor].x_speed += 20;
 
  if (dirt_left == 0 && dirt_right == 1)
   actor[pactor].x_speed -= 20;

 }

}


void run_th_bases(void)
{

 int i;
 for (i = 0; i < game[0].th_base_no; i ++)
 {
  arena[0].th_base_x [i] = pickup[arena[0].th_base_pickup [i]].x;
  arena[0].th_base_y [i] = pickup[arena[0].th_base_pickup [i]].y;
  if (arena[0].th_base_taken [i] == TEAM_NONE)
   continue;
  if (arena[0].th_base_counter [i] == 1
      && serial[0].game_type != SERIAL_CLIENT)
  {
       score_message(SMESSAGE_TH, arena[0].th_base_taken [i], 0, 0);
       score_event(arena[0].th_base_taken [i], SCORE_TH, 0);
       arena[0].th_base_counter [i] = 900;
       arena[0].th_base_speed [i] = 1;
       arena[0].th_base_direction [i] *= -1;
       arena[0].th_base_move [i] ++;
       if (arena[0].th_base_move [i] >= 5)
       {
        teleport_th_base(i);
        arena[0].th_base_move [i] = 0;
        arena[0].th_base_taken [i] = TEAM_NONE;
        arena[0].th_base_angle [i] = 128;
       }
       continue;
  }
  arena[0].th_base_counter [i] --;
  if (arena[0].th_base_counter [i] % 33 == 0) arena[0].th_base_speed [i] ++;
  arena[0].th_base_angle [i] += arena[0].th_base_speed [i] * arena[0].th_base_direction [i];
  if (arena[0].th_base_angle [i] >= 255)
   arena[0].th_base_angle [i] -= 255;
  if (arena[0].th_base_angle [i] <= 0)
   arena[0].th_base_angle [i] += 255;
 }

}


void drop_porkball(int pactor)
{

 if (arena[0].who_has_porkball != -1)
 {
  actor[arena[0].who_has_porkball].has_porkball = -1;
  arena[0].who_has_porkball = -1;
 }

 int px = 0;
 int py = 0;

 int tries = 0;

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
 } while(!check_free_area(px - actor[pactor].width, py - actor[pactor].height, px + actor[pactor].width, py + actor[pactor].height));

 actor[pactor].x = px * GRAIN;
 actor[pactor].y = py * GRAIN;
 actor[pactor].x_speed = 0;
 actor[pactor].y_speed = 0;

 blast_dirt(actor[pactor].x, actor[pactor].y, 8, 1);

 arena[0].porkball_x = actor[pactor].x;
 arena[0].porkball_y = actor[pactor].y;
 arena[0].porkball_state = -1;
 
   create_cloud(CLOUD_ORANGE_EXPLOSION, arena[0].porkball_x, arena[0].porkball_y, 0, 0, 500, 3, 0, 20);
   create_cloud(CLOUD_ORANGE_EXPLOSION, arena[0].porkball_x - 10 * GRAIN, arena[0].porkball_y, 0, 0, 300, 5, 0, 20);
   create_cloud(CLOUD_ORANGE_EXPLOSION, arena[0].porkball_x + 10 * GRAIN, arena[0].porkball_y, 0, 0, 300, 5, 0, 20);
   create_cloud(CLOUD_ORANGE_EXPLOSION, arena[0].porkball_x, arena[0].porkball_y - 10 * GRAIN, 0, 0, 300, 5, 0, 20);
   create_cloud(CLOUD_ORANGE_EXPLOSION, arena[0].porkball_x, arena[0].porkball_y + 10 * GRAIN, 0, 0, 300, 5, 0, 20);

}


char *team_name(int team)
{
 switch(team)
 {
  case TEAM_RED: return "Red Team";
  case TEAM_BLUE: return "Blue Team";
  case TEAM_GREEN: return "Green Team";
  case TEAM_GREY: return "Grey Team";
  case TEAM_GOLD: return "Gold Team";
  case TEAM_BROWN: return "Brown Team";
 }

 return "No Team";
}
