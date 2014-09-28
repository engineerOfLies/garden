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
  switch(condition)
  {
    case 0:
      center->sprite = LoadSwappedSprite("images/grave1.png",64,96,0,0,0);
      center->target = SpawnTestPeon(mx,my,Player,0);
    break;
    case 1:
      center->sprite = LoadSwappedSprite("images/grave2.png",64,96,0,0,0);
      center->target = SpawnTestPeon(mx,my,Player,1);
      break;
  }
  SDL_SetColorKey(center->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(center->sprite->image->format, 0,0,0));
  strcpy(center->EntName,"Grave");
  center->head = NULL;
  center->headstate = -1;
  center->selectoffsety = -32;
  center->shown = 1;
  center->s.x = (mx * 32) - 16;
  center->s.y = (my - 1)* 32 - 16;
  center->frame = 0;
  center->UpdateRate = 90;
  center->update = UpdateCenter;
  center->NextUpdate = 0;
  center->think = NULL;
  center->ThinkRate = 0;
  center->NextThink = 0;
  center->state = ST_IDLE;
  center->sightrange = 9;
  center->tilesize.x = 1;/*width*/
  center->tilesize.y = 1;/*height*/
  EntityGroundList[center->m.y][center->m.x] = center;
  for(i = 3;i < 12;i++)center->Commands[i] = -1;
  center->health = 10;
  center->healthmax = 400;
  center->target->owner = center;
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
  if((self->health > 10)&&(self->target != NULL))
  {
    self->target->mojo+= 10;
    self->health -=10;
  }
}

void ThinkCenter(Entity *self)
{
  switch(self->state)
  {
    default:
    break;
    case ST_IDLE:
      if(self->health > 0)
      {
        
      }
    break;
  }
}

/*eof*/
