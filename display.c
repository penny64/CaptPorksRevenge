
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

File: display.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions displaying stuff on the screen while the game is going
 - functions setting up the game screen
 - message functions
 
*/

#include "allegro.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "globvar.h"
#include "cloud.h"
#include "prand.h"
#include "grid.h"
#include "disp2.h"
#include "bullet.h"

#include "grabhead.h"
#include "objhead.h"
#include "pointhd.h"
#include "base.h"

#include "light.h"
#include "palette.h"

#include "acthead.h"

// only need this while using colour macros
//#include <conio.h>

#define BLOOD_RED COLOUR_RED3
// also defined in bullet.c

#ifndef PI
#define PI 3.14159265358979323846
#endif

extern BITMAP *game_over_sign;
extern BITMAP *you_win_sign;
extern int game_over;
extern int video_mode;

char* itoa(int value, char* str, int radix) {
    static char dig[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";
    int n = 0, neg = 0;
    unsigned int v;
    char* p, *q;
    char c;

    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }
    v = value;
    do {
        str[n++] = dig[v%radix];
        v /= radix;
    } while (v);
    if (neg)
        str[n++] = '-';
    str[n] = '\0';

    for (p = str, q = p + (n-1); p < q; ++p, --q)
        c = *p, *p = *q, *q = c;
    return str;
}

// in move.c:
char actor_grounded(int gactor, char wide);


void grid_to_bitmap(int x_pos, int y_pos, int bm_width, int bm_height,
     BITMAP *g_to_bmp);
     
void brick_grid_to_bitmap(int x_pos, int y_pos, int bm_width, int bm_height,
     BITMAP *g_to_bmp);
     
int grid_ref_x(int position);
int grid_ref_y(int position);
void display_actor(int dactor, int x_centre, int y_centre, int x_limit,
     int y_limit, int window_width, BITMAP *bmp, char display_names);
void draw_actor_sprite(int dactor, int flip, BITMAP *sprite,
     BITMAP *bmp, int put_x, int put_y, int effect);
     
void display_windows(BITMAP *bmp_p1, BITMAP *bmp_p2);
void display_all_bullets(int x_centre, int y_centre, int x_limit, int y_limit,
     int window_width, BITMAP *bmp);
     
void display_bullet(int dbull, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);

void display_cloud(int dcloud, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);
void display_spawn_cloud(BITMAP *bmp, int dcloud, int put_x, int put_y);
void draw_pentagram(BITMAP *bmp, int put_x, int put_y, int dcloud, int col);

void display_all_clouds(int x_centre, int y_centre, int x_limit, int y_limit,
     int window_width, BITMAP *bmp);

void display_cords(int dactor, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);
void draw_grapple(BITMAP *bmp, int gbull, int put_x, int put_y);

void display_beams(int dactor, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);
     
void display_crosshair(int dactor, int x_centre, int y_centre, int x_limit,
     int y_limit, BITMAP *bmp);

void display_all_pickups(int x_centre, int y_centre, int x_limit, int y_limit,
     int window_width, BITMAP *bmp);
     
void display_pickup(int dpick, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);

void display_weapon_name(int dplayer, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);

void display_word(char *dword, int x_pos, int y_pos, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, int colour, BITMAP *bmp, char force_display);

void display_map(BITMAP *bmp, int map_x, int dactor);

void update_score(void);

void soft_shield_effect(int dactor, BITMAP *bmp, int put_x, int put_y, int magnitude);
void shield_effect(int dactor, BITMAP *bmp, int put_x, int put_y, int magnitude);
void distortion_mask(BITMAP *mask, BITMAP *bmp, int x, int y, int width, int height, int disp_x, int disp_y, int flip_x, int flip_y);


int animate_legs(int dactor);

void display_health(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int harmed);
void display_player_health(int dplayer, int harmed);
void distort_circle(BITMAP *bmp, int put_x, int put_y, int radius, int magnitude);

//void display_ammunition(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int harmed);

void display_actor_ammunition(int dactor);
void display_player_ammunition(int dplayer);
void display_ammunition(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int slot);

void display_actor_clips(int dactor);
void display_player_clips(int dplayer);
void display_clips(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int slot);

void display_player_armour(int dplayer);
void display_armour(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point);

void display_messages(BITMAP *bmp, int playerm);

//void init_stat_display(BITMAP *bmp, int x_pos, int y_pos, int window_width, char players, int bar_width, int pix_per_point);
void init_stat_display(int dactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point);
int health_proportion(int hlth, int max_hlth);

//void display_stats(int sactor, BITMAP *bmp, int x_pos, int y_pos, int window_width);
void ingame_menu(int mplayer, BITMAP *bmp, int centre_x);
void ingame_scorelist(int mplayer, BITMAP *bmp, int centre_x);

void apply_light_to_sprite(BITMAP *bmp, int x, int y, int lit);
void display_bricks(int x_pos, int y_pos, int bm_width, int bm_height, BITMAP *g_to_bmp);


char cloud_pattern_status = 0;

int brick_colour(int bricky, int lit);
//void cast_light(int lx, int lx, int lrad, int lit);
void cast_light(BITMAP *bmp, int lx, int ly, int lrad, int lit);

void display_all_lights(int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);
void display_a_light(int dlight, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp);
     
int cube_outer(int input);
int is_point_lit(BITMAP *bmp, int px, int py);
void display_game_writing(BITMAP *bmp, int which_writing, int centre_x, int centre_y, int distance);
int pulse_colour(int colour, int speed);


#define NO_LIGHTS 100

struct light_struct
{
 int x;
 int y;
 int level;
 int size;
 char light_or_dark;
};

extern struct light_struct light [NO_LIGHTS];



extern int window_x_sp, window_y, window_x_2p, window_grid_x_sp, window_grid_y;
extern int window_grid_x_2p, window_centre_x_sp, window_centre_x_2p;
extern int window_2p_offset, window_centre_y, crosshair_distance;
extern int stat_x_pos, stat_y_pos, stat_bar_width, stat_pix_per_point_1p;
extern int stat_pix_per_point_2p;
extern int score_sp_x_pos, score_2p_x_pos, score_y_pos;
extern int window_sp_offset, text_size;

extern int map_sp_x, map_2p_x, map_y, map_width, map_height;


extern volatile int frames_per_second;
extern volatile int inputs_per_second;
extern volatile int turns_per_second;

extern int slacktime;

extern BITMAP *brick_bmp [5] [5];

extern BITMAP *actor_bmp [NO_ACTORS] [NO_ACTOR_BITMAPS] [2] [2];
extern BITMAP *dirt_bmp;

BITMAP *cloud_pattern;
BITMAP *shield_circle [20];
BITMAP *disrupter_circle;
BITMAP *disrupter_circle2;
BITMAP *disrupter_circle3;
//BITMAP *obj_bmp [85];
BITMAP *point_bmp [20];
RLE_SPRITE *th_base_bmp [16];
BITMAP *rocket_bmp [6] [7]; // 6 types, 7 frames each, all fully lit
BITMAP *grapple_bmp [6] [5]; // 6 types, 5 frames, 5 light levels
BITMAP *bomb_bmp [7]; // 8 types, 5 light levels
BITMAP *pickup_bmp [25];
RLE_SPRITE *base_bmp [16];
BITMAP *lfruit_bmp [2];
BITMAP *mfruit_bmp [3];
BITMAP *sfruit_bmp [4];

RLE_SPRITE *light_circle [100];
RLE_SPRITE *dark_circle [5];

enum
{
BOMB_GRENADE,
BOMB_DIRTBOMB,
BOMB_TOXIN,
BOMB_FRAG,
BOMB_MORTAR,
BOMB_REDBOMB,
BOMB_FIREBOMB
};

enum
{
PKBMP_WEAPON,
PKBMP_HEALTH,
PKBMP_SHIELD,
PKBMP_ARMOUR,
PKBMP_CLOAK,
PKBMP_FLAG_RED,
PKBMP_FLAG_BLUE,
PKBMP_FLAG_GREEN,
PKBMP_FLAG_GOLD,
PKBMP_FLAG_GREY, // now purple
PKBMP_FLAG_BROWN,
PKBMP_FLAG_SILVER, // not yet existing
PKBMP_GRAIL,
PKBMP_SHADOW,
PKBMP_JETPACK,
PKBMP_ROCKETPACK,
PKBMP_REGENERATOR,
PKBMP_SPOTLIGHT,
PKBMP_RFLAG_RED,
PKBMP_RFLAG_BLUE,
PKBMP_RFLAG_GREEN,
PKBMP_RFLAG_GOLD,
PKBMP_RFLAG_GREY, // now purple
PKBMP_RFLAG_BROWN,
PKBMP_RFLAG_SILVER // not yet existing

};

enum
{
BASEBMP_RED,
BASEBMP_BLUE,
BASEBMP_GREEN,
BASEBMP_GOLD,
BASEBMP_BROWN,
BASEBMP_GREY,
BASEBMP_SILVER,
BASEBMP_TH_BACK,
BASEBMP_TH_MIDDLE,
BASEBMP_TH_FRONT,
BASEBMP_TH_FRONT_RED,
BASEBMP_TH_FRONT_BLUE,
BASEBMP_TH_FRONT_GREEN,
BASEBMP_TH_FRONT_GOLD,
BASEBMP_TH_FRONT_BROWN,
BASEBMP_TH_FRONT_GREY

};


enum
{
LFRUIT_DURIAN,
LFRUIT_PINEAPPLE
};

enum
{
MFRUIT_PEACH,
MFRUIT_RAPPLE,
MFRUIT_PEAR
};

enum
{
SFRUIT_CHERRY,
SFRUIT_STRAWBERRY,
SFRUIT_FIG,
SFRUIT_BLUEBERRY
};


//BITMAP *map_bmp;

//int mapx [NO_ACTORS], mapy [NO_ACTORS];

char message_text [10] [2] [50];
int message_timeout [10] [2]; // associated with message_text, not _rank
int message_colour [10] [2];


//int message_rank [10] [2];
/*
display:
goes through message_rank and copies message_text [message_rank []] into
message_display in the order of message_rank [0] to [9].
Then prints message_display in order, if their timeout is >0.

*/

/*
send:
if a slot available, strcpies msg into an available message_text
(timeout < 0) and puts it on the end of message_rank, bumping
down message_rank if there're holes.

Bumps down message_rank:

*/
void send_message(int player_mess, const char *msg, int colour);
//void bump_messages(char playerm);



void display_messages(BITMAP *bmp, int playerm)
{
 int mri = 1;
 text_mode(-1);

 int colour = 0;

 for (mri = 0; mri < 10; mri ++)
 {
  if (message_timeout [mri] [playerm] > 0)
   message_timeout [mri] [playerm] --;
  if (message_timeout [mri] [playerm] == 0) continue;
  if (message_colour [mri] [playerm] == COLOUR_GREY8)
   colour = COLOUR_GREY1 + (message_timeout [mri] [playerm] / 13);
    else colour = message_colour [mri] [playerm] - 3 + (message_timeout [mri] [playerm] / 26);
  textprintf(bmp, large_font, 32, 23 + mri * text_size, colour, message_text [mri] [playerm]);
 }

}

void message(int user_target, const char *msg, int colour)
{

 if (user_target == -1 || user_target == player[1].user)
 {
  send_message(0, msg, colour);
 }
 if (user_target == -1 || user_target == player[2].user)
 {
  send_message(1, msg, colour);
 }
 return;

}

void teamed_message(int user_target, const char *msg, int team)
{

 int colour = team_colours(team);
 
 if (team == TEAM_NONE)
  colour = COLOUR_GREY8;

 if (user_target == -1 || user_target == player[1].user)
 {
  send_message(0, msg, colour);
 }
 if (user_target == -1 || user_target == player[2].user)
 {
  send_message(1, msg, colour);
 }
 return;
 
}


void send_message(int player_mess, const char *msg, int colour)
{
 int mri;
 int lowest_timeout = 5000;
 int lowest_t_mri = 0;

 for (mri = 0; mri < 10; mri ++)
 {
  if (message_timeout [mri] [player_mess] == 0)
  {
   strcpy(message_text [mri] [player_mess], msg);
   message_timeout [mri] [player_mess] = 100;
   message_colour [mri] [player_mess] = colour;
   return;
  }
  if (message_timeout [mri] [player_mess] < lowest_timeout)
  {
   lowest_timeout = message_timeout [mri] [player_mess];
   lowest_t_mri = mri;
  }
 }

   strcpy(message_text [lowest_t_mri] [player_mess], msg);
   message_timeout [lowest_t_mri] [player_mess] = 100;
   message_colour [lowest_t_mri] [player_mess] = colour;
 
}



/*
void display_messages(BITMAP *bmp, int playerm)
{
 int mri = 1;
 text_mode(0);

 int i = 0;
 int need_bump = 0;
 char message_display [30] [10];


 for (mri = 0; mri < 10; mri ++)
 {
//  strcpy(message_display [mri], "");
  message_display [mri] [0] = 0;
  if (message_rank [mri] [playerm - 1] == -1) continue;
  if (message_timeout [message_rank [mri] [playerm - 1]] [playerm - 1] == 0) continue;
  message_timeout [message_rank [mri] [playerm - 1]] [playerm - 1] --;
  if (message_timeout [message_rank [mri] [playerm - 1]] [playerm - 1] == 0)
   need_bump = 1;
    else
     strcpy(message_display [mri], message_text [message_rank [mri] [playerm - 1]] [playerm - 1]);
/ *  if (message_timeout [mri] [playerm - 1] == 0)
  {
   strcpy(message_rank [mri] [playerm - 1], "");
   message_rank [mri] [playerm - 1] = NULL;* /
//  }
 }

 for (mri = 0; mri < 10; mri ++)
 {
    if (message_display [mri] [0] != '\0')
     textprintf(bmp, large_font, 32, 23 + mri * 15, WHITE, message_display [mri]);
 }

 if (need_bump) bump_messages(playerm);

}

void message(int user_target, const char *msg)
{

 if (user_target == -1 || user_target == player[1].user)
 {
  send_message(1, msg);
 }
 if (user_target == -1 || user_target == player[2].user)
 {
  send_message(2, msg);
 }
 return;

}

void send_message(int player_mess, const char *msg)
{
// return;
 int mri = 9;
 int j;

/ * strcpy(message_text [0] [player_mess - 1], "hello");
 message_timeout [0] [player_mess - 1] = 1000;
 message_rank [0] [player_mess - 1] = 0;
 return;* /

 message_timeout [message_rank [9] [player_mess - 1]] [player_mess - 1] = 0;

 for (mri = 0; mri < 10; mri ++)
 {
  if (message_timeout [mri] [player_mess - 1] == 0)
  {
   strcpy(message_text [mri] [player_mess - 1], "hello"); //msg);
   message_timeout [mri] [player_mess - 1] = 1000;
   for (j = 0; j < 10; j ++)
   {
    if (message_rank [j] [player_mess - 1] == -1 || message_timeout [message_rank [j] [player_mess - 1]] [player_mess - 1] == 0)
    {
     message_rank [j] [player_mess - 1] = mri;
     return;
    }
   }
   return; // should never happen
  }
 }
/ *
 strcpy(message_text [mri2] [player_mess], msg);
 message_timeout [mri] [player_mess] = 100;
 message_rank [mri] [player_mess] = message_text [mri2] [player_mess];
* /
}


void bump_messages(char playerm)
{

 int last_empty_mess = 8;
 int curr_mess = 0;

 for (curr_mess = 0; curr_mess < 10; curr_mess ++)
 {
  if (message_timeout [message_rank [curr_mess] [playerm - 1]] [playerm - 1] == 0)
  {
   if (message_rank [last_empty_mess] [playerm - 1] == -1 || message_timeout [message_rank [last_empty_mess] [playerm - 1]] [playerm - 1] != 0)
    last_empty_mess = curr_mess;
     // otherwise, is an empty slot preceded by empty slot(s), so ignore
  }
  else
   if (message_timeout [message_rank [last_empty_mess] [playerm - 1]]  [playerm - 1]== 0)
   {
     message_rank [last_empty_mess] [playerm - 1] = curr_mess;
     message_timeout [curr_mess] [playerm - 1] = 0;
     curr_mess = last_empty_mess + 1; // + 1???
   }

  for (curr_mess = 0; curr_mess < 10; curr_mess ++)
  {
   if (message_timeout [message_rank [curr_mess] [playerm - 1]] [playerm - 1])
    message_rank [curr_mess] [playerm - 1] = -1;
  }

//   last_empty_mess ++:

 }

}
*/

/*
void display_messages(BITMAP *bmp, int playerm)
{
 int mri = 1;
 text_mode(0);

 int i = 0;


 for (i = 0; i < 10; i ++)
 {
  if (message_timeout [mri] [playerm - 1] == 0) continue;
  message_timeout [mri] [playerm - 1] --;
  textprintf(bmp, large_font, 32, 23 + mri * 10, WHITE, message_rank [mri] [playerm]);
  if (message_timeout [mri] [playerm - 1] == 0)
  {
   strcpy(message_rank [mri] [playerm - 1], "");
   message_rank [mri] [playerm - 1] = NULL;
//   rerank_messages();
  }
  mri ++;
 }
}

void message(int user_target, const char *msg)
{

 if (user_target == -1 || user_target == player[1].user)
 {
  send_message(0, msg);
 }
 if (user_target == -1 || user_target == player[2].user)
 {
  send_message(1, msg);
 }
 return;

}

void send_message(int player_mess, const char *msg)
{
 return;
 int mri = 9;

 message_rank [9] = NULL;

 while (mri > 0)
 {
  message_rank [mri] [player_mess] = message_rank [mri - 1] [player_mess];
  message_timeout [mri] [player_mess] = message_timeout [mri - 1] [player_mess];
  mri --;
 }

 mri = 0;
 int mri2 = 0;

 while(message_rank [mri] [player_mess] != NULL)
 {
  mri ++;
 }

 while(strlen(message_text [mri2] [player_mess]) > 0)
 {
  mri2 ++;
 }

 strcpy(message_text [mri2] [player_mess], msg);
 message_timeout [mri] [player_mess] = 100;
 message_rank [mri] [player_mess] = message_text [mri2] [player_mess];

}
*/



void init_cloud_pattern(void)
{

if (cloud_pattern != NULL)
 destroy_bitmap(cloud_pattern);

cloud_pattern = create_bitmap(50, 50);
   if (cloud_pattern == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: cannot create bitmap (not enough memory?)");
      exit(1);
   }

int i, j;

for (i = 0; i < 50; i ++)
{
 for (j = 0; j < 50; j ++)
 {
  if ((i + j) % 2 == 0) putpixel(cloud_pattern, i, j, 1);
     else putpixel(cloud_pattern, i, j, 0);
 }
}

if (disrupter_circle != NULL)
 destroy_bitmap(disrupter_circle);
if (disrupter_circle2 != NULL)
 destroy_bitmap(disrupter_circle2);
disrupter_circle2 = create_bitmap(42, 42);
   if (disrupter_circle2 == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: cannot create disruptor bitmap (not enough memory?)");
      exit(1);
   }
clear_bitmap(disrupter_circle2);
circlefill(disrupter_circle2, 20, 20, 19, 1);

for (i = 0; i < 20; i ++)
{

 if (shield_circle [i] != NULL)
  destroy_bitmap(shield_circle [i]);
 shield_circle [i] = create_bitmap(i * 2 + 1, i * 2 + 1);
    if (shield_circle [i] == NULL)
    {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Fatal Error: cannot create shield bitmap (not enough memory?)");
       exit(1);
    }
 clear_bitmap(shield_circle [i]);
 circlefill(shield_circle [i], i, i, i, 1);
}

disrupter_circle = create_bitmap(11, 11);
clear_bitmap(disrupter_circle);
circlefill(disrupter_circle, 5, 5, 4, 1);

if (disrupter_circle3 != NULL)
 destroy_bitmap(disrupter_circle3);
disrupter_circle3 = create_bitmap(62, 62);
   if (disrupter_circle3 == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: cannot create disruptor bitmap (not enough memory?)");
      exit(1);
   }
clear_bitmap(disrupter_circle3);


for (i = 0; i < 10; i ++)
{
// message_rank [i] [0] = -1;
// message_rank [i] [1] = -1;
 message_timeout [i] [0] = 0;
 message_timeout [i] [1] = 0;
 strcpy(message_text [i] [0], "");
 strcpy(message_text [i] [1], "");
 message_text [0] [i] [0] = '\0';
 message_text [0] [i] [1] = '\0';
// strcpy(message_display [i] [0], "");
// strcpy(message_display [i] [1], "");
}

/*if (map_bmp != NULL)
 destroy_bitmap(map_bmp);
map_bmp = create_bitmap(map_width + 2, map_height + 2);
   if (map_bmp == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: cannot create bitmap (not enough memory?)");
      exit(1);
   }
clear_bitmap(map_bmp);
rect(map_bmp, 0, 0, map_width + 1, map_height + 1, 75);

for (i = 0; i < NO_ACTORS; i ++)
{
 mapx [i] = map_width / 2;
 mapy [i] = map_height / 2;
}
*/
}



