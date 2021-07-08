#pragma once
#include "Sekiro Mod.h"
#pragma warning(push, 0)        
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning(pop)


typedef char(__fastcall* _BulletBoi)(__int64 a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, int a7, __int64 a8, char a9, char a10);
_BulletBoi bulletBoi = (_BulletBoi)(0x1407D4FA0);

bool doBullet() {
	DWORD64 WorldCharManImp = *(DWORD64*)0x143D7A1E0;
	if (!WorldCharManImp)
		return 0;
	DWORD64 a = *(DWORD64*)(WorldCharManImp + 0x88);
	if (!a)
		return 0;
	DWORD64 b = a + 0x10E0;
	if (!b)
		return 0;

	printf_s("%IX\n", b);
	bulletBoi(b, 0x660C4DE, 0x5BCC, 2, 0xFFFFFFFF, 1, 0x06600101, 0xFFFFFFFE, 0xFFFFFFFE, 0);
	return 1;
}

Present oPresent; //some imgui stuff
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

bool remnantMenuActive = false;
typedef __int64(__fastcall* __ConstructsRemnantMenu)(__int64 a1, void*** a2, __int64 a3, __int64 a4, __int64 a5, char a6, __int64 a7, int a8);
__ConstructsRemnantMenu remConst = (__ConstructsRemnantMenu)(0x140C7F2F0);
__ConstructsRemnantMenu remConstOriginal = NULL;
__int64 __fastcall hookerRemnant(__int64 a1, void*** a2, __int64 a3, __int64 a4, __int64 a5, char a6, __int64 a7, int a8) {
	remnantMenuActive = true;
	return remConstOriginal(a1, a2, a3, a4, a5, a6, a7, a8);
}

typedef void* (__fastcall* __DeconstructRemnantMenu)(LPVOID lpMem, char a2);
__DeconstructRemnantMenu remDeconst = (__DeconstructRemnantMenu)(0x140CA3420);
__DeconstructRemnantMenu remDeconstOriginal = NULL;
bool skipRemnantMenu = true;

void* hookerDeremnant(LPVOID lpMem, char a2) {
	remnantMenuActive = false;
	skipRemnantMenu = true;
	return remDeconstOriginal(lpMem, a2);
}

DWORD GetAnimationID() {

	DWORD64 WorldCharManImp = *(DWORD64*)0x143D7A1E0;
	if (!WorldCharManImp)
		return 0;
	DWORD64 a = *(DWORD64*)(WorldCharManImp + 0x88);
	if (!a)
		return 0;
	DWORD64 b = *(DWORD64*)(a + 0x1FF8);
	if (!b)
		return 0;
	DWORD64 c = *(DWORD64*)(b + 0x80);
	if (!c)
		return 0;
	DWORD64 animID = *(DWORD64*)(c + 0xc8);
	if (!animID)
		return 0;

	return (DWORD) animID;
}
typedef __int64(__fastcall* _CheckIfModificationAllowed)(int slotNumArg);
_CheckIfModificationAllowed checkIfModAllowed = (_CheckIfModificationAllowed)(0x140D599D0);
_CheckIfModificationAllowed checkIfModAllowedOriginal = NULL;

__int64 hookerModAllowed(int slotNumArg) {


	__int64 allowed = checkIfModAllowedOriginal(slotNumArg);

	if (!allowed)
	{
		
		DWORD64 animID = GetAnimationID();
		
		if (animID != 0xC0E18 && animID != 0 && animID != 0xC0E04 && animID != 0x637BE54)
		{
			return 1; 
		}
		
	}

	return allowed;
}

