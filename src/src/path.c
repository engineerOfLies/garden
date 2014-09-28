#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "path.h"
#include "map.h"
#include "graphics.h"
#include "entity.h"

#define MAXNODES  4096

int CanTrace(int sx, int sy, int gx, int gy);

extern Map GameMap; /*this is the map we'll be using to test.*/
extern SDL_Surface *background;
MapInfo map;

int Rez;  /*Resolution for Summarizing*/
int PathLength;
int NodeSize = sizeof(Node);

Node FreeList[MAXNODES];
Node ClosedList[MAXNODES];
Node FinalPath[MAXNODES];
Node SuccessorList[4];/*this is what is filled with Get Successor and used to compare */


/*this function will call the path finding functions to solve how to get from every node to every other node.
  This will be called by the map editing program to 'compile' a map, so it can be used by the game easily.*/
void MapWayPoints()
{
  int i,j,u,v;/*OOOHHHH nelly, is this gonna be a LOOOONG function to call.*/
    /*i and u are the x traversal, j and v are the y traversal*/
  int p; /*path stepping variable*/
  int Offset = MAP_H / PATH_REZ;
  memset(GameMap.map.path,-2,sizeof(GameMap.map.path));
  for(j = 0; j < Offset;j+=5)
  {
    for(i = 0; i < (MAP_W / PATH_REZ);i+=5)
    {
      for(v = 0; v < Offset;v+=5)
      {
        for(u = 0; u < (MAP_W / PATH_REZ);u+=5)
        {
          /*first lets check to see if we're checking for ourself*/
          fprintf(stdout, "%i, %i\n",i + (j * Offset),u + (v * Offset));
          if((i == u)&&(j == v))
          {
            GameMap.map.path[i + (j * Offset)][u + (v * Offset)][P_GOAL] = i + (j * Offset);
            GameMap.map.path[i + (j * Offset)][u + (v * Offset)][P_DISTANCE] = 0;
          }
          if(GameMap.map.path[i + (j * Offset)][u + (v * Offset)][P_GOAL] == -2)
          { /*if we already have the reverse done, lets not do it again*/
            if(FindPath(i, j, u,v) == -1)
            { /*goal node unreachable*/
              GameMap.map.path[i + (j * Offset)][u + (v * Offset)][P_GOAL] = -1;
              GameMap.map.path[i + (j * Offset)][u + (v * Offset)][P_DISTANCE] = -1;
              GameMap.map.path[u + (v * Offset)][i + (j * Offset)][P_GOAL] = -1;
              GameMap.map.path[u + (v * Offset)][i + (j * Offset)][P_DISTANCE] = -1;
            }
            else
            { /*we have a path*/
              GameMap.map.path[i + (j * Offset)][u + (v * Offset)][P_GOAL] = FinalPath[PathLength - 1].x + (FinalPath[PathLength - 1].y * Offset);
              GameMap.map.path[i + (j * Offset)][u + (v * Offset)][P_DISTANCE] = PathLength;
              GameMap.map.path[u + (v * Offset)][i + (j * Offset)][P_GOAL] = FinalPath[1].x + (FinalPath[1].y * Offset);
              GameMap.map.path[u + (v * Offset)][i + (j * Offset)][P_DISTANCE] = PathLength;
            }
          }
        }
      } 
    }
  }
}

void SetEntPath(Entity *self)
{
  int  checked = 0;
  int i,j = 0;
  while(checked < PathLength)
  {
    if(FinalPath[j].used)
    {
      self->Unit->PathList[i].x = FinalPath[j].x * Rez;
      self->Unit->PathList[i].y = FinalPath[j].y * Rez;
      i++;
      checked++;
    }
    j++;
  }
}

int TileBlock(Uint16 tile)
{
  switch(tile)
  {
    case 16:
    case 21:
    case 22:
    case 23:
    case 24:
      return 1;
    default:
      return 0;
  }
  return 1;
}

void SummarizeMap()
{
  int x,y,i,j;
  int count = 0;
  for(y = 0;y < MAP_H / Rez;y++)
  {
    for(x = 0;x < MAP_W / Rez;x++)
    {
      switch(Rez)
      {
        case 1:
          map.tile[y][x] = TileBlock(GameMap.map.tile[y][x]);
        case 2:
          map.tile[y][x] = (TileBlock(GameMap.map.tile[y * 2][x * 2]) + TileBlock(GameMap.map.tile[y * 2][x * 2 + 1]) + TileBlock(GameMap.map.tile[y * 2 + 1][x * 2]) + TileBlock(GameMap.map.tile[y * 2 + 1][x * 2 + 1]) + 2)/6;
        break;
        case 4:
          map.tile[y][x] = (TileBlock(GameMap.map.tile[y * 4][x * 4]) + TileBlock(GameMap.map.tile[y * 4][x * 4 + 1]) + TileBlock(GameMap.map.tile[y * 4 + 1][x * 4]) + TileBlock(GameMap.map.tile[y * 4 + 1][x * 4 + 1]) + TileBlock(GameMap.map.tile[y * 4][x * 4 + 2]) + TileBlock(GameMap.map.tile[y * 4][x * 4 + 3]) + TileBlock(GameMap.map.tile[y * 4 + 1][x * 4 + 2]) + TileBlock(GameMap.map.tile[y * 4 + 1][x * 4 + 3]) + TileBlock(GameMap.map.tile[y * 4 + 2][x * 4]) + TileBlock(GameMap.map.tile[y * 4 + 2][x * 4 + 1]) + TileBlock(GameMap.map.tile[y * 4 + 3][x * 4]) + TileBlock(GameMap.map.tile[y * 4 + 3][x * 4 + 1]) + TileBlock(GameMap.map.tile[y * 4 + 2][x * 4 + 2]) + TileBlock(GameMap.map.tile[y * 4 + 2][x * 4 + 3]) + TileBlock(GameMap.map.tile[y * 4 + 3][x * 4 + 2]) + TileBlock(GameMap.map.tile[y * 4 + 3][x * 4 + 3]) + 4)/20;
        break;
        case 8:
          count = 0;
          for(j = 0;j < 8;j++)
          {
             for(i = 0;i < 8;i++)
             {
               count += TileBlock(GameMap.map.tile[(y * 8)+j][(x * 8)+i]);
             }
          }
          map.tile[y][x] = count / 128;
        break;
      }
    }
  }
}

/*putting it all together*/
int FindPath(int sx, int sy, int gx, int gy)
{
  int done = 0;
  int failed = 0;
  int len;
  Uint32 GoalDist = fabs(sx - gx) + fabs(sy - gy);
//  Uint32 FinalTime;
 // Uint32 Time;
  //Uint32 StartTime = SDL_GetTicks();
  Node *node;
  PathLength = 0;
/*
  if(GoalDist > 100)Rez = 8;
  else if(GoalDist > 45)Rez = 4;
  else if(GoalDist > 22)Rez = 2;
  else Rez = 1;
*/
  Rez = PATH_REZ;
  ClearLists();
  SummarizeMap();
  sx /= Rez;
  sy /= Rez;
  gx /= Rez;
  gy /= Rez;
  StartNode(sx,sy,gx,gy);
  while(!done)
  {
    node = GetNextOpen();
    if(IsGoalNode(node,gx,gy))
    {
      done = 1;
      MoveNodeFromList(node,L_Closed);
      len = BuildFinalPath(gx,gy);
 //     CullFinalPath(len);
 //     DrawPath(len);
//      return SDL_GetTicks() - StartTime;
      return len;
    }
   //                   Time = SDL_GetTicks();
    AddSuccessorsToList(node,gx,gy);
                     //FinalTime += SDL_GetTicks()- Time;
 //                     fprintf(stdout,"this %i ticks.\n",SDL_GetTicks()- Time);
    if(DidFail())
    {
      failed = 0;
      done = 1;
      return -1;
    }
  }
  return -1;
}

void CullFinalPath(int length)
{
  int i,j,k;
  int check;
  PathLength = length;
  for(i = length;i > 0;--i)
  {
    if(FinalPath[i].used)
    {
      j = i;
      check = 1;
      do
      {
        j--;
        if(j > 0)
        {
          if(!FinalPath[j].used)continue;
          check = CanTrace(FinalPath[i].x,FinalPath[i].y,FinalPath[j].x,FinalPath[j].y);
        }
        else check = 0;
      }
      while(check);
      for(k = i - 1;k > j + 1;--k)
      {
        if(k > 0)
        {
          FinalPath[k].used = 0;
          PathLength--;
        }
        else break;
      }
    }
  }
}

int BuildFinalPath(int gx,int gy)/*length*/
{
  int px,py;
  int done = 0;
  int count = 0;
  Node *node = GetNodeFromList(gx,gy,L_Closed);
  while(!done)
  {
    node->x *= Rez;
    node->y *= Rez;
    px = node->parentx;
    py = node->parenty;
    MoveNodeFromList(node,L_Final);
    if(px <= -1)
    {
      return count;
    }
    count++;
    node = GetNodeFromList(px,py,L_Closed);
    if(node == NULL)break;
  }
  return 0;
}

/*
void DrawPath(int length)
{
  int i;
  for(i = length;i > 0;--i)
  {
    if(FinalPath[i].used)
      DrawSprite(GameMap.textures,background,FinalPath[i].x * 32,FinalPath[i].y * 32, 16);
  }
}
*/
int CanMove(int mx, int my)     /*obviously this will be more complex once real information is created*/
{
  if((my < MAP_H/Rez)&&(my >= 0)&&(mx < MAP_W/Rez)&&(mx >= 0))
  {
//    if(Rez < 8)
    {
      if(map.tile[my][mx] < Rez)
      {
        return 1;
      }
    }
//    else if(map.tile[my][mx] < 16)return 1;
  }
  return 0;
}

int IsGoalNode(Node *node,int gx,int gy)
{
  if((node->x == gx)&&(node->y == gy))return 1;
  return 0;
}

int CompareCost(Uint32 cost, Node *node)
{
  return (cost < node->cost);
}

Uint32 EstimateGoalCost(Node *node,int gx,int gy)
{
  int dx,dy;

  dx = node->x - gx;
  dy = node->y - gy;

  return fabs(dx) + fabs(dy);/*we don't have to be soo accurate since we're dealing with relatives here*/

}

int DidFail()
{
  int i,count;
  count = 0;
  for(i = 0;i < MAXNODES;i++)
  {
    if(FreeList[i].used == 1)
    {
      count++;
    }
  }
  if(count > 0)return 0;
  return 1;
}

Node *GetNextOpen()
{
  int i;
  Uint32 bestf = 2000;
  Node *node = NULL;
  for(i = 0;i < MAXNODES;i++)
  {
    if(FreeList[i].used == 1)
    {
      if(FreeList[i].estimate + FreeList[i].cost < bestf)
      {
        bestf = FreeList[i].estimate + FreeList[i].cost;
        node = &FreeList[i];
      }
    }
  }
  return node;
}

int AddSuccessorsToList(Node *node,int gx,int gy) /*returns zero if there were no successors, -1 if there was no room in list*/
{
  Node *freenode;
  Node *closednode;
  int i, x,y,newcost;
  ClearSuccessorList();
  for(i = 0; i < 4; i++)
  {
    switch(i)
    {
      case 0:
        x = -1;
        y = 0;
      break;
      case 1:
        x = 1;
        y = 0;
      break;
      case 2:
        x = 0;
        y = -1;
      break;
      case 3:
        x = 0;
        y = 1;
      break;
    }
    if(CanMove(node->x + x, node->y + y))
    {
      newcost = (node->cost + 1); //attempting to make things very shitty, yet fast.
      freenode = GetNodeFromList(node->x + x, node->y + y,L_Free);
      if(freenode != NULL) /*we do occure in the open list already*/
      {
          continue;/*if its already there ignore it*/
      }
      closednode = GetNodeFromList(node->x + x, node->y + y,L_Closed);
      if(closednode != NULL) /*we do occure in the open list already*/
      {
//        if(!CompareCost(newcost, closednode))/*if our path of getting there isn't better, then ignore it*/
//        {
          continue;
//        } 
      }
/*      if(closednode != NULL)
      {
        closednode->cost = newcost;
        closednode->total = newcost + closednode->estimate;
        closednode->parentx = node->x;
        closednode->parenty = node->y;
        MoveNodeFromList(closednode,L_Free);
        continue;
      }
*/
      freenode = NewNode(L_Free);
      freenode->cost = newcost;
      freenode->estimate = EstimateGoalCost(freenode,gx,gy);
      freenode->x = node->x + x;
      freenode->y = node->y + y;
      freenode->parentx = node->x;
      freenode->parenty = node->y;
    }
  }
  /*all of my offspring have been catalogued*/
  MoveNodeFromList(node,L_Closed);
  return 1;
}

