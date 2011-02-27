/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2002 Xodnizel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AS3.h"

#ifdef SDL
#include "SDL.h"
#endif

#include "../../types.h"
#include "../../git.h"

#include "flash_sdl-sound.h"


#ifndef FCEUX_SOUND

// No sound support -- expose stubs

int InitSound(FCEUGI *gi)
{
	return 1;
}

void WriteSound(int32 *Buffer, int Count){}

int KillSound(void)
{
	return 1;
}
uint32 GetMaxSound(void)
{
	return 0;
}
uint32 GetWriteSound(void)
{
	return 0;
}
void SilenceSound(int s) {}


#else




// Attempt to port FCEUX's sound integration


// #include "../common/configSys.h"
// extern Config *g_config;	// Points to FCEUX's config struct

static volatile float *flashSoundBuffer = 0;
// static volatile int32 *flashSoundBuffer = 0;
static unsigned int s_BufferSize;
static unsigned int s_BufferRead;
static unsigned int s_BufferWrite;
static volatile unsigned int s_BufferIn;

static int s_mute = 0;

int f_soundvolume = 0;



/**
 * Initialize the audio subsystem.
 */
int InitSound(FCEUGI *gi)
{
	AS3_Trace(AS3_String("[CALL] InitSound"));
	
    int sound, soundrate, soundbufsize, soundvolume, soundtrianglevolume, soundsquare1volume, soundsquare2volume, soundnoisevolume, soundpcmvolume, soundq;

#ifdef SDL
    SDL_AudioSpec spec;
#endif
	
	// Sound enabled?
    // g_config->getOption("SDL.Sound", &sound);
    // if(!sound) {
    //     return 0;
    // }

#ifdef SDL
    memset(&spec, 0, sizeof(spec));
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
        puts(SDL_GetError());
        KillSound();
		
		AS3_Trace(AS3_String("\t\t[ERROR] RETURNING FROM AUDIO SETUP."));
        return(0);
    }
#endif

    // load configuration variables
    // g_config->getOption("SDL.SoundRate", &soundrate);
    // g_config->getOption("SDL.SoundBufSize", &soundbufsize);
    // g_config->getOption("SDL.SoundVolume", &soundvolume);
    // g_config->getOption("SDL.SoundQuality", &soundq);
    // g_config->getOption("SDL.TriangleVolume", &soundtrianglevolume);
    // g_config->getOption("SDL.Square1Volume", &soundsquare1volume);
    // g_config->getOption("SDL.Square2Volume", &soundsquare2volume);
    // g_config->getOption("SDL.NoiseVolume", &soundnoisevolume);
    // g_config->getOption("SDL.PCMVolume", &soundpcmvolume);
	
	// Hard-coding FCEUX's defaults
	soundrate = 44100;	// Sample rate, Flash uses a rate of 44,100 Hz
	soundbufsize = 120;	// Buffer in ms -- must be greater than 46ms otherwise we underflow Flash's required buffer of 2048, 80ms => 3528 s_BufferSize
	soundvolume = f_soundvolume = 100;
	soundq = 1;	// Sound quality -- default is 1, standard quality
	soundtrianglevolume = 256;
	soundsquare1volume = 256;
	soundsquare2volume = 256;
	soundnoisevolume = 256;
	soundpcmvolume = 256;

#ifdef SDL
    spec.freq = soundrate;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 512;
    spec.callback = fillaudio;
    spec.userdata = 0;
	
    s_BufferSize = soundbufsize * soundrate / 1000;	// Num samples in buffer (3528 @ 80ms)
	
    // For safety, set a bare minimum:
    if (s_BufferSize < spec.samples * 2)	// passes
		s_BufferSize = spec.samples * 2;
#else
	int numSamples = 512;
	
	s_BufferSize = soundbufsize * soundrate / 1000;	// Num samples in buffer (3528 @ 80ms)
	
    // For safety, set a bare minimum:
    if (s_BufferSize < numSamples * 2)	// passes
		s_BufferSize = numSamples * 2;
