/*
 *    Donald Kehoe
 *    Sometime in February
 *    Last Modified: 3/21/05
 *
 *    Description: definitions for Entity handling functions (methods).
 *      
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "entity.h"
#include "command.h"
/*
  Entity function definitions
*/

extern SDL_Surface *screen;
extern SDL_Surface *background;
extern SDL_Surface *fogofwar;
extern SDL_Surface *unseenmask;
extern SDL_Rect Camera;
extern Map GameMap;
extern Uint32 NOW;

Entity EntityList[NUMPLAYERS+1][MAXENTITIES];  /*the last column is the world*/
Entity *EntityGroundList[MAP_H][MAP_W];/*Pointers to entities, by location*/
Entity *EntityAirList[MAP_H][MAP_W];/*Pointers to entities, by location int he air*/
Entity DEADSPACE;/*this info will be used to mark the above list to ensure that we never assign an entity to a dead spot.*/

/*unit and building information*/
/*I will make this like I make sprites, each "peon" will point to the same peon information, per player*/


int ThisPlayer = 0;
int NumLives = 3;
int NumEnts[NUMPLAYERS+1];
int TotalEnts = 0;
int MOUSEMOVE = 1;


void InitEntityList()
{
  int i,j,p;
  TotalEnts = 0;
  for(p = 0; p < NUMPLAYERS+1;p++)
  {
    NumEnts[p] = 0;
    for(i = 0;i < MAXENTITIES; i++)
    {
      EntityList[p][i].sprite = NULL;
      EntityList[p][i].owner = NULL;
      EntityList[p][i].think = NULL;
      EntityList[p][i].target = NULL;
      EntityList[p][i].update = NULL;
      EntityList[p][i].Building = NULL;
      EntityList[p][i].Unit = NULL;
      EntityList[p][i].head = NULL;
      for(j = 0;j < SOUNDSPERENT;j++)
      {
        EntityList[p][i].sound[j] = NULL;
      }
      EntityList[p][i].shown = 0;
      EntityList[p][i].used = 0;
    }
  }
  for(j = 0;j < MAP_H;j++)
  {
    for(i = 0; i < MAP_W;i++)
    {
      EntityGroundList[j][i] = NULL;/*make sure we don't point at anything useful*/
    }
  }
  /*lets make sure our deadspace entity is blank and safe.*/
  DEADSPACE.sprite = NULL;
  DEADSPACE.head = NULL;
  DEADSPACE.owner = NULL;
  DEADSPACE.think = NULL;
  DEADSPACE.target = NULL;
  DEADSPACE.update = NULL;
  DEADSPACE.Building = NULL;
  DEADSPACE.Unit = NULL;
  for(j = 0;j < SOUNDSPERENT;j++)
  {
    DEADSPACE.sound[j] = NULL;
  }
  DEADSPACE.shown = 0;
  DEADSPACE.used = 0;
  NewEntity(0);/*just to start things off*/
}

void DrawEntities()
{
  int i,j;
  int sx = Camera.x / 32 - 2; /*lets be on the safe side*/
  int sy = Camera.y / 32 - 2;
  int sw = Camera.w / 32 + 4;
  int sh = Camera.h / 32 + 4;
  if(sx < 0)sx = 0;
  if(sy < 0)sy = 0;
  for(j = sy; (j < sh + sy)&&(j < GameMap.map.h);j++)
  {
    for(i = sx; (i < sw + sx)&&(i < GameMap.map.w);i++)
    {
      if((EntityGroundList[j][i] != NULL)&&(EntityGroundList[j][i] != &DEADSPACE))
      {
       if(EntityGroundList[j][i]->used)
       {
           if(EntityGroundList[j][i]->LastDrawn != NOW)
           {
            DrawEntity(EntityGroundList[j][i]);
            EntityGroundList[j][i]->LastDrawn = NOW;/*only draw everything once per frame*/
           }
       }
     }
    }
  }
  for(j = sy; (j < sh + sy)&&(j < GameMap.map.h);j++)
  {
    for(i = sx; (i < sw + sx)&&(i < GameMap.map.w);i++)
    {
      if((EntityAirList[j][i] != NULL)&&(EntityAirList[j][i] != &DEADSPACE))
      {
        if(EntityAirList[j][i]->used)
        {
          if(EntityAirList[j][i]->LastDrawn != NOW)
          {
            DrawEntity(EntityAirList[j][i]);
            EntityAirList[j][i]->LastDrawn = NOW;/*only draw everything once per frame*/
          }
        }
      }
    }
  }
}

