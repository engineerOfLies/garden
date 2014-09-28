#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"

#define MaxSprites    511

SDL_Surface *screen; /*pointer to the draw buffer*/
SDL_Surface *background;/*pointer to the background image buffer*/
SDL_Surface *videobuffer;
SDL_Surface *unseenmask;  /*this is a display mask for the fog*/
SDL_Surface *fogofwar; /*this is where the fog of war is drawn*/
SDL_Rect Camera; /*x & y are the coordinates for the background map, w and h are of the screen*/
Sprite SpriteList[MaxSprites];
Uint32 NOW;  /*this represents the current time for the game loop.  Things move according to time*/

int NumSprites;

/*some data on the video settings that can be useful for a lot of functions*/
Uint32 rmask,gmask,bmask,amask;
ScreenData  S_Data;


void Init_Graphics()
{
    Uint32 Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT;
    Uint32 HWflag = 0;
    SDL_Surface *temp;
    S_Data.xres = 1024;
    S_Data.yres = 768;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    #endif
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_DOUBLEBUF) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    if(SDL_VideoModeOK(1024, 768, 32, SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE | SDL_SRCALPHA))
    {
        S_Data.xres = 1024;
        S_Data.yres = 768;
        S_Data.depth = 32;
		    fprintf(stderr,"32 bits of depth\n");
        Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE | SDL_SRCALPHA;
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(1024, 768, 16, SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE | SDL_SRCALPHA))
    {
        S_Data.xres = 1024;
        S_Data.yres = 768;
        S_Data.depth = 16;
        Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE | SDL_SRCALPHA;
		fprintf(stderr,"16 bits of depth\n");
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(1024, 768, 16, SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_SRCALPHA))
    {
        S_Data.xres = 1024;
        S_Data.yres = 768;
        S_Data.depth = 16;
		fprintf(stderr,"16 bits of depth\n");
        Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_SRCALPHA;
        HWflag = SDL_SWSURFACE;
    }
    else                                                         
    {
        fprintf(stderr, "Unable to Use your crap: %s\n Upgrade \n", SDL_GetError());
        exit(1);
    }
    videobuffer = SDL_SetVideoMode(S_Data.xres, S_Data.yres,S_Data.depth, Vflags);
    if ( videobuffer == NULL )
    {
        fprintf(stderr, "Unable to set 1024x768 video: %s\n", SDL_GetError());
        exit(1);
    }
    temp = SDL_CreateRGBSurface(SDL_HWSURFACE, S_Data.xres, S_Data.yres, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    screen = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    temp = SDL_CreateRGBSurface(SDL_HWSURFACE, 2048, 2048, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    background = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    temp = SDL_CreateRGBSurface(SDL_SRCCOLORKEY | SDL_HWSURFACE, 2048, 2048, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize unseen mask buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    unseenmask = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    temp = SDL_CreateRGBSurface(SDL_SRCALPHA | SDL_HWSURFACE, 2048, 2048, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize fog of war buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    fogofwar = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_ShowCursor(SDL_DISABLE);
    Camera.x = 0;
    Camera.y = 0;
    Camera.w = screen->w;
    Camera.h = screen->h;
    srand(SDL_GetTicks());
}


void ResetBuffer()
{
    SDL_BlitSurface(fogofwar,&Camera,screen,NULL);
}

void NextFrame()
{
  Uint32 Then;
  SDL_BlitSurface(screen,NULL,videobuffer,NULL);
  SDL_Flip(videobuffer);
  FrameDelay(33);
  Then = NOW;
  NOW = SDL_GetTicks();
  fprintf(stdout,"Ticks passed this frame: %i\n", NOW - Then);
}

/*
  InitSpriteList is called when the program is first started.
  It just sets everything to zero and sets all pointers to NULL.
  It should never be called again.
*/

void InitSpriteList()
{
  int x;
  NumSprites = 0;
  memset(SpriteList,0,sizeof(Sprite) * MaxSprites);
  for(x = 0;x < MaxSprites;x++)SpriteList[x].image = NULL;
}

/*Create a sprite from a file, the most common use for it.*/

Sprite *LoadSprite(char *filename,int sizex, int sizey)
{
  int i;
  SDL_Surface *temp;
  /*first search to see if the requested sprite image is alreday loaded*/
  for(i = 0; i < NumSprites; i++)
  {
    if(strncmp(filename,SpriteList[i].filename,20)==0)
    {
      SpriteList[i].used++;
      return &SpriteList[i];
    }
  }
  /*makesure we have the room for a new sprite*/
  if(NumSprites + 1 >= MaxSprites)
  {
        fprintf(stderr, "Maximum Sprites Reached.\n");
        exit(1);
  }
  /*if its not already in memory, then load it.*/
  NumSprites++;
  for(i = 0;i <= NumSprites;i++)
  {
    if(!SpriteList[i].used)break;
  }
  temp = IMG_Load(filename);
  if(temp == NULL)
  {
    fprintf(stderr,"unable to load a vital sprite: %s\n",SDL_GetError());
    exit(0);
  }
  SpriteList[i].image = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  /*sets a transparent color for blitting.*/
  SDL_SetColorKey(SpriteList[i].image, SDL_SRCCOLORKEY , SDL_MapRGB(SpriteList[i].image->format, 255,255,255));
   /*then copy the given information to the sprite*/
  strncpy(SpriteList[i].filename,filename,20);
      /*now sprites don't have to be 16 frames per line, but most will be.*/
  SpriteList[i].framesperline = 16;
  SpriteList[i].w = sizex;
  SpriteList[i].h = sizey;
  SpriteList[i].used++;
  return &SpriteList[i];
}

Sprite *LoadSwappedSprite(char *filename,int sizex, int sizey, int c1, int c2, int c3)
{
  int i;
  SDL_Surface *temp;
  /*first search to see if the requested sprite image is alreday loaded*/
  for(i = 0; i < NumSprites; i++)
  {
    if((strncmp(filename,SpriteList[i].filename,20)==0)&&(SpriteList[i].used >= 1)&&(c1 == SpriteList[i].color1)&&(c2 == SpriteList[i].color2)&&(c3 == SpriteList[i].color3))
    {
      SpriteList[i].used++;
      return &SpriteList[i];
    }
  }
  /*makesure we have the room for a new sprite*/
  if(NumSprites + 1 >= MaxSprites)
  {
        fprintf(stderr, "Maximum Sprites Reached.\n");
        exit(1);
  }
  /*if its not already in memory, then load it.*/
  NumSprites++;
  for(i = 0;i <= NumSprites;i++)
  {
    if(!SpriteList[i].used)break;
  }
  temp = IMG_Load(filename);
  if(temp == NULL)
  {
        fprintf(stderr, "FAILED TO LOAD A VITAL SPRITE.\n");
        exit(1);
  }
  SpriteList[i].image = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  /*sets a transparent color for blitting.*/
  SDL_SetColorKey(SpriteList[i].image, SDL_SRCCOLORKEY , SDL_MapRGB(SpriteList[i].image->format, 255,255,255));
  //fprintf(stderr,"asked for colors: %d,%d,%d \n",c1,c2,c3);
  SwapSprite(SpriteList[i].image,c1,c2,c3);
   /*then copy the given information to the sprite*/
  strcpy(SpriteList[i].filename,filename);
      /*now sprites don't have to be 16 frames per line, but most will be.*/
  SpriteList[i].framesperline = 16;
  SpriteList[i].w = sizex;
  SpriteList[i].h = sizey;
  SpriteList[i].color1 = c1;
  SpriteList[i].color2 = c2;
  SpriteList[i].color3 = c3;
  SpriteList[i].used++;
  return &SpriteList[i];
}

/*
 * When we are done with a sprite, lets give the resources back to the system...
 * so we can get them again later.
 */

void FreeSprite(Sprite *sprite)
{
  /*first lets check to see if the sprite is still being used.*/
  sprite->used--;
  if(sprite->used == 0)
  {
  strcpy(sprite->filename,"\0");
     /*just to be anal retentive, check to see if the image is already freed*/
  if(sprite->image != NULL)SDL_FreeSurface(sprite->image);
  sprite->image = NULL;
  }
 /*and then lets make sure we don't leave any potential seg faults 
  lying around*/
}

void CloseSprites()
{
  int i;
   for(i = 0;i < MaxSprites;i++)
   {
     /*it shouldn't matter if the sprite is already freed, 
     FreeSprite checks for that*/
      FreeSprite(&SpriteList[i]);
   }
}

void DrawGreySprite(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame)
{
  int i,j;
  int offx,offy;
  Uint8 r,g,b;
  Uint32 pixel;
  Uint32 Key = sprite->image->format->colorkey;
  offx = frame%sprite->framesperline * sprite->w;
  offy = frame/sprite->framesperline * sprite->h;
  if ( SDL_LockSurface(sprite->image) < 0 )
  {
      fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
      exit(1);
  }
  for(j = 0;j < sprite->h;j++)
  {
    for(i = 0;i < sprite->w;i++)
    {
      pixel = getpixel(sprite->image, i + offx ,j + offy);
      if(Key != pixel)
      {
        SDL_GetRGB(pixel, sprite->image->format, &r, &g, &b);
        r = (r + g + b)/3;
        putpixel(surface, sx + i, sy + j, SDL_MapRGB(sprite->image->format, r, r, r));
      }
    }
  }
  SDL_UnlockSurface(sprite->image);
}

void DrawSprite(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame)
{
    SDL_Rect src,dest;
    src.x = frame%sprite->framesperline * sprite->w;
    src.y = frame/sprite->framesperline * sprite->h;
    src.w = sprite->w;
    src.h = sprite->h;
    dest.x = sx;
    dest.y = sy;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

void DrawSpritePixel(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame)
{
    SDL_Rect src,dest;
    src.x = frame%sprite->framesperline * sprite->w + sprite->w/2;
    src.y = frame/sprite->framesperline * sprite->h + sprite->h/2;
    src.w = 1;
    src.h = 1;
    dest.x = sx;
    dest.y = sy;
    dest.w = 1;
    dest.h = 1;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

Uint32 SetColor(Uint32 color, int newcolor1,int newcolor2, int newcolor3)
{
    Uint8 r,g,b;
    Uint8 intensity;
	int newcolor;
    SDL_GetRGB(color, screen->format, &r, &g, &b);
    if((r == 0) && (g == 0)&&(b !=0))
    {
        intensity = b;
        newcolor = newcolor3;
    }
    else if((r ==0)&&(b == 0)&&(g != 0))
    {
        intensity = g;
        newcolor = newcolor2;
    }
    else if((g == 0)&&(b == 0)&&(r != 0))
    {
        intensity = r;
        newcolor = newcolor1;
    }
    else return color;
    switch(newcolor)
    {
        case Red:
            r = intensity;
            g = 0;
            b = 0;
            break;
        case Green:
            r = 0;
            g = intensity;
            b = 0;
            break;
        case Blue:
            r = 0;
            g = 0;
            b = intensity;
            break;
        case Yellow:
            r = (Uint8)(intensity * 0.7);
            g = (Uint8)(intensity * 0.7);
            b = 0;
            break;
        case Orange:
            r = (Uint8)(intensity * 0.9);
            g = (Uint8)(intensity * 0.4);
            b = (Uint8)(intensity * 0.1);
            break;
        case Violet:
            r = (Uint8)(intensity * 0.7);
            g = 0;
            b = (Uint8)(intensity * 0.7);
            break;
        case Brown:
            r = (Uint8)(intensity * 0.6);
            g = (Uint8)(intensity * 0.3);
            b = (Uint8)(intensity * 0.15);
            break;
        case Grey:
            r = (Uint8)(intensity * 0.5);
            g = (Uint8)(intensity * 0.5);
            b = (Uint8)(intensity * 0.5);
            break;
        case DarkRed:
            r = (Uint8)(intensity * 0.5);
            g = 0;
            b = 0;
            break;
        case DarkGreen:
            r = 0;
            g = (Uint8)(intensity * 0.5);
            b = 0;
            break;
        case DarkBlue:
            r = 0;
            g = 0;
            b = (Uint8)(intensity * 0.5);
            break;
        case DarkYellow:
            r = (Uint8)(intensity * 0.4);
            g = (Uint8)(intensity * 0.4);
            b = 0;
            break;
        case DarkOrange:
            r = (Uint8)(intensity * 0.6);
            g = (Uint8)(intensity * 0.2);
            b = (Uint8)(intensity * 0.1);
            break;
        case DarkViolet:
            r = (Uint8)(intensity * 0.4);
            g = 0;
            b = (Uint8)(intensity * 0.4);
            break;
        case DarkBrown:
            r = (Uint8)(intensity * 0.2);
            g = (Uint8)(intensity * 0.1);
            b = (Uint8)(intensity * 0.05);
            break;
        case DarkGrey:
            r = (Uint8)(intensity * 0.3);
            g = (Uint8)(intensity * 0.3);
            b = (Uint8)(intensity * 0.3);
            break;
        case LightRed:
            r = intensity;
            g = (Uint8)(intensity * 0.45);
            b = (Uint8)(intensity * 0.45);
            break;
        case LightGreen:
            r = (Uint8)(intensity * 0.45);
            g = intensity;
            b = (Uint8)(intensity * 0.45);
            break;
        case LightBlue:
            r = (Uint8)(intensity * 0.45);
            b = intensity;
            g = (Uint8)(intensity * 0.45);
            break;
        case LightYellow:
            r = intensity;
            g = intensity;
            b = (Uint8)(intensity * 0.45);
            break;
        case LightOrange:
            r = intensity;
            g = (Uint8)(intensity * 0.75);
            b = (Uint8)(intensity * 0.35);
            break;
        case LightViolet:
            r = intensity;
            g = (Uint8)(intensity * 0.45);
            b = intensity;
            break;
        case LightBrown:
            r = intensity;
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.45);
            break;
        case LightGrey:
            r = (Uint8)(intensity * 0.85);
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.85);
            break;
        case Black:
            r = (Uint8)(intensity * 0.15);
            g = (Uint8)(intensity * 0.15);
            b = (Uint8)(intensity * 0.15);
            break;
        case White:
            r = intensity;
            g = intensity;
            b = intensity;
            break;
        case Tan:
            r = intensity;
            g = (Uint8)(intensity * 0.9);
            b = (Uint8)(intensity * 0.6);
            break;
        case Gold:
            r = (Uint8)(intensity * 0.8);
            g = (Uint8)(intensity * 0.7);
            b = (Uint8)(intensity * 0.2);
            break;
        case Silver:
            r = (Uint8)(intensity * 0.95);
            g = (Uint8)(intensity * 0.95);
            b = intensity;
            break;
        case YellowGreen:
            r = (Uint8)(intensity * 0.45);
            g = (Uint8)(intensity * 0.75);
            b = (Uint8)(intensity * 0.2);
            break;
        case Cyan:
            r = 0;
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.85);
            break;
        case Magenta:
            r = (Uint8)(intensity * 0.7);
            g = 0;
            b = (Uint8)(intensity * 0.7);
            break;
		default:
            r = 0;
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.85);

			break;
    }
	color = SDL_MapRGB(screen->format,r,g,b);
//    fprintf(stderr,"newcolor: %d, asked for: %d,%d,%d \n",color,newcolor1,newcolor2,newcolor3); 
    return color;
}


/*
 * and now bringing it all together, we swap the pure colors in the sprite out
 * and put the new colors in.  This maintains any of the artist's shading and
 * detail, but still lets us have that old school palette swapping.
 */
void SwapSprite(SDL_Surface *sprite,int color1,int color2,int color3)
{
    int x, y;
	SDL_Surface *temp;
    Uint32 pixel,pixel2;
    Uint32 Key = sprite->format->colorkey;
	temp = SDL_DisplayFormat(sprite);
   /*First the precautions, that are tedious, but necessary*/
    if(color1 == -1)return;
    if(sprite == NULL)return;
    if ( SDL_LockSurface(temp) < 0 )
    {
        fprintf(stderr, "Can't lock surface: %s\n", SDL_GetError());
        exit(1);
    }
   /*now step through our sprite, pixel by pixel*/
    for(y = 0;y < sprite->h ;y++)
    {
        for(x = 0;x < sprite->w ;x++)
        {
            pixel = getpixel(temp,x,y);/*and swap it*/
			if(pixel != Key)
			{
			  pixel2 = SetColor(pixel,color1,color2,color3);
              putpixel(sprite,x,y,pixel2);
			}
        }
    }
    SDL_UnlockSurface(temp);
	SDL_FreeSurface(temp);
}

Uint32 IndexColor(int color)
{
    switch(color)
    {
    case Red:
        return SDL_MapRGB(screen->format,138,0,0);;
    case Green:
        return SDL_MapRGB(screen->format,0,138,0);;
    case Blue:
        return SDL_MapRGB(screen->format,0,0,138);;
    case Yellow:
        return SDL_MapRGB(screen->format,128,128,0);;
    case Orange:
        return SDL_MapRGB(screen->format,148,118,0);;
    case Violet:
        return SDL_MapRGB(screen->format,128,0,128);
    case Brown:
        return SDL_MapRGB(screen->format,100,64,4);
    case Grey:
        return SDL_MapRGB(screen->format,128,128,128);
    case DarkRed:
        return SDL_MapRGB(screen->format,64,0,0);
    case DarkGreen:
        return SDL_MapRGB(screen->format,0,64,0);
    case DarkBlue:
        return SDL_MapRGB(screen->format,0,0,64);
    case DarkYellow:
        return SDL_MapRGB(screen->format,60,60,0);
    case DarkOrange:
        return SDL_MapRGB(screen->format,64,56,0);
    case DarkViolet:
        return SDL_MapRGB(screen->format,60,0,60);
    case DarkBrown:
        return SDL_MapRGB(screen->format,56,32,2);
    case DarkGrey:
        return SDL_MapRGB(screen->format,64,64,64);
    case LightRed:
        return SDL_MapRGB(screen->format,255,32,32);
    case LightGreen:
        return SDL_MapRGB(screen->format,32,255,32);
    case LightBlue:
        return SDL_MapRGB(screen->format,32,32,255);
    case LightYellow:
        return SDL_MapRGB(screen->format,250,250,60);
    case LightOrange:
        return SDL_MapRGB(screen->format,255,234,30);
    case LightViolet:
        return SDL_MapRGB(screen->format,250,30,250);
    case LightBrown:
        return SDL_MapRGB(screen->format,200,100,32);
    case LightGrey:
        return SDL_MapRGB(screen->format,196,196,196);
    case Black:
        return SDL_MapRGB(screen->format,0,0,0);
    case White:
        return SDL_MapRGB(screen->format,255,255,255);
    case Tan:
        return SDL_MapRGB(screen->format,255,128,64);
    case Gold:
        return SDL_MapRGB(screen->format,255,245,30);
    case Silver:
        return SDL_MapRGB(screen->format,206,206,226);
    case YellowGreen:
        return SDL_MapRGB(screen->format,196,255,30);
    case Cyan:
        return SDL_MapRGB(screen->format,0,255,255);;
    case Magenta:
        return SDL_MapRGB(screen->format,255,0,255);
    }
    return SDL_MapRGB(screen->format,0,0,0);
}

/*
  Copied from SDL's website.  I use it for palette swapping
  Its not plagerism if you document it!
*/
void DrawLine(SDL_Surface *screen,Uint32 color,int sx,int sy,int gx,int gy)
{
  Uint32 dx = fabs(gx - sx);
  Uint32 dy = fabs(gy - sy);
  int i;
  int length = sqrt(dx*dx + dy*dy);
  float slopey = (float)dy/dx;
  float slopex = (float)dx/dy;
  for(i = 0;i < length;i++)
  {
    putpixel(screen, sx + (int)(i * slopex),sy + (int)(i * slopey),color);
  }
}

void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);

    if ( SDL_LockSurface(screen) < 0 )
    {
      return;
    }
    switch (screen->format->BytesPerPixel)
    {
        case 1:
        { /* Assuming 8-bpp */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *bufp = color;
        }
        break;

        case 2:
        { /* Probably 15-bpp or 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3:
        { /* Slow 24-bpp mode, usually not used */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *(bufp+screen->format->Rshift/8) = R;
            *(bufp+screen->format->Gshift/8) = G;
            *(bufp+screen->format->Bshift/8) = B;
        }
        break;

        case 4:
        { /* Probably 32-bpp */
            Uint32 *bufp;

            bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
            *bufp = color;
        }
        break;
    }
    SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, x, y, 1, 1);
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    /* Here p is the address to the pixel we want to retrieve*/
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;

    switch(surface->format->BytesPerPixel)
    {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /*shouldn't happen, but avoids warnings*/
    }
}