void StartNode(int sx,int sy,int gx,int gy)
{
  Node *node = NULL;
  node = NewNode(L_Free);
  if(node != NULL)
  {
    node->cost = 0;
    node->estimate = EstimateGoalCost(node,gx,gy);
    node->x = sx;
    node->y = sy;
    node->parentx = -1;/*starting location*/
    node->parenty = -1;
  }
}

/*
 *    List and Node handling functions follow.
 */

void ClearLists()
{
  int i;
  for(i = 0;i < MAXNODES;i++)
  {
    FreeList[i].used = 0;
    ClosedList[i].used = 0;
    FinalPath[i].used = 0;
  }
  for(i = 0;i < 4;i++)
  {
    SuccessorList[i].used = 0;
  }
}
 
void MoveNodeFromList(Node *src,int Dlist)
{
  Node *dst;
  dst = NewNode(Dlist);
  if(dst == NULL)
  {
    fprintf(stderr,"Couldn't allocate a needed node: %s\n", SDL_GetError());
    exit(1);
  }
  memcpy(dst,src,NodeSize);
//  dst->cost = src->cost;
//  dst->estimate = src->estimate;
//  dst->x = src->x;
//  dst->y = src->y;
//  dst->parentx = src->parentx;
//  dst->parenty = src->parenty;
  FreeNode(src);
}
 
Node *GetNodeFromList(int nx,int ny,int list)
{
  int i;
  switch(list)
  {
    case L_Free:
      for(i = 0;i < MAXNODES;i++)
      {
        if(FreeList[i].used)
        {
          if((FreeList[i].x == nx)&&(FreeList[i].y == ny))
            return &FreeList[i];
        }
      }
    break;
    case L_Closed:
      for(i = 0;i < MAXNODES;i++)
      {
        if(ClosedList[i].used)
        {
          if((ClosedList[i].x == nx)&&(ClosedList[i].y == ny))
            return &ClosedList[i];
        }
      }
    break;
    case L_Successor:
      for(i = 0;i < MAXNODES;i++)
      {
        if(SuccessorList[i].used)
        {
          if((SuccessorList[i].x == nx)&&(SuccessorList[i].y == ny))
            return &SuccessorList[i];
        }
      }
    break;
    case L_Final:
      for(i = 0;i < MAXNODES;i++)
      {
        if(FinalPath[i].used)
        {
          if((FinalPath[i].x == nx)&&(FinalPath[i].y == ny))
            return &FinalPath[i];
        }
      }
    break;
  }
  return NULL;
}

Node *NewNode(int list)
{
  switch(list)
  {
    case L_Free:
      return NewFreeNode();
    case L_Closed:
      return NewClosedNode();
    case L_Successor:
      return NewSuccessorNode();
    case L_Final:
      return NewFinalNode();
  }
  return NULL;
}

void ClearSuccessorList()
{
  int i;
  for(i = 0;i < 4;i++)
  {
    SuccessorList[i].used = 0;
  }
}

Node *NewSuccessorNode()
{
  int i;
  for(i=0;i < 4;i++)
  {
    if(SuccessorList[i].used == 0)
    {
      SuccessorList[i].used = 1;
      return &SuccessorList[i];
    }
  }
  return NULL;
}

Node *NewFinalNode()
{
  int i;
  for(i=0;i < MAXNODES;i++)
  {
    if(FinalPath[i].used == 0)
    {
      FinalPath[i].used = 1;
      return &FinalPath[i];
    }
  }
  return NULL;
}

Node *NewFreeNode()
{
  int i;
  for(i=0;i < MAXNODES;i++)
  {
    if(FreeList[i].used == 0)
    {
      FreeList[i].used = 1;
      return &FreeList[i];
    }
  }
  return NULL;
}

Node *NewClosedNode()
{
  int i;
  for(i=0;i < MAXNODES;i++)
  {
    if(ClosedList[i].used == 0)
    {
      ClosedList[i].used = 1;
      return &ClosedList[i];
    }
  }
  return NULL;
}

int CanTrace(int sx, int sy, int gx, int gy)
{
  Uint32 dx = fabs(gx - sx);
  Uint32 dy = fabs(gy - sy);
  int i;
  float slopey = (float)dy/dx;
  float slopex = (float)dx/dy;
  int slx = (int)slopex;
  int sly = (int)slopey;
  for(i = 0;i < dx;i++)
  {
    if(TileBlock(GameMap.map.tile[(sy + (i * sly))][sx + (i * slx)]) != 0)return 0;
  }
  return 1;
}

void FreeNode(Node *node)
{
  node->used = 0;
}

