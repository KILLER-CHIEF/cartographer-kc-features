// stdafx.cpp : source file that includes just the standard includes
// H2Codez.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file



// dllmain.cpp : Defines the entry point for the DLL application.
#include "H2CodezMain.h"
#include "H2hooks.h"
#include "Cooperative.h"


DWORD g_threadID;


H2class halo;
Debug Dbg;
Game game;

Logs pLog = Logs("H2Codez.log");
Cooperative Coop;


BOOL EnableDbgConsole;





//Enable Hooking System
void APPLYHOOKS()
{
	pLog.WriteLog("Hooks:: Initializing Hooking System....");
	//CreateThread(0,0,( LPTHREAD_START_ROUTINE)H2Hooks,0,0,0);
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



//DWORD WINAPI H2CodezInitialize(LPVOID)
void H2CodezInitialize()
{

	EnableDbgConsole = TRUE;
	halo.Start();
	APPLYHOOKS();


#pragma region InfiniteLoop	
	//Do here Like Checks,Hotkeys,etc..
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
			/*
			pLog.WriteLog("Coop:: All Players Dead..Spawning Them.");
			Sleep(1500);
			for (int i = 1;i < *(BYTE*)Coop.PlayerCount;i++)
			{
			call_SpawnPlayer(i);
			}*/
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
			}



			if (Coop.Begin)
			{
				HostFixes();
				int IsGameLost = 0x30004B64;
				*(BYTE*)Coop.PauseGame = 0;
				if (*(BYTE*)IsGameLost == 1)
				{

					if (GetPlayersAlive() == 0)
					{
						pLog.WriteLog("Coop:: All Players Dead..Spawning Them.");
						Sleep(1500);
						for (int i = 0; i < *(BYTE*)Coop.PlayerCount; i++)
						{
							call_SpawnPlayer(i);
						}



					}


				}
			}



		}




	}
#pragma endregion 
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