void ThinkEntities()
{
  int i,p;
  int checked[NUMPLAYERS + 1] = {0,0,0,0,0,0,0,0,0};
  int totalchecked = 0;
  for(i = 0;i < MAXENTITIES;i++)
  {
    for(p = 0; p <= NUMPLAYERS;p++)
    {
     if(checked[p] < NumEnts[p])
      if(EntityList[p][i].used)
      {
        checked[p]++;
        totalchecked++;
        if(totalchecked >= TotalEnts)return;
        if(EntityList[p][i].NextThink < NOW)
        {
          if(EntityList[p][i].think != NULL)
          {
            EntityList[p][i].think(&EntityList[p][i]);
            EntityList[p][i].NextThink = NOW + EntityList[p][i].ThinkRate;
          }
        }
      }
    }
  }
}

void UpdateEntities()
{
  int i,p;
  int checked[NUMPLAYERS + 1] = {0,0,0,0,0,0,0,0,0};
  int totalchecked = 0;
  for(i = 0;i < MAXENTITIES;i++)
  {
    for(p = 0; p <= NUMPLAYERS;p++)
    {
     if(checked[p] < NumEnts[p])
      if(EntityList[p][i].used)
      {
        checked[p]++;
        totalchecked++;
        if(totalchecked >= TotalEnts)return;
        if(EntityList[p][i].NextUpdate < NOW)
        {
          if(EntityList[p][i].update != NULL)
          {
            EntityList[p][i].update(&EntityList[p][i]);
            EntityList[p][i].NextUpdate = NOW + EntityList[p][i].UpdateRate;
          }
        }
      }
    }
  }
}

void DrawEntity(Entity *ent)
{
  if(ent->headstate == -1)
  {
    DrawSprite(ent->sprite,screen,ent->s.x - Camera.x,ent->s.y - Camera.y,ent->frame);
    return;    
  }
  if(ent->headlayer == 0)
  {
    DrawSprite(ent->sprite,screen,ent->s.x - Camera.x,ent->s.y - Camera.y,ent->frame);
    DrawSprite(ent->head,screen,ent->Hs.x - (ent->head->w / 2),ent->Hs.y - (ent->head->h / 2),ent->headframe);
  }
  else
  {
    DrawSprite(ent->head,screen,ent->Hs.x - (ent->head->w / 2),ent->Hs.y - (ent->head->h / 2),ent->headframe);
    DrawSprite(ent->sprite,screen,ent->s.x - Camera.x,ent->s.y - Camera.y,ent->frame);
  }
}

/*this is just the prep for generic Buildings, each unit will have more specific data set later*/
Entity *NewBuildingEntity(int Player,int mx,int my)
{
  Entity *Unit;
  if((mx < 0)||(mx > GameMap.map.w)||(my < 0)||(my > GameMap.map.h))return NULL;/*can't spawn outside of reality*/
  Unit = NewEntity(Player);
  if(Unit == NULL)return NULL;/*I guess we're all filled up.*/
  Unit->Building = malloc(sizeof(UnitInfo));
  if(Unit->Building == NULL)
  {     /*this is going to be for testing purposes.  I will change this to a static list later*/
    fprintf(stderr,"Couldn't Allocate memory: %s\n", SDL_GetError());
    exit(1);
  }
  Unit->m.x = mx;  /*My current map position*/
  Unit->m.y = my;  /*My current map position*/
  return Unit;
}


/*this is just the prep for generic Units, each unit will have more specific data set later*/
Entity *SpawnUnitEntity(int Player,int mx,int my) /*spawns at a map location*/
{
  Entity *Unit;
  if((mx < 0)||(mx > GameMap.map.w)||(my < 0)||(my > GameMap.map.h))return NULL;/*can't spawn outside of reality*/
  Unit = NewEntity(Player);
  if(Unit == NULL)return NULL;/*I guess we're all filled up.*/
  Unit->Unit = malloc(sizeof(UnitInfo));
  if(Unit->Unit == NULL)
  {     /*this is going to be for testing purposes.  I will change this to a static list later*/
    fprintf(stderr,"Couldn't Allocate memory: %s\n", SDL_GetError());
    exit(1);
  }
  Unit->m.x = mx;  /*My current map position*/
  Unit->m.y = my;  /*My current map position*/
  Unit->Unit->Orders = OR_NONE;         /*what I was told to do*/
  return Unit; 
}

