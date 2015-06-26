
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

File: weapon.c
History:
Sometime in 2002 - Started work (Capt Pork)
28/1/03 - Version 1.0 finalised (CP)

This file contains:
 - just a big array full of details of weaponry.
 
*/

#include "allegro.h"

#include "config.h"

#include "globvar.h"

//#define NO_WEAPONS 11

/*struct armoury
{
 char *long_name;
 char *short_name;
 int launch_speed;
 int recycle_time;
 int ammunition;
 int is_bullet;
 int reload_time;
 int damage; // note - damage is only from bullet impact, not any explosions.
     if damage == -1, bullet won't explode on impact with player
 int scatter;
 int recoil;
 int bullet_weight;
 int status;
};*/


struct armoury wlist [NO_WEAPONS] =
{
{
// Null weapon - means slot doesn't exist
"This is a bug!",
"None",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0, // bullet_weight
WPNCLASS_NONE // weak

},
{
// No weapon - means slot is empty
"No Weapon",
"None",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0, // bullet_weight
WPNCLASS_NONE // weak

},

// Note: NO_EQUIP needs to be accurate

{
"Bubble Shield",
"B-SHLD",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP

},
{
"Soft Shield",
"S-SHLD",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP

},
{
"Cloak",
"CLK",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP
},
{
"Jetpack",
"JET",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP
},
{
"Rocketpack",
"JET",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP
},
{
"Spotlight",
"SLIGHT",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP
},
{
"Regenerator",
"Regen",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP
},
{
"Shadow Field",
"SHAD",
0, // launch_speed
0, // slow_speed
0, // recycle_time
0, // ammunition
0, // is_bullet
0, // reload_time
0, // damage
0, // scatter
0, // recoil
0 // bullet_weight
,WPNCLASS_EQUIP
},



{
"Light Machine Gun",
"LMG",
90, // launch_speed
20, // slow_speed
4, // recycle_time
30, // ammunition
1, // is_bullet
45, // reload_time
100, // damage
40, // scatter
10, // recoil
10 // bullet_weight
,WPNCLASS_WEAPON

},
{
"Heavy Machine Gun",
"HMG",
80, // launch_speed
16, // slow_speed
6, // recycle_time
20, // ammunition
1, // is_bullet
75, // reload_time
200, // damage
70, // scatter
20, // recoil
20 // bullet_weight
,WPNCLASS_WEAPON

},
{
"Shotgun",
"SGUN",
50, // launch_speed
17, // slow_speed
25, // recycle_time
6, // ammunition
1, // is_bullet
50, // reload_time
50, // damage
0, // scatter
20, // recoil
6, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Slug Gun",
"SLUG",
60, // launch_speed
20, // slow_speed
20, // recycle_time
12, // ammunition
1, // is_bullet
50, // reload_time
400, // damage
10, // scatter
20, // recoil
80, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Rocket Launcher",
"R-L",
22, // launch_speed
22, // slow_speed
35, // recycle_time
3, // ammunition
0, // is_bullet
80, // reload_time
1, // damage
0, // scatter
10, // recoil
60 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Mortar", // generally referred to as grenade launcher/GR_L in code
"MORT",
15, // launch_speed
15, // slow_speed
35, // recycle_time
4, // ammunition
0, // is_bullet
60, // reload_time
0, // damage
0, // scatter
10, // recoil
50, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Grenade",
"GREN",
20, // launch_speed
20, // slow_speed
35, // recycle_time
4, // ammunition
0, // is_bullet
45, // reload_time
-1, // damage
0, // scatter
10, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}

,
{
"LR Rocket",
"LRR-L",
5, // launch_speed
5, // slow_speed
35, // recycle_time
3, // ammunition
0, // is_bullet
80, // reload_time
1, // damage
0, // scatter
5, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}

,
{
"Swarm Rockets",
"SWARM",
5, // launch_speed
5, // slow_speed
7, // recycle_time
10, // ammunition
0, // is_bullet
90, // reload_time
1, // damage
120, // scatter
5, // recoil
30 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Plasma Rifle",
"PLAS-R",
30, // launch_speed
30, // slow_speed
8, // recycle_time
12, // ammunition
0, // is_bullet
50, // reload_time
1, // damage
30, // scatter
5, // recoil
10 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Plasma Cannon",
"PLAS-C",
20, // launch_speed
20, // slow_speed
20, // recycle_time
7, // ammunition
0, // is_bullet
50, // reload_time
1, // damage
30, // scatter
30, // recoil
15 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Rectifier",
"RECT",
10, // launch_speed
10, // slow_speed
20, // recycle_time
1, // ammunition
0, // is_bullet
100, // reload_time
1, // damage
0, // scatter
70, // recoil
30 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Nuclear Missile",
"NUKE-M",
5, // launch_speed
5, // slow_speed
20, // recycle_time
1, // ammunition
0, // is_bullet
100, // reload_time
1000, // damage
0, // scatter
100, // recoil
100 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Frag Grenade",
"FRAG",
20, // launch_speed
20, // slow_speed
35, // recycle_time
4, // ammunition
0, // is_bullet
50, // reload_time
-1, // damage
0, // scatter
10, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Flakker",
"FLAK",
30, // launch_speed
15, // slow_speed
12, // recycle_time
8, // ammunition
1, // is_bullet
70, // reload_time
1, // damage
50, // scatter
80, // recoil
40, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Scattergun",
"SCGUN",
50, // launch_speed
17, // slow_speed
8, // recycle_time
12, // ammunition
1, // is_bullet
35, // reload_time
50, // damage
0, // scatter
20, // recoil
6 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Blunderbuss",
"BLUN",
35, // launch_speed
17, // slow_speed
60, // recycle_time
2, // ammunition
1, // is_bullet
80, // reload_time
50, // damage
0, // scatter
90, // recoil
6, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Autocannon",
"AUTOC",
35, // launch_speed
13, // slow_speed
8, // recycle_time
10, // ammunition
1, // is_bullet
90, // reload_time
1, // damage
90, // scatter
40, // recoil
40, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Flamethrower",
"FLTHR",
13, // launch_speed
13, // slow_speed
2, // recycle_time
40, // ammunition
0, // is_bullet
50, // reload_time
120, // damage
60, // scatter
6, // recoil
5 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Firebomb",
"FBOMB",
15, // launch_speed
15, // slow_speed
70, // recycle_time
4, // ammunition
0, // is_bullet
70, // reload_time
0, // damage
0, // scatter
10, // recoil
30, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Plinker",
"PLINK",
30, // launch_speed
30, // slow_speed
30, // recycle_time
7, // ammunition
0, // is_bullet
50, // reload_time
1, // damage
10, // scatter
40, // recoil
10 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Nibbler",
"NIBB",
100, // launch_speed
23, // slow_speed
2, // recycle_time
50, // ammunition
1, // is_bullet
40, // reload_time
50, // damage
30, // scatter
5, // recoil
5 // bullet_weight
,WPNCLASS_WEAPON
}
,
{
"Remote Rocket",
"REM-R",
7, // launch_speed
7, // slow_speed
35, // recycle_time
4, // ammunition
0, // is_bullet
50, // reload_time
1, // damage
0, // scatter
10, // recoil
50 // bullet_weight
,WPNCLASS_WEAK
}
,
{
"C-Remote Rocket",
"C-REM-R",
7, // launch_speed
7, // slow_speed
35, // recycle_time
3, // ammunition
0, // is_bullet
100, // reload_time
1, // damage
0, // scatter
10, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON
}
,
{
"Semi-Automatic",
"SEMI",
80, // launch_speed
20, // slow_speed
16, // recycle_time
30, // ammunition
1, // is_bullet
25, // reload_time
200, // damage
20, // scatter
20, // recoil
20 // bullet_weight
,WPNCLASS_WEAK

}
,
{
"Laser Beam",
"L-BEAM",
100, // launch_speed
100, // slow_speed
0, // recycle_time
100, // ammunition
0, // is_bullet
70, // reload_time
40, // damage
0, // scatter
0, // recoil
1 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Laser Pulse",
"L-PULSE",
100, // launch_speed
100, // slow_speed
15, // recycle_time
10, // ammunition
0, // is_bullet
70, // reload_time
300, // damage
0, // scatter
0, // recoil
1 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Laser Tracer", // should never exist
"",
7, // launch_speed
7, // slow_speed
35, // recycle_time
6, // ammunition
0, // is_bullet
50, // reload_time
100, // damage
0, // scatter
10, // recoil
50, // bullet_weight
WPNCLASS_NONE
}
,
{
"Tracker",
"TRACK",
7, // launch_speed
7, // slow_speed
35, // recycle_time
3, // ammunition
0, // is_bullet
70, // reload_time
1, // damage
0, // scatter
10, // recoil
50, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Hunter-Seeker",
"HUNT-SEEK",
20, // launch_speed
20, // slow_speed
30, // recycle_time
4, // ammunition
0, // is_bullet
80, // reload_time
-1, // damage
0, // scatter
10, // recoil
5 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Funky Bomb",
"FUNKY",
15, // launch_speed
15, // slow_speed
70, // recycle_time
1, // ammunition
0, // is_bullet
80, // reload_time
0, // damage
0, // scatter
50, // recoil
80 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Disrupter",
"DISR",
25, // launch_speed
25, // slow_speed
40, // recycle_time
4, // ammunition
0, // is_bullet
80, // reload_time
0, // damage
0, // scatter
50, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Toxin Grenade",
"TOXIN",
20, // launch_speed
20, // slow_speed
50, // recycle_time
3, // ammunition
0, // is_bullet
80, // reload_time
-1, // damage
0, // scatter
10, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Bomb",
"BOMB",
20, // launch_speed
20, // slow_speed
35, // recycle_time
4, // ammunition
0, // is_bullet
60, // reload_time
1, // damage
0, // scatter
10, // recoil
30 // bullet_weight
,WPNCLASS_WEAK

}
,
{
"Grapeshot",
"GRAPE",
50, // launch_speed
17, // slow_speed
30, // recycle_time
10, // ammunition
1, // is_bullet
35, // reload_time
40, // damage
0, // scatter
10, // recoil
4 // bullet_weight
,WPNCLASS_WEAK

}
,
{
"RPG",
"RPG",
5, // launch_speed
5, // slow_speed
35, // recycle_time
4, // ammunition
0, // is_bullet
50, // reload_time
1, // damage
0, // scatter
5, // recoil
40 // bullet_weight
,WPNCLASS_WEAK

}
,
{
"Needler",
"NEED",
90, // launch_speed
20, // slow_speed
3, // recycle_time
15, // ammunition
1, // is_bullet
40, // reload_time
45, // damage
50, // scatter
5, // recoil
5 // bullet_weight
,WPNCLASS_WEAK
}
,
{
"Bouncy Bomb",
"BOUNCY",
10, // launch_speed
10, // slow_speed
50, // recycle_time
3, // ammunition
0, // is_bullet
60, // reload_time
1, // damage
0, // scatter
10, // recoil
40 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Prongthrower",
"PRTHROW",
20, // launch_speed
20, // slow_speed
4, // recycle_time
12, // ammunition
0, // is_bullet
80, // reload_time
200, // damage
70, // scatter
40, // recoil
20 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Gas Grenade",
"GASG",
20, // launch_speed
20, // slow_speed
50, // recycle_time
4, // ammunition
0, // is_bullet
75, // reload_time
-1, // damage
0, // scatter
10, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Clod of Dirt",
"CLOD",
30, // launch_speed
30, // slow_speed
4, // recycle_time
20, // ammunition
0, // is_bullet
50, // reload_time
0, // damage
10, // scatter
10, // recoil
20 // bullet_weight
,WPNCLASS_WEAK

}
,
{
"Dirt Bomb",
"DIRT-B",
20, // launch_speed
20, // slow_speed
20, // recycle_time
8, // ammunition
0, // is_bullet
50, // reload_time
-1, // damage
0, // scatter
10, // recoil
40 // bullet_weight
,WPNCLASS_WEAK

}
,
{
"Implosion Device",
"IMPL",
30, // launch_speed
30, // slow_speed
20, // recycle_time
2, // ammunition
0, // is_bullet
50, // reload_time
1, // damage
0, // scatter
100, // recoil
40 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Seeker",
"SEEK",
7, // launch_speed
7, // slow_speed
35, // recycle_time
3, // ammunition
0, // is_bullet
70, // reload_time
1, // damage
0, // scatter
10, // recoil
50, // bullet_weight
WPNCLASS_WEAPON

}
,
{
"Squirm Rockets",
"SQRM",
5, // launch_speed
5, // slow_speed
5, // recycle_time
15, // ammunition
0, // is_bullet
100, // reload_time
1, // damage
120, // scatter
5, // recoil
30 // bullet_weight
,WPNCLASS_WEAPON
}
,


{
"Fumigator",
"FUMIG",
13, // launch_speed
13, // slow_speed
2, // recycle_time
30, // ammunition
0, // is_bullet
50, // reload_time
100, // damage
60, // scatter
4, // recoil
5 // bullet_weight
,WPNCLASS_WEAPON

},

{
"Napalm Gun",
"NAP-G",
13, // launch_speed
13, // slow_speed
2, // recycle_time
15, // ammunition
0, // is_bullet
70, // reload_time
100, // damage
60, // scatter
4, // recoil
5 // bullet_weight
,WPNCLASS_WEAPON

},

{
"Fireball",
"FBALL",
20, // launch_speed
20, // slow_speed
20, // recycle_time
4, // ammunition
0, // is_bullet
100, // reload_time
10, // damage
0, // scatter
20, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

},

{
"Incendiaries",
"INCEND",
80, // launch_speed
20, // slow_speed
5, // recycle_time
18, // ammunition
1, // is_bullet
65, // reload_time
30, // damage
30, // scatter
10, // recoil
10 // bullet_weight
,WPNCLASS_WEAPON

},


{
"Shredder",
"SHRED",
10, // launch_speed
10, // slow_speed
20, // recycle_time
4, // ammunition
0, // is_bullet
60, // reload_time
150, // damage
0, // scatter
20, // recoil
6 // bullet_weight
,WPNCLASS_WEAPON

},
{
"Disrupter Wave",
"DIS-W",
25, // launch_speed
25, // slow_speed
40, // recycle_time
3, // ammunition
0, // is_bullet
80, // reload_time
-1, // damage
0, // scatter
50, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Smiter",
"SMIT",
1000, // launch_speed
1000, // slow_speed
40, // recycle_time
4, // ammunition
0, // is_bullet
80, // reload_time
1, // damage
0, // scatter
50, // recoil
50 // bullet_weight
,WPNCLASS_WEAPON

}
,
{
"Cube Rifle",
"CUBE",
30, // launch_speed
30, // slow_speed
8, // recycle_time
12, // ammunition
0, // is_bullet
50, // reload_time
1, // damage
30, // scatter
5, // recoil
10 // bullet_weight
,WPNCLASS_WEAPON
}
,
{
"Lightning Gun",
"LGHTN",
80, // launch_speed
80, // slow_speed
20, // recycle_time
8, // ammunition
0, // is_bullet
70, // reload_time
350, // damage
30, // scatter
10, // recoil
10 // bullet_weight
,WPNCLASS_WEAPON
}



};





