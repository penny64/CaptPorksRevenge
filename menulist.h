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

File: menulist.h
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)
15/6/03 - Revenge: Version 1.0 finalised (CP)

This file contains:
 - scripts (in my special menu scripting language, PorkScript) controlling
   the game's menus.
 
*/

#define AI_MENU_ENTRIES 7


enum
{
SPECIAL_NONE,
SPECIAL_ESCAPE
};

enum
{
RESPONSE_START_GAME,
//RESPONSE_CHALLENGE,
RESPONSE_QUICKSTART,
RESPONSE_QUIT_GAME,
RESPONSE_CHANGE_MENU,
RESPONSE_RETURN_MENU,
RESPONSE_CYCLE,
RESPONSE_ENTER_KEY,
RESPONSE_ENTER_STRING,
RESPONSE_ENTER_SERVER_ADDRESS,
RESPONSE_NUMBER,
RESPONSE_SLIDER,
RESPONSE_SAVE_CONFIG,
RESPONSE_NONE,
RESPONSE_FINISH_CUSTOM,
RESPONSE_CYCLE_WEAPON,
RESPONSE_CYCLE_EQUIP,
RESPONSE_CHOOSE_WEAPON,
RESPONSE_CHOOSE_ATYPE,
RESPONSE_CHOOSE_TEAM,
RESPONSE_FINISH_CHOICE,
RESPONSE_CONNECT,
RESPONSE_INIT_CONNECT,
RESPONSE_SHUTDOWN_CONNECT,
RESPONSE_SELECT_ALL_AI,
RESPONSE_TEST_STEREO,
RESPONSE_WEAPON_SET
};

enum
{
ENTRY_NULL,
ENTRY_BEGIN_GAME,
ENTRY_START,
ENTRY_SELECT_PLAYERS,
ENTRY_SELECT_TEAMS,
ENTRY_CHALLENGE,
ENTRY_QUICKSTART,
ENTRY_MELEE,
ENTRY_MELEE_TEAM,
ENTRY_DUEL,
ENTRY_DUEL_TEAM,
ENTRY_CAPFLAG,
ENTRY_FRUIT,
ENTRY_TAKEHOLD,
ENTRY_LAST_ONE,
ENTRY_GRAIL,
ENTRY_TOURNAMENT,

ENTRY_QUICKSTART_DIFFICULTY,
ENTRY_QUICKSTART_DIRT,
ENTRY_QUICKSTART_LIGHTSOURCE,
ENTRY_QUICKSTART_WEAPONS,
/*ENTRY_SELECT_AI,
ENTRY_SELECT_AI_SKILL,
ENTRY_SELECT_AI_TEAMS,*/
ENTRY_DEFINE_KEYS,
ENTRY_EXIT,
ENTRY_RETURN,
ENTRY_SAVE_KEYS,
ENTRY_SAVE_PLAYERS,
ENTRY_CONFIG_PLAYERS,
ENTRY_NAME1,
ENTRY_NAME2,
ENTRY_COLOUR1_P1,
ENTRY_COLOUR2_P1,
ENTRY_COLOUR1_P2,
ENTRY_COLOUR2_P2,
ENTRY_HANDICAP1,
ENTRY_HANDICAP2,
ENTRY_TEAM1,
ENTRY_TEAM2,
ENTRY_SOLDIER1,
ENTRY_SOLDIER2,
ENTRY_KEY_UP1,
ENTRY_KEY_DOWN1,
ENTRY_KEY_LEFT1,
ENTRY_KEY_RIGHT1,
ENTRY_KEY_JUMP1,
ENTRY_KEY_SHOOT1,
ENTRY_KEY_CHANGE1,
ENTRY_KEY_SCORE1,
ENTRY_KEY_NAMES1,
ENTRY_KEY_GRAPPLE1,
ENTRY_KEY_UP2,
ENTRY_KEY_DOWN2,
ENTRY_KEY_LEFT2,
ENTRY_KEY_RIGHT2,
ENTRY_KEY_JUMP2,
ENTRY_KEY_SHOOT2,
ENTRY_KEY_CHANGE2,
ENTRY_KEY_SCORE2,
ENTRY_KEY_NAMES2,
ENTRY_KEY_GRAPPLE2,
ENTRY_TITLE,

ENTRY_SELECT_GAME,
ENTRY_CUSTOM_GAME,
ENTRY_GAME_1,
ENTRY_GAME_2,
ENTRY_GAME_3,
ENTRY_GAME_4,
ENTRY_GAME_5,
ENTRY_GAME_6,
ENTRY_BACK_TO_MAIN,
ENTRY_NAME_GAME,
ENTRY_GRAVITY,
ENTRY_IMPACT,
ENTRY_GAME_TYPE,
ENTRY_PK_NUMBER,
ENTRY_PK_TIME_BETWEEN,
ENTRY_PK_HEALTH,
ENTRY_PK_WEAPON,
ENTRY_PK_EQUIP,
ENTRY_PK_NAMES,
ENTRY_SCORE_LIMIT,
ENTRY_LIVES_LIMIT,
ENTRY_LEVEL_WIDTH,
ENTRY_LEVEL_HEIGHT,
ENTRY_SAVE_GAME,
ENTRY_NO_WEAPONS,
ENTRY_FAST_BULLETS,
ENTRY_BULLETS_EXPLODE,
ENTRY_SOFT_DIRT,
ENTRY_HEALTH_AMOUNT,
ENTRY_RELOAD_TIME,
ENTRY_RECHOOSE_ATYPE,
ENTRY_UNLIMITED_CLIPS,
ENTRY_RESET_WEAPONS,
ENTRY_FRUIT_NUMBER,
ENTRY_TH_BASE_NUMBER,
ENTRY_PENALTY,
ENTRY_SHOW_MAP,
ENTRY_BLAST_BULLETS,
ENTRY_ATYPES_AVAIL,
ENTRY_LIGHTSOURCING,
ENTRY_WEAPON_SET_NONE,
ENTRY_WEAPON_SET_BASIC,
ENTRY_WEAPON_SET_EXTENDED,
ENTRY_WEAPON_SET_FULL,

ENTRY_LEVEL_MENU,
ENTRY_LEVEL_COLOURS,
ENTRY_LEVEL_STYLE,
ENTRY_LEVEL_SOLIDS,
ENTRY_LEVEL_DIRT,
ENTRY_LEVEL_REDIRT,
ENTRY_AMBIENT_LIGHT,
ENTRY_LAMPS,
ENTRY_SAVE_LEVEL_CONFIG,
ENTRY_LOAD_LEVEL,


ENTRY_AI_MENU,
// if any more ai config entries added, must increase #define at top of file.
ENTRY_NAME_AI1,
ENTRY_SKILL_AI1,
ENTRY_TEAM_AI1,
ENTRY_ATYPE_AI1,
ENTRY_ACTIVE_AI1,
ENTRY_HANDICAP_AI1,
ENTRY_SOLDIER_AI1,
ENTRY_NAME_AI2,
ENTRY_SKILL_AI2,
ENTRY_TEAM_AI2,
ENTRY_ATYPE_AI2,
ENTRY_ACTIVE_AI2,
ENTRY_HANDICAP_AI2,
ENTRY_SOLDIER_AI2,
ENTRY_NAME_AI3,
ENTRY_SKILL_AI3,
ENTRY_TEAM_AI3,
ENTRY_ATYPE_AI3,
ENTRY_ACTIVE_AI3,
ENTRY_HANDICAP_AI3,
ENTRY_SOLDIER_AI3,
ENTRY_NAME_AI4,
ENTRY_SKILL_AI4,
ENTRY_TEAM_AI4,
ENTRY_ATYPE_AI4,
ENTRY_ACTIVE_AI4,
ENTRY_HANDICAP_AI4,
ENTRY_SOLDIER_AI4,
ENTRY_NAME_AI5,
ENTRY_SKILL_AI5,
ENTRY_TEAM_AI5,
ENTRY_ATYPE_AI5,
ENTRY_ACTIVE_AI5,
ENTRY_HANDICAP_AI5,
ENTRY_SOLDIER_AI5,
ENTRY_NAME_AI6,
ENTRY_SKILL_AI6,
ENTRY_TEAM_AI6,
ENTRY_ATYPE_AI6,
ENTRY_ACTIVE_AI6,
ENTRY_HANDICAP_AI6,
ENTRY_SOLDIER_AI6,
ENTRY_NAME_AI7,
ENTRY_SKILL_AI7,
ENTRY_TEAM_AI7,
ENTRY_ATYPE_AI7,
ENTRY_ACTIVE_AI7,
ENTRY_HANDICAP_AI7,
ENTRY_SOLDIER_AI7,
ENTRY_SAVE_AI_CONFIG,
ENTRY_SELECT_ALL_AI,

ENTRY_AI_IMPACT,
ENTRY_AI_CLIPS,
ENTRY_AI_WEAPONS,

ENTRY_OPTIONS_MENU,
ENTRY_SAVE_OPTIONS,
ENTRY_SERIAL_CONFIG,
ENTRY_SERIAL_CONNECTION,
ENTRY_CHANNEL,
ENTRY_CONNECT,
ENTRY_CONNECTIONS,
ENTRY_INIT_CONNECT,
ENTRY_SHUTDOWN_CONNECT,
ENTRY_WHO_DAMAGE,
ENTRY_SERVER_ADDRESS,

ENTRY_COM_PORT,
ENTRY_RUN_VSYNC,
ENTRY_STIPPLE_CLOUDS,
ENTRY_SHOW_FPS,
ENTRY_POSITIONAL_SOUND,
ENTRY_TEST_STEREO,
ENTRY_DISPLAY_DAMAGE,
ENTRY_FUZZY_MENU,
ENTRY_CONNECTION_TYPE,

ENTRY_ATYPE_LIGHT,
ENTRY_ATYPE_MEDIUM,
ENTRY_ATYPE_HEAVY,

ENTRY_TEAM_NONE,
ENTRY_TEAM_RED,
ENTRY_TEAM_BLUE,
ENTRY_TEAM_GREEN,
ENTRY_TEAM_GOLD,
ENTRY_TEAM_GREY,
ENTRY_TEAM_BROWN,

ENTRY_WEAPON_1,
ENTRY_WEAPON_2,
ENTRY_WEAPON_3,
ENTRY_WEAPON_4,
ENTRY_WEAPON_5,
ENTRY_WEAPON_6,
ENTRY_WEAPON_7,
ENTRY_WEAPON_8,
ENTRY_WEAPON_9,
ENTRY_WEAPON_10,
ENTRY_WEAPON_11,
ENTRY_WEAPON_12,
ENTRY_WEAPON_13,
ENTRY_WEAPON_14,
ENTRY_WEAPON_15,
ENTRY_WEAPON_16,
ENTRY_WEAPON_17,
ENTRY_WEAPON_18,
ENTRY_WEAPON_19,
ENTRY_WEAPON_20,
ENTRY_WEAPON_21,
ENTRY_WEAPON_22,
ENTRY_WEAPON_23,
ENTRY_WEAPON_24,
ENTRY_WEAPON_25,
ENTRY_WEAPON_26,
ENTRY_WEAPON_27,
ENTRY_WEAPON_28,
ENTRY_WEAPON_29,
ENTRY_WEAPON_30,
ENTRY_WEAPON_31,
ENTRY_WEAPON_32,
ENTRY_WEAPON_33,
ENTRY_WEAPON_34,
ENTRY_WEAPON_35,
ENTRY_WEAPON_36,
ENTRY_WEAPON_37,
ENTRY_WEAPON_38,
ENTRY_WEAPON_39,
ENTRY_WEAPON_40,
ENTRY_WEAPON_41,
ENTRY_WEAPON_42,
ENTRY_WEAPON_43,
ENTRY_WEAPON_44,
ENTRY_WEAPON_45,
ENTRY_WEAPON_46,
ENTRY_WEAPON_47,
ENTRY_WEAPON_48,
ENTRY_WEAPON_49,
ENTRY_WEAPON_50,
ENTRY_WEAPON_51,
ENTRY_WEAPON_52,
ENTRY_WEAPON_53,
ENTRY_WEAPON_54,
ENTRY_WEAPON_55,
ENTRY_END_WEAPONS,
ENTRY_EQUIP_1,
ENTRY_EQUIP_2,
ENTRY_EQUIP_3,
ENTRY_EQUIP_4,
ENTRY_EQUIP_5,
ENTRY_EQUIP_6,
ENTRY_EQUIP_7,
ENTRY_EQUIP_8,
ENTRY_END_EQUIP


};


 char melee_difficulty;
 char melee_dirt;
 char melee_lightsourced;


