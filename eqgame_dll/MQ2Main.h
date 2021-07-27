#pragma once
/*****************************************************************************
    MQ2Main.dll: MacroQuest2's extension DLL for EverQuest
    Copyright (C) 2002-2003 Plazmic, 2003-2005 Lax

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2, as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
******************************************************************************/
#define DIRECTINPUT_VERSION 0x800


// uncomment this line to turn off the single-line benchmark macro
// #define DISABLE_BENCHMARKS

#pragma warning(disable:4530)
#pragma warning(disable:4786)

// Windows Header Files:
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <winsock.h>
#include <math.h>
#include <map>
#include <string>
#include <algorithm>
#ifndef ISXEQ
#include "..\Dxsdk81\include\dinput.h"
#include "..\Detours\inc\detours.h" 
#include "..\Blech\Blech.h"
#else
#ifndef MQ2PLUGIN
#include "ISXEQ\ISXEQ.h"
#else
#pragma pack(push)
#pragma pack(4)
#include <isxdk.h>
#pragma pack(pop)
#include "ISXEQ\ISXEQServices.h"
#endif
#define PMQ2TYPEMEMBER PLSTYPEMEMBER
#define PMQ2TYPEMETHOD PLSTYPEMETHOD
#define MQ2Type LSType
#define MQ2TYPEVAR LSTYPEVAR
#endif
#include "eqgame.h"
#ifdef EQLIB_EXPORTS
#define EQLIB_API extern "C" __declspec(dllexport)
#define EQLIB_VAR extern "C" __declspec(dllexport)
#define EQLIB_OBJECT __declspec(dllexport)
#else
#define EQLIB_API extern "C" __declspec(dllimport)
#define EQLIB_VAR extern "C" __declspec(dllimport)
#define EQLIB_OBJECT __declspec(dllimport)
#endif

#ifdef MQ2PLUGIN
#define FromPlugin 1
#else
#define FromPlugin 0
#endif

// Lax/Blech event support *READY*
#define USEBLECHEVENTS


// reroute malloc/free
EQLIB_API VOID *MQ2Malloc(size_t size);
EQLIB_API VOID MQ2Free(VOID *memblock);
/*
#ifdef MQ2PLUGIN
#define malloc(x) MQ2Malloc(x)
#define free(x) MQ2Free(x)
#else
#ifdef DEBUG_ALLOC
#define malloc(x) MQ2Malloc(x)
#define free(x) MQ2Free(x)
#endif
#endif
*/
#ifdef DEBUG_ALLOC
extern DWORD CountMallocs;
extern DWORD CountFrees;
#endif

#ifndef ISXEQ
#define FUNCTION_AT_ADDRESS(function,offset) __declspec(naked) function\
{\
	__asm{mov eax, offset};\
	__asm{jmp eax};\
}

#define FUNCTION_AT_VARIABLE_ADDRESS(function,variable) __declspec(naked) function\
{\
	__asm{mov eax, [variable]};\
	__asm{jmp eax};\
}

#define FUNCTION_AT_VIRTUAL_ADDRESS(function,virtualoffset) __declspec(naked) function\
{\
	__asm{mov eax, [ecx]};\
	__asm{lea eax, [eax+virtualoffset]};\
	__asm{mov eax, [eax]};\
	__asm{jmp eax};\
}
#endif

#define PreserveRegisters(code) \
{\
	__asm {push eax};\
	__asm {push ebx};\
	__asm {push ecx};\
	__asm {push edx};\
	__asm {push esi};\
	__asm {push edi};\
	code;\
	__asm {pop edi};\
	__asm {pop esi};\
	__asm {pop edx};\
	__asm {pop ecx};\
	__asm {pop ebx};\
	__asm {pop eax};\
}

#define SetWndNotification(thisclass) \
{\
	int (thisclass::*pfWndNotification)(CXWnd *pWnd, unsigned int Message, void *unknown)=&thisclass::WndNotification;\
	SetvfTable(32,*(DWORD*)&pfWndNotification);\
}