/*
  returns NULL if all filled up, or a pointer to a newly designated Entity.
  Its up to the other function to define the data.
*/
Entity *NewEntity(int Player)
{
  int i;
  if(NumEnts[Player] + 1 >= MAXENTITIES)
  {
    return NULL;
  }
  NumEnts[Player]++;
  TotalEnts++;
  for(i = 0;i <= NumEnts[Player];i++)
  {
    if(!EntityList[Player][i].used)break;
  }
  EntityList[Player][i].used = 1;
  EntityList[Player][i].Player = Player;
  return &EntityList[Player][i];
}

/*done with an entity, now give back its water..I mean resources*/
void FreeEntity(Entity *ent)
{
  int j;
  fprintf(stdout,"Freeing %s\n",ent->EntName);
  ent->used = 0;
  NumEnts[ent->Player]--;
  TotalEnts--;
  if(ent->sprite != NULL)FreeSprite(ent->sprite);
  if(ent->head != NULL)FreeSprite(ent->head);
  for(j = 0;j < SOUNDSPERENT;j++)
  {
    if(ent->sound[j] != NULL)FreeSound(ent->sound[j]);
    ent->sound[j] = NULL;
  }
  if(EntityGroundList[ent->m.y][ent->m.x] == ent)
  {
    EntityGroundList[ent->m.y][ent->m.x] = NULL;
  }
  ent->sprite = NULL;
  ent->owner = NULL;
  ent->think = NULL;
  ent->target = NULL;
  ent->update = NULL;
      /*delete the infor first, then close the pointer*/
  free(ent->Building);
  ent->Building = NULL;
  free(ent->Unit);
  ent->Unit = NULL;
}

/*kill them all*/
void ClearEntities()
{
  int i,j;
  for(j = 0; j <= NUMPLAYERS;j++)
  {
    for(i = 0;i < MAXENTITIES;i++)
    {
      FreeEntity(&EntityList[j][i]);
    }
  }
}

void ResetFog()
{
  static Uint32 LastCall = 0;
  int i,j,e;
  SDL_Rect box;
  box.w = 32;
  box.h = 32;
  if(LastCall + 100 > NOW)return;/*too soon, lets wait a while*/
  LastCall = NOW;

  memset(GameMap.playerviews[ThisPlayer].seeing,0,sizeof(GameMap.playerviews[ThisPlayer].seeing));
  for(e = 0; e < MAXENTITIES;e++)
  {
    if(EntityList[ThisPlayer][e].used == 1)
    {
      EntitySeeing(EntityList[ThisPlayer][e].m.x + EntityList[ThisPlayer][e].tilesize.x/2,EntityList[ThisPlayer][e].m.y + EntityList[ThisPlayer][e].tilesize.y/2,EntityList[ThisPlayer][e].sightrange,ThisPlayer);
    }
  }

  for(j = 0;j < GameMap.map.h;j++)
  {
    for(i = 0;i < GameMap.map.w;i++)
    {
      if(GameMap.playerviews[ThisPlayer].seeing[j][i] != GameMap.playerviews[ThisPlayer].OldSeeing[j][i])
      { /*something has changed here*/
        GameMap.playerviews[ThisPlayer].OldSeeing[j][i] = GameMap.playerviews[ThisPlayer].seeing[j][i];
        if(GameMap.playerviews[ThisPlayer].seeing[j][i])
        {   /*we are NOW seeing it*/
          box.x = i * 32;
          box.y = j * 32;
          SDL_BlitSurface(background,&box,fogofwar,&box);
        }
        else
        { /*we are no longer seeing it*/
          DrawSprite(GameMap.FOG,fogofwar,i * 32,j *32,0);
          if((EntityGroundList[j][i] != NULL)&&(EntityGroundList[j][i] != &DEADSPACE))
          {
            if((EntityGroundList[j][i]->Unit_Type == ET_World)||(EntityGroundList[j][i]->Unit_Type == ET_Building))
            {
             if((i >= Camera.x / 32) && (i < (Camera.x + Camera.w)/32)&&(j >= Camera.y / 32) && (j < (Camera.y + Camera.h)/32))
              DrawGreySprite(EntityGroundList[j][i]->sprite,fogofwar,EntityGroundList[j][i]->s.x,EntityGroundList[j][i]->s.y,EntityGroundList[j][i]->frame);
            }
            if(IsSelected(EntityGroundList[j][i]))
            {
              UnselectEntity(EntityGroundList[j][i]);
            }
          }
        }

      }
      else if(!GameMap.playerviews[ThisPlayer].seeing[j][i])
      {
          if((EntityGroundList[j][i] != NULL)&&(EntityGroundList[j][i] != &DEADSPACE))
           if((EntityGroundList[j][i]->Unit_Type == ET_World)||(EntityGroundList[j][i]->Unit_Type == ET_Building))
           {
             if((i >= Camera.x / 32) && (i < (Camera.x + Camera.w)/32)&&(j >= Camera.y / 32) && (j < (Camera.y + Camera.h)/32))
             DrawGreySprite(EntityGroundList[j][i]->sprite,fogofwar,EntityGroundList[j][i]->s.x,EntityGroundList[j][i]->s.y,EntityGroundList[j][i]->frame);
           }
      }
    }
  }
}