void grid_to_bitmap(int x_pos, int y_pos, int bm_width, int bm_height, BITMAP *g_to_bmp)
{

 clear_bitmap(g_to_bmp);

/* int i, j;
 for (i = 0; i < game[0].max_x / GRID_WIDTH_X; i ++)
 {
  for (j = 0; j < game[0].max_y / GRID_WIDTH_Y; j ++)
  {
     draw_sprite(g_to_bmp, brick_bmp [4], i * GRID_WIDTH_X, j * GRID_WIDTH_Y);
  }
 }*/

 if (no_players == 1)
  blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - window_centre_x_sp, y_pos / GRAIN - (window_centre_y + 1), 0, 0, bm_width, bm_height);
   else
    blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - window_centre_x_2p, y_pos / GRAIN - (window_centre_y + 1), 0, 0, bm_width, bm_height);

/*
 if (no_players == 1)
  blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - (window_centre_x_sp * GRID_WIDTH_X), y_pos / GRAIN - (window_centre_y * GRID_WIDTH_Y), 0, 0, bm_width * GRID_WIDTH_X, bm_height * GRID_WIDTH_Y);
   else
    blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - (window_centre_x_2p * GRID_WIDTH_X), y_pos / GRAIN - (window_centre_y * GRID_WIDTH_Y), 0, 0, bm_width * GRID_WIDTH_X, bm_height * GRID_WIDTH_Y);
*/

}




void brick_grid_to_bitmap(int x_pos, int y_pos, int bm_width, int bm_height, BITMAP *g_to_bmp)
{

int glx = 0, gly = 0;

int x = grid_ref_x(x_pos);
int y = grid_ref_y(y_pos);


//int x_offset = ((x_pos / GRAIN) % BRICK_X) * -1;
//int y_offset = ((y_pos / GRAIN) % BRICK_Y) * -1;


//unsigned char colour = 0;

for (glx = 0; glx < bm_width; glx ++)
{
 for (gly = 0; gly < bm_height; gly ++)
 {
/*  if (get_grid(glx + (x / GRAIN) - (bm_width / 2) + 2, gly + (y / GRAIN) - (bm_height / 2) + 2) == 0)
    draw_sprite(g_to_bmp, brick_bmp [1], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset);
        else draw_sprite(g_to_bmp, brick_bmp [0], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset);
*/


/*    switch(get_specific_grid(glx + (x / GRAIN) - (bm_width / 2) + 2, gly + (y / GRAIN) - (bm_height / 2) + 2))
    {
     case GRIDREDBRICK_BACK: draw_sprite(g_to_bmp, brick_bmp [1], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCKBACK: draw_sprite(g_to_bmp, brick_bmp [6], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_REDBRICK: draw_sprite(g_to_bmp, brick_bmp [0], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK1: draw_sprite(g_to_bmp, brick_bmp [2], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK2: draw_sprite(g_to_bmp, brick_bmp [3], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK3: draw_sprite(g_to_bmp, brick_bmp [4], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK4: draw_sprite(g_to_bmp, brick_bmp [5], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_DIRT: draw_sprite(g_to_bmp, brick_bmp [7], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_DIRTG: draw_sprite(g_to_bmp, brick_bmp [8], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_DIRTBACK: draw_sprite(g_to_bmp, brick_bmp [9], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;

    }*/

    switch(get_specific_grid(glx + (x / GRAIN) - (bm_width / 2) + 2, gly + (y / GRAIN) - (bm_height / 2) + 2))
    {
//     case 0: rectfill(g_to_bmp, glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset, (glx + 1) * GRID_WIDTH_X + x_offset, (gly + 1) * GRID_WIDTH_Y + y_offset, DARKGRAY); break;
//     case 1: rectfill(g_to_bmp, glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset, (glx + 1) * GRID_WIDTH_X + x_offset, (gly + 1) * GRID_WIDTH_Y + y_offset, BROWN); break;
/*     case GRID_ROCKBACK: draw_sprite(g_to_bmp, brick_bmp [6], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_REDBRICK: draw_sprite(g_to_bmp, brick_bmp [0], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK1: draw_sprite(g_to_bmp, brick_bmp [2], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK2: draw_sprite(g_to_bmp, brick_bmp [3], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK3: draw_sprite(g_to_bmp, brick_bmp [4], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_ROCK4: draw_sprite(g_to_bmp, brick_bmp [5], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_DIRT: draw_sprite(g_to_bmp, brick_bmp [7], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_DIRTG: draw_sprite(g_to_bmp, brick_bmp [8], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_DIRTBACK: draw_sprite(g_to_bmp, brick_bmp [9], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
*/
/*     case GRID_REDBRICK1: draw_sprite(g_to_bmp, brick_bmp [0], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_REDBRICK2: draw_sprite(g_to_bmp, brick_bmp [1], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_SSTONE1: draw_sprite(g_to_bmp, brick_bmp [2], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_SSTONE2: draw_sprite(g_to_bmp, brick_bmp [3], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     case GRID_SSTONE_BACK: draw_sprite(g_to_bmp, brick_bmp [4], glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset); break;
     */
    }



//    rectfill(g_to_bmp, glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset, (glx + 1) * GRID_WIDTH_X + x_offset, (gly + 1) * GRID_WIDTH_Y + y_offset, 0);

/*  colour = palette_color [48];
     else colour = 0;
  rectfill(g_to_bmp, glx * GRID_WIDTH_X + x_offset, gly * GRID_WIDTH_Y + y_offset, (glx + 1) * GRID_WIDTH_X + x_offset, (gly + 1) * GRID_WIDTH_Y + y_offset, colour);*/
 }
}

}

int brick_colour(int bricky, int lit)
{
/* switch(bricky)
 {
  case BRICK_ROCK:
   switch(lit)
   {
    case 0: return COLOUR_BLACK;
    case 1: return COLOUR_BROWN1;
    case 2: return COLOUR_BROWN2;
    case 3: return COLOUR_BROWN3;
    default:
    case 4: return COLOUR_BROWN4;
   }
  break;
  case BRICKBACK_ROCK:
   switch(lit)
   {
    case 0: return COLOUR_BLACK;
    case 1: return COLOUR_GREY1;
    case 2: return COLOUR_GREY2;
    case 3: return COLOUR_GREY3;
    default:
    case 4: return COLOUR_GREY4;
   }
  break;
 }
*/
 return COLOUR_PURPLE;
}

void display_bricks(int x_pos, int y_pos, int bm_width, int bm_height, BITMAP *g_to_bmp)
{

 clear_bitmap(g_to_bmp);

 int x = grid_ref_x(x_pos) / GRAIN;
 int y = grid_ref_y(y_pos) / GRAIN;

// int x_offset = ((x_pos / GRAIN) % BRICK_X) * -1;// + window_centre_x_sp;
// int y_offset = ((y_pos / GRAIN) % BRICK_Y) * -1;// + window_centre_y;

 int i, j, brick_x, brick_y;
 int bx, by;
 
 for (i = 0; i < bm_width / BRICK_X; i ++)
 {
  for (j = 0; j < bm_height / BRICK_Y; j ++)
  {
//     draw_sprite(g_to_bmp, brick_bmp [4], i * GRID_WIDTH_X, j * GRID_WIDTH_Y);
//     brick_col = brick_colour(get_brick(i + x_offset, j + y_offset));
     bx = i + x - ((bm_width / BRICK_X) / 2) + 1;
     by = j + y - ((bm_height / BRICK_Y) / 2) + 2;
     brick_y = get_brick(bx, by);
//     brick_y = (brick_y * (BRICK_Y + 1) * 2) + 1;
     brick_y = (brick_y * (BRICK_Y + 1)) + 1;
     brick_x = get_light_level(bx, by);
     if (brick_x > 4) brick_x = 4;
     brick_x = brick_x * BRICK_X + brick_x + 1;
//     blit(brick_bmp, g_to_bmp, brick_x, brick_y, i * BRICK_X + x_offset, j * BRICK_Y + y_offset, BRICK_X, BRICK_Y);
//     rectfill(g_to_bmp, i * BRICK_X + x_offset, j * BRICK_Y + y_offset, (i + 1) * BRICK_X + x_offset, (j + 1) * BRICK_Y + y_offset, brick_col);
  }
 }

/* if (no_players == 1)
  blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - window_centre_x_sp, y_pos / GRAIN - (window_centre_y + 1), 0, 0, bm_width, bm_height);
   else
    blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - window_centre_x_2p, y_pos / GRAIN - (window_centre_y + 1), 0, 0, bm_width, bm_height);
*/

/*
 if (no_players == 1)
  blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - (window_centre_x_sp * GRID_WIDTH_X), y_pos / GRAIN - (window_centre_y * GRID_WIDTH_Y), 0, 0, bm_width * GRID_WIDTH_X, bm_height * GRID_WIDTH_Y);
   else
    blit(dirt_bmp, g_to_bmp, x_pos / GRAIN - (window_centre_x_2p * GRID_WIDTH_X), y_pos / GRAIN - (window_centre_y * GRID_WIDTH_Y), 0, 0, bm_width * GRID_WIDTH_X, bm_height * GRID_WIDTH_Y);
*/

}




int grid_ref_x(int position)
{
 return position / BRICK_X;
}

int grid_ref_y(int position)
{
 return position / BRICK_Y;
}



/*
dactor - actor being displayed
x_centre, y_centre - centre of current bitmap, in grid ref pixels
x_limit, y_limit - distance out from actor which is being displayed

Assumes that dactor is visible in bmp window, but no problem if not.
*/
void display_actor(int dactor, int x_centre, int y_centre, int x_limit, int y_limit, int window_width, BITMAP *bmp, char display_names)
{

  int draw_effect = (actor[dactor].cloak
   && (actor[dactor].uncloaked == 0 || prand(10) > actor[dactor].uncloaked));

  if (actor [dactor].aclass == ACLASS_NONE
   || actor[dactor].aclass == ACLASS_GHOST) return;

//  if (draw_effect == 1) return;

//  actor[dactor].lit = 3;



  int put_x = (actor [dactor].x / GRAIN) - x_centre + x_limit;
  if (put_x < 0 || put_x > window_width) return;
  
  int put_y = (actor [dactor].y / GRAIN) - y_centre + y_limit;
  if (put_y < 0 || put_y > window_grid_y) return;

  if (actor[dactor].aclass == ACLASS_PORKBALL)
  {
   circlefill(bmp, put_x, put_y, 3, team_colours(arena[0].porkball_state));
   return;
  }

  if (display_names && !draw_effect)
  {
//   if (arena[0].teams)
    display_word(user[actor[dactor].user].uname, actor[dactor].x, actor[dactor].y - 18 * GRAIN, x_centre, y_centre, x_limit, y_limit, window_width, actor[dactor].colour1, bmp, 0);
//     else
//      display_word(user[actor[dactor].user].uname, actor[dactor].x, actor[dactor].y - 18 * GRAIN, x_centre, y_centre, x_limit, y_limit, window_width, 15, bmp);
//      display_word(user[actor[dactor].user].uname, actor[dactor].x, actor[dactor].y - 18 * GRAIN, x_centre, y_centre, x_limit, y_limit, window_width, actor[dactor].colour1, bmp, 0);
  }

  if (actor[dactor].has_grail == 1)
  {
   // they get a halo:
    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
//    circlefill(bmp, put_x, put_y - 6, 7, TRANS_YELLOW);
//    circlefill(bmp, put_x, put_y - 6, rand() % 4 + 4, TRANS_WHITE);
    circle(bmp, put_x, put_y - 6, 7 + (arena[0].counter / 5) % 8, TRANS_YELLOW);
//    circle(bmp, put_x, put_y - 6, 8 + (arena[0].counter / 5) % 8, TRANS_WHITE);
//    circlefill(bmp, put_x, put_y - 6, rand() % 4 + 4, TRANS_WHITE);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  }

/*  if (actor[dactor].lit > 0)
  {
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circle(bmp, put_x, put_y - 6, actor[dactor].lit * 2, TRANS_YELLOW);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  }*/

  if (actor[dactor].has_flag != -1)
  {
    if (actor[dactor].facing == 1)
     draw_trans_sprite(bmp, pickup_bmp [PKBMP_FLAG_RED + actor[dactor].has_flag - 1], put_x - 10, put_y - 8);
      else
       draw_trans_sprite(bmp, pickup_bmp [PKBMP_RFLAG_RED + actor[dactor].has_flag - 1], put_x + 2, put_y - 8);
  }

//void draw_actor_sprite(int dactor, int flip, BITMAP *sprite, BITMAP *bmp, int put_x, int put_y, int effect)

  int leg_state = actor[dactor].leg_frame;

  if (actor[dactor].facing == -1)
  {
//    draw_sprite(bmp, actor_bmp [animate_legs(dactor)], put_x - 8, put_y - 8);
   if (actor[dactor].soldier != SOLDIER_SNAIL)
    draw_actor_sprite(dactor, 1, actor_bmp [dactor] [leg_state] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);

    if (actor[dactor].total_dip > 4)
     put_y += 4;
      else
       put_y += actor[dactor].total_dip;
/*
  if (leg_state == P1_WALK_2 || leg_state == P1_WALK_3 || leg_state == P1_WALK_8 || leg_state == P1_WALK_9)
   put_y --;
  if (leg_state == P1_WALK_6 || leg_state == P1_WALK_1)
   put_y ++;*/

     if (actor[dactor].angle > 1.2)
      draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_DR6] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 1.0)
      draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_DR5] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.9)
      draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_DR4] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.8)
      draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_DR3] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.6)
      draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_DR2] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.4)
      draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_DR1] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.1)
      draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_FR] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.4)
       draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_UR1] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.6)
       draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_UR2] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.8)
       draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_UR3] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.9)
       draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_UR4] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -1.00)
       draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_UR5] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
//     if (actor[dactor].angle > -1.2)
       draw_actor_sprite(dactor, 1, actor_bmp [dactor] [P1_AIM_UR6] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);


//     else
//   if (actor[dactor].angle > -1.05)
//       draw_sprite(bmp, actor_bmp [15], put_x - 8, put_y - 8);
//     return;

   if (actor[dactor].soldier == SOLDIER_SNAIL)
   {
    if (actor[dactor].total_dip > 4)
     put_y -= 4;
      else
       put_y -= actor[dactor].total_dip;
    draw_actor_sprite(dactor, 1, actor_bmp [dactor] [leg_state] [1] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
   }

     if (actor[dactor].shield_visible > 0) shield_effect(dactor, bmp, put_x, put_y, actor[dactor].shield_visible);
     if (actor[dactor].armour_pulse > 0) soft_shield_effect(dactor, bmp, put_x, put_y, actor[dactor].armour_pulse);

  }

  
  if (actor[dactor].facing == 1)
  {
   if (actor[dactor].soldier != SOLDIER_SNAIL)
    draw_actor_sprite(dactor, 0, actor_bmp [dactor] [leg_state] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);

    if (actor[dactor].total_dip > 4)
     put_y += 4;
      else
       put_y += actor[dactor].total_dip;
/*    put_y += actor[dactor].walk_bounce [leg_state - P1_WALK_1];
    if (actor[dactor].dip > 3)
     put_y += 3;
      else
       put_y += actor[dactor].dip;*/

/*  if (leg_state == P1_WALK_2 || leg_state == P1_WALK_3 || leg_state == P1_WALK_8 || leg_state == P1_WALK_9)
   put_y --;
  if (leg_state == P1_WALK_6 || leg_state == P1_WALK_1)
   put_y ++;*/

     if (actor[dactor].angle > 1.2)
      draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_DR6] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 1.0)
      draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_DR5] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.9)
      draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_DR4] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.8)
      draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_DR3] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.6)
      draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_DR2] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > 0.4)
      draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_DR1] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.1)
       draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_FR] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.4)
       draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_UR1] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.6)
       draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_UR2] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.8)
       draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_UR3] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -0.9)
       draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_UR4] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
     if (actor[dactor].angle > -1.00)
       draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_UR5] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
      else
//     if (actor[dactor].angle > -1.2)
       draw_actor_sprite(dactor, 0, actor_bmp [dactor] [P1_AIM_UR6] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
//     else
//   if (actor[dactor].angle > -1.05)
//       draw_sprite(bmp, actor_bmp [15], put_x - 8, put_y - 8);
   if (actor[dactor].soldier == SOLDIER_SNAIL)
   {
    if (actor[dactor].total_dip > 4)
     put_y -= 4;
      else
       put_y -= actor[dactor].total_dip;
    draw_actor_sprite(dactor, 0, actor_bmp [dactor] [leg_state] [0] [actor [dactor].backpack], bmp, put_x, put_y, draw_effect);
   }

     if (actor[dactor].shield_visible > 0) shield_effect(dactor, bmp, put_x, put_y, actor[dactor].shield_visible);
     if (actor[dactor].armour_pulse > 0) soft_shield_effect(dactor, bmp, put_x, put_y, actor[dactor].armour_pulse);

  }



//  if (actor[dactor].facing == -1)
//    draw_actor_sprite(dactor, 1, actor_bmp [animate_legs(dactor)], bmp, put_x, put_y, draw_effect);

}



void draw_actor_sprite(int dactor, int flip, BITMAP *sprite, BITMAP *bmp, int put_x, int put_y, int effect)
{

 switch(effect)
 {
  case 0:
//   switch(flip)
//   {
//    case 0: // no flip
    draw_trans_sprite(bmp, sprite, put_x - actor[dactor].sprite_width, put_y - actor[dactor].sprite_height);
//    break;
//    case 1: // h flip
//    draw_sprite_h_flip(bmp, sprite, put_x - actor[dactor].sprite_width, put_y - actor[dactor].sprite_height);
//    draw_trans_sprite(bmp, sprite, put_x - actor[dactor].sprite_width, put_y - actor[dactor].sprite_height);
//    break;
  /*  case 2: // v flip
    draw_sprite_h_flip(bmp, sprite, put_x - actor[dactor].width, put_y - actor[dactor].height);
    break;
    case 3: // v&h  flip
    draw_sprite_vh_flip(bmp, sprite, put_x - actor[dactor].width, put_y - actor[dactor].height);
    break;*/
//  }
  break;

//void distortion_mask(BITMAP *mask, BITMAP *bmp, int x, int y, int width, int height, int disp_x, int disp_y, int flip_x, int flip_y)
  
  case 1: // distortion effect
//   switch(flip)
//   {
//    case 0: // no flip
     distortion_mask(sprite, bmp, put_x - actor[dactor].sprite_width, put_y - actor[dactor].sprite_height, actor[dactor].sprite_width * 2 + 1, actor[dactor].sprite_height * 2 + 1, rand() % 3 - 1, rand() % 3 - 1, 1, 1);
//    break;
//    case 1: // h flip
//     distortion_mask(sprite, bmp, put_x - actor[dactor].sprite_width, put_y - actor[dactor].sprite_height, actor[dactor].sprite_width * 2 + 1, actor[dactor].sprite_height * 2 + 1, rand() % 3 - 1, rand() % 3 - 1, -1, 1);
//    break;
  /*  case 2: // v flip
    draw_sprite_h_flip(bmp, sprite, put_x - actor[dactor].width, put_y - actor[dactor].height);
    break;
    case 3: // v&h  flip
    draw_sprite_vh_flip(bmp, sprite, put_x - actor[dactor].width, put_y - actor[dactor].height);
    break;*/
//   }
  break;
 }


}




void shield_effect(int dactor, BITMAP *bmp, int put_x, int put_y, int magnitude)
{

     distortion_mask(shield_circle [actor[dactor].circle_radius], bmp, put_x - actor[dactor].circle_radius, put_y - actor[dactor].circle_radius, actor[dactor].circle_radius * 2 + 1, actor[dactor].circle_radius * 2 + 1, rand() % (magnitude) - (magnitude / 2), rand() % (magnitude) - (magnitude / 2), 1, 1);

}

void soft_shield_effect(int dactor, BITMAP *bmp, int put_x, int put_y, int magnitude)
{

     if (magnitude >= actor[dactor].circle_radius)
     {
      circle(bmp, put_x, put_y, actor[dactor].circle_radius, COLOUR_BLUE4);
      magnitude = actor[dactor].circle_radius - 1;
     }

//   int rad = (magnitude * actor[dactor].circle_radius) / 10;
     
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
   circlefill(bmp, put_x, put_y, magnitude, TRANS_BLUE);
   drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);


}

/*void distort_circle(BITMAP *bmp, int put_x, int put_y, int radius, int magnitude)
{

     distortion_mask(disrupter_circle, bmp, put_x - radius, put_y - radius, radius * 2 + 1, radius * 2 + 1, rand() % (magnitude) - (magnitude / 2), rand() % (magnitude) - (magnitude / 2), 1, 1);

}*/


