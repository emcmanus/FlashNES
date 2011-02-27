/*
 *  flash_sdl.h
 *  FlashNES
 *
 *  Created by Ed McManus on 11/13/09.
 *  Copyright 2009 Ed McManus.. All rights reserved.
 *
 */

#ifdef SDL
#include <SDL.h>
#endif

#include "AS3.h"
#include "flash_main.h"
#include "dface.h"
#include "input.h"

// Flash Interface

AS3_Val f_setup(void *data, AS3_Val args);
AS3_Val f_setup_sdl();
AS3_Val f_tick(void *data, AS3_Val args);
AS3_Val f_quitApplication(void *data, AS3_Val args);
AS3_Val f_getDisplayPointer(void *data, AS3_Val args);
AS3_Val f_setEventManager( void *data, AS3_Val args );



// Other

int DTestButtonJoy(ButtConfig *bc);

typedef struct {
	int xres;
	int yres;
	double xscale,yscale;
	double xscalefs,yscalefs;
	int efx,efxfs;
	int bpp;
	int fullscreen;
	int doublebuf;
	char *fshack;
	char *fshacksave;
	#ifdef OPENGL
	int opengl;
	int openglip;
	int stretchx,stretchy;
	#endif
	int special,specialfs;
} DSETTINGS;

extern DSETTINGS Settings;

#define _doublebuf Settings.doublebuf
#define _bpp Settings.bpp
#define _xres Settings.xres
#define _yres Settings.yres
#define _fullscreen Settings.fullscreen
#define _xscale Settings.xscale
#define _yscale Settings.yscale
#define _xscalefs Settings.xscalefs
#define _yscalefs Settings.yscalefs
#define _efx Settings.efx
#define _efxfs Settings.efxfs
#define _ebufsize Settings.ebufsize
#define _fshack Settings.fshack
#define _fshacksave Settings.fshacksave

#ifdef OPENGL
#define _opengl Settings.opengl
#define _openglip Settings.openglip
#define _stretchx Settings.stretchx
#define _stretchy Settings.stretchy
#endif

