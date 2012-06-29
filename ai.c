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

File: ai.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - various functions controlling the ai players (bots). Bots work in more
   or less the same way as human players - they act by simulating keyed-in
   commands. They only cheat in two ways (unless you change the game
   settings to allow them to cheat): they always know the exact location and
   velocity of any actor in the game, and they can change weapons much more
   quickly than a human could. They follow the same rules as humans in all
   other respects - eg if they want to fire off a grappling hook in
   a particular direction, they have to issue the up/down command until
   their angle is right, then fire it.
 

*/

#include "allegro.h"

#include "string.h"
#include "math.h"

#include "config.h"
#include "globvar.h"

#include "cmds.h"
#include "grid.h"
#include "display.h"
#include "prand.h"
#include "cloud.h"

//extern int user_cmd_buffer [NO_USERS] [NO_CMDS];

enum
{
WEAPON_RANGE_SHORT, // eg flamethrower
WEAPON_RANGE_ANY,
WEAPON_RANGE_LONG // weapons which might hurt shooter if range too short
};



#define AI_NAMES 110

char *ai_name [AI_NAMES] =
{
// Silly names:
"AI",
"Norbert",
"Slurpy",
"Schnorkel",
"Blobby",
"Al",
"Adolf",
"Bork",
"Leppy",
"Noreen",
// 10
"Duane",
"Wayne",
"DuWayne",
"Cuthbert",
"Sharon",
"Dorcas",
"Marilyn",
"Schnarble",
"Arnold",
"Rutabaga",
// 20
"Archibald",
"Rudiger",
"Rupert",
"Porklet",
"Lobo",
"Georgina",
"Gertrude",
"Bertha",
"Daphne",
"Boris",
// 30
"Brasidas",
"Pleistoanax",
"Xerxes",
"Artaxerxes",
"Perdiccas",
"Aristocles",
"Anaxagoras",
"Mahatma",
"Ivan Pokrov",
"Ivan Prokov",
// 40
"Alexei",
"Doris",
"Genghis",
"Attila",
"Lemur",
"Potto",
"Lee",
"Anaximander",
"Dido",
"Sybil",
// 50
"Livia",
"Petunia",
"Molbo",
"Moebius",
"Morpheus",
"Neptune",
"Demeter",
"Gorby",
"Blorb",
"Wildebeest",
// 60
"Kakapo",
"Capricorn",
"Davros",
"Nestor",
"Eden",
"Leprechaun",
"Hippo",
"Hippy",
"Porcius",
"Johann",
// 70
"Centipede",
"Millipede",
"Scutigeramorph",
"Campagnotis",
"Ratty",
"Mole",
"Toad",
"Hagbard",
"Martha",
"Markov",
// 80
"Malik",
"Stella",
"Stanley",
"Margery",
"Homer",
"Selma",
"Thelma",
"Delma",
"Thor",
"Neville",
// 90
"Rico",
"Ace",
"Kurt",
"Crut",
"Brute",
"Klop",
"Zot",
"Zob",
"Xob",
"Loborundum",
// 100
"Sif Muna",
"Xom",
"Okawaru",
"Kikubaaqudgha",
"Zin",
"Nemelex Xobeh",
"Trog",
"Olgreb",
"Master Ek",
"Shabble"
/*// 110
"",
"",
"",
"",
"",
"",
"",
"",
"",
""*/
};

int weapon_range(int wpn);

void process_intentions(int aip);
void process_commands(int aic);

int move_to_angle(int x1, int y1, int x2, int y2, int facing, float angle, float angle_wanted, float angle_inc);
float get_angle(int x1, int y1, int x2, int y2, int facing);

void steer_remote(int ais);
int ai_move(int aim);
int bullet_hits_actor(int bx, int by, int owner, int fuzziness, int left_owner, int leading, int check_only);
int ai_tracer_bullet(int x1, int y1, int target_x, int target_y, int move_x, int move_y, char limited, int owner, int is_bullet, int gravity, char facing, float angle, int hits [3], char hit_actors, char ignore_dirt, char leading);
int ai_shoot(int ais);
void angle_weapon(int aiw, int weapon_testing, int angle_move);
void angle_grapple(int aig, int angle_move);
void issue_ai_command(int user_commanded, int command_given);
int clear_sight(int x1, int y1, int x2, int y2, int info_wanted, char tracer);
int ai_check_grapple(int aig);
int ai_check_rockets(int aig);
void ai_look_around(int ail);
void reset_dead_ai(int ua);
int ai_assess_amm(int aia);
int ai_find_pickup(int aip);
void tell_ai_pickup_gone(int pk);
int replace_which_weapon(int air);
void ai_random_dest(int aim);
void ai_select_weapons(int ais);
int ai_find_health(int aip);
int ai_unrand(int aiu, int limit);
int ai_find_grail(int aip);
int ai_find_th_base(int aip);
int ai_find_flag(int aip);


extern int user_cmd_buffer [NO_USERS] [NO_CMDS];
// needs access to read this, although shouldn't write to it.

int ai_user_cmd_store [NO_USERS] [NO_CMDS];
// stores what the ai did last turn

int ai_user_cmd_buffer [NO_USERS] [NO_CMDS];

extern struct ai_config_struct ai_config [MAX_AI];


int ai_find_game_item(int aip);
int ai_find_fruit(int aip);


enum
{
MODE_NONE,
MODE_KILL
};

enum
{
TASK_NONE,
TASK_SEEK,
TASK_FLEE,
TASK_FIND_AMM,
TASK_WANDER
};

enum
{
DOING_NONE,
DOING_SHOOTING,
DOING_JUMPING,
DOING_GRAPPLING,
DOING_SIGHTING,
DOING_DIGGING
};

enum
{
NEXT_NONE,
NEXT_JUMP,
NEXT_GRAPPLE_UP,
NEXT_SHOOT
};

enum
{
AMM_ENOUGH,
AMM_WANT,
AMM_NEED,
AMM_NONE
};


struct ai_state
{

 int active;

 int actor;

 int ai_index;

 int change_dir;

 // what is the ai's current purpose? (eg kill actor[1])
 int mode;
 int mode_object;
 int mode_object2;
 // if the current mode_object is hard to get at, this one will do instead.

 // what is the ai doing? (eg moving towards object)
 int task;
 int task_object;

 // what is the ai doing right now? (eg digging)
 int doing;
 int doing_object;

 // can plan a short way into future
 int next_move;

 int dest_x, dest_y;
 int frustration;
 int clear_sight;

 // what is the ai doing about its angle? (as this can be moved independently
 //  of other commands)
 int angling;
 float angle_wanted;

 char firing_burst;
 char dodging;
 char digging;
 char grappling;
 char jetting;
 char launched_grapple;
 int grapple_count;
 char amm;
 char eqp;
 int pickup_wanted;
 int no_pickups;
 int pickup_frustration;
 int fleeing;

 float weapon_angle [4];
 float grapple_angle;

 int counter;


};

struct ai_state ai [NO_USERS];

void init_ai_struct(void)
{

 int i;

 for (i = 0; i < MAX_AI; i ++)
 {
  ai[i].active = 0;
 }

}


void init_ai_user(int ui, int act, int ai_index)
{

    int unrand_no = 20;

    int i;

    ai[ui].active = 1;

    ai[ui].ai_index = ai_index;

    for (i = 0; i < 30; i ++)
    {
     if (user[ui].uname [i] == '\0') break;
     unrand_no += user[ui].uname [i];
    }

    user[ui].weapon [0] = WPN_NONE; //prand(10) + 5;
    user[ui].weapon [1] = WPN_NONE;
    user[ui].weapon [2] = WPN_NONE;
    user[ui].weapon [3] = WPN_NONE; //prand(10) + 5;

    ai_select_weapons(ui);

/*    user[ui].colour_r = prand(64);
    user[ui].colour_g = prand(64);
    user[ui].colour_b = prand(64);*/
//    user[ui].colour1 = unrand_no % 64;
//    user[ui].colour2 = ((unrand_no * unrand_no) - unrand_no) % 64;
//    user[ui].colour_b = (((unrand_no * unrand_no * unrand_no) / (unrand_no + 1)) - unrand_no) % 64;
    user[ui].colour1 = actor_colours_array [unrand_no % 7];
    user[ui].colour2 = actor_colours_array [((unrand_no * unrand_no) - unrand_no) % 7];
//    user[ui].colour2 = ((unrand_no * unrand_no) - unrand_no) % 64;
    
    user[ui].handicap = ai_config[ai_index].handicap;

    ai[ui].mode = MODE_KILL;
    ai[ui].mode_object = -1;
    ai[ui].mode_object2 = -1;
    
    ai[ui].task = TASK_WANDER;
    ai[ui].task_object = -1;

    ai[ui].doing = DOING_NONE;
    ai[ui].doing_object = -1;

    ai[ui].next_move = NEXT_NONE;

    ai[ui].angling = 0;
    ai[ui].angle_wanted = 0;
    ai[ui].firing_burst = 0;
    ai[ui].weapon_angle [0] = 0;
    ai[ui].weapon_angle [1] = 0;
    ai[ui].weapon_angle [2] = 0;
    ai[ui].weapon_angle [3] = 0;
    ai[ui].grapple_angle = 0;

    ai[ui].counter = ui * 17;
    ai[ui].dest_x = arena[0].max_x * GRAIN / 2;
    ai[ui].dest_y = arena[0].max_y * GRAIN / 2;
    ai[ui].frustration = 0;
    ai[ui].digging = 0;
    ai[ui].grapple_count = 0;
    ai[ui].launched_grapple = 0;
    ai[ui].pickup_wanted = -1;

    ai[ui].actor = act;
    ai[ui].clear_sight = 0;
    ai[ui].no_pickups = 0;
    ai[ui].pickup_frustration = 0;
    ai[ui].fleeing = 0;
    ai[ui].grappling = 0;
    ai[ui].jetting = 0;
    ai[ui].amm = AMM_ENOUGH;
    ai[ui].eqp = 2;
    ai[ui].change_dir = 0;

}


void run_ai_user(int air)
{

 ai[air].counter ++;

 if (actor[ai[air].actor].aclass == ACLASS_GHOST)
 {
  int want_spawn = 0;

  if (ai_config[ai[air].ai_index].atype == 0)
  {
   want_spawn = prand(9);
   if (game[0].atypes_avail == 0)
    want_spawn = 1;
   if (game[0].atypes_avail == 1)
    want_spawn = prand(3);
  } else
   {
    want_spawn = ai_config[ai[air].ai_index].atype - 1;
    if (game[0].atypes_avail == 0)
     want_spawn = 1;
    if (game[0].atypes_avail == 1)
     want_spawn %= 3;
   }

   issue_ai_command(air, CMD_SPAWN_0 + want_spawn);
   
 }
 else
 {
  process_intentions(air);

  process_commands(air);
/*  switch(ai[air].task)
  {
   case TASK_SEEK: message(-1, "2.Seek", 31); break;
   case TASK_WANDER: message(-1, "2.Wander", 31); break;
   case TASK_FIND_AMM: message(-1, "2.Find Amm", 31); break;

  }*/
 }

 int i;

 for (i = 0; i < NO_CMDS; i ++)
 {
  ai_user_cmd_store [air] [i] = ai_user_cmd_buffer [air] [i];
  // stores what commands are given for reference
  if (ai_user_cmd_buffer [air] [i] == 1)
  {
   issue_user_command(air, i);
   ai_user_cmd_buffer [air] [i] = 0;
  }
 }

}

