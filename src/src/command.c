#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"
#include "command.h"
#include "map.h"
#include "entity.h"

extern SDL_Rect Camera;
extern SDL_Surface *screen;
extern SDL_Surface *background;
extern SDL_Surface *videobuffer;
extern Map GameMap;
extern int ThisPlayer;
extern Entity *EntityGroundList[MAP_H][MAP_W];/*Pointers to entities, by location*/
extern Entity *EntityAirList[MAP_H][MAP_W];/*Pointers to entities, by location int he air*/
extern Uint32 rmask,gmask,bmask,amask;
extern Entity DEADSPACE;

Interface DashBoard;
Mouse mouse;
SDL_Surface *minimap = NULL;
char hotkeys[12] = "qwerasdfzxcv";/*defaults for hot keys in the menu*/

void UpdateMiniMap(int i, int j)
{
  int ox,oy;
  ox = (128 - GameMap.map.w)/2;
  oy = (128 - GameMap.map.h)/2;
  DrawSpritePixel(GameMap.textures,DashBoard.Dash->image,i + DashBoard.minimap.x,j + DashBoard.minimap.y,GameMap.map.tile[j][i]);
}                                                                                                 

void DrawMiniUnseen()
{
  int i,j;
  Uint32 fog = SDL_MapRGB(screen->format,0,0,0);
  SDL_SetAlpha(GameMap.FOG->image,SDL_SRCALPHA,128);
  for(j = 0;j < DashBoard.minimap.h;j++)
  {
    for(i = 0;i < DashBoard.minimap.w;i++)
    {
      if(!GameMap.playerviews[ThisPlayer].seen[j][i])
        putpixel(screen,i + DashBoard.minimap.x,j + DashBoard.minimap.y,fog);
      else if(!GameMap.playerviews[ThisPlayer].seeing[j][i])
        DrawSpritePixel(GameMap.FOG,screen,i + DashBoard.minimap.x,j + DashBoard.minimap.y,0);
    }
  }
  SDL_SetAlpha(GameMap.textures->image,0,SDL_ALPHA_OPAQUE);
}

void DrawMiniFog()
{
  int i,j;
  SDL_SetAlpha(GameMap.FOG->image,SDL_SRCALPHA,128);
  for(j = 0;j < DashBoard.minimap.h;j++)
  {
    for(i = 0;i < DashBoard.minimap.w;i++)
    {
      if((GameMap.playerviews[ThisPlayer].seen[j][i])&&(!GameMap.playerviews[ThisPlayer].seeing[j][i]))
      DrawSpritePixel(GameMap.FOG,screen,i + DashBoard.minimap.x,j + DashBoard.minimap.y,0);
    }
  }
  SDL_SetAlpha(GameMap.textures->image,0,SDL_ALPHA_OPAQUE);
}

void DrawMiniMap()
{
  int i,j;
  SDL_Rect box;
  int ox,oy;
  ox = (128 - GameMap.map.w)/2;
  oy = (128 - GameMap.map.h)/2;
  box.x = DashBoard.Dash->image->w - 145;
  box.y = DashBoard.Dash->image->h - 130;
  box.w = 128;
  box.h = 128;
  SDL_FillRect(DashBoard.Dash->image,&box,IndexColor(Black));
  DashBoard.minimap.x = ox + DashBoard.Dash->image->w - 145;
  DashBoard.minimap.y = oy + DashBoard.Dash->image->h - 130;
  DashBoard.minimap.w = GameMap.map.w;
  DashBoard.minimap.h = GameMap.map.h;
  for(j = 0; j <GameMap.map.h;j++)
  {
    for(i = 0; i <GameMap.map.w;i++)
    {
      DrawSpritePixel(GameMap.textures,DashBoard.Dash->image,i + DashBoard.minimap.x,j + DashBoard.minimap.y,GameMap.map.tile[j][i]);
    }
  }
  DashBoard.minimap.y = oy + screen->h - 130;
}

