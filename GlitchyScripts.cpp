#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "H2MOD.h"
#include <iostream>
#include <Shellapi.h>
#include <fstream>
#include <iostream>
#include "GlitchyScripts.h"
#include <d3d9.h>
extern LPDIRECT3DDEVICE9 pDevice;


void OverwriteAssembly(BYTE* srcAddr, BYTE* writeAssm, int lenAssm);
void hexToByteArray(BYTE* byteArray, char* pointerHex);
HWND halo2hWnd = NULL;
DWORD halo2ThreadID = 0;

bool IsDediServer;
DWORD MemAddrBase;

int playerNumber = 0;

char** DebugStr;
int DebugTextArrayLenMax = 160;
int DebugTextArrayPos = 0;
bool DebugTextDisplay = false;
RECT rectScreenOriginal;

void setDebugTextDisplay(bool setOn) {
	DebugTextDisplay = setOn;
}

bool getDebugTextDisplay() {
	return DebugTextDisplay;
}

const wchar_t* xinputdllPath = { L"xinput/p00/xinput9_1_0.dll" };

int getPlayerNumber() {
	return playerNumber;
}

void hexToByteArray(BYTE* byteArray, char* pointerHex) {
	char totext2[32];
	memset(totext2, '0', 1);
	char* totext3 = totext2 + 1;
	sprintf(totext3, "%s", pointerHex);
	int len = strlen(totext3);
	if (len % 2 == 1) {
		totext3 = totext2;
		len++;
	}
	for (int i = 0; i < len / 2; i++) {
		char str[3];
		snprintf(str, 3, "%s", totext3 + (i * 2));
		byteArray[i] = (int)strtol(str, NULL, 16);
	}
}