struct menu_format
{
 char *entry_name;
 char *entry_description;
 int entry_index;
 int response_type;
 int special;
 int unused;
};


struct menu_format main_menu [8] =
{

{
"Quickstart",
"Start a game without much further ado.$Make sure you've configured your keys and player options first.",
ENTRY_QUICKSTART,
RESPONSE_CHANGE_MENU,
0,
0
},
{
"Setup Game",
"Set up a game using customised game settings.",
ENTRY_BEGIN_GAME,
RESPONSE_CHANGE_MENU,
0,
0
},
/*{
"Challenge",
"Start a single-player challenge game.",
ENTRY_CHALLENGE,
RESPONSE_CHALLENGE,
0,
0
},*/
{
"Network",
"Set up a networked game.",
ENTRY_SERIAL_CONFIG,
RESPONSE_CHANGE_MENU,
0,
0
},
{
"Options",
"Set game options.",
ENTRY_OPTIONS_MENU,
RESPONSE_CHANGE_MENU,
0,
0
},
{
"Define Keys",
"Redefine the keyset.",
ENTRY_DEFINE_KEYS,
RESPONSE_CHANGE_MENU,
0,
0
},

{
"Configure Players",
"Choose the names and colours of your players.",
ENTRY_CONFIG_PLAYERS,
RESPONSE_CHANGE_MENU,
0,
0
},

{
"Exit",
"Leave the program.",
ENTRY_EXIT,
RESPONSE_QUIT_GAME,
SPECIAL_ESCAPE,
0
},

{
"",
"",
ENTRY_NULL,
0,
0,
0
}



};


