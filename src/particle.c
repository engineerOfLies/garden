#include <stdlib.h>
#include <string.h>
#include "particle.h"
#include "entity.h"

#define MAXPART    8192

extern SDL_Rect    Camera;
extern SDL_Surface *screen;
extern Uint32      NOW;
extern Map GameMap;
Particle           ParticleList[MAXPART];
int                NumParticles;

float OffSet(float die)   /*this will give a random float between die and -die*/
{
  return (((rand()>>8) % ((int)(die * 20))+ 1)* 0.1) - die;
}


void ResetAllParticles()
{
  memset(&ParticleList,0,sizeof(Particle) * MAXPART);
  NumParticles = 0;
}

Particle *SpawnParticle()    /*give a pointer to newly created particles.*/
{
  int i;
  if(NumParticles >= MAXPART)return NULL;     /*all out of them darn particles*/
  for(i = 0;i < MAXPART;i++)
  {
    if(ParticleList[i].used == 0)   /*lets find the first available slot*/
    {
      ParticleList[i].used = 1;
      NumParticles++;
      return &ParticleList[i];
    }
  }
  return NULL;/*catch all*/
}

void DrawAllParticles()   /*draw and update all of the particles that are active to the draw buffer*/
{
  int i,done,killed;
  Sint32 r,g,b;
  killed = 0;
  done = 0;
  for(i = 0;i < MAXPART;i++)
  {
    if(done >= NumParticles)break;
    if(ParticleList[i].used == 1)
    {
     done++;
     if(ParticleList[i].flags & PF_SPRITE)
     {
       DrawSprite(ParticleList[i].sprite,screen,(int)ParticleList[i].sx - Camera.x - (ParticleList[i].sprite->w/2), (int)(ParticleList[i].sy - (ParticleList[i].sz * 0.6)) - Camera.y - (ParticleList[i].sprite->h/2),ParticleList[i].frame);
       ParticleList[i].delay--;
       if(ParticleList[i].delay <= 0)
       {
         ParticleList[i].frame = (ParticleList[i].frame + 1)%ParticleList[i].loop;
         ParticleList[i].delay = ParticleList[i].delayrate;
       }
     }
     else  putpixel(screen, (int)ParticleList[i].sx - Camera.x, (int)(ParticleList[i].sy - (ParticleList[i].sz * 0.6)) - Camera.y, ParticleList[i].Color);
     ParticleList[i].lifespan--;
     if((ParticleList[i].lifespan <= 0)||(ParticleList[i].sz < 0))/*if we have expired or fallen out of sight,...*/
     {
       killed++;
       ParticleList[i].used = 0;
     }
     else
     {
       if(ParticleList[i].flags & PF_WINDED)
       {
         ParticleList[i].sx += GameMap.wx;   /*apply local wind velocity to all positions*/
         ParticleList[i].sy += GameMap.wy;
       }
       ParticleList[i].sx += ParticleList[i].vx;   /*apply velocity to all positions*/
       ParticleList[i].sy += ParticleList[i].vy;
       ParticleList[i].sz += ParticleList[i].vz;
       ParticleList[i].vx += ParticleList[i].ax;   /*apply accelleration to all velocities*/
       ParticleList[i].vy += ParticleList[i].ay;
       ParticleList[i].vz += ParticleList[i].az;
       if((ParticleList[i].flags & PF_SPRITE) == 0)/*as long as we are not a sprite, adjust the color*/
       {
         SDL_GetRGB(ParticleList[i].Color, screen->format, &r, &g, &b);
         r += ParticleList[i].R;
         if(r < 0)r = 0;
         else if(r > 255)r = 255;
         g += ParticleList[i].G;
         if(g < 0)g = 0;
         else if(g > 255)g = 255;
         b += ParticleList[i].B;
         if(b < 0)b = 0;
         else if(b > 255)b = 255;
         ParticleList[i].Color = SDL_MapRGB(screen->format,(Uint8)r,(Uint8)g,(Uint8)b);
       }
     }
    }
  }
  NumParticles -= killed;
}

