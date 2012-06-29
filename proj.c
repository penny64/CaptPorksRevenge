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

File: Proj.c
History:
9/11/02 (or thereabouts) - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)
16/6/03 - Captain Pork's Revenge Version 1.0 finalised (CP)
18/6/03 - Captain Pork's Revenge Version 1.01 released
 (fixed a misfeature which was slowing the game on slow computers) (CP)

This file contains:
 - main()
 - various initialisation functions
 - the main game loop
 - miscellaneous stuff


*/

/*

Your guide to source files:
(for more information visit the files themselves)
actor.c
 - initialises actors - attributes and appearance
ai.c
 - various functions controlling the ai players (bots)
async.c
 - high-level networking functions (see also connect.c)
base.c
 - team/T&H bases and scoring for special game modes (CtFlag etc)
bullet.c
 - bullet creation, movement, destruction etc, plus actor hurting & dying
chall.c
 - Challenge mode
cloud.c
 - cloud creation, running etc
cmds.c
 - input and command implementation
connect.c
 - lower-level networking functions (see also async.c). Interfaces w/ Libnet.
disp2.c
 - not used
display.c
 - puts the game onto the screen
effects.c
 - nuclear flashes and earthquakes
grid.c
 - dirt detection, destruction and alteration
level.c
 - map generation and loading from file
menu.c
 - the front-end user interface
misc.c
 - the pop-up information box
move.c
 - actor movement
pickup.c
 - pickups (health & weapon boxes, equipment, flags, bases, the Grail etc)
prand.c
 - random number generator
proj.c
 - this file. The main one where all the important things happen
score.c
 - scoring, score messages, game end
sound.c
 - access Allegro's sound functions
weapon.c
 - a big array full of weapons


Most .h files are just function headers. But some are special:

config.h
 - structs, enums and #defines
globvar.h
 - extern declarations of global variables
menulist.h
 - scripts for the front-end menus
objhead.h, grabhead.h and pointhd.h
 - Allegro datafile headers for objects.dat, points.dat and
   proj.dat. Contain directories of the bitmaps etc in each datafile
grabwav.h
 - Allegro datafile header for sound.dat. Contains the #defines
   for all the wavs

acthead.h is no longer linked to a datafile, but still controls the order
   of the frames in an actor bitmap.

*/

//#define DEBUG_KEYS

#include <stdlib.h>
#include <string.h>
//#include <conio.h>
#include "allegro.h"
#include "libnet.h"

#include "config.h"

#include "base.h"
#include "cmds.h"
#include "move.h"
#include "grid.h"
#include "bullet.h"
#include "display.h"
#include "cloud.h"
#include "sound.h"
#include "menu.h"
#include "pickup.h"
#include "score.h"
#include "level.h"
#include "effects.h"

#include "grabhead.h"
#include "acthead.h"
#include "prand.h"
#include "connect.h"
#include "ai.h"
#include "actor.h"
#include "async.h"
#include "chall.h"
#include "palette.h"
#include "light.h"


COLOR_MAP trans_table;


void get_keys(char key_array [4]);
void init_grid(void);
int grid_ref_x(int position);
int grid_ref_y(int position);
void grid_to_bitmap(int x_pos, int y_pos, int bm_width, int bm_height, BITMAP *g_to_bmp);
unsigned char get_grid(int grid_x, int grid_y);
//void init_actor(int actor_number);
void display_actor(int dactor, int x_centre, int y_centre, int x_limit, int y_limit, BITMAP *bmp);
void turn_init(void);
void init_players(void);
void display_windows(BITMAP *bmp_p1, BITMAP *bmp_p2);
void run_game(void);
void run_actors(void);
int is_actor_lit(int i);
void video_modes(void);
void colour_table(const char *which_call);
void init_config(void);
void init_users1(void);
void init_people(int users, int game_type);
void init_a_player(int p);
void init_actor_struct(void);
void init_everything_once(void);
void init_everything_again1(void);
void init_everything_again2(void);

unsigned char grid [GRID_X] [GRID_Y];
unsigned char underlying_grid [GRID_X] [GRID_Y];
int grid_strength [GRID_X] [GRID_Y];
char keypress [NO_CMDS];
char cmd_buffer [NO_ACTORS] [NO_CMDS]; // don't need NO_CMDS, but it'll do

char cmd_key [NO_CMDS];
char no_players;
char tick_status = 0;

int video_mode = 0;
// 0 = 320x200, 1 = 640x480, 2 = 800x600
extern struct armoury wlist [NO_WEAPONS];
extern int max_menu_entries;

int window_x_sp, window_y, window_x_2p, window_grid_x_sp, window_grid_y;
int window_grid_x_2p, window_centre_x_sp, window_centre_x_2p;
int window_2p_offset, window_centre_y, crosshair_distance;
int stat_x_pos, stat_y_pos, stat_bar_width, stat_pix_per_point_1p;
int stat_pix_per_point_2p;
int score_sp_x_pos, score_2p_x_pos, score_y_pos;
int window_sp_offset, text_size;

int map_sp_x, map_2p_x, map_y, map_width, map_height;

struct bullet_list bullet [MAX_BULLETS];
struct cloud_list cloud [MAX_CLOUDS];
struct pickup_list pickup [MAX_PICKUPS];
struct team_struct team [NO_TEAMS];
struct ai_config_struct ai_config [MAX_AI];

int tick_length; // game logic in 10ms units
int frame_length; // length of frame in ticks
int turn_length; // length of input turn in ticks - must be multiple of frame_length

int pseudorandom;

int game_played = 0;

int game_over = 0;

//extern int level_colours;
//extern int level_style;
//extern int solid_density; // 0, 1, 4, 8 - the higher, the fewer solid things
// defined in level.c

//struct environ_struct environment;


//BITMAP *brick_bmp [20];
BITMAP *brick_bmp [20] [5];
BITMAP *actor_bmp [NO_ACTORS] [NO_ACTOR_BITMAPS] [2] [2];
// that is, [] [] [facings] [without & with backpack]

BITMAP *captain_pork;
BITMAP *game_over_sign;
BITMAP *you_win_sign;

BITMAP *bmp_p1;
BITMAP *bmp_p2;

BITMAP *dirt_bmp;
BITMAP *shadow_bmp;

DATAFILE *datf;
//DATAFILE *actor_file [10];
BITMAP *soldier_file [NO_SOLDIER_FILES]; // [10];
/*DATAFILE *actor_file2;
DATAFILE *actor_file3;
DATAFILE *actor_file4;
DATAFILE *actor_file5;
DATAFILE *actor_file6;
DATAFILE *actor_file7;
DATAFILE *actor_file8;
DATAFILE *actor_file9;*/

FONT *small_font;
FONT *large_font;

struct arena_struct arena [2];

struct game_struct game [NO_GAMES];

struct act_struct actor [NO_ACTORS];

struct user_struct user [NO_USERS];

/*struct player_struct
{
 char actor_controlled;
};*/

struct option_struct options [1];

struct player_struct player [3];

struct serial_game serial [1];

void framecount(void);

volatile int framecounter;
volatile int frames_per_second;

volatile int inputcounter = 0;
volatile int inputs_per_second = 0;

volatile int turncounter = 0;
volatile int turns_per_second = 0;

void tickover(void);

volatile char ticked;
volatile unsigned char tick_counter;
int slacktime;

//PALETTE palet2;
RGB palet3 [256];
RGB palet [256];
RGB palet2 [256];
//RGB palette_data [256];
//PALETTE white_palette;
//PALETTE *wh_pal;

extern char sound_active;

int trans_colours_array [7] =
{
TRANS_BLUE,
TRANS_YELLOW,
TRANS_ORANGE,
TRANS_RED,
TRANS_GREEN,
TRANS_WHITE,
TRANS_WHITE
};

int actor_colours_array [7] =
{
COLOUR_RED4,
COLOUR_BLUE4,
COLOUR_GREEN4,
COLOUR_YELLOW4,
COLOUR_GREY8,
COLOUR_BROWN4,
COLOUR_GREY4
};