/// <summary>
/// Gets the state of the desired gamepad. Lets me check what button the user is pressing. No analog, that's handled differently.
/// Arguments:
///		padDevicePtr: Pointer to instance of DLUID::PadDevice<DLKR::DLMultiThreadingPolicy> (I'm using gamepad 0)
///	Returns:
///		Some bitwise stuff, see definitions below. 
/// </summary>
typedef WORD (_fastcall* _GetGamePadState)(uintptr_t padDevicePtr); 
_GetGamePadState GetGamePadState = (_GetGamePadState)(0x141A8D3E0);
#define XINPUT_GAMEPAD_DPAD_UP          0x0001	// indexThingSorta: int decimal 40
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002	// indexThingSorta: int decimal 41
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004	// indexThingSorta: int decimal 42
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008	// indexThingSorta: int decimal 43
#define XINPUT_GAMEPAD_START            0x0010	// indexThingSorta: int decimal 44
#define XINPUT_GAMEPAD_BACK             0x0020	// indexThingSorta: int decimal 45
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040	// indexThingSorta: int decimal 46
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080	// indexThingSorta: int decimal 47
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100	// indexThingSorta: int decimal 48
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200	// indexThingSorta: int decimal 49
#define XINPUT_GAMEPAD_A                0x1000	// indexThingSorta: int decimal 50
#define XINPUT_GAMEPAD_B                0x2000	// indexThingSorta: int decimal 51
#define XINPUT_GAMEPAD_X                0x4000	// indexThingSorta: int decimal 52
#define XINPUT_GAMEPAD_Y                0x8000	// indexThingSorta: int decimal 53


typedef bool(__fastcall* _GamePadInputBool)(__int64 gamepadPtr, int indexThingSorta);
_GamePadInputBool gamePadInputBool = (_GamePadInputBool)(0x141A8CBE0);
_GamePadInputBool gamePadInputBool2 = NULL;

bool showConfigureArts = false;
int backfireCounter = 0;
bool __fastcall hookerInput(__int64 gamepadPtr, int indexThingSorta) {

	if (indexThingSorta == 45) { //XINPUT_GAMEPAD_BACK
		if (skipRemnantMenu) //if I want to override the remnant menu.
		{
			return false;
		}
		else //if I want to force the remnant menu open. 
		{
			showConfigureArts = false;

			if (remnantMenuActive == false)
			{
				if (backfireCounter < 10)
				{
					backfireCounter++;
					printf_s("returned true for xinput back\n");
					return true;
				}
				else {
					skipRemnantMenu = true;
					backfireCounter = 0;
					return false;
				}
			}
			else
			{
				return false;//gamePadInputBool2(gamepadPtr, indexThingSorta);
			}
			
			
		}
		
	}

	if (!skipRemnantMenu && indexThingSorta == 50 && !remnantMenuActive )
	{
		return false;
	}
	if (showConfigureArts)
		return false;


	return gamePadInputBool2(gamepadPtr, indexThingSorta);

}


/// <summary>
/// Equips the combat art and its icon. May be used for something else too, like prosthetics, I forgot.  
/// Arguments: 
///		equipGameDataPtr: pointer to EquipGameData
///		slotNum: slot number ( 1 for combat arts)
///		equipIDPtr: pointer to the working equipID of the combat art (0x808000...)
///		iconID: id of the icon associated with the combat art
///		stack1: always 1 ( at least for combat arts)
///		stack2: always 1 ( at least for combat arts)
///		stack3: always 0 ( at least for combat arts)
/// Returns:
///		Nothing
/// </summary>
/// 
typedef void(_fastcall* _EquipArt)(uintptr_t equipGameDataPtr, DWORD slotNum, DWORD* equipIDPtr, DWORD iconID, byte stack1, byte stack2, byte stack3 ); //lets me equip an art
_EquipArt EquipArt = (_EquipArt)(0x14079F8A0);

typedef __int64 (__fastcall* _EquipArt2)(unsigned int slotNumArg, __int64 someStupidFuckingBullshitEh, char ForceEquipArg);
_EquipArt2 EquipArt2 = (_EquipArt2)(0x140D592F0);

/// <summary>
/// Can recreate any sonud that's used by the in-game gui.
///	
/// Arguments:
///		Significance unknown, but mappings below:
///						         a1,  a2
///		Pause Menu Open :		 29, 7530
///		Pause Menu Close :		 29, 7594
///		Side Menu Switch :		 29, 1F4
///		Open Sublist :			 29, 3E8
///		Sublist Select / Close : 29, 3FC
///		Open Options :			 29, 64
///		Modify Options :		 29, 2BC
///		Open Popup Option :		 29, 0
///		Can't Open:				 29, 258
///		Highlight / Item Switch: 2B, 2710 (yes 2b)
/// 
/// Returns:
///		Nothing
/// </summary>
typedef void(_fastcall* _GUISoundMaker9000)(signed int a1, int a2);
_GUISoundMaker9000 GUISoundMaker9000 = (_GUISoundMaker9000)(0x1408CE960);


