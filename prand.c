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

File: prand.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - the random number generator

Note: some parts of the code rely on the RNG producing reliable results
from a given seed - eg given a set of parameters and a seed, the level
generator should produce the same level on different computers linked
together. Also the bullet_rand function in bullet.c calls init_prand to
make sure that bullets with random paths or patterns (eg the RPG or an
exploding toxin grenade) exhibit the same behaviour across a networked
game.
 
*/
#include "allegro.h"
#include <stdlib.h>

#include "config.h"

int prand_seed;
int last_prand;

extern struct arena_struct arena [1];

int init_prand(int seedy)
{
 srand(seedy);
 last_prand = seedy;
 srandom(seedy); // might as well
 return 1;
}



int prand(int prand_limit)
{
// return random() % prand_limit;


 if (prand_limit == 0) return 0;

// srand(last_prand);
// random();
 int retvalue = rand();
 last_prand = retvalue;
 retvalue %= prand_limit;

 return retvalue;

}



int nvrand(int nvrand_limit)
{
 if (nvrand_limit == 0) return nvrand_limit;
 
 return arena[0].counter % nvrand_limit;

}


inline int inrand(int inrand_limit)
{
 return arena[0].counter % inrand_limit;
}