void framecount(void)
{
   frames_per_second = framecounter;
   framecounter = 0;
   turns_per_second = turncounter;
   turncounter = 0;
   inputs_per_second = inputcounter;
   inputcounter = 0;
}
END_OF_FUNCTION (framecount);


void tickover(void)
{
 ticked ++;
 tick_counter++; // assumes it'll wrap at 256
}
END_OF_FUNCTION (tickover);


void init_actor_struct(void)
{

 int da;
 for (da = 0; da < NO_ACTORS; da ++)
 {
  actor[da].aclass = ACLASS_NONE;
  actor[da].atype = ATYPE_NONE;
  actor[da].incarnation = 0;
 }

}
/*
void init_player(int p)
{


init_actor(1, 1);
player [p].actor_controlled = 1;

if (no_players == 2)
{
   init_actor(2, 2);
   player [2].actor_controlled = 2;
}

player [1].changing_weapon = 0;
player [1].slot_selected = 0;
player [1].change_delay = 0;

player [2].changing_weapon = 0;
player [2].slot_selected = 0;
player [2].change_delay = 0;

player[1].weapon [0] = WPN_NONE;
player[1].weapon [1] = WPN_NONE;
player[1].weapon [2] = WPN_NONE;
player[1].weapon [3] = WPN_NONE;
player[2].weapon [0] = WPN_NONE;
player[2].weapon [1] = WPN_NONE;
player[2].weapon [2] = WPN_NONE;
player[2].weapon [3] = WPN_NONE;

}
*/



/*
This function no longer used.

void assign_ai_teams(void)
{

 int i = 0;
 int j = 0;
 int k = 0;

 switch(arena[0].ai_team_dist)
 {
   case 0: // finds an empty team, puts all ai into it.
   for (i = 0; i < NO_TEAMS; i ++)
   {
    k = 0;
    for (j = 0; j < NO_USERS; j ++)
    {
     if (user[j].team == i) k ++;
    }
    if (k == 0) break;
   }
   if (i == NO_TEAMS) i --; // if no empty teams, they go in team Brown

   for (j = 0; j < NO_USERS; j ++)
   {
    if (user[j].status == USTAT_AI) user[i].team = j;
   }
   break;
   case 1:
   for (i = 0; i < NO_TEAMS; i ++)
   {
    k = 0;
    for (j = 0; j < NO_USERS; j ++)
    {
     if (user[j].team == i) k ++;
    }
    if (k == 0) break;
   }
   break;
   case 2:
   // not used
   break;


 }

}
*/

void init_players(void)
{

 player[1].actor_controlled = -1;
 player[2].actor_controlled = -1;
 player[1].moving = 0;
 player[2].moving = 0;
 player[1].has_dug = 0;
 player[2].has_dug = 0;
 player[1].atype = ATYPE_NONE;
 player[2].atype = ATYPE_NONE;
 player[1].ingame_slot = 0;
 player[2].ingame_slot = 0;
 player[1].show_scores = 0;
 player[2].show_scores = 0;
 player[1].show_names = 1;
 player[2].show_names = 1;
 player[1].winner = 0;
 player[2].winner = 0;
 player[1].weapon [0] = WPN_NONE;
 player[1].weapon [1] = WPN_NONE;
 player[1].weapon [2] = WPN_NONE;
 player[1].weapon [3] = WPN_NONE;
 player[2].weapon [0] = WPN_NONE;
 player[2].weapon [1] = WPN_NONE;
 player[2].weapon [2] = WPN_NONE;
 player[2].weapon [3] = WPN_NONE;
 
}

void init_a_player(int p)
{

 player [p].changing_weapon = 0;
 player [p].slot_selected = 0;
 player [p].change_delay = 0;

}

void init_users1(void)
{
 int ui;

 for (ui = 0; ui < NO_USERS; ui ++)
 {
   user[ui].active = 0;
   user[ui].status = USTAT_NONE;
   user[ui].out_of_lives = 0;
   user[ui].ranked = 0;
   user[ui].penalty_time = 0;
   user[ui].resting_time = 329;
   user[ui].weapon [0] = WPN_NONE;
   user[ui].weapon [1] = WPN_NONE;
   user[ui].weapon [2] = WPN_NONE;
   user[ui].weapon [3] = WPN_NONE;
 }

}

