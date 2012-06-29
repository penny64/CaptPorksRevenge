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

File: menu.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - the whole awkward menu interface
 - saving config variables to the config file
 - the in-game menu for selection actor atype is in cmds.c and display.c.
 
*/

#include "allegro.h"
#include "libnet.h"

#include <string.h>
//#include <conio.h>

#include <math.h>
#include "config.h"
#include "globvar.h"

//#include "connect.h"
#include "menulist.h"
#include "cmds.h"
#include "sound.h"
#include "level.h"
#include "base.h"
#include "misc.h"
#include "chall.h"

#include "grabhead.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

extern volatile char ticked;


struct menu_format *cmenu;
struct menu_format *last_menu;

extern int window_x_sp;
extern BITMAP *captain_pork;
extern RGB palet [256];

extern struct ai_config_struct ai_config [MAX_AI];
extern DATAFILE *datf;


void print_menu_entry(struct menu_format *mentry, int mcount, int scrline, int colour);
void check_commands(char command_issued [3]);
void show_menu(void);
void toggle_option(int which_option, char tdir);
void enter_key(int which_option, int menu_offset, int menu_count);
void change_menu(int which_option);
char collate_keys(void);
void save_config(int which_option);
void save_game_config(int which_game);
void print_scroll_arrows(int menu_size);
int acceptable_char(int ascode);
void get_string(int which_option, int menu_offset, int menu_count);
void get_server_address(int which_option, int menu_offset, int menu_count);
void copy_gametype(int game_from, int game_to);
//void make_weapon_menu(void);
int player_choose_weapon(int which_option);
int cycle_value(int value, int min, int max, int cdir);

void quit_game(void);
unsigned char get_a_key(int menu_offset, int menu_count);
void get_number(int which_option, int number_change);
int selectable(int mcount);
int player_choose_atype(int which_option);
int player_choose_team(int which_option);
int ai_config_string(int which_option, char mstring [150], int colour);
int ai_editing(int which_option);
int get_next_colour(int input, int direction);
void append_colour_name(char mstring [100], int input);
void update_screen(void);
void run_menu_screen(void);
void menu_quickstart(int which_entry);
unsigned char menu_information_box(const char *text1, const char *text2, const char *text3, char waiting);

char *scancode_to_keyname(int scanc);

extern int video_mode;

//extern int level_colours;
//extern int level_style;
//extern int solid_density; // 0, 1, 4, 8 - the higher, the fewer solid things
// defined in level.c


int max_menu_entries; // given a value in proj.c
int line_length;
int menu_top;
int game_being_edited = 1;
int player_choosing = 1;
int slot_choosing = 0;
int player_weapons [3];

BITMAP *menu_screen;

int scanline_x, screen_width, screen_height;

unsigned char menu_information_box(const char *text1, const char *text2, const char *text3, char waiting)
{
 unsigned char retval = information_box(text1, text2, text3, waiting);
 ticked = 0;
 return retval;

}

void run_menus(int starting_menu)
{

// cmenu = custom_menu;
// make_weapon_menu();

// if (menu_screen == NULL)
 {
  if (video_mode == 0)
  {
    menu_screen = create_bitmap(320, 200);
    screen_width = 320;
    screen_height = 200;
  }
  else if (video_mode == 1)
  {
    menu_screen = create_bitmap(640, 480);
    screen_width = 640;
    screen_height = 480;
  }
  else if (video_mode == 2)
  {
    menu_screen = create_bitmap(800, 600);
    screen_width = 800;
    screen_height = 600;
  }
 }


 cmenu = main_menu;
 last_menu = main_menu;

 if (starting_menu == 1)
  cmenu = weapon_menu;

 player_choosing = 1;
 slot_choosing = 0;

 scanline_x = 0;
 ticked = 0;

 do
 {
 show_menu();
 } while(cmenu != NULL);

 clear_bitmap(screen);

 destroy_bitmap(menu_screen);

}