#endif

	
    // s_Buffer = (int *)malloc(sizeof(int) * s_BufferSize);
    flashSoundBuffer = (float *)malloc(sizeof(float) * s_BufferSize * 2);	// * 2 for stereo!
	// flashSoundBuffer = (int32 *)malloc(sizeof(int32) * s_BufferSize * 2);	// * 2 for stereo!
    s_BufferRead = s_BufferWrite = s_BufferIn = 0;
	
#ifdef SDL
    if(SDL_OpenAudio(&spec, 0) < 0)
	{
		AS3_Trace(AS3_String("\t\t[ERROR] InitSound: RETURNING FROM AUDIO SETUP."));
        puts(SDL_GetError());
        KillSound();
        return(0);
    }
    SDL_PauseAudio(0);
#endif
	
    FCEUI_SetSoundVolume(soundvolume);
    FCEUI_SetSoundQuality(soundq);
    FCEUI_Sound(soundrate);
    FCEUI_SetTriangleVolume(soundtrianglevolume);
    FCEUI_SetSquare1Volume(soundsquare1volume);
    FCEUI_SetSquare2Volume(soundsquare2volume);
    FCEUI_SetNoiseVolume(soundnoisevolume);
    FCEUI_SetPCMVolume(soundpcmvolume);
	
	AS3_Trace(AS3_String("[RETURN] InitSound SUCCESS"));
	
    return(1);
}


/**
 * Returns the size of the audio buffer.
 */
uint32 GetMaxSound(void)	// 3528 samples
{
    return(s_BufferSize);
}

/**
 * Returns the amount of free space in the audio buffer.
 */
uint32 GetWriteSound(void)
{
    return(s_BufferSize - s_BufferIn);	// s_BufferIn is equiv to FCE's buffer.position! Not to the Flash buffer, which is stereo.
}

/**
 * Send a sound clip to the audio subsystem.
 */

	char *tmpNumString;

void WriteSound(int32 *buf, int Count)
{
	// AS3_Trace(AS3_String("[CALL] WriteSound"));
	
    while(Count)	// Count == buf length
	{
		// Both are in units of samples per channel
		while(s_BufferIn >= s_BufferSize)
		{
			// AS3_Trace(AS3_String("[ERROR] C SOUND BUFFER IS FULL."));
			return;	// Don't let sound errors interrupt gameplay
		}
		
		// Notes:
		//  - s_Buffer is an array of floats, alternating (interpolating) floats for left and right channels
		//  - s_BufferWrite is the position to write to
		//  - *buf is the current int to write as a float
		//	- *buf is an array of signed ints, but we need to convert them into floats in the range of 0-1
		//	- To do that, divide the current buf value by 32768.0f (2^15, bit reserved for sign)
		// ed: is it an int or a short? use 2^31 is it's an int.
		
		// s_Buffer[s_BufferWrite] = s_Buffer[s_BufferWrite + 1] = (float *)buf / 32768.0f;	// MONO -> STEREO
		// s_BufferWrite = (s_BufferWrite + 2) % (s_BufferSize);


		
		// Trying to figure out the bit depth of FCE's PCM encoding!
		// Can't fit in a 12-bit space but could possibly be 13-bit
		
		// Float version
		flashSoundBuffer[s_BufferWrite] = *buf / 4096.0f;	// Chan 1
		s_BufferWrite = (s_BufferWrite + 1) % (s_BufferSize);
		
		flashSoundBuffer[s_BufferWrite] = *buf / 4096.0f;	// Chan 2
		s_BufferWrite = (s_BufferWrite + 1) % (s_BufferSize);
		
		// if (Count <= 50)
		// {
		// 	if (Count == 50)
		// 	{
		// 		AS3_Trace(AS3_String("From C:"));
		// 	}
		// 	
		// 	sprintf( tmpNumString, "sizeof(float): %i    int: %i     cast to float: %6.4g   fromSoundBuffer: %6.4g", sizeof(float), *buf, (float) *buf, flashSoundBuffer[s_BufferWrite-1] );
		// 	AS3_Trace(AS3_String(tmpNumString));
		// }
		
		
		// if (Count < 20)
		// {
		// 	AS3_Trace(AS3_String("Copying to position: "));
		// 	AS3_Trace(AS3_Int(s_BufferWrite));
		// }
		// 
		// // int32 Version
		// flashSoundBuffer[s_BufferWrite] = (int32) *buf;	// Chan 2
		// s_BufferWrite = (s_BufferWrite + 1) % (s_BufferSize);
		// 
		// if (Count < 20)
		// {
		// 	AS3_Trace(AS3_String("Copying to position: "));
		// 	AS3_Trace(AS3_Int(s_BufferWrite));
		// }
		// 
		// flashSoundBuffer[s_BufferWrite] = (int32) *buf;	// Chan 2
		// s_BufferWrite = (s_BufferWrite + 1) % (s_BufferSize);
		
		
		Count--;		// One down
		buf++;			// Move pointer to next sample
		s_BufferIn++;	// Update FCE's buffer.position counter
	}
	
	// AS3_Trace(AS3_String("[RETURN] WriteSound"));
}


