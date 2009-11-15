/*
 *  flash_main.h
 *  FlashNES
 *
 *  Created by Ed McManus on 11/13/09.
 *  Copyright 2009 Ed McManus.. All rights reserved.
 *
 */

#ifdef  __cplusplus
extern "C" {
#endif

#include "AS3.h"

#include "../../driver.h"
#include "../common/config.h"
#include "../common/args.h"

	extern int eoptions;
#define EO_NO8LIM      1
#define EO_SUBASE      2
#define EO_CLIPSIDES   8
#define EO_SNAPNAME    16
#define EO_NOFOURSCORE	32
#define EO_NOTHROTTLE	64
#define EO_GAMEGENIE	128
#define EO_PAL		256
#define EO_LOWPASS	512
#define EO_AUTOHIDE	1024
	
	extern int srendlinev[2],erendlinev[2];
	extern int NoWaiting;
	
	extern int soundvol;
	extern long soundq;
	extern int _sound;
	extern long soundrate;
	extern long soundbufsize;
	
	int CLImain(int argc, char *argv[]);
	
	// Flash setup -- old main function
	AS3_Val FLASH_setup_main();
	
#ifdef  __cplusplus
}
#endif