int ai_assess_amm(int aia)
{

  if (ai[aia].no_pickups > 0)
  {
//   message(-1, "Can't assess.", 31);
   return AMM_ENOUGH;
  }

//  message(-1, "Assessing ammunition.", 31);

//  if (ai[aia].task == TASK_FIND_AMM)
//   message(-1, "Looking for ammunition.", 31);

//  if (ai[aia].pickup_wanted == -1)
//   message(-1, "Want PK -1.", 31);

//  if (ai[aia].mode_object == -1)
//   message(-1, "Mo == -1.", 31);

  if (arena[0].ai_unlimited_clips == 1) return AMM_ENOUGH;

 int ac = ai[aia].actor;
 int slots = actor[ac].weapon_slots;
 int i;
 int need = 0;
 int have = 0;

 ai[aia].eqp = 0;

 for (i = 0; i < slots; i ++)
 {
  if (actor[ac].weapon [i] == WPN_CLOAK
   || actor[ac].weapon [i] == WPN_SHIELD
   || actor[ac].weapon [i] == WPN_ARMOUR
   || actor[ac].weapon [i] == WPN_ROCKETPACK
   || actor[ac].weapon [i] == WPN_SHADOW
   || actor[ac].weapon [i] == WPN_REGENERATOR
   || actor[ac].weapon [i] == WPN_SPOTLIGHT
   || actor[ac].weapon [i] == WPN_JETPACK)
   {
    ai[aia].eqp ++;
   } else
    {
      if (actor[ac].weapon [i] <= NO_EQUIP + 1
       || (actor[ac].clips [i] <= 0 && actor[ac].ammunition [i] <= 0))
       {
        need += 2;
       }
     else
     {
      have ++;
//     message(-1, "Have something.", 31);
     }
    }
 }

 need -= have;

 if (have == 0) return AMM_NONE;

 if (need <= 2) return AMM_ENOUGH;
//  message(-1, "Want more...", 31);
// if (need <= 1) return AMM_WANT;
//  message(-1, "Need more.", 31);
 return AMM_NEED;


}

int replace_which_weapon(int air)
{

// if (arena[0].ai_unlimited_clips == 1) return -1;

 int ac = ai[air].actor;
 int slots = actor[ac].weapon_slots;
 int i;

 // first we'll look for completely empty slots:
 for (i = 0; i < slots; i ++)
 {
  if (actor[ac].clips [i] == 0 && actor[ac].ammunition [i] == 0 && actor[ac].reload [i] == 0)
   return i;
  if (actor[ac].weapon [i] == WPN_NONE)
   return i;
 }

 // next we look for ones which are running low.
 //  don't bother trying too hard, as for this function to be called there
 //  are probably empty slots which will've been caught above.
 for (i = 0; i < slots; i ++)
 {
  if (actor[ac].clips [i] < actor[ac].max_clips)
   return i;
 }

 return prand(slots); // oh well.

}


void process_intentions(int aip)
{

 if (ai_user_cmd_store [aip] [CMD_SHOOT] > 0 || ai[aip].counter % 100 == aip)
  ai[aip].amm = ai_assess_amm(aip);

 if (ai[aip].task != TASK_FIND_AMM && ai[aip].counter > 200)
 {
  ai_find_game_item(aip);
 } else
  {
   if (ai[aip].counter % 127 == 0 && actor[ai[aip].actor].health <= actor[ai[aip].actor].max_health / 2)
   {
    ai[aip].task = TASK_FIND_AMM;
    ai_find_health(aip);
   }
    else
    {
     if (ai[aip].amm == AMM_NEED || ai[aip].amm == AMM_NONE)
     {
      ai[aip].task = TASK_FIND_AMM;
      ai_find_pickup(aip);
     }
    }
  }
  
 if (ai[aip].amm == AMM_WANT
  && (ai[aip].mode_object == -1 || ai[aip].task == TASK_WANDER)
//  && ai[aip].mode_object == -1
  && ai[aip].task != TASK_FIND_AMM)
  {
   ai[aip].task = TASK_FIND_AMM;
   ai_find_pickup(aip);
  }

 if (ai[aip].amm == AMM_ENOUGH)
 {
  if (ai[aip].task == TASK_FIND_AMM
  && pickup[ai[aip].pickup_wanted].pickup_type == PICKUP_WEAPON)
  {
   ai[aip].task = TASK_WANDER;
   ai_random_dest(aip);
  }
 }

/* switch(ai[aip].task)
 {
  case TASK_SEEK: message(-1, "1.Seek", 31); break;
  case TASK_WANDER: message(-1, "1.Wander", 31); break;
  case TASK_FIND_AMM: message(-1, "1.Find Amm", 31); break;

 }*/

 return;
}

void process_commands(int aic)
{

 if (ai[aic].change_dir > 0)
  ai[aic].change_dir --;

 if (actor[ai[aic].actor].remote_control != REMOTE_NONE)
 {
  steer_remote(aic);
  return;
 }

 if (ai[aic].grappling == 1)
 {
    if (actor[ai[aic].actor].angle < ai[aic].grapple_angle + 0.015
      && actor[ai[aic].actor].angle > ai[aic].grapple_angle - 0.015)
    {
     issue_ai_command(aic, CMD_GRAPPLE);
     ai[aic].grappling = 0;
     ai[aic].grapple_count = 0;
     ai[aic].launched_grapple = 60;
     return;
    }

    int angle_move = 0;

    if (actor[ai[aic].actor].angle > ai[aic].grapple_angle)
     angle_move = -1;
    if (actor[ai[aic].actor].angle < ai[aic].grapple_angle)
     angle_move = 1;

    if (angle_move != 0)
    {
     angle_weapon(aic, actor[ai[aic].actor].current_weapon, angle_move);
    }

  return;
 }
 // angling to fire grapple overrides all other actions.

 if (ai[aic].dodging == 1)
 {
     issue_ai_command(aic, CMD_JUMP);
     ai[aic].dodging = 0;
 }
 // jumping to dodge also overrides, but other actions can follow

 if (ai[aic].task == TASK_FIND_AMM)
 {
  if (ai[aic].pickup_wanted != -1)
  {
   if (pickup[ai[aic].pickup_wanted].x < actor[ai[aic].actor].x + (10 * GRAIN)
    && pickup[ai[aic].pickup_wanted].x > actor[ai[aic].actor].x - (10 * GRAIN)
    && pickup[ai[aic].pickup_wanted].y < actor[ai[aic].actor].y + (10 * GRAIN)
    && pickup[ai[aic].pickup_wanted].y > actor[ai[aic].actor].y - (10 * GRAIN))
   {
      issue_ai_command(aic, CMD_REWEAP_0 + replace_which_weapon(aic));
       // Assumes CMD_REWEAP_? are all in order.
       
//   ai[aic].task = TASK_WANDER;
//   ai[aic].pickup_wanted = -1;
//   ai[aic].amm = AMM_ENOUGH;
//      message(-1, "Picking up.", 31);
      ai[aic].task = TASK_WANDER;
      ai[aic].amm = ai_assess_amm(aic);
      // ai[].pickup_wanted shld be reset from destroy_pickup
   }
  }
   else
   {
    if (ai[aic].counter % 24 == 0)
     ai_find_pickup(aic);
      else ai[aic].no_pickups = 10;
   }

  if (ai[aic].no_pickups > 0)
  {
   ai[aic].task = TASK_WANDER;
  }

 }
  if (ai[aic].no_pickups > 0)
  {
   ai[aic].no_pickups --;
  }


 switch(ai[aic].mode)
 {
  case MODE_KILL:
  if (ai[aic].mode_object == aic) ai[aic].mode_object = -1;
//  if (ai[aic].task == TASK_SEEK)
  if (ai[aic].mode_object != -1)
   ai[aic].clear_sight = clear_sight(actor[ai[aic].actor].x, actor[ai[aic].actor].y, actor[ai[aic].mode_object].x, actor[ai[aic].mode_object].y, ai[aic].mode_object, 0);
    else
     ai[aic].clear_sight = 0;
//  if (ai[aic].task == TASK_WANDER)
//   ai[aic].clear_sight = clear_sight(actor[ai[aic].actor].x, actor[ai[aic].actor].y, ai[aic].dest_x, ai[aic].dest_y, -2);
   
  if (ai[aic].task == TASK_WANDER)
  {

   if (ai[aic].amm != AMM_NONE)
   {
    if (ai[aic].clear_sight == 1)
     ai[aic].task = TASK_SEEK;
      else
       if ((ai[aic].counter + aic) % 16 == 0)
       {
        ai_look_around(aic);
       }
   }
   ai_move(aic);
  } else
  {
   if (ai[aic].task == TASK_SEEK && ai[aic].mode_object != -1 && ai_shoot(aic) == 1)
   {
    if (actor[ai[aic].mode_object].x < actor[ai[aic].actor].x && actor[ai[aic].actor].facing == 1
        && ai[aic].change_dir == 0)
    {
     issue_ai_command(aic, CMD_LEFT);
     ai[aic].frustration ++;
     ai[aic].change_dir = 50;
    } else
        {
         if (actor[ai[aic].mode_object].x > actor[ai[aic].actor].x && actor[ai[aic].actor].facing == -1
             && ai[aic].change_dir == 0)
             {
              issue_ai_command(aic, CMD_RIGHT);
              ai[aic].frustration ++;
              ai[aic].change_dir = 50;
             }
        }
   } else
   ai_move(aic);
  }
  break;

 }


}