Entity *SpawnParticleEnt(Uint32 Color,int mx,int my,Uint8 time)
{
  int i;
  Entity *PartEnt;
  PartEnt = NewEntity(NUMPLAYERS);
  if(PartEnt == NULL)
  {
    fprintf(stdout,"Unable to allocate room for a particle fountain..\n");
    return NULL;
  }
  for(i = 0;i < SOUNDSPERENT;i++)PartEnt->sound[i] = NULL;
  PartEnt->selectoffsety = -30;
  PartEnt->owner = NULL;
  PartEnt->target = NULL;
  PartEnt->think = NULL;
  PartEnt->update = NULL;
  PartEnt->inputcommand = NULL;
  PartEnt->Unit = NULL;
  PartEnt->Building = NULL;
  PartEnt->Unit_Type = ET_World; /*resource, building, unit.*/
  strcpy(PartEnt->EntName,"Particle"); /*the name of the unit or building*/
  PartEnt->sprite = NULL;
  PartEnt->NextThink = 0;
  PartEnt->ThinkRate = 0;  /*used for incrementing above*/
  PartEnt->NextUpdate = 0; /*used for how often the entity is updated, updating is merely animations*/
  PartEnt->UpdateRate = 0;  /*used for incrementing above*/
  PartEnt->shown = 1; /*if 1 then it will be rendered when it is on screen*/
  PartEnt->frame = 0; /*this will be randome later on...*/
  PartEnt->s.x = (mx * 32) + 16;
  PartEnt->s.y = (my * 32) + 16;;
  PartEnt->m.x = mx;
  PartEnt->m.y = my;
  PartEnt->tilesize.x = 1;
  PartEnt->tilesize.y = 1;
  PartEnt->health = time;
  PartEnt->healthmax = time;/*Resources use it for how much of a resource it has left*/
  PartEnt->Color = Color;
  return PartEnt;
}

/*          Specific instances of particles lie below                              */

void FountainUpdate(Entity *self)
{
  int i;
  Uint8 r,g,b;
  float k;
  Particle *newparticle = NULL;
  if(self->health == 0)
  {
    FreeEntity(self);
    return;
  }
  if(self->health > 0)self->health--;
  for(i = 0;i < 8;i++)
  {
    newparticle = SpawnParticle();
    if(newparticle != NULL)/*lets always make sure we have a particle*/
    {
      SDL_GetRGB(self->Color, screen->format, &r, &g, &b);
      k = OffSet(0.5);
      k += 1;
      r = (Uint8)(k * r);/*shift the colors slightly*/
      g = (Uint8)(k * g);
      b = (Uint8)(k * b);
      newparticle->Color = SDL_MapRGB(screen->format,r,g,b);
      newparticle->sx = self->s.x + OffSet(0.2);
      newparticle->sy = self->s.y + OffSet(0.2);
      newparticle->sz = 6;
      newparticle->vx = OffSet(0.5);
      newparticle->vy = OffSet(0.5);
      newparticle->vz = 10 + OffSet(0.5);
      newparticle->ax = 0;
      newparticle->ay = 0;
      newparticle->R = 0;
      newparticle->G = 0;
      newparticle->B = 0;
      newparticle->az = -1.5;
      newparticle->lifespan = 80;
      newparticle->flags = PF_NONE;
    }
  }
}

void SpawnFountain(Uint32 Color,int mx,int my,Uint8 time)
{
  Entity *fount;
  fount = SpawnParticleEnt(Color,mx,my,time);
  if(fount == NULL)return;
  fount->update = FountainUpdate;
  fount->NextUpdate = 5;
  fount->UpdateRate = 10;
}

void SpawnBloodSpray(Uint32 Color,int mx,int my,float vx,float vy,Uint8 time,int volume)
{
  int i,k;
  Uint8 r,g,b;
  Particle *newparticle;
  for(i = 0;i < volume;i++)
  {
    newparticle = SpawnParticle();
    if(newparticle != NULL)/*lets always make sure we have a particle*/
    {
      SDL_GetRGB(Color, screen->format, &r, &g, &b);
      k = OffSet(0.5);
      k += 1;
      r = (Uint8)(k * r);/*shift the colors slightly*/
      g = (Uint8)(k * g);
      b = (Uint8)(k * b);
      newparticle->Color = SDL_MapRGB(screen->format,r,g,b);
      newparticle->sx = (mx * 32) + 16;
      newparticle->sy = (my * 32) + 16;
      newparticle->sz = 6;
      newparticle->vx = vx + OffSet(2);
      newparticle->vy = vy + OffSet(2);
      newparticle->vz = OffSet(1);
      newparticle->ax = 0;
      newparticle->ay = 0;
      newparticle->R = 0;
      newparticle->G = 0;
      newparticle->B = 0;
      newparticle->az = -1.5;
      newparticle->lifespan = time;
      newparticle->flags = PF_NONE;
    }
  }
  
}

