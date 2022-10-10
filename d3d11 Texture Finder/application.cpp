#include "application.h"




CApplicationCore* SekiroApp = nullptr;
void ApplicationLoader::init() {

	SekiroApp = new CApplicationCore();
	graphicsInit();
	Sleep(10000);
	Hooks::createHooks();
}