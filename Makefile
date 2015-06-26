CC     = gcc
CFLAGS = -Wall -O2
OS     = $(shell uname)

ifeq ($(OS), Linux)
	LDFLAGS = -lalleg -lm
else
	LDFLAGS = -L./ -lallegro -static-libgcc -lm
endif


all: CaptPorksRevenge

CaptPorksRevenge: actor.o ai.o base.o bullet.o chall.o cloud.o cmds.o connect.o display.o effects.o grid.o level.o light.o menu.o misc.o move.o palette.o pickup.o prand.o proj.o score.o sound.o weapon.o
		$(CC) $(LDFLAGS) -o $@ $^

clean: FRC
		ifeq ($(OS), Linux)
			rm -f CaptPorksRevenge
		else
			del CaptPorksRevenge.exe
		endif

#install:
#		mv CaptPorksRevenge /usr/bin/CaptPorksRevenge

# This pseudo target causes all targets that depend on FRC
# to be remade even in case a file with the name of the target exists.
# This works with any make implementation under the assumption that
# there is no file FRC in the current directory.
FRC:
