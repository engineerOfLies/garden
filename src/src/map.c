#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "path.h"
#include "graphics.h"
#include "entity.h"

extern SDL_Surface *background;
extern SDL_Surface *screen;
extern SDL_Surface *fogofwar;
extern SDL_Surface *unseenmask;
extern ScreenData  S_Data;
extern Uint32 rmask,gmask,bmask,amask;
extern SDL_Rect Camera;
extern int ThisPlayer;
extern Entity *EntityGroundList[MAP_H][MAP_W];
extern Entity DEADSPACE;
Map GameMap;

void LoadContent()
{
  GameMap.FOG = LoadSprite("images/fog.png",32,32);
  SDL_SetAlpha(GameMap.FOG->image,SDL_SRCALPHA,128);
  switch(GameMap.map.tileset)
  {
    case 0:
      GameMap.textures = LoadSprite("images/textures.png",32,32);
      /*GameMap.decals = LoadSprite("images/decals.png",64,64); does not exist*/
    break;
  }
}

void LoadMap(char filename[30])
{
  FILE *file;
  file = fopen(filename, "rb");
  if(file == NULL)
  {
    fprintf(stderr, "Unable to open map file: %s\n", SDL_GetError());
    exit(1);
  }
  if(fread(&GameMap.map, sizeof(MapInfo),1, file) != 1)
  {
    fprintf(stderr, "Unable to read map file: %s\n", SDL_GetError());
    exit(1);
  }
  fclose(file);
}

void LoadOldMap(char filename[30])
{
  FILE *file;
  MapOldInfo oldmap;
  file = fopen(filename, "rb");
  if(file == NULL)
  {
    fprintf(stderr, "Unable to open map file: %s\n", SDL_GetError());
    exit(1);
  }
  if(fread(&oldmap, sizeof(MapOldInfo),1, file) != 1)
  {
    fprintf(stderr, "Unable to read map file: %s\n", SDL_GetError());
    exit(1);
  }
  fclose(file);
  fprintf(stdout, "I got this far\n");
  strcpy(GameMap.filename,filename);
  memcpy(&GameMap.map.tile,&oldmap.tile,sizeof(oldmap.tile));
  GameMap.map.h = oldmap.h;
  GameMap.map.w = oldmap.w;
  GameMap.map.tileset = oldmap.tileset;
}

void SaveMap(char filename[30])
{
  FILE *file;
  file = fopen(filename, "wb");
  if(file == NULL)
  {
    fprintf(stderr, "Unable to create map file: %s\n", SDL_GetError());
    exit(1);
  }
  if(fwrite(&GameMap.map, sizeof(MapInfo),1, file) != 1)
  {
    fprintf(stderr, "Unable to write map file: %s\n", SDL_GetError());
    exit(1);
  }
  fclose(file);
}

void NewMap(int h, int w,int tileset,int base)
{
  int i,j;
  GameMap.map.h = h;
  GameMap.map.w = w;
  for(j = 0;j < h;j++)                                                     
  {
    for(i = 0; i < w;i++)
    {
      if((rand()%100))
      {
        GameMap.map.tile[j][i] = 0;
      }
      else GameMap.map.tile[j][i] = rand()%2 + 1;
    }
  }
  GameMap.map.tileset = tileset;
  GameMap.FOG = LoadSprite("images/fog.png",32,32);
  //SDL_SetAlpha(GameMap.FOG->image,SDL_SRCALPHA,128);
  switch(GameMap.map.tileset)
  {
    case 0:
      GameMap.textures = LoadSprite("images/textures.png",32,32);
      /*GameMap.decals = LoadSprite("images/decals.png",64,64); does not exist*/
    break;
  }
}

void DrawFog()
{
  //SDL_BlitSurface(fogofwar,&Camera,screen,NULL);
  SDL_BlitSurface(unseenmask,&Camera,screen,NULL);
}

