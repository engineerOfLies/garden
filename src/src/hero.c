/*
 *  Hero.c
 *  This file contains all of the functionality needed to handle the upkeep of the hero character in game
 *  and behind the scenes.
 *  DJ Kehoe '05
 */
#include <stdlib.h>

#include "hero.h"
#include "items.h"

enum SKILL_TYPES     {ST_Attribute,ST_Skill,ST_Spell,ST_Technique,ST_Formula};
enum ATTRIBUTE_INDEX {A_Strength,A_Endurance,A_Dexterity,A_Agility,A_Intelligence,A_Magic};
enum RANGE_TYPES     {R_Point,R_Short,R_Medium,R_Long,R_Great};

/*NOTE TO SELF: DO NOT CHANGE THE ORDER OF THIS LIST, THE ABOVE ENUMERATION COUNTS ON IT IN THIS ORDER*/
skill_list Attributes[] =
{
  {
   "Strength",
   0,
   {0,0,0,0,0,0}, /*This IS an Attribute*/
   {-1,-1,-1,-1,-1,-1,-1,-1},/*skills train attributes, but skills can get bonuses from the attribute as it gets larger*/
   "images/equipment1.png",  /*the path and file that is used to represent this skill*/
   7,     /*the Index of this icon in the above sprite sheet*/
   "This is the basic measure of raw muscle power.  Strength is useful for a variety of skills and contributes to health.",
   0,        /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,      /*how long something will last*/
   0
  },
  {
   "Endurance",
   0,
   {0,0,0,0,0,0}, /*This IS an Attribute*/
   {-1,-1,-1,-1,-1,-1,-1,-1},/*skills train attributes, but skills can get bonuses from the attribute as it gets larger*/
   "images/equipment1.png",  /*the path and file that is used to represent this skill*/
   23,     /*the Index of this icon in the above sprite sheet*/
   "This is the basic measure of much punishment one can take.  Endurance is a component of health and stamina.",
   0,        /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,      /*how long something will last*/
   0
  },
  {
   "Dexterity",
   0,
   {0,0,0,0,0,0}, /*This IS an Attribute*/
   {-1,-1,-1,-1,-1,-1,-1,-1},/*skills train attributes, but skills can get bonuses from the attribute as it gets larger*/
   "images/equipment2.png",  /*the path and file that is used to represent this skill*/
   18,     /*the Index of this icon in the above sprite sheet*/
   "This is the basic measure of how precise one is with their hands.  Dexterity can improve the efficiency of spell use.",
   0,        /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,      /*how long something will last*/
   0
  },
  {
   "Agility",
   0,
   {0,0,0,0,0,0}, /*This IS an Attribute*/
   {-1,-1,-1,-1,-1,-1,-1,-1},/*skills train attributes, but skills can get bonuses from the attribute as it gets larger*/
   "images/generalspells.png",  /*the path and file that is used to represent this skill*/
   11,     /*the Index of this icon in the above sprite sheet*/
   "This is the basic measure of how nimble one is with their whole body.  Agility contributes to health and speed.",
   0,        /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,      /*how long something will last*/
   0
  },
  {
   "Intelligence",
   0,
   {0,0,0,0,0,0}, /*This IS an Attribute*/
   {-1,-1,-1,-1,-1,-1,-1,-1},/*skills train attributes, but skills can get bonuses from the attribute as it gets larger*/
   "images/equipment2.png",  /*the path and file that is used to represent this skill*/
   2,     /*the Index of this icon in the above sprite sheet*/
   "This is the basic measure of how much one knows.  This applies to skills, spells, and general life.  A Major component of Mana.",
   0,        /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,      /*how long something will last*/
   0
  },
  {
   "Magic",
   0,
   {0,0,0,0,0,0}, /*This IS an Attribute*/
   {-1,-1,-1,-1,-1,-1,-1,-1},/*skills train attributes, but skills can get bonuses from the attribute as it gets larger*/
   "images/generalspells.png",  /*the path and file that is used to represent this skill*/
   5,     /*the Index of this icon in the above sprite sheet*/
   "This is the basic measure of how much control one has over their inner mana.  Without Magic, one will not have mana, nor be able to cast spells.",
   0,        /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,      /*how long something will last*/
   0
  }
};

enum SKILL_INDEX {SK_Attack,SK_Defense,SK_Throwing,SK_Climbing};

