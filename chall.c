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

File: chall.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions dealing with the challenge mode, and association data.

*/

#include "allegro.h"
#include <string.h>

#include "config.h"

#include "globvar.h"

#include "menu.h"
#include "misc.h"
#include "prand.h"

/*
To add:
arena[0].challenge_level

challenge levels start at 1 because 0 is used to indicate that we're
not in a challenge game.
*/

/*
To do:

first, copy the arena[0] struct into arena[1]. Then copy game[0] into
game[whatever] using the function in menu.c. Then start setting up.
As init_people isn't called in a challenge game, we deal with the ai
struct directly rather than going through ai_config.
*/

extern struct ai_config_struct ai_config [MAX_AI];
struct ai_config_struct chall_ai_config_copy [MAX_AI];

void setup_game_config(void);
void copy_arena(int from, int to);
void backup_ai_config(void);
void replace_ai_config(void);
void setup_challenge_ai(struct ai_config_struct *aics, int how_many, int team);
void end_challenge(void);
void chall_weapons(void);
void weapon_set(int game_edit, int which_set);



#define L1_AI_NO 1
struct ai_config_struct challenge_ai_l1 [L1_AI_NO] =
{
 {
  "Norbert",
  1, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SNAIL, // atype
  50, // handicap
  SOLDIER_SOLDIER
 }
};

#define L2_AI_NO 2
struct ai_config_struct challenge_ai_l2 [L2_AI_NO] =
{
 {
  "Slurpy",
  1, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SCOUT, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Schnorkel",
  1, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SOLDIER, // atype
  100, // handicap
  SOLDIER_SOLDIER
 }
};

#define L3_AI_NO 2
struct ai_config_struct challenge_ai_l3 [L3_AI_NO] =
{
 {
  "Eggbert",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CRUSHER, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Eggwyn",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CYBORG, // atype
  100, // handicap
  SOLDIER_SOLDIER
 }
};

#define L4_AI_NO 3
struct ai_config_struct challenge_ai_l4 [L4_AI_NO] =
{
 {
  "Blueberry",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SOLDIER, // atype
  100, // handicap
  SOLDIER_WALKER
 },
 {
  "Taro",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CRUSHER, // atype
  100, // handicap
  SOLDIER_WALKER
 },
 {
  "PorkBelly",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_WALKER, // atype
  100, // handicap
  SOLDIER_WALKER
 }
};

#define L5_AI_NO 3
struct ai_config_struct challenge_ai_l5 [L5_AI_NO] =
{
 {
  "Hannibal",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CRUSHER, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Hasdrubal",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CRUSHER, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Hamilcar",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CRUSHER, // atype
  100, // handicap
  SOLDIER_SOLDIER
 }
};

#define L6_AI_NO 4
struct ai_config_struct challenge_ai_l6 [L6_AI_NO] =
{
 {
  "George",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_WALKER, // atype
  150, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Tony",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CYBORG, // atype
  120, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Saddam",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_DEMON, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "John",
  0, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SNAIL, // atype
  30, // handicap
  SOLDIER_SOLDIER
 }
};

#define L7_AI_NO 4
struct ai_config_struct challenge_ai_l7 [L7_AI_NO] =
{
 {
  "Space Bug A",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_BUG, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Space Bug B",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_BUG, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Space Bug C",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_BUG, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "War Snail",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SNAIL, // atype
  100, // handicap
  SOLDIER_SOLDIER
 }
};

#define L8_AI_NO 5
struct ai_config_struct challenge_ai_l8 [L8_AI_NO] =
{
 {
  "Spiney",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SKELETON, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Bony",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SKELETON, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Ribby",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SKELETON, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Nasty",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_DEMON, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Pokey",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_DEMON, // atype
  100, // handicap
  SOLDIER_SOLDIER
 }
};


#define L9_AI_NO 5
struct ai_config_struct challenge_ai_l9 [L9_AI_NO] =
{
 {
  "Capt Pork",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_CYBORG, // atype
  200, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Lt Herring",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_WALKER, // atype
  100, // handicap
  SOLDIER_WALKER
 },
 {
  "Col Globby",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_WALKER, // atype
  100, // handicap
  SOLDIER_SOLDIER
 },
 {
  "Capt Lemming",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SCOUT, // atype
  100, // handicap
  SOLDIER_WALKER
 },
 {
  "Sir Porkupine",
  2, // skill
  TEAM_NONE,
  1, // active
  SOLDIER_SOLDIER, // atype
  100, // handicap
  SOLDIER_WALKER
 }
};