void init_people(int users, int game_type)
{

 switch(game[0].game_type)
 {
  case GAME_CAP_FLAG:
  case GAME_GRAIL:
  case GAME_PORKBALL:
  case GAME_TAKEHOLD: 
  arena[0].teams = 1;
  arena[0].has_bases = 1; break;
 }

 int ui;

 int i, j;

 int ai_index [NO_USERS];

 switch(serial[0].game_type)
 {
  case SERIAL_LOCAL_ONLY: // local only
  user[0].player = 1;
  user[0].actor = 0;
  actor[0].aclass = ACLASS_GHOST;
  user[0].active = 1;
  user[0].status = USTAT_LOCAL; // local
  if (no_players == 2)
  {
   user[1].player = 2;
   user[1].actor = 1;
   actor[1].aclass = ACLASS_GHOST;
   user[1].active = 1;
   user[1].status = USTAT_LOCAL; // local
//   actor[user[1].actor].aclass = ACLASS_GHOST;
  }

  for (i = 0; i < MAX_AI; i ++)
  {
   if (ai_config[i].active == 0) continue;
   for (ui = 0; ui < NO_USERS; ui ++)
   {
    if (user[ui].active == 0)
    {
     user[ui].active = 1;
     user[ui].player = 0;
     user[ui].status = USTAT_AI;
     for (j = 0; j < NO_ACTORS; j ++)
     {
      if (actor[j].aclass == ACLASS_NONE)
      {
       actor[j].aclass = ACLASS_GHOST;
       user[ui].actor = j;
       if (arena[0].teams)
       {
        user[ui].team = ai_config[i].team;
        actor[j].team = ai_config[i].team;
       }
        else
        {
         user[ui].team = TEAM_NONE;
         actor[j].team = TEAM_NONE;
        }
       team[user[ui].team].size ++;
       team[user[ui].team].active = 1;
       user[ui].handicap = ai_config[i].handicap;
       user[ui].soldier = ai_config[i].soldier;
       ai_index [ui] = i;
       if (ai_config[i].soldier == NO_SOLDIER_FILES)
        user[ui].soldier = prand(NO_SOLDIER_FILES);
       if (strlen(ai_config[i].name) == 0)
        random_ai_name(user[ui].uname);
         else
          strcpy(user[ui].uname, ai_config[i].name);
// Doesn't deal with skill levels yet.
       break;
      }
     }
     break;
    }
   }
  }
  
/*   user[2].player = 0;
   user[2].actor = 2;
   user[2].active = 1;
   user[2].status = USTAT_AI;
   user[3].player = 0;
   user[3].actor = 3;
   user[3].active = 1;
   user[3].status = USTAT_AI;
   user[4].player = 0;
   user[4].actor = 4;
   user[4].active = 1;
   user[4].status = USTAT_AI;*/
/*   user[3].player = 0;
   user[3].actor = 3;
   user[3].active = 1;
   user[3].status = USTAT_AI;
   user[4].player = 0;
   user[4].actor = 4;
   user[4].active = 1;
   user[4].status = USTAT_AI;
   user[5].player = 0;
   user[5].actor = 5;
   user[5].active = 1;
   user[5].status = USTAT_AI;
   actor[user[2].actor].user = 2;
   actor[user[3].actor].user = 3;
   actor[user[4].actor].user = 4;
   actor[user[5].actor].user = 5;*/
  break;

  case SERIAL_SERVER: // serial - server
  
/*
  user[0].player = 1;
  user[0].actor = 0;
  user[0].active = 1;
  user[0].status = USTAT_LOCAL; // local
  actor[user[0].actor].aclass = ACLASS_GHOST;
  if (no_players == 2) - not yet implemented
  {
   user[1].player = 2;
   user[1].actor = 1;
   user[1].active = 1;
   user[1].status = 1; // local
  }
  user[2].player = 0;
  user[2].actor = 2;
  user[2].active = 1;
  user[2].status = USTAT_CLIENT; // serial, at a client
  actor[user[2].actor].aclass = ACLASS_GHOST;*/
  break;
  
  case SERIAL_CLIENT: // serial - client
/*  user[2].player = 1;
  user[2].actor = 2;
  user[2].active = 1;
  user[2].status = USTAT_LOCAL; // local, but run through server
  actor[user[2].actor].aclass = ACLASS_GHOST;
  // 5 should be local and run through server, but commands implemented
  //  directly at local computer, so server doesn't have to echo them back.
  if (no_players == 2) - not yet implemented
  {
   user[1].player = 2;
   user[1].actor = 1;
   user[1].active = 1;
   user[1].status = 1; // local
  }
  user[0].player = 0; // should never be called
  user[0].actor = 0;
  user[0].active = 1;
  user[0].status = USTAT_SERVER; // serial, at the server
  actor[user[0].actor].aclass = ACLASS_GHOST;*/
  break;
 }

 for (ui = 0; ui < NO_USERS; ui ++)
 {
  if (user[ui].active == 1)
  {
   user[ui].score = 0;
   if (game[0].lives_each == 0)
    user[ui].lives = 0;
     else
      user[ui].lives = game[0].lives_each - 1;
      
   user[ui].out_of_lives = 0;

   actor[user[ui].actor].aclass = ACLASS_GHOST;

   if (user[ui].player > 0)
   {
    player[user[ui].player].user = ui;
    init_a_player(user[ui].player);
    user[ui].weapon [0] = player[user[ui].player].weapon [0];
    user[ui].weapon [1] = player[user[ui].player].weapon [1];
    user[ui].weapon [2] = player[user[ui].player].weapon [2];
    user[ui].weapon [3] = player[user[ui].player].weapon [3];
    user[ui].colour1 = player[user[ui].player].colour1;
    user[ui].colour2 = player[user[ui].player].colour2;
//    user[ui].colour_b = player[user[ui].player].colour_b;
    user[ui].handicap = player[user[ui].player].handicap;
    user[ui].soldier = player[user[ui].player].soldier;
    strcpy(user[ui].uname, player[user[ui].player].pname);
    if (arena[0].teams == 1)
    {
     user[ui].team = player[user[ui].player].team;
     actor[user[ui].actor].team = player[user[ui].player].team;
    } else
     {
      user[ui].team = TEAM_NONE;
      actor[user[ui].actor].team = TEAM_NONE;
     }
    team [player[user[ui].player].team].size ++;
    team [player[user[ui].player].team].active = 1;
    user[ui].atype = player[user[ui].player].atype;
   } else
   {
    switch(user[ui].status)
    {
     case USTAT_AI:
      if (serial[0].game_type == SERIAL_LOCAL_ONLY)
      init_ai_user(ui, user[ui].actor, ai_index [ui]);
     break;
     case USTAT_CLIENT:
     case USTAT_SERVER:
//     player[user[ui].player].user = ui;
//     init_a_player(user[ui].player);
//     user[ui].weapon [0] = WPN_NONE;
//     user[ui].weapon [1] = player[user[ui].player].weapon [1];
//     user[ui].weapon [2] = player[user[ui].player].weapon [2];
//     user[ui].weapon [3] = player[user[ui].player].weapon [3];
//     user[ui].colour_r = player[user[ui].player].colour_r;
//     user[ui].colour_g = player[user[ui].player].colour_g;
//     user[ui].colour_b = player[user[ui].player].colour_b;
//     user[ui].handicap = player[user[ui].player].handicap;
//     strcpy(user[ui].uname, player[user[ui].player].pname);
//     user[ui].team = player[user[ui].player].team;
//     actor[user[ui].actor].team = player[user[ui].player].team;
//     team [player[user[ui].player].team].size ++;
//     team [player[user[ui].player].team].active = 1;
//     user[ui].atype = player[user[ui].player].atype;
     break;
    }
   }
    
   actor[user[ui].actor].user = ui;
   actor[user[ui].actor].player = user[ui].player;
   
   init_actor(user[ui].actor, 0);
   
   if (user[ui].player > 0)
      player[user[ui].player].actor_controlled = user[ui].actor;
  }
 }
 
   text_mode(0);

}



void video_modes(void)
{
 switch(video_mode)
 {
  case 0:
  window_x_sp = 340;
  window_y = 200; // 190??
  window_x_2p = 210; // 160
  window_grid_x_sp = 330; // too high!
  window_grid_y = 210; // 180
  window_grid_x_2p = 170; // 160
  window_centre_x_sp = 160;
  window_centre_x_2p = 80; // 90
  window_centre_y = 100;
  window_2p_offset = 80; // 90
  crosshair_distance = 25;
  text_size = 10;
  
  window_sp_offset = 160;

  stat_x_pos = 5;
  stat_y_pos = 172;
  stat_bar_width = 3;
  stat_pix_per_point_1p = 2;
  stat_pix_per_point_2p = 1;
  score_sp_x_pos = 310;
  score_2p_x_pos = 155;
  score_y_pos = 190;

  map_sp_x = 290;
  map_2p_x = 130;
  map_y = 150;
  map_width = 20;
  map_height = 15;

  max_menu_entries = 7;
  break;
  case 2:
  case 1:
  window_x_sp = 655; // 640 - all these values are + 15 so that there's some
   // extra for distortion_mask to read from.
  window_y = 420; // + 10
  window_x_2p = 335; // 320
/*  window_grid_x_sp = 23;
  window_grid_y = 22;
  window_grid_x_2p = 12;*/
  window_grid_x_sp = 690;
  window_grid_y = 440;
  window_grid_x_2p = 320; // 320
  window_centre_x_sp = 330;
  window_centre_x_2p = 180; // 180
  window_centre_y = 200;
  window_2p_offset = 180;
  crosshair_distance = 30;
  text_size = 20;

  window_sp_offset = 330;

  stat_x_pos = 10;
  stat_y_pos = 400;
  stat_bar_width = 10;
  stat_pix_per_point_1p = 4;
  stat_pix_per_point_2p = 2;
  score_sp_x_pos = 630;
  score_2p_x_pos = 310;
  score_y_pos = 460;

  map_sp_x = 580;
  map_2p_x = 250;
  map_y = 350;
  map_width = 50;
  map_height = 30;

  max_menu_entries = 12;
  break;
/*  case 2:
  window_x_sp = 800;
  window_y = 500;
  window_x_2p = 400;
  window_grid_x_sp = 23;
  window_grid_y = 26;
  window_grid_x_2p = 12;
  window_centre_x_sp = 14;
  window_centre_x_2p = 7;
  window_centre_y = 13;
  window_2p_offset = 400;
  crosshair_distance = 45;
  break;*/
 }

}



int main(int argc, char *argv[])
{
/*   BITMAP *running;
   char datafile_name[256];
   int angle = 0;*/

//   environment = &env;
//   actor[0].colour_1_r = 250;

   init_everything_once();

do
{

   init_async();

   init_everything_again1();

   run_menus(0);

//   init_everything_again1();

   init_people(2, 0);

   init_everything_again2();

   init_cmds2();

   run_game();

   while(arena[0].challenge_level != 0)
   {
    init_everything_again1();
    change_challenge_level();
    if (arena[0].challenge_level == 0)
     break;
    setup_challenge_level();
    run_menus(1);
    init_people(2, 0);
    init_everything_again2();
    init_cmds2();
    run_game();
   }

/*   RGB col1 = {actor[1].colour_1_r, actor[1].colour_1_g, actor[1].colour_1_b};

   RGB *col2 = &col1;
   
   
   set_color(232, col2);*/

//  textprintf(screen, font, 100, 400, palette_color[100], "%i, %i", actor[2].x, actor[2].y);
/*
   RGB *colour_1 [2];
   RGB *colour_2 [2];

   RGB col1;
   RGB col2;
   *colour_1 [0] = col1;
   *colour_2 [0] = col2;

   colour_1 [0]->r = 100;//actor[1].colour_1_r;
   colour_1 [0]->g = 100;//actor[1].colour_1_g;
   colour_1 [0]->b = 100;//actor[1].colour_1_b;
   colour_2 [0]->r = 100; //actor[1].colour_2_r;
   colour_2 [0]->g = 100;//actor[1].colour_2_g;
   colour_2 [0]->b = 100;//actor[1].colour_2_b;

   set_color(232, colour_1 [0]);
   set_color(248, colour_2 [0]);
*/

/*   RGB col1 = {actor[1].colour_1_r, actor[1].colour_1_g, actor[1].colour_1_b};

   RGB *col2 = &col1;
   
   
   set_color(232, col2);*/
//   set_color(248, colour_2 [0]);



//play_sound(WAV_GONG);
/*   RGB col1 = {1, 60, 1};
   RGB *col2 = &col1;
   
   
   set_color(232, col2);*/
//colour_table();


} while (TRUE);

   return 0;
}