skill_list Skills[] =
{
  {
   "Attack",
   A_Dexterity,
   {1,0,1,0,0,0},              /*This Trains these attributes*/
   {-1,-1,-1,-1,-1,-1,-1,-1},  /*skills train other skills*/
   "images/skillicons.png",    /*the path and file that is used to represent this skill*/
   0,                          /*the Index of this icon in the above sprite sheet*/
   "This is the most basic skill for attacking in combat.  Anyone can attack.  This will determine if you hit someone.",
   0,                          /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,                          /*how long something will last*/
   R_Point
  },
  {
   "Defense",
   A_Agility,
   {0,1,0,1,0,0},              /*This Trains these attributes*/
   {-1,-1,-1,-1,-1,-1,-1,-1},  /*skills train other skills*/
   "images/skillicons.png",    /*the path and file that is used to represent this skill*/
   1,                          /*the Index of this icon in the above sprite sheet*/
   "This is the most basic skill for dodging an attack.  This will determine if you successfully avoid getting hit.",
   0,                          /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,                          /*how long something will last*/
   0
  },
  {
   "Throwing",
   A_Dexterity,
   {0,0,1,0,0,0},              /*This Trains these attributes*/
   {-1,-1,-1,-1,-1,-1,-1,-1},  /*skills train other skills*/
   "images/skillicons.png",    /*the path and file that is used to represent this skill*/
   2,                          /*the Index of this icon in the above sprite sheet*/
   "This is the basic skill of throwing something.  Can be applied to anything light enough to throw.  Skill determines range and accuracy.",
   0,                          /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,                          /*how long something will last*/
   R_Short
  },
  {
   "Climbing",
   A_Agility,
   {0,0,0,1,0,0},              /*This Trains these attributes*/
   {-1,-1,-1,-1,-1,-1,-1,-1},  /*skills train other skills*/
   "images/skillicons.png",    /*the path and file that is used to represent this skill*/
   3,                          /*the Index of this icon in the above sprite sheet*/
   "This is the simple skill to be able to climb difficult surfaces and ropes.  Stairs and ladders no not require skill.",
   0,                          /*damage, healing amount, anything that guages the strength of a spell or ability*/
   0,                          /*how long something will last*/
   0
  }
};

skill_list *GetSkillByIndex(int index,int type)
{
  switch(type)
  {
    case ST_Attribute:
      return &Attributes[index];
    case ST_Skill:
      return &Skills[index];
    default:
      return NULL;
  }
}

skill *GetHeroSkillByIndex(Hero *hero,int index,int type)
{
  int i;
  switch(type)
  {
    case ST_Attribute:
      if(i < 6)return &hero->attributes[i];
      else return NULL;
    break;
    case ST_Skill:
      for(i = 0;i < hero->numskills;i++)
      {
        if(hero->skills[i].skillindex == index)return &hero->skills[i];
      }
      return NULL;/*Player does not possess this skill.*/
    break;
    case ST_Spell:
      for(i = 0;i < hero->numspells;i++)
      {
        if(hero->spells[i].skillindex == index)return &hero->spells[i];
      }
      return NULL;/*Player does not possess this spell.*/
    break;
    case ST_Technique:
      for(i = 0;i < hero->numtechs;i++)
      {
        if(hero->techniques[i].skillindex == index)return &hero->techniques[i];
      }
      return NULL;/*Player does not possess this skill.*/
    break;
    case ST_Formula:
      for(i = 0;i < hero->numformulae;i++)
      {
        if(hero->formulae[i].skillindex == index)return &hero->formulae[i];
      }
      return NULL;/*Player does not possess this skill.*/
    break;
  }
  return NULL;
}


void TrainSkill(skill *trainee,Uint8 minipoints)
{
  trainee->minipoints += minipoints;  
}


void Matriculate(Hero *hero,skill *growth)
{
  int i;
  skill *trains;
  skill_list *skillinfo;
  skillinfo = GetSkillByIndex(growth->skillindex,growth->skilltype);
  while(growth->minipoints >= 10)
  {
    growth->score++;
    growth->minipoints -= 10;
    for(i = 0;i < 6;i++)
    {
      if(skillinfo->attributes[i] == 1)
      {
        TrainSkill(&hero->attributes[i],5);
      }
    }
    for(i = 0;i < 8;i++)
    {
      if(skillinfo->trains[i] != -1)
      {
        trains = GetHeroSkillByIndex(hero,growth->skillindex,growth->skilltype);
        if(trains != NULL)
          TrainSkill(trains,5);
      }
    }
  }
}

int Roll(int die)
{
  return (((rand()>>8) % die) + 1);
}

     /*returns skill roll complete with chances in hell and blunders
       CIH will be 0 if neither, 1 if Chance in hell, -1 if blunder*/
int  SkillRoll(int *CIH)
{
  int roll;
  int total = 0;
  *CIH = 0;
  do
  {
    roll = Roll(10);
    if((*CIH == -1)&&(roll = 1))total -= 10;
    if(*CIH == 0)
    {
      if(roll == 1) *CIH = -1;
      else if(roll == 10) *CIH = 1;
    }
    if(*CIH != -1)total += roll;
  }while(((roll == 1)&&(*CIH == -1))||((roll == 10)&&(*CIH == 1)));
  return total;
}

  /*-1, if less; 0 if even; 1 if greater*/
