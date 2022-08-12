#include "pch.h"
#include <cstdio>
#include <Windows.h>
#include <iostream>
#include <string>

#include <dxgi.h>
#include <ctime>
#include "dllmain.h"
#include <stdio.h>

#include "Kiero/kiero.h"
#include <d3d11.h>

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


/// <summary>
/// 
/// The initial called function in the library. It's called for process attachment, thread attachment, and both detachments. I've deleted the thread cases since it doesn't matter.
///	It then loads Sekiro Online (and silently fails if it doesn't exist.) and creates a thread for our Begin function which is the meat and potatoes.
/// 
/// </summary>
/// <param name="hModule"></param>
/// <param name="ul_reason_for_call"></param>
/// <param name="lpReserved"></param>
/// <returns></returns>
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {


    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        DisableThreadLibraryCalls(hModule);
        //LoadLibraryA("SekiroOnline.dll");
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Begin, NULL, NULL, NULL);
        break;

    case DLL_PROCESS_DETACH:

        FreeLibrary(hModule);
        break;
    }
    return TRUE;

}



DWORD WINAPI Begin(LPVOID lpParam) {

	char dllpath[MAX_PATH];
	GetSystemDirectoryA(dllpath, MAX_PATH);
	strcat_s(dllpath, "\\dinput8.dll");
	mHinstDLL = LoadLibraryA(dllpath); //load the original library that the game wanted...

	if (!mHinstDLL) {

		MessageBoxA(NULL, "Failed to load original DLL", "Error", MB_ICONERROR);
		return FALSE;

	};

	for (int i = 0; i < 6; i++) {

		mProcs[i] = (UINT_PTR)GetProcAddress(mHinstDLL, mImportNames[i]);

	};

	CreateConsole();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)testPrinter, 0, 0, 0);

	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		kiero::bind(8, (void**)&oPresent, hkPresent);
	}

	return FALSE;

};

void CreateConsole()
{
	AllocConsole();

	FILE* fp;

	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);

	SetConsoleTitleA("Sekiro Test Console!");

}

void testPrinter() {
	while (true)
	{
		printf("test\n");
		Sleep(100);
	}
}

extern "C" void _DirectInput8Create();


