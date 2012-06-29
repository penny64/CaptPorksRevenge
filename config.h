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

File: config.h
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - various configuration #defines
 - heaps of enums
 - various structs
 
*/


// dimensions of playing area, in GRID_WIDTH_ units
//  -- probably not used any more
#define GRID_X 100
#define GRID_Y 100

// also probably not used any more
#define DIRT_X 1000
#define DIRT_Y 800

// the colour in the shadow bitmap for undiggable (permanent) rock
#define PIX_PERM 1

// dimensions of a grid unit. - not used
#define GRID_WIDTH_X 10
#define GRID_WIDTH_Y 10

#define BRICK_X 15
#define BRICK_Y 10

// max number of actors in the game at once. Shouldn't be put above
//  32 as we run out of colours above 224.
#define NO_ACTORS 16
// max users
#define NO_USERS 16

// max number of different commands
#define NO_CMDS 27

// how many units of actor and bullet movement in one pixel?
//  probably shouldn't be changed
#define GRAIN 100

// max number of bullets in game
//  should be changeable
#define MAX_BULLETS 1000

// max number of clouds in game
//  should be changeable
#define MAX_CLOUDS 200

// how thin can actors be? Affects bullet collision detection - a bullet
//  checks for an actor collision every time it travels this far.
//  must be a multiple of GRAIN
//  Can be changed
#define TARGET_THICKNESS 500

// the max number of bots in a game. Can be changed, but lots of
//  other stuff would also have to be changed.
#define MAX_AI 8

//#define NO_WEAPONS 50
#define NO_EQUIP 8

#define MAX_PICKUPS 35

#define NO_GAMES 8
// number of game configs available

#define LAST_GAME NO_GAMES - 1

#define NO_TEAMS 8

#define TEAM_RED_COLOUR COLOUR_RED4
#define TEAM_BLUE_COLOUR COLOUR_BLUE4
#define TEAM_GREEN_COLOUR COLOUR_GREEN4
#define TEAM_GOLD_COLOUR COLOUR_YELLOW4
#define TEAM_GREY_COLOUR COLOUR_GREY4
#define TEAM_BROWN_COLOUR COLOUR_BROWN4
#define TEAM_NONE_COLOUR COLOUR_GREY6
// array team_colour_list, which contains these, is in globvar.h

#define ASYNC_BUFFER 1000

#define NO_ACTOR_BITMAPS 30

#define NO_SOLDIER_FILES 9

#define NO_LIGHT_LEVELS 5

// these have to be appropriate colours as they are used if transparency
//  is off.
#define TRANS_BLUE 220
#define TRANS_YELLOW 208
#define TRANS_ORANGE 211
#define TRANS_RED 212
#define TRANS_GREEN 216
#define TRANS_BLACK 190
#define TRANS_WHITE 195

#define LIGHT_1 100
#define LIGHT_2 101
#define LIGHT_3 102
#define LIGHT_4 103

#define FORCE_LIGHT_1 104
#define FORCE_LIGHT_2 105
#define FORCE_LIGHT_3 106
#define FORCE_LIGHT_4 107
#define FORCE_LIGHT_5 108

#define DARK_1 120
#define DARK_2 121
#define DARK_3 122
#define DARK_4 123

#define FADE_OUT 124

#define DIRT_1 125
#define DIRT_2 126

//#define WRITING 130

extern int trans_colours_array [7];
extern int actor_colours_array [7];

enum
{
COLOUR_BLACK = 224,
COLOUR_GREY1,
COLOUR_GREY2,
COLOUR_GREY3,
COLOUR_GREY4,
COLOUR_GREY5,
COLOUR_GREY6,
COLOUR_GREY7,
COLOUR_GREY8,
COLOUR_BROWN1,
COLOUR_BROWN2,
COLOUR_BROWN3,
COLOUR_BROWN4,
COLOUR_YELLOW1,
COLOUR_YELLOW2,
COLOUR_YELLOW3,
COLOUR_YELLOW4,
COLOUR_RED1,
COLOUR_RED2,
COLOUR_RED3,
COLOUR_RED4,
COLOUR_GREEN1,
COLOUR_GREEN2,
COLOUR_GREEN3,
COLOUR_GREEN4,
COLOUR_BLUE1,
COLOUR_BLUE2,
COLOUR_BLUE3,
COLOUR_BLUE4,
COLOUR_PURPLE,
COLOUR_WRITING,
COLOUR_31

};

enum
{
SOLDIER_SOLDIER,
SOLDIER_SCOUT,
SOLDIER_CYBORG,
SOLDIER_CRUSHER,
SOLDIER_WALKER,
SOLDIER_BUG,
SOLDIER_SKELETON,
SOLDIER_DEMON,
SOLDIER_SNAIL,
SOLDIER_NONE
};

enum
{
WPNCLASS_NONE,
WPNCLASS_EQUIP,
WPNCLASS_WEAK,
WPNCLASS_WEAPON
};

enum
{
//GAME_NONE,
GAME_KILL,
GAME_LAST_ONE,
GAME_CAP_FLAG,
GAME_FRUIT_HUNT,
GAME_PORKBALL,
GAME_TAKEHOLD,
GAME_GRAIL
};

enum
{
QSTART_NONE,
QSTART_MELEE,
QSTART_MELEE_TEAM,
QSTART_DUEL,
QSTART_DUEL_TEAM,
QSTART_CAPFLAG,
QSTART_FRUIT,
QSTART_TAKEHOLD,
QSTART_LAST_ONE,
QSTART_GRAIL,
QSTART_TOURNAMENT
};


enum
{
SCORE_KILLS_ONLY
};

enum
{
LIVES_LIMITED,
LIVES_UNLIMITED
};

enum
{
CLOUDS_FILL,
CLOUDS_STIPPLE,
CLOUDS_TRANS
};

enum
{
ACLASS_NONE,
ACLASS_GHOST,
ACLASS_PLAYER,
ACLASS_PORKBALL
};

enum
{
USTAT_NONE,
USTAT_LOCAL,
USTAT_CLIENT,
USTAT_SERVER,
USTAT_AI
};
// Note: remote ai players are considered the same as remote humans.
// USTAT_AI is only for ais who are run locally.

enum
{
ATYPE_NONE,
ATYPE_LIGHT,
ATYPE_MEDIUM,
ATYPE_HEAVY,
ATYPE_LIGHT_J,
ATYPE_MEDIUM_J,
ATYPE_HEAVY_J,
ATYPE_LIGHT_R,
ATYPE_MEDIUM_R,
ATYPE_HEAVY_R
};

enum
{
PROP_NONE,
PROP_GRAPPLE,
PROP_JET,
PROP_ROCKETS

};

enum
{
SCORE_KILL,
SCORE_SUICIDE,
SCORE_SFRUIT,
SCORE_MFRUIT,
SCORE_LFRUIT,
SCORE_GRAIL,
SCORE_FLAG,
SCORE_GOAL,
SCORE_TH
};

enum
{
SMESSAGE_FRUIT,
SMESSAGE_TH,
SMESSAGE_GOAL,
SMESSAGE_GRAIL,
SMESSAGE_FLAG,
SMESSAGE_CAPTURE,
SMESSAGE_ELIMINATED,
SMESSAGE_TEAM_ELIMINATED,
SMESSAGE_FLAG_RETURN,
SMESSAGE_HAS_FLAG
};


enum
{
SERIAL_LOCAL_ONLY,
SERIAL_SERVER,
SERIAL_CLIENT
};

enum
{
CONNECTION_NONE,
CONNECTION_SERIAL,
CONNECTION_IPX
};

enum
{
DAM_SERVER,
DAM_SUFFERER,
DAM_INFLICTOR
};

enum
{
/*GRID_REDBRICK_BACK,
GRID_ROCKBACK,
GRID_DIRTBACK,
GRID_REDBRICK = BRICK_SOLID,
GRID_ROCK1,
GRID_ROCK2,
GRID_ROCK3,
GRID_ROCK4,
GRID_DIRT,
GRID_DIRTG*/
GRID_SSTONE_BACK,
GRID_REDBRICK1,
GRID_REDBRICK2,
GRID_SSTONE1,
GRID_SSTONE2
};

#define NO_BRICKS 2