int  Skillcheck(skill *check,int bonuses,int challenge)
{
  int roll;
  int cih;
  roll = SkillRoll(&cih);
  if(check->score + bonuses + roll >= challenge)
  {
    if(check->score < challenge + 20)/*if this was cake, we didn't learn nothin*/
    {
      if(check->score < challenge)/*it was still reasonable...*/
      {
        if(cih == 1)TrainSkill(check,4);
        else TrainSkill(check,2);
      }
      TrainSkill(check,2);
    }
    return 1;
  }
  if(cih == -1)return 0;/*we blundered this one, and it caused us to fail*/
  TrainSkill(check,1); /*even a failure teaches.*/
  return 0;
}

int  SkillcheckVs(skill *check, int bonuses,skill *otherskill,int otherbonuses)
{
  int roll;
  int cih;
  roll = SkillRoll(&cih);
  if(Skillcheck(otherskill,otherbonuses,check->score + bonuses + roll) == 1)
  {
    if(check->score < otherskill->score + 20)/*if this was cake, we didn't learn nothin*/
    {
      if(check->score < otherskill->score)/*it was still reasonable...*/
      {
        if(cih == 1)TrainSkill(check,4);
        else TrainSkill(check,2);
      }
      TrainSkill(check,2);
    }
    return 1;
  }
  if(cih == -1)return 0;/*we blundered this one, and it caused us to fail*/
  TrainSkill(check,1); /*even a failure teaches.*/
  return 0;
}

int  DetermineRange(skill *target,int bonuses);


           /*Will Update stats and then regain some H,S,M*/
           /*duration is in hour increments*/
void Rest(Hero *hero,int duration,int resttype)
{
  float quality;
  UpdateStats(hero);
  if(resttype == 0)quality = 0.1;
  else quality = 0.3;
  hero->health += (hero->healthmax * quality) * duration;
  hero->mana += (hero->manamax * quality) * duration;
  hero->stamina += (hero->staminamax * quality) * duration * 2;
  if(hero->health > hero->healthmax)hero->health = hero->healthmax;
  if(hero->mana > hero->manamax)hero->mana = hero->manamax;
  if(hero->stamina > hero->staminamax)hero->stamina = hero->staminamax;
}

  /*runs matriculations for all skills and then this will
    update the maximums and everything for the player.*/
void UpdateStats(Hero *hero)
{
  int i;
  for(i =0;i < hero->numtechs;i++)
  {
    Matriculate(hero,&hero->techniques[i]);
  }
  for(i = 0;i < hero->numspells; i++)
  {
    Matriculate(hero,&hero->spells[i]);
  }
  for(i = 0;i < hero->numskills; i++)
  {
    Matriculate(hero,&hero->skills[i]);
  }
  hero->healthmax = (hero->attributes[0].score + hero->attributes[1].score + hero->attributes[3].score)/3;
  hero->staminamax = (hero->attributes[1].score + hero->attributes[3].score)/2;
  hero->manamax = (hero->attributes[2].score + hero->attributes[4].score + hero->attributes[5].score)/3;
  hero->speed = (hero->attributes[0].score + hero->attributes[3].score)/2;
  hero->perception = (hero->attributes[4].score + hero->skills[1].score)/2;
}
     /*adds a skill to the hero's rep*/
int LearnSkill(Hero *hero,int skillindex,int score)
{
  skill *targetskill;
  targetskill = GetHeroSkillByIndex(hero,skillindex,ST_Skill);
  if(targetskill == NULL)
  {
    hero->skills[hero->numskills].score = score;
    hero->skills[hero->numskills].skillindex = skillindex;
    hero->skills[hero->numskills].skilltype = ST_Skill;
    hero->numskills++;
    hero->attributes[A_Intelligence].score++; /*learned somethin!*/
    return 1;
  }
  else
  {
    if(targetskill->score >= score)return -1;  /*learned nothing*/
    TrainSkill(targetskill,score - targetskill->score);
    return 0;
  }
}

int LearnSpell(Hero *hero,int skillindex,int score)
{
  skill *targetspell;
  targetspell = GetHeroSkillByIndex(hero,skillindex,ST_Spell);
  if(targetspell == NULL)
  {
    hero->spells[hero->numskills].score = score;
    hero->spells[hero->numskills].skillindex = skillindex;
    hero->spells[hero->numskills].skilltype = ST_Spell;
    hero->numspells++;
    hero->attributes[A_Intelligence].score++; /*learned somethin!*/
    hero->attributes[A_Magic].score++; /*learned somethin magical!*/
    return 1;
  }
  else
  {
    if(targetspell->score >= score)return -1;  /*learned nothing*/
    TrainSkill(targetspell,score - targetspell->score);
    return 0;
  }
}

