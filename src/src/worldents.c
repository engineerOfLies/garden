#include <string.h>
#include <stdlib.h>
#include "entity.h"

extern Entity *EntityGroundList[MAP_H][MAP_W];


void SpawnGold(int mx,int my,int stone)
{
  Entity *tree;
  int i,j;
  tree = NewEntity(NUMPLAYERS);
  if(tree == NULL)
  {
    fprintf(stdout,"Unable to allocate room for a Gold encrusted mountain..\n");
    return;
  }
  for(i = 0;i < SOUNDSPERENT;i++)tree->sound[i] = NULL;
  tree->selectoffsety = -30;
  tree->owner = NULL;
  tree->target = NULL;
  tree->think = NULL;
  tree->update = NULL;
  tree->inputcommand = NULL;
  tree->Unit = NULL;
  tree->Building = NULL;
  tree->Unit_Type = ET_World; /*resource, building, unit.*/
  strcpy(tree->EntName,"Gold"); /*the name of the unit or building*/
  tree->sprite = LoadSprite("images/gold.png",96,70);
  SDL_SetColorKey(tree->sprite->image, SDL_SRCCOLORKEY , IndexColor(Black));
  tree->NextThink = 0;
  tree->ThinkRate = 0;  /*used for incrementing above*/
  tree->NextUpdate = 0; /*used for how often the entity is updated, updating is merely animations*/
  tree->UpdateRate = 0;  /*used for incrementing above*/
  tree->shown = 1; /*if 1 then it will be rendered when it is on screen*/
  tree->frame = 1; /*this will be randome later on...*/
  tree->s.x = mx * 32;
  tree->s.y = my * 32;
  tree->m.x = mx;
  tree->m.y = my;
  tree->tilesize.x = 3;
  tree->tilesize.y = 2;
  tree->health = stone;
  tree->frame = 0;
  tree->healthmax = stone;/*Resources use it for how much of a resource it has left*/
  for(j = 0;j < tree->tilesize.y;j++)
  {
    for(i = 0; i < tree->tilesize.x;i++)
      EntityGroundList[my + j][mx + i] = tree;
  }
}

void SpawnStone(int mx,int my,int stone)
{
  Entity *tree;
  int i,j;
  tree = NewEntity(NUMPLAYERS);
  if(tree == NULL)
  {
    fprintf(stdout,"Unable to allocate room for a stone mountain..\n");
    return;
  }
  for(i = 0;i < SOUNDSPERENT;i++)tree->sound[i] = NULL;
  tree->selectoffsety = -30;
  tree->owner = NULL;
  tree->target = NULL;
  tree->think = NULL;
  tree->update = NULL;
  tree->inputcommand = NULL;
  tree->Unit = NULL;
  tree->Building = NULL;
  tree->Unit_Type = ET_World; /*resource, building, unit.*/
  strcpy(tree->EntName,"Stone"); /*the name of the unit or building*/
  tree->sprite = LoadSprite("images/stone.png",96,70);
  SDL_SetColorKey(tree->sprite->image, SDL_SRCCOLORKEY , IndexColor(Black));
  tree->NextThink = 0;
  tree->ThinkRate = 0;  /*used for incrementing above*/
  tree->NextUpdate = 0; /*used for how often the entity is updated, updating is merely animations*/
  tree->UpdateRate = 0;  /*used for incrementing above*/
  tree->shown = 1; /*if 1 then it will be rendered when it is on screen*/
  tree->frame = 1; /*this will be randome later on...*/
  tree->s.x = mx * 32;
  tree->s.y = my * 32;
  tree->m.x = mx;
  tree->m.y = my;
  tree->tilesize.x = 3;
  tree->tilesize.y = 2;
  tree->health = stone;
  tree->frame = 0;
  tree->healthmax = stone;/*Resources use it for how much of a resource it has left*/
  for(j = 0;j < tree->tilesize.y;j++)
  {
    for(i = 0; i < tree->tilesize.x;i++)
      EntityGroundList[my + j][mx + i] = tree;  
  }
}

void SpawnTree(int mx,int my,int lumber)
{
  Entity *tree;
  int i;
  tree = NewEntity(NUMPLAYERS);
  if(tree == NULL)
  {
    fprintf(stdout,"Unable to allocate room for a tree.. \na tree, now thats not asking alot is it?\n");
    return;
  }
  for(i = 0;i < SOUNDSPERENT;i++)tree->sound[i] = NULL;
  tree->selectoffsety = 0;
  tree->owner = NULL;
  tree->target = NULL; 
  tree->think = NULL;
  tree->update = NULL;
  tree->inputcommand = NULL;
  tree->Unit = NULL;
  tree->Building = NULL;
  tree->Unit_Type = ET_World; /*resource, building, unit.*/
  strcpy(tree->EntName,"Tree"); /*the name of the unit or building*/
  tree->sprite = LoadSprite("images/trees.png",50,90);
  SDL_SetColorKey(tree->sprite->image, SDL_SRCCOLORKEY , IndexColor(Black));
  tree->NextThink = 0;
  tree->ThinkRate = 0;  /*used for incrementing above*/
  tree->NextUpdate = 0; /*used for how often the entity is updated, updating is merely animations*/
  tree->UpdateRate = 0;  /*used for incrementing above*/
  tree->shown = 1; /*if 1 then it will be rendered when it is on screen*/
  tree->frame = 1; /*this will be randome later on...*/
  tree->s.x = mx * 32 - 9;
  tree->s.y = my * 32 - 74;
  tree->m.x = mx;
  tree->m.y = my;
  tree->tilesize.x = 1;
  tree->tilesize.y = 1;
  tree->health = rand() % lumber;
  if(tree->health > (lumber *3)/4)tree->frame = 0;
  else if (tree->health > lumber/2)tree->frame = 1;
  else tree->frame = 2;
  tree->healthmax = lumber;/*Resources use it for how much of a resource it has left*/
  EntityGroundList[my][mx] = tree;
}




