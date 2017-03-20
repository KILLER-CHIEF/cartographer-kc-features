#include "H2CodezMain.h"

class Cooperative
{
public :
    DWORD Difficulty;    //DifficultyLevel
    DWORD GameType;      //GameTypeIndex
    DWORD GameTypeUI;    //GameTypeUIIndex
	DWORD CoopSpawns;    //CoopSpawn
	DWORD GameEngine;    //GameEngine(SP/MP/MM)
	DWORD PauseGame;     //Pause Game <-To fix the pause of game When Host Presses Escape
	DWORD StausCheck;    //Check User Status(Ingame/Lobby etc..)
	BYTE CoopBiped;      //To Decide Ingame Biped for Coop(MC/Dervish)
	BYTE CoopTeam;       //To Decide Ingame Team for Coop(Humans/Elites)
	DWORD PlayerCount;   //Check Players Ingame



    BOOL MPGame;        //IsGameMP
    BOOL CoopGame;      //IsGameCoop
    BOOL LoadPCR;       //LoadPCR
	BOOL Client_f;      //To Check if Client Fixes are done
	BOOL Host_f;        //To Check if Host Fixes are done
	BOOL Host_L;        //Starts Stops Host Loop
	BOOL Begin;
    
};

extern Cooperative Coop;

#pragma region Declarations
int GetPlayerDyanamic(int Index);
int GetPlayersAlive();
bool GetPlayersAlive(int index);
bool IsHost();
void _cdecl call_SpawnPlayer(int PlayerIndex);
char call_RestartLevel();
void SpawnAICharacters();
void HostFixes();
void HostHooks();
void ClientFixes();
void ClientHooks();
void TempUnHook();
void MapThings();
void SpawnPlayersCoop(); 



typedef int(__cdecl *GameSet)(void*);
GameSet pGameSet;

typedef char(__thiscall *Crashy)(void *, int , int , int , int );
Crashy pCrashy001;///Function at which Game usually Crashes <-someone plz Fix this

typedef int(__cdecl * Fp)(int, int, int);
Fp pFp;



#pragma endregion


bool IsHost()
{
	int LB=(int)*(int*)((char*)game.GetBase()+0x420FE8);	
	int HID=*(int*)(LB+0x125C);
	int LID=*(int*)((char*)game.GetBase()+0x51A629);
	
	if(*(BYTE*)Coop.StausCheck>0)
	{
		if(LID==HID)
		return true;
		else
		return false;
	}
	else
		return false;
}

int __cdecl h_InitializeGameEngineSystems(void* vpointer)
{
    pLog.WriteLog("Hooks:: EngineMode = (%i)",*(int*)vpointer);
	
    if(Coop.CoopGame==TRUE)
    {
		MapThings();
		if(IsHost())
		{
		*(int*)vpointer=1;
		pLog.WriteLog("Coop:: Starting Cooperative Gameplay.");
		pLog.WriteLog("Hooks:: NewEngineMode = (%i)",*(int*)vpointer);
		Coop.Host_f=TRUE;
		Coop.Begin = FALSE;
		
		}
		else
		{
			pLog.WriteLog("Coop:: Allright Client Get Ready for Coop.");
			Coop.Client_f=TRUE;
			*(int*)vpointer = 1;
		}
		
		Coop.CoopGame=FALSE;	
		return pGameSet(vpointer);
    }
	else
	{
		Coop.Host_L=FALSE;
		Coop.Client_f=FALSE;
		Coop.Host_f=FALSE;
		Coop.Begin = FALSE;
	    TempUnHook();
        return pGameSet(vpointer);

	
	}
    
	
}

char __fastcall h_Crashy001(void *This,void* rubbish, int a2, int a3, int a4, int a5)
{

     
     int myv5 =*(DWORD *)(a2 + 8);
     int mycrashy= (*(DWORD *)(*(DWORD *)(0x3003CEF0 + 68) + 12 * (unsigned __int16)myv5 + 8) + 0xAA);
    
     //if(Coop.CoopGame==TRUE)
     //*(BYTE*)mycrashy=1;

     return pCrashy001(This,a2,a3,a4,a5);

}

int __cdecl h_FpWeaponsSet(int PlayerIndex, int UnitDatum, int PlayerModel)
{
	PlayerModel = Coop.CoopBiped;
	HostFixes();
	return pFp(PlayerIndex, UnitDatum, PlayerModel);
}



void _cdecl h_SpawnPlayer(int PlayerIndex);
void _cdecl h_SpawnPlayer(int PlayerIndex)
{	
	if(Coop.Client_f==TRUE)
	{
		pLog.WriteLog("Coop:: Running Client Fixes.");
		Coop.Client_f==FALSE;
		CreateThread(0,0,( LPTHREAD_START_ROUTINE)ClientHooks,0,0,0);
	    ClientFixes();   
		
	}
	else if(Coop.Host_f==TRUE)
	{
		
		pLog.WriteLog("Coop:: Running Host Fixes.");	
		Coop.Host_L=TRUE;		
		Coop.Host_f=FALSE;
	}
	//return pSpawn(PlayerIndex);
}


