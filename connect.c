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

File: connect.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - all the interfacing with Libnet functions.
 - most of the multi-computer game code is in async.c. The exception in
   the serial_game_init function (passes game config information from
   server to client) which should be in async.c but is here for historical
   reasons.
 - most of the functions in this file are only called from async.c.
   The exceptions are some initialisation functions, which can be called
   from proj.c (IIRC) and menu.c.

 - Note: the term 'serial' is used frequently to refer to any network
   type - in variable and function names, for example. This is because
   originally I was using DZCOMM, a serial-only communications library,
   before I switched to Libnet for the variety of drivers.


*/

#include <stdio.h>

#include "allegro.h"

#include <string.h>

#include "libnet.h"

#include "config.h"

#include "globvar.h"
#include "prand.h"
#include "cmds.h"
#include "connect.h"
#include "misc.h"
#include "async.h"

//#define CONNECT_DEBUG

/*
Old dzcomm file in con_dzc.c
*/
//void sendval(unsigned char val);
//void send_int(int send);
//int receive_int(void);
//int get_remote_cmds(int user_controlled, int server_state, int to_which);
void bits_to_user_cmds(unsigned char cmd_buf [4], int user_cmds);
void bits_to_server_cmds(unsigned char cmd_buf [4], int user_cmds, int server_state);
void user_cmds_to_bits(unsigned char cmd_buf [4], int user_cmds);
void server_cmds_to_bits(unsigned char cmd_buf [4], int user_cmds, int server_state);
void shutdown_network(void);

//comm_port *cport;
/*comm_port *port2;
comm_port *cur_port;*/

//inline void dz_print_comm_err() {printf("%s\n",szDZCommErr);fflush(stdout);}


int send_counter;
int rec_counter;

extern int user_cmd_buffer [NO_USERS] [NO_CMDS];

#define SERVER_CMD_BUFFER_SIZE 5
#define MAX_BUFFER_SIZE 1000

int server_cmd_buffer [SERVER_CMD_BUFFER_SIZE] [NO_USERS] [NO_CMDS];


char get_buffer_conn(void);
char send_buffer_conn(int buffer_size);

int get_buffer_channel(NET_CHANNEL *buf_channel);
char send_buffer_channel(NET_CHANNEL *send_channel, int buffer_size);

int add_byte_to_send_buffer(unsigned char add, int position);
unsigned char get_byte_from_rec_buffer(int position);

int add_int_to_send_buffer(int add, int position);
int get_int_from_rec_buffer(int position);



unsigned char receive_buffer [MAX_BUFFER_SIZE];
unsigned char send_buffer [MAX_BUFFER_SIZE];

extern char last_address_received [10];
extern int local_client_no;


// also defined in async.c
#define NO_CLIENTS 4
#define ASYNC_BUFFER_NO 300
#define ASYNC_BUFFER_SIZE 20
#define ASYNC_REGISTER_SIZE 20
#define ASYNC_REGISTER_NO 300

struct client_struct
{
 unsigned char server_buffer [ASYNC_BUFFER_NO] [ASYNC_BUFFER_SIZE];
 char server_received [256];
 int sb_pos;
 unsigned char server_register [ASYNC_REGISTER_NO] [ASYNC_REGISTER_SIZE];
 int active;
 int no_players;

 char address [50];
 NET_CHANNEL *client_channel;

 // to mimic player_struct
 char player_active [2];
 int player_actor [2];
 int player_user [2];
 int player_colour_r [2];
 int player_colour_g [2];
 int player_colour_b [2];
 int player_handicap [2];
 int player_soldier [2];
 int player_team [2];
 char pname [2] [20];

};

extern struct client_struct client [NO_CLIENTS];




 NET_DRIVERNAME *drivers;
  NET_CONN *listen_conn;
  NET_CHANNEL *comm_channel;
//  NET_CONN *comm_conn;
//  char buffer[1000];
  char target[1024];
  char *chptr;
  int not_ok, x, choice;
  NET_DRIVERLIST avail;

 char address[50];



void shutdown_network(void)
{
    net_shutdown();
    serial[0].initialised = 0;
    serial[0].connections = 0;


}


/*
From libnet's gentest.c
*/
void check_config() {
  FILE *fp;
  char buffer[1000];
  
  fp = fopen ("libnet.cfg", "rt");
  if (!fp) {
    information_box("Can't load libnet.cfg!", " ", " ", 1);
    clear_box();
    return;
  }
  
  fgets (buffer, 1000, fp);
  if (!strcmp (buffer, "# CHANGE ME!\n")) {
    fprintf (stderr, "You haven't edited the config file.\nPlease do so (instructions within).\n");
    exit (4);
  }
  fclose (fp);
}



void init_connect(void)
{
//   dzcomm_init();
 net_init();
 check_config();
 net_loadconfig (NULL);
 serial[0].connections = 0;
 serial[0].initialised = 0;

 int i;

 for (i = 0; i < MAX_BUFFER_SIZE; i ++)
 {
  send_buffer [i] = 0;
  receive_buffer [i] = 0;
 }
 
}


void choose_net_drivers(void)
{
 return;
}



