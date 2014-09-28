#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "entity.h"
#include "command.h"

extern Entity *EntityGroundList[MAP_H][MAP_W];
extern Interface DashBoard;

void PeonUpdate(Entity *self);
void PeonThink(Entity *self);
void PeonCommand(Entity *self, int order);

void SpawnTestPeon(int mx, int my,int Player,int shown)
{
  int i;
  Entity *peon;
  peon = SpawnUnitEntity(Player,mx,my);
  if(peon == NULL)return;
  peon->Unit_Type = ET_Unit;
  peon->sprite = LoadSwappedSprite("images/testpeonsmall.png",32,32,DashBoard.ComColor,0,0);
  strcpy(peon->EntName,"Generic Peon");
  peon->selectoffsety = -8;
  peon->shown = shown;
  SetSXfromMX(peon);
  peon->movespeed = 6;
  peon->frame = 0;
  peon->UpdateRate = 80;
  peon->update = PeonUpdate;
  peon->NextUpdate = 0;
  peon->think = PeonThink;
  peon->ThinkRate = 90;
  peon->NextThink = 0;
  peon->inputcommand = PeonCommand;
  peon->face = F_South;
  peon->state = ST_IDLE;
  peon->sightrange = 4;
  for(i = 0;i < 12;i++)
    if(i < 4)peon->Commands[i] = i;
    else peon->Commands[i] = -1;
  peon->health = 40;
  peon->healthmax = 40;
  EntityGroundList[peon->m.y][peon->m.x] = peon;
}

void PeonCommand(Entity *self, int order)
{
  switch(order)
  {
    case 0:
    break;
    default:
      self->state = order;
    break;
  }
}

void PeonUpdate(Entity *self)
{
  switch(self->state)
  {
    default:
    break;
    case ST_IDLE:
      /*Follow standing orders*/
      if(!LinedUpOnMX(self))
        GravitateToMX(self);
    break;
    case ST_WALK:
     if(self->Unit->wait <= 0)
     {
      if(MoveUnit(self))
      {
        EntitySight(self->m.x,self->m.y,self->sightrange,self->Player);
        self->Unit->moving = 1;
        self->Unit->failcount = 0;
      }
      else  /*clearly down the road this will get more complicated*/
      {
        if(self->Unit->failcount++ >= PERSISTENCE)
        {
          self->Unit->moving = 0;
          self->state = ST_IDLE;
          self->Unit->Orders = OR_NONE;
          self->Unit->wait = PERSISTENCE;
          GravitateToMX(self);
        }
      }
      if(((self->Unit->Orders == OR_FOLLOW)&&(self->target->Unit->moving == 0)&&(self->target->Unit->wait == 0))||(self->Unit->Orders != OR_FOLLOW))
       if((fabs(self->Unit->GoalTarget.x - self->m.x) < 2)&&(fabs(self->Unit->GoalTarget.y - self->m.y) < 2))
       {
        self->state = ST_IDLE;
        self->Unit->Orders = OR_NONE;
        self->Unit->moving = 0;
        GravitateToMX(self);
       }
      self->frame += 8;
      if(self->frame >= 32)
      {
        self->frame = self->face;
        MoveTo(self,self->Unit->GoalTarget.x,self->Unit->GoalTarget.y);
        /*this makes sure every soo often that we didn't miss the goal*/
      }
     }
     else self->Unit->wait--;
    break;
  }
}

void PeonThink(Entity *self)
{
  switch(self->Unit->Orders)
  {
    case OR_WALK:
      MoveTo(self,self->Unit->GoalTarget.x,self->Unit->GoalTarget.y);
      self->Unit->Orders = OR_NONE;
      self->state = ST_WALK;
    break;
    case OR_FOLLOW:
        self->state = ST_WALK;
        MoveTo(self,self->target->m.x,self->target->m.y);
    break;
    default:
    break;
  }
}

/*eof*/