struct menu_format quickstart_menu [19] =
{
{
"Challenge",
"Face a series of increasingly difficult levels.$1 Player only.",
ENTRY_CHALLENGE,
RESPONSE_QUICKSTART,
0,
0
},
{
"Duel",
"A duel between players or against an AI.$Recommended for networked games.",
ENTRY_DUEL,
RESPONSE_QUICKSTART,
0,
0
},
{
"Team Duel",
"A duel between players or against an AI.$Teams are activated, so use this mode if you want a networked team game without AIs.",
ENTRY_DUEL_TEAM,
RESPONSE_QUICKSTART,
0,
0
},
{
"Melee",
"Enter a random level to do battle with several AIs. 1 or 2 Players (or networked).",
ENTRY_MELEE,
RESPONSE_QUICKSTART,
0,
0
},
{
"Team Melee",
"A three-on-three melee. 1 or 2 Players (or networked).",
ENTRY_MELEE_TEAM,
RESPONSE_QUICKSTART,
0,
0
},
/*{
"Capture The Flag",
"A three-on-three game of Capture the Flag. 1 or 2 Players.",
ENTRY_CAPFLAG,
RESPONSE_QUICKSTART,
0,
0
},
{
"Quest For Fruit",
"A competition to eat as many delicious fruit as you can. 1 or 2 Players.",
ENTRY_FRUIT,
RESPONSE_QUICKSTART,
0,
0
},
{
"Take & Hold",
"A three-on-three game of Take & Hold. 1 or 2 Players.",
ENTRY_TAKEHOLD,
RESPONSE_QUICKSTART,
0,
0
},*/
{
"Last One Standing",
"Enter a random level to see who can stay around longest. 1 or 2 Players (or networked).",
ENTRY_LAST_ONE,
RESPONSE_QUICKSTART,
0,
0
},
/*{
"Quest For The Holy Grail",
"A three-on-three quest for one of the holiest relics ever. 1 or 2 Players.",
ENTRY_GRAIL,
RESPONSE_QUICKSTART,
0,
0
},
{
"Tournament",
"A series of melee levels. 1 or 2 Players.",
ENTRY_MELEE,
RESPONSE_QUICKSTART,
0,
0
},

*/

{
"Options",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Number of Players",
"Select one or two players on this computer.",
ENTRY_SELECT_PLAYERS,
RESPONSE_CYCLE, // really a number, but cycle is better
0,
0
},

{
"Player 1 Team - ",
"Shortcut to choose your player's team (If teams are relevant for the game mode you're playing).",
ENTRY_TEAM1,
RESPONSE_CYCLE,
0,
0
},

{
"Player 2 Team - ",
"Shortcut to choose your player's team (If teams are relevant for the game mode you're playing).",
ENTRY_TEAM2,
RESPONSE_CYCLE,
0,
0
},

{
"Difficulty",
"Affects how smart the AIs are. On Easy they also have a 75%% handicap, and on Very Hard they have special access to weapons (AI weapon cheat).",
ENTRY_QUICKSTART_DIFFICULTY,
RESPONSE_CYCLE,
0,
0
},

{
"Weapon Set",
"Determines the set of weapons available at the start and in boxes, and also how many each player gets to start with.",
ENTRY_QUICKSTART_WEAPONS,
RESPONSE_CYCLE,
0,
0
},

{
"Dirt",
"",
ENTRY_QUICKSTART_DIRT,
RESPONSE_CYCLE,
0,
0
},

{
"Dynamic Lightsourcing",
"Looks nice but is slow.",
ENTRY_QUICKSTART_LIGHTSOURCE,
RESPONSE_CYCLE,
0,
0
},

{
"Go Back",
"Go back to the main menu.",
ENTRY_RETURN,
RESPONSE_RETURN_MENU,
SPECIAL_ESCAPE,
0
},

{
"",
"",
ENTRY_NULL,
0,
0,
0
}




};

struct menu_format options_menu [9] =
{
{
"Fuzzy Menu",
"I kind of like the fuzzy menu mode. But you might just find it really annoying.",
ENTRY_FUZZY_MENU,
RESPONSE_CYCLE,
0,
0
},
{
"Screen Sync",
"Reduces flicker but slows the game on slower computers.",
ENTRY_RUN_VSYNC,
RESPONSE_CYCLE,
0,
0
},
{
"Clouds",
"Select how clouds are displayed. Transparent looks nicest but is slow.",
ENTRY_STIPPLE_CLOUDS,
RESPONSE_CYCLE,
0,
0
},

{
"Show FPS",
"Shows the frames per second, game cycles per second and slacktime.",
ENTRY_SHOW_FPS,
RESPONSE_CYCLE,
0,
0
},

{
"Display Damage",
"Displays a damage indicator when someone gets hurt. Only displays values of at least 1 hit point (so gradual damage from eg napalm is ignored).",
ENTRY_DISPLAY_DAMAGE,
RESPONSE_CYCLE,
0,
0
},

{
"Positional Stereo",
"Gives a stereo effect based on the position of each source of sound. The speakers can be reversed. Does nothing when a split screen is being used.",
ENTRY_POSITIONAL_SOUND,
RESPONSE_CYCLE,
0,
0
},

{
"Test Stereo",
"Plays a sound through the left speaker, then both speakers, then the right speaker, according to whether positional stereo is on, off or reversed.",
ENTRY_TEST_STEREO,
RESPONSE_TEST_STEREO,
0,
0
},

/*{
"Save Options To File",
"Makes the game save your new options to disk when you leave \
the game.",
ENTRY_SAVE_OPTIONS,
RESPONSE_SAVE_CONFIG,
0,
0
},*/
{
"Go Back",
"Go back to the main menu.",
ENTRY_RETURN,
RESPONSE_RETURN_MENU,
SPECIAL_ESCAPE,
0
},

{
"",
"",
ENTRY_NULL,
0,
0,
0
}


};


struct menu_format define_keys [25] =
{
{
"Player 1",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},
{
"Player 1 Up - ",
"The Up key makes your player aim upwards.",
ENTRY_KEY_UP1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Down - ",
"The Down key makes your player aim downwards.",
ENTRY_KEY_DOWN1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Left - ",
"The Left key makes your player move left.",
ENTRY_KEY_LEFT1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Right - ",
"The Right key makes your player move right.",
ENTRY_KEY_RIGHT1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Shoot - ",
"The Shoot key makes your player shoot your current weapon.",
ENTRY_KEY_SHOOT1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Jump - ",
"The Jump key makes your player jump up in the air, and let go of \
the grapple if it's active.",
ENTRY_KEY_JUMP1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Change - ",
"The Change key allows you to change your selected weapon by holding it \
down and moving left and right. Pressing Change and Jump fires your \
grappling hook, and Change and Shoot makes you pick up anything you're \
standing on.",
ENTRY_KEY_CHANGE1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Grapple - ",
"Activate your grappling hook.",
ENTRY_KEY_GRAPPLE1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Score - ",
"See the current list of scores.",
ENTRY_KEY_SCORE1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 1 Names - ",
"Display player names.",
ENTRY_KEY_NAMES1,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Player 2 Up - ",
"The Up key makes your player aim upwards.",
ENTRY_KEY_UP2,
RESPONSE_ENTER_KEY,
0,
0
},

{
"Player 2 Down - ",
"The Down key makes your player aim downwards.",
ENTRY_KEY_DOWN2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Left - ",
"The Left key makes your player move left.",
ENTRY_KEY_LEFT2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Right - ",
"The Right key makes your player move right.",
ENTRY_KEY_RIGHT2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Shoot - ",
"The Shoot key makes your player shoot your current weapon.",
ENTRY_KEY_SHOOT2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Jump - ",
"The Jump key makes your player jump up in the air, and let go of \
the grapple if it's active.",
ENTRY_KEY_JUMP2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Change - ",
"The Change key allows you to change your selected weapon by holding it \
down and moving left and right. Pressing Change and Jump fires your \
grappling hook, and Change and Shoot makes you pick up anything you're \
standing on.",
ENTRY_KEY_CHANGE2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Grapple - ",
"Activate your grappling hook.",
ENTRY_KEY_GRAPPLE2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Score - ",
"See the current list of scores.",
ENTRY_KEY_SCORE2,
RESPONSE_ENTER_KEY,
0,
0
},
{
"Player 2 Names - ",
"Display player names.",
ENTRY_KEY_NAMES2,
RESPONSE_ENTER_KEY,
0,
0
},
/*{
"Save Keys To File",
"Makes the game save your new keyboard values to disk when you leave \
the game.",
ENTRY_SAVE_KEYS,
RESPONSE_SAVE_CONFIG,
0,
0
},*/
{
"Go Back",
"Go back to the main menu.",
ENTRY_RETURN,
RESPONSE_RETURN_MENU,
SPECIAL_ESCAPE,
0
},

{
"",
"",
ENTRY_NULL,
0,
0,
0
}


};




