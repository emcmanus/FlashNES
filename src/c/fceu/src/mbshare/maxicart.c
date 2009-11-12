/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 1998 BERO
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

#include "mapsafe.h"

typedef struct 
{
 uint8 PRGSelect;
 uint8 RAMSelect;
} M125Data;

static M125Data Mapper125;

static DECLFW(M125w)
{
 if(A==0x5000) 
 {
  Mapper125.PRGSelect=V&0xF;
  setprg32(0x8000,V&0xF);
 }
 else if(A==0x5001) 
 {
  Mapper125.RAMSelect=V&0x33;
  setchr8(V&3);
  setprg8r(0x10,0x6000,(V>>4)&3);
 }
}

static void M125_Reset(void)
{

}

static void M125_Power(void)
{
 SetReadHandler(0x6000,0xFFFF,CartBR);
 SetWriteHandler(0x5000,0x5001,M125w);
 SetWriteHandler(0x6000,0x7FFF,CartBW);
 setprg32(0x8000,~0);
}

static void M125_Restore(int version)
{
 setprg32(0x8000,Mapper125.PRGSelect&0xF);   
 setchr8(Mapper125.RAMSelect&3);
 setprg8r(0x10,0x6000,(Mapper125.RAMSelect>>4)&3);
}

int Mapper125_init(void)
{
 info->Reset=M125_Reset;
 info->Power=M125_Power;
 info->StateRestore=M125_Restore;

 AddExState(&Mapper125.RAMSelect, 1, 0, "RAMS");
 AddExState(&Mapper125.PRGSelect, 1, 0, "PRGS");
 
 if(battery)
 {
  info->SaveGame[0]=Mapper125->WRAM;
  info->SaveGameLen[0]=32768;
 }
 return(1);
}