END_OF_MAIN();













void init_everything_once(void)
{
   int globg = 0;

   allegro_init();
   install_keyboard();
   install_timer();

   set_config_file("proj.cfg"); // must be before init_cmds
   init_cmds();
   init_ai_struct();

   init_connect(); // in case we want to use a serial connection
//   serial[0].connection_type = NET_DRIVER_SERIAL_DOS;

   key_led_flag = 0; // allows player to use these keys without the leds lighting up when they're pressed
   no_players = 1;
   
   LOCK_FUNCTION (framecount);
   LOCK_FUNCTION (tickover);
   LOCK_VARIABLE (ticked);
   LOCK_VARIABLE (tick_counter);
   LOCK_VARIABLE (frames_per_second);
   LOCK_VARIABLE (framecounter);
   LOCK_VARIABLE (turns_per_second);
   LOCK_VARIABLE (turncounter);
   LOCK_VARIABLE (inputs_per_second);
   LOCK_VARIABLE (inputcounter);

   framecounter = 0;
   frames_per_second = 0;
   ticked = 0;

   tick_length = 3;
   frame_length = 1;
   turn_length = 1;
   // REMEMBER: does not account for frames taking more than one tick_length
   // to display. If this happens, results are unpredictable.
   
//   clrscr();

   cprintf("\n\r\n\rWelcome to Captain Pork's Revenge (version 1.01)!");
   cprintf("\n\r\n\rCaptain Pork's Revenge is free software");
   cprintf("\n\r(you are welcome to redistribute it under certain");
   cprintf("\n\rconditions) and comes with absolutely no warranty.");
   cprintf("\n\rFor details read LICENCE.TXT.");
   cprintf("\n\r\n\rPress 3 to start in 320x200 mode or any other key for 640x480.");
   video_mode = 1;

   init_config();
   init_sound();

   do
   {
    globg ++;
   } while (keypressed() == 0);
   switch(readkey() & 0xff)
   {
    case '3': video_mode = 0; break;
    case '8': video_mode = 2; break;
   }
   
   init_prand(globg);

   video_modes();
   // NOTE: 800x600 not tested as my monitor doesn't like set_gfx_mode

   set_color_depth(8);

   switch(video_mode)
   {
    case 0:
    if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) != 0)
    {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set 320x200 mode\n%s\n", allegro_error);
      exit(1);
    }
    break;
    case 1:
    if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0)
    {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set 640x480 mode\n%s\n", allegro_error);
      exit(1);
    }
    break;
/*    case 2:
    if (set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0) != 0)
    {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set 800x600 mode\n%s\n", allegro_error);
      exit(1);
    }
    break;*/
/*    case 3:
    if (set_gfx_mode(GFX_MODEX, 360, 360, 0, 0) != 0)
    {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set 360x360 mode\n%s\n", allegro_error);
      exit(1);
    }
    break;*/
   }


//   colour_table();

   text_mode(-1);


//   palet = &palette_data;
   //malloc(sizeof(RGB) * 256);
//   palet = datf[PALETTE_01].dat;
//   set_palette(palet);
//    colour_table();
//    colour_table();

   install_int (framecount, 1000);
   install_int (tickover, tick_length * 10);


   datf = load_datafile("proj.dat");

   if (datf == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file proj.dat");
      exit(1);
   }
   
   small_font = datf[FONT_02].dat;

   switch(video_mode)
   {
    case 0: large_font = datf[FONT_02].dat; break;
    default:
    case 1: large_font = datf[FONT_01].dat; break;

   }


   init_palette();
//    colour_table();
//   set_palette(palet);
/*
   int n,m;

   BITMAP *pal_bmp = create_bitmap(16, 16);

   for (m = 0; m < 16; m ++)
   {
    for (n = 0; n < 16; n ++)
    {
     if ((m * 16) + n <= 224)
      putpixel(pal_bmp, m, n, 0);
       else
        putpixel(pal_bmp, m, n, m * 16 + n);
    }
   }

   save_bitmap("palette.bmp", pal_bmp, palet);
*/
   RGB temp_palette [256];

   textprintf(screen, datf[FONT_01].dat, 10, 10, COLOUR_YELLOW4, "Loading...");

   soldier_file [SOLDIER_SOLDIER] = load_bitmap("soldier.bmp", temp_palette);
// soldier_file = create_bitmap(200, 700);//load_bitmap("soldier.bmp", temp_palette);

   if (soldier_file [SOLDIER_SOLDIER] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file soldier.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_WALKER] = load_bitmap("walker.bmp", temp_palette);
//   soldier_file = create_bitmap(200, 700);//load_bitmap("soldier.bmp", temp_palette);

   if (soldier_file [SOLDIER_WALKER] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file walker.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_SKELETON] = load_bitmap("skeleton.bmp", temp_palette);
//   soldier_file = create_bitmap(200, 700);//load_bitmap("soldier.bmp", temp_palette);

   if (soldier_file [SOLDIER_SKELETON] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file skeleton.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_CYBORG] = load_bitmap("cyborg.bmp", temp_palette);
//   soldier_file = create_bitmap(200, 700);//load_bitmap("soldier.bmp", temp_palette);

   if (soldier_file [SOLDIER_CYBORG] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file cyborg.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_SNAIL] = load_bitmap("snail.bmp", temp_palette);
//   soldier_file = create_bitmap(200, 700);//load_bitmap("soldier.bmp", temp_palette);

   if (soldier_file [SOLDIER_SNAIL] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file snail.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_CRUSHER] = load_bitmap("crusher.bmp", temp_palette);

   if (soldier_file [SOLDIER_CRUSHER] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file crusher.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_SCOUT] = load_bitmap("scout.bmp", temp_palette);

   if (soldier_file [SOLDIER_SCOUT] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file scout.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_DEMON] = load_bitmap("demon.bmp", temp_palette);
//   soldier_file = create_bitmap(200, 700);//load_bitmap("soldier.bmp", temp_palette);

   if (soldier_file [SOLDIER_DEMON] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file demon.bmp");
      exit(1);
   }

   soldier_file [SOLDIER_BUG] = load_bitmap("bug.bmp", temp_palette);
//   soldier_file = create_bitmap(200, 700);//load_bitmap("soldier.bmp", temp_palette);

   if (soldier_file [SOLDIER_BUG] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file demon.bmp");
      exit(1);
   }


//   clear_bitmap(soldier_file);
   
/*   actor_file [0] = load_bitmap("act0.bmp", temp_palette);

   if (actor_file [0] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act0.dat");
      exit(1);
   }
   
   actor_file [1] = load_bitmap("act1.bmp", temp_palette);

   if (actor_file [1] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act1.bmp");
      exit(1);
   }

   actor_file [2] = load_bitmap("act2.bmp", temp_palette);

   if (actor_file [2] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act2.bmp");
      exit(1);
   }

   actor_file [3] = load_bitmap("act3.bmp", temp_palette);

   if (actor_file [3] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act3.bmp");
      exit(1);
   }

   actor_file [4] = load_bitmap("act4.bmp", temp_palette);

   if (actor_file [4] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act4.bmp");
      exit(1);
   }

   actor_file [5] = load_bitmap("act5.bmp", temp_palette);

   if (actor_file [5] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act5.bmp");
      exit(1);
   }

   actor_file [6] = load_bitmap("act6.bmp", temp_palette);

   if (actor_file [6] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act6.bmp");
      exit(1);
   }

   actor_file [7] = load_bitmap("act7.bmp", temp_palette);

   if (actor_file [7] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act7.bmp");
      exit(1);
   }

   actor_file [8] = load_bitmap("act8.bmp", temp_palette);

   if (actor_file [8] == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file act8.bmp");
      exit(1);
   }
  */
   int bi, bx, bz, bj, got_pixel;
