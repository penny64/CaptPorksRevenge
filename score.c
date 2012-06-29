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

File: score.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - functions changing the scores, and messages to tell people that the
   scores have changed.
 - end of game functions (eg the rankings)
 - a few challenge-related things
 
*/

#include <string.h>
#include "libnet.h"

#include "allegro.h"

#include "config.h"

#include "globvar.h"
#include "bullet.h"
#include "display.h"
#include "objhead.h"
#include "sound.h"
#include "base.h"
//#include "async.h"
#include "connect.h"
#include "misc.h"
#include "chall.h"
#include "cloud.h"
#include "palette.h"

void end_game(int fade_speed);
int score_table(void);
int score_table_team(void);
void lose_life(int lactor);
void score_message(int stype, int user1, int user2, int async);
void game_is_over(int who_won);

extern RGB palet [256];
extern RGB mono_palet [256];
extern int game_over;
extern int text_size;

/*
Note - in capflag and porkball, sactor is a team number, not
an actor.
*/
void score_event(int sactor, int event, int event2)
{

 if (game_over != 0) return;

 if (serial[0].game_type == SERIAL_CLIENT)
  return;
   // only server is authorised to affect the score

 int i = 0;

 if (arena[0].teams == 0)
 {
  switch(event)
  {
   case SCORE_LFRUIT:
   user[actor[sactor].user].score += 2;
    // intentional fall-through...
   case SCORE_MFRUIT:
   user[actor[sactor].user].score += 2;
    // intentional fall-through...
   case SCORE_SFRUIT:
   user[actor[sactor].user].score ++;
   update_score();
   score_message(SMESSAGE_FRUIT, actor[sactor].user, event2, 0);
/*   char fstring [50];
   strcpy(fstring, user[actor[sactor].user].uname);
   strcat(fstring, " eats a");
   switch(event2)
   {
    case FRUITS_FIG: strcat(fstring, " fig."); break;
    case FRUITS_CHERRY: strcat(fstring, " cherry."); break;
    case FRUITS_BLUEBERRY: strcat(fstring, " blueberry."); break;
    case FRUITS_STRAWBERRY: strcat(fstring, " strawberry."); break;
    case FRUITM_PEACH: strcat(fstring, " peach."); break;
    case FRUITM_RAPPLE: strcat(fstring, "n apple."); break;
    case FRUITM_PEAR: strcat(fstring, " pear."); break;
    case FRUITL_DURIAN: strcat(fstring, " durian."); break;
    case FRUITL_PINEAPPLE: strcat(fstring, " pineapple."); break;
   }
   teamed_message(-1, fstring, actor[sactor].team);*/
   if (user[actor[sactor].user].score >= game[0].score_limit)
   {
    game_is_over(actor[sactor].user);
//    game_over = 200;
 //   end_game();
   }
   break;
   
   case SCORE_KILL:
   // last one standing gives you a score, though it's not used.
   if (game[0].game_type != GAME_KILL && game[0].game_type != GAME_LAST_ONE)
    break;

   case SCORE_FLAG:
   case SCORE_GRAIL:
   user[actor[sactor].user].score ++;
   update_score();
   if (user[actor[sactor].user].score >= game[0].score_limit)
   {
    game_is_over(actor[sactor].user);
//    game_over = 200;
 //   end_game();
   }
   break;
   case SCORE_SUICIDE:
   if (game[0].game_type == GAME_KILL
    || game[0].game_type == GAME_LAST_ONE)
   {
    if (user[actor[sactor].user].score > 0)
    {
     user[actor[sactor].user].score --;
     update_score();
    }
   }
   break;
  }
 }
  else
 { // Teams now:
  switch(event)
  {
   case SCORE_LFRUIT:
   team[user[actor[sactor].user].team].score += 2;
    // intentional fall-through...
   case SCORE_MFRUIT:
   team[user[actor[sactor].user].team].score += 2;
    // intentional fall-through...
   case SCORE_SFRUIT:
   team[user[actor[sactor].user].team].score ++;
   update_score();
   score_message(SMESSAGE_FRUIT, actor[sactor].user, event2, 0);
/*   char fstring [50];
   strcpy(fstring, user[actor[sactor].user].uname);
   strcat(fstring, " eats a");
   switch(event2)
   {
    case FRUITS_FIG: strcat(fstring, " fig."); break;
    case FRUITS_CHERRY: strcat(fstring, " cherry."); break;
    case FRUITS_BLUEBERRY: strcat(fstring, " blueberry."); break;
    case FRUITS_STRAWBERRY: strcat(fstring, " strawberry."); break;
    case FRUITM_PEACH: strcat(fstring, " peach."); break;
    case FRUITM_RAPPLE: strcat(fstring, "n apple."); break;
    case FRUITM_PEAR: strcat(fstring, " pear."); break;
    case FRUITL_DURIAN: strcat(fstring, " durian."); break;
    case FRUITL_PINEAPPLE: strcat(fstring, " pineapple."); break;
   }
   teamed_message(-1, fstring, actor[sactor].team);*/
   if (team[user[actor[sactor].user].team].score >= game[0].score_limit)
   {
    game_is_over(actor[sactor].team);
//    game_over = 200;
 //   end_game();
   }
   break;
   case SCORE_KILL:
   // last one standing gives you a score, though it's not used.
   if (game[0].game_type != GAME_KILL && game[0].game_type != GAME_LAST_ONE)
    break;
    if (user[actor[sactor].user].team == user[actor[event2].user].team)
    {
     if (team[user[actor[sactor].user].team].score > 0)
     {
      team[user[actor[sactor].user].team].score --;
      update_score();
     }
     break;
    }
   team[user[actor[sactor].user].team].score ++;
   update_score();
   if (team[user[actor[sactor].user].team].score >= game[0].score_limit)
   {
    game_is_over(actor[sactor].team);
//    game_over = 200;
   }
   break;

   case SCORE_GOAL:
   case SCORE_FLAG:
   team[sactor].score ++;
   for (i = 0; i < NO_TEAMS; i ++)
   {
    if (i != event2) team[i].score ++;
    if (team[i].score >= game[0].score_limit)
    {
    game_is_over(actor[sactor].team);
//     game_over = 200;
    }
   }
   update_score();
   break;
   
   case SCORE_TH:
   team[sactor].score ++;
   update_score();
   if (team[sactor].score >= game[0].score_limit)
   {
    game_is_over(actor[sactor].team);
//    game_over = 200;
 //   end_game();
   }
   break;
   
   case SCORE_GRAIL:
   team[user[actor[sactor].user].team].score ++;
   update_score();
   if (team[user[actor[sactor].user].team].score >= game[0].score_limit)
   {
    game_is_over(actor[sactor].team);
//    game_over = 200;
 //   end_game();
   }
   break;
   case SCORE_SUICIDE:
   if (game[0].game_type != GAME_KILL && game[0].game_type != GAME_LAST_ONE)
    break;
   if (team[user[actor[sactor].user].team].score > 0)
   {
    team[user[actor[sactor].user].team].score --;
    update_score();
   }
   break;
  }
 }

 //if (game_over == 200 && serial[0].game_type == SERIAL_SERVER)
 // async_end_game(0);

}

void lose_life(int lactor)
{

 if (game[0].lives_each == 0) return;

// if (game_over != 0) return;

 int la = 0;
 int highest_rank = 0;
// int team_out = 0;
 char found = 0;
// char msg [40];
// char itstring [30];

 if (user[actor[lactor].user].lives == 0)
 {
  if (serial[0].game_type == SERIAL_LOCAL_ONLY
      || serial[0].game_type == SERIAL_SERVER)
  {
     score_message(SMESSAGE_ELIMINATED, actor[lactor].user, 0, 0);
     
     if (arena[0].teams == 1) // && user[actor[lactor].user].team != TEAM_NONE)
     {
      team[user[actor[lactor].user].team].size --;
//      teamed_message(-1, msg, user[actor[lactor].user].team);
      if (team[user[actor[lactor].user].team].size == 0)
      {
       score_message(SMESSAGE_TEAM_ELIMINATED, user[actor[lactor].user].team, 0, 0);
//       teamed_message(-1, "Team wiped out.", user[actor[lactor].user].team);
       for (la = 0; la < NO_TEAMS; la ++)
       {
        if (team[la].ranked > highest_rank)
         highest_rank = team[la].ranked;
       }
       team[user[actor[lactor].user].team].ranked = highest_rank + 1;

       for (la = 0; la < NO_TEAMS; la ++)
       {
        if (team[la].size != 0)
         found ++;
       }
       if (found <= 1 && game_over == 0)
       {
        //if (serial[0].game_type == SERIAL_SERVER)
        // async_end_game(0);
        game_over = 200;
       }
      }
      
     }
/*       else
       {
        message(-1, msg, 31);
       }*/
  }
  
  user[actor[lactor].user].out_of_lives = 1;

  for (la = 0; la < NO_USERS; la ++)
  {
   if (user[la].ranked > highest_rank)
    highest_rank = user[la].ranked;
  }
  user[actor[lactor].user].ranked = highest_rank + 1;

  
  for (la = 0; la < NO_USERS; la ++)
  {
   if (user[la].active != 0 && user[la].out_of_lives == 0)
     found ++;
  }
  if (found <= 1 && game_over == 0)
  {
   //if (serial[0].game_type == SERIAL_SERVER)
   // async_end_game(0);
   game_over = 200;
  }
 }
  else
  {
   user[actor[lactor].user].lives --;
  }

  update_score();

}