struct CombatArtStruct {
	std::string artName; // Ex: "Whirlwind Slash"
	int realID;     // The ID of the combat art that is consistent between games/saves. 
	int equipID;    // The ID of the combat art that is actually used to commit equips. RealID must be used to find equipID. 
	int iconID;     // The ID of the icon for the combat art. The function takes it in separately. RealID must also be used to find this. 
	ID3D11ShaderResourceView* texture = NULL;
};
CombatArtStruct* combatArts;
 


DWORD lastArt = 0; 
void artSearch() {

	for (size_t i = 0; i < 19; i++)
	{
		CombatArtStruct* currentArt = &combatArts[i];
		currentArt->equipID = memoryScan(currentArt->realID, false, 0x1710 );
		currentArt->iconID = memoryScan(currentArt->realID, true, 0x1710);
		
	}


}

void printArts(CombatArtStruct* printArray) {

	for (size_t i = 0; i < 19; i++)
	{
		CombatArtStruct currentArt = printArray[i];

		printf_s("Art: %s\n", currentArt.artName.c_str());
		printf_s("\trealID:  %ld\n", currentArt.realID);
		printf_s("\tequipID: 0x%lX\n", currentArt.equipID);
		printf_s("\ticonID:  %ld\n\n", currentArt.iconID);
	}
}

DWORD memoryScan(DWORD ID, bool icon, size_t maxOffset) {

	DWORD64 playerGameData = GetPlayerGameDataPtr();
	if (!playerGameData)
		return 0;

	DWORD64 inventory;
	if (icon)
		inventory = *(DWORD64*)(playerGameData + 0x458);
	else
		inventory = *(DWORD64*)(playerGameData + 0x3F8);

	if (!inventory)
		return 0;

	DWORD64 IconArea = *(DWORD64*)(playerGameData + 0x458);
	if (!IconArea)
		return 0;
	
	
	DWORD64 inventoryShifter = inventory;
	DWORD64 adjacentShifter = inventoryShifter;
	DWORD currentValue; 
	DWORD adjacentValue;

	size_t scanSize = maxOffset / 4;
	for (size_t i = 0; i < scanSize; i++)
	{
		if (icon)
			adjacentShifter = inventoryShifter + 0x8;
		else
			adjacentShifter = inventoryShifter;

		inventoryShifter =inventoryShifter + 0x4;


		if (inventoryShifter) {

			currentValue = *(DWORD*)inventoryShifter;

			if (currentValue) {
				if (adjacentShifter)
				{
					adjacentValue = *(DWORD*)adjacentShifter;

					if (adjacentValue) {

						if ( !icon && currentValue == ID && (adjacentValue > 0x80800000 && adjacentValue < 0x80800100))
						{
							return adjacentValue;
						}

						
						if ( icon && currentValue == ID) //it turns out shadowrush and nightjarslash reversal (6000 and 7000) are fucked and I gotta determine their ID by looking at the values around them.
						{
						    if (adjacentValue > 0x000 && adjacentValue < 0xFFF)
								return adjacentValue;

							adjacentShifter = adjacentShifter - 0x8;
							adjacentValue = *(DWORD*)adjacentShifter;

							if (adjacentValue)
							{
								if (adjacentValue > 0x000 && adjacentValue < 0xFFF)
								{
									return adjacentValue + 1; //hopefully this stupid wonky filtering works. 
								}
							}

						}


					}
				}

			}

		}
	
	}
	return 0;
}

DWORD64 GetGamePad0Ptr() {
	DWORD64 inputStructOrSomething = *(DWORD64*)0x143F42850;
	if (!inputStructOrSomething)
		return 0;
	
	DWORD64 inputs0 = *(DWORD64*)(inputStructOrSomething + 0x18);
	if (!inputs0)
		return 0;

	DWORD64 gamepad0 = *(DWORD64*)(inputs0 + 0x10);
	if (!gamepad0)
		return 0;

	return gamepad0;
}
DWORD64 GetPlayerGameDataPtr() {

	DWORD64 GameData = *(DWORD64*)0x143D5AAC0;
	if (!GameData) 
		return 0;

	DWORD64 PlayerGameData = *(DWORD64*)(GameData + 0x8);
	if (!PlayerGameData)
		return 0;

	return PlayerGameData;
}


