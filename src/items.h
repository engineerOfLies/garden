#ifndef __ITEMS__
#define __ITEMS__

#include "graphics.h"

/*
 *  Items.h
 *  Describes the information about each item in the game.
 *  Associate data types are for static info and a second data type for altering stats of the item.
 */

#define NAMELEN 40
 
typedef struct ITEM_T
{
  /*engine related data*/
  char name[NAMELEN];
  char sprite[20];
  int picindex;
  /*game related data*/
  float weight;
  int matstr; /*material strength*/
  int material; /*this will eventually be an enumeration of materials*/
  int supertype; /*Super type will be general: 2 handed weapon,, single handed weapon, armor, tool, etc*/
  int subtype;   /*sub type will be specific, sword, dagger, axe, musical instrument, shield, gem, etc*/
  int skill;     /*associate skill level, can be quality, difficulty to learn something*/
  int basevalue; /*how much lupen this is worth at base*/
  int damage,bonus;    /*for weapons, number of die to roll for damage*/
  int reduction;  /*how much damage is soaked by a successful attack from this armor*/
  int penetration;/*for weapons, penetration; for armor protection*/
  
}Item;

typedef struct ITEM_INSTANCE
{
  int index; /*refering to item description*/
  int damage,bonus;      /*for weapons, number of die to roll for damage*/
  int reduction;     /*how much damage is soaked by a successful attack from this armor*/
  int penetration;   /*for weapons, penetration; for armor protection*/
  int prefix,postfix;  /*for adding things like, Mythril -foo- OF DEATH*/
  int skilllevel;       /*based on above skill, but will be dynamic*/
  int subsubtype;       /*if it was a Scroll, Necromancy sub type, this would be index of the necromancy spell.*/
  int weight;
  int amount;           /*you may have 12 orcish spears of the apocalypse*/
}item;

#endif
/*newline at end of file:*/
