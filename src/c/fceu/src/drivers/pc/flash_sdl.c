/*
 *  flash_sdl.c
 *  FlashNES
 *
 *  Created by Ed McManus on 11/13/09.
 *  Copyright 2009 Ed McManus.. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AS3.h"
#include "flash_sdl.h"
#include "flash_sdl-sound.h"

#ifdef SDL
#include "sdl-video.h"
#endif

#if NETWORK
#include "unix-netplay.h"
#endif

#include "throttle.h"
#include "input.h"
#include "dface.h"



DSETTINGS Settings;
CFGSTRUCT DriverConfig[]={
	AC(Settings.special),
	AC(Settings.specialfs),
	AC(_doublebuf),
	AC(_xscale),
	AC(_yscale),
	AC(_xscalefs),
	AC(_yscalefs),
	AC(_bpp),
	AC(_efx),
	AC(_efxfs),
	AC(_fullscreen),
	AC(_xres),
	AC(_yres),
	ENDCFGSTRUCT
};



// Don't care about usage
char *DriverUsage="";

ARGPSTRUCT DriverArgs[]={
	{"-special",0,&Settings.special,0},
	{"-specialfs",0,&Settings.specialfs,0},
	{"-doublebuf",0,&_doublebuf,0},
	{"-bpp",0,&_bpp,0},
	{"-xscale",0,&_xscale,2},
	{"-yscale",0,&_yscale,2},
	{"-efx",0,&_efx,0},
	{"-xscalefs",0,&_xscalefs,2},
	{"-yscalefs",0,&_yscalefs,2},
	{"-efxfs",0,&_efxfs,0},
	{"-xres",0,&_xres,0},
	{"-yres",0,&_yres,0},
	{"-fs",0,&_fullscreen,0},
	{0,0,0,0}
};

// Settings, Args
static void SetDefaults(void)
{
	// Most of this gets overridden in our call to set SDL video mode
	Settings.special=Settings.specialfs=0;
	_bpp=16;
	_xres=640;
	_yres=480;
	_fullscreen=0;
	_xscale=2.50;
	_yscale=2;
	_xscalefs=_yscalefs=2;
	_efx=_efxfs=0;
}

void DoDriverArgs(void) {}



//
// Flash Entrance. Replace sdl.c's main()
//

int main()
{
	// Build API
	AS3_Val setupMethod = AS3_Function(NULL, (AS3_ThunkProc) f_setup);
	AS3_Val tickMethod = AS3_Function(NULL, (AS3_ThunkProc) f_tick);
	AS3_Val getDisplayPointerMethod = AS3_Function(NULL, (AS3_ThunkProc) f_getDisplayPointer);
	AS3_Val quitApplicationMethod = AS3_Function(NULL, (AS3_ThunkProc) f_quitApplication);
	AS3_Val setEventManagerMethod = AS3_Function(NULL, (AS3_ThunkProc) f_setEventManager);
	AS3_Val paintSoundMethod = AS3_Function(NULL, (AS3_ThunkProc) f_PaintSound);
	
    AS3_Val libNES = AS3_Object( "setup:AS3ValType, tick:AS3ValType, getDisplayPointer:AS3ValType, quit:AS3ValType, setEventManager:AS3ValType, paintSound:AS3ValType", 
								 setupMethod, tickMethod, getDisplayPointerMethod, quitApplicationMethod, setEventManagerMethod, paintSoundMethod );
    
	AS3_Release( setupMethod );
	AS3_Release( tickMethod );
	AS3_Release( getDisplayPointerMethod );
	AS3_Release( quitApplicationMethod );
	AS3_Release( setEventManagerMethod );
	AS3_Release( paintSoundMethod );
	
    AS3_LibInit(libNES);
    return 0;
}


// Flash API

AS3_Val f_setup(void *data, AS3_Val args)
{
	f_setup_sdl();
	f_setup_main();
	return AS3_Int(0);
}


AS3_Val f_setup_sdl()
{
	// From main():
	
	FCEUD_Message("\nStarting FCE Ultra "FCEU_VERSION"...\n");
	
#ifdef SDL
	if(SDL_Init(SDL_INIT_VIDEO)) /* SDL_INIT_VIDEO Needed for (joystick config) event processing? */
	{
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		return(-1);
	}
#endif
	
	SetDefaults();
	
	return AS3_Int(0);
}

AS3_Val f_tick(void *data, AS3_Val args)
{	
	DoFun();
	
	return AS3_Int(0);
}


AS3_Val f_quitApplication(void *data, AS3_Val args)
{	
	// Old CLIMain:
	SaveConfig();
	FCEUI_Kill();

#ifdef SDL
	// Old main:
	SDL_Quit();
#endif
	return(0); // always successful
}

#ifdef SDL
extern SDL_Surface *screen;
#endif
AS3_Val f_getDisplayPointer(void *data, AS3_Val args)
{
	#ifdef SDL
	return AS3_Ptr(screen->pixels);
	#else
	return AS3_Int(0);
	#endif
}


// This is defined in SDL -- It's an AS3 reference to the event manager in the actionscript wrapper
extern AS3_Val FLASH_EVENT_MANAGER_OBJECT;
AS3_Val f_setEventManager( void *data, AS3_Val args )
{
	AS3_Val eventManager;
	AS3_ArrayValue( args, "AS3ValType", &eventManager );

	FLASH_EVENT_MANAGER_OBJECT = eventManager;
	
	return AS3_Int(0);
}

