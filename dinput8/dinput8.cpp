#pragma once
#include <stdio.h>
#include "Sekiro Mod.h"

HINSTANCE mHinst = 0, mHinstDLL = 0;
extern "C" UINT_PTR mProcs[6] = {0};

//extern bool hookerInput(uintptr_t gamepadPtr, int indexThingSorta);
//extern _GamePadInputBool gamePadInputBool;

LPCSTR mImportNames[6] = {

	"DirectInput8Create",
	"GetdfDIJoystick",
	"DllCanUnloadNow",
	"DllGetClassObject",
	"DllRegisterServer",
	"DllUnregisterServer",

};

DWORD WINAPI Begin(LPVOID lpParam) {

	char dllpath[MAX_PATH];

	GetSystemDirectory(dllpath, MAX_PATH);

	strcat_s(dllpath, "\\dinput8.dll");

	mHinstDLL = LoadLibrary(dllpath);

	if (!mHinstDLL) {

		MessageBoxA(NULL, "Failed to load original DLL", "Error", MB_ICONERROR);

		return FALSE;

	};



	for (int i = 0; i < 6; i++) {

		mProcs[i] = (UINT_PTR)GetProcAddress(mHinstDLL, mImportNames[i]);

	};

	//Add Any Code Needed Here To Hook
	bool init_hook = false;
	do
	{

		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)structInitializer, 0, 0, 0);

	//Personal Hook Attempt
	CreateConsole();

	//Kiero already initializes minhook. 
	//printf_s("Initializing Minhook\n");
	//int initialized =  MH_Initialize();
	//if (initialized != MH_OK) { printf_s("%d\n", initialized); }
	//printf_s("Minhook Initialized, Creating Hook\n");




	return FALSE;
};



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:

		DisableThreadLibraryCalls(hinstDLL);

		LoadLibraryA("SekiroOnline.dll");


		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Begin, NULL, NULL, NULL);


		break;

	case DLL_PROCESS_DETACH:

		FreeLibrary(mHinstDLL);

		break;
	}

	return TRUE;

}

void CreateConsole()
{
	AllocConsole();

	FILE* fp;

	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);

	SetConsoleTitleA("Sekiro Mod Console");

}

bool CheckPackedBytes(DWORD64 qAddress, DWORD dBytes) {
	printf_s("%lX vs %lX\n", *(DWORD*)qAddress, dBytes);
	if (*(DWORD*)qAddress == dBytes) return true;
	return false;
};

extern "C" void _DirectInput8Create();