/*make sure that whatever my units can see, I can see as well*/
void EntitySeeing(int mx,int my,int sight,int Player)
{
  int x, y,v;
  float r2;
  r2 = sight * sight;
  for (x = sight * -1; x <= sight; x++)
  {
    if((x + mx >= 0)&&(x + mx < GameMap.map.w))
    {
      y = (int) (sqrt(r2 - x*x) * 0.9);
      for(v = y * -1; v <= y;v++)
      {
        if((my + v >= 0)&&(my + v < GameMap.map.h))
          GameMap.playerviews[Player].seeing[my  + v][x + mx] = 1;
      }
    }
  }
}

void EntitySight(int mx,int my,int sight,int Player)
{
  int x, y,v;
  float r2;
  r2 = sight * sight;
  for (x = sight * -1; x <= sight; x++)
  {
    if((x + mx >= 0)&&(x + mx < GameMap.map.w))
    {
      y = (int) (sqrt(r2 - x*x) * 0.9);
      for(v =  y * -1; v <= y;v++)
      {
        if((my + v >= 0)&&(my + v < GameMap.map.h))
          if(!GameMap.playerviews[Player].seen[my + v][x + mx])
          {
            GameMap.playerviews[Player].seen[my + v][x + mx] = 1;
            DrawFilledRect((x + mx) *32,(my + v) * 32,32,32,IndexColor(White),unseenmask);
          }
      }
    }
  }
}

int GetPathStep(Entity *self, int gx, int gy)
{
  int sx = (self->m.x/20)*5;/*the map was summarized by 4 in each direction and then only every 5 of these were used*/
  int sy = (self->m.y/20)*5;
  int tx = (gx / 20)*5;
  int ty = (gy / 20)*5;
  int bestx,besty = -1;
  int i, ox,oy;
  int offset = MAP_H / PATH_REZ;
  Uint16 best = 32000;
  for (i = 0;i < 8;i++)
  {
    switch(i)
    {
      case 0:
        ox = -5;
        oy = -5;
      break;
      case 1:
        ox = 0;
        oy = -5;
      break;
      case 2:
        ox = 5;
        oy = -5;
      break;
      case 3:
        ox = -5;
        oy = 0;
      break;
      case 4:
        ox = 5;
        oy = 0;
      break;
      case 5:
        ox = -5;
        oy = 5;
      break;
      case 6:
        ox = 0;
        oy = 5;
      break;
      case 7:
        ox = 5;
        oy = 5;
      break;
    }
    if((GameMap.map.path[sx + ox + (sy + oy * offset)][tx + (ty * offset)][P_DISTANCE] < best)&&(GameMap.map.path[sx + ox + (sy + oy * offset)][tx + (ty * offset)][P_DISTANCE] != -1))
    { /*if we have a good candidate thats not a dead end*/
      bestx = sx + ox;
      besty = sy + oy;
      best = GameMap.map.path[sx + ox + (sy + oy * offset)][tx + (ty * offset)][P_DISTANCE];
    }
  }
  if(best < 32000)
  {
    self->Unit->WalkTarget.x = bestx * 4;
    self->Unit->WalkTarget.y = besty*4;
    /*or something else to set the path*/
    return 1;
  }
  return 0;
}

int AtGoal(Entity *self,int gx,int gy)
{
  if((self->m.x == gx)&&(self->m.y == gy))
    return 1;
  return 0;
}