void LoadInterface(int style,Uint16 Color)
{
  int i;
  SDL_Surface *temp = NULL;
  DashBoard.ComColor = Color;
  switch(style)
  {
    case 0:
      DashBoard.Dash = LoadSprite("images/testdash.png",1024,138);
      DashBoard.Buttons = LoadSwappedSprite("images/commands.png",32,32,Color,0,0);
    break;
    case 1:
      DashBoard.Dash = LoadSprite("images/testdashmeehan.png",1024,138);
      DashBoard.Buttons = LoadSwappedSprite("images/commands.png",32,32,Color,0,0);
    break;
    case 2:
      DashBoard.Dash = LoadSprite("images/dotheliadash.png",1024,138);
      DashBoard.Buttons = LoadSwappedSprite("images/commands.png",32,32,Color,0,0);
    break;
    default:
    case 3:
      DashBoard.Dash = LoadSprite("images/magedash.png",1024,138);
      DashBoard.Buttons = LoadSwappedSprite("images/commands.png",32,32,Color,0,0);
    break;
    case 4:
      DashBoard.Dash = LoadSprite("images/elementaldash.png",1024,138);
      DashBoard.Buttons = LoadSwappedSprite("images/commands.png",32,32,Color,0,0);
    break;
    break;
  }
  DashBoard.Resources = LoadSwappedSprite("images/resources.png",16,16,Color,0,0);
  SDL_SetColorKey(DashBoard.Resources->image, SDL_SRCCOLORKEY , IndexColor(Black));
  for(i = 0;i < NUMCOMS;i++)
  {
    DashBoard.Command[i] = i % 7;    
  }
  DashBoard.Selection = 0;
  for(i = 0; i < 18; i++)
  {
    DashBoard.ButtonState[i] = 0; /*1 is pressed*/  
  }
  DashBoard.NumberSelected = 0;
  for(i = 0; i < NUMSELECT; i++)
  {
    DashBoard.SelectedUnit[i] = NULL;
  }
  DrawMiniMap();
  temp = SDL_CreateRGBSurface(SDL_ANYFORMAT, 128,128, 16,rmask, gmask,bmask,amask);
  if(temp == NULL)
  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
  }
  /* Just to make sure that the surface we create is compatible with the screen*/
  minimap = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

}

void DrawInterface()
{
  int i;
  int sx,sy,gx,gy;
  int ox,oy;
  char resource[20];
  DrawSprite(DashBoard.Dash,screen,0,screen->h - 138,0);
  for(i = 0; i < 5;i++)
  {
    if(i == 4)sprintf(resource,"%i / %i",DashBoard.resources[R_Supply],DashBoard.resources[R_SLimit]);
    else sprintf(resource,"%i",DashBoard.resources[i]);
    DrawSprite(DashBoard.Resources,screen,(DashBoard.Dash->image->w - 400) + i * 80,4,i);
    DrawText(resource,screen,(DashBoard.Dash->image->w - 384) + i * 80,4,IndexColor(LightBlue),F_Medium);
  }
  if(!DashBoard.ButtonState[15])
    DrawText("Allies",screen,801,screen->h - DashBoard.Dash->h + 30,IndexColor(DashBoard.ComColor),F_Medium);
  else
    DrawText("Allies",screen,799,screen->h - DashBoard.Dash->h + 32,IndexColor(White),F_Medium);
  if(!DashBoard.ButtonState[16])
    DrawText("Menu",screen,803,screen->h - DashBoard.Dash->h + 62,IndexColor(DashBoard.ComColor),F_Medium);
  else
    DrawText("Menu",screen,801,screen->h - DashBoard.Dash->h + 64,IndexColor(White),F_Medium);
  if(!DashBoard.ButtonState[17])
    DrawText("Options",screen,795,screen->h - DashBoard.Dash->h + 94,IndexColor(DashBoard.ComColor),F_Medium);
  else
    DrawText("Options",screen,793,screen->h - DashBoard.Dash->h + 96,IndexColor(White),F_Medium);
  for(i = 0;i < NUMCOMS;i++)
  {
    if(DashBoard.Command[i] != C_Empty)
    {
      DrawSprite(DashBoard.Buttons,screen,(i % 4)*39 + ((i / 4)*5) + 12 - (2 * DashBoard.ButtonState[i]),(i / 4)*39 + (screen->h - 138) + 16 + (2 * DashBoard.ButtonState[i]),DashBoard.Command[i]);
    }
  }
  {/*handles the minimap... which I plan to streamline a bit*/
    DrawMiniUnseen();
    ox = (128 - GameMap.map.w)/2;
    oy = (128 - GameMap.map.h)/2;
    sx = Camera.x / 32 + ox + screen->w - 145;  /*this section draws the camera box*/
    sy = Camera.y / 32 + oy + screen->h - 130;
    gx = Camera.w/32;
    gy = Camera.h/32;
    DrawRect(sx,sy,gx,gy,IndexColor(White),screen);
  }
  if(DashBoard.NumberSelected == 1)
  {
    DrawSprite(DashBoard.SelectedUnit[0]->sprite,screen,screen->w/2 - 32,screen->h - 78,0);
  }
  else if(DashBoard.NumberSelected > 1)
  {
    sx = DashBoard.NumberSelected % 12;
    sy = DashBoard.NumberSelected / 12;
    for(i = 0;i < DashBoard.NumberSelected;i++)
    {
      DrawSprite(DashBoard.SelectedUnit[i]->sprite,screen,screen->w/2 - (16 * sx) + (i%12)*32 ,screen->h - 78 - (16 * sy) + (i/12)*32,0); 
    }
  }
}

