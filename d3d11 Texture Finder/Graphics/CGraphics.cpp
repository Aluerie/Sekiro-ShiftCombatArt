#pragma once
#include "CGraphics.h"
#include "PieMenu.h"
#include <string>
#include "../Hooks/Hooks.h"
#include "../Functions/Functions.h"
#include <thread>
#include <future>
#include <algorithm>
#include <fstream>
bool graphicsInit() {
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
		//Kiero already initializes minhook. 
			//printf_s("Initializing Minhook\n");
			//int initialized =  MH_Initialize();
			//if (initialized != MH_OK) { printf_s("%d\n", initialized); }
			//printf_s("Minhook Initialized, Creating Hook\n");

	} while (!init_hook);

	return 0;
}
bool wantTimeSlow;
bool wantHotKeys;
int operatingMode;
#define SLOT_COUNT_MIN 6
#define SLOT_COUNT_MAX 10
bool imguiInit = false;
bool equipSuccess = true;
int lastAttemptedEquip = 0;
int my_image_width = 0;
int my_image_height = 0;
ImFont* font;
std::vector<EquipmentStruct> wheelItems;
std::vector<EquipmentStruct> fileItems;
bool wheelChecked = false;
bool fileTimeSlow = false;
bool fileHotKeys = false;
int fileOperatingMode = 0;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!imguiInit)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(pDevice, pContext);



			structInitializer();

			for (auto it = imageMap.begin(); it != imageMap.end(); ++it)
			{
				std::string strRealID = std::to_string(it->first);
				std::string path = "WeaponWheelResources/" + strRealID + ".png";
				bool ret = LoadTextureFromFile(path.c_str(), &it->second, &my_image_width, &my_image_height);
				//printf("%llx\n", &(currentArt->texture));
				IM_ASSERT(ret);

			}



			ImFontConfig config;
			font = io.Fonts->AddFontFromFileTTF("WeaponWheelResources/NotoSans.ttf", 28, NULL, io.Fonts->GetGlyphRangesDefault());
			config.MergeMode = true;
			font = io.Fonts->AddFontFromFileTTF("WeaponWheelResources/NotoSansTC-Bold.otf", 28, &config, io.Fonts->GetGlyphRangesChineseFull());
			font = io.Fonts->AddFontFromFileTTF("WeaponWheelResources/NotoSansSC-Bold.otf", 28, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			font = io.Fonts->AddFontFromFileTTF("WeaponWheelResources/NotoSansKR-Bold.otf", 28, &config, io.Fonts->GetGlyphRangesKorean());
			font = io.Fonts->AddFontFromFileTTF("WeaponWheelResources/NotoSansThai-Bold.ttf", 28, &config, io.Fonts->GetGlyphRangesThai());

			
			std::ifstream in;
			in.open("WeaponWheelResources/wheelItems.txt");
			std::string content;
			
			while (in >> content)
			{
				int fileRealID = stoi(content);
				if (fileRealID > 0 && fileRealID < 100000)
				{
					fileItems.emplace_back("", fileRealID);
				}
			}
			in.close();
			std::ifstream in2;
			in2.open("WeaponWheelResources/settings.txt");
			in2 >> fileTimeSlow >> fileHotKeys >> fileOperatingMode;
			in2.close();
			wantTimeSlow = fileTimeSlow;
			wantHotKeys = fileHotKeys;
			operatingMode = fileOperatingMode;
			if (fileItems.size() < SLOT_COUNT_MIN)
			{
				for (size_t i = 0; i < SLOT_COUNT_MIN; i++)
				{
					wheelItems.emplace_back("", 5000);
				}
			}
			else {
				for (size_t i = 0; i < fileItems.size(); i++)
					wheelItems.push_back(fileItems[i]);
			}


			imguiInit = true;

		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	float reference_res = 900;
	float reference_scale = 0.5;
	float desired_scale = io.DisplaySize.y / (reference_res / reference_scale);
	font->Scale = desired_scale;

	//ImGui::ShowDemoWindow();
	//do stuff
	if (!loadedIn())
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::Begin("Mod Debug", NULL, flags);
		ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, 0));
		ImGui::Text("Weapon Wheel Controls: Dpad-Down + Right Joystick, or Tab + Mouse. This message will disappear once you're loaded in.");
		ImGui::End();
		wheelChecked = false;

	}



	if (loadedIn())
	{

		if (!wheelChecked)
		{
			for (size_t i = 0; i < wheelItems.size(); i++)
			{
				if (!validateItem(wheelItems[i].realID)) {
					wheelItems[i].realID = 5000;
				}
			}
			wheelChecked = true;
		}


		//TESTING GROUND
		WeaponWheelSettings();

		ImGui::SetNextWindowPos({ -100, -100 }); //weird black box fix
		int artIndexSelected = -1;
		int prostheticIndexSelected = -1;


		for (size_t i = 0; i < wheelItems.size(); i++) //sets highlight to whatever is currently equipped.
		{
			int itemIconID = iconIDCalculator(wheelItems[i].realID);
			if (itemIconID == getCurrentEquippedIconID(COMBAT_ART_SLOT))
			{
				artIndexSelected = i;
			}
			if (itemIconID == getCurrentEquippedIconID(getActiveProstheticSlot()))
			{
				prostheticIndexSelected = i;
				//printf("PROSTHETICINDEXSELECTED: %d", prostheticIndexSelected); 
			}
		}
		if (!ImGui::IsAnyItemHovered() && (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Tab)) || ImGui::IsKeyDown(ImGuiKey_GamepadDpadDown)) && getGameMenuState() == 0 && getOtherMenuStates() == 0)
		{
			ImGui::OpenPopup("##piepopup");
			popupOpen = true;
			Hooks::setGameMenu(8, 0);
		}
		else
		{
			popupOpen = false;
			if (!equipSuccess)
			{
				printf("ATTEMPTING LAST EQUIP: %d \n", lastAttemptedEquip);
				equipSuccess = attemptEquip(lastAttemptedEquip);
				if (equipSuccess && lastAttemptedEquip < 10000)
				{
					GUISoundMaker9000(0x29, 0x64);

				}
			}

		}
		
		ImVec2 windowCenter = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2);
		int n = PiePopupSelectMenu(windowCenter, "##piepopup", wheelItems, &artIndexSelected, &prostheticIndexSelected);

		if (loadedIn() && n != -1)
		{
			if (artIndexSelected != -1)
			{
				int artSelectedID = iconIDCalculator(wheelItems[artIndexSelected].realID);
				int tempTestID = getCurrentEquippedIconID(COMBAT_ART_SLOT);
				equipSuccess = attemptEquip(wheelItems[artIndexSelected].realID);
				if (!equipSuccess)
				{
					lastAttemptedEquip = wheelItems[artIndexSelected].realID;
					//printf("ART EQUIP FAILED\n");
				}
				if (equipSuccess && artSelectedID != tempTestID)
				{
					GUISoundMaker9000(0x29, 0x64);

				}

			}
			if (prostheticIndexSelected != -1)
			{
				int prostheticSelectedID = iconIDCalculator(wheelItems[prostheticIndexSelected].realID);

				if (equipSuccess) //if the last equip was successful
				{
					equipSuccess = attemptEquip(wheelItems[prostheticIndexSelected].realID);
					if (!equipSuccess)
					{
						lastAttemptedEquip = wheelItems[prostheticIndexSelected].realID;
						//printf("PROSTHETIC EQUIP FAILED\n");
					}
				}
			}
		}
		else if (loadedIn() && wantHotKeys)
		{
			for (int i = 0; i <= 9; i++) {
				
				if(ImGui::IsKeyPressed(ImGuiKey_0 + i) || ImGui::IsKeyPressed(ImGuiKey_Keypad0 + i)) {

					int wheelItemIndex = i - 1;
					if (wheelItemIndex < 0)
					{
						wheelItemIndex = 9;
					}
					if (wheelItemIndex < wheelItems.size())
					{
						equipSuccess = attemptEquip(wheelItems[wheelItemIndex].realID);
						GUISoundMaker9000(0x29, 0x1F4);
						if (!equipSuccess)
						{
							lastAttemptedEquip = wheelItems[wheelItemIndex].realID;
						}

						if (wheelItems[wheelItemIndex].realID < 10000)
						{
							int artSelectedID = iconIDCalculator(wheelItems[artIndexSelected].realID);
							int tempTestID = getCurrentEquippedIconID(COMBAT_ART_SLOT);
							if (equipSuccess && artSelectedID != tempTestID)
							{
								GUISoundMaker9000(0x29, 0x64);
							}
						}
					}
				}
			}
		}
	}

	


	//the end
	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}


LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	/**/
	//oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	if (io.WantCaptureMouse)
	{
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool openConfigurator = false;
bool configPopupOpen = false;
void WeaponWheelSettings() {

	if (getGameMenuState() == 0x2)
	{
		ImGuiIO& io = ImGui::GetIO();


			ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
			//popupOpen = false;
			ImGui::Begin("This should not be visible", NULL, flags);
			ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, 0));

			if (ImGui::Button("Weapon Wheel Options [select]") || ImGui::IsKeyPressed(ImGuiKey_GamepadBack)) {
				GUISoundMaker9000(0x29, 0x3E8);
				ImGui::OpenPopup("wheel options");
				configPopupOpen = true;
			}
			flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
			if (ImGui::BeginPopup("wheel options", flags))
			{
				const float pixel_ratio = ImGui::GetIO().DisplaySize.y / 900;
				float image_size_modifier = 0.2 * pixel_ratio;
				configPopupOpen = true;
				ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, 0));
				static int wheelSize = wheelItems.size();
				ImGui::SliderInt("Slot Count", &wheelSize, SLOT_COUNT_MIN, SLOT_COUNT_MAX);
				if (wheelSize > SLOT_COUNT_MAX)
					wheelSize = SLOT_COUNT_MAX;
				if (wheelSize < SLOT_COUNT_MIN)
					wheelSize = SLOT_COUNT_MIN;
				int wheelDifference = wheelSize - wheelItems.size();
				if (wheelDifference > 0)
				{
					for (size_t i = 0; i < wheelDifference; i++) {
						wheelItems.emplace_back("", 5000);

					}
				}
				else if (wheelDifference < 0)
				{
					for (int i = 0; i > wheelDifference; i--) {
						wheelItems.pop_back();
					}
				}

				for (size_t i = 0; i < wheelItems.size(); i++)
				{

					if (ImGui::Button(("SLOT " + std::to_string(i + 1) + ": " + wheelItems[i].getName()).c_str())) {
						GUISoundMaker9000(0x29, 0x3E8);
						ImGui::OpenPopup(("slot_modification_popup" + std::to_string(i + 1)).c_str());
					}

					if (imageMap.find(wheelItems[i].realID) != imageMap.end())
					{
						ImGui::Image(imageMap[wheelItems[i].realID], ImVec2(my_image_width * image_size_modifier, my_image_height * image_size_modifier));

					}
					if (ImGui::BeginPopup(("slot_modification_popup" + std::to_string(i + 1)).c_str()))
					{

						if (ImGui::Button("Select Combat Art...")) {
							GUISoundMaker9000(0x29, 0x3E8);
							ImGui::OpenPopup(("combat art popup" + std::to_string(i + 1)).c_str());
						}
						if (ImGui::BeginPopup(("combat art popup" + std::to_string(i + 1)).c_str()))
						{
							for (int j = 5000; j < 10000; j+=100) {
								int testIconID = iconIDCalculator(j);
								if (testIconID)
								{
									std::string selectableStr = getNameFromRealID(j);
									if (selectableStr != "")
									{
										if (ImGui::Selectable(getNameFromRealID(j).c_str()))
											wheelItems.at(i) = EquipmentStruct("", j);
									}

								}
								else
								{
									//printf("FAILED ICON ID TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
								}

							}

							ImGui::EndPopup();
						}

						ImGui::Text("or");

						if (ImGui::Button("Select Prosthetic...")) {
							GUISoundMaker9000(0x29, 0x3E8);
							ImGui::OpenPopup(("prosthetic popup" + std::to_string(i + 1)).c_str());
						}
						if (ImGui::BeginPopup(("prosthetic popup" + std::to_string(i + 1)).c_str()))
						{

							for (int j = 70000; j < 100000; j += 100) {
								int testIconID = iconIDCalculator(j);
								if (testIconID)
								{
									std::string selectableStr = getNameFromRealID(j);
									if (selectableStr != "")
									{
										if (ImGui::Selectable(getNameFromRealID(j).c_str()))
											wheelItems.at(i) = EquipmentStruct("", j);
									}

								}
							}

							ImGui::EndPopup();
						}

						ImGui::EndPopup();
					}

					ImGui::Separator();

				}

				if (ImGui::Button("Advanced Settings")) {
					GUISoundMaker9000(0x29, 0x3E8);
					ImGui::OpenPopup("Advanced Settings");
					configPopupOpen = true;
				}

				if (ImGui::BeginPopup("Advanced Settings", flags))
				{
					ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, 0));
					ImGui::Text("Advanced Settings                           ");
					ImGui::Checkbox("Slow Down Time On Wheel Open", &wantTimeSlow);
					ImGui::Checkbox("Enable HotKeys", &wantHotKeys);
					ImGui::Text("Operating Mode:");
					ImGui::RadioButton("Vanilla", &operatingMode, 0);
					ImGui::RadioButton("Instant Switch", &operatingMode, 1);
					ImGui::TextWrapped("Instant Switch removes the buffer overwrite protection. Very cool and stylish (with practice), but buggy and not very useful. Vanilla recommended for immersion.");
					ImGui::EndPopup();
				}

				if (ImGui::Button("Close Options")) {
					GUISoundMaker9000(0x29, 0x3E8);
					ImGui::CloseCurrentPopup();
				}


				ImGui::EndPopup();
			}
			else {
				configPopupOpen = false;
			}

			ImGui::End();

			if (wheelItems != fileItems)
			{
				std::ofstream out;
				out.open("WeaponWheelResources/wheelItems.txt");
				for (size_t i = 0; i < wheelItems.size(); i++)
				{
					out << wheelItems[i].realID << std::endl;

				}
				out.close();
				fileItems.clear();
				for (size_t i = 0; i < wheelItems.size(); i++)
					fileItems.push_back(wheelItems[i]);
			}

			if ((wantTimeSlow != fileTimeSlow) ||( wantHotKeys != fileHotKeys) || (operatingMode != fileOperatingMode))
			{
				std::ofstream out;
				out.open("WeaponWheelResources/settings.txt");
				out << wantTimeSlow << std::endl << wantHotKeys << std::endl << operatingMode << std::endl;
				fileTimeSlow = wantTimeSlow;
				out.close();
			}
	}

	else
	{
		openConfigurator = false;
	}
}