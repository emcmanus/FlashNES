/*
 *  flash_main.c
 *  FlashNES
 *
 *  Created by Ed McManus on 11/13/09.
 *  Copyright 2009 Ed McManus.. All rights reserved.
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "AS3.h"
#include "flash_main.h"

#include "throttle.h"

#include "../common/cheat.h"

#include "input.h"
#include "dface.h"


int CloseGame(void);

static char *soundrecfn=0;	/* File name of sound recording. */

static int ntsccol=0,ntschue=0,ntsctint=0;
int soundvol=100;
long soundq=0;
int _sound=1;
long soundrate=48000;
#ifdef WIN32
long soundbufsize=52;
#else
long soundbufsize=24;
#endif

#ifdef FRAMESKIP
static int frameskip=0;
#endif
static int inited=0;
static int isloaded=0;	// Is game loaded?

int srendlinev[2]={8,0};
int erendlinev[2]={231,239};


static uint8 *DrBaseDirectory;

int eoptions=0;

static void DriverKill(void);
static int DriverInitialize(FCEUGI *gi);
int gametype;

FCEUGI *CurGame=NULL;

// FCEUGI *gi comes from the call to loadGame -- if it sets a controller type it'll override the user's prefs
static void ParseGI(FCEUGI *gi)
{
	ParseGIInput(gi);
	gametype=gi->type;
}

void FCEUD_PrintError(char *s)
{
	AS3_Trace(AS3_String(s));
}

void FCEUD_MessageInt(int *i)
{
	AS3_Trace(AS3_Int((int) i));
}

void FCEUD_Message(char *s)
{
	AS3_Trace(AS3_String(s));
}

static char *cpalette=0;
static void LoadCPalette(void)
{
	uint8 tmpp[192];
	FILE *fp;
	
	if(!(fp=fopen(cpalette,"rb")))
	{
		printf(" Error loading custom palette from file: %s\n",cpalette);
		return;
	}
	fread(tmpp,1,192,fp);
	FCEUI_SetPaletteArray(tmpp);
	fclose(fp);
}
#ifdef EXTGUI
extern CFGSTRUCT GUIConfig;
#endif
static CFGSTRUCT fceuconfig[]={
	AC(soundrate),
	AC(soundq),
	AC(_sound),
	AC(soundvol),
	AC(soundbufsize),
	ACS(cpalette),
	AC(ntsctint),
	AC(ntschue),
	AC(ntsccol),
	AC(eoptions),
	ACA(srendlinev),
	ACA(erendlinev),
	ADDCFGSTRUCT(InputConfig),
	ADDCFGSTRUCT(DriverConfig),
#ifdef EXTGUI
	ADDCFGSTRUCT(GUIConfig),
#endif
	ENDCFGSTRUCT
};




static void SaveConfig(void)
{
	// Implement save state later
	return;
	
	/*
	char tdir[2048];
	sprintf(tdir,"%s"PSS"fceu98.cfg",DrBaseDirectory);
	FCEUI_GetNTSCTH(&ntsctint, &ntschue);
	SaveFCEUConfig(tdir,fceuconfig);
	 */
}

static void LoadConfig(void)
{
	char tdir[2048];
	sprintf(tdir,"%s"PSS"fceu98.cfg",DrBaseDirectory);
	FCEUI_GetNTSCTH(&ntsctint, &ntschue);	/* Get default settings if no config file exists. */
	LoadFCEUConfig(tdir,fceuconfig);	// Effectively does nothing
	InputUserActiveFix();
}

static void CreateDirs(void)
{
	// Not required for flash
}

#ifndef WIN32
static void SetSignals(void (*t)(int))
{
	int sigs[11]={SIGINT,SIGTERM,SIGHUP,SIGPIPE,SIGSEGV,SIGFPE,SIGKILL,SIGALRM,SIGABRT,SIGUSR1,SIGUSR2};
	int x;
	for(x=0;x<11;x++)
		signal(sigs[x],t);
}
#endif