void MapDrawInterface()
{
  int i;
  int sx,sy,gx,gy;
  int ox,oy;
  SDL_Rect box;
  DrawSprite(DashBoard.Dash,screen,0,screen->h - 138,0);
  if(!DashBoard.ButtonState[15])
    DrawText("Allies",screen,801,screen->h - DashBoard.Dash->h + 30,IndexColor(DashBoard.ComColor),F_Medium);
  else
    DrawText("Allies",screen,799,screen->h - DashBoard.Dash->h + 32,IndexColor(White),F_Medium);
  if(!DashBoard.ButtonState[16])
    DrawText("Menu",screen,803,screen->h - DashBoard.Dash->h + 62,IndexColor(DashBoard.ComColor),F_Medium);
  else
    DrawText("Menu",screen,801,screen->h - DashBoard.Dash->h + 64,IndexColor(White),F_Medium);
  if(!DashBoard.ButtonState[17])
    DrawText("Options",screen,795,screen->h - DashBoard.Dash->h + 94,IndexColor(DashBoard.ComColor),F_Medium);
  else
    DrawText("Options",screen,793,screen->h - DashBoard.Dash->h + 96,IndexColor(White),F_Medium);
  for(i = 0;i < NUMCOMS;i++)
  {
    if(DashBoard.Command[i] != C_Empty)
    {
      DrawSprite(DashBoard.Buttons,screen,(i % 4)*39 + ((i / 4)*5) + 12 - (2 * DashBoard.ButtonState[i]),(i / 4)*39 + (screen->h - 138) + 16 + (2 * DashBoard.ButtonState[i]),DashBoard.Command[i]);
    }
  }
  DrawMiniFog();
  ox = (128 - GameMap.map.w)/2;
  oy = (128 - GameMap.map.h)/2;
  sx = Camera.x / 32 + ox + screen->w - 145;
  sy = Camera.y / 32 + oy + screen->h - 130;
  gx = Camera.w/32;
  gy = Camera.h/32;
  box.x = sx;
  box.y = sy;
  box.w = gx;
  box.h = 1;
  SDL_FillRect(screen,&box,IndexColor(White));
  box.y = sy + gy;
  SDL_FillRect(screen,&box,IndexColor(White));
  box.y = sy;
  box.w = 1;
  box.h = gy;
  SDL_FillRect(screen,&box,IndexColor(White));
  box.x = sx + gx;
  SDL_FillRect(screen,&box,IndexColor(White));
  DrawSprite(GameMap.textures,screen,screen->w/2 - 32,screen->h - 78,DashBoard.Selection);
}