//   DATAFILE *afile;

   for (bx = 0; bx < NO_SOLDIER_FILES; bx ++)
   {
//    if (bx == 9) continue;
//    afile = actor_file [bx];
//    for (bi = 0; bi < (18 * 27 + 1); bi ++)
    if (soldier_file [bx] == NULL) continue;
    for (bi = 0; bi < (18 * 30 + 1); bi ++)
    {
     for (bz = 0; bz < 18 * 9 + 1; bz ++)
     {
//      for (bj = 0; bj < 17; bj ++)
//      {
       got_pixel = getpixel(soldier_file [bx], bi, bz);
       if (got_pixel <= 224) putpixel(soldier_file [bx], bi, bz, 0);
//      }
     }
    }
   }

/*   set_palette(white_palette);
//   wh_pal = white_palette;
   RGB *wcolour [2];
   wcolour [0]->r = 0;
   wcolour [0]->g = 0;
   wcolour [0]->b = 0;
//   set_color(0, wcolour [0]);
   wcolour [0]->r = 64;
   wcolour [0]->g = 64;
   wcolour [0]->b = 64;
   int i;
   for (i = 0; i < 256; i ++)
   {
    set_color(i, wcolour [0]);
   }*/

//   set_palette(datf[PALETTE_01].dat);

//   colour_table();

/*
   RGB temp_palette2 [256];
   RGB *tp2 = temp_palette2;

   for (bi = 0; bi < 256; bi ++)
   {
    if (bi < 224)
    {
     tp2 [bi].r = 0;
     tp2 [bi].g = 0;
     tp2 [bi].b = 0;
    }
     else
     {
      tp2 [bi].r = palet [bi].r;
      tp2 [bi].g = palet [bi].g;
      tp2 [bi].b = palet [bi].b;
     }
   }

   set_palette(tp2);
*/
   BITMAP *temp_bmp = load_bitmap("bricks.bmp", temp_palette);

   if (temp_bmp == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file bricks.bmp");
      exit(1);
   }

   for (bi = 0; bi < 2 * (BRICK_X + 1) + 1; bi ++)
   {
    for (bx = 0; bx < NO_BRICKS * 5 * (BRICK_Y + 1) + 1; bx ++)
    {
     if (getpixel(temp_bmp, bi, bx) < 224)
      putpixel(temp_bmp, bi, bx, 224);
    }
   }


   for (bi = 0; bi < 20; bi ++)
   {
    for (bx = 0; bx < 5; bx ++)
    {
     brick_bmp [bi] [bx] = create_bitmap(BRICK_X, BRICK_Y);
     blit(temp_bmp, brick_bmp [bi] [bx], 1, bi * (BRICK_Y + 1) + 1, 0, 0, BRICK_X, BRICK_Y);
    }
   }

   for (bi = 0; bi < 20; bi ++) // which brick
   {
    for (bx = 0; bx < 5; bx ++) // light level
    {
//     for (bz = 0; bz < BRICK_X; bz ++)
//     {
//      for (bj = 0; bj < BRICK_Y; bj ++)
//      {
         drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
         rectfill(brick_bmp [bi] [bx], 0, 0, BRICK_X, BRICK_Y, FORCE_LIGHT_1 + bx);
         drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

//      }
//     }
    }
   }

   destroy_bitmap(temp_bmp);
   
//   set_palette(palet);

/*   brick_bmp [0] = datf[BRICK_RBRICK1].dat;
   brick_bmp [1] = datf[BRICK_RBRICK2].dat;
   brick_bmp [2] = datf[BRICK_SSTONE1].dat;
   brick_bmp [3] = datf[BRICK_SSTONE2].dat;
   brick_bmp [4] = datf[BRICK_SSTONE_BACK].dat;
/ *   brick_bmp [5] = datf[BRICK_ROCK4].dat;
   brick_bmp [6] = datf[BRICK_ROCKBACK].dat;
   brick_bmp [7] = datf[BRICK_DIRT].dat;
   brick_bmp [8] = datf[BRICK_DIRTG].dat;
   brick_bmp [9] = datf[BRICK_DIRTBACK].dat;*/
   
   captain_pork = datf[CAPTAIN_PORK].dat;
     for (bz = 0; bz < 398; bz ++)
     {
      for (bj = 0; bj < 64; bj ++)
      {
       got_pixel = getpixel(captain_pork, bz, bj);
       if (got_pixel == 224) putpixel(captain_pork, bz, bj, 0);
      }
     }
   game_over_sign = datf[GAME_OVER_SIGN].dat;
     for (bz = 0; bz < 80; bz ++)
     {
      for (bj = 0; bj < 80; bj ++)
      {
       got_pixel = getpixel(game_over_sign, bz, bj);
       if (got_pixel == 224) putpixel(game_over_sign, bz, bj, 0);
      }
     }
   you_win_sign = datf[YOU_WIN_SIGN].dat;
     for (bz = 0; bz < 80; bz ++)
     {
      for (bj = 0; bj < 80; bj ++)
      {
       got_pixel = getpixel(you_win_sign, bz, bj);
       if (got_pixel == 224) putpixel(you_win_sign, bz, bj, 0);
      }
     }


   init_bases();
   init_actor_struct();

   init_players();
   init_users1();

//   set_palette(palet);
   
   init_object_sprites();



//   arena [0].dirt = 1;
   arena [0].shake_x = 0;
   arena [0].shake_y = 0;
   arena [0].shake_time = 0;
   arena [0].nuke_flash = 0;
   arena [0].generate_level = 1;

   arena [0].teams = 0;

   int i;

   for (i = 0; i < NO_TEAMS; i ++)
   {
    team[i].ranked = 0;
    team[i].active = 0;
    team[i].size = 0;
    team[i].score = 0;
    team[i].lives = 0;
   }
   
//   colour_table();


}












void init_everything_again1(void)
{

   int i;

   for (i = 0; i < NO_TEAMS; i ++)
   {
    team[i].ranked = 0;
    team[i].active = 0;
    team[i].size = 0;
    team[i].score = 0;
    team[i].lives = 0;
    team[i].base_counter = 0;
   }
   

   init_actor_struct();

   init_players();
   init_users1();

   init_bullets();
   init_clouds();

   slacktime = 0;

   init_cloud_pattern();
    // actually inits several display things


/*   for (i = 0; i < NO_TEAMS; i ++)
   {
    team[i].ranked = 0;
//    team[i].active = 0;
    team[i].size = 0;
    team[i].score = 0;
    team[i].lives = 0;
   }*/
   

}