struct menu_format player_menu [19] =
{
{
"Player 1",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Player 1 Name - ",
"Choose what your player is called.",
ENTRY_NAME1,
RESPONSE_ENTER_STRING,
0,
0
},

{
"Player 1 Colour 1 - ",
"Choose your player's first colour$(replaced by their team colour in team games).",
ENTRY_COLOUR1_P1,
RESPONSE_NUMBER,
0,
0
},
{
"Player 1 Colour 2 - ",
"Choose your player's second colour.",
ENTRY_COLOUR2_P1,
RESPONSE_NUMBER,
0,
0
},
{
"Player 1 Team - ",
"Choose your player's team.",
ENTRY_TEAM1,
RESPONSE_CYCLE,
0,
0
},
{
"Player 1 Handicap - ",
"Choose your player's handicap. A handicap of less than 100 reduces your player's health proportionally.",
ENTRY_HANDICAP1,
RESPONSE_NUMBER,
0,
0
},
/*{
"Player 1 Class",
"Choose your player's class.",
ENTRY_SOLDIER1,
RESPONSE_CYCLE,
0,
0
},*/
{
"Player 2",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Player 2 Name - ",
"Choose what your player is called.",
ENTRY_NAME2,
RESPONSE_ENTER_STRING,
0,
0
},
{
"Player 2 Colour 1 - ",
"Choose your player's first colour$(replaced by their team colour in team games).",
ENTRY_COLOUR1_P2,
RESPONSE_NUMBER,
0,
0
},
{
"Player 2 Colour 2 - ",
"Choose your player's second colour.",
ENTRY_COLOUR2_P2,
RESPONSE_NUMBER,
0,
0
},
{
"Player 2 Team - ",
"Choose your player's team.",
ENTRY_TEAM2,
RESPONSE_CYCLE,
0,
0
},
{
"Player 2 Handicap - ",
"Choose your player's handicap. A handicap of less than 100 reduces your player's health proportionally.",
ENTRY_HANDICAP2,
RESPONSE_NUMBER,
0,
0
},
/*{
"Player 2 Class",
"Choose your player's class.",
ENTRY_SOLDIER2,
RESPONSE_CYCLE,
0,
0
},*/
/*{
"Save players to file",
"Makes the game save your new options to disk when you leave \
the game.",
ENTRY_SAVE_PLAYERS,
RESPONSE_SAVE_CONFIG,
0,
0
},*/

{
"Go Back",
"Go back to the main menu.",
ENTRY_RETURN,
RESPONSE_RETURN_MENU,
SPECIAL_ESCAPE,
0
},

{
"",
"",
ENTRY_NULL,
0,
0,
0
}

};


struct menu_format games_menu [20] =
{
{
"Start Game",
"Start playing your currently selected game.",
ENTRY_START,
RESPONSE_START_GAME,
0,
0
},

{
"Number of Players",
"Select one or two players on this computer.",
ENTRY_SELECT_PLAYERS,
RESPONSE_CYCLE, // really a number, but cycle is better
0,
0
},

{
"Teams",
"Are there teams, or is everyone on their own?",
ENTRY_SELECT_TEAMS,
RESPONSE_CYCLE, 0, 0},

{
"Level Menu",
"Change the settings for generating the level.",
ENTRY_LEVEL_MENU,
RESPONSE_CHANGE_MENU, 0, 0},

{
"AI Players Menu",
"Set up the AI players.",
ENTRY_AI_MENU,
RESPONSE_CHANGE_MENU, 0, 0},

/*{
"AI Players: ",
"How many AI players to include in the game.",
ENTRY_SELECT_AI,
RESPONSE_NUMBER, 0, 0},

{
"AI Skill: ",
"How skillful are the AI players?",
ENTRY_SELECT_AI_SKILL,
RESPONSE_CYCLE, 0, 0},

{
"AI Teams: ",
"How are the AI players distributed in teams? (irrelevant if Teams is off)",
ENTRY_SELECT_AI_TEAMS,
RESPONSE_CYCLE, 0, 0},

{
"Play: ",
"Which game configuration is being used.",
ENTRY_SELECT_GAME,
RESPONSE_NONE,
0,
0
},
*/
{
"Custom game",
"Edit the current game configuration.",
ENTRY_CUSTOM_GAME,
RESPONSE_CHANGE_MENU,
0,
0
},

{
"Game Profiles",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Game A: ",
"Customise this game configuration.",
ENTRY_GAME_1,
RESPONSE_CHANGE_MENU,
0,
0
},

{
"Game B: ",
"Customise this game configuration.",
ENTRY_GAME_2,
RESPONSE_CHANGE_MENU,
0,
0
},

{
"Game C: ",
"Customise this game configuration.",
ENTRY_GAME_3,
RESPONSE_CHANGE_MENU,
0,
0
},

{
"Game D: ",
"Customise this game configuration.",
ENTRY_GAME_4,
RESPONSE_CHANGE_MENU,
0,
0
},

{
"Game E: ",
"Customise this game configuration.",
ENTRY_GAME_5,
RESPONSE_CHANGE_MENU,
0,
0
},

/*{
"Game F: ",
"Customise this game configuration.",
ENTRY_GAME_6,
RESPONSE_CHANGE_MENU,
0,
0
},*/
{
"Go Back",
"Go back to the main menu.",
ENTRY_BACK_TO_MAIN,
RESPONSE_CHANGE_MENU,
SPECIAL_ESCAPE,
0
},



{
"",
"",
ENTRY_NULL,
0,
0,
0
}


};