void DripUpdate(Entity *self)
{
  int i;
  Uint8 r,g,b;
  float k;
  Particle *newparticle = NULL;
  if(self->health == 0)
  {
    FreeEntity(self);
    return;
  }
  if(self->health > 0)self->health--;
  for(i = 0;i < 4;i++)
  {
    newparticle = SpawnParticle();
    if(newparticle != NULL)/*lets always make sure we have a particle*/
    {
      SDL_GetRGB(self->Color, screen->format, &r, &g, &b);
      k = OffSet(0.5);
      k += 1.2;
      r = (Uint8)(k * r);/*shift the colors slightly*/
      g = (Uint8)(k * g);
      b = (Uint8)(k * b);
      newparticle->Color = SDL_MapRGB(screen->format,r,g,b);
      newparticle->sx = self->s.x + OffSet(0.2);
      newparticle->sy = self->s.y + OffSet(0.2);
      newparticle->sz = self->healthmax;
      newparticle->vx = OffSet(0.1);
      newparticle->vy = OffSet(0.1);
      newparticle->vz = 1.5;
      newparticle->ax = 0;
      newparticle->ay = 0;
      newparticle->R = 0;
      newparticle->G = 0;
      newparticle->B = 0;
      newparticle->az = -0.5;
      newparticle->lifespan = 80;
      newparticle->flags = PF_NONE;
    }
  }
}

void SpawnDrip(Uint32 Color,int sx,int sy,int sz,Uint8 time)
{
  Entity *drip;
  drip = SpawnParticleEnt(Color,sx,sy,time);
  if(drip == NULL)return;
  drip->healthmax = sz;
  drip->update = DripUpdate;
  drip->NextUpdate = 5;
  drip->UpdateRate = 200;
}

void FireUpdate(Entity *self)
{
  int i;
  Uint8 r,g,b;
  float k;
  Particle *newparticle = NULL;
  if(self->health == 0)
  {
    FreeEntity(self);
    return;
  }
  if(self->health > 0)self->health--;
  for(i = 0;i < 12;i++)
  {
    newparticle = SpawnParticle();
    if(newparticle != NULL)/*lets always make sure we have a particle*/
    {
      SDL_GetRGB(self->Color, screen->format, &r, &g, &b);
      k = OffSet(0.5);
      k += 1;
      r = (Uint8)(k * r);/*shift the colors slightly*/
      g = (Uint8)(k * g);
      b = (Uint8)(k * b);
      newparticle->Color = SDL_MapRGB(screen->format,r,g,b);
      newparticle->sx = self->s.x + OffSet(10);
      newparticle->sy = self->s.y + OffSet(10);
      newparticle->sz = 2;
      newparticle->vx = OffSet(0.1);
      newparticle->vy = OffSet(0.1);
      newparticle->vz = 5 + OffSet(0.5);
      newparticle->ax = 0;
      newparticle->ay = 0;
      newparticle->az = 0.1;
      newparticle->R = -4;
      newparticle->G = -8;
      newparticle->B = -4;
      newparticle->lifespan = 10 + OffSet(6);
      newparticle->flags = PF_WINDED;
   }
  }
}

void SpawnFire(Uint32 Color,int mx,int my,Uint8 time)
{
  Entity *fount;
  fount = SpawnParticleEnt(Color,mx,my,time);
  if(fount == NULL)return;
  fount->update = FireUpdate;
  fount->NextUpdate = 0;
  fount->UpdateRate = 30;
  FireUpdate(fount);
}

