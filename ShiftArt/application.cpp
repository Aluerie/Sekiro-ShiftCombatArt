#include "application.h"




CApplicationCore* SekiroApp = nullptr;
void ApplicationLoader::init() {

	SekiroApp = new CApplicationCore();
	Sleep(10000);
	graphicsInit();
	Hooks::createHooks();
}