void LoadMouse(int c1,int c2,int c3)
{
  mouse.image = LoadSwappedSprite("images/mouse.png",16,16,c1,c2,c3);
  mouse.frame = 0;                                                                                            
  mouse.step = 0;
  mouse.state = 0;  
}

void DrawMouse()
{
  mouse.step = (mouse.step+1)%16;
  mouse.frame = mouse.step + (mouse.state * 16);
  mouse.buttons = SDL_GetMouseState(&mouse.x,&mouse.y);
  DrawSprite(mouse.image,screen,mouse.x,mouse.y,mouse.frame);
}

void MapMouseThink()
{
    Uint16 tilex,tiley = -1;
    mouse.buttons = SDL_GetMouseState(&mouse.x,&mouse.y);
    
    if((mouse.x < 10)&&(Camera.x > 0))
    {
      Camera.x-=20;
      if(Camera.x < 0) Camera.x = 0;
    }
    else if(mouse.x > screen->w - 10)
    {
      Camera.x+=20;
      if(Camera.x > background->w - screen->w) Camera.x = background->w - screen->w;
    }
    if((mouse.y < 10)&&(Camera.y > 0))
    {
      Camera.y-=20;
      if(Camera.y < 0) Camera.y = 0;
    }
    else if(mouse.y > screen->h - 10)
    {
      Camera.y+=20;
      if(Camera.y > background->h - screen->h + 118) Camera.y = background->h - screen->h + 118;
    }
    if(mouse.buttons&SDL_BUTTON(1))
    {
      if(mouse.y > screen->h - 128)/*we're in the dashboard*/
      {
        if((mouse.x > DashBoard.minimap.x)&&(mouse.x <  DashBoard.minimap.x + DashBoard.minimap.w)&&(mouse.y > DashBoard.minimap.y)&&(mouse.y < DashBoard.minimap.y + DashBoard.minimap.h))
        { /*we are clicking in the mini map, now center the map on it.*/
          Camera.x = ((mouse.x - DashBoard.minimap.x)*32) - Camera.w/2;
          if(Camera.x < 0)Camera.x = 0;
          if(Camera.x > background->w - screen->w) Camera.x = background->w - screen->w;
          Camera.y = ((mouse.y - DashBoard.minimap.y)*32) - Camera.h/2;
          if(Camera.y < 0)Camera.y = 0;
          if(Camera.y > background->h - screen->h + 118) Camera.y = background->h - screen->h + 118;
        }
      }
      else
      {
        tilex = (mouse.x + Camera.x)/32;
        tiley = (mouse.y + Camera.y)/32;
        ChangeTile(DashBoard.Selection,tilex,tiley);
        UpdateMiniMap(tilex,tiley);
      }
    }
}