void DrawFog2()
{
  int i,j;
  int x = Camera.x / 32 - 1;
  int y = Camera.y / 32 - 1;
  int w = Camera.w / 32 + 2;
  int h = Camera.h / 32 + 2;
  for(j = y;j <= y + h;j++)
  {
    for(i = x; i <= x + w;i++)
    {
     if(!GameMap.playerviews[ThisPlayer].seeing[j][i])
      {
        if(!GameMap.playerviews[ThisPlayer].seen[j][i])
        {
          DrawSprite(GameMap.textures,screen,(i *32) - Camera.x,(j*32) - Camera.y,63);
        }
        else
        {
          DrawSprite(GameMap.FOG,screen,(i *32) - Camera.x,(j*32) - Camera.y,0);
        }
      }
    }
  }
}

void DrawMap()
{
  int x,y,k;
  SDL_Surface *temp;
  if(background != NULL)
    SDL_FreeSurface(background);  /*lets just make sure that the background buffer is removed from memory in case we are RE-drawing a map.*/
  temp = SDL_CreateRGBSurface(SDL_HWSURFACE, 32 * GameMap.map.w, 32 * GameMap.map.h, S_Data.depth,rmask, gmask,bmask,amask);
  if(temp == NULL)            
  {
    fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
    exit(1);
  }
  /* Just to make sure that the surface we create is compatible with the screen*/
  background = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  if(unseenmask != NULL)
    SDL_FreeSurface(unseenmask);  /*lets just make sure that the background buffer is removed from memory in case we are RE-drawing a map.*/
  temp = SDL_CreateRGBSurface(SDL_SRCCOLORKEY | SDL_HWSURFACE, 32 * GameMap.map.w, 32 * GameMap.map.h, S_Data.depth,rmask, gmask,bmask,amask);
  if(temp == NULL)            
  {
    fprintf(stderr,"Couldn't initialize unseen mask buffer: %s\n", SDL_GetError());
    exit(1);
  }
  /* Just to make sure that the surface we create is compatible with the screen*/
  unseenmask = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  if(fogofwar != NULL)
    SDL_FreeSurface(fogofwar);  /*lets just make sure that the background buffer is removed from memory in case we are RE-drawing a map.*/
  temp = SDL_CreateRGBSurface(SDL_SRCALPHA | SDL_HWSURFACE, 32 * GameMap.map.w, 32 * GameMap.map.h, S_Data.depth,rmask, gmask,bmask,amask);
  if(temp == NULL)            
  {
    fprintf(stderr,"Couldn't initialize fog of war buffer: %s\n", SDL_GetError());
    exit(1);
  }
  /* Just to make sure that the surface we create is compatible with the screen*/
  fogofwar = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  BlankScreen(unseenmask,IndexColor(White));
  SDL_SetColorKey(unseenmask, SDL_SRCCOLORKEY , SDL_MapRGB(unseenmask->format, 255,255,255));
  for(y = 0;y < GameMap.map.h;y++)
  {
    for(x = 0;x < GameMap.map.w;x++)
    {
      DrawSprite(GameMap.textures,background,x * 32,y * 32, GameMap.map.tile[y][x]);
      DrawSprite(GameMap.textures,unseenmask,x * 32,y * 32, 63);
      for(k = 0; k < NUMPLAYERS;k++)
      {
        GameMap.playerviews[k].seen[y][x] = 0;
        GameMap.playerviews[k].seeing[y][x] = 0;
        GameMap.playerviews[k].OldSeen[y][x] = 0;
        GameMap.playerviews[k].OldSeeing[y][x] = 0;
      }
      if(TileBlock(GameMap.map.tile[y][x]))
      {
         EntityGroundList[y][x] = &DEADSPACE;
      }
    }
  }
  /*when we are done drawing the background...*/
  SDL_BlitSurface(background,NULL,fogofwar,NULL);
}

void ChangeTile(int tile,int sx,int sy)
{
  GameMap.map.tile[sy][sx] = tile;
  DrawSprite(GameMap.textures,background,sx * 32,sy * 32, GameMap.map.tile[sy][sx]);
}

void AddDecal(int decal,int sx,int sy)
{
  
}

void AddInfo(int info,int sx,int sy,int item1,int item2,int item3)
{
  
}

void AddObject(int object,int sx,int sy)
{
  
}


