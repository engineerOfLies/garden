#include <stdlib.h>
#include <string.h>
#include "SDL_ttf.h"
#include "graphics.h"

extern SDL_Surface *screen;
TTF_Font *Font = NULL;
TTF_Font *SFont = NULL;
TTF_Font *LFont = NULL;

#define MAXMES    16

typedef struct MESSAGE_T
{
  char message[MAXMES][30];
  Uint32 fadeout[MAXMES];     /*this is when the test will fade out.*/
  Uint32 color[MAXMES]; 
  int last;
}Message;

Message messages;

void LoadFonts()
{
  LoadFont("fonts/font2.ttf",14,F_Small);
  LoadFont("fonts/font.ttf",14,F_Medium);
  LoadFont("fonts/font2.ttf",18,F_Large);
}

void LoadFont(char filename[40],int ptsize,int type)
{
  if(TTF_Init()==0)
  {
    atexit(TTF_Quit);
  }
  else
  {
    fprintf(stderr,"Couldn't initialize Font System: %s\n", SDL_GetError());
    exit(1);
  }
  switch(type)
  {
    case F_Small:
      SFont = TTF_OpenFont(filename,ptsize);
      if(SFont == NULL)
      {
        fprintf(stderr,"Couldn't initialize Font: %s\n", SDL_GetError());
        exit(1);
      }
    break;
    case F_Medium:
      Font = TTF_OpenFont(filename,ptsize);
      if(Font == NULL)
      {
        fprintf(stderr,"Couldn't initialize Font: %s\n", SDL_GetError());
        exit(1);
      }
    case F_Large:
      LFont = TTF_OpenFont(filename,ptsize);
      if(LFont == NULL)
      {
        fprintf(stderr,"Couldn't initialize Font: %s\n", SDL_GetError());
        exit(1);
      }
    break;
  }
}

void InitMessages()
{
  int i;
  for(i = 0; i < MAXMES; i++)
  {
    strncpy(messages.message[i]," \0",30);
    messages.fadeout[i] = 0;
  }
}

void NewMessage(char *text,Uint32 color)
{
  int i,x = -1;
  Uint32 Now = SDL_GetTicks();
  Uint32 best = Now * 2;
  for(i = 0; i < MAXMES;i ++)
  {
    if(messages.fadeout[i] < Now)
    {
      x = i;
      break;
    }
  }
  if(x == -1) /*everything is in use, so lets hurry along the oldest one, shall we.*/
  {
    for(i = 0; i < MAXMES;i ++)
    {
      if(messages.fadeout[i] < best)
      {
        best = messages.fadeout[i];
        x = i;
      }
    }
  }
  strncpy(messages.message[x],text,30);
  messages.fadeout[x] = Now + 3500;
  messages.color[x] = color;
  messages.last = x;
}

void DrawMessages()
{
  int i,j;
  int t; /*target message*/
  Uint32  Now = SDL_GetTicks();
  j = MAXMES;
  for(i = MAXMES;i > 0;--i)
  {
    t = (messages.last + i)%MAXMES;
    if(messages.fadeout[t] > Now)
      DrawText(messages.message[t],screen,10,screen->h - 155 - (14 * j--),messages.color[t],F_Small);
  }
}

void DrawText(char *text,SDL_Surface *surface,int sx,int sy,Uint32 color,int size)
{
  SDL_Surface *temp1;
  SDL_Surface *fontpic;
  SDL_Color colortype,bgcolor;
  SDL_Rect dst;
  SDL_GetRGBA(color, screen->format, &colortype.r, &colortype.g, &colortype.b, &colortype.unused);
  bgcolor.r = 0;
  bgcolor.g = 0;
  bgcolor.b = 0;
  bgcolor.unused = SDL_ALPHA_TRANSPARENT;
  switch(size)
  {
    case F_Small:
      if(SFont == NULL)return;
      temp1 = TTF_RenderText_Shaded(SFont, text,colortype,bgcolor);
    break;
    case F_Medium:
      if(Font == NULL)return;
      temp1 = TTF_RenderText_Shaded(Font, text,colortype,bgcolor);
    break;
    case F_Large:
      if(LFont == NULL)return;
      temp1 = TTF_RenderText_Shaded(LFont, text,colortype,bgcolor);
    break;
  }
  fontpic = SDL_DisplayFormat(temp1);
  SDL_FreeSurface(temp1);
  dst.x = sx;
  dst.y = sy;
  SDL_SetColorKey(fontpic, SDL_SRCCOLORKEY , SDL_MapRGBA(screen->format, bgcolor.r,bgcolor.g,bgcolor.b,bgcolor.unused));
  SDL_BlitSurface(fontpic,NULL,surface,&dst);
  SDL_FreeSurface(fontpic);
}