static void DoArgs(int argc, char *argv[])
{
	int x;
	
	static ARGPSTRUCT FCEUArgs[]={
		{"-soundbufsize",0,&soundbufsize,0},
		{"-soundrate",0,&soundrate,0},
		{"-soundq",0,&soundq,0},
#ifdef FRAMESKIP
		{"-frameskip",0,&frameskip,0},
#endif
		{"-sound",0,&_sound,0},
		{"-soundvol",0,&soundvol,0},
		{"-cpalette",0,&cpalette,0x4001},
		{"-soundrecord",0,&soundrecfn,0x4001},
		
		{"-ntsccol",0,&ntsccol,0},
		{"-pal",0,&eoptions,0x8000|EO_PAL},
		
		{"-lowpass",0,&eoptions,0x8000|EO_LOWPASS},
		{"-gg",0,&eoptions,0x8000|EO_GAMEGENIE},
		{"-no8lim",0,&eoptions,0x8001},
		{"-snapname",0,&eoptions,0x8000|EO_SNAPNAME},
		{"-nofs",0,&eoptions,0x8000|EO_NOFOURSCORE},
		{"-clipsides",0,&eoptions,0x8000|EO_CLIPSIDES},
		{"-nothrottle",0,&eoptions,0x8000|EO_NOTHROTTLE},
		{"-slstart",0,&srendlinev[0],0},{"-slend",0,&erendlinev[0],0},
		{"-slstartp",0,&srendlinev[1],0},{"-slendp",0,&erendlinev[1],0},
		{0,(int *)InputArgs,0,0},
		{0,(int *)DriverArgs,0,0},
		{0,0,0,0}
	};
	
	ParseArguments(argc, argv, FCEUArgs);
	if(cpalette)
	{
		if(cpalette[0]=='0')
			if(cpalette[1]==0)
			{
				free(cpalette);
				cpalette=0;
			}
	}
	FCEUI_SetVidSystem((eoptions&EO_PAL)?1:0);
	FCEUI_SetGameGenie((eoptions&EO_GAMEGENIE)?1:0);
	FCEUI_SetLowPass((eoptions&EO_LOWPASS)?1:0);
	
	FCEUI_DisableSpriteLimitation(eoptions&1);
	FCEUI_SetSnapName(eoptions&EO_SNAPNAME);
	
	for(x=0;x<2;x++)
	{
		if(srendlinev[x]<0 || srendlinev[x]>239) srendlinev[x]=0;
		if(erendlinev[x]<srendlinev[x] || erendlinev[x]>239) erendlinev[x]=239;
	}
	
	FCEUI_SetRenderedLines(srendlinev[0],erendlinev[0],srendlinev[1],erendlinev[1]);
	DoDriverArgs();	// In the Flash driver this does nothing
}


/* Loads a game, given a full path/filename.  The driver code must be
 initialized after the game is loaded, because the emulator code
 provides data necessary for the driver code (number of scanlines to
 render, what virtual input devices to use, etc.).
 */
int LoadGame(const char *path)
{
	FCEUGI *tmp;
	
	CloseGame();
	if(!(tmp=FCEUI_LoadGame(path)))
		return 0;
	CurGame=tmp;
	
	ParseGI(tmp);
	RefreshThrottleFPS();
	
	if(!DriverInitialize(tmp))
		return(0);  
	if(soundrecfn)
	{
		if(!FCEUI_BeginWaveRecord(soundrecfn))
		{
			free(soundrecfn);
			soundrecfn=0;
		}
	}
	isloaded=1;
#ifdef EXTGUI
	if(eoptions&EO_AUTOHIDE) GUI_Hide(1);
#endif
	
#if NETWORK
	FCEUD_NetworkConnect();
#endif
	
	return 1;
}


/* Closes a game.  Frees memory, and deinitializes the drivers. */
int CloseGame(void)
{
	if(!isloaded) return(0);
	FCEUI_CloseGame();
	DriverKill();
	isloaded=0;
	CurGame=0;
	
	if(soundrecfn)
		FCEUI_EndWaveRecord();
	
#ifdef EXTGUI
	GUI_Hide(0);
#endif
	InputUserActiveFix();
	return(1);
}