void display_crosshair(int dactor, int x_centre, int y_centre, int x_limit, int y_limit, BITMAP *bmp)
{

 if (actor[dactor].aclass == ACLASS_GHOST) return;

 int cross_x, cross_y, thc;

 float crx1, cry1;


 if (actor[dactor].lock_on != -1 && actor [actor[dactor].lock_on].aclass == ACLASS_PLAYER)
 {
/*
  int put_x = (actor [actor[dactor].lock_on].x / GRAIN) - x_centre + x_limit;
  int put_y = (actor [actor[dactor].lock_on].y / GRAIN) - y_centre + y_limit;
  
  if (put_x >= 0 && put_x <= x_limit * 2 && put_y >= 0 && put_y <= y_limit * 2)
  {
    circle(bmp, put_x, put_y, 8 - arena[0].counter % 8, 64 + arena[0].counter % 15);
  }*/

  int put_x = (actor [actor[dactor].lock_on].x / GRAIN) - x_centre + x_limit;
  int put_y = (actor [actor[dactor].lock_on].y / GRAIN) - y_centre + y_limit;
  
  if (put_x >= 0 && put_x <= x_limit * 2 && put_y >= 0 && put_y <= y_limit * 2)
  {
    int aim_col = COLOUR_YELLOW1 + arena[0].counter % 4;
    circle(bmp, put_x, put_y, 11, aim_col);
    putpixel(bmp, put_x, put_y, aim_col);
    int lx1, lx2, ly1, ly2;
    lx1 = put_x + cos(((float) arena[0].counter / 32) * PI) * 11;
    lx2 = put_x + cos(((float) arena[0].counter / 32) * PI) * 15;
    ly1 = put_y + sin(((float) arena[0].counter / 32) * PI) * 11;
    ly2 = put_y + sin(((float) arena[0].counter / 32) * PI) * 15;
    line(bmp, lx1, ly1, lx2, ly2, aim_col);
    lx1 = put_x + cos(((float) (arena[0].counter + 16) / 32) * PI) * 11;
    lx2 = put_x + cos(((float) (arena[0].counter + 16) / 32) * PI) * 15;
    ly1 = put_y + sin(((float) (arena[0].counter + 16) / 32) * PI) * 11;
    ly2 = put_y + sin(((float) (arena[0].counter + 16) / 32) * PI) * 15;
    line(bmp, lx1, ly1, lx2, ly2, aim_col);
    lx1 = put_x + cos(((float) (arena[0].counter + 32) / 32) * PI) * 11;
    lx2 = put_x + cos(((float) (arena[0].counter + 32) / 32) * PI) * 15;
    ly1 = put_y + sin(((float) (arena[0].counter + 32) / 32) * PI) * 11;
    ly2 = put_y + sin(((float) (arena[0].counter + 32) / 32) * PI) * 15;
    line(bmp, lx1, ly1, lx2, ly2, aim_col);
    lx1 = put_x + cos(((float) (arena[0].counter + 48) / 32) * PI) * 11;
    lx2 = put_x + cos(((float) (arena[0].counter + 48) / 32) * PI) * 15;
    ly1 = put_y + sin(((float) (arena[0].counter + 48) / 32) * PI) * 11;
    ly2 = put_y + sin(((float) (arena[0].counter + 48) / 32) * PI) * 15;
    line(bmp, lx1, ly1, lx2, ly2, aim_col);
  }  
 }
 
 if (actor[dactor].remote_control == REMOTE_ROCKET || actor[dactor].remote_control == REMOTE_ROCKET_C)
 {
  thc = actor[dactor].remote_thing;
  cross_x = cos(bullet[thc].angle) * crosshair_distance / 2;

  cross_x *= bullet[thc].facing;
 
  cross_y = sin(bullet[thc].angle) * crosshair_distance / 2;

  cross_x += bullet[thc].x / GRAIN + x_limit - x_centre;
  cross_y += bullet[thc].y / GRAIN + y_limit - y_centre;

  circle(bmp, cross_x, cross_y, 2, COLOUR_RED4);
 
  return;
 }

 if (actor[dactor].weapon [actor[dactor].current_weapon] == WPN_TRACKER
     || actor[dactor].weapon [actor[dactor].current_weapon] == WPN_SEEKER)
 {
  cross_x = cos(actor[dactor].angle) * 80;

  cross_x *= actor [dactor].facing;
 
  cross_y = sin(actor[dactor].angle) * 80;

  cross_x += actor [dactor].x / GRAIN + actor[dactor].fire_x + x_limit - x_centre;
  cross_y += actor [dactor].y / GRAIN + actor[dactor].fire_y + y_limit - y_centre;

  if (actor[dactor].lock_on != -1)
  {
  circle(bmp, cross_x, cross_y, 8, COLOUR_YELLOW4);
  
  cross_x += cos((float) arena[0].counter / PI) * 13;
  cross_y += sin((float) arena[0].counter / PI) * 13;

  circle(bmp, cross_x, cross_y, 1, COLOUR_GREY8);

  }
    else
     circle(bmp, cross_x, cross_y, 8, COLOUR_RED4); //palette_color [100 + (dactor * 10)]);

 }
  else
 {
  crx1 = cos(actor[dactor].angle);
  cry1 = sin(actor[dactor].angle);

  cross_x = crx1 * crosshair_distance;
  cross_x *= actor [dactor].facing;
 
  cross_y = cry1 * crosshair_distance;

  cross_x += actor [dactor].x / GRAIN + actor[dactor].fire_x + x_limit - x_centre;
  cross_y += actor [dactor].y / GRAIN + actor[dactor].fire_y + y_limit - y_centre;

  if (actor[dactor].lock_on != -1)
   circle(bmp, cross_x, cross_y, 2, COLOUR_RED4);
    else
    {
     circle(bmp, cross_x, cross_y, 3, COLOUR_YELLOW4); //palette_color [100 + (dactor * 10)]);

     cross_x = crx1 * (crosshair_distance - 4);
     cross_x *= actor [dactor].facing;
 
     cross_y = cry1 * (crosshair_distance - 4);

     cross_x += actor [dactor].x / GRAIN + actor[dactor].fire_x + x_limit - x_centre;
     cross_y += actor [dactor].y / GRAIN + actor[dactor].fire_y + y_limit - y_centre;

     putpixel(bmp, cross_x, cross_y, COLOUR_YELLOW4); //palette_color [100 + (dactor * 10)]);

     cross_x = crx1 * (crosshair_distance - 5);
     cross_x *= actor [dactor].facing;
 
     cross_y = cry1 * (crosshair_distance - 5);

     cross_x += actor [dactor].x / GRAIN + actor[dactor].fire_x + x_limit - x_centre;
     cross_y += actor [dactor].y / GRAIN + actor[dactor].fire_y + y_limit - y_centre;

     putpixel(bmp, cross_x, cross_y, COLOUR_YELLOW4); //palette_color [100 + (dactor * 10)]);
    }
 }
 
}

void display_weapon_name(int dplayer, int x_centre, int y_centre, int x_limit, int y_limit, int window_width, BITMAP *bmp)
{

 int weapon_named = actor[(int) player[dplayer].actor_controlled].weapon [(int) player[dplayer].slot_selected];

 int x_pos = actor[player[dplayer].actor_controlled].x;
 int y_pos = (actor[player[dplayer].actor_controlled].y - (actor[player[dplayer].actor_controlled].height * GRAIN)) - 20 * GRAIN;

 display_word(wlist [weapon_named].long_name, x_pos, y_pos, x_centre, y_centre, x_limit, y_limit, window_width, COLOUR_GREY8, bmp, 1);

}


void display_word(char *dword, int x_pos, int y_pos, int x_centre, int y_centre, int x_limit, int y_limit, int window_width, int colour, BITMAP *bmp, char force_display)
{

text_mode(-1);

  int put_x = (x_pos / GRAIN) - x_centre + x_limit;
  if (put_x < 0 || put_x > window_width) return;
  
  int put_y = (y_pos / GRAIN) - y_centre + y_limit;
  if (put_y < 0 || put_y > window_grid_y) return;

 if (force_display == 1 || is_point_lit(bmp, put_x, put_y))
 {
  textprintf_centre(bmp, small_font, put_x, put_y, colour, dword);
 }

}


void display_bullet(int dbull, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp)
{

  int put_x = (bullet[dbull].x / GRAIN) - x_centre + x_limit;
  if (put_x < 0 || put_x > window_width) return;
  
  int put_y = (bullet[dbull].y / GRAIN) - y_centre + y_limit;
  if (put_y < 0 || put_y > window_grid_y) return;

//  int to_draw = 0;
  int bullet_state = 0;
  int vflip = 1;

  int put_x2, put_y2, bcol;
//  int end_x, end_y;

//  int bullet_state = 0;

  switch(bullet[dbull].bullet_type)
  {
  case BULLET_GRAPPLE:
  draw_grapple(bmp, dbull, put_x, put_y);
//  circle(bmp, put_x, put_y, 3, palette_color [75]);
  break;
  case BULLET_GREN_LAUNCHER:
  draw_trans_sprite(bmp, bomb_bmp [BOMB_MORTAR], put_x - 2, put_y - 3);
  break;
  case BULLET_FIREBOMB:
  draw_trans_sprite(bmp, bomb_bmp [BOMB_FIREBOMB], put_x - 2, put_y - 3);
  break;
  case BULLET_RPG:
  case BULLET_NUKE_M:
  case BULLET_SQUIRM:
  case BULLET_SWARM:
  case BULLET_LR_ROCKET:
  case BULLET_REMOTE_ROCKET:
  case BULLET_REMOTE_ROCKET_C:
  case BULLET_TRACKER:
  case BULLET_SEEKER:
  case BULLET_ROCKET:
  bullet_state = (bullet[dbull].angle * 12) / PI;
  if (bullet[dbull].bullet_type == BULLET_RPG)
   bullet_state %= 5;
  if (bullet_state < 0)
  {
    vflip = 0;
    bullet_state *= -1;
  } //else
  if (bullet_state > 6)
  {
    bullet_state = 6;
  }
  int which_rocket, which_frame;
  switch(bullet[dbull].bullet_type)
  {
   case BULLET_SQUIRM: which_rocket = 3; break;
   case BULLET_SWARM: which_rocket = 2; break;
   case BULLET_NUKE_M: which_rocket = 1; break;
   case BULLET_RPG: which_rocket = 4; break;
   default: which_rocket = 0; break;
  }
  which_frame = bullet_state;
  if (bullet[dbull].facing == 1)
  {
   if (vflip == 1)
    draw_sprite_v_flip(bmp, rocket_bmp [which_rocket] [which_frame], put_x - 4, put_y - 4);
     else
      draw_sprite(bmp, rocket_bmp [which_rocket] [which_frame], put_x - 4, put_y - 4);
  } else
        {
         if (vflip == 1)
           draw_sprite_vh_flip(bmp, rocket_bmp [which_rocket] [which_frame], put_x - 4, put_y - 4);
             else
               draw_sprite_h_flip(bmp, rocket_bmp [which_rocket] [which_frame], put_x - 4, put_y - 4);
        }

  break;
  case BULLET_TOX_BOMB:
  draw_trans_sprite(bmp, bomb_bmp [BOMB_TOXIN], put_x - 2, put_y - 3);
  break;
  case BULLET_DIRTCLOD:
  putpixel(bmp, put_x - 1 + rand() % 3, put_y - 1 + rand() % 3, COLOUR_BROWN3);
  putpixel(bmp, put_x, put_y, COLOUR_BROWN3);
  break;
  case BULLET_DIRTBOMB:
//  lit = get_light_level_pixel(bullet[dbull].x, bullet[dbull].y);
//  if (lit > 4) lit = 4;
  draw_trans_sprite(bmp, bomb_bmp [BOMB_DIRTBOMB], put_x - 2, put_y - 3);
  break;
  case BULLET_BOUNCY:
//  lit = get_light_level_pixel(bullet[dbull].x, bullet[dbull].y);
//  if (lit > 4) lit = 4;
  draw_trans_sprite(bmp, bomb_bmp [BOMB_REDBOMB], put_x - 2, put_y - 3);
  break;
  case BULLET_BOMB:
//  lit = get_light_level_pixel(bullet[dbull].x, bullet[dbull].y);
//  if (lit > 4) lit = 4;
  draw_trans_sprite(bmp, bomb_bmp [BOMB_REDBOMB], put_x - 2, put_y - 3);
  break;
  case BULLET_GRENADE:
  draw_trans_sprite(bmp, bomb_bmp [BOMB_GRENADE], put_x - 2, put_y - 3);
  break;
  case BULLET_FRAG:
  draw_trans_sprite(bmp, bomb_bmp [BOMB_FRAG], put_x - 2, put_y - 3);
  break;
  case BULLET_GAS_GREN:
  draw_trans_sprite(bmp, bomb_bmp [BOMB_GRENADE], put_x - 2, put_y - 3);
  break;
  case BULLET_GAS:
    bullet_state = bullet[dbull].fuse / 25;
    if (bullet[dbull].fuse > 200)// && bullet[dbull].fuse < 200)
     bullet_state = (750 - (bullet[dbull].fuse * 3)) / 25;
    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, bullet_state, TRANS_GREEN); //bullet[dbull].status);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;

// NOTE: CHECK to see if put_y should be -3/put_x - 2????
  
  case BULLET_PRONG:
//  end_x = bullet[dbull].time_since_clou
//  end_x = put_x + cos((float) (bullet[dbull].time_since_cloud % (int) (PI * 3) / 3)) * 3;
//  end_y = put_y + sin((float) (bullet[dbull].time_since_cloud % (int) (PI * 3) / 3)) * 3;
//  line(bmp, put_x, put_y, end_x, end_y, WHITE);
  bullet_state = bullet[dbull].time_since_cloud % 9;
  if (bullet[dbull].facing == -1)
   bullet_state = 9 - bullet_state;
  if (bullet_state < 5)
   draw_sprite(bmp, rocket_bmp [5] [bullet_state], put_x - 3, put_y - 3);
    else
     draw_sprite_h_flip(bmp, rocket_bmp [5] [9 - bullet_state], put_x - 6, put_y - 3);
//     draw_sprite_h_flip(bmp, rocket_bmp [5] [9 - bullet_state], put_x - 3, put_y - 3);
  break;
  case BULLET_SHREDDER:
   bullet_state = abs((bullet[dbull].angle * 2) / PI);
   draw_trans_sprite(bmp, grapple_bmp [1] [bullet_state], put_x - 3, put_y - 3);
   if (bullet[dbull].left_owner == 0)
   {
    break;
   }
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
   bcol = COLOUR_GREY4;
   put_x = bullet[dbull].x - (x_centre * GRAIN) + (x_limit * GRAIN);
   put_y = bullet[dbull].y - (y_centre * GRAIN) + (y_limit * GRAIN);
   put_x2 = (bullet[dbull].x_speed * -1);// / GRAIN;
   put_x2 /= 3;
   put_y2 = (bullet[dbull].y_speed * -1);// / GRAIN;
   put_y2 /= 3;
   line(bmp, put_x / GRAIN, put_y / GRAIN, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, bcol);
   bcol --;
   line(bmp, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, bcol);
   bcol --;
   line(bmp, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, (put_x + put_x2 * 3) / GRAIN, (put_y + put_y2 * 3) / GRAIN, bcol);
   drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);
   break;
  case BULLET_FLAK:
   if (game[0].fast_bullets == 2)
    break;
  case BULLET_AUTOCANNON:
  case BULLET_FAT_BULLET:
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
   bcol = bullet[dbull].status;
   putpixel(bmp, put_x, put_y, bcol);
   putpixel(bmp, put_x + 1, put_y, bcol);
   putpixel(bmp, put_x - 1, put_y, bcol);
   putpixel(bmp, put_x, put_y + 1, bcol);
   putpixel(bmp, put_x, put_y - 1, bcol);
   if (bullet[dbull].left_owner == 0)
   {
    break;
   }
   put_x = bullet[dbull].x - (x_centre * GRAIN) + (x_limit * GRAIN);
   put_y = bullet[dbull].y - (y_centre * GRAIN) + (y_limit * GRAIN);
   put_x2 = (bullet[dbull].x_speed * -1);// / GRAIN;
   put_x2 /= 3;
   put_y2 = (bullet[dbull].y_speed * -1);// / GRAIN;
   put_y2 /= 3;
   line(bmp, put_x / GRAIN, put_y / GRAIN, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, bcol);
   bcol --;
   line(bmp, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, bcol);
   bcol --;
   line(bmp, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, (put_x + put_x2 * 3) / GRAIN, (put_y + put_y2 * 3) / GRAIN, bcol);
   drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);
   break;
//  case BULLET_AUTOCANNON:
//  case BULLET_FLAK:
//  case BULLET_FAT_BULLET:
  case BULLET_GRAPESHOT:
  case BULLET_BULLET:
  case BULLET_SHRAPNEL:
//  cast_light(bmp, put_x, put_y, 20, 1);
//  cast_light(bmp, put_x, put_y, 16, 1);
//  lit = get_light_level_pixel(bullet[dbull].x, bullet[dbull].y);
//  if (lit > 4) lit = 4;
//  bcol = lit + 2 + COLOUR_BLACK;
//  if (lit == 0) break;
  drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  bcol = bullet[dbull].status;
  if (bullet[dbull].left_owner == 0)
  {
   putpixel(bmp, put_x, put_y, bcol);
   break;
  }
  put_x = bullet[dbull].x - (x_centre * GRAIN) + (x_limit * GRAIN);
  put_y = bullet[dbull].y - (y_centre * GRAIN) + (y_limit * GRAIN);
  put_x2 = (bullet[dbull].x_speed * -1);// / GRAIN;
  put_x2 /= 3;
  put_y2 = (bullet[dbull].y_speed * -1);// / GRAIN;
  put_y2 /= 3;
  line(bmp, put_x / GRAIN, put_y / GRAIN, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, bcol);
  bcol --;
  line(bmp, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, bcol);
  bcol --;
  line(bmp, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, (put_x + put_x2 * 3) / GRAIN, (put_y + put_y2 * 3) / GRAIN, bcol);
  drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);
  break;
  case BULLET_INCENDIARY:
  if (bullet[dbull].left_owner == 0)
  {
   putpixel(bmp, put_x, put_y, COLOUR_YELLOW4);
   break;
  }
  put_x = bullet[dbull].x - (x_centre * GRAIN) + (x_limit * GRAIN);
  put_y = bullet[dbull].y - (y_centre * GRAIN) + (y_limit * GRAIN);
  put_x2 = (bullet[dbull].x_speed * -1);// / GRAIN;
  put_x2 /= 3;
  put_y2 = (bullet[dbull].y_speed * -1);// / GRAIN;
  put_y2 /= 3;
  line(bmp, put_x / GRAIN, put_y / GRAIN, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, COLOUR_RED4);
  bcol --;
  line(bmp, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, COLOUR_RED3);
  bcol --;
  line(bmp, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, (put_x + put_x2 * 3) / GRAIN, (put_y + put_y2 * 3) / GRAIN, COLOUR_RED2);
  putpixel(bmp, put_x / GRAIN, put_y / GRAIN, COLOUR_YELLOW4);
  break;
  case BULLET_GREN_SHRAPNEL:
  drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  bcol = bullet[dbull].status;
  put_x = bullet[dbull].x - (x_centre * GRAIN) + (x_limit * GRAIN);
  put_y = bullet[dbull].y - (y_centre * GRAIN) + (y_limit * GRAIN);
  put_x2 = (bullet[dbull].x_speed * -1);// / GRAIN;
  put_x2 /= 3;
  put_y2 = (bullet[dbull].y_speed * -1);// / GRAIN;
  put_y2 /= 3;
  line(bmp, put_x / GRAIN, put_y / GRAIN, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, bcol);
  bcol --;
  line(bmp, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, bcol);
  bcol --;
  line(bmp, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, (put_x + put_x2 * 3) / GRAIN, (put_y + put_y2 * 3) / GRAIN, bcol);
  drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);
  break;
  case BULLET_ELECTRO:
  break;
/*  if (bullet[dbull].left_owner == 0)
  {
   putpixel(bmp, put_x, put_y, COLOUR_GREY8);
   break;
  }
  drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);
  bcol = COLOUR_GREY8;*/
  /*
  put_x = (bullet[dbull].x - (x_centre * GRAIN) + (x_limit * GRAIN));
  put_y = (bullet[dbull].y - (y_centre * GRAIN) + (y_limit * GRAIN));
  put_x2 = (put_x - (bullet[dbull].x_speed)) + ((bullet[dbull].status * 3) % 40 - 19) * GRAIN;// / GRAIN;
//  put_x2 /= 3;
  put_y2 = (put_y - (bullet[dbull].y_speed)) + ((bullet[dbull].status * 7) % 40 - 19) * GRAIN;// / GRAIN;
  put_x += ((bullet[dbull].seed * 3) % 40 - 19) * GRAIN;
  put_y += ((bullet[dbull].seed * 7) % 40 - 19) * GRAIN;
  line(bmp, put_x / GRAIN, put_y / GRAIN, put_x2 / GRAIN, put_y2 / GRAIN, bcol);*/

