#pragma once



void mainLoop();
void ProcessStartup();

int getPlayerNumber();

int getDebugTextArrayMaxLen();
void addDebugText(char* text);
char* getDebugText(int ordered_index);

extern HWND halo2hWnd;

void setDebugTextDisplay(bool setOn);
bool getDebugTextDisplay();