// must be large as it must accomodate weapons
struct menu_format custom_menu [102] =
{
{
"Select this game and return",
"Go back to the previous menu.",
ENTRY_RETURN,
RESPONSE_FINISH_CUSTOM,
SPECIAL_ESCAPE,
0
},


{
"Name: ",
"Name this customised game.",
ENTRY_NAME_GAME,
RESPONSE_ENTER_STRING,
0,
0
},

{
"Rules",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Game Type: ",
"What type of game is it? See the manual for a description of each one.",
ENTRY_GAME_TYPE,
RESPONSE_CYCLE,
0,
0
},

/*{
"Rechoose Atype: ",
"Can you choose a new player type when you respawn?",
ENTRY_RECHOOSE_ATYPE,
RESPONSE_CYCLE,
0,
0
},*/


{
"Health: ",
"Increases or reduces the number of hit points each player has. Is applied on top of handicaps.",
ENTRY_HEALTH_AMOUNT,
RESPONSE_NUMBER,
0,
0
},

{
"Classes: ",
"Which player classes are available?",
ENTRY_ATYPES_AVAIL,
RESPONSE_CYCLE,
0,
0
},

{
"Score Limit: ",
"What score wins the game?",
ENTRY_SCORE_LIMIT,
RESPONSE_NUMBER,
0,
0
},

{
"Lives: ",
"How many lives does each player have? Set it to zero for unlimited lives.",
ENTRY_LIVES_LIMIT,
RESPONSE_NUMBER,
0,
0
},

{
"Number of Fruit: ",
"In Hunt for Fruit, how many fruit are there?",
ENTRY_FRUIT_NUMBER,
RESPONSE_NUMBER,
0,
0
},
{
"T&H Bases:  ",
"In Take & Hold, how many bases are there?",
ENTRY_TH_BASE_NUMBER,
RESPONSE_NUMBER,
0,
0
},
{
"Penalty:  ",
"How long do you sit out after being killed?",
ENTRY_PENALTY,
RESPONSE_NUMBER,
0,
0
},

{
"Radar: ",
"Is the radar display active?",
ENTRY_SHOW_MAP,
RESPONSE_CYCLE,
0,
0
},

{
"Dynamic Light Sourcing: ",
"Is dynamic light sourcing (which allows objects other than lamps to emit light) activated?",
ENTRY_LIGHTSOURCING,
RESPONSE_CYCLE,
0,
0
},

{
"Physics",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Gravity: ",
"How strong is the force of gravity? 30 is average.",
ENTRY_GRAVITY,
RESPONSE_NUMBER,
0,
0
},

{
"Impact Damage: ",
"How much damage do you take from falling or hitting walls at a high speed?",
ENTRY_IMPACT,
RESPONSE_NUMBER,
0,
0
},

{
"Blast Bullets: ",
"Are bullets affected by shock waves or implosion fields? It's recommended that you turn this off for networked games.",
ENTRY_BLAST_BULLETS,
RESPONSE_CYCLE,
0,
0
},

{
"Soft Dirt: ",
"Soft dirt is removed much more quickly by explosions, but is no easier to tunnel through.",
ENTRY_SOFT_DIRT,
RESPONSE_CYCLE,
0,
0
},


{
"Weapons",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Starting Weapons: ",
"How many weapons do you get to choose at the start of each game?",
ENTRY_NO_WEAPONS,
RESPONSE_CYCLE,
0,
0
},


{
"Reload Time: ",
"How long does it take to reload weapons? 10 is average.",
ENTRY_RELOAD_TIME,
RESPONSE_NUMBER,
0,
0
},

{
"Unlimited Clips: ",
"Do your clips run out?",
ENTRY_UNLIMITED_CLIPS,
RESPONSE_CYCLE,
0,
0
},


/*
{
"Reset Weapons: ",
"Are your weapons reset when you respawn?",
ENTRY_RESET_WEAPONS,
RESPONSE_CYCLE,
0,
0
},
*/
{
"Bullet Speed: ",
"Do bullets travel slowly, quickly or almost instantaneously?",
ENTRY_FAST_BULLETS,
RESPONSE_CYCLE,
0,
0
},

{
"Bullets: ",
"Bullets can explode on impact, or you can set them to be multicoloured paintballs.",
ENTRY_BULLETS_EXPLODE,
RESPONSE_CYCLE,
0,
0
},

/*{
This is redundant with the ability to set starting_weapons to zero
"Weapon Set - None",
"Sets all weapons and equipment to Common or Rare status (none are available at start).",
ENTRY_WEAPON_SET_NONE,
RESPONSE_WEAPON_SET,
0,
0
},*/
{
"Weapon Set - Basic",
"Sets the status of all weapons and equipment according to the Basic set. Only weak weapons are available at the start, and a limited set is available in boxes.",
ENTRY_WEAPON_SET_BASIC,
RESPONSE_WEAPON_SET,
0,
0
},
{
"Weapon Set - Extended",
"Sets the status of all weapons and equipment according to the Extended set. A limited set is available at the start, and all are available in boxes.",
ENTRY_WEAPON_SET_EXTENDED,
RESPONSE_WEAPON_SET,
0,
0
},
{
"Weapon Set - Full",
"Sets the status of all weapons and equipment according to the Full set. Almost all are available at the start, and all are in boxes.",
ENTRY_WEAPON_SET_FULL,
RESPONSE_WEAPON_SET,
0,
0
},

{
"Pickups",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},



{
"Number of Pickups: ",
"How many pickup boxes can appear on the level at once?",
ENTRY_PK_NUMBER,
RESPONSE_NUMBER,
0,
0
},
{
"Speed of Pickups: ",
"How often new pickups appear.",
ENTRY_PK_TIME_BETWEEN,
RESPONSE_CYCLE,
0,
0
},
{
"Weapon Pickups: ",
"How common are boxes of weapons?",
ENTRY_PK_WEAPON,
RESPONSE_CYCLE,
0,
0
},
{
"Equipment Pickups: ",
"How common are boxes of equipment?",
ENTRY_PK_EQUIP,
RESPONSE_CYCLE,
0,
0
},
{
"Health Pickups: ",
"How common are boxes of healing goodness?",
ENTRY_PK_HEALTH,
RESPONSE_CYCLE,
0,
0
},
{
"Name Pickups: ",
"Do you get to see what's inside a box before you pick it up?",
ENTRY_PK_NAMES,
RESPONSE_CYCLE,
0,
0
},

/*{
"Save configuration to file",
"Makes the game save your new options to disk when you leave \
the game.",
ENTRY_SAVE_GAME,
RESPONSE_SAVE_CONFIG,
0,
0
},*/


{
"Select this game and return",
"Go back to the previous menu.",
ENTRY_RETURN,
RESPONSE_FINISH_CUSTOM,
SPECIAL_ESCAPE,
0
},

{
"Weapons",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},


{
"Autocannon",
"",
ENTRY_WEAPON_18, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Blunderbuss",
"",
ENTRY_WEAPON_17, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Bouncy bomb",
"",
ENTRY_WEAPON_38, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"C-Remote Rocket",
"",
ENTRY_WEAPON_24, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Clod of Dirt",
"",
ENTRY_WEAPON_41, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Cube Rifle",
"",
ENTRY_WEAPON_53, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Dirt Bomb",
"",
ENTRY_WEAPON_42, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Disrupter",
"",
ENTRY_WEAPON_32, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Disrupter Wave",
"",
ENTRY_WEAPON_51, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Fireball",
"",
ENTRY_WEAPON_48, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Firebomb",
"",
ENTRY_WEAPON_20, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Flakker",
"",
ENTRY_WEAPON_15, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Flamethrower",
"",
ENTRY_WEAPON_19, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Frag Grenade",
"",
ENTRY_WEAPON_14, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Fumigator",
"",
ENTRY_WEAPON_46, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Funky bomb",
"",
ENTRY_WEAPON_31, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Gas Grenade",
"",
ENTRY_WEAPON_40, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Grenade",
"",
ENTRY_WEAPON_7, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Heavy Machine Gun",
"",
ENTRY_WEAPON_2, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Hunter-Seeker",
"",
ENTRY_WEAPON_30, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Implosion Device",
"",
ENTRY_WEAPON_43, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Incendiaries",
"",
ENTRY_WEAPON_49, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Laser Beam",
"",
ENTRY_WEAPON_26, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Laser Pulse",
"",
ENTRY_WEAPON_27, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Light Machine Gun",
"",
ENTRY_WEAPON_1, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Lightning Gun",
"",
ENTRY_WEAPON_54, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"LR Rocket",
"",
ENTRY_WEAPON_8, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Mortar",
"",
ENTRY_WEAPON_6, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Napalm Gun",
"",
ENTRY_WEAPON_47, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Nibbler",
"",
ENTRY_WEAPON_22, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Nuclear Missile",
"",
ENTRY_WEAPON_13, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Plasma Cannon",
"",
ENTRY_WEAPON_11, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Plasma Rifle",
"",
ENTRY_WEAPON_10, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Plinker",
"",
ENTRY_WEAPON_21, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Prongthrower",
"",
ENTRY_WEAPON_39, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Rectifier",
"",
ENTRY_WEAPON_12, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Remote Rocket",
"",
ENTRY_WEAPON_23, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Rocket Launcher",
"",
ENTRY_WEAPON_5, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Scattergun",
"",
ENTRY_WEAPON_16, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Seeker Rocket",
"",
ENTRY_WEAPON_44, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Shotgun",
"",
ENTRY_WEAPON_3, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Shredder",
"",
ENTRY_WEAPON_50, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Slug Gun",
"",
ENTRY_WEAPON_4, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Smiter",
"",
ENTRY_WEAPON_52, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Squirm Rockets",
"",
ENTRY_WEAPON_45, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Swarm Rockets",
"",
ENTRY_WEAPON_9, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Tracker Rocket",
"",
ENTRY_WEAPON_29, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Toxin Grenade",
"",
ENTRY_WEAPON_33, RESPONSE_CYCLE_WEAPON, 0, 0},


{
"Weak Weapons",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Bomb",
"",
ENTRY_WEAPON_34, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Grapeshot",
"",
ENTRY_WEAPON_35, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Needler",
"",
ENTRY_WEAPON_37, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"RPG",
"",
ENTRY_WEAPON_36, RESPONSE_CYCLE_WEAPON, 0, 0},

{
"Semi-Automatic",
"",
ENTRY_WEAPON_25, RESPONSE_CYCLE_WEAPON, 0, 0},



{
"Equipment",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Jetpack",
"",
ENTRY_EQUIP_4, RESPONSE_CYCLE_EQUIP, 0, 0},

{
"Rocketpack",
"",
ENTRY_EQUIP_5, RESPONSE_CYCLE_EQUIP, 0, 0},

{
"Soft Shield",
"",
ENTRY_EQUIP_2, RESPONSE_CYCLE_EQUIP, 0, 0},

{
"Bubble Shield",
"",
ENTRY_EQUIP_1, RESPONSE_CYCLE_EQUIP, 0, 0},

{
"Cloak",
"",
ENTRY_EQUIP_3, RESPONSE_CYCLE_EQUIP, 0, 0},

{
"Shadow Field",
"",
ENTRY_EQUIP_8, RESPONSE_CYCLE_EQUIP, 0, 0},

{
"Regenerator",
"",
ENTRY_EQUIP_7, RESPONSE_CYCLE_EQUIP, 0, 0},

{
"Spotlight",
"",
ENTRY_EQUIP_6, RESPONSE_CYCLE_EQUIP, 0, 0},


{
"Select this game and return",
"Go back to the previous menu.",
ENTRY_RETURN,
RESPONSE_FINISH_CUSTOM,
SPECIAL_ESCAPE,
0
},



{
"",
"",
ENTRY_NULL,
0,
0,
0
}

};