void RainSpotUpdate(Entity *self)
{
  int i;
  Uint8 r,g,b;
  float k;
  Particle *newparticle = NULL;
  if(self->health == 0)
  {
    FreeEntity(self);
    return;
  }
  if(self->health > 0)self->health--;
  for(i = 0;i < 16;i++)
  {
    newparticle = SpawnParticle();
    if(newparticle != NULL)/*lets always make sure we have a particle*/
    {
      SDL_GetRGB(self->Color, screen->format, &r, &g, &b);
      k = OffSet(0.5);
      k += 1;
      r = (Uint8)(k * r);/*shift the colors slightly*/
      g = (Uint8)(k * g);
      b = (Uint8)(k * b);
      newparticle->Color = SDL_MapRGB(screen->format,r,g,b);
      newparticle->sx = self->s.x + OffSet(16);
      newparticle->sy = self->s.y + OffSet(16);
      newparticle->sz = 20;
      newparticle->vx = OffSet(0.1);
      newparticle->vy = OffSet(0.1);
      newparticle->vz = -5 + OffSet(0.5);
      newparticle->ax = 0;
      newparticle->ay = 0;
      newparticle->az = -0.1;
      newparticle->R = -1;
      newparticle->G = -1;
      newparticle->B = -1;
      newparticle->lifespan = 10 + OffSet(6);
      newparticle->flags = PF_WINDED;
    }
  }
}

void SpawnRainSpot(Uint32 Color,int mx,int my,Uint8 time)
{
  Entity *fount;
  fount = SpawnParticleEnt(Color,mx,my,time);
  if(fount == NULL)return;
  fount->update = RainSpotUpdate;
  fount->NextUpdate = 5;
  fount->UpdateRate = 10;
}

void ItsRaining(Uint32 Color,Uint8 time,int volume,float decent)
{
  int i,k;
  Uint8 r,g,b;
  Particle *newparticle;
  for(i = 0;i < volume;i++)
  {
    newparticle = SpawnParticle();
    if(newparticle != NULL)/*lets always make sure we have a particle*/
    {
      SDL_GetRGB(Color, screen->format, &r, &g, &b);
      k = OffSet(0.5);
      k += 1;
      r = (Uint8)(k * r);/*shift the colors slightly*/
      g = (Uint8)(k * g);
      b = (Uint8)(k * b);
      newparticle->Color = SDL_MapRGB(screen->format,r,g,b);
      newparticle->sx = Camera.x + (Camera.w/2)+OffSet(Camera.w);
      newparticle->sy = Camera.y + (Camera.h/2)+OffSet(Camera.h);
      newparticle->sz = 60;
      newparticle->vx = OffSet(0.1);
      newparticle->vy = OffSet(0.1);
      newparticle->vz = -1;
      newparticle->ax = 0;
      newparticle->ay = 0;
      newparticle->R = -1;
      newparticle->G = -1;
      newparticle->B = -1;
      newparticle->az = decent;
      newparticle->lifespan = time;
      newparticle->flags = PF_WINDED;
      
    }
  }
  
}

void BoltHailUpdate(Entity *self)
{
  int i;
  Particle *newparticle = NULL;
  if(self->health == 0)
  {
    FreeEntity(self);
    return;
  }
  if(self->health > 0)self->health--;
  for(i = 0;i < 4;i++)
  {
    newparticle = SpawnParticle();
    if(newparticle != NULL)/*lets always make sure we have a particle*/
    {
      newparticle->sx = self->s.x + OffSet(48);
      newparticle->sy = self->s.y + OffSet(48);
      newparticle->sz = 60;
      newparticle->vx = OffSet(0.1);
      newparticle->vy = OffSet(0.1);
      newparticle->vz = -5 + OffSet(0.5);
      newparticle->ax = 0;
      newparticle->ay = 0;
      newparticle->az = -0.1;
      newparticle->lifespan = 10 + OffSet(6);
      newparticle->flags = PF_WINDED |PF_SPRITE;
      newparticle->sprite = LoadSwappedSprite("images/magicbolt.png",32, 32, self->Color, 0,0);
      SDL_SetColorKey(newparticle->sprite->image, SDL_SRCCOLORKEY , IndexColor(Black));
      newparticle->frame = 2 + (int)OffSet(2);
      newparticle->loop = 4;
      newparticle->delay = 0;
      newparticle->delayrate = 2;
    }
  }
}

void SpawnBoltHail(Uint32 Color,int mx,int my,Uint8 time)
{
  Entity *fount;
  fount = SpawnParticleEnt(Color,mx,my,time);
  if(fount == NULL)return;
  fount->update = BoltHailUpdate;
  fount->NextUpdate = 5;
  fount->UpdateRate = 60;
}


/*EO@EOF*/
