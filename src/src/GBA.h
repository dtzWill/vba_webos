// -*- C++ -*-
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef VBA_GBA_H
#define VBA_GBA_H

#include "System.h"

#define SAVE_GAME_VERSION_1 1
#define SAVE_GAME_VERSION_2 2
#define SAVE_GAME_VERSION_3 3
#define SAVE_GAME_VERSION_4 4
#define SAVE_GAME_VERSION_5 5
#define SAVE_GAME_VERSION_6 6
#define SAVE_GAME_VERSION_7 7
#define SAVE_GAME_VERSION_8 8
#define SAVE_GAME_VERSION  SAVE_GAME_VERSION_8

typedef struct {
  u8 *address;
  u32 mask;
} memoryMap;

typedef union {
  struct {
#ifdef WORDS_BIGENDIAN
    u8 B3;
    u8 B2;
    u8 B1;
    u8 B0;
#else
    u8 B0;
    u8 B1;
    u8 B2;
    u8 B3;
#endif
  } B;
  struct {
#ifdef WORDS_BIGENDIAN
    u16 W1;
    u16 W0;
#else
    u16 W0;
    u16 W1;
#endif
  } W;
#ifdef WORDS_BIGENDIAN
  volatile u32 I;
#else
	u32 I;
#endif
} reg_pair;

#ifndef NO_GBA_MAP
extern memoryMap map[256];
#endif

extern reg_pair reg[45];
extern u8 biosProtected[4];

//Note we cannot merge this with the appropriate emulated CPSR
//since we only can count the *_FLAG components, not cpu mode
//or the other 20-some bits
extern u32 CPU_FLAGS;
//I renamed these to make it super clear wherever they were used.
//Search-replace works, but this makes super sure :)
//Also makes sure I don't much with the GB versions of these
extern bool NN_FLAG;
extern bool ZZ_FLAG;
extern bool CC_FLAG;
extern bool VV_FLAG;
extern bool armIrqEnable;
extern bool armState;
extern int armMode;
extern void (*cpuSaveGameFunc)(u32,u8);

extern bool freezeWorkRAM[0x40000];
extern bool freezeInternalRAM[0x8000];
extern bool CPUReadGSASnapshot(const char *);
extern bool CPUWriteGSASnapshot(const char *, const char *, const char *, const char *);
extern bool CPUWriteBatteryFile(const char *);
extern bool CPUReadBatteryFile(const char *);
extern bool CPUExportEepromFile(const char *);
extern bool CPUImportEepromFile(const char *);
extern bool CPUWritePNGFile(const char *);
extern bool CPUWriteBMPFile(const char *);
extern void CPUCleanUp();
extern void CPUUpdateRender();
extern bool CPUReadMemState(char *, int);
extern bool CPUReadState(const char *);
extern bool CPUWriteMemState(char *, int);
extern bool CPUWriteState(const char *);
extern int CPULoadRom(const char *);
extern void CPUUpdateRegister(u32, u16);
extern void CPUWriteHalfWord(u32, u16);
extern void CPUWriteByte(u32, u8);
extern void CPUInit(const char *,bool);
extern void CPUReset();
extern void CPULoop(int);
extern void CPUCheckDMA(int,int);
extern bool CPUIsGBAImage(const char *);
extern bool CPUIsZipFile(const char *);
#ifdef PROFILING
extern void cpuProfil(char *buffer, int, u32, int);
extern void cpuEnableProfiling(int hz);
#endif

extern struct EmulatedSystem GBASystem;

#define R13_IRQ  18
#define R14_IRQ  19
#define SPSR_IRQ 20
#define R13_USR  26
#define R14_USR  27
#define R13_SVC  28
#define R14_SVC  29
#define SPSR_SVC 30
#define R13_ABT  31
#define R14_ABT  32
#define SPSR_ABT 33
#define R13_UND  34
#define R14_UND  35
#define SPSR_UND 36
#define R8_FIQ   37
#define R9_FIQ   38
#define R10_FIQ  39
#define R11_FIQ  40
#define R12_FIQ  41
#define R13_FIQ  42
#define R14_FIQ  43
#define SPSR_FIQ 44

#include "Cheats.h"
#include "Globals.h"
#include "EEprom.h"
#include "Flash.h"

//Takes ZZ_FLAG, etc and puts them back into the cpsr
inline void update_flags_from_components()
{
  u32 CPSR = 0;
  if(NN_FLAG)
    CPSR |= 0x80000000;
  if(ZZ_FLAG)
    CPSR |= 0x40000000;
  if(CC_FLAG)
    CPSR |= 0x20000000;
  if(VV_FLAG)
    CPSR |= 0x10000000;

  CPU_FLAGS = CPSR;

}

inline void update_components_from_flags()
{
  NN_FLAG = CPU_FLAGS & 0x80000000;
  ZZ_FLAG = CPU_FLAGS & 0x40000000;
  CC_FLAG = CPU_FLAGS & 0x20000000;
  VV_FLAG = CPU_FLAGS & 0x10000000;
}

#endif //VBA_GBA_H
