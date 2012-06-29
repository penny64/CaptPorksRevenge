/*
Captain Pork's World of Violence
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

File: globvar.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - extern declarations of global variables.
 
*/

//extern int team_colour_list [7];

//extern char grid [GRID_X] [GRID_Y];
extern char keypress [NO_CMDS];
extern char cmd_buffer [NO_ACTORS] [NO_CMDS]; // don't need NO_CMDS, but it'll do

extern char cmd_key [NO_CMDS];
extern char no_players;

extern int tick_length;
extern int turn_length;

extern int pseudorandom;

extern int game_played;

extern FONT *small_font;
extern FONT *large_font;


/*struct act_struct
{
 char aclass;
 unsigned int x, y;
 int x_speed, y_speed;
};*/

extern struct act_struct actor [NO_ACTORS];

extern struct user_struct user [NO_USERS];

/*struct player_struct
{
 char actor_controlled;
};*/

extern struct player_struct player [3];
extern struct team_struct team [NO_TEAMS];

extern struct game_struct game [NO_GAMES];
extern struct arena_struct arena [2];

extern struct bullet_list bullet [MAX_BULLETS];
extern struct cloud_list cloud [MAX_CLOUDS];

extern struct armoury wlist [NO_WEAPONS];

extern struct pickup_list pickup [MAX_PICKUPS];
extern struct serial_game serial [1];

extern struct option_struct options [1];

