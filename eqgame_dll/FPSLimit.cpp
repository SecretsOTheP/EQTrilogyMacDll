#include <Windows.h>
#include <stdio.h>
#include "common.h"
HWND EQhWnd = 0;
HMODULE heqwMod = 0;
DWORD heqwModoff = 0;
DWORD gFG_MAX=30;
DWORD gBG_MAX=30;
DWORD CurMax=0;
#define FRAME_COUNT 64
DWORD FrameArray[FRAME_COUNT+1]={0};
DWORD CurrentFrame=0;
DWORD FrameTime=0;
DWORD LastSleep=0;
BOOL bFrameArrayFilled=0;
float FPS=0.0f;
BOOL gMouseLeftClickInProgress = FALSE;
BOOL gMouseRightClickInProgress = FALSE;
bool was_background = true;
int __cdecl InitKeys(int a1);
FUNCTION_AT_ADDRESS(int __cdecl InitKeys(int a1),0x55B7BC);
signed int FlushMouse();
void StoreDisplay();
FUNCTION_AT_ADDRESS(signed int FlushMouse(),0x55B5B9);

PMOUSECLICK EQADDR_MOUSECLICK=(PMOUSECLICK)0x798614;

#define FPS_ABSOLUTE  0
#define FPS_CALCULATE 1

DWORD MaxFPSMode=FPS_CALCULATE;

void LoadIniSettings()
{
	char szResult[255];
	char szDefault[255];
	sprintf(szDefault, "%d",60);
	DWORD error = GetPrivateProfileStringA("Options",  "MaxFPS", szDefault, szResult, 255, "./eqclient.ini");
	if (GetLastError())
	{
		WritePrivateProfileStringA("Options", "MaxFPS", szDefault, "./eqclient.ini");
	}
	gFG_MAX = atoi(szResult);
	sprintf(szDefault, "%d", 30);
	error = GetPrivateProfileString("Options",  "MaxBGFPS", szDefault, szResult, 255, "./eqclient.ini");
	if (GetLastError())
	{	
		WritePrivateProfileStringA("Options", "MaxBGFPS", szDefault, "./eqclient.ini");
	}
	gBG_MAX = atoi(szResult);
}

BOOL IsMouseWaitingForButton()
{
	return ((EQADDR_MOUSECLICK->RightClick == EQADDR_MOUSECLICK->ConfirmRightClick) 
		&& (EQADDR_MOUSECLICK->LeftClick == EQADDR_MOUSECLICK->ConfirmLeftClick)) ? FALSE : TRUE;
}

BOOL IsMouseWaiting()
{
	BOOL Result = FALSE;

	if (IsMouseWaitingForButton())
		Result = TRUE;
	else {
		if (gMouseLeftClickInProgress) {
			if (!((!EQADDR_MOUSECLICK->LeftClick) && (EQADDR_MOUSECLICK->ConfirmLeftClick == 0x80))) EQADDR_MOUSECLICK->LeftClick = 0x0;
			gMouseLeftClickInProgress = FALSE;
			Result = TRUE;
		}
		if (gMouseRightClickInProgress) {
			if (!((!EQADDR_MOUSECLICK->RightClick) && (EQADDR_MOUSECLICK->ConfirmRightClick == 0x80))) EQADDR_MOUSECLICK->RightClick = 0x0;
			gMouseRightClickInProgress = FALSE;
			Result = TRUE;
		}
	}
	return Result;
}

VOID ProcessFrame()
{
	// Update frame array
	DWORD Now=FrameArray[CurrentFrame]=GetTickCount();

	DWORD FirstFrame=0;
	DWORD Frames=CurrentFrame;
	if (bFrameArrayFilled)
	{
		FirstFrame=CurrentFrame+1;
		if (FirstFrame>FRAME_COUNT)
		{
			FirstFrame=FRAME_COUNT;
		}
		Frames=FRAME_COUNT;
	}
	// Calculate time this frame
	DWORD LastFrame=CurrentFrame-1;
	if (LastFrame>FRAME_COUNT)
	{
		if (bFrameArrayFilled)
		{
			LastFrame=FRAME_COUNT;
			FrameTime=Now-FrameArray[LastFrame];
		}
		else
			FrameTime=0;
	}
	else
		FrameTime=Now-FrameArray[LastFrame];

	// Calculate FPS
	// Get amount of time between first frame and now
	DWORD Elapsed=Now-FrameArray[FirstFrame];


	if (Elapsed)
	{
		// less than one second?
		if (Elapsed<1000)
		{
			// elapsed 150 ms
			// extrapolate. how many frame arrays would fit in one second?
			FPS=(float)(1000.0f/(float)Elapsed); 
			// 6.66667=1000/150
			// now multiply by the number of frames we've gone through
			// Frames 10
			FPS*=(float)Frames;
			// 66.6667= FPS * 10
	//		FPS=
		}
		else
		{
			// Frames = 100
			// Elapsed = 2000ms
			// FPS = 100 / (2000/1000) = 50

			// interpolate. how many seconds did it take for our frame array?
			FPS=(float)Frames/(float)((float)Elapsed/1000.0f); // Frames / number of seconds
		}
	}
	else
		FPS=999.0f;
	// advance frame count
	if (++CurrentFrame>FRAME_COUNT)
	{
		CurrentFrame=0;
		bFrameArrayFilled=1;
	}
}

void Pulse()
{
	ProcessFrame();
	if (IsMouseWaiting())
		return;
	
	if (GetForegroundWindow()==EQhWnd) {
		CurMax=gFG_MAX;
		if (gFG_MAX) {
			int SleepTime=(int)(1000.0f/(float)gFG_MAX);
			if (MaxFPSMode==FPS_CALCULATE) {
				// assume last frame time is constant, so a 30ms frame = 33 fps
				// 
				SleepTime-=(FrameTime-LastSleep);
				/**/
				if (SleepTime<1)
					SleepTime=0;
				else if (SleepTime>100)
					SleepTime=100;
				if (SleepTime > 0)
					Sleep(SleepTime);
				LastSleep=SleepTime;
			} else {
				Sleep(SleepTime);
				LastSleep=SleepTime;
			}
		} else {
			Sleep(1);
		}
	} else {
		CurMax=gBG_MAX;
		if (gBG_MAX) {
			int SleepTime=(int)(1000.0f/(float)gBG_MAX);
			if (MaxFPSMode==FPS_CALCULATE) {
				SleepTime-=(FrameTime-LastSleep);
				/**/
				if (SleepTime<1)
					SleepTime=1;
				else if (SleepTime>100)
					SleepTime=100;
				Sleep(SleepTime);
				LastSleep=SleepTime;
			} else {
				Sleep(SleepTime);
				LastSleep=SleepTime;
			}
		} else {
			Sleep(1);
		}
	}
}
