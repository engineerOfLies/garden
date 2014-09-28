#ifndef __GAME__
#define __GAME__

/*
 *    This file contains the structures and prototypes for the GAME itself
 */


typedef struct Game_T
{
  int level;
  int score;
  int NumZombies;
  int NumGraves;
  int NumVillagers;
}Game;


void NewGame(Game *game);   /*resets the data for a new game.*/
void NewLevel(Game *game, int graves,int villagers);  /*sets up a new level*/
void DeadZombie(Game *game);  /*sets the game info to he beone less zombie.. possible game over.*/
void GameOptions(Game *game,int options);   /*new game, quit, select difficulty*/
void GrownZombie(Game *game);   /*sets a new zombie towards the scoreboard*/








#endif
