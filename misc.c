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

File: misc.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - the pop-up information box that is sometimes used in the menus.
 - it was supposed to included various miscellaneous things that didn't
   fit anywhere else, but it doesn't really.
 
*/

#include "allegro.h"

#include <string.h>

#include "config.h"
#include "globvar.h"

extern int video_mode;

unsigned char information_box(const char *text1, const char *text2, const char *text3, char waiting)
{

 unsigned char read_k = 0;

 switch(video_mode)
 {
  case 0: // 320x200
  rectfill(screen, 50, 80, 270, 140, 0);
  rect(screen, 50, 80, 270, 140, 5);

  textprintf_centre(screen, large_font, 160, 90, 15, text1);
  textprintf_centre(screen, large_font, 160, 105, 15, text2);
  textprintf_centre(screen, large_font, 160, 120, 15, text3);
  break;
  default:
  rectfill(screen, 140, 170, 500, 300, 0);
  rect(screen, 140, 170, 500, 300, 5);

  textprintf_centre(screen, large_font, 320, 200, 15, text1);
  textprintf_centre(screen, large_font, 320, 230, 15, text2);
  textprintf_centre(screen, large_font, 320, 260, 15, text3);
  break;
 }

 if (waiting)
 {
  rest(500);
  switch(video_mode)
  {
   case 0: // 320x200
   rect(screen, 50, 80, 270, 140, 180);
   break;
   default:
   rect(screen, 140, 170, 500, 300, 180);
   break;
  }
  while(!keypressed())
  {
  }
  read_k = readkey();
 }

 return read_k;

}


void clear_box(void)
{
 switch(video_mode)
 {
  case 0: // 320x200
  rectfill(screen, 50, 80, 270, 140, 0);
  break;
  default:
  rectfill(screen, 140, 170, 500, 300, 0);
  break;
 }

}
