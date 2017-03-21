// stdafx.cpp : source file that includes just the standard includes
// H2Codez.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file



// dllmain.cpp : Defines the entry point for the DLL application.
#include "H2CodezMain.h"
#include "Cooperative.h"
#include "H2MOD.h"


DWORD g_threadID;


H2class halo;
Debug Dbg;
Game game;

Logs pLog = Logs("H2Codez.log");
Cooperative Coop;



//Enable Hooking System
void APPLYHOOKS()
{
	pLog.WriteLog("Hooks:: Initializing Hooking System....");	
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CooperativeHook, 0, 0, 0);
	pLog.WriteLog("Hooks:: InProgress");

}
//Remove Hooking System
void DisEngageHOOKS()
{

	DetourTransactionBegin();
	CooperativeUnHook();
	DetourTransactionCommit();
	pLog.WriteLog("\n\n***H2 Codez ShutDown****");
	pLog.Exit();

	ExitProcess(true);
}


void H2CodezLoop() {
#pragma region HotKeys

	unsigned int datum;
	while (true)
	{
		if (GetAsyncKeyState(VK_F5) & 1)
		{

			InitializeCoop();




		}
		if (GetAsyncKeyState(VK_F10) & 1)
		{

			*(BYTE*)Coop.GameEngine = 2;
			char(*PlayerEffects)();
			PlayerEffects = (char(*)(void))((char*)game.GetBase() + 0xA3E39);
			PlayerEffects();
			*(BYTE*)Coop.GameEngine = 1;
		}

		if (GetAsyncKeyState(VK_NEXT) & 1)
		{
			//50A398
			pLog.WriteLog("Party Privacy : InviteOnly ");
			*(int*)((char*)game.GetBase() + 0x50A398) = 2;



		}
		if (GetAsyncKeyState(VK_PRIOR) & 1)
		{
			pLog.WriteLog("Party Privacy : Open ");
			*(int*)((char*)game.GetBase() + 0x50A398) = 0;


		}
		if (GetAsyncKeyState(VK_HOME) & 1)
		{


			//datum= halo.SpawnObjAtCamera(0xEEE30D6D);
			if (*(int*)0x30004D04 != -1)
			{
				datum = *(int*)0x30004D04;
				game.call_AssignObjDatumToPlayer(0, datum);
			}


		}

		if (Coop.Host_L)
		{
			if (*(BYTE*)Coop.PlayerCount>1)
			{
				Coop.Begin = TRUE;
				*(BYTE*)Coop.CoopSpawns = 1;
				pLog.WriteLog("Coop:: Co-op Started. Enjoy With Others");
			}



			if (Coop.Begin)
			{
				Fixes();
				int IsGameLost = 0x30004B64;
				*(BYTE*)Coop.PauseGame = 0;  //Stop Host Game From Pausing
				if (*(BYTE*)IsGameLost == 1)
				{

					if (GetPlayersAlive() == 0)
					{
						pLog.WriteLog("Coop:: All Players Dead..Spawning Them.");
						Sleep(1500);
						SpawnPlayersCoop();
					}
				}


			}



		}




	}
#pragma endregion 
}

class later
{
public:
	template <class callable, class... arguments>
	later(int after, bool async, callable&& f, arguments&&... args)
	{
		std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

		if (async)
		{
			std::thread([after, task]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
				task();
			}).detach();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(after));
			task();
		}
	}

};

void H2CodezInitialize()
{
	
	halo.Start();
	APPLYHOOKS();

#pragma region InfiniteLoop	
	while (true)
	{
		later later_test1(1, false, &H2CodezLoop);
	}
#pragma endregion 
}



/*BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{

	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, NULL, &Initialize, NULL, NULL, &g_threadID);

		break;

	case DLL_PROCESS_DETACH:

		DisEngageHOOKS();
		break;
	}
	return TRUE;
}*/
