#ifndef __MAP__
#define __MAP__
/***************************************************************************
                          map.h  -  description
                             -------------------
    begin                : Sun Mar 6 2005
    copyright            : (C) 2005 by djkehoe
    email                : dj_kehoe@yahoo.com
 ***************************************************************************/
#include "graphics.h"

#define MAP_W           256
#define MAP_H           256
#define NUMPLAYERS      8
#define NUMOBJECTS      256
#define NUMDECALS       32
#define NUMINFO         64
#define PATH_REZ        4
#define PATHSIZE        MAP_H/PATH_REZ * MAP_W/PATH_REZ
#define P_GOAL          0
#define P_DISTANCE      1
                        /*the array of tiles linearly*/
typedef struct POINT /*this is a very simple coordinate*/
{
  int x,y;
}Coord;

typedef struct MAP_OLD_T   /*this is the actual map information*/
{
  char mapname[30];             /*the map's descriptive name*/
  Uint16  tile[MAP_H][MAP_W];   /*the actual map information*/
  Uint16  w,h;                  /*the width and height for the map*/
  Uint8   tileset;              /*referencing the different textures and decals that will be loaded*/
  Uint8   objects[NUMOBJECTS][3]; /*0 = decal frame number, 1 = x coord, 2 = y coord, Map entities*/
  Uint8   decals[NUMDECALS][3];  /*0 = decal frame number, 1 = x coord, 2 = y coord*/
  Uint8   infopoints[NUMINFO][5];/*this will be the same as above, but the info points won't be rendered.
                                     They will be things like starting locations for units and buildings as
                                     well as some of their stats (team, health,upgrades,etc)*/
}MapOldInfo;

typedef struct MAP_INFO_T   /*this is the actual map information*/
{
  char mapname[30];             /*the map's descriptive name*/
  Uint16  tile[MAP_H][MAP_W];   /*the actual map information*/
  Sint8   path[PATHSIZE][PATHSIZE][2];   /*the node list for each node, the next waypoint and distance from*/
  Uint16  w,h;                  /*the width and height for the map*/
  Uint8   tileset;              /*referencing the different textures and decals that will be loaded*/
  Uint8   objects[NUMOBJECTS][3]; /*0 = decal frame number, 1 = x coord, 2 = y coord, Map entities*/
  Uint8   decals[NUMDECALS][3];  /*0 = decal frame number, 1 = x coord, 2 = y coord*/
  Uint8   infopoints[NUMINFO][5];/*this will be the same as above, but the info points won't be rendered.
                                     They will be things like starting locations for units and buildings as
                                     well as some of their stats (team, health,upgrades,etc)*/
}MapInfo;

typedef struct VIEW_T     /*The mask for what the players can see and cannot see*/
{
  Uint8 seen[MAP_H][MAP_W];
  Uint8 seeing[MAP_H][MAP_W];
  Uint8 OldSeen[MAP_H][MAP_W];      /*this is kept track of so I don't have to redraw everything every frame*/
  Uint8 OldSeeing[MAP_H][MAP_W];    /*same here*/
}ViewMap;

typedef struct MAP_T  /*this is the information needed to render the map, that won't be needed in the saved file*/
{
  char filename[30];
  MapInfo map;
  Sprite *FOG;
  Sprite *textures;
  Sprite *decals;
  ViewMap playerviews[NUMPLAYERS];
  float wx, wy;   /*wind vector.  It may only be used for particles and perhaps some weather changing puzzles.*/
}Map;

/*these are the functions for loading and displaying maps.*/
void LoadMap(char filename[30]);
void LoadContent();
void LoadOldMap(char filename[30]);
void DrawMap();
void DrawFog();
void DrawFog2();
void ResetFog();/*this is going to reset the fog every so often and then check to see whats supposed to be seen*/

/*These are the functions for making maps.*/
void NewMap(int h, int w,int tileset,int base);
void SaveMap(char filename[30]);
void ChangeTile(int tile,int sx,int sy);
void AddDecal(int decal,int sx,int sy);
void AddInfo(int info,int sx,int sy,int item1,int item2,int item3);
void AddObject(int object,int sx,int sy);
int TileBlock(Uint16 tile);

#endif