void prepare_challenge_game(void)
{

 no_players = 1;
 copy_arena(0, 1);
 copy_gametype(0, LAST_GAME);
 backup_ai_config();

 setup_game_config();

}




void setup_challenge_level(void)
{

 int i;

 int team = TEAM_RED;

 if (player[1].team == TEAM_RED)
  team = TEAM_BLUE;

 setup_game_config();


 switch(arena[0].challenge_level)
 {
  case 1:
  chall_weapons(); // must be before ais are set up.
  setup_challenge_ai(challenge_ai_l1, L1_AI_NO, team);
  information_box("Welcome to the Challenge!", "You must win each of", "seven rounds to be victorious.", 1);
  information_box("Level 1", "Take on Norbert, who is fat,", "slow and not very dangerous.", 1);
  arena[0].max_x = 300;
  arena[0].max_y = 300;
  arena[0].level_colours = COLOURS_CLASSIC;
  arena[0].level_style = LEVEL_CLASSIC;
  game[0].score_limit = 5;
  game[0].pk_number = 4;
  break;
  case 2:
  information_box("Level 2", "Slurpy and Schnorkel are a little more", "dangerous than Norbert. But not much.", 1);
  chall_weapons();
  setup_challenge_ai(challenge_ai_l2, L2_AI_NO, team);
  arena[0].max_x = 400;
  arena[0].max_y = 300;
  game[0].score_limit = 5;
  arena[0].level_colours = COLOURS_CLASSIC;
  arena[0].level_style = LEVEL_CLASSIC;
  game[0].pk_number = 7;
  break;
  case 3:
  arena[0].teams = 1;
  information_box("Level 3", "This time, Eggwyn and Eggbert are", "teaming up against you.", 1);
  chall_weapons();
  setup_challenge_ai(challenge_ai_l3, L3_AI_NO, team);
  arena[0].level_colours = COLOURS_EARTH;
  arena[0].level_style = LEVEL_PLATFORM;
  arena[0].max_x = 500;
  arena[0].max_y = 400;
  game[0].score_limit = 10;
  break;
  case 4:
  information_box("Level 4", "A free-for-all against three", "reasonably capable opponents.", 1);
  chall_weapons();
  setup_challenge_ai(challenge_ai_l4, L4_AI_NO, team);
  arena[0].level_colours = COLOURS_EARTH;
  arena[0].level_style = LEVEL_CITY;
  arena[0].max_x = 500;
  arena[0].max_y = 400;
  game[0].score_limit = 10;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (game[0].weapon_status [i] == 4)
    game[0].weapon_status [i] = 2;
  }
  game[0].starting_weapons = 1;
  arena[0].dirt_fill = 1;
  break;
  case 5:
  information_box("Level 5", "You're severely outnumbered!", "Can you overcome the odds?", 1);
  arena[0].teams = 1;
  chall_weapons();
  setup_challenge_ai(challenge_ai_l5, L5_AI_NO, team);
  arena[0].level_colours = COLOURS_CRAZY;
  arena[0].level_style = LEVEL_PLATFORM;
  arena[0].max_x = 600;
  arena[0].max_y = 300;
  game[0].score_limit = 10;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (game[0].weapon_status [i] == 4)
    game[0].weapon_status [i] = 2;
  }
