#pragma once

#include "pluginmain.h"

#include <fstream>
#include <sstream>

using namespace std;

extern int hViewManual;
extern int hLocalManual;
extern vector<string> instructions;
extern string manualUrl;
extern bool debugging;
extern bool localManual;
extern duint cursor;

bool pluginInit(PLUG_INITSTRUCT* initStruct);
void pluginStop();
void pluginSetup();

void initMenu();
void initCallback();
void initInstructions();
void initManual();

void cbMenuEntry(CBTYPE cbType, void* callbackInfo);
void cbEnterDebugging(CBTYPE cbType, void* callbackInfo);
void cbLeaveDebugging(CBTYPE cbType, void* callbackInfo);