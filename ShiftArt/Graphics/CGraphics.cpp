#pragma once
#include "CGraphics.h"
#include <string>
#include "../Hooks/Hooks.h"
#include "../Functions/Functions.h"
#include <thread>
#include <future>
#include <algorithm>
#include <fstream>
bool graphicsInit()
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void **)&oPresent, hkPresent);
			init_hook = true;
		}
		// Kiero already initializes minhook.
		// printf_s("Initializing Minhook\n");
		// int initialized =  MH_Initialize();
		// if (initialized != MH_OK) { printf_s("%d\n", initialized); }
		// printf_s("Minhook Initialized, Creating Hook\n");

	} while (!init_hook);

	return 0;
}

int artID = 5700;
int latestArtID = 5000;

bool wantHotKeys;
int operatingMode;
#define SLOT_COUNT_MIN 6
#define SLOT_COUNT_MAX 10
bool imguiInit = false;
bool equipSuccess = true;
int lastAttemptedEquip = 0;
int my_image_width = 0;
int my_image_height = 0;
std::vector<EquipmentStruct> wheelItems;
std::vector<EquipmentStruct> fileItems;
bool wheelChecked = false;
bool fileTimeSlow = false;
bool fileHotKeys = false;
int fileOperatingMode = 0;
HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!imguiInit)
	{
		// DEBUG CONSOLE
		// UNCOMMENT TO CREATE A CONSOLE WHEN THE GAME STARTS SO WE CAN SEE ALL OUR PRINTF'S THERE.
		// AllocConsole();
		// FILE *fDummy;
		// freopen_s(&fDummy, "CONIN$", "r", stdin);
		// freopen_s(&fDummy, "CONOUT$", "w", stderr);
		// freopen_s(&fDummy, "CONOUT$", "w", stdout);
		// END OF DEBUG CONSOLE

		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D *pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui::CreateContext();
			ImGuiIO &io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(pDevice, pContext);

			imguiInit = true;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	if (loadedIn())
	{
		// choose combat art
		// For combat arts that aren't listed refer to original repo files :D
		if (ImGui::IsKeyPressed(ImGuiKey_1, false))
		{
			artID = 5700; // Mortal Draw
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_2, false))
		{
			artID = 5400; // Dragon Flash
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_3, false))
		{
			artID = 7700; // Sakura Dance
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_4, false))
		{
			artID = 7100; // Double Ichimonji
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_5, false))
		{
			artID = 7300; // Empowered Mortal Draw
		}

		// Shift Key
		if (ImGui::IsKeyPressed(ImGuiKey_LeftShift, false))
		{
			latestArtID = artID;
			equipSuccess = attemptEquip(latestArtID);
		}
		else if (ImGui::IsKeyReleased(ImGuiKey_LeftShift))
		{
			latestArtID = 5000; // Unequip Combat Art
			equipSuccess = attemptEquip(latestArtID);
		}

		if (!equipSuccess)
		{
			// printf("ATTEMPTING LAST EQUIP: %d \n", artID);
			equipSuccess = attemptEquip(latestArtID);
		}
	}

	// the end
	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/**/
	// oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	if (io.WantCaptureMouse)
	{
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool openConfigurator = false;
bool configPopupOpen = false;