char* my_my_itoa(int value, char* str, int radix) {
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


void print_a_menu(int offset, int mcount, int colour)
{

char ststring [120];
int stc;

//   print_menu_entry(cmenu, menu_count, 1); //palette_color [108]);
   if (video_mode > 0)
   {
     draw_sprite(menu_screen, captain_pork, 20, 1);
   } else textprintf_centre(menu_screen, large_font, window_x_sp / 2, 5, COLOUR_BLUE4, "Captain Pork's Revenge");

// subtitle:
   if (cmenu == weapon_menu || cmenu == team_menu)
   {
    if (player_choosing == 1) strcpy(ststring, player[1].pname);
     else strcpy(ststring, player[2].pname);
// strcat(ststring, " - ");
    for (stc = 0; stc < 4; stc ++)
    {
     if (stc >= player_weapons [player_choosing]) break;
     strcat(ststring, " - ");
     if (player[player_choosing].weapon [stc] == WPN_NONE)
     {
      strcat(ststring, "(empty)");
     } else strcat(ststring, wlist[player[player_choosing].weapon [stc]].long_name);
    }
    textprintf_right(menu_screen, large_font, window_x_sp - 25, 5 + line_length + (85 * (video_mode == 1)), COLOUR_YELLOW4, ststring);

   }
   if (cmenu == atype_menu)
   {
    if (player_choosing == 1) strcpy(ststring, player[1].pname);
     else strcpy(ststring, player[2].pname);
    textprintf_centre(menu_screen, large_font, window_x_sp / 2, 5 + line_length + (55 * (video_mode == 1)), COLOUR_YELLOW4, ststring);

   }

 switch(video_mode)
 {
  case 0: line_length = 14; menu_top = 50; break;
  case 1: line_length = 20; menu_top = 140; break;
 }

 int menu_count = offset;
 int menu_size = 0;
 char scroll_arrows;

 do
 {
  if (menu_count == mcount)
  {
   text_mode(1);
   print_menu_entry(cmenu, menu_count, menu_count - offset, colour); //104);
   text_mode(-1);
  }
    else
    {
     if (selectable(menu_count) == 0)
     {
      text_mode(COLOUR_YELLOW4);
      print_menu_entry(cmenu, menu_count, menu_count - offset, COLOUR_BLUE1); //108);
      text_mode(-1);
     }
       else
        if (selectable(menu_count) == -1)
         print_menu_entry(cmenu, menu_count, menu_count - offset, COLOUR_GREY4); //108);
          else
           print_menu_entry(cmenu, menu_count, menu_count - offset, COLOUR_GREEN2); //108);
    }
  menu_count++;
  menu_size++;
  if (menu_size > max_menu_entries)
  {
   if (cmenu[menu_count + 1].entry_index != ENTRY_NULL
       || offset > 0)
    scroll_arrows = 1;
   break;
  }
 } while(cmenu[menu_count].entry_index != ENTRY_NULL);

 if (scroll_arrows)
  print_scroll_arrows(menu_size);

 char temp_string [10] [100];

 int i, xpos = 0, ypos = 0;

  for (i = 0; i < 5; i ++)
  {
   strcpy(temp_string [i], "");
  }

 if (video_mode == 1)
 {
  for (i = 0; i < strlen(cmenu[mcount].entry_description); i ++)
  {
   if (cmenu[mcount].entry_description [i] == '$')
   {
    ypos ++;
    xpos = 0;
    continue;
   }
   temp_string [ypos] [xpos] = cmenu[mcount].entry_description [i];
   temp_string [ypos] [xpos + 1] = '\0';
   xpos ++;
   if (xpos > 40 && cmenu[mcount].entry_description [i] == ' ')
   {
    ypos ++;
    xpos = 0;
   }
  }

  for (i = 0; i < ypos + 1; i ++)
  {
   textprintf_right(menu_screen, large_font, 600, 200 + i * 20, COLOUR_YELLOW4, temp_string [i]);
  }
 }

/*
 if (cmenu == player_menu)
 {
    RGB *colour_1 [2];

    RGB col1;
    colour_1 [0] = &col1;

    colour_1 [0]->r = player[1].colour_r;
    colour_1 [0]->g = player[1].colour_g;
    colour_1 [0]->b = player[1].colour_b;

//    set_color(224, colour_1 [0]);
    
    colour_1 [1] = &col1;
    
    colour_1 [1]->r = player[2].colour_r;
    colour_1 [1]->g = player[2].colour_g;
    colour_1 [1]->b = player[2].colour_b;

/    set_color(225, colour_1 [1]);

    if (video_mode == 0)
    {
      rectfill(screen, 50, 50, 60, 60, 224);
      rectfill(screen, 270, 50, 280, 60, 225);
    } else
     {
      rectfill(screen, 140, 110, 150, 120, 224);
      rectfill(screen, 500, 110, 510, 120, 225);
     }
 }
*/
}

// assumes is already in correct graphics mode
void show_menu(void)
{

clear_to_color(menu_screen, 0);


int menu_count = 0;
int escape = 0;
//char moved_down = 0;
//char moved_up = 0;
char command_issued [3] = {0, 0, 0};
int menu_size = 0;
int menu_offset = 0;

int i;
char switch_to = 1;


// assumes menu has at least 1 valid, non-NULL member:
do
{

// print_menu_entry(cmenu, menu_count, 1);

 menu_count++;
 menu_size++;
} while(cmenu[menu_count].entry_index != ENTRY_NULL);

menu_count = 0;
while (selectable(menu_count) <= 0)
{
 if (menu_count == max_menu_entries + menu_offset) menu_offset ++;
 menu_count ++;
//if (cmenu[menu_count].entry_index == ENTRY_TITLE) menu_count = 1;
}

if (menu_count > 6)
 menu_offset += 5;
// if we're past the first screen or so, centre the current selected item.

do
{


   print_a_menu(menu_offset, menu_count, COLOUR_GREEN4); //palette_color [108]);
//   update_screen();
//   run_menu_screen();
   
   if (keypressed())
      check_commands(command_issued);

   if (command_issued [0] == -1)
   {
    play_sound(WAV_SHORT_BLIP);
    do
    {
     if (menu_count == 0)
     {
      menu_count = menu_size - 1;
      menu_offset = menu_size - max_menu_entries - 1;
      if (menu_offset < 0) menu_offset = 0;
     }
      else
         {
          if (menu_count == menu_offset) menu_offset --;
          menu_count --;
         }
    } while (selectable(menu_count) <= 0); //cmenu[menu_count].entry_index == ENTRY_TITLE);
   }
   if (command_issued [0] == 1)
   {
    play_sound(WAV_SHORT_BLIP);
    do
    {
     if (menu_count == menu_size - 1)
     {
      menu_count = 0;
      menu_offset = 0;
     }
      else
         {
          if (menu_count == max_menu_entries + menu_offset) menu_offset ++;
          menu_count ++;
         }
    } while (selectable(menu_count) <= 0); //cmenu[menu_count].entry_index == ENTRY_TITLE);
   }
   
   if (command_issued [1] == -1 && cmenu[menu_count].response_type == RESPONSE_NUMBER)
     get_number(cmenu[menu_count].entry_index, -1);
   
   if (command_issued [1] == 1 && cmenu[menu_count].response_type == RESPONSE_NUMBER)
     get_number(cmenu[menu_count].entry_index, 1);

   if (command_issued [1] == -1 && (cmenu[menu_count].response_type == RESPONSE_CYCLE || cmenu[menu_count].response_type == RESPONSE_CYCLE_WEAPON || cmenu[menu_count].response_type == RESPONSE_CYCLE_EQUIP))
     toggle_option(cmenu[menu_count].entry_index, -1);
   
   if (command_issued [1] == 1 && (cmenu[menu_count].response_type == RESPONSE_CYCLE || cmenu[menu_count].response_type == RESPONSE_CYCLE_WEAPON || cmenu[menu_count].response_type == RESPONSE_CYCLE_EQUIP))
     toggle_option(cmenu[menu_count].entry_index, 1);

     
/*   if (command_issued [2] == KEY_PGUP || command_issued [2] == KEY_9_PAD)
   {

   }*/
     
   if (command_issued [2] == KEY_ESC)
   {
    menu_count = 0;
    while(cmenu [menu_count].special != SPECIAL_ESCAPE)
    {
     menu_count ++;
    }
    command_issued [2] = KEY_ENTER;
   }

   
   if (command_issued [2] == KEY_ENTER || command_issued [2] == KEY_ENTER_PAD)
   {
    play_sound(WAV_SELECT2);
    switch(cmenu[menu_count].response_type)
    {
     case RESPONSE_WEAPON_SET:
     switch(cmenu[menu_count].entry_index)
     {
      case ENTRY_WEAPON_SET_NONE:
       weapon_set(game_being_edited, 0); break;
      case ENTRY_WEAPON_SET_BASIC:
       weapon_set(game_being_edited, 1); break;
      case ENTRY_WEAPON_SET_EXTENDED:
       weapon_set(game_being_edited, 2); break;
      case ENTRY_WEAPON_SET_FULL:
       weapon_set(game_being_edited, 3); break;
     }
     break;
     case RESPONSE_CYCLE:
     toggle_option(cmenu[menu_count].entry_index, 1);
     break;
     case RESPONSE_START_GAME:
   //   if (serial[0].game_type != SERIAL_LOCAL_ONLY)
   //   {
   //     serial_game_init();
   //     conn_to_channel();
   //   }
          player_weapons[1] = 1;
          player_weapons[2] = 1;
           if (game[0].starting_weapons == 2)
           {
            if (game[0].atypes_avail == 0)
            {
             player_weapons[1] = 3;
             player_weapons[2] = 3;
            } else
             {
              player_weapons[1] = 4;
              player_weapons[2] = 4;
             }
           }
/*      if (arena[0].teams == 1 || game[0].game_type == GAME_CAP_FLAG || game[0].game_type == GAME_PORKBALL || game[0].game_type == GAME_GRAIL || game[0].game_type == GAME_TAKEHOLD)
       cmenu = team_menu;
        else //cmenu = atype_menu;*/
       if (game[0].starting_weapons == 0
           || game[0].starting_weapons == 3)
            cmenu = NULL;
        else
         {
          cmenu = weapon_menu;
         }
      return;
     case RESPONSE_QUICKSTART:
          menu_quickstart(cmenu[menu_count].entry_index);
          return;
  //   case RESPONSE_SHUTDOWN_CONNECT:
  //        //shutdown_network();
  //        break;
     case RESPONSE_CHOOSE_ATYPE:
      if (player_choose_atype(cmenu[menu_count].entry_index))
      {
       if (no_players == 2 && player_choosing == 1)
       {
        player_choosing = 2;
        return;
       }
       player_choosing = 1;
       cmenu = weapon_menu;
       return;
      }
     break;
     case RESPONSE_CHOOSE_TEAM:
      if (player_choose_team(cmenu[menu_count].entry_index))
      {
       if (no_players == 2 && player_choosing == 1)
       {
        player_choosing = 2;
        return;
       }
       player_choosing = 1;
//       cmenu = atype_menu;
       if (game[0].starting_weapons == 0) cmenu = NULL;
        else
         {
          cmenu = weapon_menu;
         }
       return;
      }
     break;
     case RESPONSE_FINISH_CHOICE:
      if (slot_choosing == player_weapons [player_choosing] - 1)
      {
       if (no_players == 2 && player_choosing == 1)
       {
        player_choosing = 2;
        slot_choosing = 0;
        cmenu = weapon_menu;
        return;
       }
       cmenu = NULL;
       return;
      }
      slot_choosing ++;
      return;
     case RESPONSE_QUIT_GAME:
     save_config(cmenu[menu_count].entry_index);
     quit_game();
     case RESPONSE_CHANGE_MENU:
     save_config(cmenu[menu_count].entry_index);
     last_menu = cmenu;
     change_menu(menu_count); return;
     case RESPONSE_RETURN_MENU:
     save_config(cmenu[menu_count].entry_index);
     cmenu = last_menu; return;
     case RESPONSE_FINISH_CUSTOM:
     save_config(cmenu[menu_count].entry_index);
      cmenu = last_menu;
      switch(game_being_edited)
      {
       case 1: copy_gametype(1, 0); break;
       case 2: copy_gametype(2, 0); break;
       case 3: copy_gametype(3, 0); break;
       case 4: copy_gametype(4, 0); break;
       case 5: copy_gametype(5, 0); break;
      }
      return;
     case RESPONSE_ENTER_KEY:
//     print_menu_entry(cmenu, menu_count, 3);//palette_color [100]);
      enter_key(cmenu[menu_count].entry_index, menu_offset, menu_count); break;
/*     case RESPONSE_SAVE_CONFIG:
     save_config(cmenu[menu_count].entry_index);
     break;*/
     case RESPONSE_ENTER_STRING:
      get_string(cmenu[menu_count].entry_index, menu_offset, menu_count);
      break;
     case RESPONSE_ENTER_SERVER_ADDRESS:
      get_server_address(cmenu[menu_count].entry_index, menu_offset, menu_count);
      break;
   //  case RESPONSE_CONNECT:
   ////   if (serial[0].initialised == 0)
   ////    init_serial_port(serial[0].com_port);
   //   switch(serial[0].game_type)
   //   {
   //    case SERIAL_SERVER:
   //    server_get_connections();
   //    break;
   //    case SERIAL_CLIENT:
   //    if (client_connect() == -1)
   //    {
   //     rest(1000);
   //     break;
   //    }
   //    rest(1000);
   //    menu_information_box("Connection Established.", " ", "Waiting for server to start...", 0);
   //    serial_game_init();
   //    player_weapons[1] = 1;
   //    player_weapons[2] = 1;
   //    if (game[0].starting_weapons == 2) player_weapons[1] = 4;
   //    if (game[0].starting_weapons == 2) player_weapons[2] = 4;
   //    if (game[0].starting_weapons == 0
   //        || game[0].starting_weapons == 3)
   //         cmenu = NULL;
   //          else
   //           cmenu = weapon_menu;
   //    return;
   //   }
   //   break;
//     case RESPONSE_INIT_CONNECT:
//      break;
      
     case RESPONSE_CHOOSE_WEAPON:
      if (player_choose_weapon(cmenu[menu_count].entry_index))
      {

      if (slot_choosing == player_weapons [player_choosing] - 1)
      {
       if (no_players == 2 && player_choosing == 1)
       {
        player_choosing = 2;
        slot_choosing = 0;
        cmenu = weapon_menu;
        return;
       }
       cmenu = NULL;
       return;
      }
      slot_choosing ++;
      cmenu = weapon_menu;
      return;

     case RESPONSE_SELECT_ALL_AI:
      switch_to = 1;
      for (i = 0; i < 7; i ++)
      {
       if (ai_config[i].active == 1) switch_to = 0;
      }
      for (i = 0; i < 7; i ++)
      {
       ai_config[i].active = switch_to;
      }
      break;
     case RESPONSE_TEST_STEREO: // Horrible.
      if (options[0].positional_sound == 0)
      {
       play_sound2(WAV_RECTBANG, 1000, 255, 127);
       menu_information_box("Stereo Test", "Left Speaker.", "", 1);
       play_sound2(WAV_RECTBANG, 1000, 255, 127);
       menu_information_box("Stereo Test", "Both Speakers.", "", 1);
       play_sound2(WAV_RECTBANG, 1000, 255, 127);
       menu_information_box("Stereo Test", "Right Speaker.", "", 1);
      }
      if (options[0].positional_sound == 2)
      {
       play_sound2(WAV_RECTBANG, 1000, 255, 0);
       menu_information_box("Stereo Test", "Left Speaker.", "", 1);
       play_sound2(WAV_RECTBANG, 1000, 255, 127);
       menu_information_box("Stereo Test", "Both Speakers.", "", 1);
       play_sound2(WAV_RECTBANG, 1000, 255, 255);
       menu_information_box("Stereo Test", "Right Speaker.", "", 1);
      }
      if (options[0].positional_sound == 1)
      {
       play_sound2(WAV_RECTBANG, 1000, 255, 255);
       menu_information_box("Stereo Test", "Left Speaker.", "", 1);
       play_sound2(WAV_RECTBANG, 1000, 255, 127);
       menu_information_box("Stereo Test", "Both Speakers.", "", 1);
       play_sound2(WAV_RECTBANG, 1000, 255, 0);
       menu_information_box("Stereo Test", "Right Speaker.", "", 1);
      }
      break;

/*
       if (no_players == 2 && player_choosing == 1)
       {
        player_choosing = 2;
        return;
       }
       cmenu = NULL;
       return;*/
      }
      break;

    }
   }
   
   do
   {
//   print_menu_entry(cmenu, menu_count, 2); //palette_color [104]);
   print_a_menu(menu_offset, menu_count, COLOUR_GREEN4); //palette_color [108]);
//   update_screen();
   
//    clear_keybuf();
    rest(1);
    run_menu_screen();
   } while (keypressed() == 0);
//   run_menu_screen();

} while (escape == 0);

}

int selectable(int mcount)
{
 if (cmenu[mcount].entry_index == ENTRY_TITLE) return 0;
 if (cmenu[mcount].response_type == RESPONSE_CHOOSE_WEAPON)
 {
  if (cmenu[mcount].entry_index > ENTRY_END_WEAPONS)
  {
     if (game[0].equip_status [cmenu[mcount].entry_index - ENTRY_EQUIP_1] < 1 || game[0].equip_status [cmenu[mcount].entry_index - ENTRY_EQUIP_1] > 3)
      return -1;
  } else
   {
     if (game[0].weapon_status [cmenu[mcount].entry_index - ENTRY_WEAPON_1 + 2 + NO_EQUIP] < 1 || game[0].weapon_status [cmenu[mcount].entry_index - ENTRY_WEAPON_1 + 2 + NO_EQUIP] > 3)
      return -1;
   }
 }
   
 return 1;
}


void menu_quickstart(int which_entry)
{

   switch(which_entry)
   {
       case ENTRY_CHALLENGE:
          //shutdown_network();
          arena[0].challenge_level = 1;
          player_weapons[1] = 1;
          no_players = 1;
          // must set up challenge here for the weapon menu
          prepare_challenge_game();
          setup_challenge_level();
          cmenu = weapon_menu;
          return;
// Note return here.

       case ENTRY_MELEE:
          arena[0].qstart = QSTART_MELEE;
          break;
       case ENTRY_MELEE_TEAM:
          arena[0].qstart = QSTART_MELEE_TEAM;
          break;
       case ENTRY_DUEL:
          arena[0].qstart = QSTART_DUEL;
          break;
       case ENTRY_DUEL_TEAM:
          arena[0].qstart = QSTART_DUEL_TEAM;
          break;
       case ENTRY_CAPFLAG:
          arena[0].qstart = QSTART_CAPFLAG;
          break;
       case ENTRY_FRUIT:
          arena[0].qstart = QSTART_FRUIT;
          break;
       case ENTRY_TAKEHOLD:
          arena[0].qstart = QSTART_TAKEHOLD;
          break;
       case ENTRY_LAST_ONE:
          arena[0].qstart = QSTART_LAST_ONE;
          break;
       case ENTRY_GRAIL:
          arena[0].qstart = QSTART_GRAIL;
          break;
   }

//          shutdown_network(); // ???????????????????
          prepare_qstart_game(arena[0].qstart);
          setup_qstart_level(arena[0].qstart, (serial[0].game_type != SERIAL_LOCAL_ONLY));
          player_weapons[1] = 1;
          player_weapons[2] = 1;
          game[0].starting_weapons = 1;
          if (arena[0].quickstart_weapons > 3) // excepts 7 later
          {
           player_weapons[1] = 4;
           player_weapons[2] = 4;
           game[0].starting_weapons = 2;
          }
//          no_players = 1;
          if (arena[0].quickstart_weapons == 0
              || arena[0].quickstart_weapons == 7)
          {
           player_weapons[1] = 0;
           player_weapons[2] = 0;
           game[0].starting_weapons = 3;
           cmenu = NULL;
          } else
           cmenu = weapon_menu;
           
    //      if (serial[0].game_type != SERIAL_LOCAL_ONLY)
    //      {
    //        serial_game_init();
    //        conn_to_channel();
    //      }


}


void save_config(int which_option)
{
 char miscstring [20];
 char wstring [20];
 char itstring [20];
 int wcount;

 int which_menu = ENTRY_NULL;

 if (cmenu == options_menu)
  which_menu = ENTRY_SAVE_OPTIONS;
// if (cmenu == serial_menu)
//  which_menu = ENTRY_SAVE_OPTIONS;
 if (cmenu == player_menu)
  which_menu = ENTRY_SAVE_PLAYERS;
 if (cmenu == custom_menu)
  which_menu = ENTRY_SAVE_GAME;// assumes game_being_edited is set
 if (cmenu == define_keys)
  which_menu = ENTRY_SAVE_KEYS;
 if (cmenu == ai_menu)
  which_menu = ENTRY_SAVE_AI_CONFIG;
 if (cmenu == level_menu)
  which_menu = ENTRY_SAVE_LEVEL_CONFIG;

 // why be picky? It doesn't actually matter where this is done.
 set_config_int("Quickstart", "quickstart_difficulty", arena[0].quickstart_difficulty);
 set_config_int("Quickstart", "quickstart_dirt", arena[0].quickstart_dirt);
 set_config_int("Quickstart", "quickstart_lightsourced", arena[0].quickstart_lightsourced);
 set_config_int("Quickstart", "quickstart_weapons", arena[0].quickstart_weapons);

 set_config_string("Network", "port_string", serial[0].port_string);
 set_config_int("Network", "com_port", serial[0].com_port);

 switch(which_menu)
 {
  default:
  case ENTRY_NULL:
  break;
 
  case ENTRY_SAVE_OPTIONS:
  set_config_int("Options", "run_vsync", options[0].run_vsync);
  set_config_int("Options", "fuzzy_menu", options[0].fuzzy_menu);
  set_config_int("Options", "stipple_clouds", options[0].stipple_clouds);
  set_config_int("Options", "show_fps", options[0].show_fps);
  set_config_int("Options", "positional_sound", options[0].positional_sound);
  set_config_int("Options", "display_damage", options[0].display_damage);
  break;
  case ENTRY_SAVE_KEYS:
  set_config_int("KeyDef", "CMD_KEY_UP1", cmd_key [CMD_KEY_UP1]);
  set_config_int("KeyDef", "CMD_KEY_DOWN1", cmd_key [CMD_KEY_DOWN1]);
  set_config_int("KeyDef", "CMD_KEY_LEFT1", cmd_key [CMD_KEY_LEFT1]);
  set_config_int("KeyDef", "CMD_KEY_RIGHT1", cmd_key [CMD_KEY_RIGHT1]);
  set_config_int("KeyDef", "CMD_KEY_SHOOT1", cmd_key [CMD_KEY_SHOOT1]);
  set_config_int("KeyDef", "CMD_KEY_CHANGE1", cmd_key [CMD_KEY_CHANGE1]);
  set_config_int("KeyDef", "CMD_KEY_JUMP1", cmd_key [CMD_KEY_JUMP1]);
  set_config_int("KeyDef", "CMD_KEY_SCORE1", cmd_key [CMD_KEY_SCORE1]);
  set_config_int("KeyDef", "CMD_KEY_NAMES1", cmd_key [CMD_KEY_NAMES1]);
  set_config_int("KeyDef", "CMD_KEY_GRAPPLE1", cmd_key [CMD_KEY_GRAPPLE1]);
  set_config_int("KeyDef", "CMD_KEY_UP2", cmd_key [CMD_KEY_UP2]);
  set_config_int("KeyDef", "CMD_KEY_DOWN2", cmd_key [CMD_KEY_DOWN2]);
  set_config_int("KeyDef", "CMD_KEY_LEFT2", cmd_key [CMD_KEY_LEFT2]);
  set_config_int("KeyDef", "CMD_KEY_RIGHT2", cmd_key [CMD_KEY_RIGHT2]);
  set_config_int("KeyDef", "CMD_KEY_SHOOT2", cmd_key [CMD_KEY_SHOOT2]);
  set_config_int("KeyDef", "CMD_KEY_CHANGE2", cmd_key [CMD_KEY_CHANGE2]);
  set_config_int("KeyDef", "CMD_KEY_JUMP2", cmd_key [CMD_KEY_JUMP2]);
  set_config_int("KeyDef", "CMD_KEY_SCORE2", cmd_key [CMD_KEY_SCORE2]);
  set_config_int("KeyDef", "CMD_KEY_NAMES2", cmd_key [CMD_KEY_NAMES2]);
  set_config_int("KeyDef", "CMD_KEY_GRAPPLE2", cmd_key [CMD_KEY_GRAPPLE2]);
  break;
  case ENTRY_SAVE_PLAYERS:
  set_config_string("Player1", "Name", player[1].pname);
  set_config_int("Player1", "Colour1", player[1].colour1);
  set_config_int("Player1", "Colour2", player[1].colour2);
//  set_config_int("Player1", "Blue", player[1].colour_b);
  set_config_int("Player1", "Handicap", player[1].handicap);
  set_config_int("Player1", "Team", player[1].team);
  set_config_int("Player1", "Soldier", player[1].soldier);
  set_config_string("Player2", "Name", player[2].pname);
  set_config_int("Player2", "Colour1", player[2].colour1);
  set_config_int("Player2", "Colour2", player[2].colour2);
//  set_config_int("Player2", "Blue", player[2].colour_b);
  set_config_int("Player2", "Handicap", player[2].handicap);
  set_config_int("Player2", "Team", player[2].team);
  set_config_int("Player2", "Soldier", player[2].soldier);
  break;
  case ENTRY_SAVE_GAME:
  strcpy(miscstring, "Game");
  strcat(miscstring, my_itoa(game_being_edited, itstring, 10));
  set_config_string(miscstring, "Name", game[game_being_edited].gname);
  set_config_int(miscstring, "gravity", game[game_being_edited].gravity);
  set_config_int(miscstring, "health_amount", game[game_being_edited].health_amount);
  set_config_int(miscstring, "reload_time", game[game_being_edited].reload_time);
  set_config_int(miscstring, "impact_damage", game[game_being_edited].impact_damage);
  set_config_int(miscstring, "bullet_speed", game[game_being_edited].bullet_speed);
  set_config_int(miscstring, "game_type", game[game_being_edited].game_type);
  set_config_int(miscstring, "score_type", game[game_being_edited].score_type);
  set_config_int(miscstring, "score_limit", game[game_being_edited].score_limit);
  set_config_int(miscstring, "lives_type", game[game_being_edited].lives_type);
  set_config_int(miscstring, "lives_each", game[game_being_edited].lives_each);
//  set_config_int(miscstring, "max_x", game[game_being_edited].max_x);
//  set_config_int(miscstring, "max_y", game[game_being_edited].max_y);
  set_config_int(miscstring, "pk_number", game[game_being_edited].pk_number);
  set_config_int(miscstring, "pk_time_between", game[game_being_edited].pk_time_between);
  set_config_int(miscstring, "lightsourcing", game[game_being_edited].lightsourcing);
  set_config_int(miscstring, "pk_health", game[game_being_edited].pk_health);
  set_config_int(miscstring, "pk_weapon", game[game_being_edited].pk_weapon);
  set_config_int(miscstring, "pk_equip", game[game_being_edited].pk_equip);
  set_config_int(miscstring, "pk_named", game[game_being_edited].name_boxes);
  set_config_int(miscstring, "fruit_no", game[game_being_edited].fruit_no);
  set_config_int(miscstring, "penalty", game[game_being_edited].penalty);
  set_config_int(miscstring, "th_base_no", game[game_being_edited].th_base_no);
  set_config_int(miscstring, "starting_weapons", game[game_being_edited].starting_weapons);
  set_config_int(miscstring, "bullets_explode", game[game_being_edited].bullets_explode);
  set_config_int(miscstring, "fast_bullets", game[game_being_edited].fast_bullets);
  set_config_int(miscstring, "soft_dirt", game[game_being_edited].soft_dirt);
  set_config_int(miscstring, "rechoose_atype", game[game_being_edited].rechoose_atype);
  set_config_int(miscstring, "unlimited_clips", game[game_being_edited].unlimited_clips);
  set_config_int(miscstring, "show_map", game[game_being_edited].show_map);
  set_config_int(miscstring, "blast_bullets", game[game_being_edited].blast_bullets);
  set_config_int(miscstring, "atypes_avail", game[game_being_edited].atypes_avail);

  for (wcount = 0; wcount < NO_WEAPONS; wcount ++)
  {
   strcpy(wstring, "Wpn");
   strcat(wstring, my_itoa(wcount, itstring, 10));
   set_config_int(miscstring, wstring, game[game_being_edited].weapon_status [wcount]);
  }

  for (wcount = 0; wcount < NO_EQUIP; wcount ++)
  {
   strcpy(wstring, "Eqp");
   strcat(wstring, my_itoa(wcount, itstring, 10));
   set_config_int(miscstring, wstring, game[game_being_edited].equip_status [wcount]);
  }

  
//  set_config_int(miscstring, "", game[game_being_edited].);
  break;
  case ENTRY_SAVE_AI_CONFIG:
  for (wcount = 0; wcount < MAX_AI; wcount ++)
  {
   strcpy(miscstring, "AI-");
   strcat(miscstring, my_itoa(wcount, itstring, 10));
   set_config_string(miscstring, "Name", ai_config[wcount].name);
   set_config_int(miscstring, "Active", ai_config[wcount].active);
   set_config_int(miscstring, "Skill", ai_config[wcount].skill);
   set_config_int(miscstring, "Team", ai_config[wcount].team);
   set_config_int(miscstring, "Type", ai_config[wcount].atype);
   set_config_int(miscstring, "Handicap", ai_config[wcount].handicap);
   set_config_int(miscstring, "Soldier", ai_config[wcount].soldier);
  }
  set_config_int("AI-General", "Impact", arena[0].ai_no_impact);
  set_config_int("AI-General", "Clips", arena[0].ai_unlimited_clips);
  set_config_int("AI-General", "Weapons", arena[0].ai_weapons);
  break;
  case ENTRY_SAVE_LEVEL_CONFIG:
  set_config_int("Level", "Colours", arena[0].level_colours);
  set_config_int("Level", "Style", arena[0].level_style);
  set_config_int("Level", "Solids", arena[0].solid_density);
  set_config_int("Level", "Redirt", arena[0].replace_dirt);
  set_config_int("Level", "Dirty", arena[0].dirt_fill);
  set_config_int("Level", "Ambience", arena[0].ambient_light);
  set_config_int("Level", "Lamps", arena[0].lamp_amount);
  set_config_int("Level", "Max_x", arena[0].max_x);
  set_config_int("Level", "Max_y", arena[0].max_y);
  set_config_string("Level", "Loaded", arena[0].level_loaded);
  break;
 }
}

int get_next_colour(int input, int direction)
{
 int i, j;

 for (i = 0; i < 7; i ++)
 {
  if (input == actor_colours_array [i])
   j = i + direction;
 }

 if (j <= -1)
  j = 6;
 if (j >= 7)
  j = 0;

 return actor_colours_array [j];
}

void get_number(int which_option, int number_change)
{
 play_sound(WAV_SHORT_BLIP);

 switch(which_option)
 {
  case ENTRY_COLOUR1_P1:
  player[1].colour1 = get_next_colour(player[1].colour1, number_change);
  break;
  case ENTRY_COLOUR2_P1:
  player[1].colour2 = get_next_colour(player[1].colour2, number_change);
  break;
  case ENTRY_HANDICAP1:
  if (number_change == 1 && player[1].handicap < 100) player[1].handicap += 10;
  if (number_change == -1 && player[1].handicap > 10) player[1].handicap -= 10;
  break;
  case ENTRY_COLOUR1_P2:
  player[2].colour1 = get_next_colour(player[2].colour1, number_change);
  break;
  case ENTRY_COLOUR2_P2:
  player[2].colour2 = get_next_colour(player[2].colour2, number_change);
  break;
  case ENTRY_HANDICAP2:
  if (number_change == 1 && player[2].handicap < 100) player[2].handicap += 10;
  if (number_change == -1 && player[2].handicap > 10) player[2].handicap -= 10;
  break;
  case ENTRY_GRAVITY:
  if (number_change == 1 && game[game_being_edited].gravity < 60) game[game_being_edited].gravity ++;
  if (number_change == -1 && game[game_being_edited].gravity > 3) game[game_being_edited].gravity --;
  break;
  case ENTRY_HEALTH_AMOUNT:
  if (number_change == 1 && game[game_being_edited].health_amount < 500) game[game_being_edited].health_amount += 10;
  if (number_change == -1 && game[game_being_edited].health_amount > 10) game[game_being_edited].health_amount -= 10;
  break;
  case ENTRY_RELOAD_TIME:
  if (number_change == 1 && game[game_being_edited].reload_time < 50) game[game_being_edited].reload_time += 1;
  if (number_change == -1 && game[game_being_edited].reload_time > 1) game[game_being_edited].reload_time -= 1;
  break;
  case ENTRY_IMPACT:
  if (number_change == 1 && game[game_being_edited].impact_damage < 30) game[game_being_edited].impact_damage ++;
  if (number_change == -1 && game[game_being_edited].impact_damage > 0) game[game_being_edited].impact_damage --;
  break;
  case ENTRY_SCORE_LIMIT:
  if (number_change == 1 && game[game_being_edited].score_limit < 200) game[game_being_edited].score_limit ++;
  if (number_change == -1 && game[game_being_edited].score_limit > 1) game[game_being_edited].score_limit --;
  break;
  case ENTRY_PK_NUMBER:
  if (number_change == 1 && game[game_being_edited].pk_number < 20) game[game_being_edited].pk_number ++;
  if (number_change == -1 && game[game_being_edited].pk_number > 2) game[game_being_edited].pk_number --;
  break;
  case ENTRY_FRUIT_NUMBER:
  if (number_change == 1 && game[game_being_edited].fruit_no < 20) game[game_being_edited].fruit_no ++;
  if (number_change == -1 && game[game_being_edited].fruit_no > 2) game[game_being_edited].fruit_no --;
  break;
  case ENTRY_PENALTY:
  if (number_change == 1 && game[game_being_edited].penalty < 30) game[game_being_edited].penalty += 3;
  if (number_change == -1 && game[game_being_edited].penalty > 0) game[game_being_edited].penalty -= 3;
  break;
  case ENTRY_TH_BASE_NUMBER:
  if (number_change == 1 && game[game_being_edited].th_base_no < 4) game[game_being_edited].th_base_no ++;
  if (number_change == -1 && game[game_being_edited].th_base_no > 1) game[game_being_edited].th_base_no --;
  break;
  case ENTRY_LIVES_LIMIT:
  if (number_change == 1 && game[game_being_edited].lives_each < 30) game[game_being_edited].lives_each ++;
  if (number_change == -1 && game[game_being_edited].lives_each > 0) game[game_being_edited].lives_each --;
  break;
  case ENTRY_LEVEL_WIDTH:
  if (number_change == 1 && arena[0].max_x < DIRT_X) arena[0].max_x += 100;
  if (number_change == -1 && arena[0].max_x > 100) arena[0].max_x -= 100;
  break;
  case ENTRY_LEVEL_HEIGHT:
  if (number_change == 1 && arena[0].max_y < DIRT_Y) arena[0].max_y += 100;
  if (number_change == -1 && arena[0].max_y > 100) arena[0].max_y -= 100;
  break;
  case ENTRY_HANDICAP_AI1:
  case ENTRY_HANDICAP_AI2:
  case ENTRY_HANDICAP_AI3:
  case ENTRY_HANDICAP_AI4:
  case ENTRY_HANDICAP_AI5:
  case ENTRY_HANDICAP_AI6:
  case ENTRY_HANDICAP_AI7:
  if (number_change == 1 && ai_config[ai_editing(which_option)].handicap < 100) ai_config[ai_editing(which_option)].handicap += 10;
  if (number_change == -1 && ai_config[ai_editing(which_option)].handicap > 10) ai_config[ai_editing(which_option)].handicap -= 10;
  break;
 }

}


void toggle_option(int which_option, char tdir)
{
 switch(which_option)
 {
  case ENTRY_SELECT_PLAYERS:
  if (no_players == 2) no_players = 1; else no_players = 2;
  break;
  case ENTRY_RUN_VSYNC:
  options[0].run_vsync = cycle_value(options[0].run_vsync, 0, 1, tdir);
  break;
  case ENTRY_FUZZY_MENU:
  options[0].fuzzy_menu = cycle_value(options[0].fuzzy_menu, 0, 1, tdir);
  break;
  case ENTRY_STIPPLE_CLOUDS:
  options[0].stipple_clouds = cycle_value(options[0].stipple_clouds, 0, 2, tdir);
  break;
  case ENTRY_SHOW_FPS:
  options[0].show_fps = cycle_value(options[0].show_fps, 0, 1, tdir);
  break;
  case ENTRY_POSITIONAL_SOUND:
  options[0].positional_sound = cycle_value(options[0].positional_sound, 0, 2, tdir);
  break;
  case ENTRY_DISPLAY_DAMAGE:
  options[0].display_damage = cycle_value(options[0].display_damage, 0, 1, tdir);
  break;
  case ENTRY_PK_TIME_BETWEEN:
  if (tdir == -1)
  {
/*   if (game[game_being_edited].pk_time_between == 700)
    game[game_being_edited].pk_time_between = 1;
    else*/
   if (game[game_being_edited].pk_time_between == 300)
    game[game_being_edited].pk_time_between = 700;
    else
   if (game[game_being_edited].pk_time_between == 100)
    game[game_being_edited].pk_time_between = 300;
    else
//   if (game[game_being_edited].pk_time_between == 0)
    game[game_being_edited].pk_time_between = 100;
   } else
   {
/*    if (game[game_being_edited].pk_time_between == 100)
     game[game_being_edited].pk_time_between = 1;
     else*/
    if (game[game_being_edited].pk_time_between == 300)
     game[game_being_edited].pk_time_between = 100;
     else
    if (game[game_being_edited].pk_time_between == 700)
     game[game_being_edited].pk_time_between = 300;
     else
//   if (game[game_being_edited].pk_time_between == 0)
     game[game_being_edited].pk_time_between = 700;
   }
  break;
  case ENTRY_LIGHTSOURCING:
  game[game_being_edited].lightsourcing = cycle_value(game[game_being_edited].lightsourcing, 0, 1, tdir);
  break;
  case ENTRY_GAME_TYPE:
  game[game_being_edited].game_type = cycle_value(game[game_being_edited].game_type, 0, 6, tdir);
  if (game[game_being_edited].game_type == GAME_PORKBALL)
   game[game_being_edited].game_type = GAME_TAKEHOLD;
  break;
  case ENTRY_NO_WEAPONS:
  game[game_being_edited].starting_weapons = cycle_value(game[game_being_edited].starting_weapons, 0, 3, tdir);
  break;
  case ENTRY_FAST_BULLETS:
  game[game_being_edited].fast_bullets = cycle_value(game[game_being_edited].fast_bullets, 0, 2, tdir);
  break;
  case ENTRY_SOFT_DIRT:
  game[game_being_edited].soft_dirt = cycle_value(game[game_being_edited].soft_dirt, 0, 1, tdir);
  break;
  case ENTRY_UNLIMITED_CLIPS:
  game[game_being_edited].unlimited_clips = cycle_value(game[game_being_edited].unlimited_clips, 0, 1, tdir);
  break;
  case ENTRY_RESET_WEAPONS:
  game[game_being_edited].reset_weapons = cycle_value(game[game_being_edited].reset_weapons, 0, 1, tdir);
  break;
  case ENTRY_RECHOOSE_ATYPE:
  game[game_being_edited].rechoose_atype = cycle_value(game[game_being_edited].rechoose_atype, 0, 1, tdir);
  break;
  case ENTRY_BULLETS_EXPLODE:
  game[game_being_edited].bullets_explode = cycle_value(game[game_being_edited].bullets_explode, 0, 2, tdir);
  break;
  case ENTRY_BLAST_BULLETS:
  game[game_being_edited].blast_bullets = cycle_value(game[game_being_edited].blast_bullets, 0, 1, tdir);
  break;
  case ENTRY_SHOW_MAP:
  game[game_being_edited].show_map = cycle_value(game[game_being_edited].show_map, 0, 1, tdir);
  break;
  case ENTRY_ATYPES_AVAIL:
  game[game_being_edited].atypes_avail = cycle_value(game[game_being_edited].atypes_avail, 0, 2, tdir);
  break;
  case ENTRY_PK_HEALTH:
  game[game_being_edited].pk_health = cycle_value(game[game_being_edited].pk_health, 0, 3, tdir);
  break;
  case ENTRY_PK_WEAPON:
  game[game_being_edited].pk_weapon = cycle_value(game[game_being_edited].pk_weapon, 0, 3, tdir);
  break;
  case ENTRY_PK_EQUIP:
  game[game_being_edited].pk_equip = cycle_value(game[game_being_edited].pk_equip, 0, 3, tdir);
  break;
  case ENTRY_PK_NAMES:
  game[game_being_edited].name_boxes = cycle_value(game[game_being_edited].name_boxes, 0, 1, tdir);
  break;
  case ENTRY_SERIAL_CONNECTION:
  serial[0].game_type = cycle_value(serial[0].game_type, SERIAL_LOCAL_ONLY, SERIAL_CLIENT, tdir);
  break;
  case ENTRY_COM_PORT:
  serial[0].com_port = cycle_value(serial[0].com_port, 1, 2, tdir);
  break;
  //case ENTRY_CONNECTION_TYPE:
  //if (serial[0].connection_type == NET_DRIVER_IPX_DOS)
  // serial[0].connection_type = NET_DRIVER_SERIAL_DOS;
  //  else
  //   serial[0].connection_type = NET_DRIVER_IPX_DOS;
  //break;
  case ENTRY_WHO_DAMAGE:
  serial[0].who_decides_damage = cycle_value(serial[0].who_decides_damage, 0, 2, tdir);
  break;
  case ENTRY_SELECT_TEAMS:
  arena[0].teams = cycle_value(arena[0].teams, 0, 1, tdir);
  break;
  case ENTRY_ACTIVE_AI1:
  case ENTRY_ACTIVE_AI2:
  case ENTRY_ACTIVE_AI3:
  case ENTRY_ACTIVE_AI4:
  case ENTRY_ACTIVE_AI5:
  case ENTRY_ACTIVE_AI6:
  case ENTRY_ACTIVE_AI7:
  ai_config[ai_editing(which_option)].active = cycle_value(ai_config[ai_editing(which_option)].active, 0, 1, tdir);
  break;
  case ENTRY_SKILL_AI1:
  case ENTRY_SKILL_AI2:
  case ENTRY_SKILL_AI3:
  case ENTRY_SKILL_AI4:
  case ENTRY_SKILL_AI5:
  case ENTRY_SKILL_AI6:
  case ENTRY_SKILL_AI7:
  ai_config[ai_editing(which_option)].skill = cycle_value(ai_config[ai_editing(which_option)].skill, 0, 2, tdir);
  break;
  case ENTRY_TEAM_AI1:
  case ENTRY_TEAM_AI2:
  case ENTRY_TEAM_AI3:
  case ENTRY_TEAM_AI4:
  case ENTRY_TEAM_AI5:
  case ENTRY_TEAM_AI6:
  case ENTRY_TEAM_AI7:
  ai_config[ai_editing(which_option)].team = cycle_value(ai_config[ai_editing(which_option)].team, 1, 6, tdir);
  break;
  case ENTRY_ATYPE_AI1:
  case ENTRY_ATYPE_AI2:
  case ENTRY_ATYPE_AI3:
  case ENTRY_ATYPE_AI4:
  case ENTRY_ATYPE_AI5:
  case ENTRY_ATYPE_AI6:
  case ENTRY_ATYPE_AI7:
  ai_config[ai_editing(which_option)].atype = cycle_value(ai_config[ai_editing(which_option)].atype, 0, 9, tdir);
  break;
  case ENTRY_SOLDIER_AI1:
  case ENTRY_SOLDIER_AI2:
  case ENTRY_SOLDIER_AI3:
  case ENTRY_SOLDIER_AI4:
  case ENTRY_SOLDIER_AI5:
  case ENTRY_SOLDIER_AI6:
  case ENTRY_SOLDIER_AI7:
  ai_config[ai_editing(which_option)].soldier = cycle_value(ai_config[ai_editing(which_option)].soldier, 0, NO_SOLDIER_FILES, tdir);
  break;
  case ENTRY_AI_IMPACT:
  arena[0].ai_no_impact = cycle_value(arena[0].ai_no_impact, 0, 1, tdir);
  break;
  case ENTRY_AI_CLIPS:
  arena[0].ai_unlimited_clips = cycle_value(arena[0].ai_unlimited_clips, 0, 1, tdir);
  break;
  case ENTRY_AI_WEAPONS:
  arena[0].ai_weapons = cycle_value(arena[0].ai_weapons, 0, 1, tdir);
  break;
  case ENTRY_LEVEL_COLOURS:
  arena[0].level_colours = cycle_value(arena[0].level_colours, 0, 2, tdir);
  break;
  case ENTRY_LEVEL_STYLE:
  arena[0].level_style = cycle_value(arena[0].level_style, 0, 3, tdir);
  break;
  case ENTRY_LEVEL_SOLIDS:
  arena[0].solid_density = cycle_value(arena[0].solid_density, 0, 4, tdir);
  break;
  case ENTRY_LEVEL_REDIRT:
  arena[0].replace_dirt = cycle_value(arena[0].replace_dirt, 0, 3, tdir);
  break;
  case ENTRY_LEVEL_DIRT:
  arena[0].dirt_fill = cycle_value(arena[0].dirt_fill, 0, 3, tdir);
  break;
  case ENTRY_AMBIENT_LIGHT:
  arena[0].ambient_light = cycle_value(arena[0].ambient_light, 0, 4, tdir);
  break;
  case ENTRY_LAMPS:
  arena[0].lamp_amount = cycle_value(arena[0].lamp_amount, 0, 6, tdir);
  break;
  case ENTRY_TEAM1:
  player[1].team = cycle_value(player[1].team, 1, 6, tdir);
  break;
  case ENTRY_TEAM2:
  player[2].team = cycle_value(player[2].team, 1, 6, tdir);
  break;
  case ENTRY_SOLDIER1:
  player[1].soldier = cycle_value(player[1].soldier, 0, NO_SOLDIER_FILES - 1, tdir);
  break;
  case ENTRY_SOLDIER2:
  player[2].soldier = cycle_value(player[2].soldier, 0, NO_SOLDIER_FILES - 1, tdir);
  break;

  case ENTRY_QUICKSTART_DIFFICULTY:
  arena[0].quickstart_difficulty = cycle_value(arena[0].quickstart_difficulty, 0, 3, tdir);
  break;
  case ENTRY_QUICKSTART_DIRT:
  arena[0].quickstart_dirt = cycle_value(arena[0].quickstart_dirt, 0, 3, tdir);
  break;
  case ENTRY_QUICKSTART_LIGHTSOURCE:
  arena[0].quickstart_lightsourced = cycle_value(arena[0].quickstart_lightsourced, 0, 1, tdir);
  break;
  case ENTRY_QUICKSTART_WEAPONS:
  arena[0].quickstart_weapons = cycle_value(arena[0].quickstart_weapons, 0, 7, tdir);
  break;
  
 }




 if (which_option >= ENTRY_WEAPON_1 && which_option < ENTRY_END_WEAPONS)
 {
  if (tdir == 1)
  {
      if (game[game_being_edited].weapon_status [which_option - ENTRY_WEAPON_1 + 2 + NO_EQUIP] >= 5)
         game[game_being_edited].weapon_status [which_option - ENTRY_WEAPON_1 + 2 + NO_EQUIP] = 0;
          else
           game[game_being_edited].weapon_status [which_option - ENTRY_WEAPON_1 + 2 + NO_EQUIP] ++;
  } else
  {
      if (game[game_being_edited].weapon_status [which_option - ENTRY_WEAPON_1 + 2 + NO_EQUIP] == 0)
         game[game_being_edited].weapon_status [which_option - ENTRY_WEAPON_1 + 2 + NO_EQUIP] = 5;
          else
           game[game_being_edited].weapon_status [which_option - ENTRY_WEAPON_1 + 2 + NO_EQUIP] --;
  }
 }
 
 if (which_option >= ENTRY_EQUIP_1 && which_option < ENTRY_END_EQUIP)
 {
  if (tdir == 1)
  {
      if (game[game_being_edited].equip_status [which_option - ENTRY_EQUIP_1] >= 5)
         game[game_being_edited].equip_status [which_option - ENTRY_EQUIP_1] = 0;
          else
           game[game_being_edited].equip_status [which_option - ENTRY_EQUIP_1] ++;
  } else
  {
      if (game[game_being_edited].equip_status [which_option - ENTRY_EQUIP_1] == 0)
         game[game_being_edited].equip_status [which_option - ENTRY_EQUIP_1] = 5;
          else
           game[game_being_edited].equip_status [which_option - ENTRY_EQUIP_1] --;
  }
 }
 
}

int cycle_value(int value, int min, int max, int cdir)
{
 if (cdir == 1)
 {
  if (value == max) return min;
  return value + 1;
 }
  if (value == min) return max;
  return value - 1;
}

void enter_key(int which_option, int menu_offset, int menu_count)
{
 int newkey = 0;

 // assume the enter key is still being pressed
 do
 {
  rest(1);
  newkey = collate_keys();
  print_a_menu(menu_offset, menu_count, COLOUR_RED4);
  run_menu_screen();
 } while (newkey != -1);

 do
 {
  rest(1);
  newkey = collate_keys();
  print_a_menu(menu_offset, menu_count, COLOUR_RED4);
  run_menu_screen();
 } while (newkey == -1 || newkey == KEY_ESC);

 if (keypressed()) readkey(); // clears buffer
// run_menu_screen();
 play_sound(WAV_SHORT_BLIP);
 switch(which_option)
 {
  case ENTRY_KEY_UP1:
  cmd_key [CMD_KEY_UP1] = newkey;
  break;
  case ENTRY_KEY_DOWN1:
  cmd_key [CMD_KEY_DOWN1] = newkey;
  break;
  case ENTRY_KEY_LEFT1:
  cmd_key [CMD_KEY_LEFT1] = newkey;
  break;
  case ENTRY_KEY_RIGHT1:
  cmd_key [CMD_KEY_RIGHT1] = newkey;
  break;
  case ENTRY_KEY_SHOOT1:
  cmd_key [CMD_KEY_SHOOT1] = newkey;
  break;
  case ENTRY_KEY_JUMP1:
  cmd_key [CMD_KEY_JUMP1] = newkey;
  break;
  case ENTRY_KEY_CHANGE1:
  cmd_key [CMD_KEY_CHANGE1] = newkey;
  break;
  case ENTRY_KEY_SCORE1:
  cmd_key [CMD_KEY_SCORE1] = newkey;
  break;
  case ENTRY_KEY_NAMES1:
  cmd_key [CMD_KEY_NAMES1] = newkey;
  break;
  case ENTRY_KEY_GRAPPLE1:
  cmd_key [CMD_KEY_GRAPPLE1] = newkey;
  break;

  case ENTRY_KEY_UP2:
  cmd_key [CMD_KEY_UP2] = newkey;
  break;
  case ENTRY_KEY_DOWN2:
  cmd_key [CMD_KEY_DOWN2] = newkey;
  break;
  case ENTRY_KEY_LEFT2:
  cmd_key [CMD_KEY_LEFT2] = newkey;
  break;
  case ENTRY_KEY_RIGHT2:
  cmd_key [CMD_KEY_RIGHT2] = newkey;
  break;
  case ENTRY_KEY_SHOOT2:
  cmd_key [CMD_KEY_SHOOT2] = newkey;
  break;
  case ENTRY_KEY_JUMP2:
  cmd_key [CMD_KEY_JUMP2] = newkey;
  break;
  case ENTRY_KEY_CHANGE2:
  cmd_key [CMD_KEY_CHANGE2] = newkey;
  break;
  case ENTRY_KEY_SCORE2:
  cmd_key [CMD_KEY_SCORE2] = newkey;
  break;
  case ENTRY_KEY_NAMES2:
  cmd_key [CMD_KEY_NAMES2] = newkey;
  break;
  case ENTRY_KEY_GRAPPLE2:
  cmd_key [CMD_KEY_GRAPPLE2] = newkey;
  break;

 }
}

int player_choose_weapon(int which_option)
{
 if (which_option > ENTRY_END_WEAPONS)
 {
  // it's equipment:
  player[player_choosing].weapon [slot_choosing] = which_option - ENTRY_EQUIP_1 + 2;
 } else
  player[player_choosing].weapon [slot_choosing] = which_option - ENTRY_WEAPON_1 + 2 + NO_EQUIP;

 return 1; // has finished choosing
}

int player_choose_atype(int which_option)
{
 player_weapons[player_choosing] = 1;
 if (game[0].starting_weapons == 0) player_weapons[player_choosing] = 0;
 switch(which_option)
 {
 case ENTRY_ATYPE_LIGHT: player[player_choosing].atype = ATYPE_LIGHT;
 if (game[0].starting_weapons == 2) player_weapons[player_choosing] = 2;
 break;
 case ENTRY_ATYPE_MEDIUM: player[player_choosing].atype = ATYPE_MEDIUM;
 if (game[0].starting_weapons == 2) player_weapons[player_choosing] = 3;
 break;
 case ENTRY_ATYPE_HEAVY: player[player_choosing].atype = ATYPE_HEAVY;
 if (game[0].starting_weapons == 2) player_weapons[player_choosing] = 4;
// if (game[0].starting_weapons == 1) player_weapons[player_choosing] = 2;
 break;
 }
 return 1;
}


int player_choose_team(int which_option)
{
 switch(which_option)
 {
  case ENTRY_TEAM_NONE: player[player_choosing].team = TEAM_NONE;
  break;
  case ENTRY_TEAM_RED: player[player_choosing].team = TEAM_RED;
  break;
  case ENTRY_TEAM_BLUE: player[player_choosing].team = TEAM_BLUE;
  break;
  case ENTRY_TEAM_GREEN: player[player_choosing].team = TEAM_GREEN;
  break;
  case ENTRY_TEAM_GOLD: player[player_choosing].team = TEAM_GOLD;
  break;
  case ENTRY_TEAM_GREY: player[player_choosing].team = TEAM_GREY;
  break;
  case ENTRY_TEAM_BROWN: player[player_choosing].team = TEAM_BROWN;
  break;


 }
 return 1;
}


void get_string(int which_option, int menu_offset, int menu_count)
{

 char strng [20];
 char key_returned = 0;
 int str_pos = 0;

 strcpy(strng, "");
 strng [10] = 0;
 strng [11] = 0;



 do
 {
 
  switch(which_option)
  {
   case ENTRY_NAME1: strcpy(player[1].pname, strng); break;
   case ENTRY_NAME2: strcpy(player[2].pname, strng); break;
   case ENTRY_NAME_GAME: strcpy(game[game_being_edited].gname, strng); break;
   case ENTRY_NAME_AI1:
   case ENTRY_NAME_AI2:
   case ENTRY_NAME_AI3:
   case ENTRY_NAME_AI4:
   case ENTRY_NAME_AI5:
   case ENTRY_NAME_AI6:
   case ENTRY_NAME_AI7: strcpy(ai_config[ai_editing(which_option)].name, strng); break;
   case ENTRY_LOAD_LEVEL: strcpy(arena[0].level_loaded, strng); break;
  }
  print_a_menu(menu_offset, menu_count, COLOUR_RED4);
  run_menu_screen();
  key_returned = get_a_key(menu_offset, menu_count);
  play_sound(WAV_SHORT_BLIP);
  if (key_returned == 13) break;
  if (key_returned == 8)
  {
   if (str_pos > 0)
   {
    strng [str_pos - 1] = 0;
    str_pos --;
   }
   continue;
  }
  print_a_menu(menu_offset, menu_count, COLOUR_RED4);
  run_menu_screen();
  strng [str_pos] = key_returned;
  strng [str_pos + 1] = 0;
  if (str_pos < 15) str_pos ++;

 } while (TRUE);

 switch(which_option)
 {
  case ENTRY_LOAD_LEVEL:
  if (strlen(arena[0].level_loaded) == 0)
  {
   arena[0].generate_level = 1;
   return;
  }
  if (!check_load_level())
  {
    menu_information_box("Failed to load map.", "Out of memory, bad format", "or file not found.", 1);
    strcpy(arena[0].level_loaded, "");
  }
     else
     {
      menu_information_box("", "Map loaded successfully.", "", 1);
      arena[0].generate_level = 0;
     }
   break;

 }

}

void get_server_address(int which_option, int menu_offset, int menu_count)
{

 char strng [50];
 char key_returned = 0;
 int str_pos = 0;

 strcpy(strng, "");
 strng [10] = 0; // ??
 strng [11] = 0;

 do
 {
 
  strcpy(serial[0].port_string, strng);
  print_a_menu(menu_offset, menu_count, COLOUR_RED4);
  run_menu_screen();
  key_returned = get_a_key(menu_offset, menu_count);
  play_sound(WAV_SHORT_BLIP);
  if (key_returned == 13) break;
  if (key_returned == 8)
  {
   if (str_pos > 0)
   {
    strng [str_pos - 1] = 0;
    str_pos --;
   }
   continue;
  }
  print_a_menu(menu_offset, menu_count, COLOUR_RED4);
  run_menu_screen();
  strng [str_pos] = key_returned;
  strng [str_pos + 1] = 0;
  if (str_pos < 48) str_pos ++;

 } while (TRUE);


}


unsigned char get_a_key(int menu_offset, int menu_count)
{
 int newkey = 0;

 // assume the enter key is still being pressed
 do
 {
  do
  {
   rest(1);
   print_a_menu(menu_offset, menu_count, COLOUR_RED4);
   run_menu_screen();
  } while (keypressed() == 0);
//  run_menu_screen();

  newkey = readkey() & 0xff;
  if (newkey == 13) return 13;
  if (newkey == 8) return 8;
 } while (acceptable_char(newkey) == 0);

 return newkey;

}



// checks if any keys are being pressed, and returns the first
//  one it finds
char collate_keys(void)
{
 int ki;

 for (ki = 0; ki < KEY_MAX; ki++)
 {
  if (key [ki] != 0) return ki;
 }

 return -1;

}


void change_menu(int which_option)
{
 switch(cmenu[which_option].entry_index)
 {
  case ENTRY_DEFINE_KEYS: cmenu = define_keys; break;
  case ENTRY_SERIAL_CONFIG: cmenu = serial_menu; break;
  case ENTRY_CONFIG_PLAYERS: cmenu = player_menu; break;
  case ENTRY_BEGIN_GAME: cmenu = games_menu; break;
  case ENTRY_BACK_TO_MAIN: cmenu = main_menu; break;
  case ENTRY_GAME_1: cmenu = custom_menu;
       game_being_edited = 1;
       copy_gametype(1, 0);
       break;
  case ENTRY_GAME_2: cmenu = custom_menu;
       game_being_edited = 2;
       copy_gametype(2, 0);
       break;
  case ENTRY_GAME_3: cmenu = custom_menu;
       game_being_edited = 3;
       copy_gametype(3, 0);
       break;
  case ENTRY_GAME_4: cmenu = custom_menu;
       game_being_edited = 4;
       copy_gametype(4, 0);
       break;
  case ENTRY_GAME_5: cmenu = custom_menu;
       game_being_edited = 5;
       copy_gametype(5, 0);
       break;
  case ENTRY_GAME_6: cmenu = custom_menu;
       game_being_edited = 6;
       copy_gametype(6, 0);
       break;
  case ENTRY_CUSTOM_GAME: cmenu = custom_menu;
       game_being_edited = 0;
//       copy_gametype(3, 0);
       break;
  case ENTRY_OPTIONS_MENU: cmenu = options_menu;
  break;
  case ENTRY_AI_MENU: cmenu = ai_menu;
  break;
  case ENTRY_LEVEL_MENU: cmenu = level_menu;
  break;
  case ENTRY_QUICKSTART: cmenu = quickstart_menu;
  break;
 }
}


void check_commands(char command_issued [3])
{

 int keyp = readkey() >> 8;

 command_issued [0] = 0;
 command_issued [1] = 0;
 command_issued [2] = 0;

 if (keyp == KEY_8_PAD || keyp == KEY_UP) command_issued [0] = -1;
 else
 if (keyp == KEY_2_PAD || keyp == KEY_DOWN) command_issued [0] = 1;
 else
 if (keyp == KEY_4_PAD || keyp == KEY_LEFT) command_issued [1] = -1;
 else
 if (keyp == KEY_6_PAD || keyp == KEY_RIGHT) command_issued [1] = 1;
 else
  command_issued [2] = keyp;

}

void print_scroll_arrows(int menu_size)
{

// return;

 int tri_x = window_x_sp / 2;

 if (video_mode == 1)
 {
  tri_x = 100;
//void draw_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y);
//        textprintf(menu_screen, large_font, 250 + (scanline_x / 2) % 7, menu_top + (line_length * scrline), COLOUR_YELLOW4, ">>>");
  draw_sprite(menu_screen, datf[SCROLL_ARROW].dat, tri_x, menu_top - (line_length) - (scanline_x) % 10 - 21);
  rectfill(menu_screen, tri_x, menu_top - (line_length) - 32, tri_x + 20, menu_top - (line_length) - 15, 0);
  rectfill(menu_screen, tri_x, menu_top - (line_length), tri_x + 20, menu_top - (line_length) + 18, 0);
  draw_sprite_v_flip(menu_screen, datf[SCROLL_ARROW].dat, tri_x, menu_top + line_length * (max_menu_entries + 2) + (scanline_x) % 10 - 15);
  rectfill(menu_screen, tri_x, menu_top + line_length * (max_menu_entries + 2) - 19, tri_x + 20, menu_top + line_length * (max_menu_entries + 2), 0);
  rectfill(menu_screen, tri_x, menu_top + line_length * (max_menu_entries + 2) + 15, tri_x + 20, menu_top + line_length * (max_menu_entries + 2) + 40, 0);
//   captain_pork = datf[CAPTAIN_PORK].dat;
  return;
 }
// if (menu_size > max_menu_entries)
 {
//  putpixel(screen, window_x_sp, menu_top - line_length, 1);
//  putpixel(screen, window_x_sp, menu_top + line_length * (max_menu_entries + 2), 1);
//  rectfill(screen, window_x_sp / 2 - 2, menu_top - (line_length), window_x_sp / 2 + 2, menu_top + 3 - (line_length), 1);
//  rectfill(screen, window_x_sp / 2 - 2, menu_top + line_length * (max_menu_entries + 2), window_x_sp / 2 + 2, menu_top - 3 + line_length * (max_menu_entries + 2), 1);
  triangle(menu_screen, tri_x, menu_top - (line_length), tri_x - 5, menu_top + 3 - (line_length), tri_x + 5, menu_top + 3 - (line_length), COLOUR_BLUE4);
  triangle(menu_screen, tri_x, menu_top + line_length * (max_menu_entries + 2), tri_x - 5, menu_top - 3 + line_length * (max_menu_entries + 2), tri_x + 5, menu_top - 3 + line_length * (max_menu_entries + 2), COLOUR_BLUE4);
 }// else
// {
//  putpixel(screen, window_x_sp, menu_top - line_length, 0);
//  putpixel(screen, window_x_sp, menu_top + line_length * (max_menu_entries + 2), 0);
//  rectfill(screen, window_x_sp / 2 - 2, menu_top - (line_length), window_x_sp / 2 + 2, menu_top + 3 - (line_length), 0);
//  rectfill(screen, window_x_sp / 2 - 2, menu_top + line_length * (max_menu_entries + 2), window_x_sp / 2 + 2, menu_top - 3 + line_length * (max_menu_entries + 2), 0);

//  triangle(menu_screen, tri_x, menu_top - (line_length), tri_x - 5, menu_top + 3 - (line_length), tri_x + 5, menu_top + 3 - (line_length), 0);
//  triangle(menu_screen, tri_x, menu_top + line_length * (max_menu_entries + 2), tri_x - 5, menu_top - 3 + line_length * (max_menu_entries + 2), tri_x + 5, menu_top - 3 + line_length * (max_menu_entries + 2), 0);

//  triangle(screen, window_x_sp / 2, menu_top - (line_length), window_x_sp / 2 - 5, menu_top + 3 - (line_length), window_x_sp / 2 + 5, menu_top + 3 - (line_length), 0);
//  triangle(screen, window_x_sp / 2, menu_top + line_length * (max_menu_entries + 2), window_x_sp / 2 - 5, menu_top - 3 + line_length * (max_menu_entries + 2), window_x_sp / 2 + 5, menu_top - 3 + line_length * (max_menu_entries + 2), 0);
//  triangle(screen, window_x_sp / 2, menu_top - 10, window_x_sp / 2 - 5, menu_top + 5, window_x_sp / 2 + 5, menu_top + 5, 0);
//  triangle(screen, window_x_sp / 2, menu_top + line_length * (max_menu_entries + 2), window_x_sp / 2 - 5, menu_top + 5 + line_length * (max_menu_entries + 2), window_x_sp / 2 + 5, menu_top + 5 + line_length * (max_menu_entries + 2), 0);
// }

}

void print_menu_entry(struct menu_format *mentry, int mcount, int scrline, int colour)
{

char mstring [150];
char itstring [5];
FONT *print_font = large_font;
strcpy(mstring, "   ");
strcat(mstring, mentry[mcount].entry_name);

    switch(mentry[mcount].response_type)
    {
      case RESPONSE_CYCLE_WEAPON:
      switch(game[game_being_edited].weapon_status [mentry[mcount].entry_index - ENTRY_WEAPON_1 + 2 + NO_EQUIP])
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - Start Only"); break;
       case 2: strcat(mstring, " - Start + Common"); break;
       case 3: strcat(mstring, " - Start + Rare"); break;
       case 4: strcat(mstring, " - Common in Boxes"); break;
       case 5: strcat(mstring, " - Rare in Boxes"); break;
      }
      break;
      case RESPONSE_CYCLE_EQUIP:
      switch(game[game_being_edited].equip_status [mentry[mcount].entry_index - ENTRY_EQUIP_1])
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - Start Only"); break;
       case 2: strcat(mstring, " - Start + Common"); break;
       case 3: strcat(mstring, " - Start + Rare"); break;
       case 4: strcat(mstring, " - Common in Boxes"); break;
       case 5: strcat(mstring, " - Rare in Boxes"); break;
      }
      break;
    }