//  game[0].starting_weapons = 0;
  arena[0].dirt_fill = 1;
  break;
  case 6:
  information_box("Level 6", "A big, nasty free for all.", "", 1);
  arena[0].teams = 0;
  chall_weapons();
  setup_challenge_ai(challenge_ai_l6, L6_AI_NO, team);
  arena[0].level_colours = COLOURS_CLASSIC;
  arena[0].level_style = LEVEL_CLASSIC;
  arena[0].max_x = 800;
  arena[0].max_y = 300;
  game[0].score_limit = 15;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (game[0].weapon_status [i] == 4)
    game[0].weapon_status [i] = 2;
  }
  game[0].starting_weapons = 2;
  arena[0].dirt_fill = 1;
  break;
  case 7:
  information_box("Level 7", "It's you against the space bugs", "and an angry snail.", 1);
  arena[0].teams = 1;
  chall_weapons();
  setup_challenge_ai(challenge_ai_l7, L7_AI_NO, team);
  arena[0].level_colours = COLOURS_CRAZY;
  arena[0].level_style = LEVEL_PLATFORM;
  arena[0].max_x = 800;
  arena[0].max_y = 300;
  game[0].score_limit = 15;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (game[0].weapon_status [i] == 4)
    game[0].weapon_status [i] = 2;
  }
  game[0].starting_weapons = 2;
  arena[0].dirt_fill = 1;
  break;
  case 8:
  information_box("Level 8", "Welcome to Hell.", "", 1);
  arena[0].teams = 0;
  chall_weapons();
  setup_challenge_ai(challenge_ai_l8, L8_AI_NO, team);
  arena[0].level_colours = COLOURS_HELL;
  arena[0].level_style = LEVEL_CLASSIC;
  arena[0].max_x = 800;
  arena[0].max_y = 300;
  game[0].score_limit = 15;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (game[0].weapon_status [i] == 4)
    game[0].weapon_status [i] = 2;
  }
  game[0].starting_weapons = 2;
  arena[0].dirt_fill = 1;
  arena[0].ambient_light = 0;
  break;
  case 9:
  information_box("Level 9", "Your final challenge.", "Good luck!", 1);
  arena[0].teams = 1;
  chall_weapons();
  setup_challenge_ai(challenge_ai_l9, L9_AI_NO, team);
  arena[0].level_colours = COLOURS_CLASSIC;
  arena[0].level_style = LEVEL_CITY;
  arena[0].max_x = 900;
  arena[0].max_y = 300;
  game[0].score_limit = 15;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (game[0].weapon_status [i] == 4)
    game[0].weapon_status [i] = 2;
  }
  game[0].starting_weapons = 2;
  arena[0].dirt_fill = 1;
  arena[0].ambient_light = 1;
  break;

 }


}


void chall_weapons(void)
{
  game[0].weapon_status [WPN_BOMB] = 2;
  game[0].weapon_status [WPN_NEEDLER] = 2;
  game[0].weapon_status [WPN_GRAPESHOT] = 2;
  game[0].weapon_status [WPN_RPG] = 2;
  game[0].weapon_status [WPN_SEMI_AUTO] = 2;
  if (arena[0].challenge_level == 1)
   return;
  game[0].weapon_status [WPN_LMG] = 4;
  game[0].weapon_status [WPN_HMG] = 4;
  game[0].weapon_status [WPN_GR_L] = 4;
  game[0].weapon_status [WPN_GREN] = 4;
  game[0].weapon_status [WPN_FRAG] = 4;
  game[0].weapon_status [WPN_SGUN] = 4;
  game[0].weapon_status [WPN_SCATTER] = 4;
  game[0].weapon_status [WPN_LR_R] = 4;
  if (arena[0].challenge_level == 2)
   return;
  game[0].weapon_status [WPN_R_L] = 4;
  game[0].weapon_status [WPN_SLUG] = 4;
  game[0].weapon_status [WPN_FLAKKER] = 4;
  game[0].weapon_status [WPN_SWARM] = 4;
  game[0].weapon_status [WPN_SQUIRM] = 4;
  game[0].weapon_status [WPN_AUTOCANNON] = 4;
  game[0].weapon_status [WPN_FIREBOMB] = 4;
  game[0].weapon_status [WPN_FIREBALL] = 4;
  game[0].weapon_status [WPN_BLUNDER] = 4;
  game[0].weapon_status [WPN_NIBBLER] = 4;
  game[0].weapon_status [WPN_GAS_GREN] = 4;
  game[0].weapon_status [WPN_FUMIGATOR] = 4;
  game[0].weapon_status [WPN_CLOD_OF_DIRT] = 4;
  game[0].weapon_status [WPN_DIRTBOMB] = 4;
  game[0].weapon_status [WPN_INCENDIARIES] = 4;
  if (arena[0].challenge_level == 3)
   return;
  game[0].weapon_status [WPN_PLAS_R] = 4;
  game[0].weapon_status [WPN_PLAS_C] = 4;
  game[0].weapon_status [WPN_CUBE] = 4;
  game[0].weapon_status [WPN_PLINKER] = 4;
  game[0].weapon_status [WPN_LASER_PULSE] = 4;
  game[0].weapon_status [WPN_TOXIN] = 4;
  game[0].weapon_status [WPN_PRONGTHROWER] = 4;
  game[0].weapon_status [WPN_REMOTE_ROCKET] = 4;
  game[0].weapon_status [WPN_REMOTE_ROCKET_C] = 5;
  game[0].weapon_status [WPN_TRACKER] = 4;
  game[0].weapon_status [WPN_SEEKER] = 4;
  game[0].weapon_status [WPN_SHREDDER] = 4;
  game[0].equip_status [EQUIP_JETPACK] = 4;
  game[0].equip_status [EQUIP_ROCKETPACK] = 4;
  game[0].equip_status [EQUIP_SPOTLIGHT] = 4;
  game[0].pk_equip = 1;
  if (arena[0].challenge_level == 4)
   return;
  game[0].weapon_status [WPN_IMPLODER] = 4;
  game[0].weapon_status [WPN_NUKE_M] = 5;
  game[0].weapon_status [WPN_RECTIFIER] = 5;
  game[0].weapon_status [WPN_FUNKY_BOMB] = 5;
  game[0].weapon_status [WPN_DISRUPTER] = 4;
  game[0].weapon_status [WPN_DISRUPTER_WAVE] = 4;
  game[0].weapon_status [WPN_HUNTER] = 5;
  game[0].weapon_status [WPN_BOUNCY] = 4;
  game[0].weapon_status [WPN_FTHROWER] = 4;
  game[0].weapon_status [WPN_NAPALM] = 4;
  game[0].weapon_status [WPN_LASER_BEAM] = 4;
  game[0].weapon_status [WPN_LASER_PULSE] = 4;
  game[0].weapon_status [WPN_SMITER] = 4;
  game[0].weapon_status [WPN_ELECTRO] = 4;
  game[0].equip_status [EQUIP_SHIELD] = 4;
  game[0].equip_status [EQUIP_ARMOUR] = 4;
  game[0].equip_status [EQUIP_CLOAK] = 4;
  game[0].equip_status [EQUIP_REGENERATOR] = 4;
  game[0].equip_status [EQUIP_SHADOW] = 4;

  return;

}

