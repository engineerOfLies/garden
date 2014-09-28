#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "entity.h"
#include "command.h"
#include "particle.h"
#include "game.h"

extern Entity *EntityGroundList[MAP_H][MAP_W];
extern Entity *EntityAirList[MAP_H][MAP_W];
extern Uint32 NOW;
extern Game DeadGame;


void PeonUpdate(Entity *self);
void PeonThink(Entity *self);
void PeonCommand(Entity *self, int order);

Entity *SpawnTestPeon(int mx, int my,int Player,int zomtype)
{
  int i;
  Entity *peon;
  peon = NewEntity(Player);
  if(peon == NULL)return NULL;
  peon->Unit_Type = ET_Unit;
  switch(zomtype)
  {
    case 0:
      peon->sprite = LoadSprite("images/skelrise.png",32,48);
      peon->sound[0] = LoadSound("sounds/skellpain.wav",MIX_MAX_VOLUME/2);
      peon->sound[1] = LoadSound("sounds/skelldeath.wav",MIX_MAX_VOLUME/2);
      peon->sound[2] = LoadSound("sounds/blooddeath.wav",MIX_MAX_VOLUME/2);
      peon->sound[3] = LoadSound("sounds/deadexplode.wav",MIX_MAX_VOLUME/2);
      peon->head = LoadSprite("images/skull.png",8,12);
    break;
    case 1:
      peon->sprite = LoadSwappedSprite("images/zombrise.png",32,48,rand()%32,rand()%32,rand()%32);
      peon->sound[0] = LoadSound("sounds/zompain.wav",MIX_MAX_VOLUME/2);
      peon->sound[1] = LoadSound("sounds/zombiedone.wav",MIX_MAX_VOLUME/2);
      peon->sound[2] = LoadSound("sounds/zomattack.wav",MIX_MAX_VOLUME/2);
      peon->sound[3] = LoadSound("sounds/deadexplode.wav",MIX_MAX_VOLUME/2);
      switch(rand()%3)
      {
        case 0:
          peon->head = LoadSprite("images/skull.png",8,12);
        break;
        case 1:
          peon->head = LoadSprite("images/baldzombiehead.png",10,16);
        break;
        case 2:
          peon->head = LoadSwappedSprite("images/hairzombiehead.png",10,16,rand()%32,0,0);
        break;
      }
    break;
  }
//  SDL_SetAlpha(peon->sprite->image, SDL_SRCALPHA|SDL_RLEACCEL, 128);
  SDL_SetColorKey(peon->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(peon->sprite->image->format, 0,0,0));
  strcpy(peon->EntName,"Generic Peon\0");
  SDL_SetColorKey(peon->head->image, SDL_SRCCOLORKEY , SDL_MapRGB(peon->head->image->format, 0,0,0));
  peon->headframe = 1;
  peon->headstate = -1;
  peon->headlayer = 0;    /*weather it is in front of or behind the main sprite*/
  peon->selectoffsety = -8;
  peon->shown = 1;
  SetSXfromMX(peon);
  peon->movespeed = 6;
  peon->frame = (rand() % 3);
  peon->fcount = 20;
  peon->UpdateRate = 80;
  peon->update = PeonUpdate;
  peon->NextUpdate = 0;
  peon->think = NULL;
  peon->ThinkRate = 90;
  peon->NextThink = 0;
  peon->face = F_South;
  peon->state = ST_IDLE;
  peon->level = 0;
  peon->sightrange = 4;
  peon->s.x = (mx * 32);  /*My current map position*/
  peon->s.y = (my * 32) - 20;  /*My current map position*/
  peon->m.x = mx;  /*My current map position*/
  peon->m.y = my;  /*My current map position*/
  peon->health = 0;
  peon->healthmax = 200;
  peon->framestates[0] = 4;
  peon->framestates[1] = 8;
  peon->framestates[2] = 12;
  peon->framestates[3] = 16;
  peon->mojo = 50 + (rand()%50);
  EntityAirList[peon->m.y][peon->m.x] = peon;
  PeonUpdate(peon);
  return peon;
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
    case ST_IDLE:
      /*Follow standing orders*/
      self->health++;
      self->mojo--;
      if(self->mojo >= 250) /*we over did it and we explode*/
      {
        Mix_PlayChannel(-1,self->sound[2]->sound,0);
        SpawnFountain(IndexColor(LightRed),self->m.x,self->m.y,200);
        SpawnBloodSpray(IndexColor(LightRed),self->m.x,self->m.y,OffSet(2),OffSet(2),100,10);
        self->owner->target = NULL;
        self->owner->frame = 3;
        FreeEntity(self);
        DeadZombie(&DeadGame);
        return;
      }
      if(self->health >= self->healthmax / 2)     /*we gain a level*/
      {
        self->level += 1;
        self->health = 0;
        self->frame = self->framestates[self->level - 1] + 1 + (16 * self->sick);
        if(self->level == 2)
        {
          self->headstate = 0;
          self->Hs.x = self->s.x + 21;
          self->Hs.y = self->s.y + 37;
        }
        if(self->level == 3)
        {
          self->Hs.x = self->s.x + 15;
          self->Hs.y = self->s.y + 20;
        }
        if(self->level > 3)
        {
          Mix_PlayChannel(-1,self->sound[1]->sound,0);
          self->owner->frame = 1;
          self->owner->target = NULL;
          self->state = ST_WALK;
          MoveTo(self, (rand()>>8)%32,(rand()>>8)%24);
          FreeSprite(self->sprite);
          self->sprite = LoadSprite("images/skeletonwalk.png",32,48);
          self->Hs.x = self->s.x + 16;
          self->Hs.y = self->s.y + 7;
          SDL_SetColorKey(self->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(self->sprite->image->format, 0,0,0));
          self->frame = (rand()>>8)%4;
          GrownZombie(&DeadGame);
          return;
        }
      }
      if((self->mojo < 50)&&(self->mojo >= 0))
      {
        if(self->sick != 1)
        {
          self->sick = 1;
          if(self->frame < 16)self->frame += 16;             /*we were ok, now lets show that we are worse.*/
          else while(self->frame >= 32)self->frame -= 16;     /*we were worse, how worse? who cares, lets just make sure we are under 32*/
        }
      }
      else if((self->mojo < 0)&&(self->mojo > -50))
      {
        if(self->sick != 2)
        {
          self->sick = 2;
          if(self->frame < 32)
          {
            Mix_PlayChannel(-1,self->sound[0]->sound,0);/*only play a sound when we get worse, not better*/
            while(self->frame < 32)self->frame += 16;           
          }
          else while(self->frame >= 48)self->frame -=16;
        }
        else  /*the first time we get this sick, we don't lose any health*/
        {
          self->health--;
        }
      }
      else if((self->mojo <= -50)&&(self->mojo >= -150))
      {
        if(self->sick != 3)
        {
          self->sick = 3;
          if(self->frame < 48)while(self->frame < 48)self->frame += 16;           
          else while(self->frame >= 64)self->frame -=16;
        }
        else  /*the first time we get this sick, we don't lose any health*/
        {
          self->health-=2;
        }
      }
      else
      {
        if(self->sick)
        {
          self->sick = 0;
          while(self->frame >= 16)self->frame -= 16;
        }
      }
      if(self->mojo < -150)  /*we wither and die*/
      {
        Mix_PlayChannel(-1,self->sound[3]->sound,0);
        SpawnFire(IndexColor(LightYellow),self->m.x,self->m.y,100);
        self->owner->target = NULL;
        self->owner->frame = 2;
        FreeEntity(self);
        DeadZombie(&DeadGame);
        return;
      }
      self->frame++;
      if(self->frame >= (self->framestates[self->level] + (16 * self->sick)))
      {
        if(self->level == 0)self->frame = 0 + (16 * self->sick);
        else
        {
          self->frame = self->framestates[self->level - 1] + 1 + (16 * self->sick);
        }
      }
    break;
    case ST_WALK:
      self->frame++;
      MoveUnit(self);
      switch(self->face)
      {
        case F_SE:
        case F_South:
          if(self->frame >= 4) self->frame = 0;
          self->headstate = 0;
          self->Hs.x = self->s.x + 16;
          self->headlayer = 0;
          self->Hs.y = self->s.y + 7;
        break;
        case F_SW:
        case F_West:
          if(self->frame >= 8) self->frame = 4;
          self->Hs.x = self->s.x + 17;
          self->Hs.y = self->s.y + 10;
          self->headlayer = 0;
          self->headstate = 1;
          break;
        case F_NW:
        case F_North:
          if(self->frame >= 12) self->frame = 8;
          self->headstate = 2;
          self->headlayer = 1;
          self->Hs.x = self->s.x + 16;
          self->Hs.y = self->s.y + 7;
          break;
        case F_NE:
        case F_East:
          if(self->frame >= 16) self->frame = 12;
          self->headlayer = 0;
          self->headstate = 3;
          self->Hs.x = self->s.x + 10;
          self->Hs.y = self->s.y + 10;
          break;
      }
      if(self->fcount >= 32)
      {
        self->fcount = 0;
        MoveTo(self, (rand()>>8)%32,(rand()>>8)%24);
        /*this makes sure every soo often that we didn't miss the goal*/
      }
    break;
  }
}

void PeonThink(Entity *self)
{
  switch(self->Unit->Orders)
  {
    default:
    break;
  }
}

/*eof*/