void init_everything_again2(void)
{

   init_grid();

   init_pickups();
   init_bases();
   if (serial[0].game_type != SERIAL_CLIENT)
    prepare_bases();



/*     if (bmp_p1 != NULL)
      destroy_bitmap(bmp_p1);
     if (bmp_p2 != NULL)
      destroy_bitmap(bmp_p2);*/

if (no_players == 1)
{
     if (bmp_p1 == NULL)
      bmp_p1 = create_bitmap(window_x_sp, window_y);
     clear_bitmap(bmp_p1);
//     BITMAP *bmp_p2 = create_bitmap(5, 5);
//   bmp_p2 is never used.
}



if (no_players == 2)
{
     if (bmp_p1 == NULL)
      bmp_p1 = create_bitmap(window_x_2p, window_y);
     clear_bitmap(bmp_p1);
     if (bmp_p2 == NULL)
      bmp_p2 = create_bitmap(window_x_2p, window_y);
     clear_bitmap(bmp_p2);
}



if (no_players == 1)
   init_stat_display(player[1].actor_controlled, stat_x_pos, stat_y_pos, window_x_sp, stat_bar_width, stat_pix_per_point_1p);
      else
      {
         init_stat_display(player[1].actor_controlled, stat_x_pos, stat_y_pos, window_x_2p, stat_bar_width, stat_pix_per_point_2p);
         init_stat_display(player[2].actor_controlled, stat_x_pos + window_grid_x_2p, stat_y_pos, window_x_2p, stat_bar_width, stat_pix_per_point_2p);

      }
display_player_ammunition(1);
if (no_players == 2) display_player_ammunition(2);
display_player_clips(1);
if (no_players == 2) display_player_clips(2);
display_player_health(1, 0);
if (no_players == 2) display_player_health(2, 0);

update_score();

/*   for (i = 0; i < NO_ACTORS; i ++)
   {
    if (actor[i].aclass != ACLASS_NONE)
     spawn_actor(i);
//     place_actor(i);
   }*/
   
turn_init();


 arena[0].pk_counter = 1;
 arena[0].shake_time = 0;
 arena[0].shake_x = 0;
 arena[0].shake_y = 0;
 arena[0].counter = 0;
 arena[0].ingame_slots = NO_SOLDIER_FILES;
 if (game[0].atypes_avail == 0)
  arena[0].ingame_slots = 1;
 if (game[0].atypes_avail == 1)
  arena[0].ingame_slots = 3;
// if (game[0].atypes_avail == 1)
//  arena[0].ingame_slots = 2;
 arena[0].dirt_storm = 0;
 arena[0].dirt_count = 2000;
 arena[0].has_bases = 0;
 arena[0].who_has_porkball = -1;

 arena[0].dirt_possible = 0;
 if (arena[0].dirt_fill != 0) arena[0].dirt_possible = 1;
 if (arena[0].replace_dirt != 0) arena[0].dirt_possible = 1;
 if (game[0].weapon_status [WPN_DIRTBOMB] != 0) arena[0].dirt_possible = 1;
 if (game[0].weapon_status [WPN_CLOD_OF_DIRT] != 0) arena[0].dirt_possible = 1;
 // if I wanted to be really thorough, dirt_possible could be set to zero if
 //  dirt weapons are available at the start (not in boxes) but because
 //  nobody chose them they can't exist, and there're no AIs with the full
 //  AI weapons cheat. But that'd be too much trouble.

 int i;
  
 arena[0].need_actor_lit = 0;
 if (arena[0].ambient_light > 0)
  arena[0].need_actor_lit = 0;
   else
    {
     for (i = 0; i < MAX_AI; i ++)
     {
       if (ai_config[i].active != 0)
        arena[0].need_actor_lit = 1;
     }
    }

 switch(game[0].game_type)
 {
  case GAME_CAP_FLAG:
  case GAME_GRAIL:
  case GAME_PORKBALL:
  case GAME_TAKEHOLD: 
  arena[0].teams = 1;
  arena[0].has_bases = 1; break;
 }

 game_over = 0;


}











void run_game(void)
{

   if (serial[0].game_type == SERIAL_SERVER ||
        serial[0].game_type == SERIAL_CLIENT)
          synch_gamestart();
#ifdef DEBUG_KEYS
   static int scrshnum = 0;
   static int scount = 0;
   char itstring [10];
#endif
          
   ticked = 0;
   tick_counter = 0;
   do
   {
#ifdef DEBUG_KEYS
if (key [KEY_C]) colour_table("Hello");
if (key [KEY_G] && arena[0].counter % 5 == 0)
 score_event(player[1].actor_controlled, SCORE_KILL, 1);
//   set_palette(datf[PALETTE_01].dat);
if (key [KEY_P] && scount == 0)
{
 BITMAP *scrshot = create_bitmap(640, 480);
 blit(screen, scrshot, 0, 0, 0, 0, 640, 480);
 char scr_name [20];
 strcpy(scr_name, "scrs\\scrs");
 strcat(scr_name, itoa(scrshnum, itstring, 10));
 strcat(scr_name, ".bmp");
 RGB palt [256];
 get_palette(palt);
 save_bitmap(scr_name, scrshot, palt);
 scrshnum++;
 destroy_bitmap(scrshot);
 rectfill(screen, 100, 100, 300, 200, 15);
 scount = 35;
}

if (scount > 0) scount --;
#endif
   
//      get_keypress(); // called as often as convenient



      tick_status ++;
      if ((tick_status % frame_length == 0 && ticked == 0) || arena[0].counter % 30 == 0)
      {
       text_mode(0);
//       textprintf(screen, small_font, 100, 70, COLOUR_YELLOW1 + arena[0].counter % 4, "(%ims slack)", slacktime);
       display_windows(bmp_p1, bmp_p2);
       framecounter++;
      }
      slacktime = 0;

      if (arena[0].counter == 255) arena[0].counter = 0;
       else
        arena[0].counter ++;

         turn_light();
         run_bullets();
         run_clouds();
         run_pickups();
         switch(game[0].game_type)
         {
          case GAME_FRUIT_HUNT:
           if (arena[0].counter % 100 == 0) run_fruit();
           break;
          case GAME_TAKEHOLD:
           run_th_bases();
           break;
         }
         run_effects();
         if (serial[0].game_type != SERIAL_LOCAL_ONLY)
          run_async();
         enact_commands();
         run_actors();
         turncounter ++;
// NOTE: problems may be caused by later location of enact_commands!!!!!!
      while (ticked == 0)
      {
//       rest(1);
       get_keypress();
       slacktime ++;
      }
       get_keypress();
      ticked --;

      if (tick_status == turn_length - 1 || turn_length == 1)
      {
         clear_cmd_buffer();
         get_movement(); // issues commands. Server shld receive info
         // before this call, and send it afterwards.
//         if (serial[0].game_type != SERIAL_LOCAL_ONLY)
//          run_async();
         send_out_commands();
         turn_init();
         inputcounter++;
/*         while (tick_status == turn_length - 1)
         {
          int waste_time = 0;
         }*/
         tick_status = 0;
      }

      if (key [KEY_ESC] != 0)
      {
//       end_challenge();
       if (serial[0].game_type == SERIAL_SERVER)
       {
        async_end_game(1);
        game_over = 200;
       }
        else
        {
         if (serial[0].game_type == SERIAL_CLIENT)
         {
          async_end_game(1);
          game_over = 200;
         }
          else
          {
           end_game(45);
           game_over = 1;
           break;
          }
        }
      }

      if (game_over > 0) game_over --;

      if (game_over == 1)
      {
       end_game(1);
       game_over = 0;
       break;
      }

   } while (TRUE);



}










void turn_init(void)
{

int i;
//int j;

for (i = 0; i < NO_CMDS; i ++)
{
 keypress [i] = 0;
}

for (i = 0; i < NO_ACTORS; i++)
{
   if (actor [i].aclass != ACLASS_PLAYER) continue;
//   if (actor[i].leg_action != LA_CLIMB)
    actor[i].leg_action = LA_STAND;
//     else actor[i].y_speed = 0;
}


}




void init_grid(void)
{

 textprintf(screen, large_font, 10, 10, COLOUR_YELLOW4, "Please wait, I'm thinking of a level for you...");

// arena[0].level_type = LEVELTYPE_BRICKS;

 if (dirt_bmp != NULL)
  destroy_bitmap(dirt_bmp);
 if (shadow_bmp != NULL)
  destroy_bitmap(shadow_bmp);
// if (arena[0].light != NULL)
//  destroy_bitmap(arena[0].light);
// if (arena[0].base_light != NULL)
//  destroy_bitmap(arena[0].base_light);
// if (arena[0].bricks != NULL)
//  destroy_bitmap(arena[0].bricks);
   if (arena[0].bricks != NULL)
    destroy_bitmap(arena[0].bricks);
//   if (arena[0].light != NULL)
//    destroy_bitmap(arena[0].light);
//   if (arena[0].base_light != NULL)
//    destroy_bitmap(arena[0].base_light);

 dirt_bmp = create_bitmap(arena[0].max_x, arena[0].max_y);
 shadow_bmp = create_bitmap(arena[0].max_x, arena[0].max_y);

 if (dirt_bmp == NULL || shadow_bmp == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create dirt bitmap.\n%s\n", allegro_error);
      exit(1);
 }

 clear_bitmap(dirt_bmp);
 clear_bitmap(shadow_bmp);

  arena[0].bricks_x = arena[0].max_x / BRICK_X + 1;
  arena[0].bricks_y = arena[0].max_y / BRICK_Y + 1;
  arena[0].bricks = create_bitmap(arena[0].bricks_x, arena[0].bricks_y);


 generate_level();

 init_lightmap();

}




