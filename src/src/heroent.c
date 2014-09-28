
#include "hero.h"
#include "entity.h"

#define HeroMax   16

extern Entity *EntityGroundList[MAP_H][MAP_W];
HeroEnt HeroList[16];

void HeroUpdate(Entity *self);
void HeroThink(Entity *self);
void HeroCommand(Entity *self, int order);

/*Hero character related functions*/
 /*creates a blank hero instance*/
HeroEnt *SpawnHero(int player)
{
  int i;
  for(i = 0;i < HeroMax;i++)
  {
    if(HeroList[i].used == 0)
    {
      HeroList[i].used = 1;
      HeroList[i].player = player;
      return &HeroList[i];
    }
  }
  fprintf(stderr,"Runned out of hero slots.\n");
  return NULL;
}

HeroEnt *GetHeroByPlayer(int player)
{
  int i;
  for(i = 0;i < HeroMax;i++)
  {
    if((HeroList[i].used)&&(HeroList[i].player == player))return &HeroList[i];
  }
  return NULL;    /*that player does not control a living hero*/
}

void FreeHeroEnt(HeroEnt *hero)
{
  FreeSprite(hero->Headshot);
  hero->used = 0;
  hero = NULL;
}


void SpawnHeroEnt(int mx, int my,int Player,char sprite[30],int c1,int c2,int c3)
{
  int i;
  Entity *hero;
  hero = SpawnUnitEntity(Player,mx,my);
  if(hero == NULL)return;
  SpawnHero(Player);
  hero->Unit_Type = ET_Unit;
  hero->sprite = LoadSwappedSprite(sprite,32,32,c1,c2,c3);
  strcpy(hero->EntName,"Hero");
  hero->selectoffsety = -8;
  hero->shown = 1;
  SetSXfromMX(hero);
  hero->movespeed = 6;
  hero->frame = 0;
  hero->UpdateRate = 80;
  hero->update = HeroUpdate;
  hero->NextUpdate = 0;
  hero->think = HeroThink;
  hero->ThinkRate = 90;
  hero->NextThink = 0;
  hero->inputcommand = HeroCommand;
  hero->face = F_South;
  hero->state = ST_IDLE;
  hero->sightrange = 4;
  for(i = 0;i < 12;i++)
    if(i < 4)hero->Commands[i] = i;
    else hero->Commands[i] = -1;
  EntityGroundList[hero->m.y][hero->m.x] = hero;
}

void HeroCommand(Entity *self, int order)
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

void HeroUpdate(Entity *self)
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

void HeroThink(Entity *self)
{
  switch(self->Unit->Orders)
  {
    default:
    break;
    case OR_WALK:
      MoveTo(self,self->Unit->GoalTarget.x,self->Unit->GoalTarget.y);
      self->Unit->Orders = OR_NONE;
      self->state = ST_WALK;
    break;
    case OR_FOLLOW:
        self->state = ST_WALK;
        MoveTo(self,self->target->m.x,self->target->m.y);
    break;
  }
}



/*EOL at the EOF*/

