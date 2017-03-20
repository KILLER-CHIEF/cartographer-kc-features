#include "H2CodezMain.h"


int *TEST,*b,c;
int *SS,k,*p;
int* myThis;


int *mya3;

#pragma  region H2Hooks

#pragma  region Declarations
//Declarations
typedef int(__cdecl* hgetobj)(signed int , int );
hgetobj pgetobj;


typedef void(__cdecl* objenew)(void*);
objenew pobjnew;

typedef void(_stdcall* unit_create)(int,int,int,int,int);
unit_create punit;

typedef void(__stdcall* vehi_create)(int,int,int,int,int);
vehi_create pvehi;

typedef void(__stdcall* generic_create)(int,int,int,int,int);
generic_create pitem;

typedef void(__stdcall* wep_create)(int,int,int,int,int);
wep_create pwep;

typedef char *(__thiscall *TxtUpdate)(void *, int , int , int , int , int , int , int , int );
TxtUpdate ptxtup;

//char *__cdecl sub_EDFF1D(int a1, int a2, unsigned int a3)
typedef char *(__cdecl *sub_EDFF1D)(int,int,unsigned int);
sub_EDFF1D psub_EDFF1D;

//void __thiscall GetGameOptions(int this, int a2, int *a3)
typedef void(__thiscall* GameOptions)(void*,int,int);
GameOptions pgameoptions;

//int __stdcall call_MPMapSelect(int a1)
typedef int(__stdcall* MapSelect)(int);
MapSelect pMapSelect;

typedef int(__stdcall* MpGame)(int);
MpGame pMpgame;

typedef void(_stdcall* XX)(int);
XX pxx;

//int __cdecl call_SquadSettingsDialog(int a1)
typedef int(__cdecl* SquadSettings)(int);
SquadSettings psquadsettings;

//int __usercall sub_12EAABD@<eax>(double a1@<st0>)
typedef int(_cdecl* PlayerSpeed)(double);
PlayerSpeed pPlayerSpeed;

typedef char(__cdecl *attach)(unsigned __int16, int);
attach pattach;

//int __cdecl CreateCharacter(int CharDatumIndex, signed __int16 a2, int a3)

typedef int(__cdecl *createchar)(int , signed __int16, int* );
createchar pcreatechar;






#pragma  endregion 
#pragma region HookedFunctions
int __cdecl Hook01(signed int object_datum_index, int object_type)
{
	pLog.WriteLog("Hooks:: Get_Object: %X , %d ",object_datum_index,object_type);
	return pgetobj(object_datum_index,object_type);

}



void __cdecl Hook04(void*a1)
{
	pLog.WriteLog("Hooks:: CreateObject : %X , Val : %X ",a1,*(int*)(a1));	
	return pobjnew(a1);
    
    
}


void _stdcall CreateUnit(int a1,int a2,int a3,int a4,int a5)
{
	pLog.WriteLog("Hooks:: Creating Unit(%08X , %08X ,%08X ,%08X ,%08X )",a1,a2,a3,a4,a5);
	return punit(a1,a2,a3,a4,a5);
}

void _stdcall CreateVehi(int a1,int a2,int a3,int a4,int a5)
{
	pLog.WriteLog("Hooks:: Creating Vehicle(%08X , %08X ,%08X ,%08X ,%08X )",a1,a2,a3,a4,a5);
    
	return pvehi(a1,a2,a3,a4,a5);
}

void _stdcall CreateItem(int a1,int a2,int a3,int a4,int a5)
{
	pLog.WriteLog("Hooks:: Creating Item(%08X , %08X ,%08X ,%08X ,%08X )",a1,a2,a3,a4,a5);
	return pitem(a1,a2,a3,a4,a5);
}

void _stdcall CreateWep(int a1,int a2,int a3,int a4,int a5)
{
	pLog.WriteLog("Hooks:: Creating Weapon(%08X , %08X ,%08X ,%08X ,%08X )",a1,a2,a3,a4,a5);
	return pwep(a1,a2,a3,a4,a5);
}

//char *__thiscall h_TxtUpdate(void *p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9)
char *__cdecl h_sub_EDFF1D(int a1, int a2, unsigned int a3)
{
    pLog.WriteLog("Hooks:: sub_EDFF1D(a1: %08X (%s) ,a2: %08X ,a3: %08X)",a1,(char*)a1,a2,a3);
    return psub_EDFF1D(a1,a2,a3);
}

void __fastcall h_GetGameOptions(void* This, void* garbage,  int a2,int a3)
{
 
    pLog.WriteLog("Hooks:: GetGameOptions(%08X,%08X,%08X)",(int*)This,a2,a3);   
    myThis=(int*)This;
    int i =*(int*)a3;
    int j = (*(DWORD *)(*((DWORD *)This + 0x1C) + 0x44) + 4 * (unsigned __int16)i + 2);
    pLog.WriteLog("Hooks:: GetGameOptions(%X(%i)",j,(WORD*)j); 
    return pgameoptions(This,a2,a3);

}



