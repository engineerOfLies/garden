#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "graphics.h"
#include "map.h"
#include "path.h"
#include "command.h"
#include "audio.h"
#include "entity.h"
#include "hero.h"
#include "particle.h"
#include "game.h"

#define MAXSTATE 1
extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Entity *EntityGroundList[MAP_H][MAP_W];
extern Entity DEADSPACE;

Sound *gamesounds[4];
Game DeadGame;
Mix_Music *BgMusic = NULL;

void Init_All();
void Update_ALL();
int Think_ALL();
void Draw_ALL();
void DrawSplashScreen();
void GiveInfo();
void PopUpWindow(Sprite *sprite,char *text,Uint32 Color,int pwait);
    
int main(int argc, char *argv[])
{
  int done = 0;
  int i,sx,sy;
  Uint8 *keys;
  Init_All();
  gamesounds[0] = LoadSound("sounds/victory.wav",MIX_MAX_VOLUME/2);
  gamesounds[1] = LoadSound("sounds/failure.wav",MIX_MAX_VOLUME/2);
  GameOptions(&DeadGame,0);
  GiveInfo();
  do
  {
    ResetBuffer();
    SDL_PumpEvents();
    Update_ALL();
    Think_ALL();
    keys = SDL_GetKeyState(NULL);    
    if(keys[SDLK_ESCAPE] == 1)done = 1;
    if(keys[SDLK_F1] == 1)GameOptions(&DeadGame,1);
    Draw_ALL();
    if(keys[SDLK_b] == 1)SDL_SaveBMP(screen,"screen0.bmp");
   // ItsRaining(IndexColor(LightRed),10,200,-0.2);
    NextFrame();
  }while(!done);
    
  exit(0);
  return 0;
}

void CleanUpAll()
{
  CloseSprites();
  //ClearEntities();
  //ClearSoundList();
  /*any other cleanup functions can be added here*/ 
}

void Init_All()
{
  Init_Graphics();
  InitSpriteList();
  Init_Audio();
  InitSoundList();
  atexit(CleanUpAll);
  LoadFonts();
  InitMessages();
  LoadMouse(Cyan,Gold,DarkViolet);
  InitEntityList();
  ResetAllParticles();
  DrawSplashScreen();
  LoadContent();
  NewMap(32, 48,0,15);
  DrawMap();
}

void GiveInfo()
{
  NewMessage("Press Esc to Quit",IndexColor(White));
  NewMessage("Press F1 For Options Menu",IndexColor(White));
}

void PopUpWindow(Sprite *splash,char *text,Uint32 Color,int frame)
{
  Sprite *popup;
  Uint8 *keys;
  int len = strlen(text);
  popup = LoadSprite("images/popup.png",400,300);
  DrawSprite(popup,screen,(screen->w - 400)/2,(screen->h - 300)/2,0);
  if(splash != NULL)
    DrawSprite(splash,screen,(screen->w - splash->w)/2,(screen->h - splash->h)/2,frame);
  if(text != NULL)
    DrawText(text,screen,(screen->w - (len *11))/2 ,(screen->h - 300)/2 + 50,Color,F_Large);
  DrawText("Hit Spacebar to continue",screen,(screen->w)/2 - 136 ,(screen->h/2) + 100,Color,F_Large);
  NextFrame();
  FreeSprite(splash);
  FreeSprite(popup);
  do
  {
    SDL_PumpEvents();
    keys = SDL_GetKeyState(NULL);
  }
  while(keys[SDLK_SPACE] != 1);
}    

void DrawSplashScreen()
{
  SDL_Surface *splash;
  Uint8 *keys;
  splash = IMG_Load("images/welcome.png");
  if(splash != NULL)
  {
    SDL_BlitSurface(splash,NULL,screen,NULL);
    NextFrame();
  }
  SDL_FreeSurface(splash);
  do
  {
    SDL_PumpEvents();
    keys = SDL_GetKeyState(NULL);
  }
  while((SDL_GetTicks() < 5000)&&(keys[SDLK_SPACE] != 1));
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
  DrawAllParticles();
  DrawMessages();
  DrawMouse();
}

void NewGame(Game *game)
{
  game->level = 0;
  game->score = 0;
  game->NumZombies = 0;
  game->NumGraves = 0;
  game->NumVillagers = 0;  
  ClearEntities();
  InitEntityList();
  ResetAllParticles();
}