int ai_move(int aim)
{

 int ac = ai[aim].actor;
 int mo = ai[aim].mode_object;

 int angle_move = 0;

 char move_x = 0;


 if (ai[aim].firing_burst != 0 || ai[aim].dodging != 0) return 0;

  if (actor[ai[aim].mode_object2].lit == 0 && ai[aim].counter % 32 == 0)
  {
   ai[aim].mode_object2 = -1;
  }
  
 if (ai[aim].task == TASK_SEEK && ai[aim].mode == MODE_KILL && mo != -1)
 {
  if (ai[aim].clear_sight == 0
  || abs(actor[ac].x - actor[mo].x) > 30 * GRAIN
  || abs(actor[ac].y - actor[mo].y) > 30 * GRAIN) // don't want to get
                                                  //  too close.
  {
   ai[aim].dest_x = actor[mo].x;
   ai[aim].dest_y = actor[mo].y;
  }
  if (actor[mo].lit == 0 && ai[aim].counter % 32 == 0)
  {
   ai[aim].mode_object = -1;
   mo = -1;
  }
 }
 if (ai[aim].task == TASK_FIND_AMM && ai[aim].pickup_wanted != -1)
 {
//  if (ai[aim].clear_sight == 0
//  || abs(actor[ac].x - pickup[ai[aim].pickup_wanted].x) > 30 * GRAIN
//  || abs(actor[ac].y - actor[mo].y) > 30 * GRAIN) // don't want to get
                                                  //  too close.
  {
   ai[aim].dest_x = pickup[ai[aim].pickup_wanted].x;
   ai[aim].dest_y = pickup[ai[aim].pickup_wanted].y;
  }

 }

 if ((ai[aim].frustration > 30)
 ||
 ((mo != -1 && ai[aim].task != TASK_FIND_AMM)
 && actor[ac].x / (GRAIN * 2) == actor[mo].x / (GRAIN * 2)
 && abs(actor[mo].y < actor[ac].y - (GRAIN * 30)))
// && ai[aim].clear_sight == 0)
 ||
 ((ai[aim].task == TASK_FIND_AMM && ai[aim].pickup_wanted != -1)
 && actor[ac].x / (GRAIN * 8) == pickup[ai[aim].pickup_wanted].x / (GRAIN * 8)
 && abs(actor[ac].y - pickup[ai[aim].pickup_wanted].y) > GRAIN * 20)
 ||
 (ai[aim].task == TASK_WANDER
 && actor[ac].x / (GRAIN * 2) == ai[aim].dest_x / (GRAIN * 2)))
 {
//  if (ai[aim].task != TASK_WANDER)
  ai[aim].task = TASK_WANDER;
  ai[aim].frustration = 5;
  ai[aim].pickup_wanted = -1;
  ai_random_dest(aim);
//  message(-1, "I'm going wandering.", 31);
  if (ai[aim].mode == MODE_KILL && ai[aim].mode_object2 != -1)
  {
   int mob = ai[aim].mode_object2;
   ai[aim].mode_object2 = ai[aim].mode_object;
   ai[aim].mode_object = mob;
  }
  ai[aim].no_pickups = 10;
   // we're stuck, so don't go chasing pickups for a while
 }

 if (ai[aim].task == TASK_FIND_AMM && ai[aim].pickup_wanted != -1)
 {
  ai[aim].dest_x = pickup[ai[aim].pickup_wanted].x;
  ai[aim].dest_y = pickup[ai[aim].pickup_wanted].y;
 }

 if (ai[aim].task == TASK_WANDER || ai[aim].task == TASK_FIND_AMM)
 {
/*    if (ai[aim].mode == MODE_KILL && ai[aim].counter % 255 == 0 && mo != -1)
    {
     ai[aim].task = TASK_SEEK;
     ai[aim].dest_x = actor[mo].x;
     ai[aim].dest_x = actor[mo].y;
    }*/
 
    angle_move = move_to_angle(actor[ac].x + (actor[ac].fire_x * actor[ac].facing * GRAIN), actor[ac].y + (actor[ac].fire_y * GRAIN), ai[aim].dest_x,
     ai[aim].dest_y, actor[ac].facing, actor[ac].angle,
    get_angle(actor[ac].x + (actor[ac].fire_x * actor[ac].facing * GRAIN), actor[ac].y + (actor[ac].fire_y * GRAIN), ai[aim].dest_x, ai[aim].dest_y,
    actor[ac].facing),
     0.01);

    if (angle_move != 0)
    {
     angle_weapon(aim, actor[ac].current_weapon, angle_move);
    }
 }


 if (actor[ac].x < ai[aim].dest_x) move_x = 1;
 if (actor[ac].x > ai[aim].dest_x) move_x = -1;

 ai[aim].frustration ++;

 if (abs(actor[ac].x_speed) > 50)
  ai[aim].frustration = 0;

 if (abs(actor[ac].x_speed) < 10) // REPLACE!!! && move_x != 0)// &&
// (ai_user_cmd_store [aim] [CMD_LEFT] != 0
// || ai_user_cmd_store [aim] [CMD_RIGHT] != 0))
 {
//  if (ai[aim].counter % 3 == 0) // for a more natural digging motion
  if (ai[aim].counter % 4 != 0 || ai[aim].grapple_count > 0)
  {
   if (ai[aim].counter % 2 == 0) issue_ai_command(aim, CMD_DIG);
   ai[aim].digging = 3; // was 10
  }
   else
    issue_ai_command(aim, CMD_JUMP);
  ai[aim].frustration ++;
    // no point digging if angle too high.
 } else ai[aim].frustration -= 1;

 if (ai[aim].digging > 0)
 {
  ai[aim].digging --;

//  if (ai[aim].dest_y > actor[ac].y && actor[ac].angle > PI / 4)
//   issue_ai_command(aim, CMD_DOWN);

  
  if (actor[ac].angle < PI / -4) // checked & correct
   if (ai[aim].clear_sight == 0)
    issue_ai_command(aim, CMD_DOWN);
 }

// if (actor[ac].propulsion == PROP_GRAPPLE)
 {
  if (ai[aim].launched_grapple == 0)
  {
   if (ai[aim].clear_sight == 0 || ai[aim].counter % 100 == 0)
    ai_check_grapple(aim);
  } else ai[aim].launched_grapple--;

  if (ai[aim].grapple_count > 5)
  {
   if (ai[aim].clear_sight == 1 && ai[aim].counter % 3 != 0) ai[aim].grapple_count --;
  }
 }

 if (actor[ac].propulsion == PROP_ROCKETS)
 {
  if (actor[ac].rocket_charge == 0)
  {
   if (ai[aim].jetting > 0)
   {
     issue_ai_command(aim, CMD_JUMP);
     ai[aim].jetting --;
   }
   else
   {
    if (actor[ac].y > ai[aim].dest_y && ai[aim].counter % 10 == 0)
    {
     issue_ai_command(aim, CMD_JUMP);
     ai[aim].jetting = 3;
    }
     else
     {
      if (actor[ac].y > ai[aim].dest_y && ai[aim].clear_sight == 0)
       ai_check_rockets(aim);
     }
   }
  }
 }

 if (actor[ac].propulsion == PROP_JET)
 {
  if (ai[aim].jetting > 0)
  {
     issue_ai_command(aim, CMD_JUMP);
     ai[aim].jetting --;
  }
  else
  // only jet if jets cold or already jetting
  {
   if (actor[ac].jet_heat < 4000 || actor[ac].jet_spin > 0)
   {
    if (actor[ac].y > ai[aim].dest_y - (40 * GRAIN) && (ai[aim].counter % 256 < 128))
    {
     issue_ai_command(aim, CMD_JUMP);
     ai[aim].jetting = 20;
    }
     else
     {
      if (actor[ac].y > ai[aim].dest_y - (40 * GRAIN))// && ai[aim].clear_sight == 0)
      {
       if (ai_check_rockets(aim))
       move_x = 0;
      }
     }
   }
  }
 }


 if (actor[ac].grapple_anchored == 1)
 {
  ai[aim].grapple_count ++;
//  if (ai[aim].clear_sight == 0)
  {
   if (ai[aim].grapple_count > 60)
   {
    issue_ai_command(aim, CMD_JUMP);
    ai[aim].grapple_count = 0;
    actor[ac].prevent_jump = 0; // NOTE: prevent_jump should really be in
     // the player/user struct; changing it here isn't cheating
   }
  }
  // grapple_count is also reset if the actor does something useful, like
  //  shooting, as this indicates it's in an okay position
 }

 if (ai[aim].fleeing > 0)
 {
  if (move_x == 1)
   issue_ai_command(aim, CMD_LEFT);
  if (move_x == -1)
   issue_ai_command(aim, CMD_RIGHT);
  ai[aim].fleeing --;
 }
 else
 {
  if (move_x == -1)
  {
   if (actor[ai[aim].actor].facing == -1 || ai[aim].change_dir == 0)
   {
    issue_ai_command(aim, CMD_LEFT);
    if (actor[ai[aim].actor].facing == 1)
     ai[aim].change_dir = 50;
   }
     else
      issue_ai_command(aim, CMD_RIGHT);
  }
  if (move_x == 1)
  {
   if (actor[ai[aim].actor].facing == 1 || ai[aim].change_dir == 0)
   {
    issue_ai_command(aim, CMD_RIGHT);
    if (actor[ai[aim].actor].facing == -1)
     ai[aim].change_dir = 50;
   }
     else
      issue_ai_command(aim, CMD_LEFT);
  }
 }

 return 1;
       //
}


void ai_random_dest(int aim)
{

  ai[aim].dest_x = actor[ai[aim].actor].x - (400 * GRAIN) + (prand(800) * GRAIN);
  ai[aim].dest_y = actor[ai[aim].actor].y - (400 * GRAIN) + (prand(800) * GRAIN);
  if (ai[aim].dest_x < 0) ai[aim].dest_x = 0;
  if (ai[aim].dest_x > arena[0].max_x * GRAIN) ai[aim].dest_x = arena[0].max_x * GRAIN;
  if (ai[aim].dest_y < 0) ai[aim].dest_y = 0;
  if (ai[aim].dest_y > arena[0].max_y * GRAIN) ai[aim].dest_y = arena[0].max_y * GRAIN;

  if (actor[ai[aim].actor].x < GRAIN * 80)
   ai[aim].dest_x = arena[0].max_x * GRAIN;
  if (actor[ai[aim].actor].x > (arena[0].max_x * GRAIN) - (GRAIN * 80))
   ai[aim].dest_x = 0;

//  message(-1, "Let's go somewhere.", 31);

}


