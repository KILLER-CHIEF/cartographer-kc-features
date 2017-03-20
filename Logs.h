#include "H2CodezMain.h"
using namespace std;

class Logs
{
public:
	Logs(char* filename); //Constructor that creates and sets the Log name
	void Exit();      //close file
	void WriteLog(const char* line, ...); //Write to Log File

private:
	ofstream file;

};

class Debug
{
	
public:
	
	VOID Start_Console();///AllocConsole		
	
private:
	
	
};
extern Logs pLog;
extern Debug Dbg;