#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "graphics.h"
#include "map.h"
#include "path.h"
#include "command.h"

#define MAXSTATE 1
extern SDL_Surface *screen;
extern SDL_Surface *background;
extern SDL_Rect Camera;

void Init_All();
int QuitCheck();

int main(int argc, char *argv[])
{
  char time[20];
  int done = 0;
  Uint32 timetook;
  Init_All();
  do
  {
    ResetBuffer();
    SDL_PumpEvents();
    MapMouseThink();
    done = MapDashThink();
    if(!done)done = QuitCheck();
    MapDrawInterface();
    DrawMouse();
    DrawMessages();
    NextFrame();
  }while(!done);
    
  exit(0);
  return 0;
}

int QuitCheck()
{
  int numkeys;
  Uint8 *keystate;
  keystate = SDL_GetKeyState(&numkeys);
  if((keystate[SDLK_F4])&&(SDL_GetModState()& KMOD_ALT))
  {
    return 1;
  }
  return 0;
}

void CleanUpAll()
{
  CloseSprites();
  /*any other cleanup functions can be added here*/ 
}

void Init_All()
{
  Init_Graphics();
  InitSpriteList();
  atexit(CleanUpAll);
  LoadFonts();
  InitMessages();
  LoadMouse(Magenta,Silver,Green);
  NewMap(64, 64,0,15);
  DrawMap();
  LoadInterface(1,Green);
}