//  put_x = (bullet[dbull].x - (x_centre * GRAIN) + (x_limit * GRAIN));
//  put_y = (bullet[dbull].y - (y_centre * GRAIN) + (y_limit * GRAIN));
//  put_x = (bullet[dbull].status - (x_centre * GRAIN) + (x_limit * GRAIN));
//  put_y = (bullet[dbull].status2 - (y_centre * GRAIN) + (y_limit * GRAIN));
/*  put_x2 = (bullet[dbull].status / GRAIN) - x_centre + x_limit;
  put_y2 = (bullet[dbull].status2 / GRAIN) - y_centre + y_limit;
//  put_x2 = (put_x - (bullet[dbull].x_speed));
//  put_y2 = (put_y - (bullet[dbull].y_speed));
  line(bmp, put_x, put_y, put_x2, put_y2, bcol);*/
//  put_y2 /= 3;
//  line(bmp, put_x / GRAIN, put_y / GRAIN, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, bcol);
//  bcol --;
//  line(bmp, (put_x + put_x2) / GRAIN, (put_y + put_y2) / GRAIN, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, bcol);
//  bcol --;
//  line(bmp, (put_x + put_x2 * 2) / GRAIN, (put_y + put_y2 * 2) / GRAIN, (put_x + put_x2 * 3) / GRAIN, (put_y + put_y2 * 3) / GRAIN, bcol);
//  break;
  
  case BULLET_HUNTER_BOMB:
  putpixel(bmp, put_x - 1 + rand() % 3, put_y - 1 + rand() % 3, COLOUR_GREEN1 + (arena[0].counter / 10) % 4);
  case BULLET_HUNTER:
  putpixel(bmp, put_x, put_y, COLOUR_GREEN1 + (arena[0].counter / 10) % 4);
  break;
  case BULLET_TOXIN:
//  putpixel(bmp, put_x, put_y, 41 - (arena[0].counter % 10));
//  putpixel(bmp, put_x, put_y, 55 - (arena[0].counter % 8));
  drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
  putpixel(bmp, put_x, put_y, COLOUR_YELLOW4 - (((arena[0].counter + dbull) / 4) % 4));
  drawing_mode(DRAW_MODE_SOLID, NULL, NULL, 0);
  break;
  case BULLET_PLAS_R:
  if (options[0].stipple_clouds == CLOUDS_TRANS)
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  circlefill(bmp, put_x, put_y, 1 + (rand() % 2), TRANS_BLUE);
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  circlefill(bmp, put_x, put_y, 1 + (rand() % 2), COLOUR_BLUE4);
//  circle(bmp, put_x, put_y, 6, 48 + rand() % 32);
  break;
  case BULLET_CUBE:
  if (options[0].stipple_clouds == CLOUDS_TRANS)
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  rectfill(bmp, put_x - 1, put_y - 1, put_x + 1, put_y + 1, bullet[dbull].status);
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  bcol = cube_outer(bullet[dbull].status);
  rect(bmp, put_x - 2, put_y - 2, put_x + 2, put_y + 2, bcol);
  break;
  case BULLET_IMPLODER:
  if (rand() % 6 == 0)
   circlefill(bmp, put_x, put_y, 3, 32 + rand() % 40);
  circlefill(bmp, put_x, put_y, 2, 0);
  break;
  case BULLET_IMPLOSION:
  if (rand() % 6 == 0)
   circlefill(bmp, put_x, put_y, bullet[dbull].status + 1, 32 + rand() % 40);
  circlefill(bmp, put_x, put_y, bullet[dbull].status, 0);
  distortion_mask(disrupter_circle2, bmp, put_x - 19, put_y - 19, 42, 42, ((rand() % bullet[dbull].fuse + 1) / 5) - ((bullet[dbull].fuse + 1) / 10), ((rand() % bullet[dbull].fuse + 1) / 5) - ((bullet[dbull].fuse + 1) / 10), 1, 1);
  break;
  case BULLET_NAPALM:
  if (options[0].stipple_clouds == CLOUDS_TRANS)
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  circlefill(bmp, put_x, put_y, (1 + rand() % 3), TRANS_RED);
  circlefill(bmp, put_x + rand() % 10 - 5, put_y + rand() % 10 - 5, 1 + rand() % 5, TRANS_ORANGE);
/*  if (bullet[dbull].fuse >= 16)
  {
       circlefill(bmp, put_x + rand() % 8 - 4, put_y + rand() % 8 - 4, 1 + rand() % 5, TRANS_YELLOW);
  }
        else
        {
         circlefill(bmp, put_x + rand() % 8 - 4, put_y + rand() % 8 - 4, 1 + rand() % 5, TRANS_RED);
        }
  if (rand() % 8 == 0)
   circlefill(bmp, put_x, put_y, (1 + rand() % 2), TRANS_YELLOW);*/
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  break;
  case BULLET_FIREBALL:
  if (options[0].stipple_clouds == CLOUDS_TRANS)
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  circlefill(bmp, put_x, put_y, (bullet[dbull].fuse / 50) + 1 + rand() % 3, TRANS_RED);
  circlefill(bmp, put_x, put_y, (bullet[dbull].fuse / 60) + 1, TRANS_YELLOW);
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  break;
  case BULLET_PLAS_C:
  if (options[0].stipple_clouds == CLOUDS_TRANS)
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  circlefill(bmp, put_x, put_y, (2 + rand() % 4), TRANS_RED);
  circlefill(bmp, put_x, put_y, (2 + rand() % 3), TRANS_YELLOW);
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
//  cast_light(bmp, put_x, put_y, 20, 1);
//  cast_light(bmp, put_x, put_y, 16, 1);
//  cast_light(bmp, put_x, put_y, 14, 1);
//  circle(bmp, put_x, put_y, 7, 48 + rand() % 32);
  break;
  case BULLET_NUKE_PLAS:
  if (options[0].stipple_clouds == CLOUDS_TRANS)
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  circlefill(bmp, put_x, put_y, (7 + rand() % 3), TRANS_RED);
  circlefill(bmp, put_x, put_y, (6 + rand() % 2), TRANS_YELLOW);
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  break;
  case BULLET_PLINKER:
//  circlefill(bmp, put_x, put_y, (1 + rand() % 3), COLOUR_BLUE3);
  circlefill(bmp, put_x, put_y, (3 + rand() % 2), COLOUR_PURPLE);
  break;
  case BULLET_RECTIFIER:
  if (rand() % 2)
   circlefill(bmp, put_x, put_y, (3 + rand() % 2), COLOUR_GREEN4);
    else circlefill(bmp, put_x, put_y, (3 + rand() % 2), COLOUR_GREEN3);
  break;
  case BULLET_FLAME:
/*  circlefill(bmp, put_x, put_y, bullet[dbull].status, palette_color [45]);
  if (bullet[dbull].fuse >= 13)
       circlefill(bmp, put_x, put_y, (1 + rand() % (bullet[dbull].fuse - 12)), palette_color [YELLOW]);
        else
         circlefill(bmp, put_x, put_y, (1 + rand() % bullet[dbull].status), palette_color [RED]);
         */
     if (arena[0].counter % 3 != 0)
      drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
//    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
//     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  circlefill(bmp, put_x + rand() % 10, put_y + rand() % 10, bullet[dbull].status / 2 + 1, TRANS_ORANGE);
//  circlefill(bmp, put_x + rand() % 10, put_y + rand() % 10, bullet[dbull].status / 2 + 1, TRANS_ORANGE);
//  circlefill(bmp, put_x + rand() % 10, put_y + rand() % 10, bullet[dbull].status / 2 + 1, TRANS_ORANGE);
  if (bullet[dbull].fuse >= 16)
  {
       circlefill(bmp, put_x + rand() % 5, put_y + rand() % 5, (1 + rand() % (bullet[dbull].fuse - 12)) / 2, TRANS_YELLOW);
//       circlefill(bmp, put_x + rand() % 5, put_y + rand() % 5, (1 + rand() % (bullet[dbull].fuse - 12)) / 2, TRANS_YELLOW);
//       circlefill(bmp, put_x + rand() % 5, put_y + rand() % 5, (1 + rand() % (bullet[dbull].fuse - 12)) / 2, TRANS_YELLOW);
  }
        else
        {
         circlefill(bmp, put_x + rand() % 5, put_y + rand() % 5, (1 + rand() % bullet[dbull].status), TRANS_RED);
//         circlefill(bmp, put_x + rand() % 5, put_y + rand() % 5, (1 + rand() % bullet[dbull].status), TRANS_RED);
//         circlefill(bmp, put_x + rand() % 5, put_y + rand() % 5, (1 + rand() % bullet[dbull].status), TRANS_RED);
        }

    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  break;
  case BULLET_FUNKY_BOMB:
  case BULLET_FUNKY_BOMBLET:
//  lit = get_light_level_pixel(bullet[dbull].x, bullet[dbull].y);
//  if (lit > 4) lit = 4;
  if (!(rand() % 3))
   draw_trans_sprite(bmp, bomb_bmp [BOMB_GRENADE], put_x - 2, put_y - 3);
    else
     if (!(rand() % 3))
      draw_trans_sprite(bmp, bomb_bmp [BOMB_FIREBOMB], put_x - 2, put_y - 3);
    else
     if (!(rand() % 3))
      draw_trans_sprite(bmp, bomb_bmp [BOMB_TOXIN], put_x - 2, put_y - 3);
    else
      draw_trans_sprite(bmp, bomb_bmp [BOMB_REDBOMB], put_x - 2, put_y - 3);
  break;
  case BULLET_DISRUPTER_WAVE:
  distortion_mask(disrupter_circle2, bmp, put_x - 19, put_y - 19, 42, 42, (rand() % 11) - 5, (rand() % 11) - 5, 1, 1);
  break;
  case BULLET_DISRUPTER:
  distortion_mask(disrupter_circle, bmp, put_x - 5, put_y - 5, 11, 11, 5, 5, 1, 1);
//  distort_circle(bmp, put_x, put_y, 4, 5);
  break;
  case BULLET_DISRUPTER_BANG:
  clear_bitmap(disrupter_circle3);
  circlefill(disrupter_circle3, 30, 30, 30 - (bullet[dbull].fuse / 2), 1);
  circlefill(disrupter_circle3, 30, 30, 30 - (bullet[dbull].fuse), 0);
  distortion_mask(disrupter_circle3, bmp, put_x - 29, put_y - 29, 62, 62, ((rand() % bullet[dbull].fuse + 1) / 2) - ((bullet[dbull].fuse + 1) / 4), ((rand() % bullet[dbull].fuse + 1) / 2) - ((bullet[dbull].fuse + 1) / 4), 1, 1);
  break;
  case BULLET_NUKE_EXP:
  if (options[0].stipple_clouds == CLOUDS_TRANS)
   drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
  if (prand(3) == 0)
  {
   circlefill(bmp, put_x, put_y, 10 + (rand() % 5), TRANS_ORANGE);
  }
    else
    {
     if (prand(3) == 0)
      circlefill(bmp, put_x, put_y, 10 + (rand() % 5), TRANS_YELLOW);
       else
         circlefill(bmp, put_x, put_y, 10 + (rand() % 5), TRANS_RED);
    }
  break;
  case BULLET_FLESH:
//  lit = get_light_level_pixel(bullet[dbull].x, bullet[dbull].y);
//  bcol = adjust_lit_colour(bullet[dbull].status, lit);
  drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
  circlefill(bmp, put_x, put_y, 1 + rand() % 2, bullet[dbull].status);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
  break;
  case BULLET_SPLINTER:
  case BULLET_BLOOD:
//  lit = get_light_level_pixel(bullet[dbull].x, bullet[dbull].y);
//  bcol = adjust_lit_colour(bullet[dbull].status, lit);
  drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
  putpixel(bmp, put_x, put_y, bullet[dbull].status);
  putpixel(bmp, put_x - 1 + rand() % 3, put_y - 1 + rand() % 3, bullet[dbull].status);
  putpixel(bmp, put_x - 1 + rand() % 3, put_y - 1 + rand() % 3, bullet[dbull].status);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
  break;
  case BULLET_SPECK:
  drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
  case BULLET_SPARK: // sparks aren't lightsourced (they glow), so no trans.
  putpixel(bmp, put_x, put_y, bullet[dbull].status);
  drawing_mode(DRAW_MODE_SOLID, NULL, NULL, 0);
  break;
  
/*  case BULLET_NAPALM:
  circlefill(bmp, put_x, put_y, bullet[dbull].height, palette_color [45]);
  if (bullet[dbull].fuse >= 13)
       circlefill(bmp, put_x, put_y, (1 + rand() % ((bullet[dbull].fuse / 5) - 12)), palette_color [YELLOW]);
        else
         circlefill(bmp, put_x, put_y, (1 + rand() % bullet[dbull].height), palette_color [RED]);
  break;*/
  }

}



/*void draw_direction_bullet(int put_x, int put_y, int dbull, BITMAP *bmp, BITMAP *fowards, BITMAP *u1, BITMAP *u2, BITMAP *u3, BITMAP *u4, BITMAP *u5, BITMAP *u6)
{
  int bullet_state = abs((bullet[dbull].angle * 12) / PI);
  if (bullet[dbull].angle > PI) // ie if it's downwards
//     if (bullet_state >= 21
draw_sprite(bmp, obj_bmp[OBJ_RCKT_F], put_x - 4, put_y - 4);
  switch(bullet_state)
  {
  default:
   case 0: draw_sprite(bmp, forwards, put_x - 4, put_y - 4); break;
   case 1: draw_sprite(bmp, obj_bmp[OBJ_RCKT_U1], put_x - 4, put_y - 4); break;
   case 2: draw_sprite(bmp, obj_bmp[OBJ_RCKT_U2], put_x - 4, put_y - 4); break;
   case 3: draw_sprite(bmp, obj_bmp[OBJ_RCKT_U3], put_x - 4, put_y - 4); break;
   case 4: draw_sprite(bmp, obj_bmp[OBJ_RCKT_U4], put_x - 4, put_y - 4); break;
   case 5: draw_sprite(bmp, obj_bmp[OBJ_RCKT_U5], put_x - 4, put_y - 4); break;
   case 6: draw_sprite(bmp, obj_bmp[OBJ_RCKT_U6], put_x - 4, put_y - 4); break;
   case 21: draw_sprite_v_flip(bmp, obj_bmp[OBJ_RCKT_U6], put_x - 4, put_y - 4); break;
   case 22: draw_sprite_v_flip(bmp, obj_bmp[OBJ_RCKT_U5], put_x - 4, put_y - 4); break;
   case 23: draw_sprite_v_flip(bmp, obj_bmp[OBJ_RCKT_U4], put_x - 4, put_y - 4); break;
   case 24: draw_sprite_v_flip(bmp, obj_bmp[OBJ_RCKT_U3], put_x - 4, put_y - 4); break;
   case 25: draw_sprite_v_flip(bmp, obj_bmp[OBJ_RCKT_U2], put_x - 4, put_y - 4); break;
   case 26: draw_sprite_v_flip(bmp, obj_bmp[OBJ_RCKT_U1], put_x - 4, put_y - 4); break;
   case 27: draw_sprite(bmp, obj_bmp[OBJ_GR_L], put_x - 4, put_y - 4); break;
  }


}
*/


// this function relies on allegro's line function clipping.
void display_cords(int dactor, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp)
{
   if (actor[dactor].aclass != ACLASS_PLAYER) return;

   if (actor[dactor].grapple_bullet <= 0) return;

  int put_x_1 = (actor[dactor].x / GRAIN) + (actor[dactor].fire_x * actor[dactor].facing) - x_centre + x_limit;
  int put_y_1 = ((actor[dactor].y + actor[dactor].total_dip) / GRAIN) + actor[dactor].fire_y - y_centre + y_limit;
/*  int put_x_2 = (actor[dactor].grapple_x / GRAIN) - x_centre + x_limit;
  int put_y_2 = (actor[dactor].grapple_y / GRAIN) - y_centre + y_limit;*/
  int put_x_2 = (bullet[actor[dactor].grapple_bullet].x / GRAIN) - x_centre + x_limit;
  int put_y_2 = (bullet[actor[dactor].grapple_bullet].y / GRAIN) - y_centre + y_limit;

  if (put_x_1 < -1000 || put_x_1 > 2000
      || put_x_2 < -1000 || put_x_2 > 2000
      || put_y_1 < -1000 || put_y_1 > 2000
      || put_y_2 < -1000 || put_y_2 > 2000)
       return;

//    drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, 0, 0);
    int grap_col = COLOUR_BROWN4;

    switch(actor[dactor].soldier)
    {
     case SOLDIER_DEMON:
      grap_col = COLOUR_RED4;
      break;
     case SOLDIER_SKELETON:
      grap_col = COLOUR_GREY7;
      break;
     case SOLDIER_BUG:
      grap_col = COLOUR_YELLOW4;
      break;
     case SOLDIER_SNAIL:
      grap_col = COLOUR_GREY4;
      break;
    }

    drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    line(bmp, put_x_1, put_y_1, put_x_2, put_y_2, grap_col);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);

/*  switch(bullet[dbull].bullet_type)
  {
  case BULLET_GRAPPLE:
  circle(bmp, put_x, put_y, 3, palette_color [75]);
  break;
  case BULLET_BOMB:
  circlefill(bmp, put_x, put_y, 2, palette_color [110]);
  break;
  }*/

}

void display_beams(int dactor, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp)
{
   int lcol = 0;

   if (actor[dactor].aclass != ACLASS_PLAYER) return;

   if (actor[dactor].firing_laser == LASER_NONE) return;

//  int put_x_1 = (actor[dactor].x / GRAIN) + (actor[dactor].fire_x * actor[dactor].facing) - x_centre + x_limit;
//  int put_y_1 = ((actor[dactor].y + actor[dactor].total_dip) / GRAIN) + actor[dactor].fire_y - y_centre + y_limit;
  int put_x_1 = actor [dactor].x / GRAIN + (actor[dactor].fire_x * actor[dactor].facing) + x_limit - x_centre;
  int put_y_1 = actor [dactor].y / GRAIN + actor[dactor].fire_y + y_limit - y_centre;
  int put_x_2 = (actor[dactor].laser_x / GRAIN) - x_centre + x_limit;
  int put_y_2 = (actor[dactor].laser_y / GRAIN) - y_centre + y_limit;

  switch(actor[dactor].firing_laser)
  {
   case LASER_TRACER:
   return;
   
   case LASER_BEAM:
   lcol = TRANS_RED;
   break;
   case LASER_PULSE:
   lcol = TRANS_BLUE; //152 - actor[dactor].laser_strength / 2;
   break;
  }

    int flare_x = cos(actor[dactor].angle) * 5;

    flare_x *= actor [dactor].facing;
 
    int flare_y = sin(actor[dactor].angle) * 5;

    put_x_1 += flare_x;
    put_y_1 += flare_y;

    drawing_mode(DRAW_MODE_TRANS, cloud_pattern, 0, 0);
    line(bmp, put_x_1, put_y_1, put_x_2, put_y_2, lcol);
//    circlefill(bmp, put_x_1, put_y_1, 3 + prand(3), lcol);

/*    flare_x *= actor[dactor].firing_distance;
    flare_x /= 10;
    flare_y *= actor[dactor].firing_distance;
    flare_y /= 10;*/

//    flare_x += actor [dactor].x / GRAIN + (actor[dactor].fire_x * actor[dactor].facing) + x_limit - x_centre;
//    flare_y += actor [dactor].y / GRAIN + actor[dactor].fire_y + y_limit - y_centre;

    circlefill(bmp, put_x_1, put_y_1, 3 + rand() % 2, lcol);
    
//    circlefill(bmp, put_x_2, put_y_2, 3 + prand(3), lcol);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
//   actor[dactor].firing_laser = LASER_NONE;


}


void display_all_bullets(int x_centre, int y_centre, int x_limit, int y_limit,
     int window_width, BITMAP *bmp)
{
 int dbloop;

 for (dbloop = 0; dbloop < MAX_BULLETS; dbloop ++)
 {
  if (bullet[dbloop].bullet_type == 0) continue;
  display_bullet(dbloop, x_centre, y_centre, x_limit, y_limit, window_width, bmp);
 }

}

void display_all_pickups(int x_centre, int y_centre, int x_limit, int y_limit,
     int window_width, BITMAP *bmp)
{
 int dploop;

 if (arena[0].has_bases == 1)
 {
  for (dploop = 0; dploop < MAX_PICKUPS; dploop ++)
  {
   if (pickup[dploop].pickup_type != PICKUP_BASE
       && pickup[dploop].pickup_type != PICKUP_TH_BASE) continue;
   display_pickup(dploop, x_centre, y_centre, x_limit, y_limit, window_width, bmp);
  }
 }

 for (dploop = 0; dploop < MAX_PICKUPS; dploop ++)
 {
  if (pickup[dploop].pickup_type == PICKUP_NONE
   || pickup[dploop].pickup_type == PICKUP_BASE
   || pickup[dploop].pickup_type == PICKUP_TH_BASE) continue;
  display_pickup(dploop, x_centre, y_centre, x_limit, y_limit, window_width, bmp);
 }

}