void NewLevel(Game *game, int graves,int villagers)  /*sets up a new level*/
{
  int i;
  int j;
  int width;
  int height;
  int sx,sy;
  if(BgMusic != NULL)
  {
    Mix_HaltMusic();
    Mix_FreeMusic(BgMusic);
  }
  BgMusic = Mix_LoadMUS("sounds/ambiant02.ogg");
  if(BgMusic == NULL)
  {
    fprintf(stderr, "Unable to Load Background Music: %s\n", SDL_GetError());
    exit(1);
  }
  Mix_VolumeMusic(MIX_MAX_VOLUME);
  Mix_PlayMusic(BgMusic,-1);/*play my background music infinitely*/
  game->level++;
  width = (int)sqrt(graves);
  if(width == 0)width = 1;
  height = graves / width;
  sx = 14 - (width/2);
  sy = 12 - (height/2);
  game->NumGraves =0;
  game->NumVillagers = villagers;  
  game->NumZombies = 0;
  ClearEntities();  /*lets get rid of what we ain't usin*/
  InitEntityList();
  ResetAllParticles();
  for(i = 0;i < 16;i++)
    SpawnTree((((rand()>>8) % 15)*2),(((rand()>>8) % 10)*2)+2, 200);
  for(j = 0;j < height;j++)
  {
    for(i = 0;i < width;i++)
    {
      SpawnCenter((i*2) + sx,(j*2)+ sy, 1,(rand()>>8)%2);
      game->NumGraves++;
    }
  }
  UpdateEntities();
}

void GameOptions(Game *game,int options)   /*new game, quit, select difficulty*/
{
  Uint8 *keys;
  Sprite *menu;
  int done = 0;
  menu = LoadSprite("images/menu.png",640,480);
  do
  {
    ResetBuffer();
    SDL_PumpEvents();
    MouseThink();
    keys = SDL_GetKeyState(NULL);    
    if(keys[SDLK_ESCAPE] == 1)
    {
        exit(0);
    }
    if(keys[SDLK_RETURN] == 1)
    {
      FreeSprite(menu);
      SDL_PumpEvents();
      return;
    }
    if(keys[SDLK_F2] == 1)
    {
      NewGame(game);
      NewLevel(game,8,4);  /*sets up a new level*/
      done = 1;
      FreeSprite(menu);
    }
    DrawMessages();
    DrawSprite(menu,screen,(screen->w - 640)/2,(screen->h - 480)/2,0);
    DrawText("New Game  : F2",screen,(screen->w - 640)/2 + 50,(screen->h - 480)/2 + 160,IndexColor(LightGreen),F_Medium);
    DrawText("Quit      : Escape",screen,(screen->w - 640)/2 + 50,(screen->h - 480)/2 + 200,IndexColor(LightGreen),F_Medium);
    if(options == 0)
    {
      DrawText("This Menu : F1",screen,(screen->w - 640)/2 + 50,(screen->h - 480)/2 + 180,IndexColor(LightGreen),F_Medium);
    }
    else
    {
      DrawText("Cancel    : Enter",screen,(screen->w - 640)/2 + 50,(screen->h - 480)/2 + 180,IndexColor(LightGreen),F_Medium);
      DrawText("Game Paused",screen,(screen->w / 2) - 80,(screen->h - 480)/2 + 440,IndexColor(LightRed),F_Small);
    }
    DrawMouse();
    NextFrame();
  }while(!done);
  
}

void DeadZombie(Game *game)  /*sets the game info to he beone less zombie.. possible game over.*/
{
  Sprite *grave;
  int frame;
  int channel;
  game->NumGraves--;
  if(game->NumGraves + game->NumZombies < game->NumVillagers)
  {
    if(rand()%2 == 1)
    {
      grave = LoadSprite("images/grave1.png",64,96);
      frame = 3;
    }
    else
    {
      grave = LoadSprite("images/skelrise.png",32,48);
      frame = 15;
    }
    Mix_HaltMusic();
    channel = Mix_PlayChannel(-1,gamesounds[1]->sound,0);
    PopUpWindow(grave,"The Villagers live, You LOSE!",IndexColor(Red),frame);
    GameOptions(game,0);
    Mix_HaltChannel(channel);
    FreeSprite(grave);
  }
  else if(game->NumGraves == 0)/*then we must have won already*/
  {
    grave = LoadSprite("images/levelup.png",96,96);
    Mix_HaltMusic();
    channel = Mix_PlayChannel(-1,gamesounds[0]->sound,0);
    PopUpWindow(grave,"The Villagers Die!!",IndexColor(LightViolet),0);
    NewLevel(game,(game->level * 4) + 8,(game->level * 3) + 2);  /*sets up a new level*/
    Mix_HaltChannel(channel);
    FreeSprite(grave);
  }
}

void GrownZombie(Game *game)
{
  Sprite *levelup;
  int channel;
  game->NumZombies++;
  game->NumGraves--;
  if((game->NumZombies >= game->NumVillagers)&&(game->NumGraves == 0))
  {
    Mix_HaltMusic();
    levelup = LoadSprite("images/levelup.png",96,96);
    channel = Mix_PlayChannel(-1,gamesounds[0]->sound,0);
    PopUpWindow(levelup,"The Villagers Die!!",IndexColor(LightViolet),0);
    NewLevel(game,(game->level * 4) + 8,(game->level * 3) + 2);  /*sets up a new level*/
    Mix_HaltChannel(channel);
    FreeSprite(levelup);
  }
}

/*the necessary blank line at the end of the file:*/