enum
{
BRICKBACK_RED_BRICK,
BRICKBACK_YELLOW_STONE,
BRICKBACK_BLACK_TILE,
BRICKBACK_BLUE_BRICK,
BRICKBACK_GREEN_CIRCLE,
BRICKBACK_DIRT,
BRICK_RED_BRICK,
BRICK_YELLOW_STONE,
BRICK_YELLOW_CROSS,
BRICK_WHITE_TILE,
BRICK_BLUE_BRICK,
BRICK_GREEN_CIRCLE,
BRICK_DIRT,
BRICK_STONE
};

#define BRICK_SOLID BRICK_RED_BRICK
// number of the first brick to be solid (ie not a background)


enum
{
LEVEL_CLASSIC,
LEVEL_ANY,
LEVEL_PLATFORM,
LEVEL_CITY

};

enum
{
LEVELTYPE_DIRT,
LEVELTYPE_BRICKS
};

enum
{
COLOURS_CRAZY,
COLOURS_CLASSIC,
COLOURS_EARTH,
COLOURS_HELL
};

// leg actions
enum
{
LA_STAND,
LA_WALK_LEFT,
LA_WALK_RIGHT,
LA_DRIFT_LEFT,
LA_DRIFT_RIGHT,
LA_START_JUMP,
LA_SLAM_JUMP,
LA_CLIMB
};

enum
{
BULLETS_NO_EXPLODE,
BULLETS_EXPLODE,
BULLETS_PAINTBALL
};

enum
{
BULLET_NONE,
BULLET_GRAPPLE,
BULLET_GREN_LAUNCHER,
BULLET_BULLET,
BULLET_FAT_BULLET,
BULLET_ROCKET,
BULLET_GRENADE,
BULLET_LR_ROCKET,
BULLET_SWARM,
BULLET_PLAS_R,
BULLET_PLAS_C,
BULLET_RECTIFIER,
BULLET_NUKE_M,
BULLET_NUKE_EXP,
BULLET_FLAK,
BULLET_FRAG,
BULLET_AUTOCANNON,
BULLET_FLAME,
BULLET_NAPALM,
BULLET_FIREBOMB,
BULLET_PLINKER,
BULLET_REMOTE_ROCKET,
BULLET_REMOTE_ROCKET_C,
BULLET_SHRAPNEL,
BULLET_GREN_SHRAPNEL,
BULLET_LASER_BEAM,
BULLET_LASER_PULSE,
BULLET_LASER_TRACER,
BULLET_TRACKER,
BULLET_HUNTER_BOMB,
BULLET_HUNTER,
BULLET_FUNKY_BOMB,
BULLET_FUNKY_BOMBLET,
BULLET_DISRUPTER,
BULLET_DISRUPTER_BANG,
BULLET_BLOOD,
BULLET_FLESH,
BULLET_TOX_BOMB,
BULLET_TOXIN,
BULLET_BOMB,
BULLET_RPG,
BULLET_GRAPESHOT,
BULLET_BOUNCY,
BULLET_PRONG,
BULLET_GAS_GREN,
BULLET_GAS,
BULLET_DIRTCLOD,
BULLET_DIRTBOMB,
BULLET_IMPLODER,
BULLET_IMPLOSION,
BULLET_NUKE_PLAS,
BULLET_SEEKER,
BULLET_SPARK,
BULLET_SPECK, // of dirt
BULLET_SQUIRM,
BULLET_INCENDIARY,
BULLET_FIREBALL,
BULLET_SHREDDER,
BULLET_DISRUPTER_WAVE,
BULLET_SMITER,
BULLET_CUBE,
BULLET_SMITELET,
BULLET_SPLINTER,
BULLET_ELECTRO
};



enum
{
WPN_NULL,
WPN_NONE,
WPN_SHIELD,
WPN_ARMOUR,
WPN_CLOAK,
WPN_JETPACK,
WPN_ROCKETPACK,
WPN_SPOTLIGHT,
WPN_REGENERATOR,
WPN_SHADOW,
WPN_LMG,
WPN_HMG,
WPN_SGUN,
WPN_SLUG,
WPN_R_L,
WPN_GR_L,
WPN_GREN,
WPN_LR_R,
WPN_SWARM,
WPN_PLAS_R,
WPN_PLAS_C,
WPN_RECTIFIER,
WPN_NUKE_M,
WPN_FRAG,
WPN_FLAKKER,
WPN_SCATTER,
WPN_BLUNDER,
WPN_AUTOCANNON,
WPN_FTHROWER,
WPN_FIREBOMB,
WPN_PLINKER,
WPN_NIBBLER,
WPN_REMOTE_ROCKET,
WPN_REMOTE_ROCKET_C,
WPN_SEMI_AUTO,
WPN_LASER_BEAM,
WPN_LASER_PULSE,
WPN_LASER_TRACER,
WPN_TRACKER,
WPN_HUNTER,
WPN_FUNKY_BOMB,
WPN_DISRUPTER,
WPN_TOXIN,
WPN_BOMB,
WPN_GRAPESHOT,
WPN_RPG,
WPN_NEEDLER,
WPN_BOUNCY,
WPN_PRONGTHROWER,
WPN_GAS_GREN,
WPN_CLOD_OF_DIRT,
WPN_DIRTBOMB,
WPN_IMPLODER,
WPN_SEEKER,
WPN_SQUIRM,
WPN_FUMIGATOR,
WPN_NAPALM,
WPN_FIREBALL,
WPN_INCENDIARIES,
WPN_SHREDDER,
WPN_DISRUPTER_WAVE,
WPN_SMITER,
WPN_CUBE,
WPN_ELECTRO,
WPN_END

};

#define NO_WEAPONS WPN_END

enum
{
EQUIP_SHIELD,
EQUIP_ARMOUR,
EQUIP_CLOAK,
EQUIP_JETPACK,
EQUIP_ROCKETPACK,
EQUIP_SPOTLIGHT,
EQUIP_REGENERATOR,
EQUIP_SHADOW
};

enum
{
CLOUD_NONE,
CLOUD_GREY_SMOKE,
CLOUD_RED_EXPLOSION,
CLOUD_ORANGE_EXPLOSION,
CLOUD_YELLOW_EXPLOSION,
CLOUD_NUMBER,
CLOUD_DEBUG,
CLOUD_BLUE_EXPLOSION,
CLOUD_LBLUE_EXPLOSION,
CLOUD_GREEN_EXPLOSION,
CLOUD_GREEN_ELLIPSE,
CLOUD_LGREEN_EXPLOSION,
CLOUD_LGREEN_ELLIPSE,
CLOUD_YELLOW_FLASH,
CLOUD_YELLOW2_FLASH,
CLOUD_RED_FLASH,
CLOUD_BLUE_FLASH,
CLOUD_GREEN_FLASH,
CLOUD_BLACK_SMOKE,
CLOUD_RED_BLOOD,
CLOUD_COL_EXPLOSION, // any trans_colour
CLOUD_COL_FLASH, // any trans_colour (doesn't rise or slow)
CLOUD_COL_CIRCLE, // any colour (doesn't rise or slow)
CLOUD_COL_BURST, // any colour
CLOUD_COL_SMOKE,
CLOUD_SHOCKWAVE,
CLOUD_1_POINT,
CLOUD_3_POINTS,
CLOUD_5_POINTS,
CLOUD_LIGHT,
CLOUD_RISING_LIGHT,
CLOUD_BIGLIGHT, // like light, but bigger for a given size value
CLOUD_RISING_BIGLIGHT,
CLOUD_CUBE,
CLOUD_BLOOD,
CLOUD_ELECTRO,
CLOUD_SPAWN
};


enum
{
REMOTE_NONE,
REMOTE_ROCKET,
REMOTE_ROCKET_C
};

enum
{
LASER_NONE,
LASER_BEAM,
LASER_PULSE,
LASER_SIGHT,
LASER_TRACER
};

enum
{
TEAM_NONE,
TEAM_RED,
TEAM_BLUE,
TEAM_GREEN,
TEAM_GOLD,
TEAM_GREY,
TEAM_BROWN
};

enum
{
PICKUP_NONE,
PICKUP_WEAPON,
PICKUP_POWER,
PICKUP_EQUIP,
PICKUP_FRUIT,
PICKUP_GRAIL,
PICKUP_FLAG,
PICKUP_BASE,
PICKUP_TH_BASE
};

