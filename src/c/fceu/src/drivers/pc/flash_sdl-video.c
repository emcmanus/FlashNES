
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../types.h"

#include "flash_sdl.h"
#include "flash_sdl-video.h"

#include "../common/vidblit.h"

#include "dface.h"



//
// Flash version of sdl-video.c
//
// DOES NOT USE SDL!
//


// Surfaces
Flash_Surface *screen;

// Palette
static int refreshPalette;


int KillVideo(void)		/* Return 1 if video was killed, 0 otherwise(video wasn't initialized). */
{
	return 1;
}


int InitVideo(FCEUGI *gi)
{
	// Initialize Struct
	screen = malloc(sizeof(Flash_Surface));
	
	screen->w = 256;
	screen->h = 224;
	
	// Display Buffer
	screen->pixels = (uint32 *) malloc( sizeof(uint32) * screen->w * screen->h );
	
	return 1;
}


void ToggleFS(void)
{
	// Do nothing in C
}


static Flash_Color psdl[256];

void FCEUD_SetPalette(uint8 index, uint8 r, uint8 g, uint8 b)
 {
    psdl[index].r = r;
    psdl[index].g = g;
    psdl[index].b = b;
	
	refreshPalette = 1;
}

void FCEUD_GetPalette(uint8 index, uint8 *r, uint8 *g, uint8 *b)
 {
    *r = psdl[index].r;
    *g = psdl[index].g;
    *b = psdl[index].b;
}

static void RedoPalette(void)
{
	// sure. ... what?
	
	SetPaletteBlitToHigh((uint8 *) psdl);
}

void LockConsole() {}
void UnlockConsole() {}

void BlitScreen(uint8 *XBuf)	// Only called once per cycle, so we can upcast here
{
	
}

uint32 PtoV(uint16 x, uint16 y)
{
	return 0;
}
