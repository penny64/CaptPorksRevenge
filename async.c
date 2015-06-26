///*
//Captain Pork's Revenge
//Copyright (C) 2003 Captain Pork
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public Licence as published by
//    the Free Software Foundation; either version 2 of the Licence, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public Licence for more details.
//
//    You should have received a copy of the GNU General Public Licence
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    The GPL version 2 is included in this distribution in a file called
//    LICENCE.TXT. Use any text editor or the TYPE command to read it.
//
//    You should be able to reach Captain Pork by sending an email to
//    captainpork@fastmail.fm or sending a letter to
//    Captain Pork, 3/4 Kensington Rd, Rose Park, SA 5067 Australia
//    but my lease is a short-term one so email is probably your best bet.
//
//File: async.c
//History:
//Sometime in 2002 - Started work (Capt Pork)
//28/1/03 - Version 1.0 finalised (CP)
//
//This file contains:
// - The game code for multiplayer games played on multiple computers.
// - It doesn't do any interfacing with Libnet functions that I can think of
//   right now - all of that is handled in connect.c.
// - It's really ugly. Keep in mind that this is the first networked
//   application I've ever written. This may be obvious from my probably
//   unique use of terms like 'chunk', 'buffer' and 'register'.
//
//*/
//
//#include "allegro.h"
////#include "libnet.h"
//
//#include <math.h>
//#include <string.h>
//
//#include "config.h"
//
//#include "ai.h"
//#include "prand.h"
//#include "actor.h"
//#include "globvar.h"
////#include "connect.h"
//#include "cmds.h"
//#include "bullet.h"
//#include "assert.h"
//#include "display.h"
//#include "pickup.h"
//#include "sound.h"
//#include "score.h"
//#include "cloud.h"
//#include "move.h"
//
//#define CHUNK_CONTROL 254
//
//#define WPN_GRAPPLE WPN_SHIELD
//
////#define ASYNC_DEBUG
//
//enum
//{
//CHUNK_NONE,
//CHUNK_ACTOR,
//CHUNK_SPAWN,
//CHUNK_KILL,
//CHUNK_HURT,
//CHUNK_DIRT,
//CHUNK_GAMESTATE,
//CHUNK_END_TRANSMISSION, // not currently used
//CHUNK_STATUS,
//CHUNK_PICKUP,
//CHUNK_PICKUP_TAKEN,
//CHUNK_SCORE,
//CHUNK_BASE,
//CHUNK_SMART_INIT,
//CHUNK_SMART,
//CHUNK_SMART_DESTROY,
//CHUNK_CLIENT_ID,
//CHUNK_REMOVE_USER,
//CHUNK_END_GAME,
//
//ACK_CHUNK_ACTOR,
//ACK_CHUNK_SPAWN,
//ACK_CHUNK_KILL,
//ACK_CHUNK_HURT,
//ACK_CHUNK_DIRT,
//ACK_CHUNK_PICKUP,
//ACK_CHUNK_PICKUP_TAKEN,
//ACK_CHUNK_SCORE,
//ACK_CHUNK_BASE,
//ACK_CHUNK_SMART_INIT,
//ACK_CHUNK_SMART_DESTROY,
//ACK_CHUNK_REMOVE_USER,
//ACK_CHUNK_END_GAME
//
//
//
//};
//
//
///*
//Guided weapons work like this:
//- Smart weapons are fired by the client, then controlled by the server.
//  - firing actor chunk is sent by client
//  - server receives it and assigns the bullet an index in the smart weapons
//    struct.
//  - server then relays this actor chunk to all clients, putting the index
//    in the seed field of the chunk (which will be buffered as it's a firing
//    chunk).
//  - each turn, server sends unbuffered updates of the weapon's location,
//    speed etc.
//  - clients can't destroy the weapon except by fuse expiry. Server sends out
//    a buffered update with a command to destroy the weapon when it needs to.
//
//- Remote weapons work similarly, except that there is also a chunk passed
//  from the client controlling the weapon to the server which in turn affects
//  the updates being sent out.
//
//CHUNK_SMART_INIT
//- Buffered.
//- Only sent by the server to the client who fired the weapon, as it's sent
//  to all other clients in the firing actor chunk.
//- 1b index
//- 1b type
//- 3b x
//- 3b y
//- s2b xs
//- s2b ys
//- 1b angle
//- 1b seed/lock
//- 14 bytes
//
//CHUNK_SMART
//- Not buffered
//- Sent by server to all clients except the one that sent it (if any)
//- Sent by controlling client (if there is one) to server
//- 1b index
//- 3b x
//- 3b y
//- s2b xs
//- s2b ys
//- 1b angle
//- 1b seed/lock
//- 13 bytes
//
//CHUNK_SMART_DESTROY
//- Buffered
//- Send by server to all clients including controller
//- 1b index
//- 3b x
//- 3b y
//- 5 bytes
//
//*/
//
//// don't include the first byte containing CHUNK_*
//#define CHUNK_ACTOR_SIZE 18
//#define CHUNK_DIRT_SIZE 10
//#define CHUNK_SPAWN_SIZE 2
//#define CHUNK_KILL_SIZE 2
//#define CHUNK_HURT_SIZE 4
//#define CHUNK_STATUS_SIZE 3
//#define CHUNK_PICKUP_SIZE 12
//#define CHUNK_PICKUP_TAKEN_SIZE 3
//#define CHUNK_SCORE_SIZE 4
//#define CHUNK_SMART_INIT_SIZE 16
//#define CHUNK_SMART_SIZE 15
//#define CHUNK_SMART_DESTROY_SIZE 8
//#define CHUNK_CLIENT_ID_SIZE 1
//#define CHUNK_REMOVE_USER_SIZE 1
//#define CHUNK_END_GAME_SIZE 1
//// NOTE: chunk_gamestate_size is not #defined as it's variable:
//int var_chunk_gamestate_size;
//
//#define NO_SMART 200
//
//char last_address_received [10];
//
//int smart_bullet [NO_SMART];
//int smart_client [NO_SMART];
//int smart_user [NO_SMART];
//unsigned char smart_incarn [NO_SMART];
//
//// also defined in connect.c
//#define NO_CLIENTS 4
//
//unsigned char chunk [1000];
//int chunk_pos;
//
//unsigned char inchunk [1000];
//int inchunk_pos;
//
//#define BUFFER_SAFE_LIMIT 900
//
//
//void append_1b(unsigned char app);
//void append_2b(int app);
//void append_s2b(int app);
//void append_3b(int app);
//
//unsigned char get_1b(void);
//int get_2b(void);
//int get_s2b(void);
//int get_3b(void);
//
//
//// Also defined in connect.c:
//#define ASYNC_BUFFER_NO 300
//#define ASYNC_BUFFER_SIZE 20
//#define ASYNC_REGISTER_SIZE 20
//#define ASYNC_REGISTER_NO 300
//
//void init_async_turn(void);
//void init_async(void);
////int read_inchunk(NET_CHANNEL *inchunk_channel, char process, char client_no [1]);
//void interpret_inchunk(int client_no);
//void async_server(void);
//void send_to_client(int which_client);
//void async_client(void);
//void add_actor_chunk(int chactor);
////void send_chunk(NET_CHANNEL *chunk_channel);
////char check_chunk(NET_CHANNEL *chunk_channel);
//void apply_actor_chunk(void);
//void apply_dirt_chunk(void);
//int time_elapsed(int time1, int time2);
//int find_buffer_match(const char *buffer, int ch_pos);
//int find_buffer_match2(const char *buffer, unsigned char chunkmatch [30], int length);
//int buffer_copy(const char *from_buffer, int fbloc, char *to_buffer, int tbloc);
//int find_buffer_space(const char *buffer, int spaces);
//int shift_forward(int chunk_type);
//int associated_chunk(int chunk_type);
//void dump_buffer(char *to_buffer, char *from_buffer);
//void issue_ack(int size, int type);
//int clear_buffer_space(char *buffer, int loc, int space);
//void receive_ack(int ack_length, int client_no);
//void append_gamestate_chunk(void);
//void append_status_chunk(int chactor);
//void apply_status_chunk(void);
//void apply_gamestate_chunk(void);
//void init_gamestate_chunk_size(void);
//void skip_inchunk(int skipped);
//void apply_score_chunk(void);
//
//char inchunk_broken;
//
//char user_fired [NO_USERS];
//int user_seed [NO_USERS];
//int user_x [NO_USERS];
//int user_y [NO_USERS];
//int user_x_speed [NO_USERS];
//int user_y_speed [NO_USERS];
//char user_facing [NO_USERS];
//unsigned char user_angle [NO_USERS];
//
//void debug_show_buffer_size(void);
//
//int find_empty_client_buffer(void);
//int find_empty_server_buffer(int which_client);
//void copy_to_server_buffer(const char *from_buffer, int fbloc, int client_no);
//void copy_to_client_buffer(const char *from_buffer, int fbloc);
//void dump_server_buffer(int which_client);
//void dump_client_buffer(void);
//int find_client_buffer_match(int ch_pos);
//int find_server_buffer_match(int which_client, int ch_pos);
//
//int find_empty_server_register(int which_client);
//int find_empty_client_register(void);
//void copy_to_server_register(const char *from_buffer, int fbloc, int client_no);
//void copy_to_client_register(const char *from_buffer, int fbloc);
//int find_client_register_match(int ch_pos);
//int find_server_register_match(int which_client, int ch_pos);
//
//int check_timestamp_received(int timestamp);
//void register_timestamp_received(int timestamp);
//void update_timestamps(void);
//void append_end_chunk(void);
//
//void async_init_a_client(int which_client);
//
//void apply_spawn_chunk(void);
//void apply_hurt_chunk(void);
//void apply_kill_chunk(void);
//void apply_pickup_chunk(void);
//void apply_pickup_taken_chunk(int client_no);
//
//
//void apply_smart_chunk(void);
//void apply_smart_init_chunk(void);
//void apply_smart_destroy_chunk(void);
//void init_smart_bullet(int weapon_type, int x, int y, int xs, int ys, int base_angle, int smart_index, int lock, int owner);
//void async_smart_init(int abull, int client_no, int weapon);
//
//void apply_end_game_chunk(int eg_status);
//void remove_user(int which_user);
//void async_end_game(int end_status);
//
//
//
//struct client_struct
//{
// unsigned char server_buffer [ASYNC_BUFFER_NO] [ASYNC_BUFFER_SIZE];
// char server_received [256];
// int sb_pos;
// unsigned char server_register [ASYNC_REGISTER_NO] [ASYNC_REGISTER_SIZE];
// int active;
// int no_players;
//
// char address [50];
// //NET_CHANNEL *client_channel;
//
// // to mimic player_struct
// char player_active [2];
// int player_actor [2];
// int player_user [2];
//// int player_colour_r [2];
//// int player_colour_g [2];
//// int player_colour_b [2];
// int player_colour1 [2];
// int player_colour2 [2];
// int player_handicap [2];
// int player_soldier [2];
// int player_team [2];
// char player_name [2] [20];
//
//};
//
//int local_client_no;
//
////extern NET_CHANNEL *comm_channel;
//
//struct client_struct client [NO_CLIENTS];
//
//unsigned char client_buffer [ASYNC_BUFFER_NO] [ASYNC_BUFFER_SIZE];
//char client_received [256];
//
//unsigned char client_register [ASYNC_REGISTER_NO] [ASYNC_REGISTER_SIZE];
//
//int cb_pos;
//
//extern volatile unsigned char tick_counter;
//extern int game_over;
//extern struct ai_config_struct ai_config [MAX_AI];
//
//unsigned char async_tick;
//unsigned char score_count;
//
//void init_async(void)
//{
//
// arena[0].counter = 15;
//
// chunk_pos = 0;
// inchunk_pos = 0;
// inchunk_broken = 0;
// async_tick = 0;
//
// int i, j, k;
//
// for (i = 0; i < NO_USERS; i ++)
// {
//  user_fired [i] = WPN_NONE;
// }
//
// for (i = 0; i < 256; i ++)
// {
//  client_received [i] = 0;
// }
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  for (j = 0; j < ASYNC_BUFFER_SIZE; j ++)
//  {
//   client_buffer [i] [j] = CHUNK_NONE;
//  }
// }
//
// for (i = 0; i < ASYNC_REGISTER_NO; i ++)
// {
//  for (j = 0; j < ASYNC_REGISTER_SIZE; j ++)
//  {
//   client_register [i] [j] = CHUNK_NONE;
//  }
// }
// 
// cb_pos = 0;
//
// 
// for (i = 0; i < NO_CLIENTS; i ++)
// {
//  client[i].active = 0;
//  client[i].client_channel = NULL;
//  client[i].player_active [0] = 0;
//  client[i].player_active [1] = 0;
//  client[i].player_user [0] = -1;
//  client[i].player_user [1] = -1;
//  for (j = 0; j < 256; j ++)
//  {
//   client[i].server_received [j] = 0;
//  }
//  for (j = 0; j < ASYNC_BUFFER_NO; j ++)
//  {
//   for (k = 0; k < ASYNC_BUFFER_SIZE; k ++)
//   {
//    client[i].server_buffer [j] [k] = CHUNK_NONE;
//    client[i].sb_pos = 0;
//    client[i].active = 0;
//   }
//  }
//  for (j = 0; j < ASYNC_REGISTER_NO; j ++)
//  {
//   for (k = 0; k < ASYNC_REGISTER_SIZE; k ++)
//   {
//    client[i].server_register [j] [k] = CHUNK_NONE;
//   }
//  }
// }
//
// for (i = 0; i < NO_SMART; i ++)
// {
//  smart_bullet [i] = -1;
//  smart_client [i] = -1;
//  smart_user [i] = -1;
//  smart_incarn [i] = 0;
// }
//
//
// var_chunk_gamestate_size = 0;
// 
//}
//
//
///*
//
//Goes through important things and sends them to the server when
// the client initiates the connection.
//
//*/
//void async_client_join(void)
//{
//
//// has already sent the initial contact (199)
//
////get_buffer_channel();
//
//  chunk_pos = 0;
//  append_1b(no_players);
//  append_1b(player[1].colour1);
//  append_1b(player[2].colour1);
//  append_1b(player[1].colour2);
//  append_1b(player[2].colour2);
//  append_1b(player[1].colour1);
//  append_1b(player[2].colour1);
//  append_1b(player[1].handicap);
//  append_1b(player[2].handicap);
//  append_1b(player[1].soldier);
//  append_1b(player[2].soldier);
//  append_1b(player[1].team);
//  append_1b(player[2].team);
//
//  int j;
//
//  for (j = 0; j < 20; j ++)
//  {
//   append_1b(player[1].pname [j]);
//  }
//  for (j = 0; j < 20; j ++)
//  {
//   append_1b(player[2].pname [j]);
//  }
//
//  send_chunk(comm_channel);
////  rest(5000);
//
//}
//
//
//int async_new_client(char *client_address, NET_CHANNEL *cl_channel)
//{
//
//// has already received the initial contact (199)
//
// int i, success, num_players;
//
//// int j;
//// char clr_prnt [100];
//// char itstring [10];
//// strcpy(clr_prnt, "");
//
// for (i = 0; i < 100; i ++)
// {
//  if (i == NO_CLIENTS) return -1;
//  if (client[i].active == 1) continue;
//
//  client[i].active = 1;
//  client[i].no_players = 1;
////  client[i].player_actor [1] =
//  strcpy(client[i].address, client_address);
//  client[i].client_channel = cl_channel;
//
//  inchunk_pos = 0;
//
//  char get_client_no [1];
//
//  do
//  {
////   success = get_buffer_channel(cl_channel);
//    success = read_inchunk(cl_channel, 0, get_client_no);
//  } while (success <= 0);
//
//  // to mimic player_struct
///*  client[i].player_actor [0] = get_1b();
//  client[i].player_actor [1] = get_1b();
//  client[i].player_user [0] = get_1b();
//  client[i].player_user [1] = get_1b();*/
//  num_players = get_1b();
//  client[i].player_active [0] = 1;
//  if (num_players == 2)
//   client[i].player_active [1] = 1;
//  client[i].player_colour1 [0] = get_1b();
//  client[i].player_colour1 [1] = get_1b();
//  client[i].player_colour2 [0] = get_1b();
//  client[i].player_colour2 [1] = get_1b();
//  client[i].player_colour1 [0] = get_1b();
//  client[i].player_colour1 [1] = get_1b();
//  client[i].player_handicap [0] = get_1b();
//  client[i].player_handicap [1] = get_1b();
//  client[i].player_soldier [0] = get_1b();
//  client[i].player_soldier [1] = get_1b();
//
///*  textprintf(screen, large_font, 150, 100 + i * 15, 15, "Client %i Handicap %i",
//   i, client[i].player_handicap [0]);
//  for (j = 0; j < 60; j ++)
//  {
////   clr_prnt [j] = inchunk [j];
//   strcat(clr_prnt, itoa(inchunk [j], itstring, 10));
//  }
//  textprintf(screen, large_font, 5, 150, 15, clr_prnt);
//  rest(1500);*/
//  client[i].player_team [0] = get_1b();
//  client[i].player_team [1] = get_1b();
//
//  int j;
//
//  for (j = 0; j < 20; j ++)
//  {
//   client[i].player_name [0] [j] = get_1b();
//  }
//  for (j = 0; j < 20; j ++)
//  {
//   client[i].player_name [1] [j] = get_1b();
//  }
//  client[i].player_name [0] [19] = '\0';
//  client[i].player_name [1] [19] = '\0';
//
////  strcpy(client[i].player_name [0], "Client 1");
////  strcpy(client[i].player_name [1], "Client 2");
////  textprintf(screen, large_font, 400, i * 15, 15, "New Client %i, Players %i", i, num_players);
////  rest(5000);
//  break;
//
// }
// 
// return i;
//
//}
//
///*
//Client function:
//Receives details of all clients from server
//*/
//void async_client_get_clients(void)
//{
//
//  int success = 0;
//
//  inchunk_pos = 0;
//
//  char get_client_no [1];
//
//  do
//  {
////   success = get_buffer_channel(comm_channel);
//   success = read_inchunk(comm_channel, 0, get_client_no);
//  } while (success <= 0);
//
//  int num_users = get_1b();
//  local_client_no = get_1b();
//
//  int i;
//  int play = 1;
//
//  for (i = 0; i < num_users; i ++)
//  {
//    user[i].actor = get_1b();
//    actor[user[i].actor].aclass = ACLASS_GHOST;
//    actor[user[i].actor].user = i;
//    user[i].colour1 = get_1b();
//    user[i].colour2 = get_1b();
//    user[i].colour1 = get_1b();
//    user[i].handicap = get_1b();
//    user[i].soldier = get_1b();
//    user[i].team = get_1b();
//    if (user[i].team != TEAM_NONE)
//     team[user[i].team].active = 1;
//
//    int j;
//
//    for (j = 0; j < 20; j ++)
//    {
//     user[i].uname [j] = get_1b();
//    }
//    user[i].uname [19] = '\0';
//    
//    user[i].status = get_1b();
//    user[i].active = 1;
//    if (user[i].status == USTAT_LOCAL)
//    {
//     user[i].player = play;
//     player[play].actor_controlled = user[i].actor;
//     player[play].user = i;
//     play ++;
//    }
//     else
//     {
//      user[i].player = -1;
//     }
//#ifdef ASYNC_DEBUG
//    textprintf(screen, large_font, 10, 50 + i * 15, 15, "Num_users %i, User %i, Actor %i, Status %i", num_users, i, user[i].actor, user[i].status);
//    rest(500);
//#endif
//    
//  }
//
////    textprintf(screen, large_font, 10, 50 + i * 15, 15, "Num_users %i, User %i, Actor %i, Status %i", num_users, i, user[i].actor, user[i].status);
//    init_gamestate_chunk_size();
//    rest(1000);
//}
//
//
//
///*
//Server function:
//Inits clients as users & actors and sends relevant details of them to all
//clients.
//*/
//void async_init_clients(void)
//{
//
// int i;
//
//// Here we init the user structs according to what we've been sent and
////  the settings of any players located at the server:
//
// int ui = 0;
// int ac = 0;
// 
// user[ui].player = 1;
// user[ui].colour1 = player[1].colour1;
// user[ui].colour2 = player[1].colour2;
//// user[ui].colour_b = player[1].colour_b;
// user[ui].handicap = player[1].handicap;
// user[ui].soldier = player[1].soldier;
// user[ui].team = player[1].team;
// strcpy(user[ui].uname, player[1].pname);
// user[ui].actor = ac;
// user[ui].active = 1;
// if (user[ui].team != TEAM_NONE)
// {
//  team[user[ui].team].active = 1;
// }
// user[ui].status = USTAT_LOCAL;
//// actor[user[ui].actor].aclass = ACLASS_GHOST;
// ui ++;
// ac ++;
// if (no_players == 2)
// {
//  user[ui].player = 2;
//  user[ui].colour1 = player[2].colour1;
//  user[ui].colour2 = player[2].colour2;
////  user[ui].colour_b = player[2].colour_b;
//  user[ui].handicap = player[2].handicap;
//  user[ui].soldier = player[2].soldier;
//  user[ui].team = player[2].team;
//  if (user[ui].team != TEAM_NONE)
//   team[user[ui].team].active = 1;
//  strcpy(user[ui].uname, player[2].pname);
//  user[ui].actor = ac;
//  user[ui].active = 1;
//  user[ui].status = USTAT_LOCAL;
////  actor[user[ui].actor].aclass = ACLASS_GHOST;
//  ui ++;
//  ac ++;
// }
//// Other details of the server players are initialised in init_people
////  in proj.c, in the same way as local players in a local-only game are.
//
//
//  for (i = 0; i < MAX_AI; i ++)
//  {
//   if (ai_config[i].active == 0) continue;
//   user[ui].active = 1;
//   user[ui].player = 0;
//   user[ui].status = USTAT_AI;
//   actor[ac].aclass = ACLASS_GHOST;
//   user[ui].actor = ac;
//   user[ui].team = ai_config[i].team;
//   actor[ac].team = ai_config[i].team;
//   team[user[ui].team].size ++;
//   team[user[ui].team].active = 1;
//   user[ui].handicap = ai_config[i].handicap;
//   user[ui].soldier = ai_config[i].soldier;
//   if (ai_config[i].soldier == NO_SOLDIER_FILES)
//    user[ui].soldier = prand(NO_SOLDIER_FILES);
//   if (strlen(ai_config[i].name) == 0)
//    random_ai_name(user[ui].uname);
//     else
//      strcpy(user[ui].uname, ai_config[i].name);
//   init_ai_user(ui, ac, i);
//   ui ++;
//   ac ++;
//  }
//// this AI init section takes from the one in proj.c
//
// for (i = 0; i < NO_CLIENTS; i ++)
// {
//  if (client[i].active == 0) continue;
//  user[ui].player = 0;
//  user[ui].actor = ac;
//  user[ui].active = 1;
//  user[ui].status = USTAT_CLIENT;
//
//  user[ui].colour1 = client[i].player_colour1 [0];
//  user[ui].colour2 = client[i].player_colour2 [0];
////  user[ui].colour_b = client[i].player_colour_b [0];
//  user[ui].handicap = client[i].player_handicap [0];
//  user[ui].soldier = client[i].player_soldier [0];
//  user[ui].team = client[i].player_team [0];
//  if (user[ui].team != TEAM_NONE)
//   team[user[ui].team].active = 1;
//  strcpy(user[ui].uname, client[i].player_name [0]);
//#ifdef ASYNC_DEBUG
//  textprintf(screen, large_font, 10, 50 + ui * 15, 15, "Client %i User %i Handicap %i R %i G %i B %i N %s ", i, ui, user[ui].handicap, user[ui].colour1, user[ui].colour2, user[ui].colour2, user[ui].uname);
//#endif
//  client[i].player_user [0] = ui;
//  client[i].player_actor [0] = ac;
//
//  ui ++;
//  ac ++;
//
//
//  if (client[i].player_active [1] == 1)
//  {
//   user[ui].player = 0;
//   user[ui].actor = ac;
//   user[ui].active = 1;
//   user[ui].status = USTAT_CLIENT;
//
//   user[ui].colour1 = client[i].player_colour1 [1];
//   user[ui].colour2 = client[i].player_colour1 [1];
////   user[ui].colour1 = client[i].player_colour_b [1];
//   user[ui].handicap = client[i].player_handicap [1];
//   user[ui].soldier = client[i].player_soldier [1];
//   user[ui].team = client[i].player_team [1];
//   if (user[ui].team != TEAM_NONE)
//    team[user[ui].team].active = 1;
//   strcpy(user[ui].uname, client[i].player_name [1]);
//#ifdef ASYNC_DEBUG
//   textprintf(screen, large_font, 10, 50 + ui * 15, 15, "Client %i User %i Handicap %i R %i G %i B %i N %s ", i, ui, user[ui].handicap, user[ui].colour1, user[ui].colour2, user[ui].colour2, user[ui].uname);
//#endif
//   client[i].player_user [1] = ui;
//   client[i].player_actor [1] = ac;
//
//   ui ++;
//   ac ++;
//  }
//
////  textprintf(screen, large_font, 10, 50 + ui * 15, 15, "Client %i, user %i, actor %i, status %i, 2nd player %i", i, ui, ac, user[ui].status, client[i].player_active [1]);
//  rest(500);
//
// }
//
//// Here we send out the contents of the user structs to each client.
////  async_init_a_client sets the status of each user appropriately
// for (i = 0; i < NO_CLIENTS; i ++)
// {
//
//  if (client[i].active == 1) async_init_a_client(i);
//
// }
//
// init_gamestate_chunk_size();
//
// rest(1000);
//
//}
//
///*
//Server Function:
//Sends details of all users to client[which_client]
//*/
//void async_init_a_client(int which_client)
//{
//
//// first byte sent should be the number of users.
//
// int no_users = 0, i;
//
//// int ui = 0;
//
// chunk_pos = 0;
//
// for (i = 0; i < NO_USERS; i ++)
// {
//  if (user[i].active == 1)
//   no_users ++;
// }
//
// append_1b(no_users);
// append_1b(which_client);
//
//#ifdef ASYNC_DEBUG
//  textprintf(screen, large_font, 400, 50 + which_client * 15, 15, "Client %i, no_users %i", which_client, no_users);
//  rest(500);
//#endif
//
// for (i = 0; i < NO_USERS; i ++)
// {
//  if (user[i].active == 0) continue;
//
////  append_1b(
//
//  append_1b(user[i].actor);
//  append_1b(user[i].colour1);
//  append_1b(user[i].colour2);
//  append_1b(user[i].colour1);
//  append_1b(user[i].handicap);
//  append_1b(user[i].soldier);
//  append_1b(user[i].team);
//
//  int j;
//
//  for (j = 0; j < 20; j ++)
//  {
//   append_1b(user[i].uname [j]);
//  }
//
//  // status:
//  if (client[which_client].player_user [0] == i
//      || (client[which_client].player_active [1] &&
//       client[which_client].player_user [1] == i))
//      {
//       append_1b(USTAT_LOCAL);
//      }
//       else append_1b(USTAT_SERVER);
//
// }
// 
// send_chunk(client[which_client].client_channel);
//
//}
//
//
//
///*
//When a client joins, it sends details including:
// - how many players it has
// - what their settings (colour, name etc) are
//
//When game starts (and client list is definitive) server sends details
// of all clients' players (+ server player(s)) to each one.
//(done)
//
//*/
//
//void init_async_turn(void)
//{
//
// chunk_pos = 2;
//
// chunk [0] = CHUNK_CONTROL;
// chunk [1] = CHUNK_CONTROL;
// 
// inchunk_pos = 0;
// inchunk_broken = 0;
// 
// int i;
//
// for (i = 0; i < NO_USERS; i ++)
// {
//  user_fired [i] = WPN_NONE;
// }
//
//}
//
//void run_async(void)
//{
//
//// if (arena[0].counter % 2 == 0) return;
//
// init_gamestate_chunk_size();
//
// if (serial[0].game_type == SERIAL_SERVER)
//  async_server();
//   else
//    async_client();
//
// debug_show_buffer_size();
//   
//}
//
//void async_server(void)
//{
//
// async_tick = tick_counter;
//
//  int cl;
////  int chp = chunk_pos;
//  char get_client_no [1] = {0};
//
//// Problem: An ack is issued to all clients, not just the one who needs it.
////  While this won't cause problems (the client'll just ignore it, and in
////  any case many acks go to, not from, the server) it
////  does take time and bandwidth. I can't think of an easy solution that
////  doesn't involve each client having its own chunk.
//
///*  for (cl = 0; cl < NO_CLIENTS; cl ++)
//  {
//   if (client[cl].active == 0) continue;
//   inchunk_pos = 0;
//   do
//   {
//   }
//    while (!check_chunk(client[cl].client_channel));
/// *   if (check_chunk(client[cl].client_channel))
//   {
//    read_inchunk(client[cl].client_channel, 1, get_client_no);
//    if (check_chunk(client[cl].client_channel))
//    {* /
//     read_inchunk(client[cl].client_channel, 1, get_client_no);
////    }
////    client_received [get_client_no [0]] = cl + 10;
//    interpret_inchunk(get_client_no [0]);
////   }
//  }
//*/
//
//
//
//  char client_received [10] = {0,0,0,0,0,0,0,0,0,0};
//
//  for (cl = 0; cl < NO_CLIENTS; cl ++)
//  {
//   if (client[cl].active == 0) continue;
//   inchunk_pos = 0;
//   if (check_chunk(client[cl].client_channel))
//   {
//    read_inchunk(client[cl].client_channel, 1, get_client_no);
//    if (check_chunk(client[cl].client_channel))
//    {
//     read_inchunk(client[cl].client_channel, 1, get_client_no);
//    }
//    client_received [(int) get_client_no [0]] = cl + 10;
//    interpret_inchunk(get_client_no [0]);
//   }
//  }
//
////    rest(15);
//
////  rest(10);
//
//// let's do it again, for luck
//  for (cl = 0; cl < NO_CLIENTS; cl ++)
//  {
//   if (client[cl].active == 0) continue;
//   inchunk_pos = 0;
//   if (check_chunk(client[cl].client_channel))
//   {
//    read_inchunk(client[cl].client_channel, 1, get_client_no);
//    if (check_chunk(client[cl].client_channel))
//    {
//     read_inchunk(client[cl].client_channel, 1, get_client_no);
//    }
//    client_received [(int) get_client_no [0]] = cl + 10;
//    interpret_inchunk(get_client_no [0]);
//   }
//  }
//
//  int a = 0;
//  char clients [40];
//  char itstring [10];
//
//  strcpy(clients, "Received:  ");
//
//  for (a = 0; a < 10; a ++)
//  {
////   if (client_received [a] != 0)
//   strcat(clients, itoa(client_received [a], itstring, 10));
//   strcat (clients, "  ");
//  }
//
//#ifdef ASYNC_DEBUG
//   text_mode(0);
//   textprintf(screen, large_font, 10, 445, arena[0].counter, clients);
//#endif
//
//
//  if (tick_counter % 10 == 0)
//   append_gamestate_chunk();
// 
//// update_timestamps();
//
//// Can't put init_async_turn here because chunk_pos may not be zero
////  if async functions have been called from elsewhere.
//
//// Instead we store the current position and go back to it every time
////  we start assembling a chunk to send out.
//
//// First we add the game-state chunk and various other generic stuff:
////  <Goes here>
//
//// Now add data for the users which will need to go out to all clients:
////  (ie users local to the server)
//  int i, j;
//
//  for (i = 0; i < NO_USERS; i++)
//  {
//   if (user[i].status == USTAT_NONE) continue;
//
////   if (user[i].resting_time == 1)
////    issue_user_command(i, CMD_SPAWN_0);
//
//   switch(user[i].status)
//   {
//    case USTAT_AI: // should just be able to uncomment this?
//    case USTAT_LOCAL:
//    if (actor[user[i].actor].aclass == ACLASS_NONE
//        || actor[user[i].actor].soldier == SOLDIER_NONE)
//         break;
//    add_actor_chunk(user[i].actor);
//    if (user_fired [i] != WPN_NONE)
//    {
//     for (j = 0; j < NO_CLIENTS; j ++)
//     {
//      if (client[j].active == 1)
//       copy_to_server_buffer(chunk, chunk_pos - CHUNK_ACTOR_SIZE - 1, j);
//     }
//    }
//    break;
//   }
//  }
//
//// Next add data for client-specific users. Most of this only needs to
////  go out to particular clients and not others:
//
// int chunk_start = chunk_pos;
//
// for (i = 0; i < NO_CLIENTS; i ++)
// {
//  if (client[i].active == 0) continue;
//  chunk_pos = chunk_start;
//  send_to_client(i);
// }
//
// init_async_turn();
//
//}
//
//
//void send_to_client(int which_client)
//{
//
//  int i;
//
//  for (i = 0; i < NO_USERS; i++)
//  {
//   if (user[i].status == USTAT_NONE) continue;
//
//   switch(user[i].status)
//   {
//    case USTAT_CLIENT:
//    if (client[which_client].player_user [0] == i
//     || (client[which_client].player_active [1] == 1
//         && client[which_client].player_user [1] == i))
//    {
//   // Send the client its own actors' statuses, but not their positions etc.
//      append_status_chunk(user[i].actor);
//      continue;
//    }
//    
//    if (actor[user[i].actor].aclass == ACLASS_NONE
//        || actor[user[i].actor].soldier == SOLDIER_NONE)
//         break;
//    add_actor_chunk(user[i].actor);
//    if (user_fired [i] != WPN_NONE)
//    // user_fired is set for clients in apply_actor_chunk
//    {
//     copy_to_server_buffer(chunk, chunk_pos - CHUNK_ACTOR_SIZE - 1, which_client);
//    }
//    break;
//    // Problem: if server receives two chunks for the same actor in one
//    //  turn and the actor is firing in both, it will only fire once on
//    //  any other clients. Oh well.
//   }
//  }
//
////  int cl;
////  int chp = chunk_pos;
//
////  for (cl = 0; cl < NO_CLIENTS; cl ++)
////  {
////   if (client[whichcl].active == 0) continue;
//   
////   chunk_pos = chp; // Resets chunk so that each client only gets info
//                    //  relevant to it.
//
//   dump_server_buffer(which_client);
///*
//   if (check_chunk(client[cl].client_channel))
//   {
//    read_inchunk(client[cl].client_channel, 1);
//    if (check_chunk(client[cl].client_channel))
//    {
//     read_inchunk(client[cl].client_channel, 1);
//    }
//    interpret_inchunk(cl);
//   }*/
//
//   append_end_chunk();
//
//   if (chunk_pos > 4)
//   {
//    send_chunk(client[which_client].client_channel);
//   }
////  }
//
//
//}
//
//
//void async_client(void)
//{
//
// async_tick = tick_counter;
//// update_timestamps();
//
//
// int i;
//
// append_1b(CHUNK_CLIENT_ID);
// append_1b(local_client_no);
//
//  for (i = 0; i < NO_USERS; i++)
//  {
//   if (user[i].status == USTAT_NONE) continue;
//   if (user[i].active == 0) continue;
//
////  if (user[i].resting_time == 1)
////   issue_user_command(i, CMD_SPAWN_0);
//
//   switch(user[i].status)
//   {
///*    case USTAT_AI:
//    run_ai_user(i);
//    send_remote_commands(user_cmd_buffer [i], i);
//    break;*/
//    case USTAT_LOCAL:
//    if (actor[user[i].actor].aclass != ACLASS_PLAYER)
//     //        || actor[user[i].actor].atype == ATYPE_NONE)
//         break;
//    add_actor_chunk(user[i].actor);
//    if (user_fired [i] != WPN_NONE)
//     copy_to_client_buffer(chunk, chunk_pos - CHUNK_ACTOR_SIZE - 1);
////     buffer_copy(chunk, chunk_pos - CHUNK_ACTOR_SIZE, client_buffer, -1);
//    break;
////    case USTAT_CLIENT:
////    break;
//   }
//  }
//
////  if (async_tick % 3 == 0)
//   dump_client_buffer();
//
//   char get_client_no [1];
//
//  if (check_chunk(comm_channel))
//  {
//    read_inchunk(comm_channel, 1, get_client_no);
//    if (check_chunk(comm_channel))
//    {
//     read_inchunk(comm_channel, 1, get_client_no);
//    }
//    interpret_inchunk(0);
//  }
//
////  ASSERT(chunk_pos <= 900);
//
//  append_end_chunk();
//
//  if (chunk_pos > 4) // 2 control at start + 2 at end
//  {
//   send_chunk(comm_channel);
//  }
//
// init_async_turn();
//
//
//}
//
//void interpret_inchunk(int client_no)
//{
//
// inchunk_pos = 0;
// unsigned char timestamp = 0;
//
// char clr_print [100];
// char itstring [10];
//// int i;
//
//// for (i = 0; i < 100; i ++)
//// {
////  clr_print [i] = client_received [i] + 48;
//// }
//// clr_print [99] = 0;
////   textprintf(screen, large_font, 10, 165, arena[0].counter, clr_print);
// strcpy(clr_print, "From Client ");
// strcat(clr_print, itoa(client_no, itstring, 10));
// strcat(clr_print, " inp ");
// strcat(clr_print, itoa(inchunk_pos, itstring, 10));
// strcat(clr_print, " add ");
//// strcat(clr_print, client[client_no].address);
// strcat(clr_print, last_address_received);
// strcat(clr_print, ": ");
//
// while(inchunk [inchunk_pos] != CHUNK_NONE)
// {
//  switch(inchunk [inchunk_pos])
//  {
//   case CHUNK_REMOVE_USER:
//   issue_ack(CHUNK_REMOVE_USER_SIZE, ACK_CHUNK_REMOVE_USER);
//   inchunk_pos ++;
//   strcat(clr_print, "R");
//   strcat(clr_print, itoa(inchunk [inchunk_pos], itstring, 10));
//   remove_user(-1); // -1 tells it to read user no from inchunk
//   // don't bother registering this as it doesn't matter if it's reapplied
//   break;
//   case CHUNK_END_GAME:
//   issue_ack(CHUNK_END_GAME_SIZE, ACK_CHUNK_END_GAME);
//   inchunk_pos ++;
//   strcat(clr_print, "E");
//   strcat(clr_print, itoa(inchunk [inchunk_pos], itstring, 10));
//   apply_end_game_chunk(inchunk [inchunk_pos]);
//   // don't bother registering this as it doesn't matter if it's reapplied
//   break;
//   case CHUNK_GAMESTATE:
//   inchunk_pos ++;
//   apply_gamestate_chunk();
//   strcat(clr_print, "G");
//   break;
//   case CHUNK_STATUS:
//   inchunk_pos ++;
//   strcat(clr_print, "X");
//   strcat(clr_print, itoa(inchunk [inchunk_pos] , itstring, 10));
//   apply_status_chunk();
//   break;
//   case CHUNK_ACTOR:
//   strcat(clr_print, "A");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   timestamp = inchunk [inchunk_pos + 3];
//   if (inchunk [inchunk_pos + 2] != WPN_NONE)
//    issue_ack(CHUNK_ACTOR_SIZE, ACK_CHUNK_ACTOR);
//   inchunk_pos ++;
//   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//      if (inchunk [inchunk_pos + 1] != WPN_NONE)
//       copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//      apply_actor_chunk();
//     }
//       else
//        inchunk_pos += CHUNK_ACTOR_SIZE; // not + 1 because this done above
//   } else
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      if (inchunk [inchunk_pos + 1] != WPN_NONE)
//       copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_actor_chunk();
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_ACTOR_SIZE; // not + 1 because this done above
//    }
////   copy_to_server_buffer(inchunk, inchunk_pos - 1, 0);
////   if (check_timestamp_received(timestamp) < 2)
////    apply_actor_chunk();
////     else
////      inchunk_pos += CHUNK_ACTOR_SIZE; // not + 1 because this done above
////   register_timestamp_received(timestamp);
//   break;
//   case CHUNK_HURT:
//   strcat(clr_print, "H");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 3] , itstring, 10));
//   issue_ack(CHUNK_HURT_SIZE, ACK_CHUNK_HURT);
//   inchunk_pos ++;
//   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//      copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//      apply_hurt_chunk();
//     }
//       else
//        inchunk_pos += CHUNK_HURT_SIZE; // not + 1 because this done above
//   } else
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_hurt_chunk();
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_HURT_SIZE; // not + 1 because this done above
//    }
//   break;
//   case CHUNK_KILL:
//   strcat(clr_print, "K");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   issue_ack(CHUNK_KILL_SIZE, ACK_CHUNK_KILL);
//   inchunk_pos ++;
///*   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//      copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//      apply_kill_chunk();
//     }
//       else
//        inchunk_pos += CHUNK_KILL_SIZE; // not + 1 because this done above
//   } else*/
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_kill_chunk();
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_KILL_SIZE; // not + 1 because this done above
//    }
//   break;
//   case CHUNK_PICKUP:
//   strcat(clr_print, "P");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   issue_ack(CHUNK_PICKUP_SIZE, ACK_CHUNK_PICKUP);
//   inchunk_pos ++;
///*   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//      copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//      apply_pickup_chunk();
//     }
//       else
//        inchunk_pos += CHUNK__SIZE; // not + 1 because this done above
//   } else*/
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_pickup_chunk();
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_PICKUP_SIZE; // not + 1 because this done above
//    }
//   break;
//   case CHUNK_SCORE:
//   strcat(clr_print, "E");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   issue_ack(CHUNK_SCORE_SIZE, ACK_CHUNK_SCORE);
//   inchunk_pos ++;
//   if (find_client_register_match(inchunk_pos - 1) == -1)
//   // check if this chunk already received...
//   {
//    copy_to_client_register(inchunk, inchunk_pos - 1);
//    apply_score_chunk();
//   }
//     else
//   // ...and if so, ignore it.
//      inchunk_pos += CHUNK_SCORE_SIZE; // not + 1 because this done above
//   break;
//   case CHUNK_PICKUP_TAKEN:
//   strcat(clr_print, "T");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   issue_ack(CHUNK_PICKUP_TAKEN_SIZE, ACK_CHUNK_PICKUP_TAKEN);
//   inchunk_pos ++;
////   textprintf(screen, large_font, 200, 190, arena[0].counter, "P %i %i ", inchunk [inchunk_pos], inchunk [inchunk_pos + 1]);
//   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
////      copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
////       It's copied (to all clients) in the apply function:
//      apply_pickup_taken_chunk(client_no);
//     }
//       else
//        inchunk_pos += CHUNK_PICKUP_TAKEN_SIZE; // not + 1 because this done above
//   } else
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_pickup_taken_chunk(-1);
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_PICKUP_TAKEN_SIZE; // not + 1 because this done above
//    }
//   break;
//   case CHUNK_SPAWN:
//   strcat(clr_print, "S");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   issue_ack(CHUNK_SPAWN_SIZE, ACK_CHUNK_SPAWN);
//   inchunk_pos ++;
//   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//      copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//      apply_spawn_chunk();
//     }
//       else
//        inchunk_pos += CHUNK_SPAWN_SIZE; // not + 1 because this done above
//   }
////   apply_spawn_chunk();
////   actor_die(i, j);
//   break;
//
//   case CHUNK_SMART_INIT:
//   strcat(clr_print, "W(I)");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 3] , itstring, 10));
//   issue_ack(CHUNK_SMART_INIT_SIZE, ACK_CHUNK_SMART_INIT);
//   inchunk_pos ++;
//   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//       copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//       apply_smart_init_chunk();
//     }
//       else
//        inchunk_pos += CHUNK_SMART_INIT_SIZE; // not + 1 because this done above
//   } else
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_smart_init_chunk();
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_SMART_INIT_SIZE; // not + 1 because this done above
//    }
//   break;
//   case CHUNK_SMART_DESTROY:
//   strcat(clr_print, "W(D)");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   issue_ack(CHUNK_SMART_DESTROY_SIZE, ACK_CHUNK_SMART_DESTROY);
//   inchunk_pos ++;
//   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//       copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//       apply_smart_destroy_chunk();
//     }
//       else
//        inchunk_pos += CHUNK_SMART_DESTROY_SIZE; // not + 1 because this done above
//   } else
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_smart_destroy_chunk();
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_SMART_DESTROY_SIZE; // not + 1 because this done above
//    }
//   break;
//   case CHUNK_SMART:
//   strcat(clr_print, "W");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
////   issue_ack(CHUNK_SMART_SIZE, ACK_CHUNK_SMART);
//   inchunk_pos ++;
//   apply_smart_chunk();
///*   if (serial[0].game_type == SERIAL_SERVER)
//   {
//     if (find_server_register_match(client_no, inchunk_pos - 1) == -1)
//     {
//       copy_to_server_register(inchunk, inchunk_pos - 1, client_no);
//       apply_smart_chunk();
//     }
//       else
//        inchunk_pos += CHUNK_SMART_SIZE; // not + 1 because this done above
//   } else
//    {
//     if (find_client_register_match(inchunk_pos - 1) == -1)
//     // check if this chunk already received...
//     {
//      copy_to_client_register(inchunk, inchunk_pos - 1);
//      apply_smart_chunk();
//     }
//       else
//     // ...and if so, ignore it.
//        inchunk_pos += CHUNK_SMART_SIZE; // not + 1 because this done above
//    }*/
//   break;
//
//   case CHUNK_CLIENT_ID:
//   strcat(clr_print, "I");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   inchunk_pos ++;
//   if (inchunk [inchunk_pos] < NO_CLIENTS)
//    client_no = inchunk [inchunk_pos];
//   inchunk_pos += CHUNK_CLIENT_ID_SIZE;
//   break;
//
//   case ACK_CHUNK_REMOVE_USER:
//   strcat(clr_print, "r");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   receive_ack(CHUNK_REMOVE_USER_SIZE, client_no);
//   inchunk_pos += CHUNK_REMOVE_USER_SIZE + 1;
//   break;
//   case ACK_CHUNK_END_GAME:
//   strcat(clr_print, "e");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   receive_ack(CHUNK_END_GAME_SIZE, client_no);
//   inchunk_pos += CHUNK_END_GAME_SIZE + 1;
//   break;
//   case ACK_CHUNK_ACTOR:
//   strcat(clr_print, "a");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
////   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   receive_ack(CHUNK_ACTOR_SIZE, client_no);
//   inchunk_pos += CHUNK_ACTOR_SIZE + 1;
////   text_mode(0);
////   textprintf(screen, large_font, 280, 165, arena[0].counter, "Ack_actor");
//
//   break;
//   case ACK_CHUNK_HURT:
//   strcat(clr_print, "h");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 3] , itstring, 10));
//   receive_ack(CHUNK_HURT_SIZE, client_no);
//   inchunk_pos += CHUNK_HURT_SIZE + 1;
//   break;
//   case ACK_CHUNK_KILL:
//   strcat(clr_print, "k");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   receive_ack(CHUNK_KILL_SIZE, client_no);
//   inchunk_pos += CHUNK_KILL_SIZE + 1;
//   break;
//   case ACK_CHUNK_PICKUP:
//   strcat(clr_print, "p");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   receive_ack(CHUNK_PICKUP_SIZE, client_no);
//   inchunk_pos += CHUNK_PICKUP_SIZE + 1;
//   break;
//   case ACK_CHUNK_SCORE:
//   strcat(clr_print, "e");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   receive_ack(CHUNK_SCORE_SIZE, client_no);
//   inchunk_pos += CHUNK_SCORE_SIZE + 1;
//   break;
//   case ACK_CHUNK_PICKUP_TAKEN:
//   strcat(clr_print, "t");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   receive_ack(CHUNK_PICKUP_TAKEN_SIZE, client_no);
//   inchunk_pos += CHUNK_PICKUP_TAKEN_SIZE + 1;
//   break;
//   case ACK_CHUNK_SPAWN:
//   strcat(clr_print, "s");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   receive_ack(CHUNK_SPAWN_SIZE, client_no);
//   inchunk_pos += CHUNK_SPAWN_SIZE + 1;
//   break;
//   case ACK_CHUNK_SMART_INIT:
//   strcat(clr_print, "w(i)");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 3] , itstring, 10));
//   receive_ack(CHUNK_SMART_INIT_SIZE, client_no);
//   inchunk_pos += CHUNK_SMART_INIT_SIZE + 1;
//   break;
//   case ACK_CHUNK_SMART_DESTROY:
//   strcat(clr_print, "w(d)");
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 3] , itstring, 10));
//   receive_ack(CHUNK_SMART_DESTROY_SIZE, client_no);
//   inchunk_pos += CHUNK_SMART_DESTROY_SIZE + 1;
//   break;
//
///*   case ACK_CHUNK_DIRT:
//   receive_ack(CHUNK_DIRT_SIZE);
//   inchunk_pos += CHUNK_DIRT_SIZE + 1;
////   textprintf(screen, large_font, 280, 170, arena[0].counter, "Ack_dirt");
//   break;*/
//
//   case CHUNK_CONTROL:
//   strcat(clr_print, "C");
////   textprintf(screen, large_font, 280, 165, async_tick, "End");
////   update_timestamps();
//   inchunk_pos ++;
//   break;
//
//   default:
//   strcat(clr_print, "B!!");
//   strcat(clr_print, itoa(inchunk [inchunk_pos] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 1] , itstring, 10));
//   strcat(clr_print, itoa(inchunk [inchunk_pos + 2] , itstring, 10));
//   inchunk_broken = 1;
//   break;
//
//  }
//  if (inchunk_broken)
//   break;
//
//   // data corrupted. Flush input buffer?
//
// }
//
//#ifdef ASYNC_DEBUG
//   textprintf(screen, large_font, 10, 165 + (client_no * 15), arena[0].counter, clr_print);
//#endif
//}
//
//
//
//void remove_user(int which_user)
//{
//
// if (which_user == -1)
//  which_user = get_1b();
//
// char msg [50];
//
// if (which_user < 0 || which_user >= NO_USERS)
//  return;
//
// if (user[which_user].active == 1)
// {
//  strcpy(msg, user[which_user].uname);
//  strcat(msg, " has left the game.");
//  teamed_message(-1, msg, user[which_user].team);
//
//  user[which_user].active = 0;
//  actor[user[which_user].actor].aclass = ACLASS_NONE;
// }
//
//}
//
//void apply_end_game_chunk(int eg_status)
//{
//
// eg_status = get_1b();
//
// if (serial[0].game_type == SERIAL_SERVER)
// {
///*  for (i = 0; i < NO_CLIENTS; i ++)
//  {
//   if (client[i].active == 1)
//    copy_to_server_buffer(inchunk, inchunk_pos - CHUNK_END_GAME_SIZE - 1, j);
//  }*/
//  if (client[eg_status].active == 1)
//  {
//   remove_user(client[eg_status].player_user [0]);
//   inchunk [inchunk_pos - 2] = CHUNK_REMOVE_USER;
//   inchunk [inchunk_pos - 1] = client[eg_status].player_user [0];
//   copy_to_server_buffer(inchunk, inchunk_pos - 2, eg_status); // Horrible hack
//   if (client[eg_status].player_active [1])
//   {
//    remove_user(client[eg_status].player_user [1]);
//    inchunk [inchunk_pos - 2] = CHUNK_REMOVE_USER;
//    inchunk [inchunk_pos - 1] = client[eg_status].player_user [1];
//    copy_to_server_buffer(inchunk, inchunk_pos - 2, eg_status); // Horrible hack
//   }
//   client[eg_status].active = 0;
//  }
// }
//  else
//   {
//    // server must've sent it. So we end the game.
//    if (game_over == 0)
//    {
//     game_over = 200;
//     if (eg_status == 1) // server has terminated prematurely
//     {
//      message(-1, "Server has ended the game.", COLOUR_GREY8);
//     }
//    }
//   }
//
//}
//
//void async_end_game(int end_status)
//{
// int i;
//
// if (serial[0].game_type == SERIAL_SERVER)
// {
//  append_1b(CHUNK_END_GAME);
//  append_1b(end_status);
//  for (i = 0; i < NO_CLIENTS; i ++)
//  {
//   if (client[i].active == 0)
//    continue;
//   copy_to_server_buffer(chunk, chunk_pos - 2, i);
//  }
// }
//  else
//   {
//    append_1b(CHUNK_END_GAME);
//    append_1b(local_client_no);
//    copy_to_client_buffer(chunk, chunk_pos - 2);
//   }
//
//}
//
//
//void apply_spawn_chunk(void)
//{
//   int a = get_1b();
//   int b = get_1b();
///*   if (a < 0 || a > NO_ACTORS)
//    return;
//   if (actor[a].aclass == ACLASS_NONE)
//    return;
//   if (b < 0 || b > ATYPE_HEAVY_R)
//    return;*/
////   textprintf(screen, large_font, 200, 190, arena[0].counter, "R:as_sp %i %i", a, b);
//
//   user[actor[a].user].soldier = b; // probably does nothing
//   actor[a].soldier = b;
//
////   issue_user_command(actor[a].user, CMD_SPAWN_0 + b - 1);
////   respawn_command(actor[a].user, a);
//   spawn_actor(a, 1);
////   actor[a].aclass = ACLASS_PLAYER;
//}
//
//void apply_hurt_chunk(void)
//{
//   int a = get_1b();
//   int b = get_1b();
//   int c = get_2b();
///*   if (a < 0 || a > NO_ACTORS)
//    return;
//   if (actor[a].aclass == ACLASS_NONE)
//    return;
//   if (b < 0 || b > NO_ACTORS)
//    return;
//   if (actor[b].aclass == ACLASS_NONE)
//    return;*/
//   hurt_actor(a, b, c, 0, 1, 1);
////   textprintf(screen, large_font, 140, 190, arena[0].counter, "Hurt %i by %i (%i)", a, b, c);
//
//}
//
//void apply_kill_chunk(void)
//{
//   int a = get_1b();
//   int b = get_1b();
///*   if (a < 0 || a > NO_ACTORS)
//    return;
//   if (actor[a].aclass == ACLASS_NONE)
//    return;
//   if (b < 0 || b > NO_ACTORS)
//    return;
//   if (actor[b].aclass == ACLASS_NONE)
//    return;*/
//   actor_die(a, b);
////   textprintf(screen, large_font, 160, 190, arena[0].counter, "Kill %i by %i", a, b);
//}
//
///*
//void register_timestamp_received(int timestamp)
//{
// if (serial[0].game_type == SERIAL_SERVER)
// {
//  if (client[0].server_received [timestamp] == 0)
//   client[0].server_received [timestamp] = 1;
// } else
//  {
//   if (client_received [timestamp] == 0)
//    client_received [timestamp] = 1;
//  }
//
//}
//
//
//void update_timestamps(void)
//{
//
// int i;
// int to_delete1, to_delete2;
//
// to_delete1 = async_tick - 100;
// if (to_delete1 < 0) to_delete1 += 256;
// to_delete2 = async_tick - 101;
// if (to_delete2 < 0) to_delete2 += 256;
//  
// if (serial[0].game_type == SERIAL_SERVER)
// {
//  for (i = 0; i < 256; i ++)
//  {
//      if (client[0].server_received [i] == 1)
//       client[0].server_received [i] = 2;
//  }
//  
//  client[0].server_received [to_delete1] = 0;
//  client[0].server_received [to_delete2] = 0;
// } else
//    {
//     for (i = 0; i < 256; i ++)
//     {
//      if (client_received [i] == 1)
//       client_received [i] = 2;
//     }
//     client_received [to_delete1] = 0;
//     client_received [to_delete2] = 0;
//    }
//}
//*/
///*
//0 - means not received
//1 - means received this turn, so don't ignore.
//2 - means received
//
//int check_timestamp_received(int timestamp)
//{
//
// text_mode(0);
//
// if (serial[0].game_type == SERIAL_SERVER)
// {
////  textprintf(screen, large_font, 280, 165, async_tick, "ts %i  ", client[0].server_received [timestamp]);
//  return client[0].server_received [timestamp];
// }
// 
////  textprintf(screen, large_font, 280, 165, async_tick, "ts %i  ", client_received [timestamp]);
//  return client_received [timestamp];
//
//}
//*/
//
//
//void debug_show_buffer_size(void)
//{
// init_async_turn();
//
// int i = 0;
// int j = 0;
// int k;
// 
// if (serial[0].game_type == SERIAL_SERVER)
// {
//  k = client[0].server_buffer [0] [0];
//  for (i = 0; i < ASYNC_BUFFER_NO; i ++)
//  {
//   if (client[0].server_buffer [i] [0] != CHUNK_NONE)
//    j ++;
//  }
// } else
// {
//  k = client_buffer [0] [0];
//  for (i = 0; i < ASYNC_BUFFER_NO; i ++)
//  {
//   if (client_buffer [i] [0] != CHUNK_NONE)
//    j ++;
//  }
// }
//
////   text_mode(0);
////   textprintf(screen, large_font, 280, 180, 15, "B %i %i  ", j, k);
//}
//
//void issue_ack(int size, int type)
//{
// int i;
//
// for (i = 0; i < size + 1; i ++)
// {
//  if (i == 0) append_1b(type);
//   else
//    append_1b(inchunk [inchunk_pos + i]);
// }
//#ifdef ASYNC_DEBUG
// ASSERT(chunk_pos < 900);
//#endif
//}
//
//
//void receive_ack(int ack_length, int client_no)
//{
//
// int where_matched;
//
// switch(serial[0].game_type)
// {
//  case SERIAL_CLIENT:
//  where_matched = find_client_buffer_match(inchunk_pos);
//  if (where_matched != -1)
//   client_buffer [where_matched] [0] = CHUNK_NONE;
//    else
//    {
//     text_mode(0);
////     textprintf(screen, large_font, 280, 180, arena[0].counter, "No match");
//    }
//#ifdef ASYNC_DEBUG
//  ASSERT(where_matched < 900);
//#endif
//  break;
//  case SERIAL_SERVER:
//  where_matched = find_server_buffer_match(client_no, inchunk_pos);
//  if (where_matched != -1)
//   client[client_no].server_buffer [where_matched] [0] = CHUNK_NONE;
//  text_mode(0);
//#ifdef ASYNC_DEBUG
//  textprintf(screen, large_font, 250, 180, arena[0].counter, "W %i  ", where_matched);
//  ASSERT(where_matched < 900);
//#endif
//  break;
// }
//
//}
//
//
//
//
//
//
//
//void send_chunk(NET_CHANNEL *chunk_channel)
//{
//  int i;
//
//  for (i = 0; i < chunk_pos + 1; i ++)
//  {
//   add_byte_to_send_buffer(chunk [i], i);
//  }
//
//  send_buffer_channel(chunk_channel, chunk_pos + 1);
//#ifdef ASYNC_DEBUG
//  text_mode(0);
//  textprintf(screen, large_font, 250, 180, arena[0].counter, "Sent %i  ", chunk_pos + 1);
//#endif
//  // sends to chunk_pos + 1 because it's null-terminated (CHUNK_NONE)
//}
//
//
//
//int read_inchunk(NET_CHANNEL *inchunk_channel, char process, char client_no [1])
//{
// int i;
//
// int success = 0;
//
// int buffer_pos = 0;
//
// int timeout = 0;
//
// do
// {
//  success = get_buffer_channel(inchunk_channel);
//  timeout ++;
//  if (timeout > 100)
//  {
//   success = -1;
//   break;
//  }
// } while (success == 0);
//
// if (success <= 0) return -1;
//
// if (success > BUFFER_SAFE_LIMIT) success = BUFFER_SAFE_LIMIT;
//
//#ifdef ASYNC_DEBUG
// text_mode(0);
// textprintf(screen, large_font, 200, 180, arena[0].counter, "Got %i  ", success);
//#endif
//
// if (process == 0)
// {
//  inchunk_pos = 0;
//  while (buffer_pos < success - 1)
//  {
//   inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//   buffer_pos ++;
//   inchunk_pos ++;
//   inchunk [inchunk_pos] = CHUNK_NONE;
//  }
//  inchunk_pos = 0;
//  return 1; // we just want the buffer; don't worry about
//                           //  interpreting it
// }
//
//
//// while (inchunk [inchunk_pos] != CHUNK_NONE) //check_chunk())
// while (buffer_pos < success - 1)
// {
//  inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//  buffer_pos ++;
//  inchunk_pos ++;
//  switch(inchunk [inchunk_pos - 1])
//  {
//   case CHUNK_CLIENT_ID:
//   for (i = 0; i < CHUNK_CLIENT_ID_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   if (inchunk [inchunk_pos - 1] < NO_CLIENTS)
//    client_no [0] = inchunk [inchunk_pos - 1];
//   break;
//   case ACK_CHUNK_ACTOR:
//   case CHUNK_ACTOR:
//   for (i = 0; i < CHUNK_ACTOR_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
////   ASSERT(inchunk_pos < 900);
//   break;
//   case ACK_CHUNK_END_GAME:
//   case CHUNK_END_GAME:
//   for (i = 0; i < CHUNK_END_GAME_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_REMOVE_USER:
//   case CHUNK_REMOVE_USER:
//   for (i = 0; i < CHUNK_REMOVE_USER_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_DIRT:
//   case CHUNK_DIRT:
//   for (i = 0; i < CHUNK_DIRT_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
////   ASSERT(inchunk_pos < 900);
//   break;
//   case CHUNK_GAMESTATE:
//   for (i = 0; i < var_chunk_gamestate_size; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case CHUNK_STATUS:
//   for (i = 0; i < CHUNK_STATUS_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_HURT:
//   case CHUNK_HURT:
//   for (i = 0; i < CHUNK_HURT_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_KILL:
//   case CHUNK_KILL:
//   for (i = 0; i < CHUNK_KILL_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_PICKUP:
//   case CHUNK_PICKUP:
//   for (i = 0; i < CHUNK_PICKUP_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_SCORE:
//   case CHUNK_SCORE:
//   for (i = 0; i < CHUNK_SCORE_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_PICKUP_TAKEN:
//   case CHUNK_PICKUP_TAKEN:
//   for (i = 0; i < CHUNK_PICKUP_TAKEN_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_SPAWN:
//   case CHUNK_SPAWN:
//   for (i = 0; i < CHUNK_SPAWN_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_SMART_INIT:
//   case CHUNK_SMART_INIT:
//   for (i = 0; i < CHUNK_SMART_INIT_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case CHUNK_SMART:
//   for (i = 0; i < CHUNK_SMART_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case ACK_CHUNK_SMART_DESTROY:
//   case CHUNK_SMART_DESTROY:
//   for (i = 0; i < CHUNK_SMART_DESTROY_SIZE; i ++)
//   {
//    inchunk [inchunk_pos] = get_byte_from_rec_buffer(buffer_pos);
//    buffer_pos ++;
//    inchunk_pos ++;
//   }
//   inchunk [inchunk_pos] = CHUNK_NONE;
//   break;
//   case CHUNK_CONTROL:
////   inchunk_pos --;
//   break;
////   case CHUNK_END_TRANSMISSION:
////   inchunk_pos ++;
////   inchunk [inchunk_pos] = CHUNK_NONE;
////   break;
//  }
// }
//
//
//// Now check for errors:
//
// i = 0;
// int j = 0, k = 0;
// int b = 0;
//
// if (inchunk [inchunk_pos - 1] != CHUNK_CONTROL || inchunk [inchunk_pos - 2] != CHUNK_CONTROL)
// {
//  i = inchunk_pos - 1;
//  while (i > 1)
//  {
//   if (inchunk [i] == CHUNK_CONTROL && inchunk [i - 1] == CHUNK_CONTROL)
//   {
//    inchunk [i + 1] = CHUNK_NONE;
//    break;
//   }
//   i --;
//   b ++;
//  }
//  if (i <= 3)
//   inchunk [0] = CHUNK_NONE;
//#ifdef ASYNC_DEBUG
//  textprintf(screen, large_font, 200, 190, arena[0].counter, "Broken1(%i, %i)", i, b);
//#endif
// }
//
// if (inchunk [0] != CHUNK_NONE)
// {
//  if (inchunk [0] != CHUNK_CONTROL || inchunk [1] != CHUNK_CONTROL)
//  {
//#ifdef ASYNC_DEBUG
//   textprintf(screen, large_font, 250, 190, arena[0].counter, "Broken2");
//#endif
//   k = 0;
//   while (k < inchunk_pos - 1)
//   {
//    if (inchunk [k] == CHUNK_CONTROL && inchunk [k + 1] == CHUNK_CONTROL)
//    {
//     j = k;
//     break;
//    }
//    k ++;
//   }
//   if (k >= i - 3)
//   {
//    inchunk [0] = CHUNK_NONE;
//    j = 0;
//   }
//  }
// }
//
// inchunk [inchunk_pos] = CHUNK_NONE;
//
//
// inchunk_pos = j;
//  // which will be zero unless changed in the second error-checking loop
//
// return 1;
//
//}
//
//
//void append_end_chunk(void)
//{
// append_1b(CHUNK_CONTROL);
// append_1b(CHUNK_CONTROL);
//}
//
//
//
///*
//Note - dump_buffer assumes user_fired is the 3rd byte
//*/
//void add_actor_chunk(int chactor)
//{
//
// unsigned char control_byte = 0;
//
// append_1b(CHUNK_ACTOR);
// append_1b(chactor);
// append_1b(user_fired [actor[chactor].user]);
// append_1b(async_tick);
// if (user_fired [actor[chactor].user] != WPN_NONE)
// {
//  append_3b(user_x [actor[chactor].user]);
//  append_3b(user_y [actor[chactor].user]);
//  append_s2b(user_x_speed [actor[chactor].user]);
//  append_s2b(user_y_speed [actor[chactor].user]);
//  append_1b(user_angle [actor[chactor].user]);
//  if (user_facing [actor[chactor].user] == 1) control_byte ++;
//  if (actor[chactor].incarnation == 1) control_byte += 2;
//  if (actor[chactor].jetting) control_byte += 4;
//  if (actor[chactor].shield) control_byte += 8;
//  if (actor[chactor].armour != 0) control_byte += 16;
//  if (actor[chactor].rocketing) control_byte += 32;
//  if (actor[chactor].cloak && !actor[chactor].uncloaked) control_byte += 64;
//  if (actor[chactor].grapple_bullet != -1) control_byte += 128;
//  append_1b(control_byte);
//  control_byte = 0;
//  if (actor[chactor].shadow == 1) control_byte += 1;
//  if (actor[chactor].spotlight == 1) control_byte += 2;
//  if (actor[chactor].regenerator == 1) control_byte += 4;
//  if (actor[chactor].backpack == 1) control_byte += 8;
//  append_1b(control_byte);
//   // if this is changed, must change in non-shooting one below.
// }
//  else
//  {
//   append_3b(actor[chactor].x);
//   append_3b(actor[chactor].y);
//   append_s2b(actor[chactor].x_speed);
//   append_s2b(actor[chactor].y_speed);
//   append_1b(actor[chactor].base_angle);
//
//   unsigned char control_byte = 0;
//   if (actor[chactor].facing == 1) control_byte ++;
//   if (actor[chactor].incarnation == 1) control_byte += 2;
//   if (actor[chactor].jetting) control_byte += 4;
//   if (actor[chactor].shield) control_byte += 8;
//   if (actor[chactor].armour != 0) control_byte += 16;
//   if (actor[chactor].rocketing) control_byte += 32;
//   if (actor[chactor].cloak && !actor[chactor].uncloaked) control_byte += 64;
//   if (actor[chactor].grapple_bullet != -1) control_byte += 128;
//
//   append_1b(control_byte);
//
//   control_byte = 0;
//   
//   if (actor[chactor].shadow == 1) control_byte += 1;
//   if (actor[chactor].spotlight == 1) control_byte += 2;
//   if (actor[chactor].regenerator == 1) control_byte += 4;
//   if (actor[chactor].backpack == 1) control_byte += 8;
//
//   append_1b(control_byte);
//   // if this is changed, must change in shooting one above.
//
//  }
//
//  if (actor[chactor].aclass != ACLASS_PLAYER)
//   append_1b(SOLDIER_NONE);
//    else
//     append_1b(actor[chactor].soldier);
//
//  append_1b(user_seed [actor[chactor].user]);
//
//  actor[chactor].jetting = 0;
//  actor[chactor].rocketing = 0;
//  
//}
//
//
//void apply_actor_chunk(void)
//{
//// has already read the 1st byte (CHUNK_ACTOR)
//
// int chactor = get_1b();
// unsigned char control_byte = 0;
// unsigned char control_byte2 = 0;
// unsigned char fired = get_1b();
//
// unsigned char tstamp = get_1b();
//
// char incarn = 0;
//
// char out_of_date = 0;
//
// int x = get_3b();
// int y = get_3b();
// int xs = get_s2b();
// int ys = get_s2b();
// unsigned char bangle = get_1b();
// control_byte = get_1b();
// control_byte2 = get_1b();
// int aty = get_1b();
// int seed = get_1b();
//
// if (chactor < 0 || chactor > NO_ACTORS)
//  return;
//
// actor[chactor].firing_laser = 0;
//
//// if (time_elapsed(actor[chactor].async_last_timestamp, tstamp) == 0)
//// basically > 120 means that tstamp probably happened earlier
//// if (FALSE) //actor[chactor].async_last_timestamp == tstamp)
//// {
////  chunk_pos += CHUNK_ACTOR_SIZE - 3;
//// }
// if (time_elapsed(actor[chactor].async_last_timestamp, tstamp) > 120)
// {
//  out_of_date = 1;
// }
////  else
//  {
//  if (fired != WPN_NONE || out_of_date == 0)
//  {
//   if (x < 0 || x > arena[0].max_x * GRAIN)
//   {
//    inchunk_broken = 1;
//    return;
//   }
//   actor[chactor].x = x;
//   if (y < 0 || y > arena[0].max_y * GRAIN)
//   {
//    inchunk_broken = 1;
//    return;
//   }
//   actor[chactor].y = y;
//   actor[chactor].x_speed = xs;
//   if (actor[chactor].x_speed > 0)
//    walk_actor(chactor, 1);
//   if (actor[chactor].x_speed < 0)
//    walk_actor(chactor, -1);
//
//   if (actor[chactor].x_speed == 0)
//    actor[chactor].leg_anim_state = 150;
//
//   actor[chactor].y_speed = ys;
//   actor[chactor].base_angle = bangle;
//   actor[chactor].angle = ((float) actor[chactor].base_angle / 256 * PI) - (PI / 2);
//
//
//
//// Interpret Control byte:
//   if (control_byte % 2 == 1) actor[chactor].facing = 1;
//    else actor[chactor].facing = -1;
//   if ((control_byte / 2) % 2 == 1) incarn = 1;
//   if ((control_byte / 4) % 2 == 1)
//   {
//     if (arena[0].counter % 6 == 0)
////      play_sound2(WAV_JET, 100 + (actor[jactor].jet_spin * 5) + random() % 20, 250, 127);
//        play_sound_pos(WAV_JET, 500 + random() % 50, 250, actor[chactor].x, actor[chactor].y);
//
//     if (prand(2) == 0)
//      create_cloud(CLOUD_GREY_SMOKE, actor[chactor].x + (-4 * GRAIN * actor[chactor].facing), actor[chactor].y + 4 * GRAIN,
//       actor[chactor].x_speed, actor[chactor].y_speed + 32 * 20, 200 + prand(500), 20,
//       TRANS_WHITE, 0);
//     actor[chactor].jetting = 1;
//   }
//    else
//      actor[chactor].jetting = 0;
//
//   if ((control_byte / 8) % 2 == 1)
//   {
//     actor[chactor].shield = 1;
//   } else actor[chactor].shield = 0;
//   if ((control_byte / 16) % 2 == 1)
//   {
//     actor[chactor].armour = 1;
//   } else actor[chactor].armour = 0;
//   if ((control_byte / 32) % 2 == 1)
//   {
//    play_sound_pos(WAV_PROPROCK, 1000, 250, actor[chactor].x, actor[chactor].y);
//    create_cloud(CLOUD_GREY_SMOKE, actor[chactor].x + (-4 * GRAIN * actor[chactor].facing), actor[chactor].y,
//      actor[chactor].x_speed, actor[chactor].y_speed + 1100 * 1, 400, 20, 0, 0);
//    create_cloud(CLOUD_RED_EXPLOSION, actor[chactor].x + (-4 * GRAIN * actor[chactor].facing), actor[chactor].y,
//      actor[chactor].x_speed, actor[chactor].y_speed + 1100 * 1, 400, 20, 0, 10);
//    create_cloud(CLOUD_ORANGE_EXPLOSION, actor[chactor].x + (-4 * GRAIN * actor[chactor].facing), actor[chactor].y,
//      actor[chactor].x_speed, actor[chactor].y_speed + 1100 * 1, 400, 25, 0, 20);
//    create_cloud(CLOUD_YELLOW_EXPLOSION, actor[chactor].x + (-4 * GRAIN * actor[chactor].facing), actor[chactor].y,
//      actor[chactor].x_speed, actor[chactor].y_speed + 1100 * 1, 400, 30, 0, 30);
//    create_cloud(CLOUD_RISING_BIGLIGHT, actor[chactor].x + (-4 * GRAIN * actor[chactor].facing), actor[chactor].y,
//      actor[chactor].x_speed, actor[chactor].y_speed + 1100 * 1, 400 * 3, 20, 0, 10);
//   }
//   if ((control_byte / 64) % 2 == 1)
//   {
//    actor[chactor].cloak = 1;
//    actor[chactor].uncloaked = 0;
//   } else
//    {
//     actor[chactor].cloak = 0;
//    }
//   if ((control_byte / 128) % 2 == 0 && out_of_date == 0)
//   {
//    if (actor[chactor].grapple_bullet != -1)
//     destroy_grapple(actor[chactor].grapple_bullet);
//   }
//
//
//// Interpret control byte 2:
//   if (control_byte2 % 2 == 1)
//    actor[chactor].shadow = 1;
//     else actor[chactor].shadow = 0;
//
//   if ((control_byte2 / 2) % 2 == 1)
//    actor[chactor].spotlight = 1;
//     else actor[chactor].spotlight = 0;
//
//   if ((control_byte2 / 4) % 2 == 1)
//    actor[chactor].regenerator = 1;
//     else actor[chactor].regenerator = 0;
//
//   if ((control_byte2 / 8) % 2 == 1)
//    actor[chactor].backpack = 1;
//     else actor[chactor].backpack = 0;
//
////   }
//
//
//   if (aty != SOLDIER_NONE && incarn != actor[chactor].incarnation)
//   {
////    actor[chactor].atype = aty;
//    user[actor[chactor].user].soldier = aty;
//    actor[chactor].soldier = aty;
////    actor[chactor].aclass = ACLASS_PLAYER;
//    if (actor[chactor].soldier != aty || actor[chactor].aclass != ACLASS_PLAYER)
//    {
//     spawn_actor(chactor, 1);
//    }
//     else
//     {
//      if (actor[chactor].incarnation == 0) actor[chactor].incarnation = 1;
//       else actor[chactor].incarnation = 1;
//     }
//   }
//
//  }
//   else
//    {
///*     get_3b(); // x
//     get_3b(); // y
//     get_s2b(); // x_speed
//     get_s2b(); // y_speed
//     get_1b(); // angle
//     get_1b(); // control
//     get_1b(); // atype
//     get_1b(); // seed*/
//     return;
//    }
////    issue_user_command(actor[chactor].user, CMD_SPAWN_0 + aty - 1);
////     else
////      actor[chactor].atype = get_1b();
//
//
//   if (fired == WPN_GRAPPLE)
//   {
//    actor[chactor].angle = ((float) actor[chactor].base_angle / 256 * PI) - (PI / 2);
//    actor_grapple(chactor);
//    if (serial[0].game_type == SERIAL_SERVER
//     && user[actor[chactor].user].status != USTAT_LOCAL)
//      user_fired [actor[chactor].user] = WPN_GRAPPLE;
//     // So that the server can send this information out to clients.
//   } else
//   if (fired != WPN_NONE)
//   {
//#ifdef ASYNC_DEBUG
//    text_mode(0);
//    textprintf(screen, large_font, 10, 185, 15, "ats %i ts %i   ", actor[chactor].async_last_timestamp, tstamp);
//#endif
//    actor[chactor].angle = ((float) actor[chactor].base_angle / 256 * PI) - (PI / 2);
//    if (fired == WPN_SEEKER
//         || fired == WPN_TRACKER
//         || fired == WPN_BOUNCY
//         || fired == WPN_REMOTE_ROCKET
//         || fired == WPN_REMOTE_ROCKET_C)
//         {
//          if (serial[0].game_type == SERIAL_SERVER)
//          {
//           if (seed == 100) seed = -1;
//           int success = actor_shoot(chactor, fired, seed, seed);
//           if (success != -1)
//            async_smart_init(success, 0, fired);
//          }
//         }
//          else
//          {
//           actor_shoot(chactor, fired, seed, seed);
//          }
//    if (serial[0].game_type == SERIAL_SERVER
//     && user[actor[chactor].user].status != USTAT_LOCAL)
//      user_fired [actor[chactor].user] = fired;
//     // So that the server can send this information out to clients.
//   }
//  }
//
// actor[chactor].async_last_timestamp = tstamp;
//
//
//// textprintf(screen, large_font, 10, 460, 15, "x: %i, y: %i, x_speed: %i, y_speed: %i, ba: %i, cb: %i        ",
////  actor[chactor].x, actor[chactor].y, actor[chactor].x_speed,
////  actor[chactor].y_speed, actor[chactor].base_angle, control_byte);
//
//
//}
//
///*
//Server function:
//
//
//*/
//void append_status_chunk(int chactor)
//{
// append_1b(CHUNK_STATUS);
// append_1b(chactor);
// append_2b(actor[chactor].health);
//// append_2b(actor[chactor].armour);
//}
//
//void apply_status_chunk(void)
//{
// int chactor = get_1b();
// int read = get_2b();
//// int read2 = get_2b();
// if (chactor < 0 || chactor > NO_ACTORS)
//  return;
//
// if (read < 0 || read > actor[chactor].max_health)
//  return;
//  
// if (read < actor[chactor].health)
// {
//  actor[chactor].time_since_hurt = 0;
//  display_actor_health(chactor, actor[chactor].health - read);
//  actor[chactor].health = read;
// } else
//  {
//   if (read > actor[chactor].health)
//   {
////    actor[chactor].time_since_hurt = 0;
//    actor[chactor].health = read;
//    display_actor_health(chactor, 0);
//   }
//  }
//
//// if (read2 < 0 || read2 > 2000)
////  return;
//// if (actor[chactor].armour != read2)
//// {
////  actor[chactor].armour = read2;
////  display_actor_armour(chactor);
//// }
//
//// textprintf(screen, large_font, 200, 190, arena[0].counter, "Z %i %i %i %i ", chactor, read, read2, actor[chactor].health);
//
//
//}
//
//
//void async_pickup(int chpick, int type, int contains, int x,
//      int y, int timeout, int wait_clear)
//{
// append_1b(CHUNK_PICKUP);
// append_1b(chpick);
// append_1b(type);
// append_1b(contains);
// append_3b(x);
// append_3b(y);
// append_2b(timeout);
// append_1b(wait_clear);
//
//// textprintf(screen, large_font, 200, 190, arena[0].counter, "P %i %i %i ", chpick, x, y);
//
// int j;
// 
// for (j = 0; j < NO_CLIENTS; j ++)
// {
//  if (client[j].active == 1)
//   copy_to_server_buffer(chunk, chunk_pos - CHUNK_PICKUP_SIZE - 1, j);
// }
// 
//}
//
//
//void apply_pickup_chunk(void)
//{
//
// int chpick = get_1b();
// int type = get_1b();
// int contains = get_1b();
// int x = get_3b();
// int y = get_3b();
// int timeout = get_2b();
// int wait_clear = get_1b();
///*
// if (chpick < 0 || chpick > MAX_PICKUPS)
//  return;
// if (x < 0 || x > arena[0].max_x * GRAIN)
//  return;
// if (y < 0 || y > arena[0].max_y * GRAIN)
//  return;
// if (timeout < 0)
//  return;*/
//
// if (pickup[chpick].pickup_type != PICKUP_NONE)
// {
//  destroy_pickup(chpick);
// }
//
//
// create_pickup(type, contains, x, y, 0, 0, timeout,
//                         chpick, wait_clear);
//// int chp = create_pickup(type, contains, x, y, 0, 0, timeout,
////                         chpick, wait_clear);
//
//// textprintf(screen, large_font, 200, 190, arena[0].counter, "P %i %i %i %i ", chpick, x, y, chp);
//
//
//}
//
//void async_take_pickup(int chactor, int chpick)
//{
// if (serial[0].game_type == SERIAL_LOCAL_ONLY) return;
//
// append_1b(CHUNK_PICKUP_TAKEN);
// append_1b(chactor);
// append_1b(chpick);
// append_1b(pickup[chpick].wait_clear);
//
// if (serial[0].game_type == SERIAL_SERVER)
// {
//  int j;
//  for (j = 0; j < NO_CLIENTS; j ++)
//  {
//   if (client[j].active == 1)
//    copy_to_server_buffer(chunk, chunk_pos - CHUNK_PICKUP_TAKEN_SIZE - 1, j);
//  }
// }
//  else
//   copy_to_client_buffer(chunk, chunk_pos - CHUNK_PICKUP_TAKEN_SIZE - 1);
//
//// text_mode(0);
//// textprintf(screen, large_font, 200, 190, arena[0].counter, "P %i %i ", chpick, pickup[chpick].wait_clear);
//// textprintf(screen, large_font, 200, 190, arena[0].counter, "Pickup taken ");
//
//}
//
//
//void apply_pickup_taken_chunk(int client_no)
//{
//
// int chactor = get_1b();
// int chpick = get_1b();
// int wait_clear = get_1b();
// if (chpick < 0 || chpick > MAX_PICKUPS)
//  return;
//
// if (pickup[chpick].pickup_type != PICKUP_NONE
//     && pickup[chpick].wait_clear == wait_clear)
// {
//  if (pickup[chpick].pickup_type == PICKUP_WEAPON
//      || pickup[chpick].pickup_type == PICKUP_EQUIP)
//  {
//    play_sound_pos(WAV_PICKUP, 1000, 250, actor[chactor].x, actor[chactor].y);
//  }
//  if (pickup[chpick].pickup_type == PICKUP_POWER)
//  {
//    play_sound_pos(WAV_HEALTH, 1000, 250, actor[chactor].x, actor[chactor].y);
//  }
//  if (serial[0].game_type == SERIAL_SERVER)
//  {
//   switch(pickup[chpick].pickup_type)
//   {
//    case PICKUP_FRUIT:
//    pickup_game_thing(chactor, chpick, 1);
////    eat_fruit(chactor, chpick);
//    break;
//    case PICKUP_POWER:
//    pickup_game_thing(chactor, chpick, 1);
//    break;
//    case PICKUP_GRAIL:
//    pickup_game_thing(chactor, chpick, 1);
//    break;
//    case PICKUP_FLAG:
//    pickup_game_thing(chactor, chpick, 1);
//    break;
//   }
//  }
//  destroy_pickup(chpick);
// }
//
// if (serial[0].game_type == SERIAL_SERVER)
// {
//  if (client_no == -1) exit(67);
//  int j;
//  for (j = 0; j < NO_CLIENTS; j ++)
//  {
//   if (client[j].active == 1 && j != client_no)
//    copy_to_server_buffer(inchunk, inchunk_pos - CHUNK_PICKUP_TAKEN_SIZE - 1, j);
//  }
// }
//
//
//// text_mode(0);
//// textprintf(screen, large_font, 200, 190, arena[0].counter, "P %i ", chpick);
//
// 
//}
//
//
//void async_score_message(int stype, int user1, int user2)
//{
// append_1b(CHUNK_SCORE);
// append_1b(score_count);
// append_1b(stype);
// append_1b(user1);
// append_1b(user2);
//
// int j;
// 
// for (j = 0; j < NO_CLIENTS; j ++)
// {
//  if (client[j].active == 1)
//   copy_to_server_buffer(chunk, chunk_pos - CHUNK_SCORE_SIZE - 1, j);
// }
//
// score_count ++;
//#ifdef ASYNC_DEBUG
//   text_mode(0);
//   textprintf(screen, large_font, 220, 190, arena[0].counter, "Score Message ");
//#endif
//
//}
//
//void apply_score_chunk(void)
//{
// get_1b(); // the score_count value is only used for buffering/registering
// int stype = get_1b();
// int user1 = get_1b();
// int user2 = get_1b();
//
// score_message(stype, user1, user2, 1);
//
//}
//
//
////void async_client_scores
//
//
//void skip_inchunk(int skipped)
//{
//
// int i;
//
// for (i = 0; i < skipped; i ++)
// {
//  get_1b();
// }
//
//}
//
///*
//
//Server function.
//
//Appends:
//- score of each user or team
//- atype of each actor
//- status & location of each base and flag/grail
//
//Each client has the same game[0] information, so it knows exactly how
//to interpret this information (eg how many bases)
//
//Note: doesn't deal with fruit (they're considered ordinary pickups)
//
//*/
//void append_gamestate_chunk(void)
//{
//
// append_1b(CHUNK_GAMESTATE);
//
// int i;
// int find_grail = 100;
//
// if (arena[0].teams == 0)
// {
//  for (i = 0; i < NO_USERS; i ++)
//  {
//   if (user[i].active)
//    append_1b(user[i].score);
//  }
// } else
//  {
//   for (i = 0; i < NO_TEAMS; i ++)
//   {
//    if (team[i].active)
//     append_1b(team[i].score);
//   }
//  }
//
//
//
// for (i = 0; i < NO_USERS; i ++)
// {
//  if (!user[i].active) continue;
//  if (game[0].lives_each != 0)
//  {
//   append_1b(user[i].lives);
//  }
//  switch(game[0].game_type)
//  {
//   case GAME_GRAIL:
//   if (actor[user[i].actor].has_grail == 1)
//    find_grail = i;
//   break;
//  }
///*  if (actor[user[i].actor].aclass != ACLASS_PLAYER)
//   append_1b(ATYPE_NONE);
//    else
//     append_1b(actor[user[i].actor].atype);*/
// }
//
//// int flag;
//
// switch(game[0].game_type)
// {
//  case GAME_GRAIL:
//  append_1b(find_grail);
//  break;
///*  case GAME_CAP_FLAG:
//  for (i = 0; i < NO_TEAMS; i ++)
//  {
//   if (team[i].active == 0) continue;
//   flag = -1;
//     for (j = 0; j < MAX_PICKUPS; j ++)
//     {
//      if (pickup[j].pickup_type == PICKUP_FLAG
//       && pickup[j].contains == i)
//       {
//        flag = j;
//        break;
//       }
//     }
//     if (flag == -1)
//     {
//      append_3b(0);
//      append_3b(0);
//     }
//      else
//      {
//       append_3b(pickup[flag].x);
//       append_3b(pickup[flag].y);
//      }
//     flag = 100;
//     for (j = 0; j < NO_USERS; j ++)
//     {
//      if (user[j].active == 0) continue;
//      if (actor[user[j].actor].has_flag == i)
//      {
//       flag = j;
//      }
//     }
//     append_1b(flag);
//     append_1b(team[i].flag_at_base);
//  }
//  break;*/
//  case GAME_TAKEHOLD:
///*
// This is very inefficient, but at least it's robust and as I'm almost
//  finished the async code I want to get it out of the way.
//*/
//  for (i = 0; i < game[0].th_base_no; i ++)
//  {
//   append_1b(arena[0].th_base_pickup [i]);
//   append_3b(arena[0].th_base_x [i]);
//   append_3b(arena[0].th_base_y [i]);
//   append_1b(arena[0].th_base_taken [i]);
//   append_1b(arena[0].th_base_speed [i]);
//   append_1b(arena[0].th_base_direction [i] + 100);
//  }
//  break;
//
// }
//
//}
//
//
//void apply_gamestate_chunk(void)
//{
//
// int i, find_grail, x, y, j;
//
// int scoring = 0;
// char update = 0;
//
// if (arena[0].teams == 0)
// {
//  for (i = 0; i < NO_USERS; i ++)
//  {
//   if (user[i].active == 1)
//   {
//    scoring = get_1b();
//    if (user[i].score != scoring)
//     update = 1;
//    user[i].score = scoring;
//   }
//  }
// } else
//  {
//   for (i = 0; i < NO_TEAMS; i ++)
//   {
//    if (team[i].active == 1)
//    {
//     scoring = get_1b();
//     if (team[i].score != scoring)
//      update = 1;
//     team[i].score = scoring;
//    }
//   }
//  }
//
// if (game[0].lives_each != 0)
// {
//  for (i = 0; i < NO_USERS; i ++)
//  {
//   if (user[i].active == 1)
//   {
//    scoring = get_1b();
//    if (user[i].lives != scoring)
//     update = 1;
//    user[i].lives = scoring;
//   }
//  }
///*  actor[user[i].actor].atype = get_1b();
//  if (actor[user[i].actor].atype == ATYPE_NONE)
//   actor[user[i].actor].aclass = ACLASS_GHOST;
//    else
//     actor[user[i].actor].aclass = ACLASS_PLAYER;*/
// }
//
// if (update)
//  update_score();
//
// switch(game[0].game_type)
// {
//  case GAME_GRAIL:
//  find_grail = get_1b();
//  for (i = 0; i < NO_USERS; i ++)
//  {
//   if (user[i].active == 0) continue;
//   if (i == find_grail)
//    actor[user[i].actor].has_grail = 1;
//     else
//      actor[user[i].actor].has_grail = 0;
//  }
//  break;
///*  case GAME_CAP_FLAG:
//  for (i = 0; i < NO_TEAMS; i ++)
//  {
//   if (team[i].active == 0) continue;
//   flag = -1;
//     for (j = 0; j < MAX_PICKUPS; j ++)
//     {
//      if (pickup[j].pickup_type == PICKUP_FLAG
//       && pickup[j].contains == i)
//       {
//        flag = j;
//        break;
//       }
//     }
//     if (flag == -1) // Flag doesn't exist here, so can't do anything to it.
//     {
//      get_3b(); // and we ignore these.
//      get_3b();
//     }
//      else
//      {
//       x = get_3b();
//       y = get_3b();
//       if (x > 1 && x < arena[0].max_x * GRAIN
//           && y > 1 && y < arena[0].max_y * GRAIN)
//           {
//            pickup[flag].x = x;
//            pickup[flag].y = y;
//           }
//      }
//      flag = get_1b();
//      for (j = 0; j < NO_USERS; j ++)
//      {
//       if (user[j].active == 0) continue;
//       if (j == flag)
//        actor[user[j].actor].has_flag = i;
//         else
//          actor[user[j].actor].has_flag = -1;
//      }
//      flag = get_1b();
//      team[i].flag_at_base = flag;
//  }
//  break;*/
//  case GAME_TAKEHOLD:
//  for (i = 0; i < game[0].th_base_no; i ++)
//  {
//   j = get_1b();
//   if (j >= 0 && j < MAX_PICKUPS)
//   {
//    if (arena[0].th_base_pickup [i] >= 0
//        && arena[0].th_base_pickup [i] < MAX_PICKUPS)
//    {
//     if (j != arena[0].th_base_pickup [i])
//     {
//      if (arena[0].th_base_pickup [i] != -1)
//      {
//       create_cloud(CLOUD_COL_EXPLOSION, pickup[arena[0].th_base_pickup [i]].x, pickup[arena[0].th_base_pickup [i]].y, 0, 0, 2000, 40, 15, 10);
//       destroy_pickup(arena[0].th_base_pickup [i]);
//      }
//     }
//    }
//    arena[0].th_base_pickup [i] = j;
//    pickup[arena[0].th_base_pickup [i]].th_index = i;
//   }
//   x = get_3b();
//   y = get_3b();
//   if (x > 0 && x < arena[0].max_x * GRAIN && y > 0 && y < arena[0].max_y * GRAIN)
//   {
//    arena[0].th_base_x [i] = x;
//    arena[0].th_base_y [i] = y;
//   }
//   j = get_1b();
//   if (j >= 0 && j < NO_TEAMS)
//    arena[0].th_base_taken [i] = j;
//   arena[0].th_base_speed [i] = get_1b();
//   arena[0].th_base_direction [i] = get_1b() - 100;
//  }
//  break;
// }
//
//
//
//}
//
//
//void init_gamestate_chunk_size(void)
//{
//
// var_chunk_gamestate_size = 0;
//
// int i, j;
// 
// if (arena[0].teams == 0)
// {
//  for (i = 0; i < NO_USERS; i ++)
//  {
//   if (user[i].active == 1)
//    var_chunk_gamestate_size ++;
//  }
// } else
//  {
//   for (i = 0; i < NO_TEAMS; i ++)
//   {
//    if (team[i].active == 1)
//     var_chunk_gamestate_size ++;
//   }
//  }
//
// if (game[0].lives_each != 0)
// {
//  for (i = 0; i < NO_USERS; i ++)
//  {
//   if (user[i].active)
////  var_chunk_gamestate_size ++;
//     var_chunk_gamestate_size ++;
//  }
// }
//
// switch(game[0].game_type)
// {
//  case GAME_GRAIL:
//  var_chunk_gamestate_size ++; // who has the grail
//  break;
//  case GAME_CAP_FLAG:
//  j = 0;
//  for (i = 0; i < NO_TEAMS; i ++)
//  {
//   if (team[i].active) j ++;
//  }
//  var_chunk_gamestate_size += j * 8;
//  // 6b for each flag's x/y, 1 for who has it, 1 for is it at base?
//  break;
//  case GAME_TAKEHOLD:
//  var_chunk_gamestate_size += game[0].th_base_no * 10;
//  break;
// }
//
//}
//
///*
//Assumes that time1 happened before time2
//Also that the elapsed time < 256
//*/
//int time_elapsed(int time1, int time2)
//{
//
// if (time1 <= time2) return time2 - time1;
// time2 += 255;
// return time2 - time1;
//
//}
//
//
///*
//
//Functions to be called from elsewhere, indicating stuff that's happened
//
//*/
//
///*
//Issued by clients to the server. Never by server to clients.
//Basically instructs the server to reinitialise the client actor's
// health etc.
//*/
//void async_spawn(int sactor, int soldier)
//{
// append_1b(CHUNK_SPAWN);
// append_1b(sactor);
// append_1b(soldier);
//
// copy_to_client_buffer(chunk, chunk_pos - CHUNK_SPAWN_SIZE - 1);
//
//// textprintf(screen, large_font, 200, 190, arena[0].counter, "S:as_sp %i %i", sactor, satype);
//}
//
///*
//Only clients ever issue this, as it only ever goes from a client to
// the server (and may not even do that if who_decides_damage == DAM_SERVER)
//
//UPDATE - this function no longer used.
// 
//*/
//void async_hurt(int hactor, int hurter, int harm)
//{
//// return;
// append_1b(CHUNK_HURT);
// append_1b(hactor);
// append_1b(hurter);
// append_2b(harm);
//
// if (serial[0].game_type == SERIAL_CLIENT)
//   copy_to_client_buffer(chunk, chunk_pos - CHUNK_HURT_SIZE - 1);
// 
//}
//
//
///*
//Only the server ever issues this, as only it has authority to decide when
// someone dies - who_decides_damage only decides damage, not death.
//*/
//void async_kill(int kactor, int cause)
//{
//// return;
// append_1b(CHUNK_KILL);
// append_1b(kactor);
// append_1b(cause);
//
// int j;
// 
// for (j = 0; j < NO_CLIENTS; j ++)
// {
//  if (client[j].active == 1)
//   copy_to_server_buffer(chunk, chunk_pos - CHUNK_KILL_SIZE - 1, j);
// }
// 
//}
//
//
//void async_dirt(int x, int y, int radius, int colour)
//{
// return;
// append_1b(CHUNK_DIRT);
// append_1b(async_tick);
// append_3b(x);
// append_3b(y);
// append_2b(radius);
// append_1b(colour);
//
//}
//
//// not used
//void apply_dirt_chunk(void)
//{
//// unsigned char tstamp = get_1b();
// get_1b();
// int dirt_x = get_3b();
// int dirt_y = get_3b();
// int radius = get_2b();
// unsigned char colour = get_1b();
//
// if (colour == 0)
//  blast_dirt(dirt_x, dirt_y, radius, 0);
//   else
//    dirt_splodge(dirt_x, dirt_y, radius, colour);
//
//}
//
//void async_user_shoots(int shuser, int shactor, int wpn_fired)
//{
// user_fired [shuser] = wpn_fired;
// user_x [shuser] = actor[shactor].x;
// user_y [shuser] = actor[shactor].y;
// user_x_speed [shuser] = actor[shactor].x_speed;
// user_y_speed [shuser] = actor[shactor].y_speed;
// user_facing [shuser] = actor[shactor].facing;
// user_angle [shuser] = actor[shactor].base_angle;
// user_seed [shuser] = arena[0].counter;
// if (wpn_fired == WPN_SEEKER
//     || wpn_fired == WPN_REMOTE_ROCKET
//     || wpn_fired == WPN_REMOTE_ROCKET_C
//     || wpn_fired == WPN_TRACKER
//     || wpn_fired == WPN_BOUNCY)
//     {
//      user_seed [shuser] = actor[shactor].lock_on;
//      if (actor[shactor].lock_on == -1)
//       user_seed [shuser] = 100;
//      
//     }
////     textprintf(screen, small_font, 50, 165, 15, "x %i y %i xs %i ys %i, ba %i       ",
////     actor[shactor].x, actor[shactor].y, actor[shactor].x_speed, actor[shactor].y_speed, actor[shactor].base_angle);
// 
//}
//
///*
//
//CHUNK_SMART_INIT
//- Buffered.
//- Only sent by the server to the client who fired the weapon, as it's sent
//  to all other clients in the firing actor chunk.
//- 1b type
//- 1b index
//- 3b x
//- 3b y
//- s2b xs
//- s2b ys
//- 1b angle
//- 1b seed/lock
//- 14 bytes
//
//CHUNK_SMART
//- Not buffered
//- Sent by server to all clients except the one that sent it (if any)
//- Sent by controlling client (if there is one) to server
//- 1b index
//- 3b x
//- 3b y
//- s2b xs
//- s2b ys
//- 1b angle
//- 1b seed/lock
//- 13 bytes
//
//CHUNK_SMART_DESTROY
//- Buffered
//- Send by server to all clients including controller
//- 1b index
//- 3b x
//- 3b y
//- 5 bytes
//
//*/
//
//void async_smart(int abull)
//{
//
// int i;
//
// for (i = 0; i < NO_SMART + 1; i ++)
// {
//  if (i == NO_SMART) return; // oops
//  if (smart_bullet [i] == abull)
//   break;
// }
//
// append_1b(CHUNK_SMART);
// append_1b(i);
// append_1b(smart_incarn [i]);
// append_3b(bullet[abull].x);
// append_3b(bullet[abull].y);
// append_s2b(bullet[abull].x_speed);
// append_s2b(bullet[abull].y_speed);
// append_1b(bullet[abull].base_angle);
// append_1b(bullet[abull].status); // status is an int, but if it's > 255
//  // it's not important as this means it can't be a lock-on value
//  unsigned char control_byte = 0;
// if (bullet[abull].facing == 1)
//  control_byte ++;
// append_1b(control_byte);
//
//}
//
//
//
//void async_smart_init(int abull, int client_no, int weapon)
//{
//
// int i;
//
// for (i = 0; i < NO_SMART + 1; i ++)
// {
//  if (i == NO_SMART) return; // oops
//  if (smart_bullet [i] == -1)
//   break;
// }
//
// smart_bullet [i] = abull;
// smart_client [i] = client_no;
// smart_incarn [i] ++;
//
// append_1b(CHUNK_SMART_INIT);
// append_1b(i);
// append_1b(bullet[abull].owner);
// append_1b(smart_incarn [i]);
// append_1b(weapon); // SHOULD BE WPN TYPE
// append_3b(bullet[abull].x);
// append_3b(bullet[abull].y);
// append_s2b(bullet[abull].x_speed);
// append_s2b(bullet[abull].y_speed);
// append_1b(bullet[abull].base_angle);
// if (bullet[abull].status == -1)
//   append_1b(100);
//    else
//     append_1b(bullet[abull].status);
//
// // now copy to client buffers...
//
// int j;
// 
// for (j = 0; j < NO_CLIENTS; j ++)
// {
//  if (client[j].active == 1)
//   copy_to_server_buffer(chunk, chunk_pos - CHUNK_SMART_INIT_SIZE - 1, j);
// }
//
//}
//
///*
//CHUNK_SMART_INIT
//- Buffered.
//- Only sent by the server to the client who fired the weapon, as it's sent
//  to all other clients in the firing actor chunk.
//  - NO. Is sent by server to all clients, including the firer as the firer
//    can't create smart bullets itself - it has to wait for the server.
//- 1b index
//- 1b type (wpn, not bullet?)
//- 3b x
//- 3b y
//- s2b xs
//- s2b ys
//- 1b angle
//- 1b seed/lock
//// - 1b owner ??? may be necessary
//- 14 bytes
//
//*/
//
//void async_smart_destroy(int dbull)
//{
// int i;
//
// for (i = 0; i < NO_SMART + 1; i ++)
// {
//  if (i == NO_SMART) return; // oops
//  if (smart_bullet [i] == dbull)
//   break;
// }
//
// append_1b(CHUNK_SMART_DESTROY);
// append_1b(i);
// append_1b(smart_incarn [i]);
// append_3b(bullet[dbull].x);
// append_3b(bullet[dbull].y);
//
// smart_bullet [i] = -1;
//
// int j;
// 
// for (j = 0; j < NO_CLIENTS; j ++)
// {
//  if (client[j].active == 1)
//   copy_to_server_buffer(chunk, chunk_pos - CHUNK_SMART_DESTROY_SIZE - 1, j);
// }
//
//
//}
//
//
//void apply_smart_chunk(void)
//{
//
// int i = get_1b(); // 1
// int incarn = get_1b();
// int b = smart_bullet [i];
// int x = get_3b();
// int y = get_3b();
// int xs = get_s2b();
// int ys = get_s2b();
// unsigned char bangle = get_1b();
// int status = get_1b();
// unsigned char control_byte = get_1b();
// 
// if (b == -1 || bullet[b].bullet_type == 0
//     || smart_incarn [i] != incarn) // || it's not a smart bullet
// {
//  return;
// }
//
// if (serial[0].game_type == SERIAL_CLIENT
//     && (bullet[b].bullet_type == BULLET_REMOTE_ROCKET
//         || bullet[b].bullet_type == BULLET_REMOTE_ROCKET_C)
//     && user[actor[bullet[b].owner].user].status == USTAT_LOCAL)
//      return;
//
// if (x > 0 && x < arena[0].max_x * GRAIN)
//  bullet[b].x = x;
// if (y > 0 && y < arena[0].max_y * GRAIN)
//  bullet[b].y = y;
//  
// bullet[b].x_speed = xs;
// bullet[b].y_speed = ys;
//  
// bullet[b].base_angle = bangle;
// bullet[b].angle = ((float) bullet[b].base_angle / 128 * PI) - (PI / 2);
//
// bullet[b].status = status;
//
// if (control_byte % 2 == 1)
// {
//  bullet[b].facing = 1;
// }
//  else
//   bullet[b].facing = -1;
//}
//
//void apply_smart_init_chunk(void)
//{
//
// int i = get_1b();
// int owner = get_1b();
// int incarn = get_1b();
// int type = get_1b();
// int x = get_3b();
// int y = get_3b();
// int xs = get_s2b();
// int ys = get_s2b();
// unsigned char angle = get_1b();
// unsigned char status = get_1b();
//
// if (status == 100)
//  status = -1;
// 
// if (i < 0 || i > NO_SMART)
//  return; // oops
// if (x < 0 || x > arena[0].max_x * GRAIN)
//  return;
// if (y < 0 || y > arena[0].max_y * GRAIN)
//  return;
//
//// need to use the status somehow
// smart_incarn [i] = incarn;
//
// init_smart_bullet(type, x, y, xs, ys, angle, i, status, owner);
//
//}
//
//
//void apply_smart_destroy_chunk(void)
//{
//
// int i = get_1b();
// int incarn = get_1b();
// if (i < 0 || i > NO_SMART || smart_bullet [i] == -1
//     || bullet[smart_bullet [i]].bullet_type == 0 || smart_incarn [i] != incarn)
// {
////  get_1b();
//  get_3b();
//  get_3b();
////  ASSERT(FALSE);
//  return; // oops
// }
//
// int x = get_3b();
// int y = get_3b();
//
// if (x < 0 || x > arena[0].max_x * GRAIN) return;
// if (y < 0 || y > arena[0].max_y * GRAIN) return;
//// if (smart_incarn [i] != incarn) return;
//
// bullet[smart_bullet [i]].x = x;
// bullet[smart_bullet [i]].y = y;
// bullet[smart_bullet [i]].fuse = 1;
//
// smart_bullet [i] = -1;
//// ASSERT(FALSE);
//
// if (serial[0].game_type == SERIAL_SERVER)
// {
////if (client_no == -1) exit(67);
//  int j;
//  for (j = 0; j < NO_CLIENTS; j ++)
//  {
//   if (client[j].active == 1)
//    copy_to_server_buffer(inchunk, inchunk_pos - CHUNK_SMART_DESTROY_SIZE - 1, j);
//  }
// }
//
// 
//
//}
//
//
///*
//Client function. Responds to a smart_init chunk or an actor chunk launching
//a smart bullet by creating a bullet and assigning a smart index to it.
//*/
//void init_smart_bullet(int weapon_type, int x, int y, int xs, int ys, int base_angle, int smart_index, int lock, int owner)
//{
//
// float angle = ((float) base_angle / 128 * PI) - (PI / 2);
//
// int success = discharge_weapon(weapon_type, x, y, xs, ys, owner, 0, angle, 0, 1, lock);
//
// smart_bullet [smart_index] = success;
//
//}
//
//
//
///*
//
//
//Basic functions:
//
//From here are the functions that deal more directly with the chunk
// arrays.
//
//*/
//
//
///*
//Checks to see if there're any chunks waiting to be read.
//*/
//char check_chunk(NET_CHANNEL *chunk_channel)
//{
// return anything_received(chunk_channel);
//}
//
//
//
//void append_1b(unsigned char app)
//{
// chunk [chunk_pos] = app;
// chunk_pos ++;
// 
// chunk [chunk_pos] = CHUNK_NONE;
// 
//}
//
//void append_2b(int app)
//{
// chunk [chunk_pos] = app % 256;
// chunk [chunk_pos + 1] = app / 256;
// chunk_pos += 2;
//
//  chunk [chunk_pos] = CHUNK_NONE;
//
//}
//
//void append_s2b(int app)
//{
// char neg = 0;
//
// if (app < 0)
// {
//  neg = 1;
//  app *= -1;
// }
//
// chunk [chunk_pos] = app % 256;
// chunk [chunk_pos + 1] = app / 256;
// if (neg)
//  chunk [chunk_pos + 1] |= 128;
// chunk_pos += 2;
//
//  chunk [chunk_pos] = CHUNK_NONE;
//
//}
//
///*
//Assumes that an int is at least 32 bits
//*/
//void append_3b(int app)
//{
// chunk [chunk_pos] = app % 256;
// chunk [chunk_pos + 1] = (app % 65536) / 256;
// chunk [chunk_pos + 2] = (app % 16777216) / 65536;
// 
// chunk_pos += 3;
// 
// chunk [chunk_pos] = CHUNK_NONE;
// 
//}
//
//
//unsigned char get_1b(void)
//{
// int app = inchunk [inchunk_pos];
// inchunk_pos ++;
//
//// inchunk [inchunk_pos + 1] = CHUNK_NONE;
//
// return app;
//}
//
//int get_2b(void)
//{
// int app = inchunk [inchunk_pos];
// app += inchunk [inchunk_pos + 1] * 256;
// 
// inchunk_pos += 2;
//
//// inchunk [inchunk_pos + 1] = CHUNK_NONE;
//
// return app;
//}
//
//int get_s2b(void)
//{
// char neg = 0;
//
// int app = inchunk [inchunk_pos];
// int app2 = inchunk [inchunk_pos + 1];
//
// if (app2 & 128)
// {
//  app2 &= 127;
//  neg = 1;
// }
//
// app += app2 * 256;
//
// if (neg) app *= -1;
// 
// inchunk_pos += 2;
//
//// inchunk [inchunk_pos + 1] = CHUNK_NONE;
//
// return app;
//}
//
//int get_3b(void)
//{
// int app = inchunk [inchunk_pos];
// app += inchunk [inchunk_pos + 1] * 256;
// app += inchunk [inchunk_pos + 2] * 65535;
// 
// inchunk_pos += 3;
//
//// inchunk [inchunk_pos + 1] = CHUNK_NONE;
// 
// return app;
//}
//
//
///*
//Currently only usable to dump client/server buffers into chunk
//
//void dump_buffer(char *to_buffer, char *from_buffer)
//{
//
// int buf_pos = 0;
//
// while(from_buffer [buf_pos] != CHUNK_NONE)
// {
//  switch(from_buffer [buf_pos])
//  {
//   case CHUNK_ACTOR:
//   if (time_elapsed(from_buffer [buf_pos + 3], arena[0].counter) > 10)
//   {
//    clear_buffer_space(from_buffer, buf_pos, CHUNK_ACTOR_SIZE + 1);
//   }
//    else
//    {
//     if (from_buffer [buf_pos + 2] != WPN_NONE)
//      buffer_copy(from_buffer, buf_pos, to_buffer, -1);
//     buf_pos += CHUNK_ACTOR_SIZE + 1;
//    }
//   break;
//
//   case CHUNK_DIRT:
//   if (time_elapsed(from_buffer [buf_pos + 1], arena[0].counter) > 10)
//   {
//    clear_buffer_space(from_buffer, buf_pos, CHUNK_DIRT_SIZE + 1);
//   }
//    else
//    {
//     buffer_copy(from_buffer, buf_pos, to_buffer, -1);
//     buf_pos += CHUNK_DIRT_SIZE + 1;
//    }
//   break;
//
////   default: inchunk_broken = 1; break;
//
//  }
//   ASSERT(buf_pos < 900);
// }
//
//
//}
//*/
//
//void dump_client_buffer(void)
//{
//
// int i = 0, length, j;
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  if (client_buffer [i] [0] == CHUNK_NONE
//   || client_buffer [i] [0] == CHUNK_END_TRANSMISSION)
//   continue;
//  switch(client_buffer [i] [0])
//  {
//   case CHUNK_END_GAME:
//   break;
//   case CHUNK_REMOVE_USER:
//   break;
//   case CHUNK_ACTOR:
///*   if (time_elapsed(client_buffer [i] [3], async_tick) > 100)
//    {
//     client_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }*/
////   if (client_buffer [i] [3] % 2 != async_tick % 2) continue;
//   break;
//   case CHUNK_HURT:
///*   if (time_elapsed(client_buffer [i] [5], async_tick) > 100)
//    {
//     client_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }*/
//   if (client_buffer [i] [3] % 2 != async_tick % 2) continue;
//   break;
//   case CHUNK_SPAWN:
///*   if (time_elapsed(client_buffer [i] [3], async_tick) > 100)
//    {
//     client_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }*/
////   if (client_buffer [i] [3] % 5 != async_tick % 5) continue;
//   break;
//   case CHUNK_PICKUP_TAKEN:
//   break;
///*   case CHUNK_KILL:
//   if (time_elapsed(client_buffer [i] [3], async_tick) > 30)
//    {
//     client_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }
//   if (client_buffer [i] [3] % 5 != async_tick % 5) continue;
//   break;*/
//   case CHUNK_DIRT:
///*   if (time_elapsed(client_buffer [i] [3], async_tick) > 30)
//    {
//     client_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }*/
//   if (client_buffer [i] [1] % 5 != async_tick % 5) continue;
//   break;
//   case CHUNK_SMART_DESTROY:
//   break;
//   default: continue;
//  }
//   
//  length = shift_forward(client_buffer [i] [0]) + 1;
//  for (j = 0; j < length; j ++)
//  {
//   append_1b(client_buffer [i] [j]);
//  }
//  if (chunk_pos > BUFFER_SAFE_LIMIT)
//  {
//   append_end_chunk();
//   break;
//  }
//
// }
//
//}
//
//void dump_server_buffer(int which_client)
//{
//
// int i = 0, length, j;
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  if (client[which_client].server_buffer [i] [0] == CHUNK_NONE
//   || client[which_client].server_buffer [i] [0] == CHUNK_END_TRANSMISSION)
//   continue;
//  switch(client[which_client].server_buffer [i] [0])
//  {
//   case CHUNK_END_GAME:
//   break;
//   case CHUNK_REMOVE_USER:
//   break;
//   case CHUNK_ACTOR:
///*   if (time_elapsed(client[which_client].server_buffer [i] [3], async_tick) > 100)
//    {
//     client[which_client].server_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }*/
//   // Don't bother sending client details of its own actors:
//   if (client[which_client].player_actor [0] == client[which_client].server_buffer [i] [1]
//    || (client[which_client].player_active [1]
//        && client[which_client].player_actor [1] == client[which_client].server_buffer [i] [1]))
//   {
//     client[which_client].server_buffer [i] [0] = CHUNK_NONE;
//     continue;
//   }
//     // Actually this might not happen anyway, as this may be caught before
//     //  the actor chunk is even copied into this client's server buffer.
//     //  But let's be safe.
////   if (client[which_client].server_buffer [i] [3] % 2 != async_tick % 2) continue;
//   break;
//   case CHUNK_SCORE:
//   break;
//   case CHUNK_PICKUP:
//   break;
//   case CHUNK_PICKUP_TAKEN:
//   break;
//   case CHUNK_SMART:
////   if (smart_owner [] == client[which_client].player_user [0]
////       || smart_owner [] == client[which_client].player_user [1])
////      continue;
//   break;
//   case CHUNK_SMART_INIT:
//   break;
//   case CHUNK_SMART_DESTROY:
//   break;
//   case CHUNK_KILL:
///*   if (time_elapsed(client[which_client].server_buffer [i] [3], async_tick) > 30)
//    {
//     client[which_client].server_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }*/
////   if (client[which_client].server_buffer [i] [3] % 5 != async_tick % 5) continue;
//   break;
///*   case CHUNK_DIRT:
//   if (time_elapsed(client_buffer [i] [3], async_tick) > 30)
//    {
//     client_buffer [i] [0] = CHUNK_NONE;
//     continue;
//    }
//   if (client[which_client].server_buffer [i] [1] % 5 != async_tick % 5) continue;
//   break;*/
//   default: continue;
//  }
//
//  length = shift_forward(client[which_client].server_buffer [i] [0]) + 1;
//  for (j = 0; j < length; j ++)
//  {
//   append_1b(client[which_client].server_buffer [i] [j]);
//  }
//  if (chunk_pos > BUFFER_SAFE_LIMIT)
//  {
//   append_end_chunk();
//   break;
//  }
// }
//
//}
//
//
//
//
///*
//Copies a chunk starting at fbloc in from_buffer to tbloc in to_buffer.
// If tbloc is -1, it finds its own empty space in to_buffer.
//*/
//int buffer_copy(const char *from_buffer, int fbloc, char *to_buffer, int tbloc)
//{
//
////buffer_copy(chunk, chunk_pos - CHUNK_ACTOR_SIZE - 1, client[0].server_buffer, -1) == -1)
////buffer_copy(chunk, chunk_pos - CHUNK_ACTOR_SIZE, client_buffer, -1);
//
// int i;
// int chlength = shift_forward(from_buffer [fbloc]);
//// int chlength = CHUNK_ACTOR_SIZE;
//
// if (tbloc == -1)
// {
//  tbloc = find_buffer_space(to_buffer, chlength);
////  tbloc = 0;
// }
//
// if (tbloc == -1) return -1;
//
// for (i = 0; i < chlength; i ++)
// {
//  to_buffer [i + tbloc] = from_buffer [i + fbloc];
// }
//#ifdef ASYNC_DEBUG
// text_mode(0);
// textprintf(screen, large_font, 10, 165, async_tick, "chl %i fr %i to %i  ", chlength, fbloc, tbloc);
//#endif
// return 1;
//
//}
//
//
//void copy_to_server_buffer(const char *from_buffer, int fbloc, int client_no)
//{
// int tbloc = find_empty_server_buffer(client_no);
// if (tbloc == -1) return;
//
// int chlength = shift_forward(from_buffer [fbloc]) + 1;
// int i;
// 
// for (i = 0; i < chlength; i ++)
// {
//  client[client_no].server_buffer [tbloc] [i] = from_buffer [i + fbloc];
// }
//// text_mode(0);
//// textprintf(screen, large_font, 10, 165, arena[0].counter, "chl %i fr %i to %i  ", chlength, fbloc, tbloc);
//
//}
//
//
//void copy_to_client_buffer(const char *from_buffer, int fbloc)
//{
// int tbloc = find_empty_client_buffer();
// if (tbloc == -1) return;
//
// int chlength = shift_forward(from_buffer [fbloc]) + 1;
// int i;
// 
// for (i = 0; i < chlength; i ++)
// {
//  client_buffer [tbloc] [i] = from_buffer [i + fbloc];
// }
//// text_mode(0);
//// textprintf(screen, large_font, 10, 165, arena[0].counter, "chl %i fr %i to %i  ", chlength, fbloc, tbloc);
//
//}
//
//int find_empty_client_buffer(void)
//{
// int i;
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  if (client_buffer [i] [0] == CHUNK_NONE)
//   return i;
// }
//
// return -1;
//
//}
//
//int find_empty_server_buffer(int which_client)
//{
// int i;
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  if (client[which_client].server_buffer [i] [0] == CHUNK_NONE)
//   return i;
// }
//
// return -1;
//
//}
//
//
//
///*
//Finds a space of spaces size in buffer array and returns
// the index of its first element
//*/
//int find_buffer_space(const char *buffer, int spaces)
//{
//
// int i = 0, j = 0;
//
// for (i = 0; i < ASYNC_BUFFER; i ++)
// {
//  if (buffer [i] == CHUNK_NONE)
//  {
//   j ++;
//  }
//    else
//     {
//      j = 0;
//      i += shift_forward(buffer [i]) + 1;
//     }
//  if (j >= spaces) return i - spaces + 1;
//// WAS spaces; (no + 1)
//
// }
//
// return -1;
// 
//}
//
//
//int shift_forward(int chunk_type)
//{
//
// switch(chunk_type)
// {
//  case ACK_CHUNK_ACTOR:
//  case CHUNK_ACTOR: return CHUNK_ACTOR_SIZE;
//  case ACK_CHUNK_SPAWN:
//  case CHUNK_SPAWN: return CHUNK_SPAWN_SIZE;
//  case ACK_CHUNK_KILL:
//  case CHUNK_KILL: return CHUNK_KILL_SIZE;
//  case ACK_CHUNK_HURT:
//  case CHUNK_HURT: return CHUNK_HURT_SIZE;
//  case ACK_CHUNK_DIRT:
//  case CHUNK_DIRT: return CHUNK_DIRT_SIZE;
//  case ACK_CHUNK_PICKUP:
//  case CHUNK_PICKUP: return CHUNK_PICKUP_SIZE;
//  case ACK_CHUNK_SCORE:
//  case CHUNK_SCORE: return CHUNK_SCORE_SIZE;
//  case ACK_CHUNK_SMART_INIT:
//  case CHUNK_SMART_INIT: return CHUNK_SMART_INIT_SIZE;
//  case ACK_CHUNK_SMART_DESTROY:
//  case CHUNK_SMART_DESTROY: return CHUNK_SMART_DESTROY_SIZE;
//  case ACK_CHUNK_PICKUP_TAKEN:
//  case CHUNK_PICKUP_TAKEN: return CHUNK_PICKUP_TAKEN_SIZE;
//  case CHUNK_STATUS: return CHUNK_STATUS_SIZE;
//  case CHUNK_GAMESTATE: return var_chunk_gamestate_size;
//  case CHUNK_SMART: return CHUNK_SMART_SIZE;
//  case ACK_CHUNK_END_GAME:
//  case CHUNK_END_GAME: return CHUNK_END_GAME_SIZE;
//  case ACK_CHUNK_REMOVE_USER:
//  case CHUNK_REMOVE_USER: return CHUNK_REMOVE_USER_SIZE;
//  case CHUNK_END_TRANSMISSION: return 0;
//
// }
//
// return 0;
//
//}
//
//int associated_chunk(int chunk_type)
//{
// switch(chunk_type)
// {
//  case ACK_CHUNK_ACTOR: return CHUNK_ACTOR;
//  case CHUNK_ACTOR: return ACK_CHUNK_ACTOR;
//  case ACK_CHUNK_SPAWN: return CHUNK_SPAWN;
//  case CHUNK_SPAWN: return ACK_CHUNK_SPAWN;
//  case ACK_CHUNK_KILL: return CHUNK_KILL;
//  case CHUNK_KILL: return ACK_CHUNK_KILL;
//  case ACK_CHUNK_HURT: return CHUNK_HURT;
//  case CHUNK_HURT: return ACK_CHUNK_HURT;
//  case ACK_CHUNK_PICKUP: return CHUNK_PICKUP;
//  case CHUNK_PICKUP: return ACK_CHUNK_PICKUP;
//  case ACK_CHUNK_SCORE: return CHUNK_SCORE;
//  case CHUNK_SCORE: return ACK_CHUNK_SCORE;
//  case ACK_CHUNK_PICKUP_TAKEN: return CHUNK_PICKUP_TAKEN;
//  case CHUNK_PICKUP_TAKEN: return ACK_CHUNK_PICKUP_TAKEN;
//  case CHUNK_SMART_INIT: return ACK_CHUNK_SMART_INIT;
//  case ACK_CHUNK_SMART_INIT: return CHUNK_SMART_INIT;
//  case CHUNK_SMART_DESTROY: return ACK_CHUNK_SMART_DESTROY;
//  case ACK_CHUNK_SMART_DESTROY: return CHUNK_SMART_DESTROY;
//  case CHUNK_END_GAME: return ACK_CHUNK_END_GAME;
//  case ACK_CHUNK_END_GAME: return CHUNK_END_GAME;
//  case CHUNK_REMOVE_USER: return ACK_CHUNK_REMOVE_USER;
//  case ACK_CHUNK_REMOVE_USER: return CHUNK_REMOVE_USER;
//  case CHUNK_END_TRANSMISSION: return 0;
// }
//
// return CHUNK_NONE;
//
//}
//
//
///*
//int find_client_buffer_match(int ch_pos)
//{
// int i, j;
// char matched = 0;
//
//// unsigned char chmatch [30];
//
// int chlength = shift_forward(inchunk [ch_pos]) + 1;
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  if (client_buffer [i] [0] != inchunk [ch_pos]) continue;
//  matched = 1;
//  for (j = 0; j < chlength; j ++)
//  // should it be chlength + 1? probably won't matter
//  {
//   if (client_buffer [i] [j] != inchunk [ch_pos + j])
//    matched = 0;
//  }
//  if (matched == 1) return i;
// }
//
// return -1;
//
//}*/
//
//int find_client_buffer_match(int ch_pos)
//{
// int i, j;
// char matched = 0;
//
//// unsigned char chmatch [30];
//
// int chlength = shift_forward(inchunk [ch_pos]) + 1;
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  if (client_buffer [i] [0] != associated_chunk(inchunk [ch_pos])) continue;
//  matched = 1;
//  for (j = 1; j < chlength; j ++)
//  // should it be chlength + 1? probably won't matter
//  {
//   if (client_buffer [i] [j] != inchunk [ch_pos + j])
//    matched = 0;
//  }
//  if (matched == 1) return i;
// }
//
// return -1;
//
//}
//
//int find_server_buffer_match(int which_client, int ch_pos)
//{
// int i, j;
// char matched = 0;
//
//// unsigned char chmatch [30];
//
// int chlength = shift_forward(inchunk [ch_pos]);
//
// for (i = 0; i < ASYNC_BUFFER_NO; i ++)
// {
//  if (client[which_client].server_buffer [i] [0] != associated_chunk(inchunk [ch_pos])) continue;
//  text_mode(0);
////  textprintf(screen, large_font, 280, 180, async_tick, "M %i  ", i);
//  matched = 1;
//  for (j = 1; j < chlength; j ++)
//  // should it be chlength + 1? probably won't matter
//  {
//   if (client[which_client].server_buffer [i] [j] != inchunk [ch_pos + j])
//    matched = 0;
//  }
//  if (matched == 1) return i;
// }
//
// return -1;
//
//}
//
//
//
//
//
///*
//
//BROKEN - note use of chunk_pos
//
//void cl_append_1b(int cl, unsigned char app)
//{
// client_buffer [chunk_pos] = app;
// chunk_pos ++;
// 
// client_buffer [chunk_pos] = CHUNK_NONE;
// 
// ASSERT(chunk_pos < 900);
//}
//
//void cl_append_2b(int cl, int app)
//{
// client_buffer [chunk_pos] = app % 256;
// client_buffer [chunk_pos + 1] = app / 256;
// chunk_pos += 2;
//
//  client_buffer [chunk_pos] = CHUNK_NONE;
//
// ASSERT(chunk_pos < 900);
//}
//
//void cl_append_3b(int cl, int app)
//{
// client_buffer [chunk_pos] = app % 256;
// client_buffer [chunk_pos + 1] = (app % 65536) / 256;
// client_buffer [chunk_pos + 2] = (app % 16777216) / 65536;
// 
// chunk_pos += 3;
// 
// client_buffer [chunk_pos] = CHUNK_NONE;
//
// ASSERT(chunk_pos < 900);
//}
//*/
///*
//unsigned char cl_get_1b(int cl)
//{
// int app = inchunk [inchunk_pos];
// inchunk_pos ++;
//
//// inchunk [inchunk_pos + 1] = CHUNK_NONE;
//
// return app;
//}
//
//int cl_get_2b(int cl)
//{
// int app = inchunk [inchunk_pos];
// app += inchunk [inchunk_pos + 1] * 256;
// 
// inchunk_pos += 2;
//
//// inchunk [inchunk_pos + 1] = CHUNK_NONE;
//
// return app;
//}
//
//int cl_get_3b(int cl)
//{
// int app = inchunk [inchunk_pos];
// app += inchunk [inchunk_pos + 1] * 256;
// app += inchunk [inchunk_pos + 2] * 65535;
// 
// inchunk_pos += 3;
//
//// inchunk [inchunk_pos + 1] = CHUNK_NONE;
// 
// return app;
//}
//*/
//
//
//
//
//
//
//
//
//void copy_to_server_register(const char *from_buffer, int fbloc, int client_no)
//{
// int tbloc = find_empty_server_register(client_no);
// if (tbloc == -1) exit(4); //return;
//
// int chlength = shift_forward(from_buffer [fbloc]) + 1;
// int i;
// 
// for (i = 0; i < chlength; i ++)
// {
//  client[client_no].server_register [tbloc] [i] = from_buffer [i + fbloc];
// }
//
// client[client_no].server_register [tbloc] [i] = async_tick;
// 
//// text_mode(0);
//// textprintf(screen, large_font, 10, 165, arena[0].counter, "chl %i fr %i to %i  ", chlength, fbloc, tbloc);
//
//}
//
//void copy_to_client_register(const char *from_buffer, int fbloc)
//{
// int tbloc = find_empty_client_register();
// if (tbloc == -1) exit(5); //return;
//
// int chlength = shift_forward(from_buffer [fbloc]) + 1;
// int i;
// 
// for (i = 0; i < chlength; i ++)
// {
//  client_register [tbloc] [i] = from_buffer [i + fbloc];
// }
// 
// client_register [tbloc] [i] = async_tick;
// 
//// text_mode(0);
//// textprintf(screen, large_font, 10, 165, arena[0].counter, "chl %i fr %i to %i  ", chlength, fbloc, tbloc);
//
//}
//
//int find_empty_client_register(void)
//{
// int i;
//
// for (i = 0; i < ASYNC_REGISTER_NO; i ++)
// {
//  if (client_register [i] [0] == CHUNK_NONE)
//   return i;
// }
//
// return -1;
//
//}
//
//int find_empty_server_register(int which_client)
//{
// int i;
//
// for (i = 0; i < ASYNC_REGISTER_NO; i ++)
// {
//  if (client[which_client].server_register [i] [0] == CHUNK_NONE)
//  {
////   textprintf(screen, large_font, 280, 180, async_tick, "Found %i  ", i);
//   return i;
//  }
// }
//
// return -1;
//
//}
//
//
//int find_client_register_match(int ch_pos)
//{
// int i, j;
// char matched = 0;
//
//// unsigned char chmatch [30];
//
// int chlength = shift_forward(inchunk [ch_pos]);
//
// for (i = 0; i < ASYNC_REGISTER_NO; i ++)
// {
//   switch(client_register [i] [0])
//   {
//    case CHUNK_ACTOR:
//    if (time_elapsed(client_register [i] [CHUNK_ACTOR_SIZE + 1], async_tick) > 100)
//    {
//     client_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     continue;
//    }
//    break;
//    case CHUNK_KILL:
//    if (time_elapsed(client_register [i] [CHUNK_KILL_SIZE + 1], async_tick) > 100)
//    {
//     client_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     continue;
//    }
//    break;
//    case CHUNK_PICKUP:
//    if (time_elapsed(client_register [i] [CHUNK_PICKUP_SIZE + 1], async_tick) > 100)
//    {
//     client_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     continue;
//    }
//    break;
//    case CHUNK_SCORE:
//    if (time_elapsed(client_register [i] [CHUNK_SCORE_SIZE + 1], async_tick) > 100)
//    {
//     client_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     continue;
//    }
//    break;
//    case CHUNK_PICKUP_TAKEN:
//    if (time_elapsed(client_register [i] [CHUNK_PICKUP_TAKEN_SIZE + 1], async_tick) > 100)
//    {
//     client_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     continue;
//    }
//    break;
//    case CHUNK_SMART_INIT:
//    if (time_elapsed(client_register [i] [CHUNK_SMART_INIT_SIZE + 1], async_tick) > 100)
//    {
//     client_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     continue;
//    }
//    break;
//    case CHUNK_SMART_DESTROY:
//    if (time_elapsed(client_register [i] [CHUNK_SMART_DESTROY_SIZE + 1], async_tick) > 100)
//    {
//     client_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     continue;
//    }
//    break;
//    case CHUNK_DIRT:
//    if (FALSE) //time_elapsed(client_register [i] [CHUNK_DIRT_SIZE + 1], async_tick) > 30)
//    {
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i  ", i);
//     client_register [i] [0] = CHUNK_NONE;
//     continue;
//    }
//    break;
//   }
//  if (client_register [i] [0] != inchunk [ch_pos]) continue;
//  matched = 1;
//  for (j = 0; j < chlength; j ++)
//  // should it be chlength + 1? probably won't matter
//  {
//   if (client_register [i] [j] != inchunk [ch_pos + j])
//    matched = 0;
//  }
//  if (matched == 1)
//  {
//#ifdef ASYNC_DEBUG
//   textprintf(screen, large_font, 280, 180, async_tick, "Found %i %i ", i, client_register [i] [0]);
//#endif
//   return i;
//  }
// }
//
// return -1;
//
//}
//
//int find_server_register_match(int which_client, int ch_pos)
//{
// int i, j;
// char matched = 0;
//
//// unsigned char chmatch [30];
//text_mode(0);
//
// int chlength = shift_forward(inchunk [ch_pos]);
//
// for (i = 0; i < ASYNC_REGISTER_NO; i ++)
// {
//   switch(client[which_client].server_register [i] [0])
//   {
//    case CHUNK_ACTOR:
////    if (time_elapsed(client[which_client].server_register [i] [3], async_tick) > 50)
//    if (time_elapsed(client[which_client].server_register [i] [CHUNK_ACTOR_SIZE + 1], async_tick) > 100)
//    {
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i %i ", i, client[which_client].server_register [i] [0]);
//     client[which_client].server_register [i] [0] = CHUNK_NONE;
//     continue;
//    }
//    break;
//    case CHUNK_HURT:
//    if (time_elapsed(client[which_client].server_register [i] [CHUNK_HURT_SIZE + 1], async_tick) > 100)
//    {
//     client[which_client].server_register [i] [0] = CHUNK_NONE;
//     continue;
//    }
//    break;
//    case CHUNK_SPAWN:
//    if (time_elapsed(client[which_client].server_register [i] [CHUNK_SPAWN_SIZE + 1], async_tick) > 100)
//    {
//     client[which_client].server_register [i] [0] = CHUNK_NONE;
//     continue;
//    }
//    break;
//    case CHUNK_SMART_DESTROY:
//    if (time_elapsed(client[which_client].server_register [i] [CHUNK_SMART_DESTROY_SIZE + 1], async_tick) > 100)
//    {
//     client[which_client].server_register [i] [0] = CHUNK_NONE;
//     continue;
//    }
//    break;
//    case CHUNK_DIRT:
////    if (time_elapsed(client[which_client].server_register [i] [1], async_tick) > 20)
//    if (FALSE) //time_elapsed(client[which_client].server_register [i] [CHUNK_DIRT_SIZE + 1], async_tick) > 50)
//    {
//     client[which_client].server_register [i] [0] = CHUNK_NONE;
////     textprintf(screen, large_font, 280, 180, async_tick, "Empty %i %i ", i, client[which_client].server_register [i] [0]);
//     continue;
//    }
//    break;
//   }
////  if (client[which_client].server_buffer [i] [0] != inchunk [ch_pos]) continue;
////  text_mode(0);
////  textprintf(screen, large_font, 280, 180, async_tick, "M %i  ", i);
//  matched = 1;
//  for (j = 1; j < chlength; j ++)
//  // should it be chlength + 1? probably won't matter
//  {
//   if (client[which_client].server_register [i] [j] != inchunk [ch_pos + j])
//    matched = 0;
//  }
//  if (matched == 1)
//  {
//#ifdef ASYNC_DEBUG
//   textprintf(screen, large_font, 280, 180, async_tick, "Found %i %i ", i, client_register [i] [0]);
//#endif
//   return i;
//  }
// }
//
// return -1;
//
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