void MouseThink()
{
    int mx,my;
    int tx,ty,tw,th;
    int i,j;
    int moved = 0;
    Entity *Ent = NULL;
    mouse.buttons = SDL_GetMouseState(&mouse.x,&mouse.y);
    if((mouse.x < 10)&&(Camera.x > 0))
    {
      Camera.x-=20;
      if(Camera.x < 0) Camera.x = 0;
    }
    else if(mouse.x > screen->w - 10)
    {
      Camera.x+=20;
      if(Camera.x > background->w - screen->w) Camera.x = background->w - screen->w;
    }
    if((mouse.y < 10)&&(Camera.y > 0))
    {
      Camera.y-=20;
      if(Camera.y < 0) Camera.y = 0;
    }
    else if(mouse.y > screen->h - 10)
    {
      Camera.y+=20;
      if(Camera.y > background->h - screen->h + 118) Camera.y = background->h - screen->h + 118;
    }
    if(!mouse.buttons) /*a release event*/
    {
      if(mouse.pressed) /*select friendly units in the region*/
      {
        mouse.pressed = 0;
        if(mouse.x < mouse.startx)tx = (mouse.x + Camera.x) / 32;
        else tx = (mouse.startx + Camera.x) / 32;
        if(mouse.y < mouse.starty)ty = (mouse.y + Camera.y) / 32;
        else ty = (mouse.starty + Camera.y) / 32;
        tw = fabs(mouse.x - mouse.startx)/32;
        th = fabs(mouse.y - mouse.starty)/32;
        for(j = 0;j <= th;j++)
        {
          for(i = 0;i <= tw;i++)
          {
            if((EntityGroundList[ty + j][tx + i] != NULL)&&(EntityGroundList[ty + j][tx + i] != &DEADSPACE)&&(EntityGroundList[ty + j][tx + i]->Unit_Type == ET_Unit))
            {
              SelectNone(); /*lets clear our buffer.*/
              goto DONE;/*AAHHAHHAHA finally the correct use of the goto...*/
            }
          }
        }
        DONE:
        for(j = 0;j <= th;j++)
        {
          for(i = 0;i <= tw;i++)
          {
            if((EntityGroundList[ty + j][tx + i] != NULL)&&(EntityGroundList[ty + j][tx + i] != &DEADSPACE))
            {
              if((EntityGroundList[ty + j][tx + i]->Player == ThisPlayer)&&(EntityGroundList[ty + j][tx + i]->Unit_Type == ET_Unit))
              { /*only group select my units.*/
                if(!IsSelected(EntityGroundList[ty + j][tx + i]))/*don't add something twice*/
                  AddSelectEntity(EntityGroundList[ty + j][tx + i]);
              }
            }
          }
        }
      }
    }
    if(mouse.buttons&SDL_BUTTON(1))
    {
      if(mouse.y > screen->h - 128)/*we're in the dashboard*/
      {
        if((mouse.x > DashBoard.minimap.x)&&(mouse.x <  DashBoard.minimap.x + DashBoard.minimap.w)&&(mouse.y > DashBoard.minimap.y)&&(mouse.y < DashBoard.minimap.y + DashBoard.minimap.h))
        { /*we are clicking in the mini map, now center the map on it.*/
          Camera.x = ((mouse.x - DashBoard.minimap.x)*32) - Camera.w/2;
          if(Camera.x < 0)Camera.x = 0;
          if(Camera.x > background->w - screen->w) Camera.x = background->w - screen->w;
          Camera.y = ((mouse.y - DashBoard.minimap.y)*32) - Camera.h/2;
          if(Camera.y < 0)Camera.y = 0;
          if(Camera.y > background->h - screen->h + 118) Camera.y = background->h - screen->h + 118;
        }
      }
      else
      {
        /*Here is what happens when I click in the map*/
        mx = (mouse.x + Camera.x)/32;
        my = (mouse.y + Camera.y)/32;
        /*I would check here first to see if I clicked on an air unit...*/
        if((EntityGroundList[my][mx] != NULL)&&(EntityGroundList[my][mx] != &DEADSPACE) && (!mouse.pressed))
        {
          if(EntityGroundList[my][mx]->Player == ThisPlayer)SelectEntity(EntityGroundList[my][mx]);
          else if(GameMap.playerviews[ThisPlayer].seeing[my][mx])
            SelectEntity(EntityGroundList[my][mx]);/*make sure I can see what I select*/
        }
        if(!mouse.pressed)
        {
          mouse.pressed = 1;
          mouse.startx = mouse.x;
          mouse.starty = mouse.y;
        }
      }
    }
    if(mouse.buttons&SDL_BUTTON(3))
    {
      if(mouse.y < screen->h - 128)/*we're not in the dashboard*/
      {
        mx = (mouse.x + Camera.x)/32;
        my = (mouse.y + Camera.y)/32;
        moved = 1;
      }
      else
      {
        if((mouse.x > DashBoard.minimap.x)&&(mouse.x <  DashBoard.minimap.x + DashBoard.minimap.w)&&(mouse.y > DashBoard.minimap.y)&&(mouse.y < DashBoard.minimap.y + DashBoard.minimap.h))
        {
        mx = (mouse.x - DashBoard.minimap.x);
        my = (mouse.y - DashBoard.minimap.y);
        moved = 1;
        }
      }
      if((moved)&&(DashBoard.NumberSelected > 0))
      {
        Ent = GetClosestEnt(mx,my);
        for(i = 0; i < DashBoard.NumberSelected;i++)
        {
          switch(DashBoard.SelectedUnit[i]->Unit_Type)
          {
            default:
            break;
            case ET_Unit:
              if(Ent == DashBoard.SelectedUnit[i])
              {
                NewMessage("order receive",IndexColor(White));/*to be replaced by notification sound*/
                DashBoard.SelectedUnit[i]->Unit->GoalTarget.x = mx;
                DashBoard.SelectedUnit[i]->Unit->GoalTarget.y = my;
//                GetPathStep(DashBoard.SelectedUnit[i], mx, my);
                MoveTo(DashBoard.SelectedUnit[i],DashBoard.SelectedUnit[i]->Unit->WalkTarget.x,DashBoard.SelectedUnit[i]->Unit->WalkTarget.y);
                DashBoard.SelectedUnit[i]->Unit->Orders = OR_WALK;
              }
              else
              {
                DashBoard.SelectedUnit[i]->target = Ent;
                DashBoard.SelectedUnit[i]->Unit->Orders = OR_FOLLOW;
              }
              DashBoard.SelectedUnit[i]->Unit->moving = 1;
              DashBoard.SelectedUnit[i]->Unit->Trail[0].x = -1;
              DashBoard.SelectedUnit[i]->Unit->Trail[0].y = -1;
              DashBoard.SelectedUnit[i]->Unit->failcount = 0;
            break;
          }
        }
      }
    }
}

