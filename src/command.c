#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"
#include "command.h"
#include "map.h"
#include "entity.h"
#include "particle.h"

#define Testing

extern SDL_Rect Camera;
extern SDL_Surface *screen;
extern SDL_Surface *background;
extern SDL_Surface *videobuffer;
extern Map GameMap;
extern int ThisPlayer;
extern Entity *EntityGroundList[MAP_H][MAP_W];/*Pointers to entities, by location*/
extern Entity *EntityAirList[MAP_H][MAP_W];/*Pointers to entities, by location int he air*/
extern Uint32 rmask,gmask,bmask,amask;
extern Entity DEADSPACE;
extern Uint32 NOW;

Interface DashBoard;
Mouse mouse;
SDL_Surface *minimap = NULL;
char hotkeys[12] = "qwerasdfzxcv";/*defaults for hot keys in the menu*/




void LoadMouse(int c1,int c2,int c3)
{
  mouse.image = LoadSwappedSprite("images/mouse.png",16,16,c1,c2,c3);
  mouse.sounds[0] = LoadSound("sounds/badmojo1.wav",MIX_MAX_VOLUME);
  mouse.sounds[1] = LoadSound("sounds/badmojo2.wav",MIX_MAX_VOLUME);
  mouse.sounds[2] = LoadSound("sounds/badmojo3.wav",MIX_MAX_VOLUME/2);
  mouse.frame = 0;                                                                                            
  mouse.step = 0;
  mouse.state = 0;  
}

void DrawMouse()
{
  mouse.step = (mouse.step+1)%16;
  mouse.frame = mouse.step + (mouse.state * 16);
  DrawSprite(mouse.image,screen,mouse.x,mouse.y,mouse.frame);
}


void MouseThink()
{
    int tx,ty,tw,th;
    int i,j;
    int moved = 0;
    Entity *Ent = NULL;
    tx = (mouse.x + Camera.x) / 32;
    ty = (mouse.y + Camera.y) / 32;
    mouse.buttons = SDL_GetMouseState(&mouse.x,&mouse.y);
    if((mouse.x < 10)&&(Camera.x > 0))
    {
      Camera.x-=20;
      if(Camera.x < 0) Camera.x = 0;
    }
    else if(mouse.x > screen->w - 10)
    {
      Camera.x+=20;
      if(Camera.x > background->w - screen->w) Camera.x = background->w - screen->w;
    }
    if((mouse.y < 10)&&(Camera.y > 0))
    {
      Camera.y-=20;
      if(Camera.y < 0) Camera.y = 0;
    }
    else if(mouse.y > screen->h - 10)
    {
      Camera.y+=20;
      if(Camera.y > background->h - screen->h) Camera.y = background->h - screen->h;
    }
    if((mouse.startx != mouse.x)||(mouse.starty != mouse.y))
    {
      moved = 1;
    }
    mouse.startx = mouse.x;
    mouse.starty = mouse.y;
    if(moved)
    {
      Ent = EntityGroundList[ty][tx];
      if(Ent != DashBoard.SelectedUnit[0])
      {
        DashBoard.SelectedUnit[0] = Ent;
        if((Ent != NULL)&&(Ent != &DEADSPACE)&&(Ent->Unit_Type == ET_Building))
        {
          DashBoard.NumberSelected = 1;
        }
        else DashBoard.NumberSelected = 0;
      }
    } 
    if(mouse.buttons&SDL_BUTTON(1))
    {
      if(mouse.pressed <= NOW)
      {  
        mouse.pressed = NOW + 120;
        if(DashBoard.NumberSelected == 1)
        {
          SpawnFire(IndexColor(LightGreen),DashBoard.SelectedUnit[0]->m.x,DashBoard.SelectedUnit[0]->m.y,20);
          DashBoard.SelectedUnit[0]->health += 15;
          Mix_PlayChannel(-1,mouse.sounds[0]->sound,0);
        }
      }
      else if(mouse.pressed <= NOW + 60)    /*we can add mojo more often than we make green fire.*/
      {
        if(DashBoard.NumberSelected == 1)
        {
          DashBoard.SelectedUnit[0]->health += 15;
        }
      }
    }
}