void display_pickup(int dpick, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp)
{

  int put_x = (pickup[dpick].x / GRAIN) - x_centre + x_limit;
  if (put_x < 0 || put_x > window_width) return;
  
  int put_y = (pickup[dpick].y / GRAIN) - y_centre + y_limit;
  if (put_y < 0 || put_y > window_grid_y) return;

//  int lit, lit2;
/*  if (pickup[dpick].pickup_type == PICKUP_TH_BASE || pickup[dpick].pickup_type == PICKUP_BASE)
  {
     lit = get_light_level_pixel(pickup[dpick].x - 15 * GRAIN, pickup[dpick].y);
     lit2 = get_light_level_pixel(pickup[dpick].x + 15 * GRAIN, pickup[dpick].y);
     if (lit2 > lit) lit = lit2;
  }
   else
   {
     lit = get_light_level_pixel(pickup[dpick].x, pickup[dpick].y);
   }

  if (lit > 4)
   lit = 4;*/

//   lit = 2;

//  rectfill(bmp, put_x - 4, put_y - 4, put_x + 4, put_y + 4, GREEN);
  switch(pickup[dpick].pickup_type)
  {
   case PICKUP_WEAPON: draw_trans_sprite(bmp, pickup_bmp [PKBMP_WEAPON], put_x - 4, put_y - 4);
   break;
   case PICKUP_POWER: draw_trans_sprite(bmp, pickup_bmp [PKBMP_HEALTH], put_x - 4, put_y - 4);
   break;
   case PICKUP_GRAIL: draw_trans_sprite(bmp, pickup_bmp [PKBMP_GRAIL], put_x - 4, put_y - 4);
   break;
   case PICKUP_FLAG: draw_trans_sprite(bmp, pickup_bmp [PKBMP_FLAG_RED + pickup[dpick].contains - 1], put_x - 4, put_y - 4);
   break;
   case PICKUP_BASE:
//   rectfill(bmp, put_x - 15, put_y - 15, put_x + 15, put_y + 15,
//    team_colours(pickup[dpick].contains));
   draw_trans_rle_sprite(bmp, base_bmp [BASEBMP_RED + pickup[dpick].contains - 1], put_x - 15, put_y - 15);
   break;
   case PICKUP_TH_BASE:
   draw_trans_rle_sprite(bmp, base_bmp [BASEBMP_TH_BACK], put_x - 15, put_y - 15);
   draw_trans_rle_sprite(bmp, th_base_bmp [arena[0].th_base_angle [pickup[dpick].th_index] / 16], put_x - 15, put_y - 15);
//   X draw_trans_sprite(bmp, obj_bmp[OBJ_TH_BASE_B2], put_x - 15, put_y - 15);
   if (arena[0].th_base_taken [pickup[dpick].th_index] == TEAM_NONE)
      draw_trans_rle_sprite(bmp, base_bmp [BASEBMP_TH_FRONT], put_x - 15, put_y - 15);
       else
        draw_trans_rle_sprite(bmp, base_bmp [BASEBMP_TH_FRONT_RED + arena[0].th_base_taken [pickup[dpick].th_index] - 1], put_x - 15, put_y - 15);
//        draw_trans_sprite(bmp, obj_bmp[OBJ_TH_BASE_1 + arena[0].th_base_taken [pickup[dpick].th_index] - 1], put_x - 15, put_y - 15);
   break;
   case PICKUP_EQUIP:
   switch(pickup[dpick].contains)
   {
    case EQUIP_SHIELD: draw_trans_sprite(bmp, pickup_bmp [PKBMP_SHIELD], put_x - 4, put_y - 4); break;
    case EQUIP_ARMOUR: draw_trans_sprite(bmp, pickup_bmp [PKBMP_ARMOUR], put_x - 4, put_y - 4); break;
    case EQUIP_CLOAK: draw_trans_sprite(bmp, pickup_bmp [PKBMP_CLOAK], put_x - 4, put_y - 4); break;
    case EQUIP_JETPACK: draw_trans_sprite(bmp, pickup_bmp [PKBMP_JETPACK], put_x - 4, put_y - 4); break;
    case EQUIP_ROCKETPACK: draw_trans_sprite(bmp, pickup_bmp [PKBMP_ROCKETPACK], put_x - 4, put_y - 4); break;
    case EQUIP_SPOTLIGHT: draw_trans_sprite(bmp, pickup_bmp [PKBMP_SPOTLIGHT], put_x - 4, put_y - 4); break;
    case EQUIP_REGENERATOR: draw_trans_sprite(bmp, pickup_bmp [PKBMP_REGENERATOR], put_x - 4, put_y - 4); break;
    case EQUIP_SHADOW: draw_trans_sprite(bmp, pickup_bmp [PKBMP_SHADOW], put_x - 4, put_y - 4); break;
   }
   break;
   case PICKUP_FRUIT:
   switch(pickup[dpick].contains)
   {
    case FRUITS_CHERRY:
     draw_trans_sprite(bmp, sfruit_bmp [SFRUIT_CHERRY], put_x - 4, put_y - 3);
     break;
    case FRUITS_BLUEBERRY:
     draw_trans_sprite(bmp, sfruit_bmp [SFRUIT_BLUEBERRY], put_x - 4, put_y - 3);
     break;
    case FRUITS_FIG:
     draw_trans_sprite(bmp, sfruit_bmp [SFRUIT_FIG], put_x - 4, put_y - 3);
     break;
    case FRUITS_STRAWBERRY:
     draw_trans_sprite(bmp, sfruit_bmp [SFRUIT_STRAWBERRY], put_x - 4, put_y - 3);
     break;
    case FRUITM_PEACH:
     draw_trans_sprite(bmp, mfruit_bmp [MFRUIT_PEACH], put_x - 6, put_y - 5);
     break;
    case FRUITM_RAPPLE:
     draw_trans_sprite(bmp, mfruit_bmp [MFRUIT_RAPPLE], put_x - 6, put_y - 5);
     break;
    case FRUITM_PEAR:
     draw_trans_sprite(bmp, mfruit_bmp [MFRUIT_PEAR], put_x - 6, put_y - 5);
     break;
    break;
    case FRUITL_DURIAN:
     draw_trans_sprite(bmp, lfruit_bmp [LFRUIT_DURIAN], put_x - 8, put_y - 8);
     break;
    case FRUITL_PINEAPPLE:
     draw_trans_sprite(bmp, lfruit_bmp [LFRUIT_PINEAPPLE], put_x - 8, put_y - 8);
     break;
   }
   break;
  }

//  int lit = 4;

  if (game[0].name_boxes == 1)
  {
   switch(pickup[dpick].pickup_type)
   {
    case 1:
    display_word(wlist [pickup[dpick].contains].long_name, pickup[dpick].x, pickup[dpick].y - (20 * GRAIN), x_centre, y_centre, x_limit, y_limit, window_width, COLOUR_WRITING, bmp, 0);
    break;
    case 3:
    display_word(wlist [pickup[dpick].contains + 2].long_name, pickup[dpick].x, pickup[dpick].y - (20 * GRAIN), x_centre, y_centre, x_limit, y_limit, window_width, COLOUR_WRITING, bmp, 0);
    break;
   }
  }

}





void display_cloud(int dcloud, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp)
{

  int put_x = (cloud[dcloud].x / GRAIN) - x_centre + x_limit;
  if (put_x < 0 || put_x > window_width) return;
  
  int put_y = (cloud[dcloud].y / GRAIN) - y_centre + y_limit;
  if (put_y < 0 || put_y > window_grid_y) return;
//    drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);

  int cube_size, put_x2, put_y2;
  

  switch(cloud[dcloud].cloud_type)
  {
   case CLOUD_ELECTRO:
   drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);
//   put_x2 = (put_x * GRAIN) + cloud[dcloud].x_speed;//(put_x - (cloud[dbull].x_speed)) + ((bullet[dbull].status / 6) % 40 - 19) * GRAIN;// / GRAIN;
//   put_y2 = (put_y * GRAIN) + cloud[dcloud].y_speed;//(put_y - (bullet[dbull].y_speed)) + ((bullet[dbull].status / 4) % 40 - 19) * GRAIN;// / GRAIN;
//   put_x += ((bullet[dbull].seed / 3) % 40 - 19) * GRAIN;
//   put_y += ((bullet[dbull].seed / 7) % 40 - 19) * GRAIN;
   put_x2 = (cloud[dcloud].x_speed / GRAIN) - x_centre + x_limit;
   put_y2 = (cloud[dcloud].y_speed / GRAIN) - y_centre + y_limit;
   cube_size = COLOUR_BLUE1 + ((cloud[dcloud].size + 1) / 2) - 1;
   if (cloud[dcloud].size == 8)
    cube_size = COLOUR_GREY8;
   if (put_x2 < 0 || put_x2 > 640 || put_y2 < 0 || put_y2 > 480)
    break;
   if (put_x < 0 || put_x > 640 || put_y < 0 || put_y > 480)
    break;
   line(bmp, put_x, put_y, put_x2, put_y2, cube_size);
  break;
  case CLOUD_GREY_SMOKE:
    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_WHITE);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_BLACK_SMOKE:
    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_BLACK);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_SPAWN:
    display_spawn_cloud(bmp, dcloud, put_x, put_y);
    break;
  case CLOUD_RED_EXPLOSION:
//    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
//     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_RED);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_ORANGE_EXPLOSION:
//    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
//     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_ORANGE);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_YELLOW_EXPLOSION:
//    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
//     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_YELLOW);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_NUMBER:
    text_mode(-1);
    textprintf_centre(bmp, small_font, put_x, put_y, COLOUR_WRITING, "%i", cloud[dcloud].state);
    break;
  case CLOUD_DEBUG:
    putpixel(bmp, put_x, put_y, palette_color [15]);
    break;
  case CLOUD_BLUE_EXPLOSION:
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_BLUE);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_LBLUE_EXPLOSION:
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, COLOUR_BLUE4);
    break;
  case CLOUD_GREEN_EXPLOSION:
//    drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_GREEN);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_LGREEN_EXPLOSION:
//    drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, COLOUR_GREEN4);
//    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_GREEN_ELLIPSE:
//    drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    ellipsefill(bmp, put_x, put_y, cloud[dcloud].state / 100, cloud[dcloud].size / 100, TRANS_GREEN);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
//    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_LGREEN_ELLIPSE:
//    drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    ellipsefill(bmp, put_x, put_y, cloud[dcloud].state / 100, cloud[dcloud].size / 100, COLOUR_GREEN4);
//    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    break;
  case CLOUD_YELLOW_FLASH:
    circlefill(bmp, put_x, put_y, cloud[dcloud].state, COLOUR_YELLOW4);
   break;
  case CLOUD_YELLOW2_FLASH:
    circlefill(bmp, put_x, put_y, cloud[dcloud].state, COLOUR_YELLOW3);
   break;
  case CLOUD_RED_FLASH:
    circlefill(bmp, put_x, put_y, cloud[dcloud].state, COLOUR_RED4);
   break;
  case CLOUD_BLUE_FLASH:
    circlefill(bmp, put_x, put_y, cloud[dcloud].state, COLOUR_BLUE4);
   break;
  case CLOUD_GREEN_FLASH:
    circlefill(bmp, put_x, put_y, cloud[dcloud].state, COLOUR_GREEN4);
   break;
  case CLOUD_RED_BLOOD:
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, BLOOD_RED);
   break;
  case CLOUD_BLOOD:
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, cloud[dcloud].state);
   break;
  case CLOUD_COL_FLASH:
  case CLOUD_COL_EXPLOSION:
//    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
//     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, cloud[dcloud].state);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
   break;
  case CLOUD_COL_CIRCLE:
  case CLOUD_COL_BURST:
//    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
//     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
//    if (options[0].stipple_clouds == CLOUDS_TRANS)
//     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, cloud[dcloud].state);
//    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
   break;
  case CLOUD_COL_SMOKE:
    if (options[0].stipple_clouds == CLOUDS_STIPPLE)
     drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
    if (options[0].stipple_clouds == CLOUDS_TRANS)
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, cloud[dcloud].state);
    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
   break;
  case CLOUD_SHOCKWAVE:
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
    if (cloud[dcloud].size > (cloud[dcloud].state * 6) / 10)
     circle(bmp, put_x, put_y, (cloud[dcloud].state - cloud[dcloud].size) / 100, COLOUR_GREY8);
    if (cloud[dcloud].size > (cloud[dcloud].state * 5) / 10)
     circle(bmp, put_x, put_y, ((cloud[dcloud].state - cloud[dcloud].size) / 100) - 1, COLOUR_GREY7);
    if (cloud[dcloud].size > (cloud[dcloud].state * 4) / 10)
     circle(bmp, put_x, put_y, ((cloud[dcloud].state - cloud[dcloud].size) / 100) - 2, COLOUR_GREY5);
    if (cloud[dcloud].size > (cloud[dcloud].state * 3) / 10)
     circle(bmp, put_x, put_y, ((cloud[dcloud].state - cloud[dcloud].size) / 100) - 3, COLOUR_GREY3);
    if (cloud[dcloud].size > (cloud[dcloud].state * 2) / 10)
     circle(bmp, put_x, put_y, ((cloud[dcloud].state - cloud[dcloud].size) / 100) - 4, COLOUR_GREY2);
    break;
  case CLOUD_1_POINT:
   textprintf_centre(bmp, small_font, put_x, put_y - 6, COLOUR_YELLOW1 + (arena[0].counter / 8) % 4, "1");
//    draw_trans_sprite(bmp, point_bmp [POINTS_1_1 + (599 - cloud[dcloud].size) / 100], put_x - 5, put_y - 5);
   break;
  case CLOUD_3_POINTS:
   textprintf_centre(bmp, small_font, put_x, put_y - 6, COLOUR_GREEN1 + (arena[0].counter / 8) % 4, "3");
//    draw_trans_sprite(bmp, point_bmp [POINTS_3_1 + (599 - cloud[dcloud].size) / 100], put_x - 5, put_y - 5);
   break;
  case CLOUD_5_POINTS:
   textprintf_centre(bmp, small_font, put_x, put_y - 6, COLOUR_RED1 + (arena[0].counter / 4) % 4, "5");
//    draw_trans_sprite(bmp, point_bmp [POINTS_5_1 + (599 - cloud[dcloud].size) / 100], put_x - 5, put_y - 5);
   break;
  case CLOUD_CUBE:
   cube_size = cloud[dcloud].size / 100 + 1;
   if (options[0].stipple_clouds == CLOUDS_TRANS)
    drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
   rectfill(bmp, put_x - (cube_size - 1), put_y - (cube_size - 1), put_x + (cube_size - 1), put_y + (cube_size - 1), cloud[dcloud].state);
   drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
   rect(bmp, put_x - cube_size, put_y - cube_size, put_x + cube_size, put_y + cube_size, cube_outer(cloud[dcloud].state));
  break;
   
/*  case CLOUD_DISTORTION:
//    circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, palette_color [42]);
    distortion_mask(disrupter_circle, bmp, put_x - 10, put_y - 10, 22, 22, 5, 5, 0, 0);
   break;*/
  }

    drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);

}


void display_spawn_cloud(BITMAP *bmp, int dcloud, int put_x, int put_y)
{

   int ccol;
   int cx, cy, crad;


   switch(cloud[dcloud].state)
   {
    case SOLDIER_WALKER:
    case SOLDIER_CRUSHER:
    case SOLDIER_CYBORG:
     if (options[0].stipple_clouds == CLOUDS_STIPPLE)
      drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
     if (options[0].stipple_clouds == CLOUDS_TRANS)
      drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
//      rectfill(bmp, put_x - cloud[dcloud].size / 100, put_y - cloud[dcloud].size / 80,
//       put_x + cloud[dcloud].size / 100, put_y + cloud[dcloud].size / 80, TRANS_BLUE);
//      rectfill(bmp, put_x - 20, put_y - cloud[dcloud].size / 80,
//       put_x + 20, put_y + cloud[dcloud].size / 80, TRANS_BLUE);
      rectfill(bmp, put_x - cloud[dcloud].size / 100, put_y - 20,
       put_x + cloud[dcloud].size / 100, put_y + 20, TRANS_BLUE);
      break;
    case SOLDIER_DEMON:
//     if (options[0].stipple_clouds == CLOUDS_STIPPLE)
//      drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
//     if (options[0].stipple_clouds == CLOUDS_TRANS)
//      drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
//      rectfill(bmp, put_x - cloud[dcloud].size / 120, put_y - cloud[dcloud].size / 100,
//       put_x + cloud[dcloud].size / 120, put_y + cloud[dcloud].size / 100, TRANS_BLUE);
      ccol = COLOUR_RED2 + prand(3);
//      if (prand(8) == 0)
//       ccol = COLOUR_YELLOW1 + prand(4);

      draw_pentagram(bmp, put_x, put_y, dcloud, ccol);
//      line(bmp, put_x, put_y + , put_x, put_y, ccol);
      break;
    case SOLDIER_SKELETON:
      ccol = COLOUR_GREEN2 + prand(3);
      draw_pentagram(bmp, put_x, put_y, dcloud, ccol);
      break;
    default:
     if (options[0].stipple_clouds == CLOUDS_STIPPLE)
      drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
     if (options[0].stipple_clouds == CLOUDS_TRANS)
      drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);

     circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_BLUE);

     cx = put_x + cos(((float) arena[0].counter / 16) * PI) * 15;
     cy = put_y + sin(((float) arena[0].counter / 16) * PI) * 15;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_BLUE);
     cx = put_x + cos(((float) (arena[0].counter + 8) / 16) * PI) * 15;
     cy = put_y + sin(((float) (arena[0].counter + 8) / 16) * PI) * 15;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_BLUE);
     cx = put_x + cos(((float) (arena[0].counter + 16) / 16) * PI) * 15;
     cy = put_y + sin(((float) (arena[0].counter + 16) / 16) * PI) * 15;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_BLUE);
     cx = put_x + cos(((float) (arena[0].counter + 24) / 16) * PI) * 15;
     cy = put_y + sin(((float) (arena[0].counter + 24) / 16) * PI) * 15;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_BLUE);
    
     break;
     case SOLDIER_BUG:
     if (options[0].stipple_clouds == CLOUDS_STIPPLE)
      drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
     if (options[0].stipple_clouds == CLOUDS_TRANS)
      drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);

     circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_YELLOW);

     crad = (15 - (cloud[dcloud].size / 100)) * 2 + 10;

     cx = put_x + cos(((float) arena[0].counter / 16) * PI) * crad;
     cy = put_y + sin(((float) arena[0].counter / 16) * PI) * crad;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_YELLOW);
     cx = put_x + cos(((float) (arena[0].counter + 8) / 16) * PI) * crad;
     cy = put_y + sin(((float) (arena[0].counter + 8) / 16) * PI) * crad;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_YELLOW);
     cx = put_x + cos(((float) (arena[0].counter + 16) / 16) * PI) * crad;
     cy = put_y + sin(((float) (arena[0].counter + 16) / 16) * PI) * crad;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_YELLOW);
     cx = put_x + cos(((float) (arena[0].counter + 24) / 16) * PI) * crad;
     cy = put_y + sin(((float) (arena[0].counter + 24) / 16) * PI) * crad;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_YELLOW);
    
     break;
     case SOLDIER_SNAIL:
     if (options[0].stipple_clouds == CLOUDS_STIPPLE)
      drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, cloud_pattern_status, 0);
     if (options[0].stipple_clouds == CLOUDS_TRANS)
      drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);

     circlefill(bmp, put_x, put_y, cloud[dcloud].size / 100, TRANS_YELLOW);

     crad = (180 - (cloud[dcloud].size / 10));

     cx = put_x - crad / 3;
     cy = put_y;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_YELLOW);
     cx = put_x - crad / 2;
     cy = put_y;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 300, TRANS_YELLOW);
     cx = put_x - crad;
     cy = put_y;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 400, TRANS_YELLOW);
     cx = put_x + crad / 3;
     cy = put_y;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 200, TRANS_YELLOW);
     cx = put_x + crad / 2;
     cy = put_y;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 300, TRANS_YELLOW);
     cx = put_x + crad;
     cy = put_y;
     circlefill(bmp, cx, cy, cloud[dcloud].size / 400, TRANS_YELLOW);
    
     break;
   }

     drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
   
}