/*
 * the putpixel function ont he SDL website doesn't always wrk right.  Here is a REAL simple alternative.
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  SDL_Rect point;
  point.x = x;
  point.y = y;
  point.w = 1;
  point.h = 1;
  SDL_FillRect(surface,&point,pixel);
}


/*
  copied and pasted and then significantly modified from the sdl website.  
  I kept ShowBMP to test my program as I wrote it, and I rewrote it to use any file type supported by SDL_image
*/

void ShowBMP(SDL_Surface *image, SDL_Surface *screen, int x, int y)
{
    SDL_Rect dest;

    /* Blit onto the screen surface.
       The surfaces should not be locked at this point.
     */
    dest.x = x;
    dest.y = y;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, screen, &dest);

    /* Update the changed portion of the screen */
    SDL_UpdateRects(screen, 1, &dest);
}


/*
  makes sure a minimum number of ticks is waited between frames
  this is to ensure that on faster machines the game won't move so fast that
  it will look terrible.
  This is a very handy function in game programming.
*/

void FrameDelay(Uint32 delay)
{
    static Uint32 pass = 100;
    Uint32 dif;
    dif = SDL_GetTicks() - pass;
    if(dif < delay)SDL_Delay( delay - dif);
    pass = SDL_GetTicks();
}
/*draws an elipse at the location specified*/
void DrawElipse(int ox,int oy, int radius, Uint32 Color, SDL_Surface *surface)
{
  int r2 = radius * radius;
  int x,y;
  for(x = radius * -1;x <= radius;x++)
  {
    y = (int) (sqrt(r2 - x*x) * 0.6);
    putpixel(surface, x + ox, oy + y, Color);
    putpixel(surface, x + ox, oy - y, Color);
  }
}