/*removes the Selected Entity from the list*/
void UnselectEntity(Entity *ent)
{
  int i,j;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  {
    if(ent == DashBoard.SelectedUnit[i])
    {
      for(j = i;j < DashBoard.NumberSelected - 1;j++)
      {
        DashBoard.SelectedUnit[j] = DashBoard.SelectedUnit[j + 1];
      }
      DashBoard.NumberSelected--;
      return;
    }
  }
}

/*finds the closest entity to the goal so I can make it the leader.*/

Entity *GetClosestEnt(int gx,int gy)
{
  int closest = 20000;
  Entity *Ent = NULL;
  int i,temp;
  for(i = 0; i < DashBoard.NumberSelected;i++)
  {
    temp = fabs(DashBoard.SelectedUnit[i]->m.x - gx) + fabs(DashBoard.SelectedUnit[i]->m.y - gy);
    if(temp < closest)
    {
      closest = temp;
      Ent = DashBoard.SelectedUnit[i];
    }
  }
  return Ent;
}

void AddSelectEntity(Entity *ent)
{
  int i;
  if(DashBoard.NumberSelected < NUMSELECT)
  {
    DashBoard.SelectedUnit[DashBoard.NumberSelected++] = ent;
    for(i = 0; i < NUMCOMS; i++)
    {
      DashBoard.Command[i] = ent->Commands[i];
    }
  }
}

int IsSelected(Entity *ent)
{
  int i;
  for(i = 0; i < DashBoard.NumberSelected; i++)
  {
    if(DashBoard.SelectedUnit[i] == ent)return 1;
  }
  return 0;
}

void SelectNone()
{
  int i;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  DashBoard.SelectedUnit[i] = NULL;
  DashBoard.NumberSelected = 0;
}

void SelectEntity(Entity *ent)
{
  int i;
  DashBoard.NumberSelected = 1;
  DashBoard.SelectedUnit[0] = ent;
  for(i = 0; i < NUMCOMS; i++)
  {
    DashBoard.Command[i] = ent->Commands[i];
  }
}

void DrawSelectionHealth()
{
  int i;
  int tx,ty,tw,th;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  {
    if(DashBoard.SelectedUnit[i]->Player == ThisPlayer)
    {
      DrawFilledRect(DashBoard.SelectedUnit[i]->s.x - Camera.x,DashBoard.SelectedUnit[i]->s.y - Camera.y,DashBoard.SelectedUnit[i]->sprite->w, 2,IndexColor(Red),screen);
      DrawFilledRect(DashBoard.SelectedUnit[i]->s.x - Camera.x,DashBoard.SelectedUnit[i]->s.y - Camera.y,(DashBoard.SelectedUnit[i]->health * DashBoard.SelectedUnit[i]->sprite->w)/DashBoard.SelectedUnit[i]->healthmax, 2,IndexColor(Green),screen);
    }
  }
  if(mouse.pressed)
  {
    if(mouse.x < mouse.startx)tx = mouse.x;
     else tx = mouse.startx;
    if(mouse.y < mouse.starty)ty = mouse.y;
     else ty = mouse.starty;
    tw = fabs(mouse.x - mouse.startx);
    th = fabs(mouse.y - mouse.starty);
    DrawRect(tx,ty,tw,th, IndexColor(LightBlue), screen);
  }
}

void DrawSelectionRings()
{
  int i;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  {
    DrawElipse(DashBoard.SelectedUnit[i]->s.x + DashBoard.SelectedUnit[i]->sprite->w/2 - Camera.x,DashBoard.SelectedUnit[i]->s.y + DashBoard.SelectedUnit[i]->sprite->h  + DashBoard.SelectedUnit[i]->selectoffsety - Camera.y,DashBoard.SelectedUnit[i]->sprite->w / 2 , IndexColor(LightBlue), screen);
  }
}

void ChangeMouse(int state)
{
  mouse.state = state;/*this will get more advanced in the future*/
}