void setup_challenge_ai(struct ai_config_struct *aics, int how_many, int team)
{
 int i;
 
 for (i = 0; i < how_many; i ++)
 {
  strcpy(ai_config[i].name, aics[i].name);
  ai_config[i].skill = aics[i].skill;
  ai_config[i].team = team;
  ai_config[i].active = 1;
  ai_config[i].handicap = aics[i].handicap;
  ai_config[i].atype = aics[i].atype + 1;
  ai_config[i].soldier = aics[i].atype;
 }

}


void change_challenge_level(void)
{

 switch(arena[0].challenge_level)
 {
  default:
  case 1:
   arena[0].challenge_level ++;
   break;

  case 9:
   information_box("You win the challenge!", "Captain Pork salutes your skill.", "", 1);
   end_challenge();
   break;

 }

}


void end_challenge(void)
{
 copy_arena(1, 0);
 copy_gametype(LAST_GAME, 0);
 replace_ai_config();
 arena[0].challenge_level = 0;

}


void copy_arena(int from, int to)
{

 arena[to].max_x = arena[from].max_x;
 arena[to].max_y = arena[from].max_y;
 arena[to].pk_counter = arena[from].pk_counter;
 arena[to].dirt_fill = arena[from].dirt_fill;
 arena[to].teams = arena[from].teams;
 arena[to].ai_no_impact = arena[from].ai_no_impact;
 arena[to].ai_unlimited_clips = arena[from].ai_unlimited_clips;
 arena[to].ai_weapons = arena[from].ai_weapons;
 arena[to].replace_dirt = arena[from].replace_dirt;
 arena[to].level_colours = arena[from].level_colours;
 arena[to].level_style = arena[from].level_style;
 arena[to].solid_density = arena[from].solid_density;
 arena[to].lamp_amount = arena[from].lamp_amount;
 arena[to].ambient_light = arena[from].ambient_light;
 strcpy(arena[to].level_loaded, arena[from].level_loaded);

}