void MoveTo(Entity *self, int tx, int ty)/*set the movement vector based on a target location*/
{
  float dx,dy;
  tx *= 32; /*we get the info as a tile, we'll set it to the absolute*/
  ty *= 32;
  dx = tx - self->s.x;
  dy = ty - self->s.y;
  ScaleVectors(&dx,&dy);
  dx *= self->movespeed;
  dy *= self->movespeed;
  self->v.x = dx;
  self->v.y = dy;
  if(dx < -2)
  {
    if(dy < -2)self->face = F_NW;
    else if(dy > 2)self->face = F_SW;
    else self->face = F_West;
  }
  else if(dx > 2)
  {
    if(dy < -2)self->face = F_NE;
    else if(dy > 2)self->face = F_SE;
    else self->face = F_East;
  }
  else
  {
    if(dy < -2)self->face = F_North;
    else if(dy > 2)self->face = F_South;
  }
}

void GetSXfromMX(Entity *self,int *sx, int *sy)
{
 *sx = (self->m.x * 32) + 16 - (self->sprite->w /2); /*sync changes here with MoveUnit*/
 *sy = (self->m.y * 32) + 16 - self->sprite->h;
}

/*
  this is going to be used by every entity's update function
  Return 1 if the entity was able to move, or 0 if there was something in the way.
*/

int MoveUnit(Entity *self)
{
 int i;
 int xposition = (self->s.x + self->v.x - 16 + (self->sprite->w /2))/32;   /*sync changes here with GetSXfromMX*/
 int yposition = (self->s.y + self->v.y + self->sprite->h - 16)/32;
 fprintf(stderr,"Got Here");
 if(((yposition >= 0)&&(xposition >= 0)&&(yposition < GameMap.map.h)&&(xposition < GameMap.map.w))&&((EntityAirList[yposition][xposition] == NULL)||(EntityAirList[yposition][xposition] == self)))
 {
/*  for(i = 0; i < MAXTRAIL - 1;i++)
  {
    self->Unit->Trail[MAXTRAIL - 1 - i].x = self->Unit->Trail[MAXTRAIL - 2 - i].x;
    self->Unit->Trail[MAXTRAIL - 1 - i].y = self->Unit->Trail[MAXTRAIL - 2 - i].y;
 }
  self->Unit->Trail[0].x = self->s.x;
   self->Unit->Trail[0].y = self->s.y;*/
   self->s.x += self->v.x;
  self->s.y += self->v.y;
  if((xposition != self->m.x)||(yposition != self->m.y))
  {
    EntityAirList[self->m.y][self->m.x] = NULL;
    self->m.x = xposition;
    self->m.y = yposition;
    EntityAirList[self->m.y][self->m.x] = self;
  }
  return 1;
 }
// self->state = ST_IDLE;
 return 0;
}

int LinedUpOnMX(Entity *self)
{
  int checkx,checky;
  GetSXfromMX(self,&checkx,&checky);
  if(fabs(self->s.x - checkx) > 8)return 0;
  if(fabs(self->s.y - checky) > 8)return 0;
  return 1;
}

void GravitateToMX(Entity *self)
{
  int targetx,targety;
  GetSXfromMX(self,&targetx,&targety);
  self->s.x += (targetx - self->s.x)/4;
  self->s.y += (targety - self->s.y)/4;
}

void SetSXfromMX(Entity *self)
{
  GetSXfromMX(self,&self->s.x, &self->s.y);
}

/*
 * This handly little function will figure pace out a looping animation for an entity.
 * It could probably have been a macro...oh well.
 */
int GetNextCount(Entity *self)
{
  return (self->fcount+1)%(self->framestates[self->state + 1] - self->framestates[self->state]);
}

int Collide(SDL_Rect box1,SDL_Rect box2)
{
  /*check to see if box 1 and box 2 clip, then check to see if box1 is in box or vice versa*/
  if((box1.x + box1.w >= box2.x) && (box1.x <= box2.x+box2.w) && (box1.y + box1.h >= box2.y) && (box1.y <= box2.y+box2.h))
    return 1;
  return 0;
}

void ScaleVectors(float *vx, float *vy)
{
  double hyp;
  hyp = sqrt((*vx * *vx) + (*vy * *vy));
  hyp = 1 / hyp;
  *vx = (*vx * hyp);
  *vy = (*vy * hyp);
}


/*
  This is the list of specific instances of entities.
*/

/**/