/*if (mentry[mcount].response_type == RESPONSE_YN
    || mentry[mcount].response_type == RESPONSE_SLIDER
    || mentry[mcount].response_type == RESPONSE_NUMBER
    || mentry[mcount].response_type == RESPONSE_ENTER_KEY
    || mentry[mcount].response_type == RESPONSE_ENTER_STRING)*/
    {
     switch(mentry[mcount].entry_index)
     {
      case ENTRY_SELECT_PLAYERS:
      if (no_players == 1)
            strcat(mstring, " - 1");
             else
              strcat(mstring, " - 2");
      break;
      case ENTRY_NAME1:
      strcat(mstring, player[1].pname);
      break;
      case ENTRY_GAME_1:
      strcat(mstring, game[1].gname);
      break;
      case ENTRY_GAME_2:
      strcat(mstring, game[2].gname);
      break;
      case ENTRY_GAME_3:
      strcat(mstring, game[3].gname);
      break;
      case ENTRY_GAME_4:
      strcat(mstring, game[4].gname);
      break;
      case ENTRY_GAME_5:
      strcat(mstring, game[5].gname);
      break;
      case ENTRY_GAME_6:
      strcat(mstring, game[6].gname);
      break;
      case ENTRY_NAME_GAME:
      strcat(mstring, game[game_being_edited].gname);
      break;
      case ENTRY_COLOUR1_P1:
      append_colour_name(mstring, player[1].colour1);
      break;
      case ENTRY_COLOUR2_P1:
      append_colour_name(mstring, player[1].colour2);
      break;
      case ENTRY_COLOUR1_P2:
      append_colour_name(mstring, player[2].colour1);
      break;
      case ENTRY_COLOUR2_P2:
      append_colour_name(mstring, player[2].colour2);
      break;
      case ENTRY_HANDICAP1:
      strcat(mstring, my_itoa(player[1].handicap, itstring, 10));
      break;
      case ENTRY_HANDICAP2:
      strcat(mstring, my_itoa(player[2].handicap, itstring, 10));
      break;
      case ENTRY_NAME2:
      strcat(mstring, player[2].pname);
      break;
      case ENTRY_RUN_VSYNC:
      switch(options[0].run_vsync)
      {
       case 0: strcat(mstring, " - No"); break;
       case 1: strcat(mstring, " - Yes"); break;
      }
      break;
      case ENTRY_FUZZY_MENU:
      switch(options[0].fuzzy_menu)
      {
       case 0: strcat(mstring, " - No"); break;
       case 1: strcat(mstring, " - Yes"); break;
      }
      break;
      case ENTRY_STIPPLE_CLOUDS:
      switch(options[0].stipple_clouds)
      {
       case CLOUDS_FILL: strcat(mstring, " - Solid"); break;
       case CLOUDS_STIPPLE: strcat(mstring, " - Stippled"); break;
       case CLOUDS_TRANS: strcat(mstring, " - Transparent"); break;
      }
      break;
      case ENTRY_SHOW_FPS:
      switch(options[0].show_fps)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - On"); break;
      }
      break;
      case ENTRY_POSITIONAL_SOUND:
      switch(options[0].positional_sound)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - On"); break;
       case 2: strcat(mstring, " - On (Reversed)"); break;
      }
      break;
      case ENTRY_DISPLAY_DAMAGE:
      switch(options[0].display_damage)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - On"); break;
      }
      break;

      case ENTRY_SELECT_GAME:
      strcat(mstring, game[0].gname);
      break;

      case ENTRY_SELECT_TEAMS:
      switch(arena[0].teams)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - On"); break;
      }
      break;
      case ENTRY_AI_IMPACT:
      switch(arena[0].ai_no_impact)
      {
       case 0: strcat(mstring, " - Affected"); break;
       case 1: strcat(mstring, " - Unaffected"); break;
      }
      break;
      case ENTRY_AI_CLIPS:
      switch(arena[0].ai_unlimited_clips)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - On"); break;
      }
      break;
      case ENTRY_AI_WEAPONS:
      switch(arena[0].ai_weapons)
      {
       case 0: strcat(mstring, " - Standard"); break;
       case 1: strcat(mstring, " - Full"); break;
      }
      break;
      case ENTRY_LEVEL_COLOURS:
      switch(arena[0].level_colours)
      {
       case COLOURS_CLASSIC: strcat(mstring, " - Classic"); break;
       case COLOURS_CRAZY: strcat(mstring, " - Crazy"); break;
       case COLOURS_EARTH: strcat(mstring, " - Earth"); break;
      }
      break;
      case ENTRY_LEVEL_STYLE:
      switch(arena[0].level_style)
      {
       case LEVEL_CLASSIC: strcat(mstring, " - Classic"); break;
       case LEVEL_ANY: strcat(mstring, " - Anything"); break;
       case LEVEL_PLATFORM: strcat(mstring, " - Platform"); break;
       case LEVEL_CITY: strcat(mstring, " - City"); break;
      }
      break;
      case ENTRY_LEVEL_SOLIDS:
      switch(arena[0].solid_density)
      {
       case 0: strcat(mstring, " - None"); break;
       case 1: strcat(mstring, " - Sparse"); break;
       case 2: strcat(mstring, " - Some"); break;
       case 3: strcat(mstring, " - Many"); break;
       case 4: strcat(mstring, " - Cramped"); break;
      }
      break;
      case ENTRY_LEVEL_REDIRT:
      switch(arena[0].replace_dirt)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - Occasionally"); break;
       case 2: strcat(mstring, " - Often"); break;
       case 3: strcat(mstring, " - Very Often"); break;
      }
      break;
      case ENTRY_LEVEL_DIRT:
      switch(arena[0].dirt_fill)
      {
       case 0: strcat(mstring, " - None"); break;
       case 1: strcat(mstring, " - Hollowed Out"); break;
       case 2: strcat(mstring, " - Tunnelled"); break;
       case 3: strcat(mstring, " - Full"); break;
      }
      break;
      case ENTRY_QUICKSTART_DIRT:
      switch(arena[0].quickstart_dirt)
      {
       case 0: strcat(mstring, " - None"); break;
       case 1: strcat(mstring, " - Hollowed Out"); break;
       case 2: strcat(mstring, " - Tunnelled"); break;
       case 3: strcat(mstring, " - Full"); break;
      }
      break;
      case ENTRY_QUICKSTART_WEAPONS:
      switch(arena[0].quickstart_weapons)
      {
       case 0: strcat(mstring, " - None"); break;
       case 1: strcat(mstring, " - Basic (one each)"); break;
       case 2: strcat(mstring, " - Extended (one each)"); break;
       case 3: strcat(mstring, " - Full (one each)"); break;
       case 4: strcat(mstring, " - Basic (four each)"); break;
       case 5: strcat(mstring, " - Extended (four each)"); break;
       case 6: strcat(mstring, " - Full (four each)"); break;
       case 7: strcat(mstring, " - Class"); break;
      }
      break;
      case ENTRY_QUICKSTART_DIFFICULTY:
      switch(arena[0].quickstart_difficulty)
      {
       case 0: strcat(mstring, " - Easy"); break;
       case 1: strcat(mstring, " - Medium"); break;
       case 2: strcat(mstring, " - Hard"); break;
       case 3: strcat(mstring, " - Very Hard"); break;
      }
      break;
      case ENTRY_QUICKSTART_LIGHTSOURCE:
      switch(arena[0].quickstart_lightsourced)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - On"); break;
      }
      break;
      case ENTRY_AMBIENT_LIGHT:
      switch(arena[0].ambient_light)
      {
       case 0: strcat(mstring, " - None"); break;
       case 1: strcat(mstring, " - Candlelit"); break;
       case 2: strcat(mstring, " - 40 Watt"); break;
       case 3: strcat(mstring, " - 75 Watt"); break;
       case 4: strcat(mstring, " - Daylight"); break;
      }
      break;
      case ENTRY_LAMPS:
      switch(arena[0].lamp_amount)
      {
       case 0: strcat(mstring, " - None"); break;
       case 1: strcat(mstring, " - Few"); break;
       case 2: strcat(mstring, " - Some"); break;
       case 3: strcat(mstring, " - Many"); break;
       case 4: strcat(mstring, " - Few (Regular)"); break;
       case 5: strcat(mstring, " - Some (Regular)"); break;
       case 6: strcat(mstring, " - Many (Regular)"); break;
      }
      break;
      case ENTRY_LOAD_LEVEL:
      if (strlen(arena[0].level_loaded) == 0)
       strcat(mstring, "<None>");
        else
         strcat(mstring, arena[0].level_loaded);
      break;
      case ENTRY_TEAM1:
      strcat(mstring, team_name(player[1].team));
      break;
      case ENTRY_TEAM2:
      strcat(mstring, team_name(player[2].team));
      break;
      case ENTRY_SOLDIER1:
      switch(player[1].soldier)
      {
       case SOLDIER_SOLDIER: strcat(mstring, " - Soldier"); break;
       case SOLDIER_WALKER: strcat(mstring, " - Walker"); break;
//       case NO_SOLDER_FILES: strcat(mstring, "Chooses"); break;
      }
      break;
      case ENTRY_SOLDIER2:
      switch(player[2].soldier)
      {
       case SOLDIER_SOLDIER: strcat(mstring, " - Soldier"); break;
       case SOLDIER_WALKER: strcat(mstring, " - Walker"); break;
//       case NO_SOLDER_FILES: strcat(mstring, "Chooses"); break;
      }
      break;
      
      case ENTRY_NAME_AI1:
      case ENTRY_ACTIVE_AI1:
      case ENTRY_SKILL_AI1:
      case ENTRY_TEAM_AI1:
      case ENTRY_ATYPE_AI1:
      case ENTRY_HANDICAP_AI1:
      case ENTRY_SOLDIER_AI1:
      case ENTRY_NAME_AI2:
      case ENTRY_ACTIVE_AI2:
      case ENTRY_SKILL_AI2:
      case ENTRY_TEAM_AI2:
      case ENTRY_ATYPE_AI2:
      case ENTRY_HANDICAP_AI2:
      case ENTRY_SOLDIER_AI2:
      case ENTRY_NAME_AI3:
      case ENTRY_ACTIVE_AI3:
      case ENTRY_SKILL_AI3:
      case ENTRY_TEAM_AI3:
      case ENTRY_ATYPE_AI3:
      case ENTRY_HANDICAP_AI3:
      case ENTRY_SOLDIER_AI3:
      case ENTRY_NAME_AI4:
      case ENTRY_ACTIVE_AI4:
      case ENTRY_SKILL_AI4:
      case ENTRY_TEAM_AI4:
      case ENTRY_ATYPE_AI4:
      case ENTRY_HANDICAP_AI4:
      case ENTRY_SOLDIER_AI4:
      case ENTRY_NAME_AI5:
      case ENTRY_ACTIVE_AI5:
      case ENTRY_SKILL_AI5:
      case ENTRY_TEAM_AI5:
      case ENTRY_ATYPE_AI5:
      case ENTRY_HANDICAP_AI5:
      case ENTRY_SOLDIER_AI5:
      case ENTRY_NAME_AI6:
      case ENTRY_ACTIVE_AI6:
      case ENTRY_SKILL_AI6:
      case ENTRY_TEAM_AI6:
      case ENTRY_ATYPE_AI6:
      case ENTRY_HANDICAP_AI6:
      case ENTRY_SOLDIER_AI6:
      case ENTRY_NAME_AI7:
      case ENTRY_ACTIVE_AI7:
      case ENTRY_SKILL_AI7:
      case ENTRY_TEAM_AI7:
      case ENTRY_ATYPE_AI7:
      case ENTRY_HANDICAP_AI7:
      case ENTRY_SOLDIER_AI7:
      colour = ai_config_string(mentry[mcount].entry_index, mstring, colour);
      break;
      
      case ENTRY_GRAVITY:
      strcat(mstring, my_itoa(game[game_being_edited].gravity, itstring, 10));
      break;
      case ENTRY_HEALTH_AMOUNT:
      strcat(mstring, my_itoa(game[game_being_edited].health_amount, itstring, 10));
      break;
      case ENTRY_RELOAD_TIME:
      strcat(mstring, my_itoa(game[game_being_edited].reload_time, itstring, 10));
      break;
      case ENTRY_IMPACT:
      strcat(mstring, my_itoa(game[game_being_edited].impact_damage, itstring, 10));
      break;
      case ENTRY_PK_NUMBER:
      strcat(mstring, my_itoa(game[game_being_edited].pk_number - 2, itstring, 10));
      break;
      case ENTRY_FRUIT_NUMBER:
      strcat(mstring, my_itoa(game[game_being_edited].fruit_no, itstring, 10));
      break;
      case ENTRY_PENALTY:
      if (game[game_being_edited].penalty == 0)
       strcat(mstring, "None");
        else
         {
          strcat(mstring, my_itoa(game[game_being_edited].penalty, itstring, 10));
          strcat(mstring, " seconds");
         }
      break;
      case ENTRY_TH_BASE_NUMBER:
      strcat(mstring, my_itoa(game[game_being_edited].th_base_no, itstring, 10));
      break;
      case ENTRY_LEVEL_WIDTH:
      strcat(mstring, my_itoa(arena[0].max_x, itstring, 10));
      break;
      case ENTRY_LEVEL_HEIGHT:
      strcat(mstring, my_itoa(arena[0].max_y, itstring, 10));
      break;
      case ENTRY_GAME_TYPE:
      switch(game[game_being_edited].game_type)
      {
       case GAME_KILL: strcat(mstring, "Kill Everything"); break;
       case GAME_LAST_ONE: strcat(mstring, "Last One Standing"); break;
       case GAME_CAP_FLAG: strcat(mstring, "Capture the Flag"); break;
       case GAME_FRUIT_HUNT: strcat(mstring, "Hunt for Fruit"); break;
       case GAME_PORKBALL: strcat(mstring, "Porkball"); break;
       case GAME_TAKEHOLD: strcat(mstring, "Take & Hold"); break;
       case GAME_GRAIL: strcat(mstring, "Holy Grail"); break;
      }
      break;
      case ENTRY_LIVES_LIMIT:
      if (game[game_being_edited].lives_each == 0)
       strcat(mstring, "Unlimited");
        else
         strcat(mstring, my_itoa(game[game_being_edited].lives_each, itstring, 10));
      break;
      case ENTRY_SCORE_LIMIT:
//      if (game[game_being_edited].score_limit == 0)
//       strcat(mstring, "Unlimited");
//        else
         strcat(mstring, my_itoa(game[game_being_edited].score_limit, itstring, 10));
      break;
      case ENTRY_PK_TIME_BETWEEN:
      switch(game[game_being_edited].pk_time_between)
      {
       case 1: strcat(mstring, "Instant"); break;
       case 100: strcat(mstring, "Fast"); break;
       case 300: strcat(mstring, "Not So Fast"); break;
       case 700: strcat(mstring, "Slow"); break;
      }
      break;
      case ENTRY_LIGHTSOURCING:
      switch(game[game_being_edited].lightsourcing)
      {
       case 0: strcat(mstring, "Off"); break;
       case 1: strcat(mstring, "On"); break;
      }
      break;
      case ENTRY_NO_WEAPONS:
      switch(game[game_being_edited].starting_weapons)
      {
       case 0: strcat(mstring, "None"); break;
       case 1: strcat(mstring, "Limited"); break;
       case 2: strcat(mstring, "Full"); break;
       case 3: strcat(mstring, "Class"); break;
      }
      break;
      case ENTRY_FAST_BULLETS:
      switch(game[game_being_edited].fast_bullets)
      {
       case 0: strcat(mstring, "Slow"); break;
       case 1: strcat(mstring, "Fast"); break;
       case 2: strcat(mstring, "Very Fast"); break;
      }
      break;
      case ENTRY_BULLETS_EXPLODE:
      switch(game[game_being_edited].bullets_explode)
      {
       case BULLETS_NO_EXPLODE: strcat(mstring, "Not Exploding"); break;
       case BULLETS_EXPLODE: strcat(mstring, "Exploding"); break;
       case BULLETS_PAINTBALL: strcat(mstring, "Paintball"); break;
      }
      break;
      case ENTRY_BLAST_BULLETS:
      switch(game[game_being_edited].blast_bullets)
      {
       case 0: strcat(mstring, "Off"); break;
       case 1: strcat(mstring, "On"); break;
      }
      break;
      case ENTRY_SHOW_MAP:
      switch(game[game_being_edited].show_map)
      {
       case 0: strcat(mstring, "Off"); break;
       case 1: strcat(mstring, "On"); break;
      }
      break;
      case ENTRY_SOFT_DIRT:
      switch(game[game_being_edited].soft_dirt)
      {
       case 0: strcat(mstring, "No"); break;
       case 1: strcat(mstring, "Yes"); break;
      }
      break;
      case ENTRY_UNLIMITED_CLIPS:
      switch(game[game_being_edited].unlimited_clips)
      {
       case 0: strcat(mstring, "No"); break;
       case 1: strcat(mstring, "Yes"); break;
      }
      break;
      case ENTRY_RESET_WEAPONS:
      switch(game[game_being_edited].reset_weapons)
      {
       case 0: strcat(mstring, "No"); break;
       case 1: strcat(mstring, "Yes"); break;
      }
      break;
      case ENTRY_RECHOOSE_ATYPE:
      switch(game[game_being_edited].rechoose_atype)
      {
       case 0: strcat(mstring, "No"); break;
       case 1: strcat(mstring, "Yes"); break;
      }
      break;
      case ENTRY_ATYPES_AVAIL:
      switch(game[game_being_edited].atypes_avail)
      {
       case 0: strcat(mstring, "Soldier Only"); break;
       case 1: strcat(mstring, "Soldier, Scout, Cyborg"); break;
       case 2: strcat(mstring, "All"); break;
      }
      break;
      case ENTRY_PK_HEALTH:
      switch(game[game_being_edited].pk_health)
      {
       case 0: strcat(mstring, "None"); break;
       case 1: strcat(mstring, "Rare"); break;
       case 2: strcat(mstring, "Common"); break;
       case 3: strcat(mstring, "Very Common"); break;
      }
      break;
      case ENTRY_PK_WEAPON:
      switch(game[game_being_edited].pk_weapon)
      {
       case 0: strcat(mstring, "None"); break;
       case 1: strcat(mstring, "Rare"); break;
       case 2: strcat(mstring, "Common"); break;
       case 3: strcat(mstring, "Very Common"); break;
      }
      break;
      case ENTRY_PK_EQUIP:
      switch(game[game_being_edited].pk_equip)
      {
       case 0: strcat(mstring, "None"); break;
       case 1: strcat(mstring, "Rare"); break;
       case 2: strcat(mstring, "Common"); break;
       case 3: strcat(mstring, "Very Common"); break;
      }
      break;
      case ENTRY_PK_NAMES:
      switch(game[game_being_edited].name_boxes)
      {
       case 0: strcat(mstring, "No"); break;
       case 1: strcat(mstring, "Yes"); break;
      }
      break;
      case ENTRY_SERIAL_CONNECTION:
      switch(serial[0].game_type)
      {
       case SERIAL_LOCAL_ONLY: strcat(mstring, "Local Only"); break;
       case SERIAL_SERVER: strcat(mstring, "Host a Game"); break;
       case SERIAL_CLIENT: strcat(mstring, "Join a Game"); break;
      }
      break;
      case ENTRY_WHO_DAMAGE:
      switch(serial[0].who_decides_damage)
      {
       case DAM_SERVER: strcat(mstring, "Server"); break;
       case DAM_SUFFERER: strcat(mstring, "Sufferer"); break;
       case DAM_INFLICTOR: strcat(mstring, "Inflictor"); break;
      }
      break;
      case ENTRY_COM_PORT:
      switch(serial[0].com_port)
      {
       case 1: strcat(mstring, "COM1"); break;
       case 2: strcat(mstring, "COM2"); break;
      }
      break;
      case ENTRY_SERVER_ADDRESS:
      print_font = small_font;
      strcat(mstring, serial[0].port_string);
      break;
      //   case ENTRY_CONNECTION_TYPE:
      //   switch(serial[0].connection_type)
      //   {
      //    case NET_DRIVER_SERIAL_DOS: strcat(mstring, "Serial Ports"); break;
      //    case NET_DRIVER_IPX_DOS: strcat(mstring, "IPX Network"); break;
      //   }
      //   break;
      case ENTRY_CONNECT:
      switch(serial[0].game_type)
      {
       case SERIAL_LOCAL_ONLY: strcat(mstring, "No Connections To Make"); break;
       case SERIAL_SERVER: strcat(mstring, "Wait For Connections"); break;
       case SERIAL_CLIENT: strcat(mstring, "Connect To Server"); break;
      }
      break;
