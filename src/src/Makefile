##############################################################################
#
# The Linux-GCC Makefile
#
##############################################################################

#
# Object files.
#

OBJ = game.o graphics.o map.o path.o font.o entity.o command.o audio.o peon.o center.o worldents.o hero.o heroent.c

#
# Compiler stuff -- adjust to your system.
#

# Linux

CC     = gcc
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer
LFLAGS = -g -o quest
CFLAGS = -g -Wall -pedantic

#
# Targets.
#

quest: $(OBJ)
	gcc $(OBJ) $(LFLAGS) $(SDL_LDFLAGS)
  
depend:
	@-rm makefile.dep
	@echo Creating dependencies.
	@for i in *.c; do $(CC) $(INC) -MM $$i; done > makefile.dep
	@echo Done.

clean:
	rm *.o quest

count:
	wc *.c *.h makefile

#
# Dependencies.
#

include makefile.dep

#
# The default rule.
#

.c.o:
	gcc $(CFLAGS) $(SDL_CFLAGS) -c $<

