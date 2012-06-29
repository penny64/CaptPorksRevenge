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

File: sound.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - sound stuff
 
*/

#include "allegro.h"
#include "sound.h"

#include "config.h"
#include "globvar.h"

#include "conio.h"
#include "math.h"
DATAFILE *soundf;

//SAMPLE *sound_list [10];

char sound_active;

void init_sound(void)
{

   if (sound_active == 0)
   {
//    cprintf("\n\r\n\rSound disabled in proj.cfg.");
    allegro_message("\n\r\n\rSound disabled in proj.cfg.");
    rest(500);
    return;
   }

   reserve_voices(10, 0);
   if (install_sound (DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == -1)
   {
    allegro_message("\n\r\n\rSound autodetect failed.");
    sound_active = 0;
    rest(300);
    do
    {
    } while (keypressed() == 0);
   }
   set_volume(255, 0);

   soundf = load_datafile("sound.dat");

   if (soundf == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file sound.dat");
      exit(1);
   }
   

//   priority->sound_list[WAV_POP] = 1;

}



void play_sound(int sample)
{

 if (sound_active == 0) return;

 stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);
 play_sample(soundf[sample].dat, 250, 127, 1000, 0);


}

void play_sound2(int sample, int frq, int vol, int pan)
{

 if (sound_active == 0) return;

 stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);
 play_sample(soundf[sample].dat, vol, pan, frq, 0);


}

/*
Positional sound for stereo effects.
y1 & y2 currently unused, but may be in future for some kind of
triangulation.
*/
void play_sound_pos(int sample, int frq, int vol, int x2, int y2)
{

 if (sound_active == 0) return;

 stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);

 if (options[0].positional_sound == 0 || no_players > 1)
 {
  play_sample(soundf[sample].dat, vol, 127, frq, 0);
 }


 int pan = 127;

 int vol2 = vol;
 int distance;

 int x1 = actor[player[1].actor_controlled].x;
 int y1 = actor[player[1].actor_controlled].y;

 distance = hypot(abs(x2 - x1), abs(y2 - y1)) / GRAIN;
 if (distance > 1000)
  return;
 if (distance > 300)
 {
  distance -= 300;
  distance = 1000 - distance;
  vol2 *= distance; //(800 - (distance - 300));
  vol2 /= 1000;
 }



 if (x1 == x2)
 {
  pan = 127;
 }
  else
  {
   if (x1 < x2 - (300 * GRAIN))
    pan = 0;
     else
      {
       if (x1 > x2 + (300 * GRAIN))
        pan = 255;
         else
         {
          if (x1 > x2)
          {
           pan = 127 + ((x1 - x2) * 125) / (300 * GRAIN);
          }
           else
           {
            pan = 127 - ((x2 - x1) * 125) / (300 * GRAIN);
           }
         }
      }
 }


 if (options[0].positional_sound == 2)
  pan = 255 - pan; // speakers reversed
   else
    pan = pan; // speakers reversed

/*  char kill_msg [50];
  char prgk [10];
  
     strcpy(kill_msg, "Pan: ");
     strcat(kill_msg, itoa(pan, prgk, 10));
     message(-1, kill_msg, COLOUR_GREY8);
*/
 play_sample(soundf[sample].dat, vol2, pan, frq, 0);

}