int LearnTechnique(Hero *hero,int skillindex,int score)
{
  skill *targetskill;
  targetskill = GetHeroSkillByIndex(hero,skillindex,ST_Skill);
  if(targetskill == NULL)
  {
    hero->skills[hero->numskills].score = score;
    hero->skills[hero->numskills].skillindex = skillindex;
    hero->skills[hero->numskills].skilltype = ST_Skill;
    hero->numskills++;
    hero->attributes[A_Intelligence].score++; /*learned somethin!*/
    return 1;
  }
  else
  {
    if(targetskill->score >= score)return -1;  /*learned nothing*/
    TrainSkill(targetskill,score - targetskill->score);
    return 0;
  }
}

int LearnFormula(Hero *hero,int skillindex,int score)
{
  skill *targetskill;
  targetskill = GetHeroSkillByIndex(hero,skillindex,ST_Skill);
  if(targetskill == NULL)
  {
    hero->skills[hero->numskills].score = score;
    hero->skills[hero->numskills].skillindex = skillindex;
    hero->skills[hero->numskills].skilltype = ST_Skill;
    hero->numskills++;
    hero->attributes[A_Intelligence].score++; /*learned somethin!*/
    return 1;
  }
  else
  {
    if(targetskill->score >= score)return -1;  /*learned nothing*/
    targetskill->score = score;   /*learned a better formula*/
    return 0;
  }
}

enum CLASSES  {C_Archer,C_Brawler,C_Ranger,C_Warrior,
               C_Assassin,C_Strider,C_Swindler,C_Thief,
               C_Conjurer,C_Illusionist,C_Mage,C_Necromancer,
               C_Druid,C_Paladin,C_Priest,C_Zealot,
               C_Alchemist,C_Apothecary,C_Runeologist,C_Smith};

/*makes the hero of the class specified, suboptions
  are for classes with sub options: paladins, mages, etc*/
void BecomeClass(Hero *hero,int classindex,int suboptions)
{
  switch(classindex)
  {
    case C_Warrior:
      hero->attributes[A_Strength].score = 17 + Roll(15);
      hero->attributes[A_Endurance].score = 17 + Roll(15);
      hero->attributes[A_Agility].score = 12 + Roll(15);
      hero->attributes[A_Dexterity].score = 10 + Roll(10);
      hero->attributes[A_Intelligence].score = 4 + Roll(6);
      hero->attributes[A_Magic].score = 5;
      LearnSkill(hero,SK_Attack,12 + Roll(15));
      LearnSkill(hero,SK_Defense,10 + Roll(10));
      LearnSkill(hero,SK_Throwing,12 + Roll(15));
      LearnSkill(hero,SK_Climbing,5);
      hero->bonus = 12 + Roll(10);
      hero->trainingclass = C_Warrior;
    break;
  }
}

enum RACES    {R_Alashanti,R_Aruran,R_Centaur,R_Chakta,R_Dracas,
               R_Dushok,R_Gargoyle,R_Goblin,R_Herpitaur,R_Hobgoblin,
               R_Humon,R_Knidow,R_Krugani,R_Luyet,R_Minotaur,
               R_NightGlider,R_Orcutani,R_TreMaKin,R_Trog,R_Ventash};
               
          /*applied after class is specified, subraces are for
            aruran, luyet, humon, etc*/
void BecomeRace(Hero *hero,int race,int subrace)
{
  skill *modskill;
  switch(race)
  {
    case R_Alashanti:
      hero->attributes[A_Strength].score -= 7;
      hero->attributes[A_Endurance].score += 7;
      hero->attributes[A_Agility].score += 7;
      hero->attributes[A_Dexterity].score -= 7;
      if(hero->attributes[A_Strength].score < 5)hero->attributes[A_Strength].score = 5;
      if(hero->attributes[A_Dexterity].score < 5)hero->attributes[A_Dexterity].score = 5;
      hero->race = R_Alashanti;
      hero->nightperception = 0.8;
      modskill = GetHeroSkillByIndex(hero,SK_Climbing,ST_Skill);
      if(modskill == NULL)
      {
        LearnSkill(hero,SK_Climbing,5);
        modskill = GetHeroSkillByIndex(hero,SK_Climbing,ST_Skill);
      }
      modskill->bonus = 5;
    break;
  }
}

void ApplyClassRaceBonuses();                              /*used when the character rests to make sure bonuses based
                                                             on percentages are maintained*/



/*ye olde end of line at end of file*/
