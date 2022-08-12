#pragma once



//directx stuff
typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern Present oPresent;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);


//my stuff
DWORD WINAPI Begin(LPVOID lpParam);
void CreateConsole();
void testPrinter();