/*draws an rectangle outline at the coordinates of the width and height*/
void DrawRect(int sx,int sy, int sw, int sh, Uint32 Color, SDL_Surface *surface)
{
  SDL_Rect box;
    box.x = sx;
    box.y = sy;
    box.w = sw;
    box.h = 1;                                        
    SDL_FillRect(surface,&box,Color);
    box.y = sy + sh;
    SDL_FillRect(surface,&box,Color);
    box.y = sy;
    box.w = 1;
    box.h = sh;
    SDL_FillRect(surface,&box,Color);
    box.x = sx + sw;
    SDL_FillRect(surface,&box,Color);
}

/*draws a filled rect at the coordinates, in the color, on the surface specified*/
void DrawFilledRect(int sx,int sy, int sw, int sh, Uint32 Color, SDL_Surface *surface)
{
  SDL_Rect dst;
  dst.x = sx;
  dst.y = sy;
  dst.w = sw;
  dst.h = sh;
  SDL_FillRect(surface,&dst,Color);
}

/*sets an sdl surface to all color.*/

void BlankScreen(SDL_Surface *buf,Uint32 color)
{
    SDL_LockSurface(buf);
    memset(buf->pixels, (Uint8)color,buf->format->BytesPerPixel * buf->w *buf->h);
    SDL_UnlockSurface(buf);
}