void FCEUD_Update(uint8 *XBuf, int32 *Buffer, int Count);


// Our "tick" function

void DoFun(void)
{
	uint8 *gfx;  
	int32 *sound;
	int32 ssize;
	static int fskipc=0;
	
#ifdef FRAMESKIP
	fskipc=(fskipc+1)%(frameskip+1);
#endif
	
	if(NoWaiting)
	{
		gfx=0;
	}
	
	FCEUI_Emulate(&gfx, &sound, &ssize, fskipc);
	FCEUD_Update(gfx, sound, ssize);
}


int CLImain(int argc, char *argv[])
{
	// Body moved to f_tick in flash_sdl
}




// AS3 API

AS3_Val f_setup_main(void *data, AS3_Val args)
{
	// From CLImain()
	int ret;
	
	if(!(ret=FCEUI_Initialize()))
		return(0);
	
	// DEBUG
	// The next three lines probably aren't necessary, but I'm testing whether they have any side effects that alter the gamepad settings
	DrBaseDirectory=GetBaseDirectory();
	FCEUI_SetBaseDirectory(DrBaseDirectory);
	CreateDirs();
	
	
	LoadConfig();
	
	char *fakeArgs[0];	// Fake empty CLI args
	DoArgs(0, fakeArgs);
	
	FCEUI_SetNTSCTH(ntsccol, ntsctint, ntschue);
	
	if(cpalette)
		LoadCPalette();
	
	// Flash will provide the ROM at this virtual address
	if(!LoadGame("balloonboy.rom"))	// it's fake :P
	{
		// Error! Exit.
		DriverKill();
		return(0);
	}
}




static int DriverInitialize(FCEUGI *gi)
{
	if(InitVideo(gi))	// Video required
	{
		inited |= 4;
		
		if(InitSound(gi))
			inited |= 1;
		
		if(InitJoysticks())
			inited |= 2;
		
		if(InitKeyboard())	// Keyboard required
		{
			inited |= 8;
			InitOtherInput();
			
			return 1;		// OK
		}
		else
		{
			return 0;		// no keyboard
		}
	}
	else
	{
		return 0;			// no video
	}
}


static void DriverKill(void)
{
	SaveConfig();
	
#ifndef WIN32
	SetSignals(SIG_IGN);
#endif
	
	if(inited&2)
		KillJoysticks();
	if(inited&8)
		KillKeyboard();
	if(inited&4)
		KillVideo();
	if(inited&1)
		KillSound();
	if(inited&16)
		KillMouse();
	inited=0;
}

void FCEUD_Update(uint8 *XBuf, int32 *Buffer, int Count)
{
	if(Count)
	{
		int32 can=GetWriteSound();
		static int uflow=0;
		int32 tmpcan;
		
		if(can >= GetMaxSound()) uflow=1;	/* Go into massive underflow mode. */
		
		if(can > Count) can=Count;
		else uflow=0;
		
		WriteSound(Buffer,can);
		
		tmpcan = GetWriteSound();
		
		if((tmpcan < Count*0.90) && !uflow)
		{
			if(XBuf)
			{
				BlitScreen(XBuf);
			}
			Buffer+=can;
			Count-=can;
			if(Count)
			{
				if(NoWaiting)
				{
					can=GetWriteSound(); 
					if(Count>can) Count=can;
				}
				WriteSound(Buffer,Count);
			}
		}
	}
	else
	{
		if(XBuf)
		{
			BlitScreen(XBuf);
		}
	}
	
	FCEUD_UpdateInput();
}


/* Maybe ifndef WXWINDOWS would be better? ^_^ */
#ifndef EXTGUI
FILE *FCEUD_UTF8fopen(const char *fn, const char *mode)
{
	return(fopen(fn,mode));
}


#endif