void draw_pentagram(BITMAP *bmp, int put_x, int put_y, int dcloud, int col)
{

     int multiple = cloud[dcloud].size / 50;

     int x1 = put_x + cos(((float) (256 - arena[0].counter) / 16) * PI) * multiple;
     int y1 = put_y + sin(((float) (256 - arena[0].counter) / 16) * PI) * multiple;
     int x2 = put_x + cos(((float) ((256 - arena[0].counter)) / 16 + 0.4) * PI) * multiple;
     int y2 = put_y + sin(((float) ((256 - arena[0].counter)) / 16 + 0.4) * PI) * multiple;
     int x3 = put_x + cos(((float) ((256 - arena[0].counter)) / 16 + 0.8) * PI) * multiple;
     int y3 = put_y + sin(((float) ((256 - arena[0].counter)) / 16 + 0.8) * PI) * multiple;
     int x4 = put_x + cos(((float) ((256 - arena[0].counter)) / 16 + 1.2) * PI) * multiple;
     int y4 = put_y + sin(((float) ((256 - arena[0].counter)) / 16 + 1.2) * PI) * multiple;
     int x5 = put_x + cos(((float) ((256 - arena[0].counter)) / 16 + 1.6) * PI) * multiple;
     int y5 = put_y + sin(((float) ((256 - arena[0].counter)) / 16 + 1.6) * PI) * multiple;
     
/*     int x1 = put_x + cos(((float) (256 - arena[0].counter) / 16) * PI) * multiple;
     int y1 = put_y + sin(((float) (256 - arena[0].counter) / 16) * PI) * multiple;
     int x2 = put_x + cos(((float) ((256 - arena[0].counter) + (16 / 5)) / 16) * PI) * multiple;
     int y2 = put_y + sin(((float) ((256 - arena[0].counter) + (16 / 5)) / 16) * PI) * multiple;
     int x3 = put_x + cos(((float) ((256 - arena[0].counter) + (16 / 5) * 2) / 16) * PI) * multiple;
     int y3 = put_y + sin(((float) ((256 - arena[0].counter) + (16 / 5) * 2) / 16) * PI) * multiple;
     int x4 = put_x + cos(((float) ((256 - arena[0].counter) + (16 / 5) * 3) / 16) * PI) * multiple;
     int y4 = put_y + sin(((float) ((256 - arena[0].counter) + (16 / 5) * 3) / 16) * PI) * multiple;
     int x5 = put_x + cos(((float) ((256 - arena[0].counter) + (16 / 5) * 4) / 16) * PI) * multiple;
     int y5 = put_y + sin(((float) ((256 - arena[0].counter) + (16 / 5) * 4) / 16) * PI) * multiple;
*/
//     int col = COLOUR_RED1 + prand(4);


     line(bmp, x1, y1, x3, y3, col);
     line(bmp, x1, y1, x4, y4, col);
     line(bmp, x2, y2, x4, y4, col);
     line(bmp, x2, y2, x5, y5, col);
     line(bmp, x3, y3, x5, y5, col);

     circle(bmp, put_x, put_y, multiple, col);


}


void display_all_clouds(int x_centre, int y_centre, int x_limit, int y_limit,
     int window_width, BITMAP *bmp)
{
 int dbloop;

 for (dbloop = 0; dbloop < MAX_CLOUDS; dbloop ++)
 {
  if (cloud[dbloop].cloud_type == 0) continue;
  display_cloud(dbloop, x_centre, y_centre, x_limit, y_limit, window_width, bmp);
 }

}


/*int window_x_sp, window_y, window_x_2p, window_grid_x_sp, window_grid_y;
int window_grid_x_2p, window_centre_x_sp, window_centre_x_2p, window_centre_y;*/

void display_windows(BITMAP *bmp_p1, BITMAP *bmp_p2)
{

int window_1 = player [1].actor_controlled;

int da;

if (cloud_pattern_status == 0) cloud_pattern_status = 1;
   else cloud_pattern_status = 0;

//int centre_x1 = actor[window_1].x + (arena [0].shake_x * GRAIN);
//int centre_y1 = actor[window_1].y + (arena [0].shake_y * GRAIN);
int centre_x1 = actor[window_1].x;
int centre_y1 = actor[window_1].y;

if (arena[0].shake_time > 0)
{
 centre_x1 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
 centre_x1 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
 centre_y1 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
 centre_y1 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
}

int centre_x2;
int centre_y2;

if (actor[player[1].actor_controlled].remote_control != REMOTE_NONE)
{
 centre_x1 = bullet[actor[player[1].actor_controlled].remote_thing].x;
 centre_y1 = bullet[actor[player[1].actor_controlled].remote_thing].y;
 if (arena[0].shake_time > 0)
 {
  centre_x1 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
  centre_x1 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
  centre_y1 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
  centre_y1 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
 }
}

if (no_players == 1)
{

    if (arena[0].nuke_flash > 2)
    {
//      if (arena[0].nuke_flash >
//     clear_to_color(bmp_p1, (COLOUR_GREY1 + arena[0].nuke_flash - 2));
     clear_to_color(bmp_p1, COLOUR_GREY8);
    }
    else
     {
//     if (arena[0].level_type == LEVELTYPE_BRICKS)
//      display_bricks(centre_x1 - (window_centre_x_sp - window_sp_offset) * GRAIN, centre_y1 + GRAIN, window_grid_x_sp, window_grid_y, bmp_p1);
//       else
        grid_to_bitmap(centre_x1 - (window_centre_x_sp - window_sp_offset) * GRAIN, centre_y1 + GRAIN, window_grid_x_sp, window_grid_y, bmp_p1);
     display_all_lights(centre_x1 / GRAIN, centre_y1 / GRAIN,
         window_centre_x_sp, window_centre_y, window_x_sp, bmp_p1);
     display_all_pickups(centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_sp, window_centre_y,
                               window_x_sp, bmp_p1);
     for (da = 0; da < NO_ACTORS; da ++)
     {
      display_cords(da, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_sp, window_centre_y,
                               window_x_sp, bmp_p1);
      display_actor(da, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_sp, window_centre_y, window_x_sp, bmp_p1, player[1].show_names > 0);
//     display_actor(2, actor [window_1].x / GRAIN, centre_y1 / GRAIN, window_centre_x_sp * GRID_WIDTH_X, window_centre_y * GRID_WIDTH_Y, window_x_sp, bmp_p1);
      display_beams(da, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_sp, window_centre_y,
                               window_x_sp, bmp_p1);
     }
     display_all_bullets(centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_sp, window_centre_y,
                               window_x_sp, bmp_p1);
     display_all_clouds(centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_sp, window_centre_y,
                               window_x_sp, bmp_p1);

     if (player[1].changing_weapon == 1 && actor[player[1].actor_controlled].aclass == ACLASS_PLAYER)
          display_weapon_name(1, centre_x1 / GRAIN, centre_y1 / GRAIN,
               window_centre_x_sp, window_centre_y, window_x_sp, bmp_p1);

     display_crosshair(player[1].actor_controlled, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_sp, window_centre_y, bmp_p1);

     text_mode(0);
     display_messages(bmp_p1, 0);
     
     if (options[0].show_fps)
     {
      textprintf(bmp_p1, small_font, 100, 40, COLOUR_YELLOW4, "(%i fps)", frames_per_second);
      textprintf(bmp_p1, small_font, 100, 55, COLOUR_YELLOW4, "(%i tps)", turns_per_second);
//      textprintf(bmp_p1, small_font, 100, 70, palette_color[3], "(%i ips)", inputs_per_second);
      textprintf(bmp_p1, small_font, 100, 70, COLOUR_YELLOW4, "(%ims slack)", slacktime);
     }

     if (game[0].show_map)
     {
      display_map(bmp_p1, map_sp_x, player[1].actor_controlled);
//      blit(map_bmp, bmp_p1, 0, 0, map_sp_x, map_y, map_width + 2, map_height + 2);
     }

     if (actor[player[1].actor_controlled].aclass == ACLASS_GHOST)
      ingame_menu(1, bmp_p1, window_centre_x_sp);

     if (player[1].show_scores == 1)
      ingame_scorelist(1, bmp_p1, window_centre_x_sp);

     if (game_over > 0
      || (user[player[1].user].out_of_lives == 1))
//      && game[0].lives_each > 0
//      && user[player[1].user].lives == 0))
     {
/*      int cross_x = cos((float) arena[0].counter / PI) * (300 - game_over) / 5;
      int cross_y = sin((float) arena[0].counter / PI) * (150 - game_over) / 5;

      circlefill(bmp_p1, cross_x + window_centre_x_sp, cross_y + window_centre_y, (200 - game_over) / 15, 176 + (200 - game_over) / 14); //arena[0].counter);
      */
      if (player[1].winner == 0)
       display_game_writing(bmp_p1, 1, window_centre_x_sp, window_centre_y, 20 + (video_mode >= 1) * 20);
//       draw_sprite(bmp_p1, game_over_sign, window_centre_x_sp - 40, window_centre_y - 40);
        else
         display_game_writing(bmp_p1, 0, window_centre_x_sp, window_centre_y, 20 + (video_mode >= 1) * 20);
//         draw_sprite(bmp_p1, you_win_sign, window_centre_x_sp - 40, window_centre_y - 40);
     }
     
    }
    
    if (options[0].run_vsync) vsync();

    blit(bmp_p1, screen, 10, 10, 0, 0, window_grid_x_sp - 15, window_grid_y - 45);
    return;

}

// okay, so two players then:

int window_2 = player [2].actor_controlled;
centre_x2 = actor[window_2].x + (arena [0].shake_x * GRAIN);
centre_y2 = actor[window_2].y + (arena [0].shake_y * GRAIN);
if (arena[0].shake_time > 0)
{
 centre_x2 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
 centre_x2 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
 centre_y2 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
 centre_y2 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
}

if (actor[player[2].actor_controlled].remote_control != REMOTE_NONE)
{
 centre_x2 = bullet[actor[player[2].actor_controlled].remote_thing].x + (arena [0].shake_x * GRAIN);
 centre_y2 = bullet[actor[player[2].actor_controlled].remote_thing].y + (arena [0].shake_y * GRAIN);
 if (arena[0].shake_time > 0)
 {
  centre_x2 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
  centre_x2 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
  centre_y1 += prand(arena[0].shake_time / 3 + 1) * GRAIN;
  centre_y1 -= prand(arena[0].shake_time / 3 + 1) * GRAIN;
 }
}

    if (arena[0].nuke_flash > 2)
    {
//      if (arena[0].nuke_flash >
//     clear_to_color(bmp_p1, (COLOUR_GREY1 + arena[0].nuke_flash - 2));
//     clear_to_color(bmp_p2, (COLOUR_GREY1 + arena[0].nuke_flash - 2));
     clear_to_color(bmp_p1, COLOUR_GREY8);
     clear_to_color(bmp_p2, COLOUR_GREY8);
    }
    else
    {
//     if (arena[0].level_type == LEVELTYPE_BRICKS)
//      display_bricks(centre_x1 - ((window_centre_x_2p - window_2p_offset) * GRAIN), centre_y1 + GRAIN, window_grid_x_2p, window_grid_y, bmp_p1);
//       else
        grid_to_bitmap(centre_x1 - ((window_centre_x_2p - window_2p_offset) * GRAIN), centre_y1 + GRAIN, window_grid_x_2p, window_grid_y, bmp_p1);
     display_all_lights(centre_x1 / GRAIN, centre_y1 / GRAIN,
       window_centre_x_2p, window_centre_y, window_x_2p, bmp_p1);
     display_all_pickups(centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p1);
     for (da = 0; da < NO_ACTORS; da ++)
     {
      display_cords(da, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p1);
      display_actor(da, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_2p, window_centre_y, window_x_2p, bmp_p1, player[1].show_names > 0);
      display_beams(da, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p1);
//     display_actor(2, actor [window_1].x / GRAIN, centre_y1 / GRAIN, window_centre_x_2p * GRID_WIDTH_X, window_centre_y * GRID_WIDTH_Y, window_x_2p, bmp_p1);
     }
     display_all_bullets(centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p1);
     display_all_clouds(centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p1);
     if (player[1].changing_weapon == 1 && actor[player[1].actor_controlled].aclass == ACLASS_PLAYER)
          display_weapon_name(1, centre_x1 / GRAIN, centre_y1 / GRAIN,
               window_centre_x_2p, window_centre_y, window_x_2p, bmp_p1);
     display_crosshair(player[1].actor_controlled, centre_x1 / GRAIN, centre_y1 / GRAIN, window_centre_x_2p, window_centre_y, bmp_p1);
     text_mode(0);
     display_messages(bmp_p1, 0);

     if (options[0].show_fps)
     {
      textprintf(bmp_p1, small_font, 100, 40, COLOUR_YELLOW4, "(%i fps)", frames_per_second);
      textprintf(bmp_p1, small_font, 100, 55, COLOUR_YELLOW4, "(%i tps)", turns_per_second);
//      textprintf(bmp_p1, small_font, 100, 70, palette_color[3], "(%i ips)", inputs_per_second);
      textprintf(bmp_p1, small_font, 100, 70, COLOUR_YELLOW4, "(%ims slack)", slacktime);
     }
/*     textprintf(bmp_p1, font, 50, 50, palette_color[1], "(%i fps)", frames_per_second);
     textprintf(bmp_p1, font, 50, 60, palette_color[2], "(%i tps)", turns_per_second);
     textprintf(bmp_p1, font, 50, 70, palette_color[3], "(%i ips)", inputs_per_second);
  */
//     if (arena[0].level_type == LEVELTYPE_BRICKS)
//      display_bricks(centre_x2 - ((window_centre_x_2p - window_2p_offset) * GRAIN), centre_y2 + GRAIN, window_grid_x_2p, window_grid_y, bmp_p2);
//       else
        grid_to_bitmap(centre_x2 - ((window_centre_x_2p - window_2p_offset) * GRAIN), centre_y2 + GRAIN, window_grid_x_2p, window_grid_y, bmp_p2);
     display_all_lights(centre_x2 / GRAIN, centre_y2 / GRAIN,
      window_centre_x_2p, window_centre_y, window_x_2p, bmp_p2);
     display_all_pickups(centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p2);
     for (da = 0; da < NO_ACTORS; da ++)
     {
      display_cords(da, centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p2);
      display_actor(da, centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p, window_centre_y, window_x_2p, bmp_p2, player[2].show_names > 0);
      display_beams(da, centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p2);

     }
//     display_actor(2, centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p * GRID_WIDTH_X, window_centre_y * GRID_WIDTH_Y, window_x_2p, bmp_p2);
     display_all_bullets(centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p2);
     display_all_clouds(centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p, window_centre_y,
                               window_x_2p, bmp_p2);
     if (player[2].changing_weapon == 1 && actor[player[2].actor_controlled].aclass == ACLASS_PLAYER)
          display_weapon_name(2, centre_x2 / GRAIN, centre_y2 / GRAIN,
               window_centre_x_2p, window_centre_y, window_x_2p, bmp_p2);
     display_crosshair(player[2].actor_controlled, centre_x2 / GRAIN, centre_y2 / GRAIN, window_centre_x_2p, window_centre_y, bmp_p2);
     display_messages(bmp_p2, 1);

     if (game[0].show_map)
     {
      display_map(bmp_p1, map_2p_x, player[1].actor_controlled);
      display_map(bmp_p2, map_2p_x, player[2].actor_controlled);
//      blit(map_bmp, bmp_p1, 0, 0, map_2p_x, map_y, map_width + 2, map_height + 2);
//      blit(map_bmp, bmp_p2, 0, 0, map_2p_x, map_y, map_width + 2, map_height + 2);
     }
     
     if (actor[player[1].actor_controlled].aclass == ACLASS_GHOST)
      ingame_menu(1, bmp_p1, window_centre_x_2p);
     if (actor[player[2].actor_controlled].aclass == ACLASS_GHOST)
      ingame_menu(2, bmp_p2, window_centre_x_2p);

     if (player[1].show_scores == 1)
      ingame_scorelist(1, bmp_p1, window_centre_x_2p);
     if (player[2].show_scores == 1)
      ingame_scorelist(2, bmp_p2, window_centre_x_2p);

     if (game_over != 0
      || (user[player[1].user].out_of_lives == 1))
//      && game[0].lives_each > 0
//      && user[player[1].user].lives == 0))
     {
/*      int cross_x = cos((float) arena[0].counter / PI) * (300 - game_over) / 5;
      int cross_y = sin((float) arena[0].counter / PI) * (150 - game_over) / 5;

      circlefill(bmp_p1, cross_x + window_centre_x_2p, cross_y + window_centre_y, (200 - game_over) / 15, 176 + (200 - game_over) / 14); //arena[0].counter);
      circlefill(bmp_p2, cross_x + window_centre_x_2p, cross_y + window_centre_y, (200 - game_over) / 15, 176 + (200 - game_over) / 14); //arena[0].counter);
      */
      if (player[1].winner == 0)
       display_game_writing(bmp_p1, 1, window_centre_x_2p, window_centre_y, 20 + (video_mode >= 1) * 20);
        else
         display_game_writing(bmp_p1, 0, window_centre_x_2p, window_centre_y, 20 + (video_mode >= 1) * 20);
     }

     if (game_over != 0
      || (user[player[2].user].out_of_lives == 1))
//      && game[0].lives_each > 0
//      && user[player[2].user].lives == 0))
     {
      if (player[2].winner == 0)
       display_game_writing(bmp_p2, 1, window_centre_x_2p, window_centre_y, 20 + (video_mode >= 1) * 20);
        else
         display_game_writing(bmp_p2, 0, window_centre_x_2p, window_centre_y, 20 + (video_mode >= 1) * 20);
     }
     
    }



    
    if (options[0].run_vsync) vsync();

    blit(bmp_p1, screen, 10, 10, 0, 0, window_grid_x_2p - 2 - 15, window_grid_y - 45);
    blit(bmp_p2, screen, 10, 10, window_grid_x_2p, 0, window_grid_x_2p - 2 - 15, window_grid_y - 45);

    return;




}

void display_map(BITMAP *bmp, int map_x, int dactor)
{

int da = 0, mapx, mapy;

/* for (da = 0; da < NO_ACTORS; da ++)
 {
   putpixel(bmp, mapx [da], mapy [da], 0);
//   circlefill(map_bmp, mapx [da], mapy [da], 1, 0);
 }*/

 rectfill(bmp, map_x - 1, map_y - 1, map_x + map_width + 2, map_y + map_height + 2, 0);
 rect(bmp, map_x - 1, map_y - 1, map_x + map_width + 2, map_y + map_height + 2, COLOUR_GREY5);

 for (da = 0; da < NO_ACTORS; da ++)
 {
  if (actor[da].aclass != ACLASS_PLAYER) continue;
  mapx = ((float) (actor[da].x / GRAIN) / arena[0].max_x) * map_width + 1;
  if (mapx > map_width) mapx = map_width;
  mapy = ((float) (actor[da].y / GRAIN) / arena[0].max_y) * map_height + 1;
  if (mapy > map_width) mapy = map_height;
/*  if (arena[0].counter % 2 == 0)
   circlefill(map_bmp, mapx [da], mapy [da], 1, LIGHTGRAY);
    else
     circlefill(map_bmp, mapx [da], mapy [da], 1, 224 + da);  */
//  if (arena[0].counter % 2 == 0)
//  {
//  }
//    else

  if (da == dactor)
  {
   line(bmp, mapx + map_x, map_y, mapx + map_x, map_y + map_height + 1, actor[dactor].colour1);
   line(bmp, map_x, map_y + mapy, map_x + map_width + 1, map_y + mapy, actor[dactor].colour2);
   putpixel(bmp, map_x + mapx, map_y + mapy, COLOUR_GREY6);
  } else
   {
/*   if (arena[0].teams == 1)
   {
    putpixel(bmp, map_x + mapx, map_y + mapy, 224 + da);
    putpixel(bmp, map_x + mapx - 1, map_y + mapy, team_colours(user[actor[da].user].team));
    putpixel(bmp, map_x + mapx + 1, map_y + mapy, team_colours(user[actor[da].user].team));
    putpixel(bmp, map_x + mapx, map_y + mapy - 1, team_colours(user[actor[da].user].team));
    putpixel(bmp, map_x + mapx, map_y + mapy + 1, team_colours(user[actor[da].user].team));
   }
     else*/
      {
       putpixel(bmp, map_x + mapx, map_y + mapy, COLOUR_GREY6);
       putpixel(bmp, map_x + mapx - 1, map_y + mapy, actor[da].colour1);
       putpixel(bmp, map_x + mapx + 1, map_y + mapy, actor[da].colour1);
       putpixel(bmp, map_x + mapx, map_y + mapy - 1, actor[da].colour2);
       putpixel(bmp, map_x + mapx, map_y + mapy + 1, actor[da].colour2);
      }
   
   }

 }

/*
// draws a radar line. Irritating.
   line(bmp, map_x + ((arena[0].counter % 32) * map_width) / 32 + 2, map_y, map_x + ((arena[0].counter % 32) * map_width) / 32 + 2, map_y + map_height + 1, 105 + rand() % 3);
   line(bmp, map_x + ((arena[0].counter % 32) * map_width) / 32 + 1, map_y, map_x + ((arena[0].counter % 32) * map_width) / 32 + 1, map_y + map_height + 1, 108 + rand() % 3);
   line(bmp, map_x + ((arena[0].counter % 32) * map_width) / 32, map_y, map_x + ((arena[0].counter % 32) * map_width) / 32, map_y + map_height + 1, 110 + rand() % 2);
*/
 
}