bool equipCombatArt(DWORD artRealID) {
	artSearch();

	int i = 0;
	for (i = 0; i < 20 ; i++)
	{
		if (combatArts[i].realID == artRealID) {
			break;
		}
	}
	DWORD64 playerGameDataPtr = GetPlayerGameDataPtr();
	CombatArtStruct currentArt = combatArts[i];

	if (playerGameDataPtr)
	{

		DWORD64 equipGameDataPtr = playerGameDataPtr + 0x278;
		//EquipArt(equipGameDataPtr, 1, &currentArt.equipID, currentArt.iconID, (byte)1, (byte)1, (byte)0);
		int icon[15] = { 0,0,0,0,0,0,0,0,0,0,0,0,0, 0,  currentArt.iconID };
		if (EquipArt2(1, (__int64)&icon, 0))
			return true;
		return false;
	}

	return false;
}

CombatArtStruct* getEquippedArt() {
	DWORD64 playerGameData = GetPlayerGameDataPtr();
	if (!playerGameData)
		return 0;
	DWORD currentValue = *(DWORD*)(playerGameData + 0x2A0);

	artSearch();
	for (size_t i = 0; i < 19; i++)
	{
		if (currentValue == combatArts[i].iconID)
		{
			return &combatArts[i];
		}
	}

	return &combatArts[19];
}


void disablePartController(bool disable) {
	DWORD curProtection;
	DWORD64 address = 0x141A8DAA6;
	VirtualProtect((LPVOID)address, 7, PAGE_EXECUTE_READWRITE, &curProtection);
	if (address)
	{
		if (disable)
			*(DWORD64*)address = 0x90909090909090;
		else
			*(DWORD64*)address = 0x66898740020000;
								 
		DWORD temp;
		VirtualProtect((LPVOID)address, 7, curProtection, &temp);
	}

}

void patchBusyCheck(bool disable) {
	DWORD curProtection;
	DWORD64 address = 0x140D59320;
	VirtualProtect((LPVOID)address, 8, PAGE_EXECUTE_READWRITE, &curProtection);
	if (address)
	{
		if (disable)
			*(DWORD64*)address = 0x75C0849090909090;
		else
			*(DWORD64*)address = 0x75C084000006ABE8; //this is probably wrong... flip everything endian after the 75. 

		DWORD temp;
		VirtualProtect((LPVOID)address, 8, curProtection, &temp);
	}

}

// Simple helper function to load an image into a DX11 texture with common settings
bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	if (pTexture) //remove conditional if breaks.
	{
		pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);

	}
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int my_image_width;
int my_image_height;
ID3D11ShaderResourceView* texture = NULL;
void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
	my_image_width = 0;
	my_image_height = 0;
	
	for (size_t i = 0; i < 20; i++)
	{
		CombatArtStruct* currentArt = &combatArts[i];
		std::string path = "CombatLinkerResources/" + std::to_string(currentArt->realID) + ".png";
		//printf_s((path + "\n").c_str());
		bool ret = LoadTextureFromFile(path.c_str(), &currentArt->texture, &my_image_width, &my_image_height);
		IM_ASSERT(ret);

	}

	bool ret = LoadTextureFromFile("CombatLinkerResources/test.png", &texture, &my_image_width, &my_image_height);
	IM_ASSERT(ret);

	//ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("CombatLinkerResources/SekiroOnlineFont.ttf", 20);
}

bool init = false;


LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	/**/
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;



	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


static int artMagazine[] = { 5500, 6000, 7700 };
static int magazineIndex = 0;
static int lastArtMagazine[] = { 5500, 6000, 7700 };
static bool artToggle = false;
clock_t start = 0;
float alpha = 0;
bool controllerArtToggle = false;

WORD currentState = 0;
WORD lastState = 0;
bool initialized = false;
ImVec2 backgroundSize;
ImVec2 backgroundLoc;

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
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
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	if (*(DWORD64*)0x143D7A1E0) //if world loaded in. 
	{
		
		if (!initialized)
		{
			MH_CreateHook((LPVOID)gamePadInputBool, (LPVOID)&hookerInput, reinterpret_cast<LPVOID*>(&gamePadInputBool2));
			MH_CreateHook((LPVOID)remConst, (LPVOID)&hookerRemnant, reinterpret_cast<LPVOID*>(&remConstOriginal));
			MH_CreateHook((LPVOID)remDeconst, (LPVOID)&hookerDeremnant, reinterpret_cast<LPVOID*>(&remDeconstOriginal));
			MH_CreateHook((LPVOID)checkIfModAllowed, (LPVOID)&hookerModAllowed, reinterpret_cast<LPVOID*>(&checkIfModAllowedOriginal));

			MH_EnableHook((LPVOID)gamePadInputBool);
			MH_EnableHook((LPVOID)remConst);
			MH_EnableHook((LPVOID)remDeconst);
			MH_EnableHook((LPVOID)checkIfModAllowed);
			backgroundSize = ImVec2(1, 1);
			backgroundLoc = ImVec2(io.DisplaySize.x * 2, io.DisplaySize.y * 2);

			initialized = true;
		}


	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {

		//showConfigureArts = !showConfigureArts;
	}

	/*
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
	{
		//MH_DisableHook((LPVOID)0x141A8CBE0);
		equipCombatArt(art0);
		artToggle = true;

	}
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
	{

		equipCombatArt(art1);
		artToggle = false;
	}
	*/

	DWORD64 gamePad0Ptr = GetGamePad0Ptr();
	if (gamePad0Ptr && initialized)
	{

		currentState = *(WORD*)(gamePad0Ptr + 0x240);

		if ((currentState & XINPUT_GAMEPAD_X) != 0) { //delete this condition entirely later. just for memez.
			doBullet();
		}

		if ((currentState & XINPUT_GAMEPAD_X) != 0 && (currentState & XINPUT_GAMEPAD_X) != (lastState & XINPUT_GAMEPAD_X)) {

			
			controllerArtToggle = true;
			
		}
		if ((currentState & XINPUT_GAMEPAD_BACK) != 0 && (currentState & XINPUT_GAMEPAD_BACK) != (lastState & XINPUT_GAMEPAD_BACK) && !remnantMenuActive)
		{
			if (!showConfigureArts)
			{
				GUISoundMaker9000(0x29, 0x3E8);
			}
			else
			{
				GUISoundMaker9000(0x29, 0x3FC);
			}
			showConfigureArts = !showConfigureArts;
		}
	}
	lastState = currentState; //used so button gets triggered only once per press




	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) || controllerArtToggle)
	{	
		controllerArtToggle = false;
		bool success = equipCombatArt(artMagazine[(magazineIndex+1)%3]);
		if (success)
		{
			magazineIndex = (magazineIndex + 1) % 3;
		}
	}

	//ART CONFIGURE GUI
	if (showConfigureArts && initialized)
	{
		int artSelection = -1;
		//ImGui::ShowDemoWindow();
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Art Selector", NULL, flags);
		//ImGui::CaptureMouseFromApp(true);

		if (ImGui::BeginTable("table1", 3))
		{
			ImGui::TableSetupColumn("Art 1");
			ImGui::TableSetupColumn("Art 2");
			ImGui::TableSetupColumn("Art 3");


			static bool column_selected[3] = {};
			// Instead of calling TableHeadersRow() we'll submit custom headers ourselves
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			for (int column = 0; column < 3; column++)
			{
				ImGui::TableSetColumnIndex(column);
				const char* column_name = ImGui::TableGetColumnName(column); // Retrieve name passed to TableSetupColumn()
				ImGui::PushID(column);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PopStyleVar();
				ImGui::TableHeader(column_name);
				ImGui::PopID();
			}

			for (int row = 0; row < 20; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 3; column++)
				{
					ImGui::TableSetColumnIndex(column);
					if (column == 0)
					{
						ImGui::RadioButton(combatArts[row].artName.c_str(), &artMagazine[0], combatArts[row].realID);
					}
					else if (column == 1)
					{
						std::string str = combatArts[row].artName + " ";
						ImGui::RadioButton(str.c_str(), &artMagazine[1], combatArts[row].realID);
					}
					else if (column == 2) {
						std::string str = combatArts[row].artName + "  ";
						ImGui::RadioButton(str.c_str(), &artMagazine[2], combatArts[row].realID);
					}
				}
			}
			ImGui::EndTable();
		}


		for (size_t i = 0; i < 3; i++)
		{
			if (lastArtMagazine[i] != artMagazine[i])
			{
				GUISoundMaker9000(0x29, 0x2BC);
				lastArtMagazine[i] = artMagazine[i];
			}
		}


		if (ImGui::Button("Open Remnant Menu"))
			skipRemnantMenu = false;
		if (ImGui::Button("Patch Active Check"))
			patchBusyCheck(true);
		if (ImGui::Button("Unpatch Active Check"))
			patchBusyCheck(false);
		ImGui::SetWindowPos(ImVec2(io.DisplaySize.x/2 - ImGui::GetWindowWidth()/2, 0));
		ImGui::End();

	}


	




	//COMBAT ART IMAGE + TEXT
	CombatArtStruct* equippedArt = getEquippedArt();
	float guiAlpha = *(float*)0x143D8BB18;

	if (equippedArt && initialized)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::Begin("Blah", NULL, flags);

		if (equippedArt->realID != lastArt)	{			
			GUISoundMaker9000(0x2b, 0x2710);
			lastArt = equippedArt->realID;	
			start = clock();
		}


		auto posX = (ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - ImGui::CalcTextSize(equippedArt->artName.c_str()).x //right align the text so it's not jittery.
			- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
		if (posX > ImGui::GetCursorPosX())
			ImGui::SetCursorPosX(posX);

		double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
		if (duration < 1.5)
		{
			if (alpha < 1)
			{
				alpha += 0.15f;

			}
			ImGui::TextColored(ImVec4(1, 1, 1, alpha), equippedArt->artName.c_str());
		}
		else {
			if (alpha > 0)
			{
				alpha -= 0.15f;
			}

			ImGui::TextColored(ImVec4(1, 1, 1, alpha), equippedArt->artName.c_str());

		}
		ImGui::SetWindowPos(ImVec2(backgroundLoc.x - ImGui::GetWindowWidth() * 0.9f, backgroundLoc.y + (backgroundSize.y * 0.2f)), 0);
		ImGui::End();

		//ART
		float artAlpha;
		DWORD animID = GetAnimationID();
		//printf_s("%IX\n", animID);
		if (animID == 0xC0E18)
			artAlpha = 0.5f;
		else
			artAlpha = 1;
		if (guiAlpha != -1)
		{
			artAlpha = -guiAlpha;
		}
		ImGui::Begin("Blah4", NULL, flags);
		ImGui::SameLine(); 
		if(equippedArt->realID != 0) ImGui::Image((void*)equippedArt->texture, ImVec2(my_image_width * 0.5f, my_image_height * 0.5f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, artAlpha)); //artAlpha was original
		ImGui::SetWindowPos(ImVec2(backgroundLoc.x + (backgroundSize.x * 0.166f), backgroundLoc.y + (backgroundSize.y * 0.22f)), 0); //trying to center combat art inside background image.
		ImGui::End();
		
	}

	//BACKGROUND IMAGE
	if (initialized)
	{
		float artAlpha2 = 1;
		if (guiAlpha != -1)
		{
			artAlpha2 = -guiAlpha;
		}
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::Begin("Blah3", NULL, flags);
		ImGui::Image((void*)texture, ImVec2(my_image_width * 0.9f, my_image_height * 0.9f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, artAlpha2));
		backgroundSize = ImGui::GetWindowSize();
		ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - backgroundSize.x, 0));
		backgroundLoc = ImGui::GetWindowPos();
		ImGui::End();
	}



	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}


void structInitializer() {
	combatArts = new CombatArtStruct[20];

	if (true)
	{
		combatArts[0].artName = "Whirlwind Slash"; //GUI order, not ID order
		combatArts[0].realID = 5100;
		combatArts[1].artName = "NightJar Slash";
		combatArts[1].realID = 5200;
		combatArts[2].artName = "Nightjar Slash Reversal";
		combatArts[2].realID = 7000;
		combatArts[3].artName = "Ichimonji";
		combatArts[3].realID = 5300;
		combatArts[4].artName = "Ichimonji Double";
		combatArts[4].realID = 7100;
		combatArts[5].artName = "Ashina Cross";
		combatArts[5].realID = 5500;
		combatArts[6].artName = "Praying Strikes";
		combatArts[6].realID = 5900;
		combatArts[7].artName = "Praying Strikes - Exorcism";
		combatArts[7].realID = 7500;
		combatArts[8].artName = "Senpou Leaping Kicks";
		combatArts[8].realID = 5800;
		combatArts[9].artName = "High Monk";
		combatArts[9].realID = 7400;
		combatArts[10].artName = "Shadow Rush";
		combatArts[10].realID = 6000;
		combatArts[11].artName = "Shadow Fall";
		combatArts[11].realID = 7600;
		combatArts[12].artName = "Dragon Flash";
		combatArts[12].realID = 5400;
		combatArts[13].artName = "One Mind";
		combatArts[13].realID = 6100;
		combatArts[14].artName = "Floating Passage";
		combatArts[14].realID = 5600;
		combatArts[15].artName = "Spiral Cloud Passage";
		combatArts[15].realID = 7200;
		combatArts[16].artName = "Sakura Dance";
		combatArts[16].realID = 7700;
		combatArts[17].artName = "Mortal Draw";
		combatArts[17].realID = 5700;
		combatArts[18].artName = "Empowered Mortal Draw";
		combatArts[18].realID = 7300;

		combatArts[19].artName = "None"; //should so some testing. Guessing might be hardcoded but ya never know. 
		combatArts[19].realID = 0;
		combatArts[19].iconID = 256;
		combatArts[19].equipID = 0x80800020;

	}

}

/*
int main()
{
	//Start Console
	CreateConsole();

	//Vars
	bool programActive = true;
	DWORD64 procModule = NULL;

	do
	{
		procModule = (DWORD64)GetModuleHandleA("sekiro.exe");
	} while (procModule == NULL);

	//Main Loop  Use existing code to console type switching before attempting more complex imgui stuff?
	while (programActive)
	{

		//Prepare Input
		int userInput;
		//char charInput;

		printf_s("1. Sekiro Test\n"
			"2. Equip Whirlwind Slash\n"
			"3. Equip Nightjar Slash\n"
			"4. Equip High Monk\n"
			"10. Exit Program\n");



		//Validate Input
		if (!inputHandler(&userInput))
		{
			do
			{
				system("cls");
				printf_s("Error! Invalid Input Was Detected, Please Reenter Choice:\n"
					"1. Sekiro Test\n"
					"2. Equip Whirlwind Slash\n"
					"3. Equip Nightjar Slash\n"
					"4. Equip High Monk\n"
					"10. Exit Program\n");
			} while (!inputHandler(&userInput));
		}

		system("cls");
		DWORD64 playerGameDataPtr = GetPlayerGameDataPtr();

		switch (userInput)
		{

		case 1:
			printf_s("Sekiro Test: \n");

			DWORD64 tempPtr;
			DWORD sen;

			tempPtr = *(DWORD64*)(procModule + 0x3D7A1E0);
			if (tempPtr != NULL)
			{
				tempPtr = *(DWORD64*)(tempPtr + 0x88);

				if (tempPtr != NULL)
				{
					tempPtr = *(DWORD64*)(tempPtr + 0x2000);

					if (tempPtr != NULL)
					{
						sen = *(DWORD*)(tempPtr + 0x7C);
					}
				}

			}

			//tempPtr = *(DWORD64*)(procModule + 0x7C);




			if (sen != NULL) {
				printf_s("Your Sen is (%ld)\n\n", sen);

			}
			else {
				printf_s("Pointer Failed.");
			}

			artSearch();
			printArts(combatArts);

			system("pause");

			break;

		case 2:
			equipCombatArt(5100);
			break;
		case 3:
			equipCombatArt(5200);
			break;
		case 4:
			equipCombatArt(7400);
			break;
		case 10:		//Exit Program
			programActive = false;
			break;

		default:
			printf_s("Invalid Menu Choice! Please Enter \"1\" or \"2\" \n");
			system("pause");
		}

		system("cls");

	}

	printf_s("Dll Exited Successfully! You May Now Close This Console Window.");

	FreeConsole();

	return 0;
}


BOOL inputHandler(int* varAddr)
{
	std::cin >> *varAddr;

	//If Invalid Input
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');

		return FALSE;
	}

	return TRUE;
}
*/



