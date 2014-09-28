#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "graphics.h"
#include "map.h"
#include "path.h"
#include "command.h"
#include "audio.h"
#include "entity.h"
#include "hero.h"

#define MAXSTATE 1
extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Surface *background;
extern SDL_Rect Camera;

void Init_All();
void Update_ALL();
int Think_ALL();
void Draw_ALL();
void DrawSplashScreen();

int main(int argc, char *argv[])
{
  int done = 0;
  int i,j;
  Uint32 Then;
  Sprite *trail1;
  Sprite *trail2;
  Sprite *trail3;
  Sprite *arrowed;
  Sprite *tree;
  Sprite *bolt;
  Sprite *Items;
  Sprite *Cliffs;
  Init_All();
  SpawnHeroEnt(3,10,1,"images/testpeonsmall.png",Brown,0,0);
  SpawnTestPeon(10, 10,0,1);
  SpawnCenter(4,2,0, 0);
  SpawnStone(1,1,1000);
  SpawnStone(15,15,1000);
  SpawnGold(15,20,1000);
  Cliffs = LoadSwappedSprite("images/magicblast.png",32,32,Silver,0,0);
  SDL_SetColorKey(Cliffs->image, SDL_SRCCOLORKEY , IndexColor(Black));
  
  arrowed = LoadSwappedSprite("images/arrow.png",32,32,Gold,0,0);
  SDL_SetColorKey(arrowed->image, SDL_SRCCOLORKEY , IndexColor(Black));

  trail1 = LoadSwappedSprite("images/magictrail.png",32,32,Blue,0,0);
  SDL_SetColorKey(trail1->image, SDL_SRCCOLORKEY , IndexColor(Black));
  SDL_SetAlpha(trail1->image, SDL_RLEACCEL|SDL_SRCALPHA, 250);
  
  trail2 = LoadSwappedSprite("images/magictrail.png",32,32,Grey,0,0);
  SDL_SetColorKey(trail2->image, SDL_SRCCOLORKEY , IndexColor(Black));
  SDL_SetAlpha(trail2->image, SDL_RLEACCEL|SDL_SRCALPHA, 250);

  trail3 = LoadSwappedSprite("images/magictrail.png",32,32,LightViolet,0,0);
  SDL_SetColorKey(trail3->image, SDL_SRCCOLORKEY , IndexColor(Black));
  SDL_SetAlpha(trail3->image, SDL_RLEACCEL|SDL_SRCALPHA, 250);

  bolt = LoadSwappedSprite("images/magicbolt.png",32,32,Violet,0,0);
  SDL_SetColorKey(bolt->image, SDL_SRCCOLORKEY , IndexColor(Black));
  
  tree = LoadSprite("images/trees.png",50,90);
  SDL_SetColorKey(tree->image, SDL_SRCCOLORKEY , IndexColor(Black));
  
  Items = LoadSwappedSprite("images/magicball.png",32,32,LightBlue,LightBlue,Brown);
  SDL_SetColorKey(Items->image, SDL_SRCCOLORKEY , IndexColor(Black));
//  fprintf(stdout,"time took to find path: %i \n",FindPath(10,10, 40,40));
  for(i = 0;i < 5;i ++)
    SpawnTree(18 + i , 8,100);
  i = 0;
  j = 7;
  Then = 0;
  do
  {
//          Then = SDL_GetTicks();
    ResetBuffer();
  //        fprintf(stdout,"time to do this: %i.\n",SDL_GetTicks() - Then);
    SDL_PumpEvents();
    Update_ALL();
    done = Think_ALL();
    ResetFog();
    Draw_ALL();
    DrawSprite(arrowed,screen,288,256,Then);
    DrawSprite(trail1,screen,256,128,Then);
    DrawSprite(Items,screen,256,128,i);
    DrawSprite(trail2,screen,128,256,(Then + 1)%8);
    DrawSprite(bolt,screen,128,256,i);
    DrawSprite(trail3,screen,256,256,(Then + 2)%8);
    DrawSprite(Cliffs,screen,256,256,i);
    j--;
    if(j < 0)
    {
      i = (i + 1)%4;
      j = 7;
      Then = (Then + 1)%8;
    }
    NextFrame();
  }while(!done);
    
  exit(0);
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
  InitEntityList();
  DrawSplashScreen();
  //LoadMap("maps/testmap2.tcm");
  LoadContent();
  NewMap(64, 64,0,15);
  DrawMap();
  LoadInterface(4,DarkViolet);
}

void DrawSplashScreen()
{
  SDL_Surface *splash;
  splash = IMG_Load("images/splashscreen.png");
  if(splash != NULL)
  {
    SDL_BlitSurface(splash,NULL,screen,NULL);
    NextFrame();
  }
  SDL_FreeSurface(splash);
}

/*calls all of the update functions for everything*/
void Update_ALL()
{
  UpdateEntities();
  
}

/*calls all of the think function for everything*/
int Think_ALL()
{
  int done = 0;
  Uint32 Then;
  MouseThink();/*mouse think should always come before dashthink*/
  done = DashThink();
    Then = SDL_GetTicks();
  ThinkEntities();
    fprintf(stdout,"time took this frame: %i \n",SDL_GetTicks() - Then);
  return done;
}

/*calls all of the draw functions for everything*/
void Draw_ALL()
{
  DrawSelectionRings();
  DrawEntities();
  DrawSelectionHealth();
  DrawFog();
  DrawMessages();
  DrawInterface();
  DrawMouse();
}

/*the necessary blank line at the end of the file:*/