signed int Test()
{
	typedef signed int(__cdecl * t)();
	t pt = (t)(((char*)game.GetBase()) + 0x51299);
	return pt();
}
//Assign Halo 2 Pointers
void CoopAssignPointers()
{
	int GameGlobals=(int)*(int*)((char*)game.GetBase()+0x482D3C);
	int GameTimeGlobals=(int)*(int*)((char*)game.GetBase()+0x4C06E4);
	

    pGameSet =(GameSet)(((char*)game.GetBase())+ 0x4A46E);
   // pCrashy001 = (Crashy)(((char*)game.GetBase())+ 0x1F2337);
	pFp = (Fp)(((char*)game.GetBase()) + 0x2295BE);
	//pSpawn = (Spawn)(((char*)game.GetBase()) + 0x55952);
	Coop.Difficulty=GameGlobals+0x2A2;
	Coop.GameType=GameGlobals+0x2EC;
	Coop.CoopSpawns=GameGlobals+0x2A4;
	Coop.GameEngine=GameGlobals+0x8;
	Coop.PauseGame=GameTimeGlobals+1;
	Coop.StausCheck=(int)((char*)game.GetBase()+0x420FC4);
	Coop.PlayerCount= 0x30004B60;
	Coop.CoopTeam = 0;
	Coop.CoopBiped = 0;

}
//MapChecks
void MapThings()
{
	//0x47cf0c
	char* map_name = (char*)(((char*)game.GetBase()) + 0x47cf0c);
	if ((!strcmp(map_name, "01a_tutorial")) || (!strcmp(map_name, "01b_spacestation")) || (!strcmp(map_name, "03a_oldmombasa")) || (!strcmp(map_name, "03b_newmombasa")) || (!strcmp(map_name, "05a_deltaapproach")) || (!strcmp(map_name, "05b_deltatowers")) || (!strcmp(map_name, "07a_highcharity")) || (!strcmp(map_name, "07b_forerunnership")))
	{
		Coop.CoopBiped = 0;
		Coop.CoopTeam = 2;
	}
	else if ((!strcmp(map_name, "04a_gasgiant")) || (!strcmp(map_name, "04b_floodlab")) || (!strcmp(map_name, "06a_sentinelwalls")) || (!strcmp(map_name, "06b_floodzone")) || (!strcmp(map_name, "08a_deltacliffs")) || (!strcmp(map_name, "08b_deltacontrol")))
	{
		Coop.CoopBiped = 1;
		Coop.CoopTeam = 3;
	}
	pLog.WriteLog("Coop:: Current Coop Map : %s", map_name);

}

//Start Cooperative Hooks (For Both Host/Client)
void CooperativeHook()
{

    CoopAssignPointers();
    DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());


    DetourAttach(&(PVOID&)pGameSet,h_InitializeGameEngineSystems);
	//DetourAttach(&(PVOID&)pSpawn, h_SpawnPlayer);
	//DWORD dwBack;

	//pSpawn = (pSpawn); DetourFunc((BYTE*)game.GetBase() + 0x55952, (BYTE*)h_SpawnPlayer, 6);
	//VirtualProtect(pSpawn, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	//DetourAttach(&(PVOID&)pSpawn, h3_SpawnPlayer);

	DetourTransactionCommit();

	pLog.WriteLog("Hooks:: Cooperative Hooks Started.");
	pLog.WriteLog("Press F5 To Start Coop..");
	pLog.WriteLog("Coop:: Coop Status : %i", Coop.CoopGame);
	
	return;
}
//Hooks meant only for Clients
void ClientHooks()
{
	
    DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
   
    DetourAttach(&(PVOID&)pFp, h_FpWeaponsSet);
    
	DetourTransactionCommit();	
	return;
}

//Remove All Cooperative Hooks
void CooperativeUnHook()
{
    DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

    DetourDetach(&(PVOID&)pGameSet,h_InitializeGameEngineSystems);
	DetourDetach(&(PVOID&)pFp, h_FpWeaponsSet);
	//DetourDetach(&(PVOID&)pSpawn, h_SpawnPlayer);

	DetourTransactionCommit();	
	pLog.WriteLog("Removed All Cooperative Hooks.");
	return;
}
//Remove Temporary Coopearative Hooks(To stop Glitches/Bugs)
void TempUnHook()
{
    DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());  
	//DetourDetach(&(PVOID&)pSpawn, h_SpawnPlayer);
	DetourDetach(&(PVOID&)pFp, h_FpWeaponsSet);	
	DetourTransactionCommit();	
	pLog.WriteLog("Removed Temporary Coop Hooks.");
	return;
}