void setup_game_config(void)
{

 int wcount = 0;

// no_players = 1;

 game[0].gravity = 30;
 game[0].game_type = GAME_KILL;
// game[0].score_type = ;
 game[0].score_limit = 2;
// game[0].lives_type = ;
 game[0].lives_each = 0;
 game[0].starting_weapons = 1;
 game[0].bullet_speed = 0;
 game[0].health_amount = 100;
 game[0].impact_damage = 0;
 game[0].reload_time = 10;
 game[0].fast_bullets = 0;
 game[0].bullets_explode = 1;
 game[0].soft_dirt = 1;
 game[0].rechoose_atype = 1;
 game[0].unlimited_clips = 0;
 game[0].reset_weapons = 0;
 game[0].penalty = 0;
 game[0].fruit_no = 5;
 game[0].th_base_no = 4;
 game[0].pk_time_between = 100;
 game[0].pk_number = 10;
 game[0].pk_health = 1;
 game[0].pk_weapon = 3;
 game[0].pk_equip = 0;
 game[0].name_boxes = 1;
 game[0].show_map = 1;
 game[0].blast_bullets = 0; //////////////// ????????????????????????
 game[0].atypes_avail = 2;
 for (wcount = 0; wcount < NO_WEAPONS; wcount ++)
 {
  game[0].weapon_status [wcount] = 0;
 }
 for (wcount = 0; wcount < NO_EQUIP; wcount ++)
 {
  game[0].equip_status [wcount] = 0;
 }
 game[0].lightsourcing = arena[0].quickstart_lightsourced;

 arena[0].max_x = 800;
 arena[0].max_y = 600;
 arena[0].dirt_fill = 2;
 arena[0].teams = 0;
 arena[0].ai_no_impact = 1;
 arena[0].ai_unlimited_clips = 0;
 arena[0].ai_weapons = 0;
 arena[0].replace_dirt = 1;
 arena[0].level_colours = COLOURS_EARTH;
 arena[0].level_style = LEVEL_PLATFORM;
 arena[0].solid_density = 1;
 arena[0].ambient_light = 1;
 arena[0].lamp_amount = 6;
 strcpy(arena[0].level_loaded, "");


}




void backup_ai_config(void)
{

 int i;
 for (i = 0; i < MAX_AI; i ++)
 {
   chall_ai_config_copy[i].skill = ai_config[i].skill;
   chall_ai_config_copy[i].team = ai_config[i].team;
   chall_ai_config_copy[i].active = ai_config[i].active;
   chall_ai_config_copy[i].atype = ai_config[i].atype;
   chall_ai_config_copy[i].handicap = ai_config[i].handicap;
   chall_ai_config_copy[i].soldier = ai_config[i].soldier;
   strcpy(chall_ai_config_copy[i].name, ai_config[i].name);

   ai_config[i].skill = 2;
   ai_config[i].team = TEAM_NONE;
   ai_config[i].active = 0;
   ai_config[i].atype = 2;
   ai_config[i].handicap = 100;
   ai_config[i].soldier = SOLDIER_SOLDIER;
   strcpy(ai_config[i].name, "");
   
 }

}

void replace_ai_config(void)
{

 int i;
 for (i = 0; i < MAX_AI; i ++)
 {
   ai_config[i].skill = chall_ai_config_copy[i].skill;
   ai_config[i].team = chall_ai_config_copy[i].team;
   ai_config[i].active = chall_ai_config_copy[i].active;
   ai_config[i].atype = chall_ai_config_copy[i].atype;
   ai_config[i].handicap = chall_ai_config_copy[i].handicap;
   ai_config[i].soldier = chall_ai_config_copy[i].soldier;
   strcpy(ai_config[i].name, chall_ai_config_copy[i].name);
 }

}




void prepare_qstart_game(int which_type)
{
 arena[0].qstart = which_type;
 copy_arena(0, 1);
 copy_gametype(0, LAST_GAME);
 backup_ai_config();

 setup_game_config();

}