// Back to the Future Part C...





// Mouse

int InitMouse(void) { return(0); }

void KillMouse(void){}

void GetMouseData(uint32 *d)
{
	int x,y;
	uint32 t;

#ifdef SDL	
	t=SDL_GetMouseState(&x,&y);
#endif

	d[2]=0;
#ifdef SDL
	if(t&SDL_BUTTON(1))
		d[2]|=1;
	if(t&SDL_BUTTON(3))
		d[2]|=2;
#endif
	t=PtoV(x,y); 
	d[0]=t&0xFFFF;
	d[1]=(t>>16)&0xFFFF;
}


// Keyboard

int InitKeyboard(void) { return(1); }

int UpdateKeyboard(void) { return(1); }

void KillKeyboard(void) {}

void UpdatePhysicalInput(void)
{
#ifdef SDL
	SDL_Event event;
	
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT: CloseGame(); puts("Quit"); break;
		}
	}
	//SDL_PumpEvents();
#endif
}

static uint8 *KeyState=NULL;

char *GetKeyboard(void)
{
#ifdef SDL
	KeyState=SDL_GetKeyState(0);
	return((char *)KeyState);
#else
	return ((char ) 0);
#endif
}


uint8 *GetBaseDirectory(void)
{
	char *ret = "";
	return(ret);
}


// Button press test (given a button struct, is that key currently being pressed?)

int DTestButton(ButtConfig *bc)
{
	int x;
	
	for(x=0; x < bc->NumC; x++)	// Cycle through available controllers
	{
		if(bc->ButtType[x]==BUTTC_KEYBOARD)	// Keyboard event
		{
			if(KeyState[bc->ButtonNum[x]])
			{
				return(1);
			}
		}
		else if(bc->ButtType[x]==BUTTC_JOYSTICK) 	// Joystick event
		{
			if(DTestButtonJoy(bc))
				return(1);
		}
	}
	return(0);
}

static int bcpv,bcpj;

int ButtonConfigBegin(void)
{
#ifdef SDL
	SDL_Surface *screen;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
#endif
	bcpv=KillVideo();
	bcpj=KillJoysticks();

#ifdef SDL	
	if(!(SDL_WasInit(SDL_INIT_VIDEO)&SDL_INIT_VIDEO))
		if(SDL_InitSubSystem(SDL_INIT_VIDEO)==-1)
		{
			FCEUD_Message(SDL_GetError());
			return(0);
		} 
	
	screen = SDL_SetVideoMode(300, 1, 8, 0); 
	SDL_WM_SetCaption("Button Config",0);
#endif
	
	InitJoysticks();
	
	return(1);
}

void ButtonConfigEnd(void)
{ 
	extern FCEUGI *CurGame;
	KillJoysticks();
#ifdef SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO); 
#endif
	if(bcpv) InitVideo(CurGame);
	if(bcpj) InitJoysticks();
}

int DWaitButton(const uint8 *text, ButtConfig *bc, int wb)
{
#ifdef SDL
	SDL_Event event;
	
	static int32 LastAx[64][64];
	int x,y;
	
	SDL_WM_SetCaption(text,0);
	
	puts(text);
	
	for(x=0;x<64;x++) 
		for(y=0;y<64;y++)
			LastAx[x][y]=0x100000;
	
	while(SDL_WaitEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:bc->ButtType[wb]=BUTTC_KEYBOARD;
				bc->DeviceNum[wb]=0;
				bc->ButtonNum[wb]=event.key.keysym.sym;
				return(1);
			case SDL_JOYBUTTONDOWN:bc->ButtType[wb]=BUTTC_JOYSTICK;
				bc->DeviceNum[wb]=event.jbutton.which;
				bc->ButtonNum[wb]=event.jbutton.button; 
				return(1);
			case SDL_JOYHATMOTION:if(!(event.jhat.value&SDL_HAT_CENTERED))
			{
				bc->ButtType[wb]=BUTTC_JOYSTICK;
				bc->DeviceNum[wb]=event.jhat.which;
				bc->ButtonNum[wb]=0x2000|((event.jhat.hat&0x1F)<<8)|event.jhat.value;
			}
				break;
			case SDL_JOYAXISMOTION: 
				if(LastAx[event.jaxis.which][event.jaxis.axis]==0x100000)
				{
					if(abs(event.jaxis.value)<1000)
						LastAx[event.jaxis.which][event.jaxis.axis]=event.jaxis.value;
				}
				else
				{
					if(abs(LastAx[event.jaxis.which][event.jaxis.axis]-event.jaxis.value)>=8192)
					{
						bc->ButtType[wb]=BUTTC_JOYSTICK;
						bc->DeviceNum[wb]=event.jaxis.which;
						bc->ButtonNum[wb]=0x8000|(event.jaxis.axis)|((event.jaxis.value<0)?0x4000:0);
						return(1);
					}
				}
				break;
		}
	}
#endif
	
	return(0);
}

uint64 FCEUD_GetTime(void)
{
#ifdef SDL
	return(SDL_GetTicks());
#endif
}

uint64 FCEUD_GetTimeFreq(void)
{
	return(1000);
}