int ai_check_grapple(int aig)
{

 int ac = ai[aig].actor;

 int gx = 0, gy = 0;

// int g2x, g2y;

// gx = actor[ac].x - (30 * GRAIN) + (prand(60) * GRAIN);
// gy = 0;

  // was / -4 and / 50
/*  if (ai[aig].grapple_angle > PI / -2 || (ai[aig].counter / 100) % 2 == 0)
   angle_grapple(aig, -1);
    else
     angle_grapple(aig, 1);*/

/*  if ((ai[aig].counter / 50) % 2 == 0)
   angle_grapple(aig, -1);
    else
     angle_grapple(aig, 1);*/
    if (ai[aig].grapple_angle >= PI / 4) //PI / 2)
    {
     ai[aig].grapple_angle = PI / -2;
    } else
     angle_grapple(aig, 1);
     
 int bull_x = cos(ai[aig].grapple_angle) * 30000;
 bull_x *= actor [ac].facing;
 int bull_y = sin(ai[aig].grapple_angle) * 30000;

 int cs = clear_sight(actor[ac].x + (actor[ac].fire_x * actor[ac].facing * GRAIN), actor[ac].y + (actor[ac].fire_y * GRAIN), actor[ac].x + bull_x + (actor[ac].fire_x * actor[ac].facing * GRAIN), actor[ac].y + bull_y + (actor[ac].fire_y * GRAIN), -1, 1);

 if (cs < 30)
 {
  return 0;
 }

 float angle_used = ai[aig].grapple_angle; //actor[ac].angle;

 bull_x = cos(angle_used) * 500;
 bull_x *= actor [ac].facing;
 bull_y = sin(angle_used) * 500;

 // remember to change this speed if it's changed in cmds.c
 //  - should use a config #define or an actor_struct field
 int bull_x_speed = bull_x * actor[ac].grapple_power;
 bull_x_speed /= 10;
 int bull_y_speed = bull_y * actor[ac].grapple_power;
 bull_y_speed /= 10;

 float angle_passed = angle_used;
 if (angle_passed < 0) angle_used = (PI * 2) + angle_passed;
 if (actor[ac].facing == -1) angle_used *= -1;

 int total_x_speed = bull_x_speed;
 int total_y_speed = bull_y_speed;
 
// total_x_speed += actor[ac].x_speed;
// total_y_speed += actor[ac].y_speed;
// Doesn't add these because the actor will probably be stationary when
//  firing the grapple.

 int hits [3] = {0, 0, 0};


 ai_tracer_bullet(actor[ac].x + bull_x + (actor[ac].fire_x * actor[ac].facing * GRAIN), actor[ac].y + bull_y + (actor[ac].fire_y * GRAIN), gx, gy, total_x_speed, total_y_speed, 1, ac, 0, game[0].gravity, actor[ac].facing, actor[ac].angle, hits, 0, 0, 1);


/*  user[0].score = hits [0];
  user[0].lives = hits [1];
  user[1].score = hits [0];
  user[1].lives = hits [1];
  user[2].score = hits [0];
  user[2].lives = hits [1];
  user[3].score = hits [0];
  user[3].lives = hits [1];
  user[4].score = hits [0];
  user[5].lives = hits [1];
  update_score();*/

 if (ai[aig].mode == MODE_KILL)
 {
  switch(ai[aig].task)
  {
   case TASK_SEEK:
   if (ai[aig].mode_object == -1) break;
   if (clear_sight(actor[ai[aig].mode_object].x + (actor[ac].fire_x * actor[ac].facing * GRAIN), actor[ai[aig].mode_object].y + (actor[ac].fire_y * GRAIN), hits [0], hits [1], ai[aig].mode_object, 1))
    ai[aig].grappling = 1;
   if (actor[ai[aig].mode_object].y < actor[ac].y - (200 * GRAIN) && cs >= 200)
    ai[aig].grappling = 1;
//   create_cloud(CLOUD_BLUE_EXPLOSION, hits [0], hits [1], 0, 0, 180, 10, 0);
   break;
   case TASK_FIND_AMM:
   case TASK_WANDER:
   if (clear_sight(hits [0], hits [1], ai[aig].dest_x, ai[aig].dest_y, -2, 1))
    ai[aig].grappling = 1;
   if (ai[aig].dest_y < actor[ac].y - (100 * GRAIN) && cs >= 100)
    ai[aig].grappling = 1;
   break;
  }
 }

 return 1;

}

int ai_check_rockets(int aig)
{

 int ac = ai[aig].actor;

// int gx = 0, gy = 0;

/*    if (ai[aig].grapple_angle >= PI / 4) //PI / 2)
    {
     ai[aig].grapple_angle = PI / -2;
    } else
     angle_grapple(aig, 1);*/
     
    if (ai[aig].grapple_angle >= 0) //PI / 4) //PI / 2)
    {
     ai[aig].grapple_angle = PI / -2;
    } else
     angle_grapple(aig, 1);

 int bull_x = cos(ai[aig].grapple_angle) * 30000;
 bull_x *= actor [ac].facing;
 int bull_y = sin(ai[aig].grapple_angle) * 30000;

 int cs = clear_sight(actor[ac].x, actor[ac].y, actor[ac].x + bull_x, actor[ac].y + bull_y, -1, 1);

 if (cs < 30)
 {
  return 0;
 }

 float angle_used = ai[aig].grapple_angle; //actor[ac].angle;

 bull_x = cos(angle_used) * 500;
 bull_x *= actor [ac].facing;
 bull_y = sin(angle_used) * 500;

 issue_ai_command(aig, CMD_JUMP);

 return 1;

}


/*
Returns 1 if target is in sights and actor shouldn't be moving
*/
int ai_shoot(int ais)
{

 int angle_move = 0;
 int tracer_result = 0;
 int is_bullet = 1; // change when multiple weapons available
 int hits [3] = {0, 0, 0};

 int sactor = ai[ais].actor;

 int gravity_used = 0;

 int return_value = 0;

 if (actor[ai[ais].mode_object].cloak != 0
  && actor[ai[ais].mode_object].uncloaked == 0)
  {
   if (ai[ais].counter % 10 != 0) return 0;
  }


// if (actor[sactor].recycle > 0) return;
// if (actor[sactor].reload [actor[sactor].current_weapon] > 0) return;
// if (actor[sactor].ammunition [actor[sactor].current_weapon] == 0) return;

 int what = -1;

 int weapon_testing = what;

 int weapons_hit [6] = {0, 0, 0, 0, 0, 0};

 ai[ais].weapon_angle [actor[sactor].current_weapon] = actor[sactor].angle;


 do
 {

 weapon_testing ++;

 if (actor[sactor].weapon [weapon_testing] == WPN_NONE ||
  actor[sactor].ammunition [weapon_testing] == 0)
 {
  continue;
 }

 int fired = actor[sactor].weapon [weapon_testing];

 if ((fired == WPN_TRACKER || fired == WPN_SEEKER)
     && actor[sactor].lock_on != ai[ais].mode_object)
  continue;

 // some weapons shouldn't be fired if you're travelling too quickly
 if (fired == WPN_TRACKER || fired == WPN_REMOTE_ROCKET_C || fired == WPN_REMOTE_ROCKET || fired == WPN_SEEKER)
 {
  if (abs(actor[sactor].x_speed) + abs(actor[sactor].y_speed) > GRAIN * 5)
   continue;
 }
 
 float angle_used = ai[ais].weapon_angle [weapon_testing];
// float angle_used = weapon_angle [weapon_testing]actor[sactor].angle;

/*

 Keep - may be useful for determining fuzziness of tracer accuracy

 if (wlist[fired].scatter > 0)
 {
  int sc_used = wlist[fired].scatter;
  if (game[0].bullet_speed == 0 && wlist[fired].is_bullet)
   sc_used /= 5;
  angle_used += (float) prand(sc_used) / 800;
  angle_used -= (float) prand(sc_used) / 800;
 }*/

// angle_used += scatter / 10;

 int bull_x = cos(angle_used) * 500;
 bull_x *= actor [sactor].facing;
 int bull_y = sin(angle_used) * 500;

 int l_speed = wlist[fired].launch_speed;
 if (wlist[fired].is_bullet && game[0].fast_bullets == 0)
 {
  gravity_used = game[0].gravity / 5;
/*  if (l_speed >= 70)
   l_speed /= 6;
   else
    if (l_speed >= 60)
     l_speed /= 5;
      else
       if (l_speed >= 40)
        l_speed /= 3;
         else
          l_speed /= 2;*/
  l_speed = wlist[fired].slow_speed;
 } else
 switch(fired)
 {

  case WPN_FTHROWER: // actually rises against gravity, but this is better.
  case WPN_LR_R:
  case WPN_R_L:
  case WPN_SWARM:
  case WPN_SQUIRM:
  case WPN_PLAS_C:
  case WPN_PLAS_R:
  case WPN_RECTIFIER:
  case WPN_NUKE_M:
  case WPN_PLINKER:
  case WPN_LASER_BEAM:
  case WPN_LASER_PULSE:
  case WPN_TRACKER:
  case WPN_SEEKER:
  case WPN_HUNTER:
  case WPN_DISRUPTER:
  case WPN_DISRUPTER_WAVE:
  case WPN_CUBE:
  case WPN_IMPLODER:
  case WPN_REMOTE_ROCKET:
  case WPN_REMOTE_ROCKET_C:
  case WPN_SMITER:
  case WPN_PRONGTHROWER:
  case WPN_RPG:
  case WPN_FUMIGATOR:
  case WPN_ELECTRO:
  gravity_used = 0;
  break;
  
  default:
  case WPN_GR_L:
  case WPN_GREN:
  case WPN_FRAG:
  case WPN_FIREBOMB:
  case WPN_FUNKY_BOMB:
  gravity_used = game[0].gravity;
  break;

 }
 if (wlist[fired].is_bullet && game[0].fast_bullets == 2)
 {
  l_speed *= 100;
 }

 int bull_x_speed = bull_x * l_speed;
 bull_x_speed /= 10;
 int bull_y_speed = bull_y * l_speed;
 bull_y_speed /= 10;

 float angle_passed = angle_used;
 if (angle_passed < 0) angle_used = (PI * 2) + angle_passed;
 if (actor[sactor].facing == -1) angle_used *= -1;

 int total_x_speed = bull_x_speed;
 int total_y_speed = bull_y_speed;
 
 if (fired != WPN_REMOTE_ROCKET_C && fired != WPN_LASER_BEAM && fired != WPN_LASER_PULSE)
 {
  total_x_speed += actor[sactor].x_speed;
  total_y_speed += actor[sactor].y_speed;
 }

 char leading = 1;
 if (ai_config[ai[ais].ai_index].skill == 0)
  leading = 0;

// if (weapon_testing == 0)
 {

  hits [2] = 0;

  if (fired == WPN_DISRUPTER_WAVE)
   tracer_result = ai_tracer_bullet(actor[ai[ais].actor].x + bull_x + (actor[ais].fire_x * actor[ais].facing * GRAIN), actor[ai[ais].actor].y + bull_y + (actor[ais].fire_y * GRAIN), actor[ai[ais].mode_object].x, actor[ai[ais].mode_object].y, total_x_speed, total_y_speed, 1, ai[ais].actor, is_bullet, gravity_used, actor[ai[ais].actor].facing, angle_used, hits, 1, 1, leading);
    else
     tracer_result = ai_tracer_bullet(actor[ai[ais].actor].x + bull_x + (actor[ais].fire_x * actor[ais].facing * GRAIN), actor[ai[ais].actor].y + bull_y + (actor[ais].fire_y * GRAIN), actor[ai[ais].mode_object].x, actor[ai[ais].mode_object].y, total_x_speed, total_y_speed, 1, ai[ais].actor, is_bullet, gravity_used, actor[ai[ais].actor].facing, angle_used, hits, 1, 0, leading);
//   else tracer_result = 0;
   
/*  user[0].handicap = tracer_result;
  user[1].handicap = tracer_result;
  user[2].handicap = tracer_result;
  user[3].handicap = tracer_result;
  user[4].handicap = tracer_result;
  user[5].handicap = tracer_result;
  user[6].handicap = tracer_result;

  update_score();*/
 }
 
  angle_move = 0;


  if (tracer_result >= 0)
  {

/*   angle_move = move_to_angle(actor[ai[ais].actor].x,
    actor[ai[ais].actor].y, actor[ai[ais].mode_object].x,
    actor[ai[ais].mode_object].y, actor[ai[ais].actor].facing,
    actor[ai[ais].actor].angle, get_angle(actor[ai[ais].actor].x,
    actor[ai[ais].actor].y, actor[ai[ais].mode_object].x,
    actor[ai[ais].mode_object].y, actor[ai[ais].actor].facing),
    actor[ai[ais].actor].angle_increment);*/
  } else
  switch(tracer_result)
  {
   case -1:
    if (ai[ais].weapon_angle [weapon_testing] > PI / -4)
     angle_move = -1;
      else
       angle_move = 1;
    break; // hits before, so move up if lofted
   case -2: angle_move = 1; break;
   case -3:
//    if (actor[user[ais].actor].angle > PI / -4)
     angle_move = -1;
   break;
   case -4: angle_move = 0; break; // on target, but blocked.
                // should get it to also look at lofting, if possible
  }


  if (angle_move != 0)
  {
   angle_weapon(ais, weapon_testing, angle_move);
//   if (angle_move > 0) return_value = 1;
    // if it's near enough to target to angle down, don't bother closing further.
  }
   

//  if (arena[0].counter % 4 != 0)
//  {
//  }
  
/*  int bmove_x = cos(actor[ai[ais].actor].angle) * 500;
  bmove_x *= actor [ai[ais].actor].facing;
  int bmove_y = sin(actor[ai[ais].actor].angle) * 500;*/

  if (tracer_result > -1)
  {
   if (arena[0].teams == 0 || actor[sactor].team == TEAM_NONE || actor[sactor].team != actor[tracer_result].team)
   {
    if (weapon_range(fired) == WEAPON_RANGE_ANY
     || (weapon_range(fired) == WEAPON_RANGE_LONG && hits [2] > 3)
     || (weapon_range(fired) == WEAPON_RANGE_SHORT && hits [2] < 10))
    {
     weapons_hit [weapon_testing] = 1;
     return_value = 1;
    }
   }
  }



 } while(weapon_testing < actor[sactor].weapon_slots && weapon_testing < 6);


 if (actor[sactor].recycle > 0) return return_value;
// if (actor[sactor].reload [actor[sactor].current_weapon] > 0) return return_value;
// if (actor[sactor].ammunition [actor[sactor].current_weapon] == 0) return return_value;



   if (ai[ais].firing_burst > 0 && ai_user_cmd_buffer [ais] [CMD_SHOOT] == 0)
   {
    issue_ai_command(ais, CMD_SHOOT);
    ai[ais].firing_burst --;
    ai[ais].grapple_count = 0;
    return 1;
   }




 weapon_testing = 0;

 int fire_weapon = -1;
 char need_to_flee = 1;
 char found_weapon = -1;

 if (weapons_hit [actor[sactor].current_weapon] == 1)
 {
  fire_weapon = actor[sactor].current_weapon;
  need_to_flee = 0;
 } else
  {
   do
   {

    if (weapons_hit [weapon_testing] == 1
     && wlist[actor[sactor].weapon [weapon_testing]].status == WPNCLASS_WEAPON)
    {
     found_weapon = weapon_testing;
     need_to_flee = 0;
    }
   
    weapon_testing ++;


   } while (weapon_testing < actor[sactor].weapon_slots);

   weapon_testing = 0;

   if (found_weapon == -1)
   // okay, we haven't found a good weapon. Now accept weak ones:
   {
    do
    {
     if (weapons_hit [weapon_testing] == 1)
     {
      found_weapon = weapon_testing;
      need_to_flee = 0;
     }
     weapon_testing ++;
    } while (weapon_testing < actor[sactor].weapon_slots);
   }

    switch(found_weapon)
    {
     case 0: issue_ai_command(ais, CMD_CHWEAP_0); return 0;
     case 1: issue_ai_command(ais, CMD_CHWEAP_1); return 0;
     case 2: issue_ai_command(ais, CMD_CHWEAP_2); return 0;
     case 3: issue_ai_command(ais, CMD_CHWEAP_3); return 0;
    }

 } // end else

 if (actor[sactor].reload [actor[sactor].current_weapon] > 0) return return_value;
 if (actor[sactor].ammunition [actor[sactor].current_weapon] == 0) return return_value;

 
/*  do
  {

   if (weapons_hit [weapon_testing] == 1)
   {
    switch(weapon_testing)
    {
     case 0: issue_ai_command(ais, CMD_CHWEAP_0); return 0;
     case 1: issue_ai_command(ais, CMD_CHWEAP_1); return 0;
     case 2: issue_ai_command(ais, CMD_CHWEAP_2); return 0;
     case 3: issue_ai_command(ais, CMD_CHWEAP_3); return 0;
    }
    need_to_flee = 0;
   }
   
   weapon_testing ++;


  } while (weapon_testing < actor[sactor].weapon_slots);
*/
 if (fire_weapon != -1)
 {
   ai[ais].firing_burst = 1;
   switch(actor[sactor].weapon [actor[sactor].current_weapon])
   {
    case WPN_FUMIGATOR:
    case WPN_PRONGTHROWER:
    case WPN_PLAS_R: ai[ais].firing_burst = 2 + prand(3); break;
    case WPN_LASER_BEAM:
    case WPN_NEEDLER:
    case WPN_NIBBLER: ai[ais].firing_burst = 4 + prand(8); break;
    case WPN_INCENDIARIES:
    case WPN_LMG: ai[ais].firing_burst = 4 + prand(8); break;
    case WPN_HMG: ai[ais].firing_burst = 4 + prand(8); break;
    case WPN_SQUIRM:
    case WPN_SWARM: ai[ais].firing_burst = 4 + prand(8); break;
   }

 }

   if (ai[ais].firing_burst > 0 && ai_user_cmd_buffer [ais] [CMD_SHOOT] == 0)
   {
    issue_ai_command(ais, CMD_SHOOT);
    ai[ais].firing_burst --;
    ai[ais].grapple_count = 0;
    need_to_flee = 0;
    return 1;
   }

  if (ai[ais].clear_sight == 1 && need_to_flee == 1
   && ai[ais].mode_object != -1
   && abs(actor[ai[ais].actor].x - actor[ai[ais].mode_object].x) < 40 * GRAIN)
    ai[ais].fleeing = 30;
 
  return return_value;

}

