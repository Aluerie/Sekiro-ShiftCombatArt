#include "application.h"
#include <thread>
#include <fstream>

HINSTANCE mHinst = 0, mHinstDLL = 0;
extern "C" UINT_PTR mProcs[6] = { 0 };

LPCSTR mImportNames[6] = {

	"DirectInput8Create",
	"GetdfDIJoystick",
	"DllCanUnloadNow",
	"DllGetClassObject",
	"DllRegisterServer",
	"DllUnregisterServer",

};

void CreateConsole()
{
	AllocConsole();

	FILE* fp;

	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);

	SetConsoleTitleA("Sekiro Mod Console");

}

std::thread main_mod_thread;
void BeginMod() {

	char dllpath[MAX_PATH];

	GetSystemDirectory(dllpath, MAX_PATH);

	strcat_s(dllpath, "\\dinput8.dll");

	mHinstDLL = LoadLibrary(dllpath);

	if (!mHinstDLL) {
		MessageBoxA(NULL, "Failed to load original DLL", "Error", MB_ICONERROR);
		return;
	};

	for (int i = 0; i < 6; i++)
		mProcs[i] = (UINT_PTR)GetProcAddress(mHinstDLL, mImportNames[i]);

	main_mod_thread = std::thread(ApplicationLoader::init);
#if _DEBUG
	CreateConsole();
#endif

	return;
};



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	std::ifstream in;
	std::string content;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:

		DisableThreadLibraryCalls(hinstDLL);


		in.open("WeaponWheelResources/chainload.txt");
		in >> content;
		in.close();
		LoadLibraryA(content.c_str());
		BeginMod();

		break;

	case DLL_PROCESS_DETACH:

		FreeLibrary(mHinstDLL);

		break;
	}

	return TRUE;

}
extern "C" void _DirectInput8Create();