void setup_qstart_level(int which_type, char async)
{

 int i;
/*
 int team = TEAM_RED;

 if (player[1].team == team)
  team = TEAM_BLUE;

 do
 {
 team
 }
  while(player[1].team == team
*/
 setup_game_config();


  arena[0].max_x = 700 + prand(4) * 100;
  arena[0].max_y = 400 + prand(3) * 100;
  arena[0].level_colours = COLOURS_EARTH;
  if (prand(4) == 0)
   arena[0].level_colours = COLOURS_CRAZY;
  arena[0].level_style = LEVEL_CLASSIC;
  if (prand(3) == 0 || (async == 1 && prand(3) != 0))
   arena[0].level_style = LEVEL_PLATFORM;
  if (prand(3) == 0 || (async == 1 && prand(3) != 0))
   arena[0].level_style = LEVEL_CITY;
  arena[0].dirt_fill = arena[0].quickstart_dirt;
  if (arena[0].dirt_fill == 0)
    arena[0].replace_dirt = 0;

  game[0].score_limit = 15;
  game[0].pk_number = 10;
//  game[0].pk_time_between = 1;
  game[0].starting_weapons = 1;
  if (arena[0].quickstart_weapons > 3)
   game[0].starting_weapons = 2;
  if (arena[0].quickstart_weapons == 7)
   game[0].starting_weapons = 3; // class weapons
//  arena[0].dirt_fill = 1;
  arena[0].ambient_light = 2;
  arena[0].lamp_amount = 5 + prand(2);
  if (arena[0].quickstart_lightsourced == 1)
  {
   game[0].lightsourcing = 1;
   arena[0].ambient_light = 1;
   if (prand(7) == 0)
    arena[0].ambient_light = 0;
  }

  switch(which_type)
  {
   case QSTART_DUEL:
    game[0].score_limit = 10;
   case QSTART_DUEL_TEAM:
   case QSTART_MELEE:
   case QSTART_MELEE_TEAM:
    game[0].game_type = GAME_KILL;
    break;
   case QSTART_LAST_ONE:
    game[0].game_type = GAME_LAST_ONE;
    game[0].lives_each = 10;
    game[0].score_limit = 200;
    break;
   case QSTART_CAPFLAG:
    game[0].game_type = GAME_CAP_FLAG;
    game[0].penalty = 9;
    break;
//   case QSTART_:
//    game[0].game_type = GAME_CAP_FLAG;
//    break;
   case QSTART_FRUIT:
    game[0].game_type = GAME_FRUIT_HUNT;
    game[0].score_limit = 50;
    game[0].penalty = 9;
    break;
   case QSTART_TAKEHOLD:
    game[0].game_type = GAME_TAKEHOLD;
    game[0].score_limit = 30;
    game[0].penalty = 9;
    break;
   case QSTART_GRAIL:
    game[0].game_type = GAME_GRAIL;
    game[0].score_limit = 15;
    game[0].penalty = 9;
    break;

  }

// arena[0].challenge_level = 8; // for chall_weapons
 weapon_set(0, arena[0].quickstart_weapons); // must be before ais are set up.
// arena[0].challenge_level = 0;

//  game[0].pk_equip = 3;
//  game[0].pk_weapon = 0;
//  game[0].pk_health = 0;
//  game[0].equip_status [2] = 4;
//  game[0].equip_status [3] = 2;



 int how_many = 4;

 arena[0].teams = 1;

 int default_team = TEAM_RED;
 if (player[1].team == TEAM_RED)
  default_team = TEAM_BLUE;

// int team_one = 1;

 if (no_players == 2)
 {
  if (player[1].team == player[2].team)
  {
   ai_config[0].team = player[1].team;
   ai_config[1].team = default_team;
   ai_config[2].team = default_team;
   ai_config[3].team = default_team;
   how_many = 4;
  }
   else
   {
    ai_config[0].team = player[1].team;
    ai_config[1].team = player[1].team;
    ai_config[2].team = player[2].team;
    ai_config[3].team = player[2].team;
    how_many = 4;
   }
 } else
 {
  ai_config[0].team = player[1].team;
  ai_config[1].team = player[1].team;
  ai_config[2].team = default_team;
  ai_config[3].team = default_team;
  ai_config[4].team = default_team;
  how_many = 5;
 }

 if (arena[0].qstart == QSTART_DUEL)
 {
  arena[0].teams = 0;
  if (no_players == 1 && async == 0) // no ais in networked duel
   how_many = 1;
    else
     how_many = 0;
 }

 if (arena[0].qstart == QSTART_DUEL_TEAM)
 {
  arena[0].teams = 1;
  if (no_players == 1 && async == 0) // no ais in networked duel
  {
   how_many = 1;
   ai_config[0].team = default_team;
  }
    else
     how_many = 0;
 }

 for (i = 0; i < how_many; i ++)
 {
  strcpy(ai_config[i].name, "");
  ai_config[i].skill = arena[0].quickstart_difficulty;
  if (arena[0].quickstart_difficulty == 3)
  {
   ai_config[i].skill = 2;
   arena[0].ai_weapons = 1;
  }
//  ai_config[i].team = eam;
  ai_config[i].active = 1;
  ai_config[i].handicap = 100; //aics[i].handicap;
  if (arena[0].quickstart_difficulty == 0) ai_config[i].handicap = 75;
  ai_config[i].atype = 0;
  ai_config[i].soldier = 0;
//  ai_config[i].soldier = aics[i].atype;
 }

  if (arena[0].qstart == QSTART_MELEE
     || arena[0].qstart == QSTART_FRUIT
     || arena[0].qstart == QSTART_LAST_ONE
     || arena[0].qstart == QSTART_TOURNAMENT)
 {
  arena[0].teams = 0;
 }

 
}