void angle_weapon(int aiw, int weapon_testing, int angle_move)
{

 float angle_inc = 0.03;
 if (ai_config[ai[aiw].ai_index].skill == 0)
 {
  if (ai[aiw].counter % 2 != 0)
   return;
  angle_inc = 0.001;
 }
 if (ai_config[ai[aiw].ai_index].skill == 1)
 {
  if (ai[aiw].counter % 2 == 0)
   return;
  angle_inc = 0.005;
 }
 
 if (weapon_testing == actor[ai[aiw].actor].current_weapon)
 {
   if (angle_move == -1) issue_ai_command(aiw, CMD_UP);
   if (angle_move == 1) issue_ai_command(aiw, CMD_DOWN);
   ai[aiw].weapon_angle [weapon_testing] = actor[ai[aiw].actor].angle;
   return;
 }

 if (angle_move == -1)
 {
          ai[aiw].weapon_angle [weapon_testing] -= angle_inc;
          if (ai[aiw].weapon_angle [weapon_testing] < PI / -2)
             ai[aiw].weapon_angle [weapon_testing] = PI / -2;
 } else
 {

          ai[aiw].weapon_angle [weapon_testing] += angle_inc;
          if (ai[aiw].weapon_angle [weapon_testing] > PI / 2)
             ai[aiw].weapon_angle [weapon_testing] = PI / 2;
 }

}

void angle_grapple(int aig, int angle_move)
{

 if (angle_move == -1)
 {
          ai[aig].grapple_angle -= 0.200;
          if (ai[aig].grapple_angle < PI / -2)
             ai[aig].grapple_angle = PI / -2;
 } else
 {

          ai[aig].grapple_angle += 0.200;
          if (ai[aig].grapple_angle > PI / 2)
             ai[aig].grapple_angle = PI / 2;
 }

}


/*
Lets an ai steer a remote missile towards its target. Only if there's a
clear line from the missile to the target, though, so that it can fly
around corners.
*/
void steer_remote(int ais)
{

 int sactor = ai[ais].actor;

 if (clear_sight(bullet[actor[sactor].remote_thing].x, bullet[actor[sactor].remote_thing].y, actor[ai[ais].mode_object].x, actor[ai[ais].mode_object].y, ai[ais].mode_object, 0) == 0)
  return;
 
 int moving = move_to_angle(bullet[actor[sactor].remote_thing].x,
  bullet[actor[sactor].remote_thing].y,
  actor[ai[ais].mode_object].x, actor[ai[ais].mode_object].y,
  bullet[actor[sactor].remote_thing].facing,
  bullet[actor[sactor].remote_thing].angle,
   get_angle(bullet[actor[sactor].remote_thing].x,
   bullet[actor[sactor].remote_thing].y,
   actor[ai[ais].mode_object].x, actor[ai[ais].mode_object].y,
   bullet[actor[sactor].remote_thing].facing),
  0);

 if (bullet[actor[sactor].remote_thing].facing == -1)
  moving *= -1;

 if (moving == -1)
  issue_ai_command(ais, CMD_LEFT);
 if (moving == 1)
  issue_ai_command(ais, CMD_RIGHT);
  
}


// assumes is facing in the right direction
int move_to_angle(int x1, int y1, int x2, int y2, int facing, float angle, float angle_wanted, float angle_inc)
{

 if (angle == angle_wanted) return 0;

 int turning = 0;

 if (facing == 1)
 {
  if (x2 >= x1)
  {
   if (angle_wanted > angle) turning = 1;
    else turning = -1;
  } else
        {
         if (angle_wanted < angle) turning = 1;
           else turning = -1;
        }

 } // end facing == 1
  else
 {

  if (x2 <= x1)
  {
   if (angle_wanted > angle)
   {
       turning = 1;
   }
    else
    {
        turning = -1;
    }
  } else
        {
         if (angle_wanted < angle)
         {
          turning = 1;
         }
           else turning = -1;
        }

 }
 
 return turning;
 
}


float get_angle(int x1, int y1, int x2, int y2, int facing)
{

 float angle_wanted = 0;

/* if (x1 == x2)
  angle_wanted = PI / 2;
   else
    angle_wanted = atan((float) ((float) y2 - y1) / ((float) x2 - x1));
*/

 if (x1 == x2)
 {
  if (y2 < y1)
   angle_wanted = PI / 2;
    else
     angle_wanted = PI / -2;
 }
   else
    angle_wanted = atan((float) ((float) y2 - y1) / ((float) x2 - x1));


 if (facing == -1)
  angle_wanted *= -1;
  
  return angle_wanted;

}