// Flush internal sound buffer
char f_tmpstr[100];
AS3_Val f_PaintSound( void *data, AS3_Val args )	// paintSound(event.data)
{
	// AS3_Trace(AS3_String("[CALL] PaintSound. Debug following:"));
	
	// sprintf( f_tmpstr, "s_BufferIn: %i    s_BufferWrite = %i    s_BufferRead = %i     s_BufferSize = %i\n", s_BufferIn, s_BufferWrite, s_BufferRead, s_BufferSize);
	
	// AS3_Trace(AS3_String(f_tmpstr));
	
	if (s_BufferWrite > 4096)
	{
		//
		// TODO: limit to 8192 samples per channel

		// Grab event.data parameter
		AS3_Val soundData;
		AS3_ArrayValue( args, "AS3ValType", &soundData );

		AS3_ByteArray_writeBytes( soundData, flashSoundBuffer, s_BufferWrite * sizeof(float) );	// s_BufferWrite is the INDEX, not the number of bytes

		// Now that we've written the data, reset our counters
		s_BufferWrite = 0;
		s_BufferIn = 0;
	}
	
	// AS3_Trace(AS3_String("[RETURN] PaintSound"));
	
	return AS3_Int(0);
}


/**
 * Pause (1) or unpause (0) the audio output.
 */
void SilenceSound(int n)
{
#ifdef SDL
    SDL_PauseAudio(n);
#endif
}

/**
 * Shut down the audio subsystem.
 */
int KillSound(void)
{	
    FCEUI_Sound(0);
#ifdef SDL
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
#endif
    if(flashSoundBuffer) {
        free((void *)flashSoundBuffer);
        flashSoundBuffer = 0;
    }
    return(0);
}


/**
 * Adjust the volume either down (-1), up (1), or to the default (0).
 * Unmutes if mute was active before.
 */
void FCEUD_SoundVolumeAdjust(int n)
{
	int soundvolume = f_soundvolume;

    switch(n) {
    case -1:
        soundvolume -= 10;
        if(soundvolume < 0) {
            soundvolume = 0;
        }
        break;
    case 0:
        soundvolume = 100;
        break;
    case 1:
        soundvolume += 10;
        if(soundvolume > 150) {
            soundvolume = 150;
        }
        break;
    }
	
    s_mute = 0;
    FCEUI_SetSoundVolume(soundvolume);
    
	f_soundvolume = soundvolume;
	
    FCEU_DispMessage("Sound volume %d.", soundvolume);
}


/**
 * Toggles the sound on or off.
 */
void FCEUD_SoundToggle(void)
{
	AS3_Trace(AS3_String("[CALL] SoundToggle"));
    if(s_mute)
	{
		int soundvolume = f_soundvolume;
        s_mute = 0;
        FCEUI_SetSoundVolume(soundvolume);
        FCEU_DispMessage("Sound mute off.");
    }
	else
	{
        s_mute = 1;
        FCEUI_SetSoundVolume(0);
        FCEU_DispMessage("Sound mute on.");
    }
}




#endif