int MapDashThink()
{
  int numkeys;
  Uint8 *keystate;
  keystate = SDL_GetKeyState(&numkeys);
  if(keystate[SDLK_LEFT])
  {
    DashBoard.Selection--;
    if(DashBoard.Selection < 0)DashBoard.Selection = 28;
  }
  if(keystate[SDLK_RIGHT])
  {
    DashBoard.Selection++;
    if(DashBoard.Selection > 28)DashBoard.Selection = 0;
  }
  if(keystate[SDLK_UP])
  {
    NewMessage("New Test",IndexColor(SDL_GetTicks()%30));
  }
  if(keystate[SDLK_F12])
  {
    SaveMap("maps/testmap2.tcm");
  }
  if(keystate[SDLK_F10])
  {
    LoadMap("maps/testmap2.tcm");
    DrawMap();
    DrawMiniMap();
    Camera.x = 0;
    Camera.y = 0;
  }
  if(keystate[SDLK_ESCAPE])return 1;
  return 0;
}

int DashThink()
{
  int numkeys,i;
  Uint8 *keystate;
  keystate = SDL_GetKeyState(&numkeys);
  for(i = 0;i < NUMCOMS;i++)
  {
    if(DashBoard.ButtonState[i] == 1)
    {
      DashBoard.ButtonState[i] = 0;
    }
  }
  for(i = 0;i < NUMCOMS;i++)
  {
    if(keystate[hotkeys[i] - 'a' + SDLK_a])
    {
      if(DashBoard.Command[i] != -1)
      {
        DashBoard.ButtonState[i] = 1;
      }
    }
  }
  if(keystate[SDLK_PRINT])
  {
    SDL_SaveBMP(videobuffer, "screenshots/tcoa00.bmp");
  }
  if(keystate[SDLK_ESCAPE])return 1;
  /*lets check to see if a button has been pressed*/
  return 0;
}