/*
returns:
actor number - if hits target!

hits contains:
hits [0, 1] = x, y coordinates of first dirt hit
hits [2] = distance travelled, in pixels
*/
int ai_tracer_bullet(int x1, int y1, int target_x, int target_y, int move_x, int move_y, char limited, int owner, int is_bullet, int gravity, char facing, float angle, int hits [2], char hit_actors, char ignore_dirt, char leading)
{

char dir_x = 0, dir_y = 0;

hits [2] = 0;

int move_x2 = move_x;
int move_y2 = move_y;

move_y2 += gravity;

int step_x, step_y;

int x2 = x1, y2 = y1;

int last_x = x2, last_y = y2;

int has_passed_y = 0; // has the bullet passed above or below target
int has_passed_x = 0; // has the bullet passed on near or far side?
int has_passed_x2 = 0; // has it passed near/far twice?
char has_reversed = 0; // has it passed the apex of its flight?

int bhit = 0;
int obstructed = 0;
//int passing = 0;

int loop_count = 0;
int loop_count2;

int timeout = 1000;

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;


/* if (move_x == 10 && move_y == 0 && bullet[mbull].damage != -1)
 {
  if (actor_collision(mbull) == 1) return;
 }*/


while(timeout > 0)
{

  timeout --;

  if (x2 <= -100 * GRAIN || y2 <= -100 * GRAIN || x2 / GRAIN >= arena[0].max_x + 100 || y2 / GRAIN >= arena[0].max_y + 100)
  {
/*
  user[0].score = has_passed_y;
  user[0].lives = has_passed_x;
  user[0].team = has_reversed;
  user[1].score = has_passed_y;
  user[1].lives = has_passed_x;
  user[1].team = has_reversed;
  user[2].score = has_passed_y;
  user[2].lives = has_passed_x;
  user[2].team = has_reversed;
  user[3].score = has_passed_y;
  user[3].lives = has_passed_x;
  user[3].team = has_reversed;
  user[4].score = has_passed_y;
  user[4].lives = has_passed_x;
  user[4].team = has_reversed;
  user[5].score = has_passed_y;
  user[6].lives = has_passed_x;
  user[0].active = has_passed_x2;
  user[1].active = has_passed_x2;
  user[2].active = has_passed_x2;
  user[3].active = has_passed_x2;
  user[4].active = has_passed_x2;
  user[5].active = has_passed_x2;
  user[6].active = has_passed_x2;
*/

/*
    switch(has_passed_y)
    {
     case 0: return -1; // hasn't passed target. Maybe aim up if lofted?
     case -1: return -3; // has passed below, so move up
     case 1: return -2; // has passed above, so move down
    }
*/
/*
if (move_x == 0)
{
 if (actor[owner].angle > 0)
  return -3;
   else return -2;
}*/

/*
all comments after the returns are as if shooter facing right at target.
*/

    if (has_passed_x == 0) // has not passed to left or right of target ...
    {
     if (has_passed_y == 1) // ... but has passed below target
     {
      if (has_reversed == 0 || has_reversed == 100)
      {
      return -3;
//       if (angle < PI / -4) return -1;
//        else return -2;
//       if (angle
//       return -1; // NOT (bullet only passes below. Aim to 45deg.)
       // bullet only passes below, had not passed below before apex.
           // May be out of range. But aim to 45deg anyway.

      }
      if (has_reversed == 101)
       return -3;
       // bullet only passes below, reaches apex after passing below.
         // May be able to hit by raising sights.
     }
     if (has_passed_y == -1) // ... but has passed above target
     {
       return -2; // bullet only passes above. Aim down always.
     }

//     if (has_passed_x == 0 && has_passed_x2 == 0 &&
     // hasn't passed above or below either, so work out where it started:
     if (y1 < target_y)
      return -2; // started above, so aim down.
       else
        return -1; // arced fully in near lower quadrant.
         // May be out of range. Try maximising range by aiming at 45deg.

    }
    
    if (has_passed_x == 1 && has_passed_x2 == 0)
    // fell below or rose above target when past target.
    {
     if (has_passed_y == 1)
      return -3; // fired from bottom left, rose above target on far side.
       // aim upwards and see where that gets us.
     if (has_passed_y == -1)
      return -2; // fired from top left, fell below target on far side.
       // aim down
//     if (has_passed_y == 0)
//      abort();
//     exit(has_passed_x + (has_passed_x2 * 10) + (has_passed_y * 100)); // should never get here.

     if (has_reversed == 0) return -3; //prob firing straight down, so aim up.
//      exit(100);
//      return -1; // Should be impossible???
    }

    if (has_passed_x == -1 && has_passed_x2 == 0)
    // fell below or rose above target on near side, but didn't fall back on
    //  other side.
    {
     if (has_passed_y >= 0 && (has_reversed == 100 || has_reversed == 0))
     {
/*      if (y1 < target_y) return -2;
       else return -1;*/
       if (facing == 1)
       {
       if (angle < PI / -4) return -2; // was -3
        else return -1;
       } else
        {
          if (angle < PI / 4) return -1; // was -3
           else return -2;
        }


/*
WAS::::
       if (angle < PI / -4) return -2;
        else return -1;
*/
     }
     if (has_passed_y == 1 || y1 < target_y)
      return -1; // Fired from above, fell short. Aim up to 45deg.
     if (has_passed_y == -1 || y1 > target_y)
      return -2; // Fired from below, went over target, didn't fall back.
       // aim down

     return -1;
    }

    if (has_passed_x == 1 && has_passed_x2 == 1)
    // rose above and fell below target on far side
     return -3; // must be below shooting up, but not aiming up enough.
      // aim further up.

    if (has_passed_x == -1 && has_passed_x2 == 1)
    // rose above on near side, fell below on far side
     return -2; // aim down.

    if (has_passed_x == -1 && has_passed_x2 == -1)
    // rose and fell on near side.
    {
//     if (move_x > 0)
//     {
       if (angle < PI / -4) return -1; // was -3
        else return -2;
/*     } else
        {
          if (angle < PI / -4) return -2; // was -3
           else return -1;
        }*/
        
        
    }
//     return -2; // aim down

//     abort();

//     exit(has_passed_x + (has_passed_x2 * 10) + (has_passed_y * 100) + 10000); // should never get here.

     return -2; // must be below


  }

//  if (prand(100) == 0) break;



/* tick_count ++;
 count_move_x += abs(step_x);
 count_move_y += abs(step_y);
 
 if ((count_move_x >= abs(move_x) * 4 && move_x != 0) || (count_move_y >= abs(move_y) * 4 && move_y != 0))
 {
  count_move_x = 0; // -= abs(move_x);
  count_move_y = 0; //-= abs(move_y);*/
  if (has_passed_y == 0)
  {
   if ((x2 > target_x && last_x <= target_x) || (x2 < target_x && last_x >= target_x))
   {
    // okay, it's passing above or below the target:
    if (y2 > target_y) has_passed_y = 1; // passing below
     else has_passed_y = -1; // passing above
   }
  }

  if (has_passed_x == 0)
  {
   if ((y2 > target_y && last_y <= target_y) || (y2 < target_y && last_y >= target_y))
   {
    if (dir_x == 10)
    {
     if (x2 > target_x) has_passed_x = 1; // passing by on far side
      else has_passed_x = -1; // passing by on near side
    } else
    {
     if (x2 > target_x) has_passed_x = -1; // passing by on near side
      else has_passed_x = 1; // passing by on far side
    }
   }
//   passing = 10;
  } else
  if (has_passed_x2 == 0)
  {
   if ((y2 > target_y && last_y <= target_y) || (y2 < target_y && last_y >= target_y))
   {
    if (dir_x == 10)
    {
     // okay, it's passing above or below the target:
     if (x2 > target_x) has_passed_x2 = 1; // passing by on far side
      else has_passed_x2 = -1; // passing by on near side
    } else
    {
     if (x2 > target_x) has_passed_x2 = -1; // passing by on near side
      else has_passed_x2 = 1; // passing by on far side
    }
   }
  }


//  if (hit_actors == 0)
//create_cloud(CLOUD_DEBUG, x2, y2, 0, 0, 40, 20, 0);
  last_x = x2;
  last_y = y2;


  move_x = move_x2;
  move_y = move_y2;
  
  move_y2 += gravity;

  step_x = move_x;
  step_y = move_y;

/*  if (step_x == 0)
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

  

  if (has_reversed == 0)
  {
   if (move_y < 0 && move_y + gravity >= 0)
   {
    // is the apex before or after it passes above/below the target?
    has_reversed = has_passed_y + 100;
   }
  }

 loop_count ++;
 hits [2] = loop_count;

 loop_count2 = 0;

 while (move_x != 0 || move_y != 0)
 {
   if (obstructed == 0 && get_dirt((x2 + step_x) / GRAIN, (y2 + step_y) / GRAIN) && ignore_dirt == 0)
   {
    obstructed = 1;
    hits [0] = x2;// + step_x;
    hits [1] = y2; // + step_y;
    if (hit_actors == 0) return 0;
   }

  if (hit_actors && loop_count2 % TARGET_THICKNESS == 0)
  {
   bhit = bullet_hits_actor(x2, y2, owner, 0, leading, loop_count, -1);
   if (bhit > -1)
   {
    if (obstructed) return -4; // aimed right, but something in the way.
    return bhit;
   }
  }


  if (step_x == 0 && step_y == 0) break;

 if (step_x == 0) move_x = 0;
 if (step_y == 0) move_y = 0;

 loop_count2 ++;
 if (loop_count2 > 100) break;

 if (move_y != 0 && (abs(move_y + step_y) == move_y + step_y) != (abs(step_y) == step_y))
 {
  step_y = move_y;
  if (move_x == 0) break;
 }
 if (move_x != 0 && (abs(move_x + step_x) == move_x + step_x) != (abs(step_x) == step_x))
 {
  step_x = move_x;
  if (move_y == 0) break;
 }

/*
  if (move_y != 0 && (abs(move_y) == move_y) != (abs(step_y) == step_y))
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
  }
*/
  if (limited == 1)
  {
   move_x -= step_x;
   move_y -= step_y;
  }

  x2 += step_x;
  y2 += step_y;


  
/*  if (bullet[mbull].bullet_type == BULLET_GRAPPLE || bullet[mbull].bullet_type == BULLET_LASER_BEAM || bullet[mbull].bullet_type == BULLET_LASER_PULSE)
  {
    if (get_dirt((x2) / GRAIN, (y2) / GRAIN))
     continue;
  }
   else*/
/*    {
     if (get_dirt((x2 + step_x) / GRAIN, (y2 + step_y) / GRAIN))
      break; // was continue;
    }*/




/*
WAS HERE!
  x2 += step_x;
  y2 += step_y;
*/



 } // end of while loop

} // end of other while loop

return -10;

}
























int bullet_hits_actor(int bx, int by, int owner, int fuzziness, int left_owner, int leading, int check_only)
{

  int actor_count = 0;
  int aim_x;
  int aim_y;
  if (check_only != -1) actor_count = check_only;

  int width, height;

  do
  {
   if (actor[actor_count].aclass != ACLASS_PLAYER)
   {
    actor_count ++;
    continue;
   }

   aim_x = actor[actor_count].x + actor[actor_count].x_speed * leading;
   aim_y = actor[actor_count].y + actor[actor_count].y_speed * leading;

  width = actor[actor_count].width;
  height = actor[actor_count].height;

/*  if (ai_config[ai[owner].ai_index].skill == 1)
  {
   width *= 2;
   height *= 2;
  }*/

  if (ai_config[ai[owner].ai_index].skill == 0)
  {
   width *= 1 + ai[owner].counter % 3;
   height *= 1 + ai[owner].counter % 3;
  }

  if (actor[actor_count].lit == 0)
  {
   width *= 1 + ai[owner].counter % 5;
   height *= 1 + ai[owner].counter % 5;
  }

   if (bx >= aim_x - (width * GRAIN)
       && bx <= aim_x + (width * GRAIN)
       && by <= aim_y + (height * GRAIN)
       && by >= aim_y - (height * GRAIN))
   {
       // need to put left_owner back in somehow
       if (owner == actor_count)// && left_owner <= 3)
       {
          actor_count ++;
          continue;
       }
       return actor_count;
   }

   if (actor_count == check_only) return -1;

   actor_count ++;
  } while (actor_count < NO_ACTORS);
   // actor_count will never == check_only + 1 here if check_only == -1

  return -1;

}