void draw_grapple(BITMAP *bmp, int gbull, int put_x, int put_y)
{

 int grapple_state = abs((bullet[gbull].angle * 2) / PI);
//int grapple_state = grapple_statef;

 int which_grapple = 2, grsize = 2;

 switch(actor[bullet[gbull].owner].soldier)
 {
  case SOLDIER_WALKER:
  case SOLDIER_CRUSHER:
  case SOLDIER_SNAIL:
   which_grapple = 0;
   grsize = 3;
   break;
  case SOLDIER_SKELETON:
   which_grapple = 3;
   grsize = 2;
   break;
  case SOLDIER_BUG:
   which_grapple = 5;
   grsize = 2;
   break;
  case SOLDIER_DEMON:
   which_grapple = 4;
   grsize = 2;
   break;
  case SOLDIER_SCOUT:
   which_grapple = 1;
   grsize = 2;
   break;
 }

 draw_trans_sprite(bmp, grapple_bmp [which_grapple] [grapple_state], put_x - grsize, put_y - grsize);

/* switch(grapple_state)
 {
  case 0: draw_sprite(bmp, obj_bmp[OBJ_GRAPPLE_1], put_x - 4, put_y - 4); break;
  case 1: draw_sprite(bmp, obj_bmp[OBJ_GRAPPLE_2], put_x - 4, put_y - 4); break;
  case 2: draw_sprite(bmp, obj_bmp[OBJ_GRAPPLE_3], put_x - 4, put_y - 4); break;
  case 3: draw_sprite(bmp, obj_bmp[OBJ_GRAPPLE_4], put_x - 4, put_y - 4); break;

 }*/

/*  int point_x [3];
  int point_y [3];

  float cos_x = cos(bullet[gbull].angle);
  float sin_y = sin(bullet[gbull].angle);

//  circlefill(bmp, put_x, put_y, 2, DARKGRAY);

  point_x [0] = put_x + (cos_x * 1000);
  point_y [0] = put_y + (sin_y * 1000);
  point_x [1] = put_x;// + (cos_x * -300);
  point_y [1] = put_y;//+ (sin_y * 300);
  point_x [2] = put_x + (cos_x * 300);
  point_y [2] = put_y + (sin_y * -300);

  triangle(bmp, point_x [0], point_y [0], point_x [1], point_y [1], point_x [2], point_y [2], WHITE);
//void triangle(BITMAP *bmp, int x1, y1, x2, y2, x3, y3, int color);
*/


}

void init_stat_disp2(int dplayer)
{

   if (dplayer == 1)
   {
    if (no_players == 1)
     init_stat_display(player[1].actor_controlled, stat_x_pos, stat_y_pos, window_x_sp, stat_bar_width, stat_pix_per_point_1p);
      else
       init_stat_display(player[1].actor_controlled, stat_x_pos, stat_y_pos, window_x_2p, stat_bar_width, stat_pix_per_point_2p);
   } else
   {
         init_stat_display(player[2].actor_controlled, stat_x_pos + window_grid_x_2p, stat_y_pos, window_x_2p, stat_bar_width, stat_pix_per_point_2p);
   }

}



void init_stat_display(int dactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point)
{

 // Health
 rectfill(screen, x_pos, y_pos, x_pos + bar_width, y_pos + bar_width, COLOUR_RED3);
// rect(screen, x_pos + bar_width * 2 - 1, y_pos, x_pos + 1 + (pix_per_point * actor[dactor].max_health / 10) + bar_width * 2, y_pos + bar_width, LIGHTGRAY);
 rect(screen, x_pos + bar_width * 2 - 1, y_pos, x_pos + 1 + (pix_per_point * 100) + bar_width * 2, y_pos + bar_width, COLOUR_GREY4);
 
 // Armour
// rectfill(screen, x_pos, y_pos + bar_width * 2, x_pos + bar_width, y_pos + bar_width * 3, DARKGRAY);
// rect(screen, x_pos + bar_width * 2 - 1, y_pos + bar_width * 2, x_pos + 1 + (pix_per_point * 100) + bar_width * 2, y_pos + bar_width * 3, LIGHTGRAY);

 // Ammunition
 rectfill(screen, x_pos, y_pos + bar_width * 3, x_pos + bar_width, y_pos + bar_width * 4, COLOUR_YELLOW3);
 rect(screen, x_pos + bar_width * 2 - 1, y_pos + bar_width * 3, x_pos + 1 + (pix_per_point * 100) + bar_width * 2, y_pos + bar_width * 4, COLOUR_GREY4);


 if (game[0].unlimited_clips == 0)
 {
  // clips
  char clipc;
//  for (clipc = 0; clipc < actor[dactor].max_clips; clipc++)
  int clipcol = COLOUR_GREY4;
  while(clipc < 4)
  {
   if (clipc >= actor[dactor].max_clips) clipcol = COLOUR_BLACK;
   rectfill(screen, x_pos + bar_width * 2 - 1 + (clipc * bar_width * 5), y_pos + (bar_width * 5) - 1, x_pos + bar_width * 2 + (clipc * bar_width * 5) + (bar_width * 4) + 1, 1 + y_pos + bar_width * 6, clipcol);
   clipc ++;
  }
 }
 
// display_health(dactor, x_pos, y_pos, window_width, bar_width, pix_per_point, 0);

}

void display_actor_health(int dactor, int harmed)
{

   if (player[1].actor_controlled == dactor) display_player_health(1, harmed);
   if (player[2].actor_controlled == dactor) display_player_health(2, harmed);

}

void display_player_health(int dplayer, int harmed)
{

//init_stat_display(1, stat_x_pos, stat_y_pos, window_x_sp, stat_bar_width, stat_pix_per_point_1p);
   if (no_players == 1)
   {
    display_health(player[dplayer].actor_controlled, stat_x_pos, stat_y_pos, 1, stat_bar_width, stat_pix_per_point_1p, harmed);
   } else
   {
     display_health(player[dplayer].actor_controlled, stat_x_pos + (window_grid_x_2p * (dplayer == 2)), stat_y_pos, 1, stat_bar_width, stat_pix_per_point_2p, harmed);
   }

}

int health_proportion(int hlth, int max_hlth)
{

  int hlth_max = max_hlth;
  int hlth_bar;

  if (hlth_max == 0) return 0;

  hlth_bar = (hlth * 100) / hlth_max;

  return hlth_bar;

}

void display_health(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int harmed)
{

 int right_harm = x_pos + bar_width * 2 + (pix_per_point * (health_proportion(actor[sactor].health - harmed, actor[sactor].max_health)));// + 4;

 if (right_harm < x_pos + bar_width * 2) right_harm = x_pos + bar_width * 2 + 1;

 int colour = COLOUR_GREEN4;
// if (actor[sactor].armour != 0)
//  colour = 26;

 if (actor[sactor].health > 0)
  rectfill(screen, x_pos + bar_width * 2, y_pos + 1, x_pos + bar_width * 2 + (pix_per_point * health_proportion(actor[sactor].health, actor[sactor].max_health)), y_pos + bar_width - 1, colour);

 if (harmed > 0 && harmed < actor[sactor].health)
  rectfill(screen, x_pos + bar_width * 2 + (pix_per_point * health_proportion(actor[sactor].health, actor[sactor].max_health)), y_pos + 1, right_harm, y_pos + bar_width - 1, COLOUR_RED3);

 text_mode(0);

// textprintf_right(screen, large_font, score_sp_x_pos - 200, score_y_pos, LIGHTGRAY, "Health(%i) %i/%i", sactor, actor[sactor].health, actor[sactor].max_health);

}

void reset_health_bar(int rplayer)
{

  if (actor[player[rplayer].actor_controlled].health == actor[player[rplayer].actor_controlled].max_health) return;

/*  if (no_players == 1)
  {
   rectfill(screen, stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_1p * (actor[player[rplayer].actor_controlled].health) / 10), stat_y_pos + 1, stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_1p * actor[player[rplayer].actor_controlled].max_health / 10), stat_y_pos + stat_bar_width - 1, BLACK);
  } else
  {
   rectfill(screen, (window_grid_x_2p * (rplayer == 2)) + stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_2p * (actor[player[rplayer].actor_controlled].health) / 10), stat_y_pos + 1, (window_grid_x_2p * (rplayer == 2)) + stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_2p * actor[player[rplayer].actor_controlled].max_health / 10), stat_y_pos + stat_bar_width - 1, BLACK);
  }*/
  if (no_players == 1)
  {
   rectfill(screen, stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_1p * health_proportion(actor[player[rplayer].actor_controlled].health, actor[player[rplayer].actor_controlled].max_health)), stat_y_pos + 1, stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_1p * 100), stat_y_pos + stat_bar_width - 1, COLOUR_BLACK);
  } else
  {
   rectfill(screen, (window_grid_x_2p * (rplayer == 2)) + stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_2p * health_proportion(actor[player[rplayer].actor_controlled].health, actor[player[rplayer].actor_controlled].max_health)), stat_y_pos + 1, (window_grid_x_2p * (rplayer == 2)) + stat_x_pos + stat_bar_width * 2 + (stat_pix_per_point_2p * 100), stat_y_pos + stat_bar_width - 1, COLOUR_BLACK);
  }
  
}

void display_actor_ammunition(int dactor)
{

   if (player[1].actor_controlled == dactor) display_player_ammunition(1);
   if (no_players == 2 && player[2].actor_controlled == dactor) display_player_ammunition(2);

}

void display_player_ammunition(int dplayer)
{

//init_stat_display(1, stat_x_pos, stat_y_pos, window_x_sp, stat_bar_width, stat_pix_per_point_1p);
   if (no_players == 1)
   {
    display_ammunition(player[dplayer].actor_controlled, stat_x_pos, stat_y_pos, 1, stat_bar_width, stat_pix_per_point_1p, player[dplayer].slot_selected);
   } else
   {
     display_ammunition(player[dplayer].actor_controlled, stat_x_pos + (window_grid_x_2p * (dplayer == 2)), stat_y_pos, 1, stat_bar_width, stat_pix_per_point_2p, player[dplayer].slot_selected);
   }
   
}


void display_ammunition(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int slot)
{

 float ammun;
 float ammun_max;
 float ammun_bar;
 unsigned char bar_colour = COLOUR_YELLOW3;

 if (actor[sactor].reload [slot] == 0)
 {
  ammun = actor[sactor].ammunition [slot] * 100;
  ammun_max = wlist[actor[sactor].weapon [slot]].ammunition;
  if (ammun_max == 0) ammun_bar = 0;
   else
    ammun_bar = ammun / ammun_max;
 } else
 {
/*  ammun = (wlist[actor[sactor].weapon [slot]].reload_time - actor[sactor].reload [slot]) * 100;
  ammun_max = wlist[actor[sactor].weapon [slot]].reload_time;
  if (ammun_max == 0) ammun_bar = 0;
   else
    ammun_bar = ammun / ammun_max;*/
  ammun = ((wlist[actor[sactor].weapon [slot]].reload_time * game[0].reload_time) - actor[sactor].reload [slot]) * 100;
  ammun /= game[0].reload_time;
//  ammun /= 10;
  ammun_max = wlist[actor[sactor].weapon [slot]].reload_time;
  if (ammun_max == 0) ammun_bar = 0;
   else
    ammun_bar = ammun / ammun_max;
    bar_colour = COLOUR_GREY3;

  ammun_bar -= 90;
  ammun_bar *= 10;
 }


 if (ammun > 0) rectfill(screen, x_pos + bar_width * 2, y_pos + bar_width * 3 + 1, x_pos + bar_width * 2 + (pix_per_point * ammun_bar), y_pos + (bar_width * 4) - 1, bar_colour);
 if (ammun < ammun_max * 100 || ammun_max == 0) rectfill(screen, x_pos + bar_width * 2 + (pix_per_point * ammun_bar), y_pos + 1 + bar_width * 3, x_pos + bar_width * 2 + (pix_per_point * 100), y_pos + (bar_width * 4) - 1, COLOUR_BLACK);

// textprintf_right(screen, large_font, score_sp_x_pos - 300, score_y_pos, LIGHTGRAY, "Amm(%i) %i", sactor, actor[sactor].ammunition [slot]);

}


void display_actor_armour(int dactor)
{
   return;

   if (player[1].actor_controlled == dactor) display_player_armour(1);
   if (no_players == 2 && player[2].actor_controlled == dactor) display_player_armour(2);

}

void display_player_armour(int dplayer)
{

   return;

   if (no_players == 1)
   {
    display_armour(player[dplayer].actor_controlled, stat_x_pos, stat_y_pos, 1, stat_bar_width, stat_pix_per_point_1p);
   } else
   {
     display_armour(player[dplayer].actor_controlled, stat_x_pos + (window_grid_x_2p * (dplayer == 2)), stat_y_pos, 1, stat_bar_width, stat_pix_per_point_2p);
   }
   
}



void display_armour(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point)
{

 return;

 int arm;
 int arm_max;
 int arm_bar;
 unsigned char bar_colour = COLOUR_GREY5;

  arm = actor[sactor].armour * 100;
  arm_max = actor[sactor].max_armour;
  if (arm_max == 0) arm_bar = 0;
   else
    arm_bar = arm / arm_max;

 if (arm > 0) rectfill(screen, x_pos + bar_width * 2, y_pos + bar_width * 2 + 1, x_pos + bar_width * 2 + (pix_per_point * arm_bar), y_pos + (bar_width * 3) - 1, bar_colour);
 if (arm < arm_max * 100 || arm_max == 0) rectfill(screen, x_pos + bar_width * 2 + (pix_per_point * arm_bar), y_pos + 1 + bar_width * 2, x_pos + bar_width * 2 + (pix_per_point * 100), y_pos + (bar_width * 3) - 1, COLOUR_BLACK);


}

void display_actor_clips(int dactor)
{

   if (player[1].actor_controlled == dactor) display_player_clips(1);
   if (no_players == 2 && player[2].actor_controlled == dactor) display_player_clips(2);

}

void display_player_clips(int dplayer)
{

   if (no_players == 1)
   {
    display_clips(player[dplayer].actor_controlled, stat_x_pos, stat_y_pos, 1, stat_bar_width, stat_pix_per_point_1p, player[dplayer].slot_selected);
   } else
   {
     display_clips(player[dplayer].actor_controlled, stat_x_pos + (window_grid_x_2p * (dplayer == 2)), stat_y_pos, 1, stat_bar_width, stat_pix_per_point_2p, player[dplayer].slot_selected);
   }

}


void display_clips(int sactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int slot)
{

 if (game[0].unlimited_clips == 1)
  return;
  
 int clipc;

 for (clipc = 0; clipc < actor[sactor].max_clips; clipc++)
 {
  if (actor[sactor].clips [slot] > clipc && actor[sactor].weapon [slot] != WPN_NONE)
   rectfill(screen, x_pos + bar_width * 2 + (clipc * bar_width * 5), y_pos + bar_width * 5, x_pos + bar_width * 2 + (clipc * bar_width * 5) + (bar_width * 4), y_pos + bar_width * 6, COLOUR_YELLOW3);
    else
      rectfill(screen, x_pos + bar_width * 2 + (clipc * bar_width * 5), y_pos + bar_width * 5, x_pos + bar_width * 2 + (clipc * bar_width * 5) + (bar_width * 4), y_pos + bar_width * 6, COLOUR_BLACK);

 }

}


void update_score(void)
{

 char score_string [40];
 char it_string [40];
 int play = 1;
 text_mode(0);
 strcpy(score_string, "");

 while (play < no_players + 1)
 {
/*  if (videomode == 0)
   strcpy(score_string, "S ");
    else*/

  if (game[0].game_type != GAME_LAST_ONE)
  {
    strcpy(score_string, "     Score ");
//    strcpy(score_string, "    actor ");
    if (arena[0].teams == 0)
     strcat(score_string, itoa(user[player[play].user].score, it_string, 10));
      else
       strcat(score_string, itoa(team[user[player[play].user].team].score, it_string, 10));
  }
  
  if (game[0].lives_each > 0)
  {
/*   if (videomode == 0)
    strcpy(score_string, "L ");
     else*/
     strcat(score_string, "  Lives ");
//     strcat(score_string, "  hpx ");
    
   strcat(score_string, itoa(user[player[play].user].lives, it_string, 10));
/*
   strcat(score_string, " u.actor ");

   strcat(score_string, itoa(user[player[play].user].actor, it_string, 10));
   
   strcat(score_string, " p.actor ");

   strcat(score_string, itoa(player[play].actor_controlled, it_string, 10));
   
   strcat(score_string, " user ");

   strcat(score_string, itoa(player[play].user, it_string, 10));
*/
  }

  if (no_players == 2)
  {
   textprintf_right(screen, large_font, score_2p_x_pos + ((play == 2) * window_grid_x_2p), score_y_pos, COLOUR_GREY6, score_string);
  } else
     textprintf_right(screen, large_font, score_sp_x_pos, score_y_pos, COLOUR_GREY6, score_string);
  strcpy(score_string, "");
  
  play ++;
  
 }


 text_mode(-1);

 
}


void ingame_menu(int mplayer, BITMAP *bmp, int centre_x)
{

  if (game_over > 0
   || user[player[mplayer].user].out_of_lives == 1)
    return;


  int window_size = (text_size * 3) + text_size * (arena[0].ingame_slots + 2);
  int window_y = text_size * 2;
//  int border_colour =

  drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, 0, 0);
  rectfill(bmp, centre_x - 100, window_y, centre_x + 100, window_size + window_y, 0);
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  rect(bmp, centre_x - 100, window_y, centre_x + 100, window_size + window_y, 96);
  rect(bmp, centre_x - 99, window_y + 1, centre_x + 99, window_size + window_y - 1, 101);
  rect(bmp, centre_x - 98, window_y + 2, centre_x + 98, window_size + window_y - 2, 106);
  rect(bmp, centre_x - 97, window_y + 3, centre_x + 97, window_size + window_y - 3, 111);

//  textprintf_centre(bmp, large_font, centre_x, 100, WHITE, "You're Dead");

  int item = 0;
  int tcol = COLOUR_GREY8;
  char tstring [40];
  char itstring [10];
  
  do
  {
   tcol = COLOUR_GREY8;
   if (user[player[mplayer].user].penalty_time > 0) tcol = COLOUR_GREY5;
   text_mode(-1);
   if (player[mplayer].ingame_slot == item - 2)
    text_mode(COLOUR_BLUE4);
//    tcol = YELLOW;
   strcpy(tstring, "    ");
   switch(item)
   {
    case 0: strcpy(tstring, "  Pickup to Continue  "); break;
    case 1:
     if (user[player[mplayer].user].penalty_time > 0)
     {
      strcpy(tstring, "  In ");
      strcat(tstring, itoa(user[player[mplayer].user].penalty_time / 33, itstring, 10));
      strcat(tstring, "s  ");
      tcol = COLOUR_YELLOW4;
     } else
        {
         strcpy(tstring, "  ");
         strcat(tstring, itoa(user[player[mplayer].user].resting_time / 33, itstring, 10));
         strcat(tstring, "s left ");
         tcol = COLOUR_YELLOW4;
        }
     break;
   }
 switch(game[0].atypes_avail)
 {
  case 0:
   switch(item)
   {
    case 2: strcpy(tstring, "  Soldier  "); break;
    default: break;
   }
   break;// end all atypes
  case 1:
   switch(item)
   {
    case 2: strcpy(tstring, "  Soldier  "); break;
    case 3: strcpy(tstring, "  Scout  "); break;
    case 4: strcpy(tstring, "  Cyborg  "); break;
    default: break;
   }
   break;// end all atypes*/
  case 2:
   switch(item)
   {
    case 2: strcpy(tstring, "  Soldier  "); break;
    case 3: strcpy(tstring, "  Scout  "); break;
    case 4: strcpy(tstring, "  Cyborg  "); break;
    case 5: strcpy(tstring, "  Crusher  "); break;
    case 6: strcpy(tstring, "  Walker  "); break;
    case 7: strcpy(tstring, "  Bug  "); break;
    case 8: strcpy(tstring, "  Skeleton  "); break;
    case 9: strcpy(tstring, "  Demon  "); break;
    case 10: strcpy(tstring, "  Snail  "); break;
/*    case 4: strcpy(tstring, "  Heavy  "); break;
    case 5: strcpy(tstring, "  J-Light  "); break;
    case 6: strcpy(tstring, "  J-Medium  "); break;
    case 7: strcpy(tstring, "  J-Heavy  "); break;
    case 8: strcpy(tstring, "  R-Light  "); break;
    case 9: strcpy(tstring, "  R-Medium  "); break;
    case 10: strcpy(tstring, "  R-Heavy  "); break;*/
    default: break;

   }
   break;// end all atypes

 }
//  textprintf_centre(bmp, large_font, centre_x, 80 + item * text_size, WHITE, tstring);
/*  if (game[0].atypes_avail == 0 && (item == 2 || item >= 4))
   tcol = DARKGRAY;
  if (game[0].atypes_avail == 1 && item >= 5)
   tcol = DARKGRAY;*/
  textprintf_centre(bmp, large_font, centre_x, (text_size * 4) + item * text_size, tcol, tstring);

  item ++;

  } while (item < 12);

}