/*removes the Selected Entity from the list*/
void UnselectEntity(Entity *ent)
{
  int i,j;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  {
    if(ent == DashBoard.SelectedUnit[i])
    {
      for(j = i;j < DashBoard.NumberSelected - 1;j++)
      {
        DashBoard.SelectedUnit[j] = DashBoard.SelectedUnit[j + 1];
      }
      DashBoard.NumberSelected--;
      return;
    }
  }
}

/*finds the closest entity to the goal so I can make it the leader.*/

Entity *GetClosestEnt(int gx,int gy)
{
  int closest = 20000;
  Entity *Ent = NULL;
  int i,temp;
  for(i = 0; i < DashBoard.NumberSelected;i++)
  {
    temp = fabs(DashBoard.SelectedUnit[i]->m.x - gx) + fabs(DashBoard.SelectedUnit[i]->m.y - gy);
    if(temp < closest)
    {
      closest = temp;
      Ent = DashBoard.SelectedUnit[i];
    }
  }
  return Ent;
}

void AddSelectEntity(Entity *ent)
{
  int i;
  if(DashBoard.NumberSelected < NUMSELECT)
  {
    DashBoard.SelectedUnit[DashBoard.NumberSelected++] = ent;
    for(i = 0; i < NUMCOMS; i++)
    {
      DashBoard.Command[i] = ent->Commands[i];
    }
  }
}

int IsSelected(Entity *ent)
{
  int i;
  for(i = 0; i < DashBoard.NumberSelected; i++)
  {
    if(DashBoard.SelectedUnit[i] == ent)return 1;
  }
  return 0;
}

void SelectNone()
{
  int i;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  DashBoard.SelectedUnit[i] = NULL;
  DashBoard.NumberSelected = 0;
}

void SelectEntity(Entity *ent)
{
  int i;
  DashBoard.NumberSelected = 1;
  DashBoard.SelectedUnit[0] = ent;
  for(i = 0; i < NUMCOMS; i++)
  {
    DashBoard.Command[i] = ent->Commands[i];
  }
}

void DrawSelectionHealth()
{
  int i;
  int tx,ty,tw,th;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  {
    if(DashBoard.SelectedUnit[i]->Player == ThisPlayer)
    {
      DrawFilledRect(DashBoard.SelectedUnit[i]->s.x - Camera.x,DashBoard.SelectedUnit[i]->s.y - Camera.y,DashBoard.SelectedUnit[i]->sprite->w, 2,IndexColor(Red),screen);
      DrawFilledRect(DashBoard.SelectedUnit[i]->s.x - Camera.x,DashBoard.SelectedUnit[i]->s.y - Camera.y,(DashBoard.SelectedUnit[i]->health * DashBoard.SelectedUnit[i]->sprite->w)/DashBoard.SelectedUnit[i]->healthmax, 2,IndexColor(Green),screen);
    }
  }
  if(mouse.pressed)
  {
    if(mouse.x < mouse.startx)tx = mouse.x;
     else tx = mouse.startx;
    if(mouse.y < mouse.starty)ty = mouse.y;
     else ty = mouse.starty;
    tw = fabs(mouse.x - mouse.startx);
    th = fabs(mouse.y - mouse.starty);
    DrawRect(tx,ty,tw,th, IndexColor(LightBlue), screen);
  }
}

void DrawSelectionRings()
{
  int i;
  for(i = 0;i < DashBoard.NumberSelected;i++)
  {
    DrawElipse(DashBoard.SelectedUnit[i]->s.x + DashBoard.SelectedUnit[i]->sprite->w/2 - Camera.x,DashBoard.SelectedUnit[i]->s.y + DashBoard.SelectedUnit[i]->sprite->h  + DashBoard.SelectedUnit[i]->selectoffsety - Camera.y,DashBoard.SelectedUnit[i]->sprite->w / 2 , IndexColor(LightBlue), screen);
  }
}

void ChangeMouse(int state)
{
  mouse.state = state;/*this will get more advanced in the future*/
}