void brick_init_grid(void)
{

int gloop_x, gloop_y;

for (gloop_x = 0; gloop_x < GRID_X; gloop_x ++)
{
 for (gloop_y = 0; gloop_y < GRID_Y; gloop_y ++)
 {
  set_grid(gloop_x, gloop_y, GRID_SSTONE_BACK);
  set_underlying_grid(gloop_x, gloop_y, GRID_SSTONE_BACK);
  set_grid_strength(gloop_x, gloop_y, 100);
 }
}

for (gloop_x = 0; gloop_x < GRID_X; gloop_x ++)
{
 for (gloop_y = 0; gloop_y < GRID_Y; gloop_y ++)
 {
   if (random() % 40 == 0)
   {
    set_grid(gloop_x, gloop_y, GRID_SSTONE1);// + random() % 4;
//    if (get_grid(gloop_x, gloop_y - 1) != 0)
//     set_grid(gloop_x, gloop_y, GRID_DIRT);// + random() % 4;


//    if (get_specific_grid(gloop_x, gloop_y + 1) == GRID_DIRTG)
//     set_grid(gloop_x, gloop_y + 1, GRID_DIRT);// + random() % 4;

    set_underlying_grid(gloop_x, gloop_y, GRID_SSTONE1);
    set_grid_strength(gloop_x, gloop_y, 100);

   }
      else
      {
       set_grid(gloop_x, gloop_y, GRID_SSTONE_BACK);
       set_underlying_grid(gloop_x, gloop_y, GRID_SSTONE_BACK);
       set_grid_strength(gloop_x, gloop_y, 0);
      }
   if (gloop_x >= arena[0].max_x || gloop_y >= arena[0].max_y)
   {
     set_grid(gloop_x, gloop_y, GRID_REDBRICK1);
     set_underlying_grid(gloop_x, gloop_y, GRID_REDBRICK1);
     if ((gloop_x + gloop_y) % 2 == 0)
     {
      set_grid(gloop_x, gloop_y, GRID_REDBRICK2);
      set_underlying_grid(gloop_x, gloop_y, GRID_REDBRICK2);
     }
     set_grid_strength(gloop_x, gloop_y, 100);
   }
 }
}

}





/*
void run_players(void)
{

int i;

for (i = 0; i < no_players; i++)
{
 if (actor[player[i].actor_controlled].time_since_hurt == 0)
    display_player_health(i, );
}

void display_player_health(int dplayer)


}*/


void run_actors(void)
{

int i;

for (i = 0; i < NO_ACTORS; i++)
{
   if (actor [i].aclass == ACLASS_GHOST)
   {
    if (user[actor[i].user].penalty_time > 0) user[actor[i].user].penalty_time --;
     else
      if (user[actor[i].user].resting_time > 0) user[actor[i].user].resting_time --;
     continue;
   }

   if (actor [i].aclass == ACLASS_NONE) continue;
// firing_laser is reset in enact_commands, cmd.h
   if (actor[i].aclass != ACLASS_PORKBALL || arena[0].who_has_porkball == -1)
    move_actor(i);
//   actor[i].jetting = 0;
//   actor[i].rocketing = 0;
   if (arena[0].need_actor_lit == 1)
   {
    if (is_actor_lit(i))
     actor[i].lit = 10;
    if (actor[i].lit > 0)
      actor[i].lit --;
   }

   
   if (actor[i].recycle > 0) actor[i].recycle --;
   if (actor[i].jump_delay > 0) actor[i].jump_delay --;
   if (actor[i].dip > 0) actor[i].dip --;
   if (actor[i].has_porkball != -1) actor_porkball(i);
   if (actor[i].aclass == ACLASS_PORKBALL) run_porkball(i);
   if (actor[i].jet_heat > 0)
   {
    actor[i].jet_heat -= actor[i].jet_speed * 2;
    if (actor[i].jet_heat < 0) actor[i].jet_heat = 0;
   }
   if (actor[i].rocket_charge > 0)
    actor[i].rocket_charge --;
   if (actor[i].jet_spin > actor[i].jet_power / 2)
    actor[i].jet_spin -= 3;
   if (actor[i].jet_spin < actor[i].jet_power / 2)
    actor[i].jet_spin = actor[i].jet_power / 2;

   if (actor[i].uncloaked > 0) actor[i].uncloaked --;
   if (actor[i].reload [actor[i].current_weapon] > 0)
   {
    actor[i].reload [actor[i].current_weapon] --;
    display_actor_ammunition(i);
    if (actor[i].reload [actor[i].current_weapon] == 0) play_sound(WAV_RELOAD);
   }
   if (actor[i].grapple_recycle > 0) actor[i].grapple_recycle --;
   actor [i].time_since_hurt ++;
   if (player[1].actor_controlled == i && actor [i].time_since_hurt >= 50)
      reset_health_bar(1);
   if (player[1].show_names > 1)
      player[1].show_names --;
   if (player[1].show_names < 0)
      player[1].show_names ++;
   if (player[2].actor_controlled == i && actor [i].time_since_hurt >= 50)
      reset_health_bar(2);
   if (player[2].show_names > 1)
      player[2].show_names --;
   if (player[2].show_names < 0)
      player[2].show_names ++;

   if (actor[i].shield_visible > 0)
    actor[i].shield_visible --;
   if (actor[i].shield && actor[i].shield_visible < 20 && !prand(50))
   {
    actor[i].shield_visible += prand(10);
   }
   if (actor[i].armour_pulse > 0)
   {
    if (arena[0].counter % 3 == 0)
     actor[i].armour_pulse --;
    place_light(actor[i].x, actor[i].y, actor[i].armour_pulse * 3);
   }
   if (actor[i].regenerator > 0
       && arena[0].counter % 2 == 0
       && serial[0].game_type != SERIAL_CLIENT
       && actor[i].health < actor[i].max_health)
   {
    actor[i].health += 1;
    display_actor_health(i, 0);
   }
   if (actor[i].shadow > 0)
   {
    place_dark(actor[i].x, actor[i].y, 5);
   }
   if (actor[i].spotlight > 0)
   {
    place_light(actor[i].x, actor[i].y, 50);
    place_light(actor[i].laser_x, actor[i].laser_y, 70);
    actor_tracer(i);
   }
   if (actor[i].armour && actor[i].armour_pulse == 0 && !prand(50))
   {
    actor[i].armour_pulse = actor[i].circle_radius + 1;
   }
   if (arena[0].has_bases)
   {
    near_base(i);
   }
   
}


}


int is_actor_lit(int i)
{
// if (arena[0].need_actor_lit == 0)
//  return 1;
 // don't need to worry about it, as there are no ais or there is ambient
 //  light.

// return 0;

 if (actor[i].spotlight != 0)
  return 1;

 if (actor[i].shadow != 0
     && arena[0].ambient_light != 4)
 {
  if ((i + arena[0].counter) % 3 == 0)
   return 0; // icky
 }
 if (arena[0].ambient_light > 0)
  return 1;
  
 int thing = getpixel(dirt_bmp, actor[i].x / GRAIN, actor[i].y / GRAIN);
  
 if (thing < 96 || thing > 128)
     {
      return 1;
     }

 return 0;
 
}



void colour_table(const char *which_call)
{
   text_mode(0);

   int bz;
   
   for (bz = 0; bz < 256; bz ++)
   {
    textprintf(screen, small_font, (bz % 16) * 50, (bz / 16) * 20, bz, "%i.%i.%i.%i", bz, palet[bz].r, palet[bz].g, palet[bz].b);
//    textprintf(screen, font, (bz % 16) * 20, (bz / 16) * 20, bz, "%i", bz);
   }

   text_mode(5);
   textprintf(screen, large_font, 300, 200, 254, which_call);


   rest(5);
   
   do
   {
    bz ++;
   } while (keypressed() == 0);
   readkey();
}