/*
How IPX tries to work:
<This information was current for World of Violence, but as I don't think
it worked the old experimental IPX system, which tried to use the broadcast
channel to find the server, has been replaced by one where the clients
manually enter the server address.>

1 Server opens a listening channel on broadcast channel with default
  listening port.
2 Client contacts server on broadcast channel with default listening port.
3 Server hears this, closes listening channel, opens a new channel with
  next available port and sets its target as the address received from the
  client through net_receive.
4 Client meanwhile waits for server to contact it. When it receives this
  new communication it sets its comm_channel target as the address received
  through net_receive.

- Question: does the port number have to be the same on both computers?
  If so, this won't work - the client needs to set its channel to next
  available port (NULL) in step 4. But how will the server know what the
  client's next available port is? Will they both be the same? I have no
  idea how ports work.
*/
void server_get_connections(void)
{

// if (listen_chan == NULL) return;
 if (serial[0].initialised == 0) return;

 rest(200);
 // rest so that the keypress that started this process is gone
 
 char itstring [10];
 char info_string [100];
 char get_k = 0;
 char buffer [100];
 int x;
 int client_no;

 static NET_CHANNEL *listen_channel [8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
 static int listen_count = 0;

/*    if (serial[0].com_port == 1)
     strcpy(target, "0:1"); // was 0:1
      else
       strcpy(target, "1:1"); // was 1:1
*/

// if (listen_channel [listen_count] != NULL)
//  net_closechannel(listen_channel [listen_count]);

 if (listen_count >= 6)
 {
    information_box("Error: too many connections!", " ", " ", 1);
    clear_box();
    return;
 }



 if (serial[0].connection_type == NET_DRIVER_SERIAL_DOS)
 {

    if (serial[0].com_port == 1)
     strcpy(target, "0:1"); // was 0:1
      else
       strcpy(target, "1:1"); // was 1:1

 listen_channel [listen_count] = net_openchannel(NET_DRIVER_SERIAL_DOS, target);
// net_assigntarget (listen_channel [listen_count], target);

       

 }

 if (serial[0].connection_type == NET_DRIVER_IPX_DOS)
 {

/*
All this is just a test and has never been executed on a computer
capable of running it properly. Handle with care.
*/
  listen_channel [listen_count] = net_openchannel(NET_DRIVER_IPX_DOS, "");
//  net_assigntarget (listen_channel [listen_count], "FFFFFFFFFFFF");
/*
I have no idea if the above code is how you're supposed to do this.
*/
       

 }

/*

TO DO: Finish this off! Making sure at each step that it still works
in serial mode.

*/


/*  if (net_assigntarget (listen_channel [listen_count], target))
  {
    information_box("Error assigning target. Try another.", " ", " ", 1);
    return;
  }*/

//  information_box("No Connections", " ", " ", 0);
  strcpy(info_string, "Connections: ");
  itoa(serial[0].connections, itstring, 10);
  strcat(info_string, itstring);
  char info_string2 [50];
  strcpy(info_string2, "Local Address: ");
//  itoa(, itstring, 10);
  strcat(info_string2, net_getlocaladdress(listen_channel [listen_count]));
  information_box("Waiting for clients to connect...", info_string, info_string2, 0);

 do
 {
  x = net_receive (listen_channel [listen_count], buffer, 1, address);
  if (x == 0) continue;

// Here check to see if that address already in use, to prevent doubling

/*  if (serial[0].connection_type == NET_DRIVER_IPX_DOS)
  {
  // Here we have to change from using the default listening port to the
  //  next available port, so server can listen for more clients. I think.
   net_closechannel(listen_channel [listen_count]);
/ *
All this is just a test and has never been executed on a computer
capable of running it properly. Handle with care.
* /
   listen_channel [listen_count] = net_openchannel(NET_DRIVER_IPX_DOS, NULL);
//   net_assigntarget (listen_channel [listen_count], address);
/ *
I have no idea if the above code is how you're supposed to do this.
* /
       

  }
*/

  if (net_assigntarget (listen_channel [listen_count], address))
  {
    information_box("Error assigning target. Try another.", " ", " ", 1);
    return;
  }

  buffer [0] = 199;
//  net_send (client[client_no].client_channel, buffer, 1);
  net_send (listen_channel [listen_count], buffer, 1);


  client_no = async_new_client(address, listen_channel [listen_count]);
//  client_no = async_new_client(target, listen_channel [listen_count]);
   // among other things, this assigns a client_channel

  if (client_no == -1)
  {
   information_box("Error detecting client.", " ", " ", 1);
   continue;
  }

//  comm_channel = NULL;
/*
  if (!(comm_channel = net_openchannel (9, target)))
  {
  // was , NULL
    information_box("Error: Couldn't open network connection.", " ", " ", 1);
    return;
  }
  
  if (net_assigntarget (comm_channel, target))
  {
    information_box("Error assigning target. Try another.", " ", " ", 1);
    return;
  }
*/
//  net_closechannel(listen_chan);


//  comm_channel = net_openchannel(choice, address);
//  if (!comm_channel) continue;

//  comm_channel = net_openchannel(choice, address);
//  if (!comm_channel) continue;
  
//  net_assigntarget (comm_conn, target);
  serial[0].connections ++;
  
  strcpy(info_string, "Connections: ");
  itoa(serial[0].connections, itstring, 10);
  strcat(info_string, itstring);
  information_box(info_string, " ", " ", 0);
  listen_count ++;
/*
  if (!(listen_channel [listen_count] = net_openchannel (9, target))) {
  // was , NULL
    information_box("Error: Couldn't open network connection.", " ", " ", 1);
    return;
  }
  
  if (net_assigntarget (listen_channel [listen_count], target))
  {
    information_box("Error assigning target. Try another.", " ", " ", 1);
    return;
  }
*/
//  information_box("No Connections", " ", " ", 0);


  break;
 } while(!keypressed());

 get_k = readkey();

 clear_box();

}

/*
Not used, as I can't get connections to work.
*/
int conn_to_channel(void)
{
//  char buffer [100];

  return 1;

//  net_closeconn(comm_conn);

  comm_channel = net_openchannel(choice, address);
  if (!comm_channel)
  {
   information_box("Connection->Channel Failed.", " ", " ", 0);
   return -1;
  }
  
//  comm_conn = net_openconn(choice, address);
//  if (!comm_conn) continue;
  
  net_assigntarget (comm_channel, target);
//  serial[0].connections ++;
//  buffer [0] = 199;
//  net_send (comm_channel, buffer, 1);
  
//  strcpy(info_string, "Channel -> Connection Successful.");
//  itoa(serial[0].connections, itstring, 10);
//  strcat(info_string, itstring);
  information_box("Connection->Channel Successful.", " ", " ", 0);

  return 1;

}



int client_connect(void)
{

 information_box("Press a key to initiate connection.", "Make sure the server is waiting first, though.", "Press Escape to abort.", 1);

 char buffer [100];
// int success;

// net_closechannel(listen_chan);

 if (serial[0].connection_type == NET_DRIVER_SERIAL_DOS)
 {

    if (serial[0].com_port == 1)
     strcpy(target, "0:1"); // was 0:1
      else
       strcpy(target, "1:1"); // was 1:1

 comm_channel = net_openchannel(NET_DRIVER_SERIAL_DOS, NULL);
 net_assigntarget (comm_channel, target);

       

 }

 if (serial[0].connection_type == NET_DRIVER_IPX_DOS)
 {

/*
All this is just a test and has never been executed on a computer
capable of running it properly. Handle with care.
*/
  comm_channel = net_openchannel(NET_DRIVER_IPX_DOS, "");
//  net_assigntarget (comm_channel, "FFFFFFFFFFFF");
  net_assigntarget (comm_channel, serial[0].port_string);
/*
I have no idea if the above code is how you're supposed to do this.
*/
       

 }

//  success = net_connect_wait_time(comm_conn, target, 5);

//  while (!net_poll_connect(comm_conn))
//  {
//  }


 if (comm_channel == NULL)
 {
  information_box("Error opening comm_channel.", " ", " ", 1);
  clear_box();
  return -1;
 }

/* if (success > 0)
 {
  information_box("Time-out on connection.", " ", " ", 1);
  return -1;
 }

 if (success < 0)
 {
  information_box("Error on connection.", " ", " ", 1);
  return -1;
 }*/

 buffer [0] = 199;
 
 net_send (comm_channel, buffer, 1);

// rest(1500);
 
 int anything;

 int timeout = 0;

// I don't think this is necessary (or that it would work):
// if (serial[0].connection_type == NET_DRIVER_IPX_DOS)
// {
//  net_closechannel(comm_channel);
/*
All this is just a test and has never been executed on a computer
capable of running it properly. Handle with care.
*/
//  comm_channel = net_openchannel(NET_DRIVER_IPX_DOS, NULL);
//  net_assigntarget (comm_channel, "FFFFFFFFFFFF");
/*
I have no idea if the above code is how you're supposed to do this.
*/
       

// }


 do
 {
  if (key [KEY_ESC]) return -1;
  anything = net_receive(comm_channel, buffer, 1, address);
  rest(1);
  timeout ++;
 } while(anything <= 0 && timeout < 5000);

 if (anything == 0)
 {
  information_box("No Server Currently Available", " ", " ", 1);
  clear_box();
  return -1;
 }
 
 if (anything == -1)
 {
  information_box("Error reading from channel.", " ", " ", 1);
  clear_box();
  return -1;
 }

// net_closechannel(comm_channel);

// comm_conn = net_openconn(choice, address);

// if (comm_conn == NULL)
// {
//  information_box("Error Establishing Connection.", " ", " ", 1);
//  return -1;
// }

 if (serial[0].connection_type == NET_DRIVER_SERIAL_DOS)
  net_assigntarget(comm_channel, address);
// According to Libnet docs, address should contain the address of the
//  sending computer in the appropriate format, whether we're in serial
//  or IPX mode. But I'm not sure if this is needed in IPX mode, as the
//  target has been added manually.

 async_client_join();

// information_box("Connection Established.", " ", " ", 1);
// clear_box();

 return 1;


}


int init_serial_port(int which_port)
{

  
//  net_init();
  
//  printf ("Getting driver names...\n");
//  drivers = net_getdrivernames (net_drivers_all);
  
//  list_drivers (drivers, "All drivers");
//  free (drivers);

//  printf ("Detecting available drivers...\n");
  avail = net_detectdrivers (net_drivers_all);
//  printf ("Getting detected driver names...\n");
/*  drivers = net_getdrivernames (avail);
  
  do {
    list_drivers (drivers, "Available drivers");
    
    printf ("Please choose a driver from the above list.\n");
    fgets (buffer, 10, stdin);
    choice = atoi (buffer);
  } while (!in_list (drivers, choice));
  free (drivers);*/
  choice = serial[0].connection_type;


//       strcpy(target, "");


  avail = net_driverlist_create();
  net_driverlist_add (avail, choice);
  if (!net_initdrivers (avail)) {
    information_box("Error initialising network!", " ", " ", 1);
    clear_box();
    return -1;
  }

  serial[0].initialised = 1;

  return 1;

//  if (serial[0].game_type == SERIAL_CLIENT)
//   return 1;


  
//  printf("\nReturn address is: %s\n", net_getlocaladdress (chan));
  
//  do {
//    printf("Enter target address (blank to quit):\n");
//    fgets(target,1024,stdin);

 
/*    if (target[1]) {
//      printf ("Checking address...\n");
      not_ok = net_assigntarget (chan,target);
      if (not_ok) printf("\nCouldn't use that address; please try another.\n");
    } else not_ok=0;
  } while (not_ok);
  
  if (target[1]==0) {
    printf("Aborted.\n");
    exit(3);
  }*/

/*
switch(which_port)
{
  case 1:
//  rest(1000);
   if ((cport = comm_port_init(_com1)) == NULL)
   {
      dz_print_comm_err();
        return -1;
   }
//  rest(1000);

   if (comm_port_load_settings(cport, "port1.ini") == 0)
   {
       dz_print_comm_err();
        return -1;
   }

   if (!comm_port_install_handler(cport))
   {
      dz_print_comm_err();
        return -1;
   }
//  rest(1000);
  break;

  case 2:
   // Set up comm2

   if ((cport = comm_port_init(_com2)) == NULL)
   {
//       dz_print_comm_err();
        return -1;
   }

   if (comm_port_load_settings(cport, "port2.ini") == 0)
   {
//       dz_print_comm_err();
        return -1;
   }

   if (!comm_port_install_handler(cport))
   {
//      dz_print_comm_err();
        return -1;
   }
  break;
}

 return 1;

 */
}

/*
TO DO:
should allow player to hit escape and break out of the loop.
In which case it returns 0.
Also: need to fix problem with players being mixed up.
*/
int serial_game_init(void)
{

int wcount;
int prand_seed = 0;
int passed = 0;
//int p;

send_counter = 0;
rec_counter = 0;
text_mode(0);

int position = 0;
int success = 0;

#ifdef CONNECT_DEBUG
char info_string [100];
char info_string2 [100];
char itstring [10];
#endif

int i;

 arena[0].dirt_possible = 0;

switch(serial[0].game_type)
{
 case SERIAL_SERVER:
// need to add , position);
 
  position = add_int_to_send_buffer(game[0].gravity, position);
 if (game[0].game_type == GAME_CAP_FLAG || game[0].game_type == GAME_PORKBALL)
  game[0].game_type = GAME_KILL;
  position = add_int_to_send_buffer(game[0].game_type, position);
  position = add_int_to_send_buffer(game[0].score_type, position);
  position = add_int_to_send_buffer(game[0].score_limit, position);
  position = add_int_to_send_buffer(game[0].lives_type, position);
  position = add_int_to_send_buffer(game[0].lives_each, position);
  position = add_int_to_send_buffer(game[0].pk_number, position);
  position = add_int_to_send_buffer(game[0].pk_time_between, position);
  position = add_int_to_send_buffer(game[0].pk_health, position);
  position = add_int_to_send_buffer(game[0].pk_weapon, position);
  position = add_int_to_send_buffer(game[0].pk_equip, position);
  position = add_int_to_send_buffer(game[0].starting_weapons, position);
  position = add_int_to_send_buffer(game[0].bullet_speed, position);
  position = add_int_to_send_buffer(game[0].health_amount, position);
  position = add_int_to_send_buffer(game[0].impact_damage, position);
  position = add_int_to_send_buffer(game[0].reload_time, position);
  position = add_int_to_send_buffer(game[0].fast_bullets, position);
  position = add_int_to_send_buffer(game[0].bullets_explode, position);
  position = add_int_to_send_buffer(game[0].soft_dirt, position);
  position = add_int_to_send_buffer(game[0].rechoose_atype, position);
  position = add_int_to_send_buffer(game[0].unlimited_clips, position);
  position = add_int_to_send_buffer(game[0].reset_weapons, position);
  position = add_int_to_send_buffer(game[0].penalty, position);
  position = add_int_to_send_buffer(game[0].fruit_no, position);
  position = add_int_to_send_buffer(game[0].th_base_no, position);
  position = add_int_to_send_buffer(game[0].pk_time_between, position);
  position = add_int_to_send_buffer(game[0].pk_number, position);
  position = add_int_to_send_buffer(game[0].pk_health, position);
  position = add_int_to_send_buffer(game[0].pk_weapon, position);
  position = add_int_to_send_buffer(game[0].pk_equip, position);
  position = add_int_to_send_buffer(game[0].show_map, position);
  position = add_int_to_send_buffer(game[0].blast_bullets, position);
  position = add_int_to_send_buffer(game[0].atypes_avail, position);
  position = add_int_to_send_buffer(game[0].lightsourcing, position);
  position = add_int_to_send_buffer(game[0].name_boxes, position);

  game[0].weapon_status [WPN_DIRTBOMB] = 0;
  game[0].weapon_status [WPN_CLOD_OF_DIRT] = 0;
  game[0].weapon_status [WPN_HUNTER] = 0;

  for (wcount = 0; wcount < NO_WEAPONS; wcount ++)
  {
   position = add_byte_to_send_buffer(game[0].weapon_status [wcount], position);
  }
  for (wcount = 0; wcount < NO_EQUIP; wcount ++)
  {
   position = add_byte_to_send_buffer(game[0].equip_status [wcount], position);
  }
  for (wcount = 0; wcount < 20; wcount ++)
  {
   position = add_byte_to_send_buffer(arena[0].level_loaded [wcount], position);
  }

  position = add_int_to_send_buffer(arena[0].max_x, position);
  position = add_int_to_send_buffer(arena[0].max_y, position);
  arena[0].dirt_fill = 0;
  position = add_int_to_send_buffer(arena[0].dirt_fill, position);
  position = add_int_to_send_buffer(arena[0].teams, position);
  position = add_int_to_send_buffer(arena[0].ai_no_impact, position);
  position = add_int_to_send_buffer(arena[0].ai_unlimited_clips, position);
  position = add_int_to_send_buffer(arena[0].ai_weapons, position);
  arena[0].replace_dirt = 0;
  position = add_int_to_send_buffer(arena[0].replace_dirt, position);
  position = add_int_to_send_buffer(arena[0].level_colours, position);
  position = add_int_to_send_buffer(arena[0].level_style, position);
  position = add_int_to_send_buffer(arena[0].solid_density, position);
  position = add_int_to_send_buffer(arena[0].ambient_light, position);
  position = add_int_to_send_buffer(arena[0].lamp_amount, position);

  prand_seed = prand(20000);
  passed = prand_seed;
  position = add_int_to_send_buffer(prand_seed, position);
  init_prand(prand_seed);
//  textprintf(screen, large_font, 32, 32, WHITE, "Seed: %i", passed);
// for (p = 0; p < 10; p ++)
// {
//  textprintf(screen, large_font, 150, p * 10 + 10, WHITE, "prand: %i", prand(1000));
// }
//  rest(3000);

  for (i = 0; i < NO_CLIENTS; i ++)
  {
   if (client[i].active == 0) continue;
   rest(500);
   send_buffer_channel(client[i].client_channel, position);
  }

#ifdef CONNECT_DEBUG
  strcpy(info_string, "Bytes Sent: ");
  itoa(position, itstring, 10);
  strcat(info_string, itstring);
  strcpy(info_string2, "First Value: ");
  itoa((int) send_buffer [0], itstring, 10);
  strcat(info_string2, itstring);
  information_box(info_string, info_string2, " ", 1);
  clear_box();
#endif

  // Clients sent in their details when joining. Now send all user
  //  details out to all clients.
  async_init_clients();

  
 break;

 case SERIAL_CLIENT:
 rest(500);


 do
 {
  success = get_buffer_channel(comm_channel);
  if (key [KEY_ESC])
   return 0;
 } while (success == -1);


#ifdef CONNECT_DEBUG
  strcpy(info_string, "Bytes Received: ");
  itoa(success, itstring, 10);
  strcat(info_string, itstring);
  strcpy(info_string2, "First Value: ");
  itoa((int) receive_buffer [0], itstring, 10);
  strcat(info_string2, itstring);
  information_box(info_string, info_string2, " ", 1);
  clear_box();
#endif

//  if (success < 10) goto get_it;


 game[0].gravity = get_int_from_rec_buffer(position); position += 2;
 game[0].game_type = get_int_from_rec_buffer(position); position += 2;
 game[0].score_type = get_int_from_rec_buffer(position); position += 2;
 game[0].score_limit = get_int_from_rec_buffer(position); position += 2;
 game[0].lives_type = get_int_from_rec_buffer(position); position += 2;
 game[0].lives_each = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_number = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_time_between = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_health = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_weapon = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_equip = get_int_from_rec_buffer(position); position += 2;
 game[0].starting_weapons = get_int_from_rec_buffer(position); position += 2;
 game[0].bullet_speed = get_int_from_rec_buffer(position); position += 2;
 game[0].health_amount = get_int_from_rec_buffer(position); position += 2;
 game[0].impact_damage = get_int_from_rec_buffer(position); position += 2;
 game[0].reload_time = get_int_from_rec_buffer(position); position += 2;
 game[0].fast_bullets = get_int_from_rec_buffer(position); position += 2;
 game[0].bullets_explode = get_int_from_rec_buffer(position); position += 2;
 game[0].soft_dirt = get_int_from_rec_buffer(position); position += 2;
 game[0].rechoose_atype = get_int_from_rec_buffer(position); position += 2;
 game[0].unlimited_clips = get_int_from_rec_buffer(position); position += 2;
 game[0].reset_weapons = get_int_from_rec_buffer(position); position += 2;
 game[0].penalty = get_int_from_rec_buffer(position); position += 2;
 game[0].fruit_no = get_int_from_rec_buffer(position); position += 2;
 game[0].th_base_no = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_time_between = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_number = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_health = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_weapon = get_int_from_rec_buffer(position); position += 2;
 game[0].pk_equip = get_int_from_rec_buffer(position); position += 2;
 game[0].show_map = get_int_from_rec_buffer(position); position += 2;
 game[0].blast_bullets = get_int_from_rec_buffer(position); position += 2;
 game[0].atypes_avail = get_int_from_rec_buffer(position); position += 2;
 game[0].lightsourcing= get_int_from_rec_buffer(position); position += 2;
 game[0].name_boxes= get_int_from_rec_buffer(position); position += 2;
 for (wcount = 0; wcount < NO_WEAPONS; wcount ++)
 {
  game[0].weapon_status [wcount] = get_byte_from_rec_buffer(position);
  position ++;
 }
 for (wcount = 0; wcount < NO_EQUIP; wcount ++)
 {
  game[0].equip_status [wcount] = get_byte_from_rec_buffer(position);
  position ++;
 }
 for (wcount = 0; wcount < 20; wcount ++)
 {
  arena[0].level_loaded [wcount] = get_byte_from_rec_buffer(position);
  position ++;
 }

 arena[0].max_x = get_int_from_rec_buffer(position); position += 2;
 arena[0].max_y = get_int_from_rec_buffer(position); position += 2;
 arena[0].dirt_fill = get_int_from_rec_buffer(position); position += 2;
 arena[0].teams = get_int_from_rec_buffer(position); position += 2;
 arena[0].ai_no_impact = get_int_from_rec_buffer(position); position += 2;
 arena[0].ai_unlimited_clips = get_int_from_rec_buffer(position); position += 2;
 arena[0].ai_weapons = get_int_from_rec_buffer(position); position += 2;
 arena[0].replace_dirt = get_int_from_rec_buffer(position); position += 2;
 arena[0].level_colours = get_int_from_rec_buffer(position); position += 2;
 arena[0].level_style = get_int_from_rec_buffer(position); position += 2;
 arena[0].solid_density = get_int_from_rec_buffer(position); position += 2;
 arena[0].ambient_light = get_int_from_rec_buffer(position); position += 2;
 arena[0].lamp_amount = get_int_from_rec_buffer(position); position += 2;

 prand_seed = get_int_from_rec_buffer(position); position += 2;
 passed = prand_seed;

 init_prand(prand_seed);

 // Get details of all clients from server
 async_client_get_clients();
 
// textprintf(screen, large_font, 32, 32, WHITE, "Seed: %i", passed);
// for (p = 0; p < 10; p ++)
// {
//  textprintf(screen, large_font, 150, p * 10 + 10, WHITE, "prand: %i", prand(1000));
// }
// rest(3000);
 break;

 }

 return 1;
 
}

void synch_gamestart(void)
{

 int i;
 unsigned char counter = 0;

 char client_here [NO_CLIENTS];
 char finished = 0;

 switch(serial[0].game_type)
 {
  case SERIAL_CLIENT:
  add_byte_to_send_buffer(local_client_no, 0);
  send_buffer_channel(comm_channel, 1);
  do
  {
   textprintf(screen, large_font, 10, 150, i, "Waiting for signal...");
   counter ++;
  } while (net_receive(comm_channel, receive_buffer, 1, NULL) == 0);
  break;
  case SERIAL_SERVER:
  if (serial[0].connection_type == CONNECTION_SERIAL)
  {
   while(finished == 0)
   {
    for (i = 0; i < NO_CLIENTS; i ++)
    {
     client_here [i] = 0;
    }
    do
    {
     textprintf(screen, large_font, 10, 150, i, "Waiting...");
     counter ++;
    } while (net_receive(client[0].client_channel, receive_buffer, 1, NULL) == 0);

    if (client[receive_buffer [0]].active == 1)
     client_here [receive_buffer [0]] = 1;

    finished = 1;

    for (i = 0; i < NO_CLIENTS; i ++)
    {
     if (client[i].active == 1 && client_here [i] == 0)
      finished = 0;
    }
   }
  } // end if serial
   else
   {
    for (i = 0; i < NO_CLIENTS; i ++)
    {
     if (client[i].active == 0) continue;
     do
     {
      textprintf(screen, large_font, 10, 150 + i * 10, counter, "Waiting for client %i...", i);
      counter ++;
     } while (net_receive(client[i].client_channel, receive_buffer, 1, NULL) == 0);
    }
   }
//    if (client[i].active == 0) continue;
//   do
//   {
//   } while (net_receive(client[i].client_channel, receive_buffer, 1, NULL) == 0);
//   textprintf(screen, large_font, 10, 150 + i * 10, 15, "Received %i from client %i.", receive_buffer [0], i);
  for (i = 0; i < NO_CLIENTS; i ++)
  {
   if (client[i].active == 0) continue;
   add_byte_to_send_buffer(199, 0);
   send_buffer_channel(client[i].client_channel, 1);
   textprintf(screen, large_font, 100, 100 + i * 10, 15, "Sent signal to client %i.", i);
  }
  break;

 }
   textprintf(screen, large_font, 130, 150, 15, "Received all... Starting.");

 rest(3000);

}






/*
void sendval(unsigned char val)
{
 unsigned char buffer [1];

 buffer [0] = val;

 net_send (comm_channel, buffer, 1);
// comm_port_out(cport, val);
}

void send_int(int send)
{
 unsigned char c1, c2;

// c2 = send & 0xff;
// c1 = send << 8;
 c2 = send % 256;
 c1 = send / 256;

// int ic1 = c1, ic2 = c2;
 unsigned char buffer [1];
 buffer [0] = c2;
 net_send (comm_channel, buffer, 1);
 buffer [0] = c1;
 net_send (comm_channel, buffer, 1);
 
// comm_port_out(cport, c2);
// comm_port_out(cport, c1);

// textprintf(screen, large_font, 10, 10 * send_counter, WHITE, "Sent: %i (%i, %i)    ", send, ic1, ic2);

// send_counter ++;
// if (send_counter > 17) send_counter = 0;
// rest(400);

}
*/
char get_buffer_conn(void)
{

 int buffer_size;

// if ((buffer_size = net_query_rdm(comm_conn)) == 0) return -1;

// net_receive_rdm(comm_conn, receive_buffer, MAX_BUFFER_SIZE);

 return buffer_size;

}

char send_buffer_conn(int buffer_size)
{

// net_send_rdm(comm_conn, send_buffer, buffer_size);

 return 1;

}

int get_buffer_channel(NET_CHANNEL *buf_channel)
{

 int buffer_size;

// char buffrec [5];
// int pos = 0;

 if ((buffer_size = net_query(buf_channel)) == 0) return -1;

/*
 do
 {
  net_receive(comm_channel, buffrec, 1, NULL);
  receive_buffer [pos] = buffrec [0];
  pos ++;
 } while(net_query(comm_channel) != 0);

 buffer_size = pos;*/

 buffer_size = net_receive(buf_channel, receive_buffer, MAX_BUFFER_SIZE - 50, last_address_received);

 return buffer_size;

}

char send_buffer_channel(NET_CHANNEL *send_channel, int buffer_size)
{
/*
 int i;

 char buffsend [2];

 for (i = 0; i < buffer_size; i ++)
 {
  buffsend [0] = send_buffer [i];
  net_send(comm_channel, buffsend, 1);
 }*/

 net_send(send_channel, send_buffer, buffer_size);
 return 1;

}

int add_byte_to_send_buffer(unsigned char add, int position)
{

 send_buffer [position] = add;
 return position + 1;

}

unsigned char get_byte_from_rec_buffer(int position)
{

 return receive_buffer [position];
 
}

int add_int_to_send_buffer(int add, int position)
{

 unsigned char c1, c2;
 c1 = add % 256;
 c2 = add / 256;

 send_buffer [position] = c1;
 send_buffer [position + 1] = c2;

 return position + 2;

}

int get_int_from_rec_buffer(int position)
{

 int c1 = receive_buffer [position];
 int c2 = receive_buffer [position + 1];

 return (c2 * 256) + (c1);

}

char anything_received(NET_CHANNEL *rec_channel)
{
// if (comm_port_in_empty(cport) == 0)
 if (net_query(rec_channel) != 0)
  return TRUE;
  
  return FALSE;
}



/*
unsigned char receive_byte(void)
{
// unsigned char c1;

 int c1;

 unsigned char buffer [1];
 
 do
 {
 } while ((c1 = net_receive(comm_channel, buffer, 1, NULL)) <= 0);

 return buffer [0];

}

int receive_int(void)
{
 //unsigned char c1, c2;

 unsigned char buffer [1];
 int c1, c2;

 do
 {
 } while ((net_receive(comm_channel, buffer, 1, NULL)) <= 0);

 c1 = buffer [0];

 do
 {
 } while ((net_receive(comm_channel, buffer, 1, NULL)) <= 0);

 c2 = buffer [0];

// int ic1 = c1 & 0xff, ic2 = c2 >> 8;

// textprintf(screen, large_font, 10, 10 * rec_counter, WHITE, "Received: %i (%i, %i)    ", (c2 * 256) + (c1), ic1, ic2);

// rec_counter ++;
// if (rec_counter > 17) rec_counter = 0;
// rest(400);
 
 return (c2 * 256) + (c1);
// return (c2 >> 8) + (c1 & 0xff);

}
*/

/*

// THE REAL ONE!:
int get_remote_cmds(int user_controlled, int server_state, int to_which)
{

//int timeout = 0;

int val = 0;
int order = 0;
unsigned char cmd_buffer [4];

text_mode(0);

user_controlled = receive_byte();

do
{

 while (TRUE)
 {

  val = comm_port_test(cport);
  if (val != -1) break;
 } //while ((val = comm_port_test(cport)) == -1);
  rec_counter ++;

//  textprintf(screen, large_font, 10, 180, WHITE, "rec_counter : %i    ", rec_counter);

 cmd_buffer [order] = data_(val);

 order ++;

} while (order < 4); // was + 1

 if (to_which == SERVER_BUFFER)
  bits_to_server_cmds(cmd_buffer, user_controlled, server_state);
   else
    bits_to_user_cmds(cmd_buffer, user_controlled);
  
return user_controlled;

}


int send_remote_commands(unsigned char user_sending, int server_state, int from_which)
{

 unsigned char ch [4];
 int order = 0;

 if (from_which == SERVER_BUFFER)
  server_cmds_to_bits(ch, user_sending, server_state);
   else
    user_cmds_to_bits(ch, user_sending);
    
 sendval(user_sending);

 for (order = 0; order < 4; order ++)
 {
    sendval(ch [order]);
 }

 
 return 1;

}

// NOT THE REAL ONE
int send_remote_commands2(int cmd_buffer [NO_CMDS], unsigned char user_sending)
{

// unsigned char ch;
 int order = 0;

// sendval(user_sending);

 for (order = 0; order < NO_CMDS; order ++)
// for (order = 0; order < 10000; order ++)
 {
  if (order == CMD_LEFT) sendval(1);
   else
    sendval(0);
//    sendval(user_cmd_buffer [user_sending] [order]);
 }

 
// do
// {
// sendval(199);
// ack = comm_port_test(cport);
// } while(ack != 199);

 return 1;

}

void user_cmds_to_bits(unsigned char cmd_buf [4], int user_cmds)
{
 int i;
 int bin = 1;

 unsigned long cmds = 0;

 for (i = 0; i < NO_CMDS; i ++)
 {
  if (user_cmd_buffer [user_cmds] [i] != 0)
   cmds += bin;
  bin *= 2;
 }

 cmd_buf [0] = cmds % 256;
 cmd_buf [1] = (cmds % 65536) / 256;
 cmd_buf [2] = (cmds % 16777216) / 65536;
 cmd_buf [3] = cmds / 16777216;
 
}

void server_cmds_to_bits(unsigned char cmd_buf [4], int user_cmds, int server_state)
{
 int i;
 int bin = 1;

 unsigned long cmds = 0;

 for (i = 0; i < NO_CMDS; i ++)
 {
  if (server_cmd_buffer [server_state] [user_cmds] [i] != 0)
   cmds += bin;
  bin *= 2;
 }

 cmd_buf [0] = cmds % 256;
 cmd_buf [1] = (cmds % 65536) / 256;
 cmd_buf [2] = (cmds % 16777216) / 65536;
 cmd_buf [3] = cmds / 16777216;
 
}

void bits_to_user_cmds(unsigned char cmd_buf [4], int user_cmds)
{

 unsigned long cmds = 0;

 char bin_string [40];

 cmds += cmd_buf [0];
 cmds += cmd_buf [1] * 256;
 cmds += cmd_buf [2] * 65536;
 cmds += cmd_buf [3] * 16777216;

 text_mode(0);

 int i;

// cmds += 4294967295;
 cmds += 1 << 31;

 itoa(cmds, bin_string, 2);
 
 for (i = 0; i < NO_CMDS; i ++)
 {
//  if (user_cmd_buffer [user_cmds] [i] != 0)
//   cmds += bin;

//  if (cmds << i & 1)
//   user_cmd_buffer [user_cmds] [i] = 1;

  if (bin_string [NO_CMDS - i - 1 + 5] == '1')
   user_cmd_buffer [user_cmds] [i] = 1;
//  if (bin_string [i] == '1')
//   user_cmd_buffer [user_cmds] [i] = 1;


   
//  bin *= 2;
 }

  textprintf(screen, large_font, 10, 180, WHITE, "%s                ", bin_string);


}

void bits_to_server_cmds(unsigned char cmd_buf [4], int user_cmds, int server_state)
{

 unsigned long cmds = 0;

 char bin_string [40];

 cmds += cmd_buf [0];
 cmds += cmd_buf [1] * 256;
 cmds += cmd_buf [2] * 65536;
 cmds += cmd_buf [3] * 16777216;

 text_mode(0);

 int i;

 cmds += 1 << 31;

 itoa(cmds, bin_string, 2);
 
 for (i = 0; i < NO_CMDS; i ++)
 {
  if (bin_string [NO_CMDS - i - 1 + 5] == '1')
   server_cmd_buffer [server_state] [user_cmds] [i] = 1;
 }

}
*/

void wait_on_port(void)
{
// while(comm_port_out_empty(cport) != -1)
// {

// }

}
