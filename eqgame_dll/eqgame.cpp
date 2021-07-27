#include "types.h"
#include "global_define.h"
#include <map>
#include "..\Detours\inc\detours.h"
#include "patches/trilogy.h"
#include "eq_stream_ident.h"
#include "eqmac.h"
#include "eqmac_functions.h"
#include "eqemu_logsys.h"
#include <sstream>
#include <iomanip>
extern std::string get_opcode_name(unsigned long opcode);
extern uint16_t get_opcode_id(std::string name);
extern void load_opcode_names();
//#include "Common.h"
extern void Pulse();
extern bool was_background;
extern void LoadIniSettings();
EQStreamIdentifier ident;
extern void SetEQhWnd();
extern HMODULE heqwMod;
extern HWND EQhWnd;
HANDLE myproc = 0;
bool title_set = false;
bool first_maximize = true;
bool can_fullscreen = false;
EQEmuLogSys Log;
bool ResolutionStored = false;
DWORD resx = 0;
DWORD resy = 0;
DWORD bpp = 0;
DWORD refresh = 0;
HMODULE eqmain_dll = 0;
BOOL bExeChecksumrequested = 0;
BOOL g_mouseWheelZoomIsEnabled = true;
unsigned int g_buffWindowTimersFontSize = 3;
bool has_focus = true;
WINDOWINFO stored_window_info;
WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
bool start_fullscreen = false;
bool in_full_screen = false;
bool startup = true;
POINT posPoint;
DWORD o_MouseEvents = 0x0055B3B9;
DWORD o_MouseCenter = 0x0055B722;
DWORD* conn_ptr;
extern std::map<unsigned long, std::string> opcode_map;
template< typename T >
std::string int_to_hex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}

typedef signed int(__cdecl* ProcessGameEvents_t)();
ProcessGameEvents_t return_ProcessGameEvents;
ProcessGameEvents_t return_ProcessMouseEvent;
ProcessGameEvents_t return_SetMouseCenter;

DWORD d3ddev = 0;
DWORD eqgfxMod = 0;
BOOL bWindowedMode = true;

typedef int(__cdecl* SendExeChecksum_t)(void);
SendExeChecksum_t SendExeChecksum_Trampoline;
typedef struct _detourinfo
{
	DWORD_PTR tramp;
	DWORD_PTR detour;
}detourinfo;
std::map<DWORD,_detourinfo> ourdetours;


#define FUNCTION_AT_ADDRESS(function,offset) __declspec(naked) function\
{\
	__asm{mov eax, offset};\
	__asm{jmp eax};\
}

#define EzDetour(offset,detour,trampoline) AddDetourf((DWORD)offset,detour,trampoline)