/*      case ENTRY_CONNECTIONS:
      switch(serial[0].game_type)
      {
       case SERIAL_LOCAL_ONLY: strcat(mstring, "No Connections To Make"); break;
       case SERIAL_SERVER: strcat(mstring, "Wait For Connections"); break;
       case SERIAL_CLIENT: strcat(mstring, "Connect To Server"); break;
      }
      break;*/
      case ENTRY_INIT_CONNECT:
      switch(serial[0].initialised)
      {
       case 0: strcat(mstring, "Initialise Connection"); break;
       case 1: strcat(mstring, "Connection Initialised"); break;
      }
      break;

      case ENTRY_KEY_UP1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_UP1])); break;
      case ENTRY_KEY_DOWN1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_DOWN1])); break;
      case ENTRY_KEY_LEFT1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_LEFT1])); break;
      case ENTRY_KEY_RIGHT1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_RIGHT1])); break;
      case ENTRY_KEY_SHOOT1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_SHOOT1])); break;
      case ENTRY_KEY_JUMP1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_JUMP1])); break;
      case ENTRY_KEY_CHANGE1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_CHANGE1])); break;
      case ENTRY_KEY_SCORE1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_SCORE1])); break;
      case ENTRY_KEY_NAMES1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_NAMES1])); break;
      case ENTRY_KEY_GRAPPLE1:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_GRAPPLE1])); break;
      case ENTRY_KEY_UP2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_UP2])); break;
      case ENTRY_KEY_DOWN2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_DOWN2])); break;
      case ENTRY_KEY_LEFT2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_LEFT2])); break;
      case ENTRY_KEY_RIGHT2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_RIGHT2])); break;
      case ENTRY_KEY_SHOOT2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_SHOOT2])); break;
      case ENTRY_KEY_JUMP2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_JUMP2])); break;
      case ENTRY_KEY_CHANGE2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_CHANGE2])); break;
      case ENTRY_KEY_SCORE2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_SCORE2])); break;
      case ENTRY_KEY_NAMES2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_NAMES2])); break;
      case ENTRY_KEY_GRAPPLE2:
      strcat(mstring, scancode_to_keyname(cmd_key [CMD_KEY_GRAPPLE2])); break;

     }

    }
    
 strcat(mstring, "   ");