void score_message(int stype, int user1, int user2, int async)
{
   char fstring [150];


   //if (serial[0].game_type == SERIAL_SERVER)
   //{
   // async_score_message(stype, user1, user2);
   //}

/*   if (serial[0].game_type == SERIAL_CLIENT)
   {
    if (async == 0) return;
   }*/

 switch(stype)
 {
  case SMESSAGE_FRUIT:
   strcpy(fstring, user[user1].uname);
   strcat(fstring, " eats a");
   switch(user2)
   {
    case FRUITS_FIG: strcat(fstring, " fig."); break;
    case FRUITS_CHERRY: strcat(fstring, " cherry."); break;
    case FRUITS_BLUEBERRY: strcat(fstring, " blueberry."); break;
    case FRUITS_STRAWBERRY: strcat(fstring, " strawberry."); break;
    case FRUITM_PEACH: strcat(fstring, " peach."); break;
    case FRUITM_RAPPLE: strcat(fstring, "n apple."); break;
    case FRUITM_PEAR: strcat(fstring, " pear."); break;
    case FRUITL_DURIAN: strcat(fstring, " durian."); break;
    case FRUITL_PINEAPPLE: strcat(fstring, " pineapple."); break;
   }
   teamed_message(-1, fstring, user[user1].team);
   break;
  case SMESSAGE_TH:
   strcpy(fstring, team_name(user1));
   strcat(fstring, " scores a point.");
   teamed_message(-1, fstring, user1);
   play_sound(WAV_SCORE);
   break;
  case SMESSAGE_GOAL:
    strcpy(fstring, team_name(user1));
    strcat(fstring, " scores against ");
    strcat(fstring, team_name(user2));
    strcat(fstring, "!");
    teamed_message(-1, fstring, user1);
    play_sound(WAV_SCORE);
    break;
  case SMESSAGE_GRAIL:
    strcpy(fstring, user[user1].uname);
    strcat(fstring, " has returned the Grail!");
    teamed_message(-1, fstring, user2);
    play_sound(WAV_SCORE);
    break;
  case SMESSAGE_FLAG:
    strcpy(fstring, user[user1].uname);
    strcat(fstring, " captures ");
    strcat(fstring, team_name(user2));
    strcat(fstring, "'s flag!");
    teamed_message(-1, fstring, user[user1].team);
    play_sound(WAV_SCORE);
    break;
  case SMESSAGE_CAPTURE:
    strcpy(fstring, user[user1].uname);
    strcat(fstring, " captures a base.");
    teamed_message(-1, fstring, user2);
    break;
  case SMESSAGE_ELIMINATED:
    strcpy(fstring, user[user1].uname);
/*    strcat(fstring, " (");
    strcat(fstring, itoa(user[user1].score, itstring, 10));
    strcat(fstring, "/");
    strcat(fstring, itoa(user[user1].lives, itstring, 10));
    strcat(fstring, ") has been eliminated!");*/
    strcat(fstring, " has been eliminated!");
    teamed_message(-1, fstring, user[user1].team);
    break;
  case SMESSAGE_TEAM_ELIMINATED:
    strcpy(fstring, team_name(user1));
    strcat(fstring, " wiped out!");
    teamed_message(-1, fstring, user1);
    break;
  case SMESSAGE_FLAG_RETURN:
    strcpy(fstring, user[user1].uname);
    strcat(fstring, " returns ");
    strcat(fstring, team_name(user[user1].team));
    strcat(fstring, "'s flag.");
    teamed_message(-1, fstring, user[user1].team);
    break;
  case SMESSAGE_HAS_FLAG:
    strcpy(fstring, user[user1].uname);
    strcat(fstring, " has ");
    strcat(fstring, team_name(user2));
    strcat(fstring, "'s flag.");
    teamed_message(-1, fstring, user[user1].team);
    break;


 }


}

void eat_fruit(int eactor, int pcount)
{
  switch(pickup[pcount].contains)
  {
    case FRUITS_CHERRY:
    case FRUITS_BLUEBERRY:
    case FRUITS_FIG:
    case FRUITS_STRAWBERRY:
    score_event(eactor, SCORE_SFRUIT, pickup[pcount].contains);
    create_cloud(CLOUD_1_POINT, pickup[pcount].x, pickup[pcount].y, 0, -20, 65, 1, 0, 0);
    break;
    case FRUITM_PEACH:
    case FRUITM_RAPPLE:
    case FRUITM_PEAR:
    score_event(eactor, SCORE_MFRUIT, pickup[pcount].contains);
    create_cloud(CLOUD_3_POINTS, pickup[pcount].x, pickup[pcount].y, 0, -20, 65, 1, 0, 0);
    break;
    case FRUITL_DURIAN:
    case FRUITL_PINEAPPLE:
    score_event(eactor, SCORE_LFRUIT, pickup[pcount].contains);
    create_cloud(CLOUD_5_POINTS, pickup[pcount].x, pickup[pcount].y, 0, -20, 65, 1, 0, 0);
    break;
  }

}

/*
There was going to be a nice fadeout effect using the allegro fade_out and
fade_in functions, but for some reason these screwed up the palette. Oh well.
*/
void end_game(int fade_speed)
{

// fade_out(fade_speed);


 rest(1000 / fade_speed);
 clear_keybuf();

 clear_bitmap(screen);
// set_palette(palet);
 init_palette();
 set_palette(palet);

 int challenge_winner = 0;

 if (arena[0].teams == 0)
  challenge_winner = score_table();
   else
    challenge_winner = score_table_team();

 if (arena[0].challenge_level != 0)
 {
  if (challenge_winner == 0)
  {
   switch(arena[0].challenge_level)
   {
    case 1: information_box("", "Norbert won?", "Hmm.", 0); break;
    case 2: information_box("", "Looks like you need some more practice.", "", 0); break;
    case 3: information_box("", "Sorry, you lose.", "", 0); break;
    case 4: information_box("", "Good, but not great.", "", 0); break;
    case 5: information_box("", "Oh well, better luck next time.", "", 0); break;
    case 6: information_box("", "Almost there. Try again.", "", 0); break;
    case 7: information_box("", "Very good, but not quite good enough.", "", 0); break;
   }
   end_challenge();
  }
 }

 if (arena[0].qstart != QSTART_NONE)
  end_qstart();

    //shutdown_network();

// fade_in(*palet, fade_speed);
// set_palette(*palet);
 do
 {

 } while (keypressed() == 0);

 clear_keybuf();

 //net_init();

}


/*
returns 1 if player 1's team wins (for challenge game)
*/
int score_table_team(void)
{

// int num_users = 0;
 int li = 0;
 int ui = 0;
 int ranked = 0;
 char found_rank = 0;
 int rank [NO_TEAMS];
 int ranknum [NO_TEAMS];
 int done_team [NO_TEAMS];
 int highest_score = 1000;
// int done_score = 0;

 char rank_text [40];
 char ittext [10];

 textprintf_centre(screen, large_font, 160, text_size, 1, "Game Over");
 textprintf_centre(screen, large_font, 160, text_size * 2, 1, "Team Ranking");

 if (game[0].game_type == GAME_LAST_ONE)
 {
  for (ui = 0; ui < NO_TEAMS; ui ++)
  {
   if (team[ui].ranked == 0) team[ui].ranked = 100;
  }
 }
 // if a game of last-one-standing is aborted, survivors should all be
 //  ranked 1.

 do
 {
//  if (user[li].active == 1)
//  num_users ++;
  rank [li] = -1;
  ranknum [li] = li;
  done_team [li] = 0;
  li ++;
 } while (li < NO_TEAMS);

 li = 0;

 do
 {
 ui = 0;

 do
 {

  if (team[ui].active == 0)
  {
   ui ++;
   continue;
  }


  switch(game[0].game_type)
  {
   default: // most games are scored on points
   case GAME_KILL:
    if (team[ui].score >= highest_score && done_team [ui] == 0)
    {
     rank [li] = ui;
     ranknum [li] = ranked;
     li ++;
     done_team [ui] = 1;
     found_rank = 1;
    }
   break;
   case GAME_LAST_ONE:
    if (team[ui].ranked >= highest_score && done_team [ui] == 0)
    {
     rank [li] = ui;
     ranknum [li] = ranked;
     li ++;
     done_team [ui] = 1;
     found_rank = 1;
    }
   break;
  }

  
  ui ++;
 } while (ui < NO_TEAMS);
 highest_score --;
 if (found_rank == 1)
  ranked ++;
 found_rank = 0;

 } while (highest_score > -5);

 li = 0;

 do
 {
  if (rank [li] == -1)
  {
   li ++;
   continue;
  }
  
//  strcpy(rank_text, itoa(li + 1, ittext, 10));
  strcpy(rank_text, itoa(ranknum [li] + 1, ittext, 10));
  strcat(rank_text, ". ");

  switch(rank [li])
  {
   case TEAM_RED: strcat(rank_text, "Red Team"); break;
   case TEAM_BLUE: strcat(rank_text, "Blue Team"); break;
   case TEAM_GREEN: strcat(rank_text, "Green Team"); break;
   case TEAM_GREY: strcat(rank_text, "Grey Team"); break;
   case TEAM_GOLD: strcat(rank_text, "Gold Team"); break;
   case TEAM_BROWN: strcat(rank_text, "Brown Team"); break;
  }
  strcat(rank_text, " - Score ");
  strcat(rank_text, itoa(team[rank [li]].score, ittext, 10));
//  if (game[0].lives_each > 0)
//  {
   if (team [rank [li]].size == 0)
    strcat(rank_text, " - Eliminated");
//     else
//      {
//       strcat(rank_text, " - Lives ");
//       strcat(rank_text, itoa(team[rank [li]].lives, ittext, 10));
//      }
//  }
  
  textprintf_centre(screen, large_font, 160, ((li + 3) * text_size), team_colours(rank [li]), rank_text);
  li ++;
 } while (li < NO_TEAMS);

 if (rank [0] == player[1].team
     && team[player[1].team].score >= game[0].score_limit)
  return 1;

 return 0;

}