#ifndef ISXEQ
#define EzDetour(offset,detour,trampoline) AddDetourf((DWORD)offset,&detour,&trampoline)
#endif

#ifndef DOUBLE
typedef double DOUBLE;
#endif
#ifndef THIS_
#define THIS_                   INTERFACE FAR* This,
#endif

// DEBUGGING
//#ifndef DEBUG_TRY
//#define DEBUG_TRY
//#endif
#ifdef DEBUG_TRY
#define DebugTry(x) DebugSpew("Trying %s",#x);x;DebugSpew("%s complete",#x)
#else
#define DebugTry(x) x
#endif

#ifndef ISXEQ
#define MakeLower(yourstring) transform (yourstring.begin(),yourstring.end(), yourstring.begin(), tolower);
#endif

#define MAX_VARNAME 64
#define MAX_STRING            2048


#include "EQData.h"
#include "EQUIStructs.h"

//class CXMLData *GetXMLData(class CXWnd *pWnd)

#include "EQClasses.h"

EQLIB_API BOOL Calculate(PCHAR szFormula, DOUBLE& Dest);

/* DETOURING API */
EQLIB_API VOID InitializeMQ2Detours();
EQLIB_API VOID ShutdownMQ2Detours();
#ifndef ISXEQ
EQLIB_API BOOL AddDetour(DWORD address, PBYTE pfDetour=0, PBYTE pfTrampoline=0, DWORD Count=20);
EQLIB_API VOID AddDetourf(DWORD address, ...);
EQLIB_API VOID RemoveDetour(DWORD address);
#else
#define RemoveDetour EzUnDetour
#endif

#define LIGHT_COUNT     13
#define MAX_COMBINES    52
#define MAX_ITEMTYPES   53

#define GAMESTATE_CHARSELECT    1
#define GAMESTATE_SOMETHING     4
#define GAMESTATE_INGAME        5
#define GAMESTATE_PRECHARSELECT 6
#define GAMESTATE_LOGGINGIN     253
#define GAMESTATE_UNLOADING     255

#define XWM_LCLICK				1
#define XWM_LMOUSEUP			2
#define XWM_RCLICK				3
#define XWM_HITENTER			6
#define XWM_CLOSE				10
#define XWM_NEWVALUE	        14
#define XWM_UNKNOWN				19
#define XWM_MOUSEOVER			21
#define XWM_HISTORY				22
#define XWM_LCLICKHOLD			23

#define XKF_SHIFT				1
#define XKF_CTRL				2
#define XKF_LALT				4
#define XKF_RALT				8

#define MAX_ITEM4xx			416

class CAutoLock {
public:
    inline void Lock() { if (!bLocked) { EnterCriticalSection(pLock); bLocked = TRUE; }}
    inline void Unlock() { if (bLocked) { LeaveCriticalSection(pLock); bLocked = FALSE; }}
    CAutoLock(LPCRITICAL_SECTION _pLock) { bLocked = FALSE; pLock = _pLock; Lock(); }
    ~CAutoLock() { Unlock(); }

private:
    LPCRITICAL_SECTION pLock;
    BOOL bLocked;
};


#define MAX_WEAPONS		0x000000ff

EQLIB_API VOID memchecks_tramp(PVOID,DWORD,PCHAR,DWORD,BOOL); 
EQLIB_API VOID memchecks(PVOID,DWORD,PCHAR,DWORD,BOOL);

EQLIB_API VOID AppendCXStr(PCXSTR *cxstr, PCHAR text); 
EQLIB_API VOID SetCXStr(PCXSTR *cxstr, PCHAR text); 
EQLIB_API DWORD GetCXStr(PCXSTR pCXStr, PCHAR szBuffer, DWORD maxlen=MAX_STRING);

#include "EQGlobals.h"