void ClientFixes()
{
	
 *(BYTE*)Coop.GameEngine=1; //Engine Hack
 *(BYTE*)Coop.CoopSpawns = 1;//Lets Enable Coop Play
 

 //Lets Fix SomeThings
 for (int i = 0;i <*(BYTE*)Coop.PlayerCount;i++)
 {
	 *(int*)(0x30002BA0 + (i * 0x204))=Coop.CoopBiped;
	 *(int*)(0x30002BD8 + (i * 0x204))=Coop.CoopTeam;
 }
#pragma region Call_Functions
 void(*AiActivate)();
 AiActivate =(void (*)(void))((char*)game.GetBase()+0x30E67C);
 AiActivate();

 signed int(*ObjScripting)();
 ObjScripting =(signed int (*)(void))((char*)game.GetBase()+0x14B083);
 ObjScripting();

  void(*ObjEarlyMovers)();
 ObjEarlyMovers =(void (*)(void))((char*)game.GetBase()+0x14B9D8);
 ObjEarlyMovers();

 int(*GameTime)();
 GameTime = (int(*)(void))((char*)game.GetBase() + 0x7C13D);
 GameTime();

 
 /*
  
  char(*PlayerGlobals)();
 PlayerGlobals = (char(*)(void))((char*)game.GetBase() + 0x52E03);
 PlayerGlobals();
  
  /*
  signed int(*GameEngineGlobals)();
  GameEngineGlobals =(signed int (*)(void))((char*)game.GetBase()+0x753B4);
  GameEngineGlobals();

 void(*Rasterizer)();
 Rasterizer =(void (*)(void))((char*)game.GetBase()+0x272986);
 Rasterizer();

  char(*GameSounds)();
  GameSounds =(char (*)(void))((char*)game.GetBase()+0x85801);
  GameSounds();

  char(*PlayerEffects)();
  PlayerEffects =(char (*)(void))((char*)game.GetBase()+0xA3E39);
  PlayerEffects();
 

  void(*UserScreen)();
 UserScreen =(void (*)(void))((char*)game.GetBase()+0x227862);
 UserScreen();

 char(*HsSystem)();
 HsSystem = (char(*)(void))((char*)game.GetBase() + 0x96BFD);
 HsSystem();


  */

 char(*CinematicGlobals)();
 CinematicGlobals = (char(*)(void))((char*)game.GetBase() + 0x3A5F3);
 CinematicGlobals();
 
 void(*UserScreen)();
 UserScreen = (void(*)(void))((char*)game.GetBase() + 0x227862);
 UserScreen();

  int(*SaveState)();
  SaveState =(int (*)(void))((char*)game.GetBase()+0x9E455);
  SaveState();



 
  


#pragma region Notes
  //Functions in Sequence

 //GameTime
 //PlayerGlobals  
 //GameEngineGlobals
 //Rasterizer
 //GameSounds
 //PlayerEffects     //Causes Black Screen On SP Mode while Fade from black in MP
 //UserScreen
 //CinematicGlobals //Brings BlackLetterbox,Removes Sound..etc.
 //ObjPlacement
 //SaveState
#pragma endregion
 
#pragma region OldCrashyWay
 //I Stopped using this becuz it caused soo many non usefull functions to load like Game Shell BG
  //I better call them manually

  /*
  int off=0;
 void (*pInitzFunction)(void);

 //int *z=(int*)((char*)game.GetBase()+0x3A0470);
 int *dword_710470 =(int*)((char*)game.GetBase()+0x3A0470);
 //Now Calling all the GameInitialization Functions
 do
  {
    pInitzFunction = (void (*)(void))dword_710470[off];
    if ( pInitzFunction )
      pInitzFunction();
    off += 9;
  }
  while ( off < 0x276 );*/
#pragma endregion

#pragma endregion
}

void HostFixes()
{
	//Lets Fix SomeThings
	for (int i = 0;i <*(BYTE*)Coop.PlayerCount;i++)
	{
		*(int*)(0x30002BA0 + (i * 0x204)) = Coop.CoopBiped;
		*(int*)(0x30002BD8 + (i * 0x204)) = Coop.CoopTeam;
	}
}





void InitializeCoop()
{

	Coop.CoopGame = !Coop.CoopGame;
	pLog.WriteLog("Coop:: Coop Status : %i", Coop.CoopGame);
	if(Coop.CoopGame)
	if (!IsHost())
	{
		pLog.WriteLog("Coop:: Join a Cooperative Game");
	}
	else
		pLog.WriteLog("Coop:: You are Hosting Coop");
}