void initPlayerNumber() {
	HANDLE mutex;
	DWORD lastErr;
	do {
		playerNumber++;
		wchar_t mutexName[32];
		swprintf(mutexName, L"Halo2Player%d", playerNumber);
		mutex = CreateMutex(0, TRUE, mutexName);
		lastErr = GetLastError();
		if (lastErr == ERROR_ALREADY_EXISTS) {
			CloseHandle(mutex);
		}
	} while (lastErr == ERROR_ALREADY_EXISTS);

	char NotificationPlayerText[20];
	sprintf(NotificationPlayerText, "You are Player #%d", playerNumber);
	addDebugText(NotificationPlayerText);
	//MessageBoxA(NULL, NotificationPlayerText, "Mutex Created!", MB_OK);

	if (!IsDediServer) {
		if (getPlayerNumber() > 1) {
			BYTE xinputNumFix[] = { '0' + (getPlayerNumber() / 10), 0, '0' + (getPlayerNumber() % 10) };
			OverwriteAssembly((BYTE*)xinputdllPath + 16, xinputNumFix, 3);

			char pointerHex[20];
			sprintf(pointerHex, "%x", (DWORD)xinputdllPath);
			BYTE byteArray[4] = { 0,0,0,0 };
			hexToByteArray(byteArray, pointerHex);
			
			char totext[255];
			sprintf(totext, "injecting new xinputdll push instruction: %ls : %x : %x %x %x %x", (DWORD)xinputdllPath, (DWORD)xinputdllPath, byteArray[3], byteArray[2], byteArray[1], byteArray[0]);
			addDebugText(totext);

			BYTE assmXinputPushIntructionPart[] = { byteArray[3], byteArray[2], byteArray[1], byteArray[0] };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x8AD28, assmXinputPushIntructionPart, 4);

			char xinputName[40];
			char xinputdir[12];
			sprintf(xinputdir, "xinput/p%d%d", getPlayerNumber() / 10, getPlayerNumber() % 10);
			sprintf(xinputName, "%s/xinput9_1_0.dll", xinputdir);
			wchar_t xinputdir2[30];
			MultiByteToWideChar(CP_ACP, 0, xinputdir, -1, xinputdir2, strlen(xinputdir) + 1);
			CreateDirectory(L"xinput", NULL);
			CreateDirectory(xinputdir2, NULL);
			if (FILE *file = fopen(xinputName, "r")) {
				fclose(file);
			}
			else {
				if (FILE* file1 = fopen("xinput9_1_0.dll", "rb")) {
					FILE* file2 = fopen(xinputName, "wb");
					char buffer[BUFSIZ];
					size_t n;
					while ((n = fread(buffer, sizeof(char), sizeof(buffer), file1)) > 0) {
						if (fwrite(buffer, sizeof(char), n, file2) != n) {
							char xinputError[255];
							sprintf(xinputError, "ERROR! Failed to write copied file: %s", xinputName);
							addDebugText(xinputError);
							MessageBoxA(NULL, xinputError, "DLL Copy Error", MB_OK);
							exit(EXIT_FAILURE);
						}
					}
					fclose(file1);
					fclose(file2);

					FILE* file3 = fopen(xinputName, "r+b");
					BYTE assmXinputDuraznoNameEdit[] = { 0x30 + (getPlayerNumber() / 10), 0x00, 0x30 + (getPlayerNumber() % 10) };
					if (fseek(file3, 0x196DE, SEEK_SET) == 0 && fwrite(assmXinputDuraznoNameEdit, sizeof(BYTE), 3, file3) == 3) {
						addDebugText("Successfully copied and patched original xinput9_1_0.dll");
					}
					else {
						fclose(file3);
						remove(xinputName);
						char xinputError[255];
						sprintf(xinputError, "ERROR! Failed to write hex edit to file: %s", xinputName);
						addDebugText(xinputError);
						MessageBoxA(NULL, xinputError, "DLL Edit Error", MB_OK);
						exit(EXIT_FAILURE);
					}
					fclose(file3);
				}
				else {
					char xinputError[] = "ERROR! xinput9_1_0.dll does not exist in the local game directory! For \'Split-screen\' play, the Durazno Xinput DLL is required.";
					addDebugText(xinputError);
					MessageBoxA(NULL, xinputError, "DLL Missing Error", MB_OK);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
}

int getDebugTextArrayMaxLen() {
	return DebugTextArrayLenMax;
}

void addDebugText(char* text) {

	int lenInput = strlen(text);

	char* endChar = strchr(text, '\n');
	if (endChar) {
		lenInput = endChar - text;
	}

	DebugTextArrayPos++;
	if (DebugTextArrayPos >= DebugTextArrayLenMax) {
		DebugTextArrayPos = 0;
	}

	free(DebugStr[DebugTextArrayPos]);
	DebugStr[DebugTextArrayPos] = (char*)malloc(sizeof(char) * lenInput + 1);
	strncpy(DebugStr[DebugTextArrayPos], text, lenInput);
	memset(DebugStr[DebugTextArrayPos] + lenInput, 0, 1);
	if (endChar) {
		addDebugText(endChar + 1);
	}
}

void initDebugText() {
	DebugStr = (char**)malloc(sizeof(char*) * DebugTextArrayLenMax);
	for (int i = 0; i < DebugTextArrayLenMax; i++) {
		DebugStr[i] = (char*)calloc(1, sizeof(char));
	}
	addDebugText("Initialised Debug Text");
}

char* getDebugText(int ordered_index) {
	if (ordered_index < DebugTextArrayLenMax) {
		int array_index = ((DebugTextArrayPos - ordered_index) + DebugTextArrayLenMax) % DebugTextArrayLenMax;
		return DebugStr[array_index];
	}
	return "";
}
bool halo2WindowExists = false;

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

void mainLoop2();

void mainLoop() {
	while (1) {
		later later_test1(1, false, &mainLoop2);
	}
}

void mainLoop2() {
	
	if (1) {
		if (!halo2WindowExists && halo2hWnd != NULL) {
			halo2WindowExists = true;
			if (getPlayerNumber() > 1) {
				wchar_t titleOriginal[200];
				wchar_t titleMod[200];
				GetWindowText(halo2hWnd, titleOriginal, 200);
				wsprintf(titleMod, L"%ls (P%d)", titleOriginal, getPlayerNumber());
				SetWindowText(halo2hWnd, titleMod);
			}
			SetWindowLong(halo2hWnd, GWL_STYLE, GetWindowLong(halo2hWnd, GWL_STYLE) | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		}

		if (GetAsyncKeyState(VK_F3) & 0x1) {
		
			addDebugText("------------------------------");
			addDebugText("Options:");
			addDebugText("~       - Toggle hiding this text display.");
			addDebugText("F3      - Print and show this help text.");
			addDebugText("F5      - Toggle online Coop mode.");
			addDebugText("F8      - Enable/Disable Borderless.");
			addDebugText("F10     - Fix in-game player camera from a white/black bad cutscene.");
			addDebugText("Home    - Sight Possession Hack.");
			addDebugText("Page Up - Set Lobby Privacy to OPEN.");
			addDebugText("Page Dn - Set Lobby Privacy to INVITE ONLY.");
			addDebugText("------------------------------");
			setDebugTextDisplay(true);
		}
		if (GetAsyncKeyState(VK_OEM_3) & 0x1) {
			setDebugTextDisplay(!getDebugTextDisplay());
		}

		//& 0x8000 is pressed
		//& 0x1 Key just transitioned from released to pressed.
		if (GetAsyncKeyState(VK_F8) & 0x1) {
			wchar_t title[255];
			wsprintf(title, L"Confirm Window Mode for Player %d", getPlayerNumber());
			int msgboxID = MessageBox(halo2hWnd,
				L"Go to Borderless Mode?\nNo = Windowed mode.\nWarning: Clicking the same option that is currently active can have weird side effects.",
				title,
				MB_ICONEXCLAMATION | MB_YESNOCANCEL
			);
			if (msgboxID == IDYES)
			{
				RECT rectScreen;
				GetWindowRect(halo2hWnd, &rectScreen);
				D3DVIEWPORT9 pViewport;
				pDevice->GetViewport(&pViewport);
				int width = pViewport.Width;
				int height = pViewport.Height;
				long borderPadX = 0;
				long borderPadY = 0;
				int excessY = GetSystemMetrics(SM_CYCAPTION);

				WINDOWPLACEMENT place3;
				GetWindowPlacement(halo2hWnd, &place3);
				if ((place3.flags & WPF_RESTORETOMAXIMIZED) == WPF_RESTORETOMAXIMIZED) {
					WINDOWPLACEMENT place2;
					GetWindowPlacement(halo2hWnd, &place2);
					place2.showCmd = (place2.showCmd | SW_SHOWNOACTIVATE) & ~SW_MAXIMIZE;
					SetWindowPlacement(halo2hWnd, &place2);
					borderPadX = GetSystemMetrics(SM_CXSIZEFRAME);
					borderPadY = GetSystemMetrics(SM_CYSIZEFRAME);
				}
				GetWindowRect(halo2hWnd, &rectScreenOriginal);

				SetWindowLong(halo2hWnd, GWL_STYLE, GetWindowLong(halo2hWnd, GWL_STYLE) & ~(WS_THICKFRAME | WS_SIZEBOX | WS_BORDER | WS_DLGFRAME));
				//SetWindowLong(halo2hWnd, GWL_STYLE, GetWindowLong(halo2hWnd, GWL_EXSTYLE) & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
				
				SetWindowPos(halo2hWnd, NULL, rectScreen.left + borderPadX, rectScreen.top + borderPadY, width, height + excessY, 0);// SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

			}
			else if (msgboxID == IDNO) {
				SetWindowLong(halo2hWnd, GWL_STYLE, GetWindowLong(halo2hWnd, GWL_STYLE) | WS_THICKFRAME | WS_BORDER | WS_DLGFRAME);
				long width = rectScreenOriginal.right - rectScreenOriginal.left;// -GetSystemMetrics(SM_CXSIZEFRAME) - GetSystemMetrics(SM_CXSIZEFRAME);
				long height = rectScreenOriginal.bottom - rectScreenOriginal.top;// -GetSystemMetrics(SM_CYSIZEFRAME) - GetSystemMetrics(SM_CYSIZEFRAME);
				SetWindowPos(halo2hWnd, NULL, rectScreenOriginal.left, rectScreenOriginal.top, width, height, SWP_FRAMECHANGED);
			}

		}
	}
	
}

int language_code = -1;
bool skip_intro = false;
bool disable_ingame_keyboard = false;

int findStartOfLine(FILE* fp, int lineLen) {
	int fp_offset_orig = ftell(fp);
	for (int i = lineLen; i < 255; i++) {
		if (fp_offset_orig - i < 0) {
			fseek(fp, fp_offset_orig, SEEK_SET);
			return 0;
		}
		fseek(fp, fp_offset_orig - i, SEEK_SET);
		int c = 0;
		if ((c = fgetc(fp)) == '\r' || c == '\n') {
			fseek(fp, fp_offset_orig, SEEK_SET);
			return fp_offset_orig - i + 1;
		}
	}
	fseek(fp, fp_offset_orig, SEEK_SET);
	return fp_offset_orig - lineLen;
}

void fileFail(FILE* fp) {
	int fperrno = GetLastError();
	if (fperrno == EACCES || fperrno == EIO || fperrno == EPERM) {
		MessageBoxA(NULL, "Cannot write a file. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else if (!fp) {
		char NotificationPlayerText[20];
		sprintf(NotificationPlayerText, "Error #%x", fperrno);
		addDebugText(NotificationPlayerText);
		MessageBoxA(NULL, NotificationPlayerText, "Unknown File Failure!", MB_OK);
	}
	exit(1);
}

void ReadStartupOptions() {
	LPWSTR fileStartupini = new WCHAR[256];
	swprintf(fileStartupini, L"h2startup%d.ini", getPlayerNumber());

	int ArgCnt;
	LPWSTR* ArgList = CommandLineToArgvW(GetCommandLineW(), &ArgCnt);
	if (ArgList != NULL)
	{
		for (int i = 0; i < ArgCnt; i++)
		{
			if (wcsstr(ArgList[i], L"-h2startup=") != NULL)
			{
				if (wcslen(ArgList[i]) < 255)
					lstrcpyW(fileStartupini, ArgList[i] + 10);
			}
		}
	}
	//Variables read check
	bool est_language_code = false;
	bool est_skip_intro = false;
	bool est_disable_ingame_keyboard = false;

	int flagged[256];
	int flagged_pos = -1;
	FILE *fp;
	if (fp = _wfopen(fileStartupini, L"r")) {
		flagged_pos = 0;
		char string[256];
		while (fgets(string, 255, fp)) {
			if (flagged_pos > 256) {
				MessageBoxA(NULL, "There are too many bad lines in SETUP config!", "File config overflow!", MB_OK);
				exit(1);
			}
			if (strlen(string) < 4 || string[0] == '#' || (string[0] == '/' && string[1] == '/')) {
				continue;
			}
			if (strstr(string, "language_code =")) {
				int temp;
				sscanf(string + strlen("language_code ="), "%d", &temp);
				if (est_language_code || temp < -1 || temp > 7) {
					flagged[flagged_pos++] = findStartOfLine(fp, strlen(string));
				}
				else {
					language_code = temp;
					est_language_code = true;
				}
			}
			else if (strstr(string, "skip_intro =")) {
				int temp;
				sscanf(string + strlen("skip_intro ="), "%d", &temp);
				if (est_skip_intro || !(temp == 0 || temp == 1) ) {
					flagged[flagged_pos++] = findStartOfLine(fp, strlen(string));
				}
				else {
					skip_intro = (bool)temp;
					est_skip_intro = true;
				}
			}
			else if (strstr(string, "disable_ingame_keyboard =")) {
				int temp;
				sscanf(string + strlen("disable_ingame_keyboard ="), "%d", &temp);
				if (est_disable_ingame_keyboard || !(temp == 0 || temp == 1)) {
					flagged[flagged_pos++] = findStartOfLine(fp, strlen(string));
				}
				else {
					disable_ingame_keyboard = (bool)temp;
					est_disable_ingame_keyboard = true;
				}
			}
		}
		fclose(fp);
		fp = NULL;
		if (!flagged_pos && !(est_language_code && est_skip_intro && est_disable_ingame_keyboard)) {
			flagged_pos = -2;
		}
	}
	if (flagged_pos) {
		if (flagged_pos == -1) {
			if (fp = _wfopen(fileStartupini, L"w")) {
				fclose(fp);
			}
			else {
				fileFail(fp);
			}
		}
		if (fp = _wfopen(fileStartupini, L"r+")) {
			if (flagged_pos == -1) {
				fputs("# language_code Options:", fp);
				fputs("\n# -1 - System Default", fp);
				fputs("\n# 0  - Chinese", fp);
				fputs("\n# 1  - German", fp);
				fputs("\n# 2  - Spanish", fp);
				fputs("\n# 3  - French", fp);
				fputs("\n# 4  - Italian", fp);
				fputs("\n# 5  - Japanese", fp);
				fputs("\n# 6  - Korean", fp);
				fputs("\n# 7  - English", fp);
				fputs("\n\n", fp);
				fputs("# skip_intro Options:", fp);
				fputs("\n# 0 - Normal Intro", fp);
				fputs("\n# 1 - No Intro", fp);
				fputs("\n\n", fp);
				fputs("# disable_ingame_keyboard Options:", fp);
				fputs("\n# 0 - Normal Game Controls", fp);
				fputs("\n# 1 - Disables ONLY Keyboard when in-game & allows controllers when game is not in focus", fp);
				fputs("\n\n", fp);
			}
			else if (flagged_pos > 0) {
				for (int i = flagged_pos - 1; i >= 0; i--) {
					fseek(fp, flagged[i], SEEK_SET);
					fputc('#', fp);
				}
			}
			fseek(fp, 0, SEEK_END);
			if (!est_language_code) {
				fputs("\nlanguage_code = -1", fp);
			}
			if (!est_skip_intro) {
				fputs("\nskip_intro = 0", fp);
			}
			if (!est_disable_ingame_keyboard) {
				char disable_ingame_keyboard_entry[40];
				sprintf(disable_ingame_keyboard_entry, "\ndisable_ingame_keyboard = %d", (bool)(getPlayerNumber() - 1));
				fputs(disable_ingame_keyboard_entry, fp);
			}
			fclose(fp);
		}
		else {
			fileFail(fp);
		}
	}
	char NotificationPlayerText[255];
	if (flagged_pos == -1) {
		sprintf(NotificationPlayerText, "Successfully created new config file: %ls", fileStartupini);
		addDebugText(NotificationPlayerText);
	}
	else if (flagged_pos == -2) {
		sprintf(NotificationPlayerText, "Successfully updated & loaded config file: %ls", fileStartupini);
		addDebugText(NotificationPlayerText);
	}
	else {
		sprintf(NotificationPlayerText, "Successfully loaded config file: %ls", fileStartupini);
		addDebugText(NotificationPlayerText);
	}
}

void ProcessStartup() {
	halo2ThreadID = GetCurrentThreadId();
	initDebugText();
	if (GetModuleHandle(L"H2Server.exe"))
	{
		MemAddrBase = (DWORD)GetModuleHandle(L"H2Server.exe");
		IsDediServer = TRUE;
		addDebugText("Process is Dedi-Server");
	}
	else
	{
		MemAddrBase = (DWORD)GetModuleHandle(L"halo2.exe");
		IsDediServer = FALSE;
		addDebugText("Process is Client");
	}
	initPlayerNumber();
	ReadStartupOptions();

	if (!IsDediServer) {
		bool IntroHQ = TRUE;//clients should set on halo2.exe -highquality

		if (language_code >= 0 && language_code <= 7) {
			BYTE assmLang[15];
			memset(assmLang, language_code, 15);
			OverwriteAssembly((BYTE*)MemAddrBase + 0x38300, assmLang, 15);
		}

		if (skip_intro) {
			BYTE assmIntroSkip[] = { 0x00 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x221BCB, assmIntroSkip, 1);
		}

		if (!skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x221C29, assmIntroHQ, 1);
		}

		//Allows unlimited clients
		BYTE assmUnlimitedClients[41];
		memset(assmUnlimitedClients, 0x00, 41);
		OverwriteAssembly((BYTE*)MemAddrBase + 0x39BCF0, assmUnlimitedClients, 41);

		//Allows on a remote desktop connection
		BYTE assmRemoteDesktop[] = { 0xEB };
		OverwriteAssembly((BYTE*)MemAddrBase + 0x7E54, assmRemoteDesktop, 1);

		//multi-process splitscreen input hacks
		if (disable_ingame_keyboard) {
			//Allows to repeat last movement when lose focus in mp, unlocks METHOD E from point after intro vid
			BYTE getFocusB[] = { 0x00 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2E3C5, getFocusB, 1);
			//Allows input when not in focus.
			BYTE getFocusE[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2F9EA, getFocusE, 6);
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2F9FC, getFocusE, 6);
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA09, getFocusE, 6);
			//Disables the keyboard only when in-game and not in a menu.
			BYTE disableKeyboard1[] = { 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA8A, disableKeyboard1, 3);
			BYTE disableKeyboard2[] = { 0x00 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA92, disableKeyboard2, 1);
			BYTE disableKeyboard3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA67, disableKeyboard3, 6);
		}

	}

	addDebugText("ProcessStartup finished.");
}

void OverwriteAssembly(BYTE* srcAddr, BYTE* writeAssm, int lenAssm) {
	DWORD dwBack;
	VirtualProtect(srcAddr, lenAssm, PAGE_EXECUTE_READWRITE, &dwBack);

	for (int i = 0; i < lenAssm; i++)
		srcAddr[i] = writeAssm[i];

	VirtualProtect(srcAddr, lenAssm, dwBack, &dwBack);
}