// int arrow_distance = 0;

// rectfill(menu_screen, 0, menu_top + (line_length * scrline), window_x_sp, menu_top + (line_length * scrline) + 20, 0);
 if (video_mode == 0)
  textprintf_centre(menu_screen, print_font, window_x_sp / 2, menu_top + (line_length * scrline), colour, mstring);
   else
   {
    textprintf_right(menu_screen, print_font, 250, menu_top + (line_length * scrline), colour, mstring);
    if (mentry[mcount].special == SPECIAL_ESCAPE)
    {
      text_mode(-1);
//      arrow_distance = 150 + text_length(large_font, mstring) / 2 + 50;
//      textprintf_centre(menu_screen, large_font, arrow_distance, menu_top + (line_length * scrline), COLOUR_YELLOW4, ">>");
        textprintf(menu_screen, print_font, 256 - (scanline_x / 2) % 7, menu_top + (line_length * scrline), COLOUR_BLUE4, "<<<");
        rectfill(menu_screen, 250, menu_top + (line_length * scrline) + 2, 255, menu_top + (line_length * scrline) + text_height(large_font), 0);
         // text_height should be large_font even if small_font is being used.
        rectfill(menu_screen, 270, menu_top + (line_length * scrline) + 2, 285, menu_top + (line_length * scrline) + text_height(large_font), 0);
//      textprintf(menu_screen, large_font, 250, menu_top + (line_length * scrline), COLOUR_BLUE4, "<<");
    }
     else
     {
      if (mentry[mcount].response_type == RESPONSE_CHANGE_MENU)
      {
        text_mode(-1);
//      arrow_distance = 150 + text_length(large_font, mstring) / 2 + 50;
//      textprintf_centre(menu_screen, large_font, arrow_distance, menu_top + (line_length * scrline), COLOUR_YELLOW4, ">>");
        textprintf(menu_screen, print_font, 250 + (scanline_x / 2) % 7, menu_top + (line_length * scrline), COLOUR_YELLOW4, ">>>");
        rectfill(menu_screen, 250, menu_top + (line_length * scrline) + 2, 255, menu_top + (line_length * scrline) + text_height(large_font), 0);
        rectfill(menu_screen, 270, menu_top + (line_length * scrline) + 2, 285, menu_top + (line_length * scrline) + text_height(large_font), 0);
      }
      if (mentry[mcount].response_type == RESPONSE_CHOOSE_WEAPON)
      {
        text_mode(-1);
        if (mentry[mcount].entry_index >= ENTRY_EQUIP_1
             && mentry[mcount].entry_index <= ENTRY_EQUIP_8)
        {
         switch(game[0].equip_status [mentry[mcount].entry_index - ENTRY_EQUIP_1])
         {
          case 2:
          case 4:
          textprintf(menu_screen, small_font, 250, menu_top + (line_length * scrline) + 2, COLOUR_BLUE2, "Common");
          break;
          case 3:
          case 5:
          textprintf(menu_screen, small_font, 250, menu_top + (line_length * scrline) + 2, COLOUR_BLUE1, "Rare");
          break;
         }
        }
         else
          switch(game[0].weapon_status [mentry[mcount].entry_index - ENTRY_WEAPON_1 + 2 + NO_EQUIP])
          {
           case 2:
           case 4:
           textprintf(menu_screen, small_font, 250, menu_top + (line_length * scrline) + 2, COLOUR_BLUE2, "Common");
           break;
           case 3:
           case 5:
           textprintf(menu_screen, small_font, 250, menu_top + (line_length * scrline) + 2, COLOUR_BLUE1, "Rare");
           break;
          }
      }

      if (mentry[mcount].response_type == RESPONSE_START_GAME
          || mentry[mcount].response_type == RESPONSE_QUICKSTART)
      {
        text_mode(-1);
//      arrow_distance = 150 + text_length(large_font, mstring) / 2 + 50;
//      textprintf_centre(menu_screen, large_font, arrow_distance, menu_top + (line_length * scrline), COLOUR_YELLOW4, ">>");
//        textprintf(menu_screen, large_font, 250 + (scanline_x / 10) % 5 - ((scanline_x + 5) / 10) % 5, menu_top + (line_length * scrline), COLOUR_RED4, "Go!");
//        textprintf(menu_screen, large_font, 250, menu_top + (line_length * scrline), COLOUR_RED1 + (scanline_x / 5) % 4, "Go!");
//        textprintf(menu_screen, large_font, 250, menu_top + (line_length * scrline), COLOUR_RED2 + (scanline_x / 20) % 3, "Go!");
        textprintf(menu_screen, print_font, 250, menu_top + (line_length * scrline), COLOUR_RED4, "Go!");
      }
     }
   }

}

