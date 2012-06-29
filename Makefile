CC     = gcc
CFLAGS = -g
ifeq ($(UNAME), Linux)
	LDFLAGS = -lalleg -lnet -lm
else
	LDFLAGS = -lallegro -L ./ -lnet -lm
endif


all: CaptPorksRevenge

CaptPorksRevenge: *.c
		$(CC) $(LDFLAGS) -o $@ $^

clean: FRC
		rm -f CaptPorksRevenge

install:
		mv ../builds/cat2012 /usr/bin/cat2012

# This pseudo target causes all targets that depend on FRC
# to be remade even in case a file with the name of the target exists.
# This works with any make implementation under the assumption that
# there is no file FRC in the current directory.
FRC: