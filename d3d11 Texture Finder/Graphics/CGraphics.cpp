#pragma once
#include "CGraphics.h"
#include "PieMenu.h"
#include "ImGuiTheme.h"
#include "../Hooks/Hooks.h"
#include "../Functions/Functions.h"
#include <string>
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

clock_t start = 0;
float alpha = 0;
int alphaIndex = 0;
bool usingGamepad2;
bool analogueTriggerState = 0;
bool wantTimeSlow;
bool wantHotKeys;
bool triggerSetting;
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
uint64_t inputAddr = 0;

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

			embraceTheDarkness();

			structInitializer();
			structInitializer();



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
					fileItems.emplace_back("", fileRealID, ImageInfoStruct());
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
					wheelItems.emplace_back("", 5000, ImageInfoStruct());
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
	if (desired_scale)
	{
		font->Scale = desired_scale;

	}

	//ImGui::ShowDemoWindow();
	//do stuff
	if (!loadedIn())
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::Begin("Mod Debug", NULL, flags);
		ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, 0));
		ImGui::Text("Weapon Wheel Controls: Dpad-Down + Right Joystick, or Tab + Mouse. This message will disappear once you're loaded in (if not, update to Sekiro v1.06)");
		ImGui::End();
		wheelChecked = false;

	}

	if (loadedIn())
	{

		if (!wheelChecked)
		{
			wheelChecked = true;
			for (size_t i = 0; i < wheelItems.size(); i++)
			{
				if (!validateItem(wheelItems[i].realID)) {
					wheelItems[i].realID = 5000;
				}
			}
			for (auto it = imageInfos.begin(); it != imageInfos.end(); ++it)
			{
				it->SetImagePtr();
				if (it->imageptr == 0)
				{
					wheelChecked = false;
				}
			}
			for (size_t i = 0; i < wheelItems.size(); i++)
			{
				wheelItems[i].image = *getImageFromRealID(wheelItems[i].realID);
				 
			}
			
		}

#if 0
		ImGui::Begin("Mod Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::InputScalar("Resource Address", ImGuiDataType_U64, &inputAddr, NULL, NULL, "%llx", ImGuiInputTextFlags_CharsHexadecimal);

		uint64_t ptr = *(uint64_t*)(0x143D98F10);
		ptr = *(uint64_t*)(ptr + 0x58);
		ptr = *(uint64_t*)(ptr + 0xC30);
		ptr = *(uint64_t*)(ptr + 0x80);




		uint64_t sb_icon_RV = *(uint64_t*)(ptr + 0x138);
		sb_icon_RV = *(uint64_t*)(sb_icon_RV + 0x68);
		sb_icon_RV = *(uint64_t*)(sb_icon_RV + 0x10);
		sb_icon_RV = *(uint64_t*)(sb_icon_RV + 0x48);
		sb_icon_RV = *(uint64_t*)(sb_icon_RV + 0x10);

		uint64_t sb_boss_RV = *(uint64_t*)(ptr + 0x108);
		sb_boss_RV = *(uint64_t*)(sb_boss_RV + 0x68);
		sb_boss_RV = *(uint64_t*)(sb_boss_RV + 0x10);
		sb_boss_RV = *(uint64_t*)(sb_boss_RV + 0x48);
		sb_boss_RV = *(uint64_t*)(sb_boss_RV + 0x10);

		uint64_t sb_icon2_RV = *(uint64_t*)(ptr + 0x140);
		sb_icon2_RV = *(uint64_t*)(sb_icon2_RV + 0x68);
		sb_icon2_RV = *(uint64_t*)(sb_icon2_RV + 0x10);
		sb_icon2_RV = *(uint64_t*)(sb_icon2_RV + 0x48);
		sb_icon2_RV = *(uint64_t*)(sb_icon2_RV + 0x10);

		uint64_t sb_icon3_RV = *(uint64_t*)(ptr + 0x148);
		sb_icon3_RV = *(uint64_t*)(sb_icon3_RV + 0x68);
		sb_icon3_RV = *(uint64_t*)(sb_icon3_RV + 0x10);
		sb_icon3_RV = *(uint64_t*)(sb_icon3_RV + 0x48);
		sb_icon3_RV = *(uint64_t*)(sb_icon3_RV + 0x10);

		uint64_t sb_icon4_RV = *(uint64_t*)(ptr + 0x150);
		sb_icon4_RV = *(uint64_t*)(sb_icon4_RV + 0x68);
		sb_icon4_RV = *(uint64_t*)(sb_icon4_RV + 0x10);
		sb_icon4_RV = *(uint64_t*)(sb_icon4_RV + 0x48);
		sb_icon4_RV = *(uint64_t*)(sb_icon4_RV + 0x10);


		ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, 0));
		if (ImGui::Button("icon id test"))
		{
			printf("%d\n", GetRealIconIDFromRealID(5100));
			printf("%d\n", GetRealIconIDFromRealID(5200));

		}
		ImGui::Text("Memory Texture Test");
		if (inputAddr && ImGui::IsKeyDown(ImGuiKey_LeftAlt))
		{
			printf("%llx\n", inputAddr);
			ID3D11ShaderResourceView* texture = (ID3D11ShaderResourceView*)inputAddr;
			ImGui::Image((void*)texture, ImVec2(1024 / 4, 2048 / 4));
		}
		else {
			ID3D11ShaderResourceView* texture = (ID3D11ShaderResourceView*)sb_boss_RV;
			ImGui::Image((void*)texture, ImVec2(2048 / 4, 2048 / 4));
			//
			texture = (ID3D11ShaderResourceView*)sb_icon_RV;
			ImGui::Image((void*)texture, ImVec2(4096 / 4, 2048 / 4));
			//
			texture = (ID3D11ShaderResourceView*)sb_icon2_RV;
			ImGui::Image((void*)texture, ImVec2(252, 252), ImVec2(768.0f/1024.0f, 0.0f), ImVec2(1020.0f/1024.0f, 252.0f/2048.0f));
			//
			texture = (ID3D11ShaderResourceView*)sb_icon3_RV;
			ImGui::Image((void*)texture, ImVec2(4096 / 4, 2048 / 4));
			//
			texture = (ID3D11ShaderResourceView*)sb_icon4_RV;
			ImGui::Image((void*)texture, ImVec2(1024 / 4, 2048 / 4));
		}

		ImGui::End();
#endif
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
			if (ImGui::IsKeyDown(ImGuiKey_GamepadDpadDown))
			{
				usingGamepad2 = true;
			}
			else
			{
				usingGamepad2 = false;
			}
			ImGui::OpenPopup("##piepopup");
			popupOpen = true;
			Hooks::setGameMenu(8, 0);
			analogueTriggerState = 1;
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
						start = clock();
						GUISoundMaker9000(0x29, 0x1F4);
						if (!equipSuccess)
						{
							lastAttemptedEquip = wheelItems[wheelItemIndex].realID;
						}

						if (wheelItems[wheelItemIndex].realID < 10000)
						{
							int artSelectedID = iconIDCalculator(wheelItems[wheelItemIndex].realID);
							int tempTestID = getCurrentEquippedIconID(COMBAT_ART_SLOT);

								GUISoundMaker9000(0x29, 0x64);
							
						}
					}
				}
			}

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
			ImGui::Begin("hotkey image", NULL, flags);
			ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - ImGui::GetWindowWidth(), 0));

			double duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
			if (duration < 1.5)
			{
				if (alpha < 1)
				{
					alpha += 0.45f;

				}
			}
			else {
				if (alpha > 0)
				{
					alpha -= 0.15f;
				}

			}
			int realImageID = 0;

			if (alpha > 0)
			{
				for (auto it = wheelItems.begin(); it != wheelItems.end(); ++it)
				{
					if (iconIDCalculator(it->realID) == getCurrentEquippedIconID(COMBAT_ART_SLOT))
					{
						ImageInfoStruct* image = &it->image;
						ImGui::Image(image->imageptr, ImVec2(image->width * 0.5, image->height * 0.5), image->uv0, image->uv1, ImVec4(1, 1, 1, alpha));
					}
				}
			}


			ImGui::End();

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
std::unordered_map<int, ImageInfoStruct*> reducedImagePool;
bool poolReduced = false;
void WeaponWheelSettings() {

	if (getGameMenuState() == 0x2)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, 0), NULL, ImVec2(0.5f, 0.0f));

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
			//popupOpen = false;
			ImGui::Begin("This should not be visible", NULL, flags);

			if (ImGui::Button("Weapon Wheel Options [select]") || ImGui::IsKeyPressed(ImGuiKey_GamepadBack)) {
				GUISoundMaker9000(0x29, 0x3E8);
				ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, 0), NULL, ImVec2(0.5f, 0.0f));
				ImGui::OpenPopup("wheel options");
				configPopupOpen = true;

			}
			flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
			if (ImGui::BeginPopup("wheel options", flags))
			{
				const float pixel_ratio = ImGui::GetIO().DisplaySize.y / 900;
				float image_size_modifier = 0.2 * pixel_ratio;
				configPopupOpen = true;
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
						wheelItems.emplace_back("", 5000, ImageInfoStruct());

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
					ImGui::PushID(i);

					ImGui::Text(("SLOT " + std::to_string(i + 1) + ": " + wheelItems[i].getName()).c_str());

					ImageInfoStruct* image = getImageFromRealID(wheelItems[i].realID);

					if (wheelItems[i].realID == 5000 || wheelItems[i].realID == 110000)
					{
						if (ImGui::ImageButton(0, ImVec2(image->width * image_size_modifier, image->height * image_size_modifier), image->uv0, image->uv1)) {
							GUISoundMaker9000(0x29, 0x3E8);
							ImGui::OpenPopup(("slot_modification_popup" + std::to_string(i + 1)).c_str());
						}
					}
					else {
						if (ImGui::ImageButton(image->imageptr, ImVec2(image->width * image_size_modifier, image->height * image_size_modifier), image->uv0, image->uv1)) {
							GUISoundMaker9000(0x29, 0x3E8);
							ImGui::OpenPopup(("slot_modification_popup" + std::to_string(i + 1)).c_str());
						}
					}

					
					ImVec2 baseWinPos = ImGui::GetWindowPos();
					float baseWinWidth = ImGui::GetWindowWidth();
					if (ImGui::BeginPopup(("slot_modification_popup" + std::to_string(i + 1)).c_str()))
					{
						if (!poolReduced)
						{
							for (size_t j = 5000; j < 10000; j++)
							{
								if (validateItem(j))
								{
									reducedImagePool[j] = getImageFromRealID(j);
								}
							}
							for (size_t j = 70000; j < 100000; j++)
							{
								if (validateItem(j))
								{
									reducedImagePool[j] = getImageFromRealID(j);
								}
							}

							poolReduced = true;
						}


						if (ImGui::Button("Select Combat Art...")) {
							GUISoundMaker9000(0x29, 0x3E8);
							ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y));
							ImGui::OpenPopup(("combat art popup" + std::to_string(i + 1)).c_str());
						}
						if (ImGui::BeginPopup(("combat art popup" + std::to_string(i + 1)).c_str()))
						{
							ImGui::Text("Combat Art List");

							for (int j = 5000; j < 10000; j+=100) {
								int testIconID = iconIDCalculator(j);
								if (testIconID)
								{
									std::string selectableStr = getNameFromRealID(j);
									if (selectableStr != "")
									{
										ImGui::Separator();


										ImageInfoStruct* image2 = reducedImagePool[j];
										if (j == 5000 || j == 110000)
										{
											ImGui::Image(0, ImVec2(image2->width * image_size_modifier * 0.5, image2->height * image_size_modifier * 0.5), image2->uv0, image2->uv1);
										}
										else {
											ImGui::Image(image2->imageptr, ImVec2(image2->width * image_size_modifier * 0.5, image2->height * image_size_modifier * 0.5), image2->uv0, image2->uv1);
										}
										ImGui::SameLine();
										ImVec2 text_size2 = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, selectableStr.c_str());
										if (ImGui::Selectable(selectableStr.c_str(), 0.0, NULL, ImVec2(NULL, image2->height * image_size_modifier * 0.5))) {
											wheelItems.at(i) = EquipmentStruct("", j, *getImageFromRealID(j));
											GUISoundMaker9000(0x29, 0x3E8);
										}

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
							ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), baseWinPos.y));
							ImGui::OpenPopup(("prosthetic popup" + std::to_string(i + 1)).c_str());
						}
						if (ImGui::BeginPopup(("prosthetic popup" + std::to_string(i + 1)).c_str()))
						{
							ImGui::Text("Prosthetic List");
							for (int j = 70000; j < 100000; j += 100) {
								int testIconID = iconIDCalculator(j);
								if (testIconID)
								{
									std::string selectableStr = getNameFromRealID(j);
									if (selectableStr != "")
									{
										ImGui::Separator();
										ImageInfoStruct* image2 = reducedImagePool[j];
										if (j == 5000 || j == 110000)
										{
											ImGui::Image(0, ImVec2(image2->width * image_size_modifier * 0.5, image2->height * image_size_modifier * 0.5), image2->uv0, image2->uv1);
										}
										else {
											ImGui::Image(image2->imageptr, ImVec2(image2->width * image_size_modifier * 0.5, image2->height * image_size_modifier * 0.5), image2->uv0, image2->uv1);
										}
										ImGui::SameLine();
										ImVec2 text_size2 = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, selectableStr.c_str());
										if (ImGui::Selectable(selectableStr.c_str(), 0.0, NULL, ImVec2(NULL, image2->height * image_size_modifier * 0.5))) {
											wheelItems.at(i) = EquipmentStruct("", j, *getImageFromRealID(j));
											GUISoundMaker9000(0x29, 0x3E8);
										}
									}

								}
							}

							ImGui::EndPopup();
						}

						ImGui::EndPopup();
					}

					ImGui::Separator();
					ImGui::PopID();

				}

				if (ImGui::Button("Advanced Settings")) {
					GUISoundMaker9000(0x29, 0x3E8);
					ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, 0), NULL, ImVec2(0.5f, 0.0f));

					ImGui::OpenPopup("Advanced Settings");
					configPopupOpen = true;
				}

				if (ImGui::BeginPopup("Advanced Settings", flags))
				{
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
		poolReduced = false;
	}
}