#ifndef __PATH__
#define __PATH__

#include "SDL.h"
/*
 *  Path Finding Module
 *  Donald Kehoe
 *  Created: 3/6/05
 *  Description:
 *    This file provides the data structures and function prototypes for Path Finding.
 *  It uses an implementation of the A* algorithm.
 *  Last Modified : 3/8/05
 *
*/

typedef struct NODE_T
{
  int used; /*the flag used for checkng to see if the node is in use.*/
  int x,y;
  Uint32 cost; /*the cost to get here, g */
  Uint32 estimate; /*the cost to get from here to goal, h' */
  int parentx;/*to make sure that while I am moving nodes around, that I know where my parents are*/
  int parenty;
}Node;

enum LIST_Types {L_Free,L_Closed,L_Successor,L_Final};

/*node list handling functions*/
void MoveNodeFromList(Node *src,int Dlist);/*the node to move, the source list and destination list*/
Node *GetNextOpen();
Node *GetNodeFromList(int nx,int ny,int list);
Node *NewNode(int list);
Node *NewSuccessorNode();
Node *NewFreeNode();
Node *NewClosedNode();
Node *NewFinalNode();
void FreeNode(Node *node);
void ClearSuccessorList();
void ClearLists();

/*aStar functions*/
void MapWayPoints();
int DidFail();
void StartNode(int sx,int sy,int gx,int gy);
Uint32 EstimateGoalCost(Node *node,int gx,int gy);/*n is the node, g is the goal*/
int CanMove(int mx, int my); /*returns 1 if the map location is free or 0 if it is blocked*/
int IsGoalNode(Node *node,int gx,int gy);
int AddSuccessorsToList(Node *node,int gx,int gy); /*returns zero if there were no successors*/
int CompareCost(Uint32 cost, Node *node); /*returns 1 if the cost is less than the node's cost*/
int FindPath(int sx, int sy, int gx, int gy);
int BuildFinalPath(int gx,int gy);/*length*/
void DrawPath(int length);
int NumObstaclesToGoal(int sx,int sy,int gx,int gy);
void SummarizeMap();
void CullFinalPath(int length);
#endif

