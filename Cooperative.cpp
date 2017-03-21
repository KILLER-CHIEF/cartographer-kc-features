#include "H2CodezMain.h"



#pragma region Functions
int GetPlayerDyanamic(int Index)
{
	int dyanamic=0;
	int playerdatum=halo.get_unit_datum_from_player_index(Index);
	if (playerdatum != -1)
    dyanamic = *(DWORD *)(*(DWORD *)(0x3003CEF0 + 68) + 12 * (unsigned __int16)playerdatum + 8);
	return dyanamic;
}

int GetPlayersAlive()
{
	int pcount=0;
	for(int i=0;i<16;i++)
	{
		if (GetPlayerDyanamic(i) != 0)
		{
			if (*(BYTE*)(GetPlayerDyanamic(i) + 0x3F4) == 0x1)
				pcount++;
		}		

	}
	return pcount;
}

bool GetPlayersAlive(int index)
{
	bool pcount=false;
	if (GetPlayerDyanamic(index) != 0)
		if(*(BYTE*)(GetPlayerDyanamic(index)+0x3F4)==0x1)
			pcount=true;

	
	return pcount;
}



void _cdecl call_SpawnPlayer(int PlayerIndex)
{
	typedef bool(__cdecl *spawn_player)(int);
	extern spawn_player pspawn_player;
	pspawn_player(PlayerIndex);
}

char call_RestartLevel()
{
	typedef char(__cdecl * Rest)();
	Rest pRest = (Rest)(((char*)game.GetBase()) + 0x23E6BC);
	return pRest();
}

//An Etc. Function (Nothing Related with Coop)
void SpawnAICharacters()
{
	//Run this Only on The Great Journey
	char* sObject = new char[0x50];
	DWORD dwBack;
	VirtualProtect(sObject, 0xC4, PAGE_EXECUTE_READWRITE, &dwBack);

	game.call_CharacterInitialize(sObject);
	// int choice = rand()%5;

	*(float*)(sObject + 0x4) = (*(float*)(((char*)game.GetBase()) + 0x4A8504) + 1.0f); //x
	*(float*)(sObject + 0x8) = (*(float*)(((char*)game.GetBase()) + 0x4A8508) + 1.0f);//y
	*(float*)(sObject + 0xC) = (*(float*)(((char*)game.GetBase()) + 0x4A850C) + 1.0f);//z

																					  //game.CreateCharacter(0xEE130C9D, 0xFF, mya3);
	game.call_CreateCharacter(0xFAA61920, 0x18, sObject);
	game.call_CreateCharacter(0xFAA91923, 0x0, sObject);
	game.call_CreateCharacter(0xFAA81922, 0x0, sObject);

	//game.call_CreateCharacter(0xFAB5192F, 0x15, sObject);
	//delete[0x50]sObject;
}

#pragma endregion