struct menu_format atype_menu [5] =
{

{
"Light",
"",
ENTRY_ATYPE_LIGHT, RESPONSE_CHOOSE_ATYPE, 0, 0},

{
"Medium",
"",
ENTRY_ATYPE_MEDIUM, RESPONSE_CHOOSE_ATYPE, SPECIAL_ESCAPE, 0},

{
"Heavy",
"",
ENTRY_ATYPE_HEAVY, RESPONSE_CHOOSE_ATYPE, 0, 0},
{
"",
"",
ENTRY_NULL,
0,
0,
0
}



};

struct menu_format team_menu [8] =
{

/*{
"No Team",
"",
ENTRY_TEAM_NONE, RESPONSE_CHOOSE_TEAM, SPECIAL_ESCAPE, 0},*/

{
"Red Team",
"",
ENTRY_TEAM_RED, RESPONSE_CHOOSE_TEAM, SPECIAL_ESCAPE, 0},

{
"Blue Team",
"",
ENTRY_TEAM_BLUE, RESPONSE_CHOOSE_TEAM, 0, 0},

{
"Green Team",
"",
ENTRY_TEAM_GREEN, RESPONSE_CHOOSE_TEAM, 0, 0},

{
"Gold Team",
"",
ENTRY_TEAM_GOLD, RESPONSE_CHOOSE_TEAM, 0, 0},

{
"Grey Team",
"",
ENTRY_TEAM_GREY, RESPONSE_CHOOSE_TEAM, 0, 0},

{
"Brown Team",
"",
ENTRY_TEAM_BROWN, RESPONSE_CHOOSE_TEAM, 0, 0},

{
"",
"",
ENTRY_NULL,
0,
0,
0
}

};

