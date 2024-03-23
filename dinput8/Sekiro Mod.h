#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include "includes.h"
#include "kiero/minhook/include/MinHook.h"

extern Present oPresent;

bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
void InitImGui();
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

DWORD memoryScan(DWORD ID, bool forward, size_t maxOffset);
DWORD64 GetPlayerGameDataPtr();

void CreateConsole();
BOOL inputHandler(int * varAddr);
int main();
bool CheckPackedBytes(DWORD64 qAddress, DWORD dBytes);
void structInitializer();
//typedef WORD(__fastcall* _GamePadInputBool)(uintptr_t gamepadPtr, int indexThingSorta);