void issue_ai_command(int user_commanded, int command_given)
{

 if (command_given == CMD_LEFT)
  ai_user_cmd_buffer [user_commanded] [CMD_RIGHT] = 0;

 if (command_given == CMD_RIGHT)
  ai_user_cmd_buffer [user_commanded] [CMD_LEFT] = 0;

 if (command_given == CMD_UP)
  ai_user_cmd_buffer [user_commanded] [CMD_DOWN] = 0;

 if (command_given == CMD_DOWN)
  ai_user_cmd_buffer [user_commanded] [CMD_UP] = 0;

 ai_user_cmd_buffer [user_commanded] [command_given] = 1;

}

/*
Return value depends on info_wanted.
If -1, returns distance to dirt along this line.
if -2, returns 0 if there's dirt between x1,y1 & x2,y2, 1 otherwise
If >= 0, returns 1 if actor[info_wanted] is in line before dirt
*/
int clear_sight(int x1, int y1, int x2, int y2, int info_wanted, char tracer)
{

char dir_x = 0, dir_y = 0;
int move_x = x2 - x1;
int move_y = y2 - y1;
//int move_x = x1 - x2;
//int move_y = y1 - y2;

int step_x, step_y;

int loop_count = 0;

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;

  step_x = move_x;
  step_y = move_y;

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

 while (TRUE)
 {

   if (get_dirt((x1 + step_x) / GRAIN, (y1 + step_y) / GRAIN))
   {
    if (info_wanted == -1) return loop_count;
    if (info_wanted == -2) return 0;
    return 0;
   }

   if (info_wanted >= 0) // looking for an actor
   {
    if (bullet_hits_actor(x1, y1, -1, 0, 1, 0, info_wanted) > -1)
     return 1;
   }

//  if (tracer == 1 && loop_count % 10 == 0) create_cloud(CLOUD_DEBUG, x1, y1, 0, 0, 80, 20, 0);


  x1 += step_x;
  y1 += step_y;
  loop_count ++;

  if (x1 == x2 && y1 == y2) break;

 } // end of while loop

 if (info_wanted == -2) return 1;

return 0;

}


void random_ai_name(char *name)
{

 strcpy(name, ai_name [rand() % AI_NAMES]);
 if (strlen(name) == 0)
  strcpy(name, "Porkupine");

}

int weapon_range(int wpn)
{
 switch(wpn)
 {
  default:
  case WPN_PLAS_R:
  case WPN_PLINKER:
  case WPN_LASER_BEAM:
  case WPN_LASER_PULSE:
  return WEAPON_RANGE_ANY;
  
  case WPN_DISRUPTER:
  case WPN_TRACKER:
  case WPN_SEEKER:
  case WPN_HUNTER:
  case WPN_R_L:
  case WPN_SWARM:
  case WPN_GR_L:
  case WPN_GREN:
  case WPN_FRAG:
  case WPN_FIREBOMB:
  case WPN_FUNKY_BOMB:
  case WPN_RECTIFIER:
  case WPN_NUKE_M:
  case WPN_PLAS_C:
  case WPN_IMPLODER:
  case WPN_INCENDIARIES:
  return WEAPON_RANGE_LONG;
  
  case WPN_FTHROWER:
  case WPN_FLAKKER:
  case WPN_FUMIGATOR:
  return WEAPON_RANGE_SHORT;
 }

 return WEAPON_RANGE_ANY;
}


void annoy_ai(int source_actor, int actor_annoyed)
{

 int ua;
 
 for (ua = 0; ua < MAX_AI; ua ++)
 {
  if (ai[ua].active == 0) continue;
  if (ai[ua].actor == actor_annoyed) break;
 }
 // can assume that actor_annoyed is an ai

 if (ai[ua].mode_object == source_actor) return;

 if (ai[ua].counter % 3 == 0) ai[ua].dodging = 1;

   if (arena[0].teams == 0 || actor[actor_annoyed].team == TEAM_NONE || actor[actor_annoyed].team != actor[source_actor].team)
   {
    switch(ai[ua].mode)
    {
     case MODE_KILL:
     if (ai[ua].mode_object == -1 || ai[ua].clear_sight == 0)
      ai[ua].mode_object = source_actor;
       else
        ai[ua].mode_object2 = source_actor;
     if (ai[ua].amm != AMM_NONE)
      ai[ua].task = TASK_SEEK;
     break;
    }
   }
}

void ai_look_around(int ail)
{
 int ua = 0;
 int closest_enemy = -1;
 int shortest_distance = 50000;
 int cs = 0;
 int dist = 0;

 for (ua = 0; ua < NO_ACTORS; ua ++)
 {
  if (ua == ail) continue;
  if (actor[ua].aclass != ACLASS_PLAYER) continue;
  if (actor[ua].lit == 0)
   continue;
//  if (arena[0].teams == 1 && (actor[ua].team == TEAM_NONE || actor[ua].team != actor[ai[ail].actor].team))
  if (arena[0].teams == 1 && actor[ua].team != TEAM_NONE && actor[ua].team == actor[ai[ail].actor].team)
   continue;
  cs = clear_sight(actor[ai[ail].actor].x, actor[ai[ail].actor].y, actor[ua].x, actor[ua].y, -2, 0);
  if (cs == 0) continue;
  dist = abs(actor[ai[ail].actor].x - actor[ua].x) + abs(actor[ai[ail].actor].y - actor[ua].y);
  if (dist < shortest_distance)
  {
   closest_enemy = ua;
   shortest_distance = dist;
  }
 }

 if (closest_enemy != -1 && ai[ail].amm != AMM_NONE)
 {
  ai[ail].mode_object = closest_enemy;
  ai[ail].task = TASK_SEEK;
  ai[ail].clear_sight = 1;
  return;
 }

// Nobody in sight. So let's just go for the closest enemy:

 closest_enemy = -1;
 shortest_distance = 50000;

 for (ua = 0; ua < NO_ACTORS; ua ++)
 {
  if (ua == ail) continue;
  if (actor[ua].aclass != ACLASS_PLAYER) continue;
  if (actor[ua].lit == 0)
   continue;
//  if (arena[0].teams == 1 && (actor[ua].team == TEAM_NONE || actor[ua].team != actor[ai[ail].actor].team))
  if (arena[0].teams == 1 && actor[ua].team != TEAM_NONE && actor[ua].team == actor[ai[ail].actor].team)
   continue;
  dist = abs(actor[ai[ail].actor].x - actor[ua].x) + abs(actor[ai[ail].actor].y - actor[ua].y);
  if (dist < shortest_distance)
  {
   closest_enemy = ua;
   shortest_distance = dist;
  }
 }

 if (closest_enemy != -1)
 {
  ai[ail].mode_object = closest_enemy;
  if ((ai[ail].counter + ail) % 256 == 0 && ai[ail].amm != AMM_NONE)
  {
   ai[ail].task = TASK_SEEK;
  }
  ai[ail].clear_sight = 0;
  return;
 }




}

void tell_ai_someone_died(int aik)
{

 int ua;

 for (ua = 0; ua < MAX_AI; ua ++)
 {
  if (ai[ua].active == 0) continue;
  if (ai[ua].mode_object == aik)
  {
   if (ai[ua].mode_object2 == -1 || ai[ua].mode_object2 == aik)
   {
    ai[ua].mode_object = -1;
    if (ai[ua].task == TASK_SEEK)
     ai[ua].task = TASK_WANDER;
   } else
    {
     ai[ua].mode_object = ai[ua].mode_object2;
    }
  }
  if (ai[ua].mode_object2 == aik)
   ai[ua].mode_object2 = -1;
 }

}


int ai_find_pickup(int aip)
{
//  message(-1, "Looking for a pickup.", 31);

// if (ai[aip].pickup_frustration

 if (game[0].pk_number == 0) return 0;
 if (game[0].pk_weapon == 0
     && game[0].pk_equip == 0) return 0;

 int i;

/* for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (i == MAX_PICKUPS - 1) return 0;
  if (pickup[pcount].pickup_type == 1) break;
 }*/

// later: here assess whether ai wants this particular weapon...


 int closest_pickup = -1;
 int shortest_distance = 50000;
 int dist = 0;

 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (i == MAX_PICKUPS - 1) break;
  if (pickup[i].pickup_type != PICKUP_WEAPON
   && pickup[i].pickup_type != PICKUP_EQUIP)// && ai[aip].eqp == 0))
    continue;
  if (pickup[i].pickup_type == PICKUP_EQUIP && ai[aip].eqp > 0)
    continue;
//  dist = abs(actor[ai[aip].actor].x - pickup[i].x) + abs(actor[ai[aip].actor].y - pickup[i].y);
  dist = hypot(abs(actor[ai[aip].actor].x - pickup[i].x), abs(actor[ai[aip].actor].y - pickup[i].y));
//  if (actor[ai[aip].actor].y > pickup[i].y) dist *= 2;
  if (actor[ai[aip].actor].y > pickup[i].y + (150 * GRAIN)) dist = 50002;
   // prefer pickups below actor - easier to get to, usually
  if (dist < shortest_distance)
  {
   closest_pickup = i;
   shortest_distance = dist;
  }
 }

 if (closest_pickup != -1)
 {
  ai[aip].task = TASK_FIND_AMM;
  ai[aip].pickup_wanted = closest_pickup;
  ai[aip].clear_sight = 0;
  ai[aip].dest_x = pickup[closest_pickup].x;
  ai[aip].dest_y = pickup[closest_pickup].y;
//  message(-1, "Found a pickup!", 31);
  return 1;
 }

 return 0;

}

int ai_find_health(int aip)
{
//  message(-1, "Looking for a pickup.", 31);

// if (ai[aip].pickup_frustration

 if (game[0].pk_number == 0) return 0;
 if (game[0].pk_health == 0) return 0;

 int i;

/* for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (i == MAX_PICKUPS - 1) return 0;
  if (pickup[pcount].pickup_type == 1) break;
 }*/


 int closest_pickup = -1;
 int shortest_distance = 50000;
 int dist = 0;

 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (i == MAX_PICKUPS - 1) break;
  if (pickup[i].pickup_type != PICKUP_POWER) continue;
  dist = hypot(abs(actor[ai[aip].actor].x - pickup[i].x), abs(actor[ai[aip].actor].y - pickup[i].y));
//  if (actor[ai[aip].actor].y > pickup[i].y) dist *= 2;
  if (actor[ai[aip].actor].y > pickup[i].y + (20 * GRAIN)) dist = 50002;
   // prefer pickups below actor - easier to get to, usually
  if (dist < shortest_distance)
  {
   closest_pickup = i;
   shortest_distance = dist;
  }
 }

 if (closest_pickup != -1)
 {
  ai[aip].task = TASK_FIND_AMM;
  ai[aip].pickup_wanted = closest_pickup;
  ai[aip].clear_sight = 0;
  ai[aip].dest_x = pickup[closest_pickup].x;
  ai[aip].dest_y = pickup[closest_pickup].y;
  return 1;
 }

 return 0;

}