void PatchA(LPVOID address, const void *dwValue, SIZE_T dwBytes) {
	unsigned long oldProtect;
	VirtualProtect((void *)address, dwBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
	FlushInstructionCache(GetCurrentProcess(), (void *)address, dwBytes);
	memcpy((void *)address, dwValue, dwBytes);
	VirtualProtect((void *)address, dwBytes, oldProtect, &oldProtect);
}

void UpdateTitle()
{
	HWND cur = NULL;
	char str[255];
	int i = 0;
	do {
		i++;
		sprintf(str, "Client%d", i);
		cur = FindWindowA(NULL, str);
	} while (cur != NULL);
	SetWindowTextA(EQhWnd, str);
}

void AddDetourf(DWORD address, ...)
{
	va_list marker;
	int i=0;
	va_start(marker, address);
	DWORD Parameters[3];
	DWORD nParameters=0;
	while (i!=-1) 
	{
		if (nParameters<3)
		{
			Parameters[nParameters]=i;
			nParameters++;
		}
		i = va_arg(marker,int);
	}
	va_end(marker);
	if (nParameters==3)
	{
		detourinfo detinf = {0};
		detinf.detour = Parameters[1];
		detinf.tramp = Parameters[2];
		ourdetours[address] = detinf;
		DetourFunctionWithEmptyTrampoline((PBYTE)detinf.tramp,(PBYTE)address,(PBYTE)detinf.detour);
	}
}

typedef char**(__cdecl* Arrival_Data_t)(DWORD* Buffer);
Arrival_Data_t Arrival_Data_Trampoline;
char** __cdecl Arrival_Data_Detour(DWORD* Buffer)
{

	char** res = Arrival_Data_Trampoline(Buffer);

	uint16 opcode1 = (uint16)res[0];

	//get eqemu style opcode
	uint16 emu_opcode = ntohs(opcode1);

	Log.Out(Logs::General, Logs::General, "Incoming RAW opcode: %s : %s", int_to_hex<uint16>((uint16)res[0]).c_str(), get_opcode_name(emu_opcode).c_str());

	return res;
}

typedef DWORD(__stdcall* CEverQuest__HandleWorldMessage_t)(DWORD *, unsigned __int32, char *, unsigned __int32);
CEverQuest__HandleWorldMessage_t CEverQuest__HandleWorldMessage_Trampoline;
class Eqmachooks {
public:
};

DWORD __stdcall CEverQuest__HandleWorldMessage_Detour(DWORD *con, unsigned __int32 Opcode, char *Buffer, unsigned __int32 len)
{
	//get eqemu style opcode
	uint16 emu_opcode = ntohs(Opcode);
	conn_ptr = (DWORD*)con;
	Log.Out(Logs::General, Logs::General, "Incoming opcode: %s Size %i", int_to_hex<uint16_t>(emu_opcode).c_str(), len);
	EQStreamIdentifier::OldPatch* p;
	//should only be 1 for testing...
	if (!ident.m_oldpatches.empty())
	{


		//first, get name of opcode;
		std::string lookupName = get_opcode_name(emu_opcode);
		Log.Out(Logs::General, Logs::General, "Converted incoming eq opcode to EmuName: %s", lookupName.c_str());

		//TODO: Do translation
		EQApplicationPacket* inapp = new EQApplicationPacket(ident.m_oldpatches[0]->opcodes->NameSearch(lookupName.c_str()), (unsigned char*)Buffer, len);
		EQApplicationPacket* outapp = ident.m_oldpatches[0]->structs->Decode(inapp);
		
		//get eq opcode from emu name
		uint16_t id = ident.m_oldpatches[0]->opcodes->EmuToEQ(ident.m_oldpatches[0]->opcodes->NameSearch(lookupName.c_str()));

		//ntohs - eq style opcode
		id = htons(id);

		if (outapp)
		{
			char* buffer = new char[outapp->size];
			memcpy((void*)buffer, outapp->pBuffer, outapp->size);
			len = outapp->size;

			Log.Out(Logs::General, Logs::General, "Final incoming eq opcode: %s", int_to_hex<uint16_t>(id).c_str());


			DWORD result;
			if (id != 0x0000)
			{
				//Finally, send the proper packet this way.
				result = CEverQuest__HandleWorldMessage_Trampoline(con, id, buffer, len);

				safe_delete_array(buffer);
				return result;
			}
		}
		else
		{
			//no outapp
			Log.Out(Logs::General, Logs::General, "NO OUTAPP FOR OPCODE: %s", int_to_hex<uint16_t>(emu_opcode).c_str());
			return 1;
		}
	}

	return CEverQuest__HandleWorldMessage_Trampoline(con, Opcode, Buffer, len);
}

typedef DWORD(__cdecl* SendMessage_t)(DWORD *, unsigned __int32, char *, unsigned __int32, int);
SendMessage_t SendMessage_Trampoline;
DWORD __cdecl SendMessage_Detour(DWORD *con, unsigned __int32 Opcode, char *Buffer, unsigned __int32 len, int unk)
{
	//get eqemu style opcode
	uint16 emu_opcode = ntohs(Opcode);

	Log.Out(Logs::General, Logs::General, "Outgoing opcode: %s Size %i", int_to_hex<uint16_t>(emu_opcode).c_str(), len);
	EQStreamIdentifier::OldPatch* p;
	//should only be 1 for testing...
	if (!ident.m_oldpatches.empty())
	{


		//first, get name of opcode
		std::string lookupName = ident.m_oldpatches[0]->opcodes->EQToName(emu_opcode);
		Log.Out(Logs::General, Logs::General, "Converted emu opcode to EmuName: %s", lookupName.c_str());
		//see what maps to that opcode by id
		uint16 id = get_opcode_id(lookupName);
		Log.Out(Logs::General, Logs::General, "Converted emu opcode to eq opcode: %s", int_to_hex<uint16_t>(id).c_str());

		//TODO: Do translation

		EQApplicationPacket* inapp = new EQApplicationPacket(ident.m_oldpatches[0]->opcodes->NameSearch(lookupName.c_str()), (unsigned char*)Buffer, len);
		EQApplicationPacket* outapp = ident.m_oldpatches[0]->structs->Encode(inapp);

		if (outapp)
		{


			//ntohs - eq style opcode
			id = htons(id);

			char* buffer = new char[outapp->size];
			memcpy((void*)buffer, outapp->pBuffer, outapp->size);
			len = outapp->size;

			Log.Out(Logs::General, Logs::General, "Final eq opcode: %s Len: %i", int_to_hex<uint16_t>(id).c_str(), len);

			DWORD result;
			if (id != 0x0000)
			{
				result = SendMessage_Trampoline(con, id, buffer, len, unk);
				safe_delete_array(buffer);
				if (outapp == inapp)
				{
					safe_delete(outapp);
				}
				else
				{
					safe_delete(outapp);
					safe_delete(inapp);
				}
				return result;
			}
		}
		if (outapp == inapp)
		{
			safe_delete(outapp);
		}
		else
		{
			safe_delete(outapp);
			safe_delete(inapp);
		}
	}

	return SendMessage_Trampoline(con, Opcode, Buffer, len, unk);
}


typedef DWORD(__cdecl* SendMessage_t)(DWORD *, unsigned __int32, char *, unsigned __int32, int);
typedef LPSTR (WINAPI* GetModuleFileNameA_t)();
GetModuleFileNameA_t GetModuleFileNameA_tramp;
int __cdecl SendExeChecksum_Detour()

{

	bExeChecksumrequested = 1;

	return SendExeChecksum_Trampoline();

}

LPSTR WINAPI GetModuleFileNameA_detour()
{
	if (bExeChecksumrequested) {
		return "eqmac.exe patchme";
	}
	return GetModuleFileNameA_tramp();
}
DWORD gmfadress = 0;
DWORD wpsaddress = 0;
DWORD swAddress = 0;
DWORD cwAddress = 0;
DWORD swlAddress = 0;
DWORD uwAddress = 0;

PVOID pHandler;
bool bInitalized=false;
void PatchSaveBypass()
{
}

void InitHooks()
{
	HMODULE hkernel32Mod = GetModuleHandle("kernel32.dll");
	gmfadress = (DWORD)GetProcAddress(hkernel32Mod, "GetCommandLineA");
	EzDetour(gmfadress, GetModuleFileNameA_detour, GetModuleFileNameA_tramp);
	CEverQuest__HandleWorldMessage_Trampoline = (CEverQuest__HandleWorldMessage_t)DetourFunction((PBYTE)0x00496743, (PBYTE)CEverQuest__HandleWorldMessage_Detour);
	SendMessage_Trampoline = (SendMessage_t)DetourFunction((PBYTE)0x004E1C45, (PBYTE)SendMessage_Detour);
	Arrival_Data_Trampoline = (Arrival_Data_t)DetourFunction((PBYTE)0x004F4C90, (PBYTE)Arrival_Data_Detour);
	SendExeChecksum_Trampoline = (SendExeChecksum_t)DetourFunction((PBYTE)0x004A4116, (PBYTE)SendExeChecksum_Detour);
	GetModuleFileNameA_tramp = (GetModuleFileNameA_t)DetourFunction((PBYTE)gmfadress, (PBYTE)GetModuleFileNameA_detour);

	bInitalized=true;
}

void RemoveDetour(DWORD address)
{
	for(std::map<DWORD,_detourinfo>::iterator i = ourdetours.begin();i!=ourdetours.end();i++) {
		DetourRemove((PBYTE)i->second.tramp,(PBYTE)i->second.detour);
	}
}

void ExitHooks()
{
	if(!bInitalized)
	{
		return;
	}
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	if (ul_reason_for_call==DLL_PROCESS_ATTACH)
	{

		Log.LoadLogSettingsDefaults();
		Log.StartFileLogs("EQhack_");
		//MessageBox(NULL,"loading","",MB_OK);
		InitHooks();
		LoadIniSettings();
		Trilogy::Register(ident);
		load_opcode_names();
		//if (!heqwMod)
		//	SetForegroundWindow(EQhWnd);
		//sprintf_s(INIFileName,"%s\\%s.ini",gszINIPath,pluginname);
		return TRUE;
	}
	else if (ul_reason_for_call==DLL_PROCESS_DETACH) {
		//MessageBox(NULL,"unloading","",MB_OK);
		ExitHooks();
	    return TRUE;
	}
}