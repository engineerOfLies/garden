#ifndef __HERO__
#define __HERO__

/*
 *    Hero.h
 *    File contains data structures and Prototypes for Character Maintainence
 *
 */
#include "entity.h"
#include "items.h"

/*
 *  Many things in TCoA can be affected by status effects.  They will range from simple poisons and sicknesses to
 *  Frozen, shocked, stunned, stoned (DUDE!), confused, etc.
 */
typedef struct STATUS_E
{
  int affect;
  int duration;   /*-1 implies infinite*/
  int degree;
}Effect;

/* every stat in TCoA is a trainable skill or a derivitive of a set of skills
 * Skills are trained through use and related skils will train each other.
 */
typedef struct SKILL_T
{
  Uint16   score;
  Uint8    minipoints;
  int      bonus;
  int      skillindex;  /*index in the global list, that contains all the static info about the skills*/
  int      skilltype;   /*used to determine which list the skill is described in. not all need this*/
}skill;

typedef struct SKILL_LIST
{
  char name[25];
  int  bonusfrom;
  int  attributes[6]; /*which attributes get trained from the use of this skill or spell or ability*/
  int  trains[8];
  char fileicon[30];  /*the path and file that is used to represent this skill*/
  int  frameindex;     /*the Index of this icon in the above sprite sheet*/
  char description[160];
  int  degree;        /*damage, healing amount, anything that guages the strength of a spell or ability*/
  int  duration;      /*how long something will last*/
  int  range;         /*how far away, a skill can be targeted*/
}skill_list;

typedef struct Character_T
{
  char  Name[30];
  int   bonus;
  int   race,subrace;
  int   trainingclass,subclass;
  int   traits[10];            /*intrinsic traits and bogies that the player starts with and acquires.*/
  int   karma,honor;            /*absolute moral measure of actions & religion specific*/
  /*these skills are formed from the basic attributes*/
  float health,stamina,mana;
  int   healthmax,staminamax,manamax;
  int   speed,perception,sightradius;
  float nightperception;       /*modifier for perception at night*/
  int   capacity;              /*how much weight can be carried before being encoumbered*/
  int   bulk;                  /*how much is being carried at the moment*/
  /*basic attributes of all characters*/
  skill attributes[6];         /*strength,endurance, agility, dexterity, intelligence, magic*/
  skill skills[50];            /*the ifrst 4 are attack, defense, throwing, climbing, al others are optional*/
  int   numskills;             /*how many skills that the hero knows*/
  skill spells[100];           /*spells are optional, at the maximum a mage can learn up to 100 spells*/
  int   numspells;             /*how many spells that the hero knows*/
  skill techniques[50];        /*reserved for special attacks, abilities and as a catch all*/
  int   numtechs;              /*how many techniques that the hero knows*/
  skill formulae[50];          /*these will refer to a specialized list that describe needed items.*/
  int   numformulae;           /*how many formulae that the hero knows*/
  /*inventory*/
  int   equipped[12];          /*head,neck,body,lhand,rhand,feet,cloak,wrists,hands,ring1,ring2*/
  item  inventory[100];        /*no more than 100 types of items can be carried, but probably less due to weight restrictions*/
  int   SLupens,Lupens,RLupens; /*universal units of currency in dothal.*/
  float Elements[10];          /*there are 10 elements in the game.  they can effect damage, and healing*/
  Effect statuseffects[30];   /*only 30 things can affect the player at once.  This includes 10 posative as well 
                                as 10 negative ones.  the last 10 are perminent things, like Undead, Werewolf, etc.*/
  /*game engine info*/
  char spritesheet[30];
  char headshot[30];
  int  hsframe;
  int  c1,c2,c3;
}Hero;

typedef struct HERO_ENT
{
  int used;
  int player;     /*the player in control of this hero*/
  Hero hero;
  Sprite *Headshot;
}HeroEnt;

/*skill related functions*/
void TrainSkill(skill *trainee,Uint8 minipoints);
void Matriculate(Hero *hero,skill *growth);
int Roll(int die);
int  SkillRoll(int *CIH);     /*returns skill roll complete with chances in hell and blunders*/
int  Skillcheck(skill *check,int bonuses,int challenge); /*-1, if less; 0 if even; 1 if greater*/
int  SkillcheckVs(skill *check, int bonuses,skill *otherskill,int otherbonuses);
int  DetermineRange(skill *target,int bonuses);
skill_list *GetSkillByIndex(int index,int type);
skill *GetHeroSkillByIndex(Hero *hero,int index,int type);

/*Hero character related functions*/
HeroEnt *SpawnHero(int player);                 /*init a blank hero instance*/
void Rest(Hero *hero,int duration,int type);                                   /*Will Update stats and then regain some H,S,M*/
void UpdateStats(Hero *hero);                              /*runs matriculations for all skills and then this will
                                                             update the maximums and everything for the player.*/
void BecomeClass(Hero *hero,int classindex,int suboptions);/*makes the hero of the class specified, suboptions
                                                              are for classes with sub options: paladins, mages, etc*/
void BecomeRace(Hero *hero,int race,int subrace);          /*applied after class is specified, subraces are for
                                                            aruran, luyet, humon, etc*/
void ApplyClassRaceBonuses();                              /*used when the character rests to make sure bonuses based
                                                             on percentages are maintained*/
int LearnSkill(Hero *hero,int skillindex,int score);                /*adds a skill to the hero's rep*/
int LearnSpell(Hero *hero,int skillindex,int score);
int LearnTechnique(Hero *hero,int skillindex,int score);
int LearnFormula(Hero *hero,int skillindex,int score);

void SpawnHeroEnt(int mx, int my,int Player,char sprite[30],int c1,int c2,int c3);

#endif
/*new line at end of file:*/