void append_colour_name(char mstring [100], int input)
{
 switch(input)
 {
  case COLOUR_RED4:
   strcat(mstring, "Red");
   break;
  case COLOUR_BLUE4:
   strcat(mstring, "Blue");
   break;
  case COLOUR_GREEN4:
   strcat(mstring, "Green");
   break;
  case COLOUR_YELLOW4:
   strcat(mstring, "Gold");
   break;
  case COLOUR_BROWN4:
   strcat(mstring, "Brown");
   break;
  case COLOUR_GREY4:
   strcat(mstring, "Grey");
   break;
  case COLOUR_GREY8:
   strcat(mstring, "Silver");
   break;
  default:
   strcat(mstring, "Invalid Colour");
   break;
 }

}


int ai_editing(int which_option)
{
 return (which_option - ENTRY_NAME_AI1) / AI_MENU_ENTRIES;
}


int ai_config_string(int which_option, char mstring [150], int colour)
{

char itstring [5];

int ai_option = ((which_option - ENTRY_NAME_AI1) % AI_MENU_ENTRIES) + ENTRY_NAME_AI1;
int ai_edited = ai_editing(which_option);

     switch(ai_option)
     {
      case ENTRY_NAME_AI1:
      if (strlen(ai_config[ai_edited].name) == 0) strcat(mstring, "<Random Name>");
       else strcat(mstring, ai_config[ai_edited].name);
      if (ai_config[ai_edited].active == 0 && colour == 2)
       colour = 109;
      break;
      case ENTRY_ACTIVE_AI1:
      switch(ai_config[ai_edited].active)
      {
       case 0: strcat(mstring, " - Off"); break;
       case 1: strcat(mstring, " - On"); break;
      }
      if (ai_config[ai_edited].active == 0 && colour == 2)
       colour = 109;
      break;
      case ENTRY_SKILL_AI1:
      switch(ai_config[ai_edited].skill)
      {
       case 0: strcat(mstring, " - Not So Bright"); break;
       case 1: strcat(mstring, " - Smarter"); break;
       case 2: strcat(mstring, " - Really Smart"); break;
      }
      if (ai_config[ai_edited].active == 0 && colour == 2)
       colour = 109;
      break;
      case ENTRY_TEAM_AI1:
      switch(ai_config[ai_edited].team)
      {
       case TEAM_NONE: strcat(mstring, " - None"); break;
       case TEAM_RED: strcat(mstring, " - Red"); break;
       case TEAM_BLUE: strcat(mstring, " - Blue"); break;
       case TEAM_GREEN: strcat(mstring, " - Green"); break;
       case TEAM_GOLD: strcat(mstring, " - Gold"); break;
       case TEAM_GREY: strcat(mstring, " - Grey"); break;
       case TEAM_BROWN: strcat(mstring, " - Brown"); break;
      }
      if (ai_config[ai_edited].active == 0 && colour == 2)
       colour = 109;
      break;
      case ENTRY_ATYPE_AI1:
      switch(ai_config[ai_edited].atype)
      {
       case 0: strcat(mstring, " - Chooses"); break;
/*       case 1: strcat(mstring, " - Light"); break;
       case 2: strcat(mstring, " - Medium"); break;
       case 3: strcat(mstring, " - Heavy"); break;
       case 4: strcat(mstring, " - J-Light"); break;
       case 5: strcat(mstring, " - J-Medium"); break;
       case 6: strcat(mstring, " - J-Heavy"); break;
       case 7: strcat(mstring, " - R-Light"); break;
       case 8: strcat(mstring, " - R-Medium"); break;
       case 9: strcat(mstring, " - R-Heavy"); break;*/
       case 1: strcat(mstring, " - Soldier"); break;
       case 2: strcat(mstring, " - Cyborg"); break;
       case 3: strcat(mstring, " - Crusher"); break;
       case 4: strcat(mstring, " - Walker"); break;
       case 5: strcat(mstring, " - Scout"); break;
       case 6: strcat(mstring, " - Bug"); break;
       case 7: strcat(mstring, " - Skeleton"); break;
       case 8: strcat(mstring, " - Demon"); break;
       case 9: strcat(mstring, " - Snail"); break;
      }
      if (ai_config[ai_edited].active == 0 && colour == 2)
       colour = 109;
      break;
      case ENTRY_SOLDIER_AI1:
      switch(ai_config[ai_edited].soldier)
      {
       case SOLDIER_SOLDIER: strcat(mstring, " - Soldier"); break;
       case SOLDIER_WALKER: strcat(mstring, " - Walker"); break;
       case NO_SOLDIER_FILES: strcat(mstring, " - Chooses"); break;
      }
      if (ai_config[ai_edited].active == 0 && colour == 2)
       colour = 109;
      break;
      case ENTRY_HANDICAP_AI1:
      strcat(mstring, my_itoa(ai_config[ai_edited].handicap, itstring, 10));
      if (ai_config[ai_edited].active == 0 && colour == 2)
       colour = 109;
      break;
     }

  return colour;

}


void copy_gametype(int game_from, int game_to)
{
int wcount;

  strcpy(game[game_to].gname, game[game_from].gname);
  game[game_to].gravity = game[game_from].gravity;
  game[game_to].health_amount = game[game_from].health_amount;
  game[game_to].bullet_speed = game[game_from].bullet_speed;
  game[game_to].impact_damage = game[game_from].impact_damage;
  game[game_to].reload_time = game[game_from].reload_time;
  game[game_to].game_type = game[game_from].game_type;
  game[game_to].score_type = game[game_from].score_type;
  game[game_to].score_limit = game[game_from].score_limit;
  game[game_to].lives_type = game[game_from].lives_type;
  game[game_to].lives_each = game[game_from].lives_each;
//  game[game_to].max_x = game[game_from].max_x;
//  game[game_to].max_y = game[game_from].max_y;
  game[game_to].pk_number = game[game_from].pk_number;
  game[game_to].fruit_no = game[game_from].fruit_no;
  game[game_to].penalty = game[game_from].penalty;
  game[game_to].th_base_no = game[game_from].th_base_no;
  game[game_to].pk_time_between = game[game_from].pk_time_between;
  game[game_to].lightsourcing = game[game_from].lightsourcing;
  game[game_to].pk_health = game[game_from].pk_health;
  game[game_to].pk_weapon = game[game_from].pk_weapon;
  game[game_to].pk_equip = game[game_from].pk_equip;
  game[game_to].name_boxes = game[game_from].name_boxes;
  game[game_to].starting_weapons = game[game_from].starting_weapons;
  game[game_to].fast_bullets = game[game_from].fast_bullets;
  game[game_to].bullets_explode = game[game_from].bullets_explode;
  game[game_to].blast_bullets = game[game_from].blast_bullets;
  game[game_to].show_map = game[game_from].show_map;
  game[game_to].soft_dirt = game[game_from].soft_dirt;
  game[game_to].rechoose_atype = game[game_from].rechoose_atype;
  game[game_to].reset_weapons = game[game_from].reset_weapons;
  game[game_to].unlimited_clips = game[game_from].unlimited_clips;
  game[game_to].atypes_avail = game[game_from].atypes_avail;

  for (wcount = 0; wcount < NO_WEAPONS; wcount ++)
  {
   game[game_to].weapon_status [wcount] = game[game_from].weapon_status [wcount];
  }

  for (wcount = 0; wcount < NO_EQUIP; wcount ++)
  {
   game[game_to].equip_status [wcount] = game[game_from].equip_status [wcount];
  }

}