void end_qstart(void)
{
 copy_arena(1, 0);
 copy_gametype(LAST_GAME, 0);
 replace_ai_config();
 arena[0].challenge_level = 0;
 arena[0].qstart = QSTART_NONE;

}



/*

TO DO:
Options:

Dynamic light sourcing
Dirt
AI difficulty level

*/







void weapon_set(int game_edit, int which_set)
{

 int i;

/*
  0 - none
   - All weapons in boxes
  1/4 - Basic
   - Only some in boxes
  2/5 - Extended
   - All weapons in boxes
  3/6 - Full
   - All weapons in boxes


REMEMBER EQUIPMENT
*/

  for (i = 0; i < NO_WEAPONS; i ++)
  {
    game[game_edit].weapon_status [i] = 0; // doesn't exist
  }

  switch(which_set)
  {
   default:
   case 0:
   for (i = 0; i < NO_WEAPONS; i ++)
   {
     game[game_edit].weapon_status [i] = 4;
   }
   game[game_edit].weapon_status [WPN_NUKE_M] = 5; // rare in boxes
   game[game_edit].weapon_status [WPN_RECTIFIER] = 5;
   game[game_edit].weapon_status [WPN_FUNKY_BOMB] = 5;
   game[game_edit].weapon_status [WPN_HUNTER] = 5;

   game[game_edit].equip_status [EQUIP_JETPACK] = 4;
   game[game_edit].equip_status [EQUIP_ROCKETPACK] = 4;
   game[game_edit].equip_status [EQUIP_SPOTLIGHT] = 4;
   
   game[game_edit].equip_status [EQUIP_SHIELD] = 4;
   game[game_edit].equip_status [EQUIP_ARMOUR] = 4;
   game[game_edit].equip_status [EQUIP_CLOAK] = 4;
   game[game_edit].equip_status [EQUIP_REGENERATOR] = 4;
   game[game_edit].equip_status [EQUIP_SHADOW] = 4;
   game[0].pk_equip = 1;
   break;
   case 1:
   case 4:
   game[game_edit].weapon_status [WPN_LMG] = 4;
   game[game_edit].weapon_status [WPN_HMG] = 4;
   game[game_edit].weapon_status [WPN_GR_L] = 4;
   game[game_edit].weapon_status [WPN_GREN] = 4;
   game[game_edit].weapon_status [WPN_FRAG] = 4;
   game[game_edit].weapon_status [WPN_SGUN] = 4;
   game[game_edit].weapon_status [WPN_SCATTER] = 4;
   game[game_edit].weapon_status [WPN_LR_R] = 4;
   game[game_edit].weapon_status [WPN_R_L] = 4;
   game[game_edit].weapon_status [WPN_SLUG] = 4;
   game[game_edit].weapon_status [WPN_FLAKKER] = 4;
   game[game_edit].weapon_status [WPN_SWARM] = 4;
   game[game_edit].weapon_status [WPN_SQUIRM] = 4;
   game[game_edit].weapon_status [WPN_AUTOCANNON] = 4;
   game[game_edit].weapon_status [WPN_FIREBOMB] = 4;
   game[game_edit].weapon_status [WPN_BLUNDER] = 4;
   game[game_edit].weapon_status [WPN_NIBBLER] = 4;
   game[game_edit].weapon_status [WPN_GAS_GREN] = 4;
   game[game_edit].weapon_status [WPN_FUMIGATOR] = 4;
   game[game_edit].weapon_status [WPN_CLOD_OF_DIRT] = 4;
   game[game_edit].weapon_status [WPN_DIRTBOMB] = 4;
   game[game_edit].weapon_status [WPN_INCENDIARIES] = 4;
   game[game_edit].weapon_status [WPN_REMOTE_ROCKET] = 4;
   game[game_edit].weapon_status [WPN_REMOTE_ROCKET_C] = 4;
   game[game_edit].weapon_status [WPN_TRACKER] = 4;
   game[game_edit].weapon_status [WPN_SEEKER] = 4;
   game[game_edit].weapon_status [WPN_FTHROWER] = 4;
   game[game_edit].weapon_status [WPN_NAPALM] = 4;
   game[game_edit].weapon_status [WPN_NUKE_M] = 5; // rare

   game[game_edit].weapon_status [WPN_BOMB] = 1;
   game[game_edit].weapon_status [WPN_NEEDLER] = 1;
   game[game_edit].weapon_status [WPN_GRAPESHOT] = 1;
   game[game_edit].weapon_status [WPN_RPG] = 1;
   game[game_edit].weapon_status [WPN_SEMI_AUTO] = 1;

   game[game_edit].equip_status [EQUIP_SPOTLIGHT] = 4;
   game[0].pk_equip = 1;
   break;
   case 2:
   case 5:
   for (i = 0; i < NO_WEAPONS; i ++)
   {
     game[game_edit].weapon_status [i] = 4; // common in boxes
   }
   game[game_edit].weapon_status [WPN_LMG] = 2; // start + common
   game[game_edit].weapon_status [WPN_HMG] = 2;
   game[game_edit].weapon_status [WPN_GR_L] = 2;
   game[game_edit].weapon_status [WPN_GREN] = 2;
   game[game_edit].weapon_status [WPN_SGUN] = 2;
   game[game_edit].weapon_status [WPN_SCATTER] = 2;
   game[game_edit].weapon_status [WPN_LR_R] = 2;
   game[game_edit].weapon_status [WPN_SLUG] = 2;
   game[game_edit].weapon_status [WPN_FLAKKER] = 2;
   game[game_edit].weapon_status [WPN_SWARM] = 2;
   game[game_edit].weapon_status [WPN_SQUIRM] = 2;
   
   game[game_edit].weapon_status [WPN_NUKE_M] = 5; // rare in boxes
   game[game_edit].weapon_status [WPN_RECTIFIER] = 5;
   game[game_edit].weapon_status [WPN_FUNKY_BOMB] = 5;
   game[game_edit].weapon_status [WPN_HUNTER] = 5;

   game[game_edit].equip_status [EQUIP_JETPACK] = 2;
   game[game_edit].equip_status [EQUIP_ROCKETPACK] = 2;
   game[game_edit].equip_status [EQUIP_SPOTLIGHT] = 2;
   
   game[game_edit].equip_status [EQUIP_SHIELD] = 4;
   game[game_edit].equip_status [EQUIP_ARMOUR] = 4;
   game[game_edit].equip_status [EQUIP_CLOAK] = 4;
   game[game_edit].equip_status [EQUIP_REGENERATOR] = 4;
   game[game_edit].equip_status [EQUIP_SHADOW] = 4;
   game[0].pk_equip = 1;

   break;
   case 3:
   case 6:
   for (i = 0; i < NO_WEAPONS; i ++)
   {
     game[game_edit].weapon_status [i] = 2; // start + common
   }
   game[game_edit].weapon_status [WPN_NUKE_M] = 5; // rare in boxes
   game[game_edit].weapon_status [WPN_RECTIFIER] = 5;
   game[game_edit].weapon_status [WPN_FUNKY_BOMB] = 5;
   game[game_edit].weapon_status [WPN_HUNTER] = 5;

   game[game_edit].equip_status [EQUIP_JETPACK] = 2;
   game[game_edit].equip_status [EQUIP_ROCKETPACK] = 2;
   game[game_edit].equip_status [EQUIP_SPOTLIGHT] = 2;
   game[game_edit].equip_status [EQUIP_SHIELD] = 2;
   game[game_edit].equip_status [EQUIP_ARMOUR] = 2;
   game[game_edit].equip_status [EQUIP_CLOAK] = 2;
   game[game_edit].equip_status [EQUIP_REGENERATOR] = 2;
   game[game_edit].equip_status [EQUIP_SHADOW] = 2;
   game[0].pk_equip = 1;
   break;
  }

}

