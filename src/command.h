#ifndef __COMMAND__
#define __COMMAND__
/***************************************************************************
                          command.h  -  description
                             -------------------
    begin                : Thu Mar 17 2005
    copyright            : (C) 2005 by djkehoe
    email                : dj_kehoe@yahoo.com
    last modified        : 3/18/05
 ***************************************************************************/
#include "entity.h"

#define NUMCOMS    12
#define NUMSELECT  24

enum Commands {C_Empty = -1, C_Walk, C_Attack,C_Stop,  C_Gather, C_Build, C_Repair,C_Defend,
               BC_Ralley,BC_Peon};
enum RESOURCES {R_Wood,R_Stone,R_Gold,R_Food,R_Supply,R_SLimit};

typedef struct INTERFACE_T
{
  Sprite *Dash;
  Sprite *Buttons;
  Sprite *Resources;
  Uint32 resources[6];
  Uint16 ComColor;
  SDL_Rect minimap;
  int Command[NUMCOMS];
  Uint8 ButtonState[18];
  int Selection;
  int MapBrush;
  Entity *SelectedUnit[NUMSELECT];
  Uint8  NumberSelected;
}Interface;

typedef struct MOUSE_T
{
  Sprite *image;
  Sound *sounds[4];
  int frame;
  int state;
  int step;
  int buttons;
  int x,y;
  int startx,starty;
  int pressed;
}Mouse;

void LoadMouse(int c1,int c2,int c3);
void DrawMouse();
void ChangeMouse(int state);

void UpdateMiniMap(int x, int y);/*This little function won't be needed in the actual game, just int he map editor.*/
void LoadInterface(int style,Uint16 Color);
void DrawInterface();
void MapDrawInterface();
        /*using callbacks to get input form an active window*/
void GetString(  void (*callback) (char string[30]));

void MouseThink();
void MapMouseThink();
int DashThink();
int MapDashThink();

void SelectNone();
int IsSelected(Entity *ent);
Entity *GetClosestEnt(int gx,int gy);
void UnselectEntity(Entity *ent);
void AddSelectEntity(Entity *ent);
void SelectEntity(Entity *ent);
void DrawSelectionRings();
void DrawSelectionHealth();

#endif
