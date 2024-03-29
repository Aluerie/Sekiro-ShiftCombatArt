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

	float reference_res = 900;
	float reference_scale = 0.5;
	float desired_scale = io.DisplaySize.y / (reference_res / reference_scale);

	if (loadedIn())
	{
		if (ImGui::IsKeyPressed(ImGuiKey_LeftShift, false)) // 5700 - MD; 7300 - EMD; for others check original repo :D
		{
			equipSuccess = attemptEquip(5700);
			if (!equipSuccess)
			{
				artID = 5700;
			}
		}
		else if (ImGui::IsKeyReleased(ImGuiKey_LeftShift))
		{
			equipSuccess = attemptEquip(5000);
			if (!equipSuccess)
			{
				artID = 5000;
			}
		}
		if (!equipSuccess)
		{
			// printf("ATTEMPTING LAST EQUIP: %d \n", artID);
			equipSuccess = attemptEquip(artID);
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