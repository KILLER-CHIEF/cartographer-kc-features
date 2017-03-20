// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#pragma region WindowsSdks

#include <Windows.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <io.h>
#include <string>
#include <stdio.h>
#include <list>
#include <fstream>
#include <stdarg.h>
#include <tchar.h>

#pragma endregion

#include "H2Codez.h"
#include "Detours/detours.h"
#include "Logs.h"
#include "Forge.h"
#include "GlitchyScripts.h"



void H2CodezInitialize();

extern BOOL EnableDbgConsole;



#pragma comment(lib, "Detours/detours")



#pragma region NameSpaces

using std::cout;
using std::cin;
using std::endl;
using std::hex;
using std::dec;
using std::to_string;
using std::ofstream;


#pragma endregion



// TODO: reference additional headers your program requires here


