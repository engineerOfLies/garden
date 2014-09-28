#ifndef _ENTITY_
#define _ENTITY_

#include "graphics.h"
#include "audio.h"
#include "map.h"


/*
  This file contains the data structures and function prototypes for handling entities.
  Entities all contain information about the associate sprite that may be unique to itself
  as well as the animation information, if it has any.
  
*/

#define MAXPATH       20
#define MAXTRAIL      8
#define QMAX          10
#define MAXENTITIES   400
#define MAXSTATES     10
#define SOUNDSPERENT  4
#define PERSISTENCE   8

enum ENTTYPES {ET_Temp,ET_Unit, ET_Building, ET_World};
enum STATES   {ST_IDLE,ST_WALK, ST_DIE, ST_DEAD,ST_ATTACK1,ST_ATTACK2,ST_ATTACK3};
enum FACE     {F_South, F_SW, F_West, F_NW,F_North, F_NE, F_East, F_SE};
enum ORDERS   {OR_NONE,OR_WALK,OR_WATTACK,OR_HALT,OR_GATHER,OR_BUILD,OR_REPAIR,OR_DEFEND,OR_RALLEY,OR_FOLLOW};

#define ST_BUILD    ST_ATTACK2    /*these are the exceptions*/
#define ST_GATHER   ST_ATTACK3
#define ST_CHOP     ST_ATTACK3 + 1

typedef struct BUILDING_ENT
{
  int Queue[QMAX][3]; /*0 = id of product,1 = timeleft,2 = used*/
  int Qlen;           /*how many things are in the Q*/
  int Qtarget;        /*which item in the Q is being worked on*/
  Coord Ralley;       /*point to ralley units around*/
}BuildInfo;

typedef struct UNIT_ENT
{
  int magic, magicmax;
  Coord WalkTarget;   /*the coordinate of the screen that I am walking to*/
  Coord GoalTarget;   /*the end goal for the path*/
  Coord PathList[MAXPATH];
  Coord Trail[MAXTRAIL];      /*used for tracking and for making sure we don't get stuck anywhere*/
  Uint8 moving;       /*a boolean for weather or not I am moving: used for following purposes*/
  Uint8 failcount;
  Uint8 wait;
  int attack;         /*the rate at which I hit*/
  int damage;         /*the ammount of damage I deal*/
  int defense;        /*the rate at which I dodge*/
  int armor;          /*the ammount of damage I soak*/
  int seen;           /*if I am not seen, I won't be rendered for some players, and I will be translucent for my player*/
  int Orders;         /*what I was told to do*/
  int SubOrders;      /*sometimes there is more info needed, such as build X or Gather X or Defend Y*/
}UnitInfo;

typedef struct ENTITY_T
{
  int used; /*used is for keeping track of what entities are free and should NEVER be touched.*/
  Uint32 LastDrawn; /*another one that should never be touched by anything but the maintainence functions*/
  int Player; /*references the player controlling the unit, NUMPLAYERS implies it is owned by the 'world'*/
  int Unit_Type; /*resource, building, unit.*/
  int selectoffsety;
  char EntName[20]; /*the name of the unit or building*/
  Sprite *sprite;
  Sound *sound[SOUNDSPERENT];
  Uint32 Color;       /*most things will provide color through the sprite*/
  struct ENTITY_T *owner;
  struct ENTITY_T *target;    /*used for MANY units: attack target, healing target, etc*/
  void (*think) (struct ENTITY_T *self);
  void (*update) (struct ENTITY_T *self);
  void (*inputcommand) (struct ENTITY_T *self, int order); /*keeps track of animation rate*/
  Sprite *head;
  int headframe;
  int headstate;
  int headlayer;    /*weather it is in front of or behind the main sprite*/
  Coord Hs;       /*coordinates to draw the head at*/
  /*the think function is the function called each frame to update the state of the entity*/
  Uint32 NextThink; /*used for how often the entity thinks*/
  Uint16 ThinkRate;  /*used for incrementing above*/
  Uint32 NextUpdate; /*used for how often the entity is updated, updating is merely animations*/
  Uint16 UpdateRate;  /*used for incrementing above*/
  int shown; /*if 1 then it will be rendered when it is on screen*/
  int frame; /*current frame to render*/
  int fcount;  /*used for animation, the loop variable*/
  Uint32 framedelay;
  int face; /*the direction we are facing, yaw as it were*/
  int state;  /*making each entity a finite state machine.*/
  /*the absolute screen coordinates and the map coordinates*/
  Coord s; /*sprite's map pixel coordinates*/
  Coord v;  /*vector values*/
  Coord m;  /*Map coordinate*/
  int busy;   /*used for some think functions when issuing a command, if its busy with an animation it won't bother the sprite*/
  /*an entity can have up to MAXSTATES states.  Each state shows the fram it Ends on, the previous state marks
    the begining.*/
  int framestates[MAXSTATES];
  int Commands[12]; /*the different commands that can be issued to the unit*/
  int sightrange;  /*how bug my sigh radius is*/
  Coord tilesize;/*the width and height in tiles taken up, so determine how many map pointers point to me.*/
  int movespeed;
  int level;
  int sick;
  int mojo;
  int health,healthmax;/*everything will need health.  Resources will use it for how much of a resource it has left*/
  /*Beyond are all pointers to specific info.  Most Ents won't have more than one.  resources may not have any.*/
  BuildInfo *Building;
  UnitInfo  *Unit;
}Entity;

/*startup and clean up*/
void InitEntityList();
void ClearEntities();

/*creation and destruction*/
Entity *NewEntity(int Player);
void FreeEntity(Entity *ent);

Entity *SpawnUnitEntity(int Player,int mx,int my);
Entity *NewBuildingEntity(int Player,int mx,int my);

/*update functions*/
void DrawEntity(Entity *ent);
void DrawEntities();
void UpdateEntities();
void ThinkEntities();


/*generic entity support functions*/
int LinedUpOnMX(Entity *self);
void GravitateToMX(Entity *self);
void SetSXfromMX(Entity *self);
void MoveTo(Entity *self, int tx, int ty);
int MoveUnit(Entity *self);
void EntitySeeing(int mx,int my,int sight,int Player);
void EntitySight(int mx,int my,int sight,int Player);
int GetNextCount(Entity *self);
void ScaleVectors(float *vx, float *vy);
int Collide(SDL_Rect box1,SDL_Rect box2);

/*specific entity creation defined in their own C files*/
void SpawnTree(int mx,int my,int lumber);
void SpawnStone(int sx,int sy,int stone);
void SpawnGold(int mx,int my,int stone);

Entity *SpawnTestPeon(int mx, int my,int Player,int shown);/*at the map coordinates of mx, my and for the Player*/
void SpawnCenter(int mx, int my, int Player, int condition);
/*Add something to the building Queue*/
void BuildingQueue(Entity *self,int build);
#endif