/*
returns 1 if player 1 wins (for challenge game)
*/
int score_table(void)
{

// int num_users = 0;
 int li = 0;
 int ui = 0;
 int ranked = 0;
 char found_rank = 0;
 int rank [NO_USERS];
 int ranknum [NO_USERS];
 int done_user [NO_USERS];
 int highest_score = 1000;
// int done_score = 0;

 char rank_text [40];
 char ittext [10];

 textprintf_centre(screen, large_font, 160, text_size, 1, "Game Over");
 textprintf_centre(screen, large_font, 160, text_size * 2, 1, "Ranking");


 if (game[0].game_type == GAME_LAST_ONE)
 {
  for (ui = 0; ui < NO_USERS; ui ++)
  {
   if (user[ui].ranked == 0) user[ui].ranked = 100;
  }
 }
 // if a game of last-one-standing is aborted, survivors should all be
 //  ranked 1.

 do
 {
//  if (user[li].active == 1)
//  num_users ++;
  rank [li] = -1;
  ranknum [li] = li;
  done_user [li] = 0;
  li ++;
 } while (li < NO_USERS);

 li = 0;

 do
 {
 ui = 0;

 do
 {

  if (user[ui].active == 0)
  {
   ui ++;
   continue;
  }


  switch(game[0].game_type)
  {
   default:
   case GAME_KILL:
    if (user[ui].score >= highest_score && done_user [ui] == 0)
    {
     rank [li] = ui;
     ranknum [li] = ranked;
     li ++;
     done_user [ui] = 1;
     found_rank = 1;
    }
   break;
   case GAME_LAST_ONE:
    if (user[ui].ranked >= highest_score && done_user [ui] == 0)
    {
     rank [li] = ui;
     ranknum [li] = ranked;
     li ++;
     done_user [ui] = 1;
     found_rank = 1;
    }
   break;
  }

  
  ui ++;
 } while (ui < NO_USERS);
 highest_score --;
 if (found_rank == 1)
  ranked ++;
 found_rank = 0;

 } while (highest_score > -5);

 li = 0;

 do
 {
  if (rank [li] == -1)
  {
   li ++;
   continue;
  }
  
//  strcpy(rank_text, itoa(li + 1, ittext, 10));
  strcpy(rank_text, itoa(ranknum [li] + 1, ittext, 10));
  strcat(rank_text, ". ");
  strcat(rank_text, user[rank [li]].uname);
  strcat(rank_text, " - Score ");
  strcat(rank_text, itoa(user[rank [li]].score, ittext, 10));
  if (game[0].lives_each > 0)
  {
   if (user [rank [li]].out_of_lives == 1)
    strcat(rank_text, " - Eliminated");
     else
      {
       strcat(rank_text, " - Lives ");
       strcat(rank_text, itoa(user[rank [li]].lives, ittext, 10));
      }
  }
  
  textprintf_centre(screen, large_font, 160, ((li + 3) * text_size), COLOUR_YELLOW4, rank_text);
  li ++;
 } while (li < NO_USERS);

 if (rank [0] == player[1].user
     && user[player[1].user].score >= game[0].score_limit)
  return 1;

 return 0;


}


void check_scores(void)
{

}

void game_is_over(int who_won)
{
  game_over = 200;
  set_palette(mono_palet);

  if (arena[0].teams == 1)
  {
   if (player[1].team == who_won)
    player[1].winner = 1;
   if (player[2].team == who_won)
    player[2].winner = 1;
  }
   else
   {
    if (player[1].user == who_won)
     player[1].winner = 1;
    if (player[2].user == who_won)
     player[2].winner = 1;
   }
}