void ingame_scorelist(int mplayer, BITMAP *bmp, int centre_x)
{

  int i, no_scores = 0;

  if (arena[0].teams == 0)
  {
   for (i = 0; i < NO_USERS; i ++)
   {
    if (user[i].active != 0) no_scores ++;
   }
  }
   else
   {
    for (i = 0; i < NO_TEAMS; i ++)
    {
     if (team[i].active != 0) no_scores ++;
    }
   }

  int window_size = (text_size * 3) + text_size * (no_scores + 3);
  int window_y = text_size * 2;
  if (arena[0].teams == 0)
   drawing_mode(DRAW_MODE_MASKED_PATTERN, cloud_pattern, 1, 0);
  // it's too hard to see coloured letters
  rectfill(bmp, centre_x - 100, window_y, centre_x + 100, window_size + window_y, 0);
  drawing_mode(DRAW_MODE_SOLID, cloud_pattern, 0, 0);
  rect(bmp, centre_x - 100, window_y, centre_x + 100, window_size + window_y, 160);
  rect(bmp, centre_x - 99, window_y + 1, centre_x + 99, window_size + window_y - 1, 165);
  rect(bmp, centre_x - 98, window_y + 2, centre_x + 98, window_size + window_y - 2, 170);
  rect(bmp, centre_x - 97, window_y + 3, centre_x + 97, window_size + window_y - 3, 175);

//  textprintf_centre(bmp, large_font, centre_x, 100, WHITE, "You're Dead");

  int item = 2;
  int j = 0, last_user = -1;
  int tcol = COLOUR_YELLOW4;
  char tstring [40];
  char itstring [10];

  textprintf_centre(bmp, large_font, centre_x, (text_size * 4), tcol, "Scores");
  if (game[0].game_type != GAME_LAST_ONE) // ie the only one with no scores
   textprintf_centre(bmp, large_font, centre_x, (text_size * 5), tcol, "(%i to win)", game[0].score_limit);

  if (arena[0].teams == 0)
  {
   for (i = 0; i < no_scores; i ++)
   {
    for (j = last_user + 1; j < NO_USERS; j ++)
    {
     if (user[j].active == 1) break;
    }
    last_user = j;
    strcpy(tstring, user[j].uname);
    strcat(tstring, " - ");
    strcat(tstring, itoa(user[j].score, itstring, 10));
    if (game[0].lives_each != 0)
    {
     strcat(tstring, " (");
     strcat(tstring, itoa(user[j].lives, itstring, 10));
     strcat(tstring, ")");
    }
    textprintf_centre(bmp, large_font, centre_x, (text_size * 4) + item * text_size, tcol, tstring);
    item ++;
   }
  }
   else
    {
     last_user = -1;
     item = 2;
     for (i = 0; i < no_scores; i ++)
     {
      for (j = last_user + 1; j < NO_TEAMS; j ++)
      {
       if (team[j].active == 1) break;
      }
      last_user = j;
      strcpy(tstring, team_name(j));
      strcat(tstring, " - ");
      strcat(tstring, itoa(team[j].score, itstring, 10));
      textprintf_centre(bmp, large_font, centre_x, (text_size * 4) + item * text_size, team_colours(j), tstring);
      item ++;
     }
    }

}





void init_object_sprites(void)
{

   BITMAP *objects2;
   RGB temp_palette [256];

   objects2 = load_bitmap("object2.bmp", temp_palette);

   if (objects2 == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file object2.bmp");
      exit(1);
   }


   int i, j;
   int bz, bi, bj;

   for (i = 0; i < 6; i ++)
   {
    for (j = 0; j < 7; j ++)
    {
     rocket_bmp [i] [j] = create_bitmap(9, 9);
     clear_bitmap(rocket_bmp [i] [j]);
     if (rocket_bmp [i] [j] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create rocket bitmap (not enough memory?)");
        exit(1);
     }
     blit(objects2, rocket_bmp [i] [j], (j * 10) + 1, ((i + 6) * 10) + 1, 0, 0, 9, 9);
    }
   }

   for (i = 0; i < 6; i ++)
   {
    for (j = 0; j < 5; j ++)
    {
      if (i == 0)
       grapple_bmp [i] [j] = create_bitmap(9, 9);
        else
         grapple_bmp [i] [j] = create_bitmap(7, 7);
      clear_bitmap(grapple_bmp [i] [j]);
      if (grapple_bmp [i] [j] == NULL)
      {
         set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
         allegro_message("Fatal Error: Unable to create grapple bitmap (not enough memory?)");
         exit(1);
      }
      blit(objects2, grapple_bmp [i] [j], (j * 10) + 1, (i * 10) + 1, 0, 0, 8, 8);
//      apply_light_to_sprite(grapple_bmp [i] [j] [k], 9, 9, k);
    }
   }
   
   destroy_bitmap(objects2);

   BITMAP *objects3;

   objects3 = load_bitmap("object3.bmp", temp_palette);

   if (objects3 == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file object3.bmp");
      exit(1);
   }


   for (i = 0; i < 7; i ++)
   {
     bomb_bmp [i] = create_bitmap(4, 4);
     clear_bitmap(bomb_bmp [i]);
     if (bomb_bmp [i] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create rocket bitmap (not enough memory?)");
        exit(1);
     }
     blit(objects3, bomb_bmp [i], (i * 10) + 1, 1, 0, 0, 4, 4);
//     apply_light_to_sprite(bomb_bmp [i] [j], 4, 4, j);
   }

   destroy_bitmap(objects3);
   
   BITMAP *objects;

   objects = load_bitmap("objects.bmp", temp_palette);

   if (objects == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file objects.bmp");
      exit(1);
   }


   for (i = 0; i < 18; i ++)
   {
     pickup_bmp [i] = create_bitmap(9, 9);
     clear_bitmap(pickup_bmp [i]);
     if (pickup_bmp [i] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create pickup bitmap (not enough memory?)");
        exit(1);
     }
     blit(objects, pickup_bmp [i], (i * 10) + 1, 1, 0, 0, 9, 9);
     apply_light_to_sprite(pickup_bmp [i], 9, 9, j);
   }

   for (i = 18; i < 25; i ++)
   {
    pickup_bmp [i] = create_bitmap(9, 9);
    clear_bitmap(pickup_bmp [i]);
    draw_sprite_h_flip(pickup_bmp [i], pickup_bmp [i - PKBMP_RFLAG_RED + PKBMP_FLAG_RED], 0, 0);
   }

   destroy_bitmap(objects);



   objects = load_bitmap("object4.bmp", temp_palette);

   if (objects == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file object4.bmp");
      exit(1);
   }


   BITMAP *temp_bmp = create_bitmap(31, 31);
   if (temp_bmp == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Can't create temp base bitmap (out of memory?)");
      exit(1);
   }

   for (i = 0; i < 16; i ++)
   {
//     base_bmp [i] = create_bitmap(31, 31);
//     clear_bitmap(base_bmp [i]);
/*     if (base_bmp [i] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create base bitmap (not enough memory?)");
        exit(1);
     }*/
     blit(objects, temp_bmp, (i * 32) + 1, 1, 0, 0, 31, 31);
     base_bmp [i] = get_rle_sprite(temp_bmp);
     if (base_bmp [i] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create base rle sprite (not enough memory?)");
        exit(1);
     }
//     apply_light_to_sprite(base_bmp [i] [j], 31, 31, j);
   }

   destroy_bitmap(objects);
//   destroy_bitmap(temp_bmp); is used later for th_base_bmps

   objects = load_bitmap("fruits.bmp", temp_palette);

   if (objects == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file fruits.bmp");
      exit(1);
   }


   for (i = 0; i < 2; i ++)
   {
     lfruit_bmp [i] = create_bitmap(17, 17);
     clear_bitmap(lfruit_bmp [i]);
     if (lfruit_bmp [i] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create fruit bitmap (not enough memory?)");
        exit(1);
     }
     blit(objects, lfruit_bmp [i], (i * 18) + 1, 1, 0, 0, 17, 16);
//     apply_light_to_sprite(lfruit_bmp [i] [j], 31, 31, j);
   }
   for (i = 0; i < 3; i ++)
   {
     mfruit_bmp [i] = create_bitmap(13, 13);
     clear_bitmap(mfruit_bmp [i]);
     if (mfruit_bmp [i] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create fruit bitmap (not enough memory?)");
        exit(1);
     }
     blit(objects, mfruit_bmp [i], (i * 14) + 1, 20, 0, 0, 13, 12);
//     apply_light_to_sprite(mfruit_bmp [i] [j], 31, 31, j);
   }
   for (i = 0; i < 4; i ++)
   {
     sfruit_bmp [i] = create_bitmap(9, 9);
     clear_bitmap(sfruit_bmp [i]);
     if (sfruit_bmp [i] == NULL)
     {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Fatal Error: Unable to create fruit bitmap (not enough memory?)");
        exit(1);
     }
     blit(objects, sfruit_bmp [i], (i * 10) + 1, 34, 0, 0, 9, 8);
//     apply_light_to_sprite(sfruit_bmp [i] [j], 31, 31, j);
   }

   destroy_bitmap(objects);


/*DATAFILE *object_data;

   object_data = load_datafile("objects.dat");
   if (object_data == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file objects.dat");
      exit(1);
   }


   for (bz = 0; bz < 82; bz ++)
   {
   obj_bmp [bz] = object_data[bz].dat;
   for (bi = 0; bi < 32; bi ++)
   {
    for (bj = 0; bj < 32; bj ++)
    {
     if (getpixel(obj_bmp [bz], bi, bj) == 224) putpixel(obj_bmp [bz], bi, bj, 0);
     if (getpixel(obj_bmp [bz], bi, bj) == 112) putpixel(obj_bmp [bz], bi, bj, 31);
    }
   }
   }*/
   
DATAFILE *point_data;

   point_data = load_datafile("points.dat");
   if (point_data == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file points.dat");
      exit(1);
   }


   for (bz = 1; bz < 19; bz ++)
   {
   point_bmp [bz] = point_data[bz].dat;
   for (bi = 0; bi < 12; bi ++)
   {
    for (bj = 0; bj < 12; bj ++)
    {
     if (getpixel(point_bmp [bz], bi, bj) == 224) putpixel(point_bmp [bz], bi, bj, 0);
     if (getpixel(point_bmp [bz], bi, bj) == 112) putpixel(point_bmp [bz], bi, bj, 31);
    }
   }
   }

    BITMAP *temp_bmp2 = create_bitmap(31, 31);
    if (temp_bmp2 == NULL)
    {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Fatal Error: cannot create T&H base temp bitmap (not enough memory?)");
       exit(1);
    }

   clear_bitmap(temp_bmp2);
   draw_rle_sprite(temp_bmp2, base_bmp [BASEBMP_TH_MIDDLE], 0, 0);


 for (bz = 0; bz < 16; bz ++)
 {
   rotate_sprite(temp_bmp, temp_bmp2, 0, 0, itofix(bz * 4));

   th_base_bmp [bz] = get_rle_sprite(temp_bmp);
   clear_bitmap(temp_bmp);
    if (th_base_bmp [bz] == NULL)
    {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Fatal Error: cannot create T&H base rle sprite (not enough memory?)");
       exit(1);
    }
 }

 destroy_bitmap(temp_bmp);
 destroy_bitmap(temp_bmp2);
 
// Now let's set up some RLE sprites for the lightsourcing:

BITMAP *temp_circle;


for (i = 0; i < 100; i ++)
{
 temp_circle = create_bitmap((i * 2) + 1, (i * 2) + 1);
    if (temp_circle == NULL)
    {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Fatal Error: cannot create temp_circle bitmap (not enough memory?)");
       exit(1);
    }
 clear_bitmap(temp_circle);

 circlefill(temp_circle, i, i, i, LIGHT_1);
 if (i > 11)
   circlefill(temp_circle, i, i, i - 10, LIGHT_2);
 if (i > 21)
   circlefill(temp_circle, i, i, i - 20, LIGHT_3);
 if (i > 31)
   circlefill(temp_circle, i, i, i - 30, LIGHT_4);

 if (light_circle [i] != NULL)
  destroy_rle_sprite(light_circle [i]);

 light_circle [i] = get_rle_sprite(temp_circle);

 destroy_bitmap(temp_circle);
}

for (i = 0; i < 5; i ++)
{
 j = i + 50;

 temp_circle = create_bitmap((j * 2) + 1, (j * 2) + 1);
    if (temp_circle == NULL)
    {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Fatal Error: cannot create temp_circle bitmap (not enough memory?)");
       exit(1);
    }
 clear_bitmap(temp_circle);

 circlefill(temp_circle, j, j, j, DARK_1);
 if (j > 11)
   circlefill(temp_circle, j, j, j - 10, DARK_2);
 if (j > 21)
   circlefill(temp_circle, j, j, j - 20, DARK_3);
 if (j > 31)
   circlefill(temp_circle, j, j, j - 30, DARK_4);

 if (dark_circle [i] != NULL)
  destroy_rle_sprite(dark_circle [i]);

 dark_circle [i] = get_rle_sprite(temp_circle);

 destroy_bitmap(temp_circle);
}

}


void apply_light_to_sprite(BITMAP *bmp, int x, int y, int lit)
{

 int i, j, px;

 for (i = 0; i < x; i ++)
 {
  for (j = 0; j < y; j ++)
  {
   px = getpixel(bmp, i, j);
   px = adjust_lit_colour(px, lit);
   putpixel(bmp, i, j, px);
  }
 }
 

}



void distortion_mask(BITMAP *mask, BITMAP *bmp, int x, int y, int width, int height, int disp_x, int disp_y, int flip_x, int flip_y)
{

 int dx, dy;
 int ox = 0, oy = 0;
 if (flip_x == -1) ox = width - 1;
// if (flip_y == -1) oy = height;

 int output [80] [80];

 for (dx = 0; dx < 80; dx ++)
 {
  for (dy = 0; dy < 80; dy ++)
  {
   output [dx] [dy] = 0;
  }
 }

 for (dx = 0; dx < width + 5; dx ++)
 {
  for (dy = 0; dy < height + 5; dy ++)
  {
   if (getpixel(mask, ox, dy))
   {
    output [dx + 5] [dy + 5] = getpixel(bmp, ox + x + disp_x, dy + y + disp_y); // getpixel(mask, ox, dy);
   } else
    {
     output [dx + 5] [dy + 5] = getpixel(bmp, ox + x, dy + y);
    }
//         output [dx + 5] [dy + 5] = getpixel(bmp, ox + x, dy + y);
   oy += flip_y;
  }
  ox += flip_x;
 }

 for (dx = 0; dx < width + 5; dx ++)
 {
  for (dy = 0; dy < height + 5; dy ++)
  {
   if (output [dx] [dy])
   {
/*    if (output [dx] [dy] == 15)
     putpixel(bmp, dx + x - 5, dy + y - 5, 1);
      else*/
       putpixel(bmp, dx + x - 5, dy + y - 5, output [dx] [dy]);
   }
  }
 }


}


void cast_light(BITMAP *bmp, int lx, int ly, int lrad, int lit)
{

 int lighted = lit + LIGHT_1;
 if (lighted > LIGHT_4)
  lighted = LIGHT_4;
  
     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, lx, ly, lrad, lighted);

     drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);

}



void display_all_lights(int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp)
{

 int i;

 for (i = 0; i < NO_LIGHTS; i ++)
 {
  if (i == NO_LIGHTS - 1)
   return; // too many.
  if (light[i].level == 0)
   return;
  display_a_light(i, x_centre, y_centre, x_limit,
                  y_limit, window_width, bmp);
 }

}


void display_a_light(int dlight, int x_centre, int y_centre,
     int x_limit, int y_limit, int window_width, BITMAP *bmp)
{

  int put_x = (light[dlight].x / GRAIN) - x_centre + x_limit;
  if (put_x < 0 || put_x > window_width) return;
  
  int put_y = (light[dlight].y / GRAIN) - y_centre + y_limit;
  if (put_y < 0 || put_y > window_grid_y) return;

  int size;

  if (light[dlight].light_or_dark == 0)
  {
   size = light[dlight].size;
    if (size > 4)
    size = 4;
    if (size < 0)
    size = 0;

   draw_trans_rle_sprite(bmp, dark_circle [size], put_x - size - 50, put_y - size - 50);
   return;
  }

// int lighted = light[dlight].level + LIGHT_1 - 1;
// if (lighted > LIGHT_4)
//  lighted = LIGHT_4;

 size = light[dlight].size;
 if (size > 99)
  size = 99;

 draw_trans_rle_sprite(bmp, light_circle [size], put_x - size, put_y - size);
  
/*     drawing_mode(DRAW_MODE_TRANS, NULL, cloud_pattern_status, 0);
    circlefill(bmp, put_x, put_y, light[dlight].size, LIGHT_1);

 if (lighted > LIGHT_1 && light[dlight].size > 5)
    circlefill(bmp, put_x, put_y, light[dlight].size - 5, LIGHT_2);

 if (lighted > LIGHT_2 && light[dlight].size > 10)
    circlefill(bmp, put_x, put_y, light[dlight].size - 10, LIGHT_3);

 if (lighted > LIGHT_3 && light[dlight].size > 15)
    circlefill(bmp, put_x, put_y, light[dlight].size - 15, LIGHT_4);

     drawing_mode(DRAW_MODE_SOLID, NULL, cloud_pattern_status, 0);
*/

}

int cube_outer(int input)
{

  switch(input)
  {
   case TRANS_BLUE: return COLOUR_BLUE4;
   case TRANS_YELLOW: return COLOUR_YELLOW4;
   case TRANS_ORANGE: return COLOUR_RED4;
   case TRANS_RED: return COLOUR_RED4;
   case TRANS_GREEN: return COLOUR_GREEN4;
   default:
   case TRANS_WHITE: return COLOUR_GREY8;
  }

return COLOUR_GREY8;

}

// returns 1 if light at point is > 1 (very dim light doesn't count)
// Have to be careful when this function is called to make sure that
//  all relevant light sourcing & transparency has already been done.
int is_point_lit(BITMAP *bmp, int px, int py)
{

 int thing = getpixel(bmp, px, py);

 if (thing == 0)
  return 0;
 if (thing < 32)
  return 1;
 if (thing < 96)
 {
  if (thing % 8 > 1)
   return 1;
  return 0;
 }
 if (thing < 160)
 {
  return 0;
 }
 
 return 1;
 
}



void display_game_writing(BITMAP *bmp, int which_writing, int centre_x, int centre_y, int distance)
{

 char writing1 [10];
 char writing2 [10];
 char writing3 [3];

 int writing_colour = COLOUR_WRITING;

 if (which_writing == 0)
 {
  strcpy(writing1, " YOU");
  strcpy(writing2, " WIN");
  writing_colour = 222;//COLOUR_YELLOW1;
 }
   else
    {
     strcpy(writing1, "GAME");
     strcpy(writing2, "OVER");
     writing_colour = 223;//COLOUR_GREEN1;
    }

// writing_colour = pulse_colour(writing_colour, 4);

 int put_x, put_y, i;


 for (i = 0; i < 4; i ++)
 {
  put_x = centre_x;
  put_x += cos(((float) (255 - arena[0].counter) / 64) * PI + (float) i / 2) * distance;
  put_y = centre_y;
  put_y += sin(((float) (255 - arena[0].counter) / 64) * PI + (float) i / 2) * distance;

  writing3 [0] = writing1 [i];
  writing3 [1] = '\0';
  
  textprintf(bmp, large_font, put_x, put_y, writing_colour, writing3);

  put_x = centre_x;
  put_x += cos(((float) (255 - arena[0].counter) / 64) * PI + PI + (float) i / 2) * distance;
  put_y = centre_y;
  put_y += sin(((float) (255 - arena[0].counter) / 64) * PI + PI + (float) i / 2) * distance;

  writing3 [0] = writing2 [i];
  writing3 [1] = '\0';
  
  textprintf(bmp, large_font, put_x, put_y, writing_colour, writing3);
 }


}

// speed should be a factor of 255. The higher the speed, the slower.
int pulse_colour(int colour, int speed)
{

 int col = (arena[0].counter / speed) % 4;

 if (((arena[0].counter / speed) / 4) % 2 == 0)
  col = 3 - col;

 return colour + col;

}