enum
{
POSITIONAL_SOUND_OFF,
POSITIONAL_SOUND_ON,
POSITIONAL_SOUND_REVERSED
};

struct act_struct
{
 int player;
 int user;
 int team;

 int aclass;
 int atype;
 unsigned int x, y;
 int x_speed, y_speed;
 char facing;
 char walking;
 char climbing;
 int leg_action;
 char width;
 char height; // actual h and width are twice these, plus one
 char sprite_width;
 char sprite_height;
 unsigned char circle_radius;
 float angle; // in radians
 unsigned char base_angle; // in 256ths of a semicircle
 
 int recycle;
 int inertia; // mass of actor
 int grapple_inertia; // mass of actor for purposes of grapple, taking
   // grapple cord strength into account
 int walk_speed;
 int walk_accel; // currently must be same as walk_speed
// int drift_speed;
// int drift_accel;
 int leg_anim_speed;
 int jump_accel;
 int health;
 int max_health;
 int shield;
 int shield_visible;
 int armour;
 int armour_pulse;
 int max_armour;
 int cloak;
 int uncloaked;
 int shadow;
 int spotlight;
 int regenerator;
 int time_since_hurt;
 int remote_control;
 int remote_thing;
 int firing_laser;
 int laser_strength;
 int laser_x;
 int laser_y;
 int lock_on;

 int propulsion;
 int backpack;

 char jet_on;
 int jet_toggle_delay;
 int jet_power;
 int jet_heat;
 int jet_speed;
 int jet_spin;

 int rocket_power;
 int rocket_charge;

 char jetting;
 char rocketing;

 char grapple_anchored;
 int grapple_power;
 int grapple_recycle;
 int grapple_bullet;
 int grapple_x;
 int grapple_y;

 char has_grail;
 int has_flag;
 char has_porkball;
 
 char prevent_jump;
 int jump_delay;
 char prevent_pickup;
 float angle_increment;
 int weapon [4];
 int ammunition [4];
 int clips [4];
 int reload [4];
 int max_clips;
 int current_weapon;
 char weapon_slots;
 int leg_anim_state;
 char walk_bounce [10];
 unsigned char dip;
 char total_dip;
 unsigned char leg_frame;
/* char colour_1_r;
 char colour_1_b;
 char colour_1_g;
 char colour_2_r;
 char colour_2_b;
 char colour_2_g;*/
 unsigned char colour1;
 unsigned char colour2;

 unsigned char async_last_timestamp;
 char incarnation;

 int lit; // will need to be set at the *end* of every turn
          //  to make sure all light sources are taken into account

 int soldier;
 int fire_x, fire_y, firing_distance;
};

struct player_struct
{
 int actor_controlled;
 int user;
 char changing_weapon;
 char slot_selected;
 int change_delay;
 char moving;
 char has_dug;
 int ingame_slot;
 char show_scores;
 char show_names;
 
 int weapon [4];
 char pname [20];
 int atype;
/* int colour_r;
 int colour_g;
 int colour_b;*/
 unsigned char colour1;
 unsigned char colour2;
 int handicap;
 int team;
 int soldier;

 char winner;
 
};

struct user_struct
{

 int active;

 int player;
 int actor;
 int status;

 int score;
 int lives;
 int out_of_lives;
 int ranked;
 int team;
 int weapon [4];
 char uname [20];
 int atype;
/* int colour_r;
 int colour_g;
 int colour_b;*/
 unsigned char colour1;
 unsigned char colour2;
 int handicap;
 int soldier;

 int penalty_time;
 int resting_time;
};

struct game_struct
{
 char gname [20];
// physics:
 int gravity;
 int bullet_speed;
// game parameters:
 int health_amount;
 int impact_damage;
 int reload_time;
 int game_type;
 int score_type;
 int score_limit;
 int lives_type;
 int lives_each;
 int starting_weapons;
 int fast_bullets;
 int bullets_explode;
 int soft_dirt;
 int rechoose_atype;
 int unlimited_clips;
 int reset_weapons;
 int penalty;
// hunt for fruit:
 int fruit_no;
// take & hold
 int th_base_no;
// level parameters:
 int weapon_status [NO_WEAPONS];
 int equip_status [NO_EQUIP];
 int pk_time_between;
 int pk_number;
 int pk_health;
 int pk_weapon;
 int pk_equip;

