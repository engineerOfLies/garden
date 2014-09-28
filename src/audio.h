#ifndef __Audio__
#define __Audio__

#include "SDL/SDL_mixer.h"

typedef struct SOUND_T
{
  Mix_Chunk *sound;
  char filename[20];
  int used;
  int volume;
}Sound;


void Init_Audio();
void InitSoundList();
void ClearSoundList();
void FreeSound(Sound *sound);
Sound *LoadSound(char filename[20],int volume);


#endif