int ai_find_game_item(int aip)
{
 switch(game[0].game_type)
 {
  default:
   return 0; // kill, last one standing
  case GAME_FRUIT_HUNT:
   ai_find_fruit(aip);
   break;
  case GAME_GRAIL:
   ai_find_grail(aip);
   break;
  case GAME_TAKEHOLD:
   ai_find_th_base(aip);
   break;
  case GAME_CAP_FLAG:
   ai_find_flag(aip);
   break;

/*
       case GAME_KILL: strcat(mstring, "Kill Everything"); break;
       case GAME_LAST_ONE: strcat(mstring, "Last One Standing"); break;
       case GAME_CAP_FLAG: strcat(mstring, "Capture the Flag"); break;
       case GAME_FRUIT_HUNT: strcat(mstring, "Hunt for Fruit"); break;
       case GAME_PORKBALL: strcat(mstring, "Porkball"); break;
       case GAME_TAKEHOLD: strcat(mstring, "Take & Hold"); break;
       case GAME_GRAIL: strcat(mstring, "Holy Grail"); break;
*/
 }

 return 1;

}


int ai_find_fruit(int aip)
{
// exit(0);

//  message(-1, "Looking for fruit.", 31);
 int i;

 int closest_pickup = -1;
 int shortest_distance = 50000;
 int dist = 0;

 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (i == MAX_PICKUPS - 1) break;
  if (pickup[i].pickup_type != PICKUP_FRUIT) continue;
  dist = hypot(abs(actor[ai[aip].actor].x - pickup[i].x), abs(actor[ai[aip].actor].y - pickup[i].y));
//  if (actor[ai[aip].actor].y > pickup[i].y + (150 * GRAIN)) dist = 50002;
  if (dist < shortest_distance)
  {
   closest_pickup = i;
   shortest_distance = dist;
  }
 }

 if (closest_pickup != -1)
 {
//  message(-1, "Found fruit!", 31);
  ai[aip].task = TASK_FIND_AMM;
  ai[aip].pickup_wanted = closest_pickup;
  ai[aip].clear_sight = 0;
  ai[aip].dest_x = pickup[closest_pickup].x;
  ai[aip].dest_y = pickup[closest_pickup].y;
  return 1;
 }

 return 0;

}

int ai_find_grail(int aip)
{
 int i;

 if (actor[ai[aip].actor].has_grail == 1)
 {
  for (i = 0; i < MAX_PICKUPS; i ++)
  {
   if (i == MAX_PICKUPS - 1) break;
   if (pickup[i].pickup_type != PICKUP_BASE) continue;
   if (pickup[i].contains != actor[ai[aip].actor].team) continue;
   ai[aip].task = TASK_FIND_AMM;
   ai[aip].pickup_wanted = i;
   ai[aip].clear_sight = 0;
   ai[aip].dest_x = pickup[i].x;
   ai[aip].dest_y = pickup[i].y;
   return 1;
  }
 }

 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (i == MAX_PICKUPS - 1) break;
  if (pickup[i].pickup_type != PICKUP_GRAIL) continue;
  ai[aip].task = TASK_FIND_AMM;
  ai[aip].pickup_wanted = i;
  ai[aip].clear_sight = 0;
  ai[aip].dest_x = pickup[i].x;
  ai[aip].dest_y = pickup[i].y;
  return 1;
 }

 return 0;

}

int ai_find_th_base(int aip)
{
 int i;

 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  if (i == MAX_PICKUPS - 1) break;
  if (pickup[i].pickup_type != PICKUP_TH_BASE) continue;
  if (arena[0].th_base_taken [pickup[i].th_index] == actor[ai[aip].actor].team) continue;
  ai[aip].task = TASK_FIND_AMM;
  ai[aip].pickup_wanted = i;
  ai[aip].clear_sight = 0;
  ai[aip].dest_x = pickup[i].x;
  ai[aip].dest_y = pickup[i].y;
  return 1;
 }

 return 0;

}

int ai_find_flag(int aip)
{

 int i;

 int closest_pickup = -1;
 int shortest_distance = 50000;
 int dist = 0;
 char okay = 0;

 for (i = 0; i < MAX_PICKUPS; i ++)
 {
  okay = 0;
  if (i == MAX_PICKUPS - 1) break;
  if (pickup[i].pickup_type != PICKUP_BASE
      && pickup[i].pickup_type != PICKUP_FLAG)
       continue; // it's neither a base nor a flag
  if (pickup[i].pickup_type == PICKUP_BASE
      && pickup[i].contains == actor[ai[aip].actor].team
      && actor[ai[aip].actor].has_flag != -1
      && team[actor[ai[aip].actor].team].flag_at_base == 1)
      {
       okay = 1;
       // if ai has enemy flag and its own flag is at base, go to base.
      }
  if (pickup[i].pickup_type == PICKUP_FLAG
      && pickup[i].contains == actor[ai[aip].actor].team
      && team[actor[ai[aip].actor].team].flag_at_base == 0)
      {
       okay = 1;
       // if ai's flag not at base, go to it.
      }
  if (pickup[i].pickup_type == PICKUP_FLAG
      && pickup[i].contains != actor[ai[aip].actor].team
      && actor[ai[aip].actor].has_flag == -1)
      {
       okay = 1;
       // if ai's not carrying a flag, look for one.
      }
  if (okay == 0)
   continue;

  dist = hypot(abs(actor[ai[aip].actor].x - pickup[i].x), abs(actor[ai[aip].actor].y - pickup[i].y));
//  if (actor[ai[aip].actor].y > pickup[i].y + (150 * GRAIN)) dist = 50002;
  if (dist < shortest_distance)
  {
   closest_pickup = i;
   shortest_distance = dist;
  }
 }

 if (closest_pickup != -1)
 {
//  message(-1, "Found fruit!", 31);
  ai[aip].task = TASK_FIND_AMM;
  ai[aip].pickup_wanted = closest_pickup;
  ai[aip].clear_sight = 0;
  ai[aip].dest_x = pickup[closest_pickup].x;
  ai[aip].dest_y = pickup[closest_pickup].y;
  return 1;
 }

 return 0;

}



void tell_ai_pickup_gone(int pk)
{

 int ua;

 for (ua = 0; ua < MAX_AI; ua ++)
 {
  if (ai[ua].active == 0) continue;
  if (ai[ua].pickup_wanted == pk)
  {
   ai[ua].pickup_wanted = -1;
  }
 }

}


void equip_ai(int aie)
{
 int i = 0;

 int last_weap = 0;
 int no_weaps = 0;

 for (i = 0; i < NO_WEAPONS; i ++)
 {
  if (game[0].weapon_status [i] >= 1 && game[0].weapon_status [i] <= 3)
  {
   no_weaps ++;
   last_weap = i;
  }
 }




}


void reset_dead_ai(int dactor)
{

 int ua;

 for (ua = 0; ua < MAX_AI; ua ++)
 {
  if (ai[ua].active == 0) continue;
  if (ai[ua].actor == dactor)
  {
   break;
  }
 }


    if (arena[0].ai_weapons == 1)
    {
     ai_select_weapons(ua);
    }

    ai[ua].mode = MODE_KILL;
    ai[ua].mode_object = -1;
    ai[ua].mode_object2 = -1;
    
    ai[ua].task = TASK_WANDER;
    ai[ua].task_object = -1;

    ai[ua].doing = DOING_NONE;
    ai[ua].doing_object = -1;

    ai[ua].next_move = NEXT_NONE;

    ai[ua].angling = 0;
    ai[ua].angle_wanted = 0;
    ai[ua].firing_burst = 0;
    ai[ua].weapon_angle [0] = 0;
    ai[ua].weapon_angle [1] = 0;
    ai[ua].weapon_angle [2] = 0;
    ai[ua].weapon_angle [3] = 0;
    ai[ua].grapple_angle = 0;

    ai[ua].counter = ua * 17;
    ai[ua].dest_x = prand(arena[0].max_x) * GRAIN;
    ai[ua].dest_y = prand(arena[0].max_y) * GRAIN;
    ai[ua].frustration = 0;
    ai[ua].digging = 0;
    ai[ua].grapple_count = 0;
    ai[ua].launched_grapple = 0;
    ai[ua].pickup_wanted = -1;

    ai[ua].clear_sight = 0;
    ai[ua].no_pickups = 0;


}


void ai_select_weapons(int ais)
{

 if (game[0].starting_weapons == 0
  && arena[0].ai_weapons == 0) return;

 int uss = ais; //ai[ais].user;

 int wpn = 0;
 int wcn = 0;
 int timeout = 0;

 char starting_equip = 0;

 int i;

 for (i = 0; i < NO_EQUIP; i ++)
 {
  if (game[0].equip_status [i] > 0
      && game[0].equip_status [i] < 4) // ie it's avail at start
      {
       starting_equip = 1;
      }
 }

 if (arena[0].ai_weapons == 1)
  starting_equip = 1;

 do
 {
  timeout = 0;

//  if (wpn == 1 && ai_unrand(ais, 10) != 0 && starting_equip == 1)
  if (wpn == 1 && ai_unrand(ais, 2) == 0 && starting_equip == 1)
  {
   // give ai equipment in its 2nd slot sometimes
    do
    {
      wcn = ai_unrand(ais, NO_EQUIP);
      timeout++;
      if (timeout > 5000)
      {
       wcn = EQUIP_SHIELD;
       break;
      }
    } while (
     (game[0].equip_status [wcn] == 0
     || game[0].equip_status [wcn] == 4
     || game[0].equip_status [wcn] == 5)
    && arena[0].ai_weapons == 0);

    user[uss].weapon [wpn] = wcn + 2;
    wpn ++;
    continue;
//   actor[ractor].weapon [slot] = pickup[pcount].contains + 2;
   
  }
   else
   {
    timeout = 0;
    do
    {
     do
     {
      if (arena[0].challenge_level == 0)
       wcn = ai_unrand(ais, NO_WEAPONS - 5) + 5;
        else
         wcn = prand(NO_WEAPONS - 5) + 5;
     } while (wcn == WPN_LASER_TRACER);
     timeout ++;
     if (timeout == 5000)
     {
      wcn = WPN_LMG;
      break;
     }
    } while (
     (game[0].weapon_status [wcn] == 0
     || game[0].weapon_status [wcn] == 4
     || game[0].weapon_status [wcn] == 5)
    && arena[0].ai_weapons == 0);
   }
  user[uss].weapon [wpn] = wcn;

//  message(-1, wlist[wcn].long_name, 31);
  wpn ++;
  if (game[0].starting_weapons == 1 && arena[0].ai_weapons == 0) break;
// doesn't get this far if equipment is given.
 } while (wpn < 4);
 

}

/*
Generate a pseudorandom number based on ai's name (to avoid using
prand during initialisation, which puts clients out of synch).
*/
int ai_unrand(int aiu, int limit)
{

 if (limit <= 0) return 0;

 static int counter = 0;

 int seedy = user[aiu].uname [0] + user[aiu].uname [1] + user[aiu].uname [2]
  + user[aiu].uname [3] + user[aiu].uname [4];

 counter ++;

 return (seedy * counter) % limit;

}