 char show_map;
 char blast_bullets;
 char atypes_avail;
 char lightsourcing;
 char name_boxes;

};

struct team_struct
{
 int active;
 int size;
 int score;
 int lives;
 int ranked;

 int base_x;
 int base_y;
 int base_counter;
 int flag_x;
 int flag_y;
 int flag_at_base;

};

struct ai_config_struct
{
 char name [30];
 int skill;
 int team;
 int active;
 int atype;
 int handicap;
 int soldier;
};

struct arena_struct
{
 int max_x;
 int max_y;

 int pk_counter;
// int grav;
 char dirt_fill; // is the arena filled with dirt? and if so, how much?
 char dirt_possible; // can dirt exist in this arena? (is 1 if: dirt_fill > 0,
  // or if dirt weapons are available, or if earthquakes are on.
 char shake_time;
 char shake_x;
 char shake_y;
 int nuke_flash;
 unsigned char counter;
 int ingame_slots;
 
 int teams;
 int ai_no_impact;
 int ai_unlimited_clips;
 int ai_weapons;

 int replace_dirt;
 int dirt_storm;
 int dirt_count;

 char has_bases;
 int th_base_x [5];
 int th_base_y [5];
 int th_base_taken [5];
 int th_base_counter [5];
 int th_base_angle [5];
 int th_base_speed [5];
 int th_base_move [5];
 int th_base_direction [5];
 int th_base_pickup [5];

 int grail_x;
 int grail_y;

 int porkball_x;
 int porkball_y;
 int porkball_state;
 int who_has_porkball;

 int level_colours;
 int level_style;
 int solid_density; // 0, 1, 4, 8 - the higher, the fewer solid things

 char level_type;

 BITMAP *bricks;
 BITMAP *light;
 BITMAP *base_light;

 int bricks_x, bricks_y;

 int ambient_light;
 int lamp_amount;
 char need_actor_lit; // is it possible for actors to be in darkness?
 // and if so, does it matter? (only if ais are present)

 char level_loaded [80];
 int generate_level;

 int challenge_level;
 int qstart;
 char quickstart_difficulty;
 char quickstart_dirt;
 char quickstart_lightsourced;
 int quickstart_weapons;

 int tournament_level;
//arena[0].melee_difficulty
};


struct bullet_list
{
         int bullet_type;
         unsigned int x;
         unsigned int y;
         unsigned int x_speed;
         unsigned int y_speed;
         int width;
         int height;
         float angle;
         unsigned char owner;
         char left_owner;
         int time_since_cloud;
         int damage;
         int fuse;
         int weight; // hmm, really should be 'mass'
         char facing;
         int status;
         int status2;
         int is_bullet;
         int seed;
         unsigned char base_angle;
         
};

struct cloud_list
{
         int cloud_type;
         unsigned int x;
         unsigned int y;
         unsigned int x_speed;
         unsigned int y_speed;
         int size;
         int rate;
         unsigned char rate_change;
         int state;
};

struct armoury
{
 char *long_name;
 char *short_name;
 int launch_speed;
 int slow_speed;
 int recycle_time;
 int ammunition;
 int is_bullet;
 int reload_time;
 int damage;
 int scatter;
 int recoil;
 int bullet_weight;
 int status;
};

struct pickup_list
{
         int pickup_type;
         int contains;
         unsigned int x;
         unsigned int y;
         unsigned int x_speed;
         unsigned int y_speed;
         int width;
         int height;
         int timeout;
         int wait_clear;

         int th_index;
};


struct serial_game
{
 int game_type;
 int com_port;
 int connection_type;
 
 int connections;
 int initialised;
 
 char who_decides_damage;

 char port_string [50];
 

};


struct option_struct
{
 char run_vsync;
// char show_map;
 char stipple_clouds;
 char show_fps;
 char positional_sound;
 char display_damage;
 char fuzzy_menu;
};