void init_config(void)
{

serial[0].game_type = SERIAL_LOCAL_ONLY;
serial[0].com_port = 1;
serial[0].who_decides_damage = DAM_SERVER;

serial[0].connection_type = get_config_int("Network", "conection_type", NET_DRIVER_SERIAL_DOS);
strcpy(serial[0].port_string, get_config_string("Network", "port_string", ""));
serial[0].com_port = get_config_int("Network", "com_port", 1);

options[0].run_vsync = get_config_int("Options", "run_vsync", 1);
options[0].fuzzy_menu = get_config_int("Options", "fuzzy_menu", 1);
options[0].stipple_clouds = get_config_int("Options", "stipple_clouds", 1);
sound_active = get_config_int("Options", "Sound", 0);
options[0].show_fps = get_config_int("Options", "show_fps", 0);
options[0].positional_sound = get_config_int("Options", "positional_sound", 1);
options[0].display_damage = get_config_int("Options", "display_damage", 0);

strcpy(player[1].pname, get_config_string("Player1", "Name", "Player 1"));
player[1].colour1 = get_config_int("Player1", "Colour1", 50);
player[1].colour2 = get_config_int("Player1", "Colour2", 0);
//player[1].colour_b = get_config_int("Player1", "Blue", 30);
player[1].handicap = get_config_int("Player1", "Handicap", 100);
player[1].team = get_config_int("Player1", "Team", 1);
player[1].soldier = get_config_int("Player1", "Soldier", 0);
strcpy(player[2].pname, get_config_string("Player2", "Name", "Player 2"));
player[2].colour1 = get_config_int("Player2", "Colour1", 50);
player[2].colour2 = get_config_int("Player2", "Colour2", 0);
//player[2].colour_b = get_config_int("Player2", "Blue", 30);
player[2].handicap = get_config_int("Player2", "Handicap", 100);
player[2].team = get_config_int("Player2", "Team", 2);
player[2].soldier = get_config_int("Player2", "Soldier", 0);

/*player [1].weapon [0] = WPN_NONE;
player [1].weapon [1] = WPN_NONE;
player [1].weapon [2] = WPN_NONE;
player [1].weapon [3] = WPN_NONE;
player [2].weapon [0] = WPN_NONE;
player [2].weapon [1] = WPN_NONE;
player [2].weapon [2] = WPN_NONE;
player [2].weapon [3] = WPN_NONE;
*/
  int ci, wcount;
  char miscstring [20];
  char wstring [20];
  char itstring [20];

for (ci = 0; ci < NO_GAMES; ci ++)
{
  strcpy(miscstring, "Game");
  strcat(miscstring, itoa(ci, itstring, 10));
  strcpy(game[ci].gname, get_config_string(miscstring, "Name", "Unknown"));
  game[ci].health_amount = get_config_int(miscstring, "health_amount", 100);
  game[ci].reload_time = get_config_int(miscstring, "reload_time", 10);
  game[ci].gravity = get_config_int(miscstring, "gravity", 30);
  game[ci].impact_damage = get_config_int(miscstring, "impact_damage", 0);
  game[ci].bullet_speed = get_config_int(miscstring, "bullet_speed", 10);
  game[ci].game_type = get_config_int(miscstring, "game_type", 0);
  game[ci].score_type = get_config_int(miscstring, "score_type", 0);
  game[ci].score_limit = get_config_int(miscstring, "score_limit", 0);
  game[ci].lives_type = get_config_int(miscstring, "lives_type", 0);
  game[ci].lives_each = get_config_int(miscstring, "lives_each", 10);
//  game[ci].max_x = get_config_int(miscstring, "max_x", 20);
//  game[ci].max_y = get_config_int(miscstring, "max_y", 20);
  game[ci].pk_number = get_config_int(miscstring, "pk_number", 5);
  game[ci].fruit_no = get_config_int(miscstring, "fruit_no", 5);
  game[ci].penalty = get_config_int(miscstring, "penalty", 0);
  game[ci].th_base_no = get_config_int(miscstring, "th_base_no", 1);
  game[ci].pk_time_between = get_config_int(miscstring, "pk_time_between", 100);
  game[ci].lightsourcing = get_config_int(miscstring, "lightsourcing", 1);
  game[ci].pk_health = get_config_int(miscstring, "pk_health", 0);
  game[ci].pk_weapon = get_config_int(miscstring, "pk_weapon", 0);
  game[ci].pk_equip = get_config_int(miscstring, "pk_equip", 0);
  game[ci].name_boxes = get_config_int(miscstring, "pk_named", 1);
  game[ci].starting_weapons = get_config_int(miscstring, "starting_weapons", 0);
  game[ci].fast_bullets = get_config_int(miscstring, "fast_bullets", 0);
  game[ci].bullets_explode = get_config_int(miscstring, "bullets_explode", 0);
  game[ci].blast_bullets = get_config_int(miscstring, "blast_bullets", 0);
  game[ci].show_map = get_config_int(miscstring, "show_map", 0);
  game[ci].soft_dirt = get_config_int(miscstring, "soft_dirt", 0);
  game[ci].rechoose_atype = get_config_int(miscstring, "rechoose_atype", 0);
  game[ci].unlimited_clips = get_config_int(miscstring, "unlimited_clips", 0);
  game[ci].reset_weapons = get_config_int(miscstring, "reset_weapons", 0);
  game[ci].atypes_avail = get_config_int(miscstring, "atypes_avail", 2);

  
  for (wcount = 0; wcount < NO_WEAPONS; wcount ++)
  {
   strcpy(wstring, "Wpn");
   strcat(wstring, itoa(wcount, itstring, 10));
   game[ci].weapon_status [wcount] = get_config_int(miscstring, wstring, 0);
  }

  for (wcount = 0; wcount < NO_EQUIP; wcount ++)
  {
   strcpy(wstring, "Eqp");
   strcat(wstring, itoa(wcount, itstring, 10));
   game[ci].equip_status [wcount] = get_config_int(miscstring, wstring, 0);
  }

  
}

for (ci = 0; ci < MAX_AI; ci ++)
{
  strcpy(miscstring, "AI-");
  strcat(miscstring, itoa(ci, itstring, 10));
  strcpy(ai_config[ci].name, get_config_string(miscstring, "Name", ""));
  ai_config[ci].active = get_config_int(miscstring, "Active", 0);
  ai_config[ci].skill = get_config_int(miscstring, "Skill", 0);
  ai_config[ci].team = get_config_int(miscstring, "Team", 1);
  ai_config[ci].atype = get_config_int(miscstring, "Type", 0);
  ai_config[ci].handicap = get_config_int(miscstring, "Handicap", 100);
  ai_config[ci].soldier = get_config_int(miscstring, "Soldier", 0);
}

arena[0].ai_no_impact = get_config_int("AI-General", "Impact", 1);
arena[0].ai_unlimited_clips = get_config_int("AI-General", "Clips", 1);
arena[0].ai_weapons = get_config_int("AI-General", "Weapons", 1);

arena[0].level_colours = get_config_int("Level", "Colours", COLOURS_CLASSIC);
arena[0].level_style = get_config_int("Level", "Style", LEVEL_CLASSIC);
arena[0].solid_density = get_config_int("Level", "Solids", 3);
arena[0].dirt_fill = get_config_int("Level", "Dirty", 1);
arena[0].ambient_light = get_config_int("Level", "Ambience", 1);
arena[0].lamp_amount = get_config_int("Level", "Lamps", 4);
arena[0].replace_dirt = get_config_int("Level", "Redirt", 0);
arena[0].max_x = get_config_int("Level", "Max_x", 500);
arena[0].max_y = get_config_int("Level", "Max_y", 500);

arena[0].quickstart_difficulty = get_config_int("Quickstart", "quickstart_difficulty", 2);
arena[0].quickstart_dirt = get_config_int("Quickstart", "quickstart_dirt", 2);
arena[0].quickstart_lightsourced = get_config_int("Quickstart", "quickstart_lightsourced", 1);
arena[0].quickstart_weapons = get_config_int("Quickstart", "quickstart_weapons", 1);


strcpy(arena[0].level_loaded, get_config_string("Level", "Loaded", ""));

}