int __stdcall h_MPMapSelect(int a1)
{
   pLog.WriteLog("Hooks:: MPMAPSELECT(%X(%X))",a1,*(int*)a1); 
    TEST=(int*)(a1);
    b=&c;
    c=*((int*)(*TEST));
    TEST=(int*)&b;
   
    return pMapSelect(a1); // pMapSelect(a1);
}


int __cdecl h_SquadSettings(int a1)
{
     pLog.WriteLog("Hooks:: SquadSettings(%X(%X))",a1,*(int*)a1);

     
	 SS = (int*)a1;
     k=*((int*)(*SS));
     p=&k;
     SS=(int*)&p;
     

     return psquadsettings(a1);
}

int __cdecl OnPlayerSpeed(double a1)
{
    pLog.WriteLog("Hooks:: OnPlayerSPeed(%f)",a1);
    return pPlayerSpeed(a1);

}

int __cdecl h_CreateCharacter(int CharDatumIndex, signed __int16 a2, int* a3)
{
	pLog.WriteLog("Hooks:: CreateChar(%X,%X,%X)", CharDatumIndex,a2,a3);
	mya3 = a3;
	return pcreatechar(CharDatumIndex, a2, a3);
}


#pragma endregion


//Set Addresses of Function  Pointers
void H2AssignPointers()
{




    pgetobj =(hgetobj)(((char*)game.GetBase())+ 0x1304E3);	
    pobjnew=(objenew)(((char*)game.GetBase())+ 0x136CA7);

    punit=(unit_create)(((char*)game.GetBase())+ 0x1F9DCB);
    pvehi=(vehi_create)(((char*)game.GetBase())+ 0x1FF34E);	
    pitem=(generic_create)(((char*)game.GetBase())+ 0x1FF70E);
    pwep=(wep_create)(((char*)game.GetBase())+ 0x1FFB67);

    ptxtup=(TxtUpdate)(((char*)game.GetBase())+ 0x1E81D6);
    psub_EDFF1D =(sub_EDFF1D)(((char*)game.GetBase())+ 0x2FF1D);

    pgameoptions =(GameOptions)(((char*)game.GetBase())+ 0x24FA19);
    pMapSelect =(MapSelect)(((char*)game.GetBase())+ 0x24F9A1);
    pMpgame =(MpGame)(((char*)game.GetBase())+ 0x24F9DD);
    pxx=(XX)(((char*)game.GetBase())+ 0x24F5FD);
    psquadsettings=(SquadSettings)(((char*)game.GetBase())+ 0x24FE89);
    pPlayerSpeed=(PlayerSpeed)(((char*)game.GetBase())+ 0x6AABD);

	pcreatechar=(createchar)(((char*)game.GetBase()) + 0x318B0E);
	

   // pattach = (attach)(((char*)game.GetBase()) + 0x13A2A6);


}

//Enable H2Function Hooks
void H2Hooks()
{
    H2AssignPointers();




#pragma  region Attach  Functions


	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

    //DetourAttach(&(PVOID&)PointerAddr,HookFunction);


	//DetourAttach(&(PVOID&)pgetobj,Hook01);
	//DetourAttach(&(PVOID&)pupdate,Hook02);
	//DetourAttach(&(PVOID&)pupdate2,Hook03);
	//DetourAttach(&(PVOID&)pobjnew,Hook04);

	//DetourAttach(&(PVOID&)punit,CreateUnit);
	//DetourAttach(&(PVOID&)pvehi,CreateVehi);
	//DetourAttach(&(PVOID&)pitem,CreateItem);
	//DetourAttach(&(PVOID&)pwep,CreateWep);
   // DetourAttach(&(PVOID&)psub_EDFF1D,h_sub_EDFF1D);
    DetourAttach(&(PVOID&)pgameoptions,h_GetGameOptions);	
   DetourAttach(&(PVOID&)pMapSelect,h_MPMapSelect);	
   DetourAttach(&(PVOID&)psquadsettings,h_SquadSettings);	
   //  DetourAttach(&(PVOID&)pPlayerSpeed,OnPlayerSpeed);	

   DetourAttach(&(PVOID&)pcreatechar,h_CreateCharacter);
   
   


    DetourTransactionCommit();	

#pragma endregion
    pLog.WriteLog("Hooks:: H2Functions Hooked.");
	return;

}

//Remove Halo 2 Hooks
VOID H2Unhook()
{
     //DetourDetach(&(PVOID&)FunctionPointerAddress,HookFunction);
	pLog.WriteLog("Hooks:: Releasing H2Hooks.");

  


   //DetourDetach(&(PVOID&)pgameoptions,h_GetGameOptions);
   // DetourDetach(&(PVOID&)pMapSelect,h_MPMapSelect);
   // DetourDetach(&(PVOID&)psquadsettings,h_SquadSettings);	
	return;
}


#pragma endregion