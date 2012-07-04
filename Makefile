CC     = gcc
CFLAGS = -Wall -O2
OS     = $(shell uname)

ifeq ($(OS), Linux)
	LDFLAGS = -lalleg -lnet -lm
else
	LDFLAGS = -L ./ -lallegro-4.4.2-monolith-md-debug -lnet -lm
endif


all: CaptPorksRevenge

CaptPorksRevenge: *.c
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
