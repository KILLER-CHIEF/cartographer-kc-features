#include "H2CodezMain.h"


enum ProcessType
{
	Halo2,
	H2Server,
		
};

class H2class
{
public:

	 ProcessType H2Type;	 //Process is Halo 2 or H2Server	
	 
							 //Initialize Things
	 void Start();
	 //Gets Player Unit Datum from Player Index
	 int get_unit_datum_from_player_index(int pIndex);      
	
	 unsigned __int16 SpawnObj(int datumindex);
	 unsigned __int16 SpawnObjAtCamera(int datumindex);




	 void GivePlayerWeapon(int PlayerIndex, int WeaponId);
	 int __cdecl unit_set_active_camo(int Unit, char Active, float Time);

	
	 


};

class Game
{
public:
	//Holds the Process Base
	 HMODULE g_base;

	 //Returns The Base Address
	 DWORD GetBase();

	int __cdecl call_get_object(signed int object_datum_index, int object_type);
	//Reset Units Equipment
	int __cdecl call_unit_reset_equipment(int unit_datum_index);
	//Destroy Object
	int __cdecl call_hs_object_destroy(int object_datum_index);
	signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit);
	//Assigns an Equipment to the Unit
	bool __cdecl call_assign_equipment_to_unit(int unit, int object_index, short unk);
	//Initializes SpawnData of the Object
	int __cdecl call_object_placement_data_new(void* s_object_placement_data, int object_definition_index, int object_owner, int unk);
	//Spawns an object Based on its SpawnData
	signed int __cdecl call_Object_create(void * objplacementdata);
	//SyncObject
	char __cdecl call_ObjectSync(int GameObjectDatum);

	void* __cdecl call_CreateCharacter(int CharDatumIndex, signed __int16 WepIndex, void* SpawnData);

	void __cdecl call_CharacterInitialize(void *SpawnData);
	int __cdecl call_AssignObjDatumToPlayer(int pIndex, int UnitDatum);



};







extern Game game;

extern H2class halo;