struct menu_format weapon_menu [70] =
{

{
"Standard Weapons",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"Autocannon",
"",
ENTRY_WEAPON_18, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Blunderbuss",
"",
ENTRY_WEAPON_17, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Bouncy Bomb",
"",
ENTRY_WEAPON_38, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"C-Remote Rocket",
"",
ENTRY_WEAPON_24, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Clod of Dirt",
"",
ENTRY_WEAPON_41, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Cube Rifle",
"",
ENTRY_WEAPON_53, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Dirt Bomb",
"",
ENTRY_WEAPON_42, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Disrupter",
"",
ENTRY_WEAPON_32, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Disrupter Wave",
"",
ENTRY_WEAPON_51, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Fireball",
"",
ENTRY_WEAPON_48, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Firebomb",
"",
ENTRY_WEAPON_20, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Flakker",
"",
ENTRY_WEAPON_15, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Flamethrower",
"",
ENTRY_WEAPON_19, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Frag Grenade",
"",
ENTRY_WEAPON_14, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Fumigator",
"",
ENTRY_WEAPON_46, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Funky Bomb",
"",
ENTRY_WEAPON_31, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Gas Grenade",
"",
ENTRY_WEAPON_40, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Grenade",
"",
ENTRY_WEAPON_7, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Heavy Machine Gun",
"",
ENTRY_WEAPON_2, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Hunter-Seeker",
"",
ENTRY_WEAPON_30, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Implosion Device",
"",
ENTRY_WEAPON_43, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Incendiaries",
"",
ENTRY_WEAPON_49, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Laser Beam",
"",
ENTRY_WEAPON_26, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Laser Pulse",
"",
ENTRY_WEAPON_27, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Light Machine Gun",
"",
ENTRY_WEAPON_1, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Lightning Gun",
"",
ENTRY_WEAPON_54, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"LR Rocket",
"",
ENTRY_WEAPON_8, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Mortar",
"",
ENTRY_WEAPON_6, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Napalm Gun",
"",
ENTRY_WEAPON_47, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Nibbler",
"",
ENTRY_WEAPON_22, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Nuclear Missile",
"",
ENTRY_WEAPON_13, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Plasma Cannon",
"",
ENTRY_WEAPON_11, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Plasma Rifle",
"",
ENTRY_WEAPON_10, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Plinker",
"",
ENTRY_WEAPON_21, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Prongthrower",
"",
ENTRY_WEAPON_39, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Rectifier",
"",
ENTRY_WEAPON_12, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Remote Rocket",
"",
ENTRY_WEAPON_23, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Rocket Launcher",
"",
ENTRY_WEAPON_5, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Scattergun",
"",
ENTRY_WEAPON_16, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Seeker Rocket",
"",
ENTRY_WEAPON_44, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Shotgun",
"",
ENTRY_WEAPON_3, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Shredder",
"",
ENTRY_WEAPON_50, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Slug Gun",
"",
ENTRY_WEAPON_4, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Smiter",
"",
ENTRY_WEAPON_52, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Squirm Rockets",
"",
ENTRY_WEAPON_45, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Swarm Rockets",
"",
ENTRY_WEAPON_9, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Toxin Grenade",
"",
ENTRY_WEAPON_33, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Tracker Rocket",
"",
ENTRY_WEAPON_29, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Weak Weapons",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"Bomb",
"",
ENTRY_WEAPON_34, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Grapeshot",
"",
ENTRY_WEAPON_35, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Needler",
"",
ENTRY_WEAPON_37, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"RPG",
"",
ENTRY_WEAPON_36, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Semi-Automatic",
"",
ENTRY_WEAPON_25, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Equipment",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},

{
"Jetpack",
"",
ENTRY_EQUIP_4, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Rocketpack",
"",
ENTRY_EQUIP_5, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Soft Shield",
"",
ENTRY_EQUIP_2, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Bubble Shield",
"",
ENTRY_EQUIP_1, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Cloak",
"",
ENTRY_EQUIP_3, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Shadow Field",
"",
ENTRY_EQUIP_8, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Regenerator",
"",
ENTRY_EQUIP_7, RESPONSE_CHOOSE_WEAPON, 0, 0},

{
"Spotlight",
"",
ENTRY_EQUIP_6, RESPONSE_CHOOSE_WEAPON, 0, 0},




{
"Done",
"Finished choosing.",
ENTRY_RETURN,
RESPONSE_FINISH_CHOICE,
SPECIAL_ESCAPE,
0
}


};



struct menu_format serial_menu [12] =
{

{
"Connection Type: ",
"Choose to play a game on this computer only, to host a game as server, or for this computer to be a client and connect to another server.",
ENTRY_SERIAL_CONNECTION,
RESPONSE_CYCLE,
0,
0
},

{
"Network Type: ",
"What type of network you're using. Serial Port mode is supported, and there is an experimental IPX mode which may or may not work.",
ENTRY_CONNECTION_TYPE,
RESPONSE_CYCLE,
0,
0
},


{
"COM Port: ",
"Serial port connections only: choose which COM port to use.",
ENTRY_COM_PORT,
RESPONSE_CYCLE,
0,
0
},

{
"Add: ",
"IPX connections only: type in the address of the server (which should appear when the server is waiting for connections). Leave this blank for serial port connections.",
ENTRY_SERVER_ADDRESS,
RESPONSE_ENTER_SERVER_ADDRESS,
0,
0
},

/*{
"Connection - ",
"Select the type of connection being used.",
ENTRY_CHANNEL,
RESPONSE_CYCLE,
0,
0
},

{
"",
"Initialise a connection.",
ENTRY_INIT_CONNECT,
RESPONSE_INIT_CONNECT,
0,
0
},*/


{
"", // Is called "connect if a client
"Connects or waits for connections.",
ENTRY_CONNECT,
RESPONSE_CONNECT,
0,
0
},

{
"Disable Connection",
"Shuts down your network connection. You can start again now.",
ENTRY_SHUTDOWN_CONNECT,
RESPONSE_SHUTDOWN_CONNECT,
0,
0
},

/*{
"No Connection",
"",
ENTRY_CONNECTIONS, RESPONSE_NONE, 0, 0 },

{
"Server Options",
"",
ENTRY_TITLE,
RESPONSE_NONE,
0,
0
},


This option is broken. Must be server.
{
"Damage determined by ",
"Who determines when damage has been done?",
ENTRY_WHO_DAMAGE,
RESPONSE_CYCLE,
0,
0
},
*/

{
"Local Players",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },


{
"Players on this computer",
"Select one or two players for this computer (each computer involved can have one or two players on it).",
ENTRY_SELECT_PLAYERS,
RESPONSE_CYCLE, // really a number, but cycle is better
0,
0
},

{
"Player 1 Team - ",
"Shortcut to choose your player's team.",
ENTRY_TEAM1,
RESPONSE_CYCLE,
0,
0
},

{
"Player 2 Team - ",
"Shortcut to choose your player's team.",
ENTRY_TEAM2,
RESPONSE_CYCLE,
0,
0
},
{
"Go Back",
"Go back to the previous menu.",
ENTRY_RETURN,
RESPONSE_RETURN_MENU,
SPECIAL_ESCAPE,
0
},



{
"",
"",
ENTRY_NULL,
0,
0,
0
}



};


struct menu_format level_menu [12] =
{

{
"Level Style",
"What type of level is it?",
ENTRY_LEVEL_STYLE, RESPONSE_CYCLE, 0, 0},


{
"Colours",
"Selects the colour scheme.",
ENTRY_LEVEL_COLOURS, RESPONSE_CYCLE, 0, 0},

{
"Obstacles",
"Selects the number of obstacles present.",
ENTRY_LEVEL_SOLIDS, RESPONSE_CYCLE, 0, 0},

{
"Dirt",
"Is the level full of dirt?",
ENTRY_LEVEL_DIRT, RESPONSE_CYCLE, 0, 0},

{
"Earthquakes",
"Does more dirt get added, and how often?",
ENTRY_LEVEL_REDIRT, RESPONSE_CYCLE, 0, 0},

{
"Ambient Light",
"Is the whole level lit up?",
ENTRY_AMBIENT_LIGHT, RESPONSE_CYCLE, 0, 0},

{
"Lamps",
"Are there lamps lighting up the level? And are they arranged regularly or just scattered around?",
ENTRY_LAMPS, RESPONSE_CYCLE, 0, 0},

{
"Level Width - ",
"How wide is the level?",
ENTRY_LEVEL_WIDTH,
RESPONSE_NUMBER,
0,
0
},

{
"Level Height - ",
"How high is the level?",
ENTRY_LEVEL_HEIGHT,
RESPONSE_NUMBER,
0,
0
},
/*
{
"Load Level - ",
"Load a level from disk.",
ENTRY_LOAD_LEVEL,
RESPONSE_ENTER_STRING,
0,
0
},
*/

{
"Go Back",
"Go back to the previous menu.",
ENTRY_RETURN,
RESPONSE_RETURN_MENU,
SPECIAL_ESCAPE,
0
},



{
"",
"",
ENTRY_NULL,
0,
0,
0
}

};





struct menu_format ai_menu [67] =
{

{
"AI-1",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"AI-1 Name - ",
"",
ENTRY_NAME_AI1, RESPONSE_ENTER_STRING, 0, 0},

{
"AI-1 is",
"",
ENTRY_ACTIVE_AI1, RESPONSE_CYCLE, 0, 0},

{
"AI-1 Skill",
"",
ENTRY_SKILL_AI1, RESPONSE_CYCLE, 0, 0},

{
"AI-1 Team",
"",
ENTRY_TEAM_AI1, RESPONSE_CYCLE, 0, 0},

{
"AI-1 Type",
"",
ENTRY_ATYPE_AI1, RESPONSE_CYCLE, 0, 0},

{
"AI-1 Handicap - ",
"",
ENTRY_HANDICAP_AI1, RESPONSE_NUMBER, 0, 0},

/*{
"AI-1 Class",
"",
ENTRY_SOLDIER_AI1, RESPONSE_CYCLE, 0, 0},*/

{
"AI-2",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"AI-2 Name - ",
"",
ENTRY_NAME_AI2, RESPONSE_ENTER_STRING, 0, 0},

{
"AI-2 is",
"",
ENTRY_ACTIVE_AI2, RESPONSE_CYCLE, 0, 0},

{
"AI-2 Skill",
"",
ENTRY_SKILL_AI2, RESPONSE_CYCLE, 0, 0},

{
"AI-2 Team",
"",
ENTRY_TEAM_AI2, RESPONSE_CYCLE, 0, 0},

{
"AI-2 Type",
"",
ENTRY_ATYPE_AI2, RESPONSE_CYCLE, 0, 0},

{
"AI-2 Handicap - ",
"",
ENTRY_HANDICAP_AI2, RESPONSE_NUMBER, 0, 0},

/*{
"AI-2 Class",
"",
ENTRY_SOLDIER_AI2, RESPONSE_CYCLE, 0, 0},*/

{
"AI-3",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"AI-3 Name - ",
"",
ENTRY_NAME_AI3, RESPONSE_ENTER_STRING, 0, 0},

{
"AI-3 is",
"",
ENTRY_ACTIVE_AI3, RESPONSE_CYCLE, 0, 0},

{
"AI-3 Skill",
"",
ENTRY_SKILL_AI3, RESPONSE_CYCLE, 0, 0},

{
"AI-3 Team",
"",
ENTRY_TEAM_AI3, RESPONSE_CYCLE, 0, 0},

{
"AI-3 Type",
"",
ENTRY_ATYPE_AI3, RESPONSE_CYCLE, 0, 0},

{
"AI-3 Handicap - ",
"",
ENTRY_HANDICAP_AI3, RESPONSE_NUMBER, 0, 0},

/*{
"AI-3 Class",
"",
ENTRY_SOLDIER_AI3, RESPONSE_CYCLE, 0, 0},*/

{
"AI-4",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"AI-4 Name - ",
"",
ENTRY_NAME_AI4, RESPONSE_ENTER_STRING, 0, 0},

{
"AI-4 is",
"",
ENTRY_ACTIVE_AI4, RESPONSE_CYCLE, 0, 0},

{
"AI-4 Skill",
"",
ENTRY_SKILL_AI4, RESPONSE_CYCLE, 0, 0},

{
"AI-4 Team",
"",
ENTRY_TEAM_AI4, RESPONSE_CYCLE, 0, 0},

{
"AI-4 Type",
"",
ENTRY_ATYPE_AI4, RESPONSE_CYCLE, 0, 0},

{
"AI-4 Handicap - ",
"",
ENTRY_HANDICAP_AI4, RESPONSE_NUMBER, 0, 0},

/*{
"AI-4 Class",
"",
ENTRY_SOLDIER_AI4, RESPONSE_CYCLE, 0, 0},*/

{
"AI-5",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"AI-5 Name - ",
"",
ENTRY_NAME_AI5, RESPONSE_ENTER_STRING, 0, 0},

{
"AI-5 is",
"",
ENTRY_ACTIVE_AI5, RESPONSE_CYCLE, 0, 0},

{
"AI-5 Skill",
"",
ENTRY_SKILL_AI5, RESPONSE_CYCLE, 0, 0},

{
"AI-5 Team",
"",
ENTRY_TEAM_AI5, RESPONSE_CYCLE, 0, 0},

{
"AI-5 Type",
"",
ENTRY_ATYPE_AI5, RESPONSE_CYCLE, 0, 0},

{
"AI-5 Handicap - ",
"",
ENTRY_HANDICAP_AI5, RESPONSE_NUMBER, 0, 0},

/*{
"AI-5 Class",
"",
ENTRY_SOLDIER_AI5, RESPONSE_CYCLE, 0, 0},*/

{
"AI-6",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"AI-6 Name - ",
"",
ENTRY_NAME_AI6, RESPONSE_ENTER_STRING, 0, 0},

{
"AI-6 is",
"",
ENTRY_ACTIVE_AI6, RESPONSE_CYCLE, 0, 0},

{
"AI-6 Skill",
"",
ENTRY_SKILL_AI6, RESPONSE_CYCLE, 0, 0},

{
"AI-6 Team",
"",
ENTRY_TEAM_AI6, RESPONSE_CYCLE, 0, 0},

{
"AI-6 Type",
"",
ENTRY_ATYPE_AI6, RESPONSE_CYCLE, 0, 0},

{
"AI-6 Handicap - ",
"",
ENTRY_HANDICAP_AI6, RESPONSE_NUMBER, 0, 0},

/*{
"AI-6 Class",
"",
ENTRY_SOLDIER_AI6, RESPONSE_CYCLE, 0, 0},*/

{
"AI-7",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"AI-7 Name - ",
"",
ENTRY_NAME_AI7, RESPONSE_ENTER_STRING, 0, 0},

{
"AI-7 is",
"",
ENTRY_ACTIVE_AI7, RESPONSE_CYCLE, 0, 0},

{
"AI-7 Skill",
"",
ENTRY_SKILL_AI7, RESPONSE_CYCLE, 0, 0},

{
"AI-7 Team",
"",
ENTRY_TEAM_AI7, RESPONSE_CYCLE, 0, 0},

{
"AI-7 Type",
"",
ENTRY_ATYPE_AI7, RESPONSE_CYCLE, 0, 0},

{
"AI-7 Handicap - ",
"",
ENTRY_HANDICAP_AI7, RESPONSE_NUMBER, 0, 0},

/*{
"AI-7 Class",
"",
ENTRY_SOLDIER_AI7, RESPONSE_CYCLE, 0, 0},*/

{
"General",
"",
ENTRY_TITLE, RESPONSE_NONE, 0, 0 },

{
"Toggle All",
"Turns all AIs either on or off.",
ENTRY_SELECT_ALL_AI, RESPONSE_SELECT_ALL_AI, 0, 0 },

{
"AI Players Impact Damage",
"Determines whether AIs suffer damage from falling (if this option is set in the game config menu).",
ENTRY_AI_IMPACT, RESPONSE_CYCLE, 0, 0},

{
"AI Players Unlimited Clips",
"AIs can be given unlimited clips of ammunition.",
ENTRY_AI_CLIPS, RESPONSE_CYCLE, 0, 0},

{
"AI Players Starting Weapons",
"If this is on, AIs get to choose a full set of any weapons each time they enter the arena.",
ENTRY_AI_WEAPONS, RESPONSE_CYCLE, 0, 0},

/*{
"Save AI Configuration To File",
"Makes the game save your new options to disk when you leave \
the game.",
ENTRY_SAVE_AI_CONFIG,
RESPONSE_SAVE_CONFIG,
0,
0
},*/

{
"Go Back",
"Go back to the previous menu.",
ENTRY_RETURN,
RESPONSE_RETURN_MENU,
SPECIAL_ESCAPE,
0
}





};



