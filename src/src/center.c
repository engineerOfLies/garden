#include <string.h>
#include "entity.h"
#include "command.h"


extern Entity *EntityGroundList[MAP_H][MAP_W];
extern Interface DashBoard;
void UpdateCenter(Entity *self);
void ThinkCenter(Entity *self);


void SpawnCenter(int mx, int my, int Player, int condition)   /*condition refering to prebuilt or being built*/
{
  Entity *center;
  int i,j;
  center = NewBuildingEntity(Player,mx,my);
  if(center == NULL)return;
  center->Unit_Type = ET_Building;
  center->sprite = LoadSwappedSprite("images/Nexus.png",160,160,DashBoard.ComColor,0,0);
  strcpy(center->EntName,"Protoss Nexus");
  center->selectoffsety = -60;
  center->shown = 1;
  center->s.x = mx * 32;
  center->s.y = (my - 1)* 32 - 16;
  center->frame = 0;
  center->UpdateRate = 90;
  center->update = UpdateCenter;
  center->NextUpdate = 0;
  center->think = ThinkCenter;
  center->ThinkRate = 100;
  center->NextThink = 0;
  center->state = ST_IDLE;
  center->sightrange = 9;
  center->tilesize.x = 5;/*width*/
  center->tilesize.y = 3;/*height*/
  for(j = 0; j < 3;j++)
  {
    for(i = 0; i < 5;i++)
    {
      EntityGroundList[center->m.y + j][center->m.x + i] = center;
    }
  }
  for(i = 3;i < 12;i++)center->Commands[i] = -1;
  center->Commands[0] = BC_Peon;
  center->Commands[1] = BC_Ralley;
  center->Commands[2] = C_Stop;
  center->health = 400;
  center->healthmax = 400;
  EntitySight(mx+ 2,my + 1,10,Player);
}

void OrderCenter(Entity *self, int order)
{
  switch(order)/* C_Walk, C_Attack,C_Stop,  C_Gather, C_Build, C_Repair,C_Defend,
               BC_Ralley,BC_Peon*/
  {
    case BC_Peon:
//      self->Building->
    break;
    case BC_Ralley:
      /*anticipate a map click or a cancel*/
    break;
    case C_Stop:
      /*if I was building something, stop and replenish resources*/
    break;
    default:
     self->state = ST_IDLE;
    break;
  }
}

void UpdateCenter(Entity *self)
{
  switch(self->state)
  {
    default:
    break;
    case ST_IDLE:
    break;
  }
}

void ThinkCenter(Entity *self)
{
  switch(self->state)
  {
    default:
    break;
    case ST_IDLE:
    break;
  }
}

/*eof*/