void update_screen(void)
{

 if (video_mode == 1)
  blit(menu_screen, screen, 0, 0, 0, 0, 640, 480);
   else
    blit(menu_screen, screen, 0, 0, 0, 0, 320, 200);

 int hline_x = random() % 600;
 int i, j;

 if (options[0].fuzzy_menu)
 {
    if (scanline_x % 4 == 0)
    {
     drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);
     rectfill(menu_screen, 0, 0, 640, 480, FADE_OUT);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    }
/*    if (random() % 3 == 0)
    {
     j = COLOUR_GREEN1;
     if (random() % 5 == 0)
      j = COLOUR_GREEN2;
     circle(menu_screen, 320, 240, 150, j);
     hline(menu_screen, 470, 240, 640, j);
     hline(menu_screen, 0, 240, 170, j);
     vline(menu_screen, 320, 0, 90, j);
     vline(menu_screen, 320, 390, 480, j);
     if (random() % 3 == 0 && j == COLOUR_GREEN2)
     {
      j = COLOUR_GREEN1;
      circle(menu_screen, 320, 240, 149, j);
      circle(menu_screen, 320, 240, 151, j);
      hline(menu_screen, 470, 241, 640, j);
      hline(menu_screen, 470, 239, 640, j);
      hline(menu_screen, 0, 240, 170, j);
      hline(menu_screen, 0, 241, 170, j);
      hline(menu_screen, 0, 239, 170, j);
      vline(menu_screen, 320, 0, 90, j);
      vline(menu_screen, 321, 0, 90, j);
      vline(menu_screen, 319, 0, 90, j);
      vline(menu_screen, 320, 390, 480, j);
      vline(menu_screen, 321, 390, 480, j);
      vline(menu_screen, 319, 390, 480, j);
     }
    }
    if (random() % 3 == 0)
    {
     j = COLOUR_GREEN1;
     if (random() % 20 == 0)
      j = COLOUR_GREEN2;
     circle(menu_screen, 320, 240, 140, j);
     for (i = 0; i < 10; i ++)
     {
      line(menu_screen, 320 + cos(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 140,
      240 + sin(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 140,
      320 + cos(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 150,
      240 + sin(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 150,
      j);
     }
     for (i = 0; i < 10; i ++)
     {
      line(menu_screen, 320 + cos(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 140,
      240 + sin(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 140,
      320 + cos(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 120,
      240 + sin(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 120,
      j);
     }
    }*/
    if (random() % 40 == 0)
     hline(menu_screen, hline_x, random() % 480, hline_x + random() % 10 + 2, COLOUR_GREEN4);
    if (random() % 80 == 0)
    {
     hline_x = random() % 440;
     vline(menu_screen, random() % 640, hline_x, hline_x + random() % 20 + 2, COLOUR_GREEN4);
    }
    if (random() % 500 == 0)
     circle(menu_screen, random() % 600, random() % 440, 3 + random() % 40, COLOUR_GREEN4);
    if (random() % 2000 == 0)
     circlefill(menu_screen, random() % 600, random() % 440, 3 + random() % 40, COLOUR_GREEN4);
 }
  else
   clear_to_color(menu_screen, 0);


 if (video_mode == 1)
 {
    if (random() % 3 == 0 || options[0].fuzzy_menu == 0)
    {
     j = COLOUR_GREEN1;
     if (random() % 5 == 0)
      j = COLOUR_GREEN2;
     circle(menu_screen, 520, 380, 70, j);
     hline(menu_screen, 590, 380, 640, j);
     hline(menu_screen, 0, 380, 450, j);
     vline(menu_screen, 520, 0, 310, j);
     vline(menu_screen, 520, 450, 480, j);
     if (random() % 3 == 0 && j == COLOUR_GREEN2 && options[0].fuzzy_menu == 1)
     {
      j = COLOUR_GREEN1;
      circle(menu_screen, 520, 380, 69, j);
      circle(menu_screen, 520, 380, 71, j);
      hline(menu_screen, 590, 381, 640, j);
      hline(menu_screen, 590, 379, 640, j);
      hline(menu_screen, 0, 380, 450, j);
      hline(menu_screen, 0, 381, 450, j);
      hline(menu_screen, 0, 379, 450, j);
      vline(menu_screen, 520, 0, 310, j);
      vline(menu_screen, 521, 0, 310, j);
      vline(menu_screen, 519, 0, 310, j);
      vline(menu_screen, 520, 450, 480, j);
      vline(menu_screen, 521, 450, 480, j);
      vline(menu_screen, 519, 450, 480, j);
     }
    }
    if (random() % 3 == 0 || options[0].fuzzy_menu == 0)
    {
     j = COLOUR_GREEN1;
     if (random() % 20 == 0)
      j = COLOUR_GREEN2;
     circle(menu_screen, 520, 380, 60, j);
     for (i = 0; i < 10; i ++)
     {
      line(menu_screen, 520 + cos(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 50,
      380 + sin(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 50,
      520 + cos(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 59,
      380 + sin(((float) (scanline_x + (PI * i * 150)) / 150) * PI) * 59,
      j);
     }
     for (i = 0; i < 10; i ++)
     {
      line(menu_screen, 520 + cos(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 61,
      380 + sin(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 61,
      520 + cos(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 69,
      380 + sin(((float) ((640 - scanline_x) + (PI * i * 150)) / 150) * PI) * 69,
      j);
     }
    }
   } // end if video_mode == 1
   

}


// Assumes timer function (tickover) has been set up.
void run_menu_screen(void)
{

//  int i, j;

// do
// {
//   clear_keybuf();

//   while (ticked == 0)
//   {
//    rest(1);
//    if (keypressed() != 0)
//     return;
//   }
//   ticked --;
  drawing_mode(DRAW_MODE_TRANS, NULL, NULL, 0);

    vline(menu_screen, scanline_x % 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, scanline_x % 100 + 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, scanline_x % 100 + 200, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, scanline_x % 100 + 300, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, scanline_x % 100 + 400, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, scanline_x % 100 + 500, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, scanline_x % 100 + 600, 0, screen_height, TRANS_YELLOW);

    vline(menu_screen, 100 - scanline_x % 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, 200 - scanline_x % 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, 300 - scanline_x % 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, 400 - scanline_x % 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, 500 - scanline_x % 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, 600 - scanline_x % 100, 0, screen_height, TRANS_YELLOW);
    vline(menu_screen, 700 - scanline_x % 100, 0, screen_height, TRANS_YELLOW);

    hline(menu_screen, 0, scanline_x % 100, screen_width, TRANS_RED);
    hline(menu_screen, 0, scanline_x % 100 + 100, screen_width, TRANS_GREEN);
    hline(menu_screen, 0, scanline_x % 100 + 200, screen_width, TRANS_GREEN);
    hline(menu_screen, 0, scanline_x % 100 + 300, screen_width, TRANS_GREEN);
    hline(menu_screen, 0, scanline_x % 100 + 400, screen_width, TRANS_GREEN);

    hline(menu_screen, 0, 100 - scanline_x % 100, screen_width, TRANS_RED);
    hline(menu_screen, 0, 200 - scanline_x % 100, screen_width, TRANS_GREEN);
    hline(menu_screen, 0, 300 - scanline_x % 100, screen_width, TRANS_GREEN);
    hline(menu_screen, 0, 400 - scanline_x % 100, screen_width, TRANS_GREEN);
    hline(menu_screen, 0, 500 - scanline_x % 100, screen_width, TRANS_GREEN);

  if (video_mode == 1)
  {

//   circlefill(menu_screen, prand(640), prand(480), 3 + prand(50), TRANS_YELLOW);
//     line(menu_screen,


  }

    
  drawing_mode(DRAW_MODE_SOLID, NULL, NULL, 0);


   if (ticked > 0)
   {
    scanline_x ++;
    if (scanline_x >= (screen_width / 100) * 100 + 100)
     scanline_x = 0;
    ticked --;
   }

    update_screen();
 // } while (keypressed() == 0);


}


//void run_menu_sparkles

/*
void make_weapon_menu(void)
{

 int wi = 0;
 int wcount = 0;

 do
 {
  wi++;
 } while (cmenu[wi].entry_index != ENTRY_NULL);

 for (wcount = 2; wcount < NO_WEAPONS; wcount ++)
 {
  cmenu[wi].entry_index = ENTRY_WEAPON + wcount;
  cmenu[wi].response_type = RESPONSE_CYCLE;
  strcpy(cmenu[wi].entry_name, wlist [wcount].long_name);
//  strcpy(cmenu[wi].entry_description, "Choose the status of this weapon.");
  wi ++;
 }

 cmenu[wi].entry_index = ENTRY_NULL;

}
*/

void quit_game(void)
{

      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
//      clrscr();
      allegro_message("\n\rBye bye!                           ");
//      free(palet);
      exit(0);

}


int acceptable_char(int ascode)
{
 if (ascode >= 32 && ascode <= 126) return 1;
 return 0;
}


char *scancode_to_keyname(int scanc)
{

switch(scanc)
{
case KEY_A: return " a";
case KEY_B: return " b";
case KEY_C: return " c";
case KEY_D: return " d";
case KEY_E: return " e";
case KEY_F: return " f";
case KEY_G: return " g";
case KEY_H: return " h";
case KEY_I: return " i";
case KEY_J: return " j";
case KEY_K: return " k";
case KEY_L: return " l";
case KEY_M: return " m";
case KEY_N: return " n";
case KEY_O: return " o";
case KEY_P: return " p";
case KEY_Q: return " q";
case KEY_R: return " r";
case KEY_S: return " s";
case KEY_T: return " t";
case KEY_U: return " u";
case KEY_V: return " v";
case KEY_W: return " w";
case KEY_X: return " x";
case KEY_Y: return " y";
case KEY_Z: return " z";
case KEY_0: return " 0";
case KEY_1: return " 1";
case KEY_2: return " 2";
case KEY_3: return " 3";
case KEY_4: return " 4";
case KEY_5: return " 5";
case KEY_6: return " 6";
case KEY_7: return " 7";
case KEY_8: return " 8";
case KEY_9: return " 9";
case KEY_0_PAD: return " Numpad Ins";
case KEY_1_PAD: return " Numpad 1";
case KEY_2_PAD: return " Numpad 2";
case KEY_3_PAD: return " Numpad 3";
case KEY_4_PAD: return " Numpad 4";
case KEY_5_PAD: return " Numpad 5";
case KEY_6_PAD: return " Numpad 6";
case KEY_7_PAD: return " Numpad 7";
case KEY_8_PAD: return " Numpad 8";
case KEY_9_PAD: return " Numpad 9";
case KEY_F1: return " F1";
case KEY_F2: return " F2";
case KEY_F3: return " F3";
case KEY_F4: return " F4";
case KEY_F5: return " F5";
case KEY_F6: return " F6";
case KEY_F7: return " F7";
case KEY_F8: return " F8";
case KEY_F9: return " F9";
case KEY_F10: return " F10";
case KEY_F11: return " F11";
case KEY_F12: return " F12";
case KEY_ESC: return " Escape"; // invalid!
case KEY_TILDE: return " `";
case KEY_MINUS: return " -";
case KEY_EQUALS: return " b";
case KEY_BACKSPACE: return " Backspace";
case KEY_TAB: return " Tab";
case KEY_OPENBRACE: return " {";
case KEY_CLOSEBRACE: return " }";
case KEY_ENTER: return " Enter";
case KEY_COLON: return " ;";
case KEY_QUOTE: return " '";
case KEY_BACKSLASH: return " \\";
case KEY_BACKSLASH2: return " \\"; // ????
case KEY_COMMA: return " ,";
case KEY_STOP: return " .";
case KEY_SLASH: return " /";
case KEY_SPACE: return " Space";
case KEY_INSERT: return " Insert";
case KEY_DEL: return " Delete";
case KEY_HOME: return " Home";
case KEY_END: return " End";
case KEY_PGUP: return " Page Up";
case KEY_PGDN: return " Page Down";
case KEY_LEFT: return " Left";
case KEY_RIGHT: return " Right";
case KEY_UP: return " Up";
case KEY_DOWN: return " Down";
case KEY_SLASH_PAD: return " Keypad /";
case KEY_ASTERISK: return " Keypad *";
case KEY_MINUS_PAD: return " Keypad -";
case KEY_PLUS_PAD: return " Keypad +";
case KEY_DEL_PAD: return " Keypad .";
case KEY_ENTER_PAD: return " Keypad Enter";
case KEY_PRTSCR: return " Print Screen";
case KEY_PAUSE: return " Pause";
case KEY_ABNT_C1: return " Weird Foreign Key"; // may as well put these in
case KEY_YEN: return " Yen";
case KEY_KANA: return " Kana";
case KEY_CONVERT: return " Convert";
case KEY_NOCONVERT: return " NOCONVERT";
case KEY_AT: return " At";
case KEY_CIRCUMFLEX: return " Circumflex";
case KEY_COLON2: return " 2nd Colon";
case KEY_KANJI: return " Kanji";
case KEY_LSHIFT: return " Left Shift";
case KEY_RSHIFT: return " Right Shift";
case KEY_LCONTROL: return " Left Control";
case KEY_RCONTROL: return " Right Control";
case KEY_ALT: return " Alt";
case KEY_ALTGR: return " AltGr";
case KEY_LWIN: return " Left Win";
case KEY_RWIN: return " Right Win";
case KEY_MENU: return " Menu";
case KEY_SCRLOCK: return " Scroll Lock";
case KEY_NUMLOCK: return " Num Lock";
case KEY_CAPSLOCK: return " Caps Lock";
//case KEY_: return " ";

default: return " unknown key";

/*

          KEY_ESC, KEY_TILDE, KEY_MINUS, KEY_EQUALS,
          KEY_BACKSPACE, KEY_TAB, KEY_OPENBRACE, KEY_CLOSEBRACE,
          KEY_ENTER, KEY_COLON, KEY_QUOTE, KEY_BACKSLASH,
          KEY_BACKSLASH2, KEY_COMMA, KEY_STOP, KEY_SLASH,
          KEY_SPACE,

*/

}


}



/*  game[gi].gravity = 30;
  game[gi].bullet_speed = 10;
  game[gi].impact_damage = 0;
  game[gi].game_type = GAME_KILL;
  game[gi].score_type = SCORE_KILLS_ONLY;
  game[gi].score_limit = 10;
  game[gi].lives_type = LIVES_LIMITED;
  game[gi].lives_each = 10;
  game[gi].max_x = 20;
  game[gi].max_y = 20;*/



//void choose_weapon(void)


