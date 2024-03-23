#pragma once
#include "Functions.h"
#include <stdio.h>
#include <wtypes.h>
#include <d3d11.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "../Graphics/stb_image.h"


_GUISoundMaker9000 GUISoundMaker9000 = (_GUISoundMaker9000)(0x1408CE960);
_EquipArt2 EquipItem = (_EquipArt2)(0x140D592F0);
__ActionRequest10 requestAction10 = (__ActionRequest10)(0x140B2BC50);
__getIconID getIconID = (__getIconID)(0x140C3D680);
__namePtrRetriever namePtrRetriever = (__namePtrRetriever)(0x1410AA340);

std::string getNameFromRealID(int realID) {

    if (realID == 5000 || realID == 110000) //returns <?null?> so override with "Empty"
    {
        return "Empty";
    }
    uint64_t MsgRepositoryImp = *(uint64_t*)0x143D964C8;
    if (!MsgRepositoryImp)
        return 0;
    
    uint64_t txtPtr = namePtrRetriever(MsgRepositoryImp, realID);
    if (txtPtr == 0)
    {
        return "";
    }
    std::wstring string_to_convert = std::wstring((const wchar_t*) txtPtr);

    if (string_to_convert.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &string_to_convert[0], (int)string_to_convert.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &string_to_convert[0], (int)string_to_convert.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

bool validateItem(int realID) {
    if (getNameFromRealID(realID) != "")
    {
        return true;
    }
    else {
        return false;
    }
}
bool operator!=(const EquipmentStruct& a, const EquipmentStruct& b) {
    return a.realID != b.realID;
}
bool popupOpen = false;

uint64_t GetPlayerGameDataPtr() {

    uint64_t GameData = *(uint64_t*)0x143D5AAC0;
    if (!GameData)
        return 0;

    uint64_t PlayerGameData = *(uint64_t*)(GameData + 0x8);
    if (!PlayerGameData)
        return 0;

    return PlayerGameData;
}

uint64_t GetEquipInventoryDataPtr() {

    uint64_t PlayerGameData = GetPlayerGameDataPtr();
    if (!PlayerGameData)
        return 0;

    uint64_t EquipInventoryData = *(uint64_t*)(PlayerGameData + 0x5B0);
    if (!EquipInventoryData)
        return 0;
    
    return EquipInventoryData;
}

uint64_t GetIconAreaPtr() {
    uint64_t PlayerGameData = GetPlayerGameDataPtr();
    if (!PlayerGameData)
        return 0;

    uint64_t iconArea = *(uint64_t*)(PlayerGameData + 0x458);
    if (!iconArea)
        return 0;

    return iconArea;
}

uint64_t GetPlayerInstancePtr() {

    uint64_t WorldChrMan = *(uint64_t*)0x143D7A1E0;
    if (!WorldChrMan)
        return 0;

    uint64_t PlayerInstance = *(uint64_t*)(WorldChrMan + 0x88);
    if (!PlayerInstance)
        return 0;

    return PlayerInstance;
}

uint64_t GetActionRequestModule() {
    uint64_t PlayerInstance = GetPlayerInstancePtr();
    if (!PlayerInstance)
        return 0;

    uint64_t CharModules = *(uint64_t*)(PlayerInstance + 0x1ff8);
    if (!CharModules)
        return 0;

    uint64_t ActionRequest = *(uint64_t*)(CharModules + 0x80);
    if (!ActionRequest)
        return 0;

    return ActionRequest;
}

bool loadedIn() {
    uint64_t MenuMan = *(uint64_t*)0x143D67408;
    if (!MenuMan)
        return 0;

    uint64_t WorldCharManImp = *(uint64_t*)0x143D7A1E0;

    return *(bool*)(MenuMan + 0x248) && WorldCharManImp;

}

int getGameMenuState() {
    uint64_t MenuMan = *(uint64_t*)0x143D67408;
    if (!MenuMan)
        return 0;

    return *(int*)(MenuMan + 0x40);;

}

int getOtherMenuStates() {
    uint64_t MenuMan = *(uint64_t*)0x143D67408;
    if (!MenuMan)
        return 0;

    int value = *(int*)(MenuMan + 0x3B8);
    value |= *(int*)(0x143D89904);
    return value;
}

unsigned int iconIDCalculator(u_int realID) {
    uint64_t EquipInventoryDataPtr = GetEquipInventoryDataPtr();
    if (!EquipInventoryDataPtr)
        return 0;

    unsigned int iconID = getIconID(EquipInventoryDataPtr+0x10, &realID);

    if (!iconID || iconID > 0xFFFF)
    {
        return 0;
    }

    return iconID;
}

//EquipmentStruct* CombatArts;
//EquipmentStruct* Prosthetics;
std::vector<EquipmentStruct> CombatArts;
std::vector<EquipmentStruct> Prosthetics;
std::unordered_map<int, ID3D11ShaderResourceView*> imageMap;
void structInitializer() {

    namespace fs = std::filesystem;

        fs::path path = fs::current_path();
        path /= "WeaponWheelResources";
        for (const auto& entry : fs::directory_iterator(path)) {

            if (entry.path().extension() == ".png")
            {
                
                std::string str = entry.path().filename().string();
                auto start_position_to_erase = str.find(".png");
                str.erase(start_position_to_erase, 4);
                int processedID = stoi(str);
                imageMap[processedID] = NULL;
                std::cout << str << std::endl;
            }

        }

        

#if 0
    //Combat Arts
    CombatArts.emplace_back("Empty", 5000);
    CombatArts.emplace_back("Whirlwind Slash", 5100);

    CombatArts.emplace_back("Nightjar Slash", 5200);
    CombatArts.emplace_back("Nightjar Slash Reversal", 7000);

    CombatArts.emplace_back("Ichimonji", 5300);
    CombatArts.emplace_back("Ichimonji Double", 7100);

    CombatArts.emplace_back("Ashina Cross", 5500);

    CombatArts.emplace_back("Praying Strikes", 5900);
    CombatArts.emplace_back("Praying Strikes Exorcism", 7500);

    CombatArts.emplace_back("Senpou Leaping Kicks", 5800);
    CombatArts.emplace_back("High Monk", 7400);

    CombatArts.emplace_back("Shadow Rush", 6000);
    CombatArts.emplace_back("Shadow Fall", 7600);

    CombatArts.emplace_back("Dragon Flash", 5400);

    CombatArts.emplace_back("One Mind", 6100);

    CombatArts.emplace_back("Floating Passage", 5600);
    CombatArts.emplace_back("Spiral Cloud Passage", 7200);

    CombatArts.emplace_back("Sakura Dance", 7700);

    CombatArts.emplace_back("Mortal Draw", 5700);
    CombatArts.emplace_back("Empowered Mortal Draw", 7300);

    //Prosthetics
    Prosthetics.emplace_back("Loaded Shuriken", 70000);
    Prosthetics.emplace_back("Spinning Shuriken", 70100);
    Prosthetics.emplace_back("Gouging Top", 70200);
    Prosthetics.emplace_back("Phanrom Kunai", 70300);
    Prosthetics.emplace_back("Sen Throw", 70400);
    Prosthetics.emplace_back("Lazulite Suriken", 70500);

    Prosthetics.emplace_back("Loaded Axe", 73000);
    Prosthetics.emplace_back("Spring-load Axe", 73100);
    Prosthetics.emplace_back("Sparking Axe", 73200);
    Prosthetics.emplace_back("Lazulite Axe", 73300);

    Prosthetics.emplace_back("Loaded Spear", 78000);
    Prosthetics.emplace_back("Loaded Spear Thrust", 78100);
    Prosthetics.emplace_back("Loaded Spear Cleave", 78200);
    Prosthetics.emplace_back("Spiral Spear", 78300);
    Prosthetics.emplace_back("Leaping Flame", 78400);

    Prosthetics.emplace_back("Sabimaru", 75000);
    Prosthetics.emplace_back("Improved Sabimaru", 75100);
    Prosthetics.emplace_back("Piercing Sabimaru", 75200);
    Prosthetics.emplace_back("Lazulite Sabimaru", 75300);

    Prosthetics.emplace_back("Loaded Umbrella", 76000);
    Prosthetics.emplace_back("Loaded Umbrella Magnet", 76100);
    Prosthetics.emplace_back("Phoenix Lilac Umbrella", 76300);
    Prosthetics.emplace_back("Suzaku Lotus Umbrella", 76200);

    Prosthetics.emplace_back("Shinobi Firecracker", 71000);
    Prosthetics.emplace_back("Spring-load Firecracker", 71100);
    Prosthetics.emplace_back("Long Spark", 71200);
    Prosthetics.emplace_back("Purple Fume Spark", 71300);

    Prosthetics.emplace_back("Flame Vent", 72000);
    Prosthetics.emplace_back("Spring-load Flame Vent", 72100);
    Prosthetics.emplace_back("Okinaga's Flame Vent", 72200);
    Prosthetics.emplace_back("Lazulite Sacred Flame", 72300);

    Prosthetics.emplace_back("Mist Raven", 74000);
    Prosthetics.emplace_back("Aged Mist Raven", 74100);
    Prosthetics.emplace_back("Great Mist Raven", 74200);

    Prosthetics.emplace_back("Divine Abduction", 77000);
    Prosthetics.emplace_back("Double Divine Abduction", 77100);
    Prosthetics.emplace_back("Golden Vortex", 77200);

    Prosthetics.emplace_back("Finger Whistle", 79000);
    Prosthetics.emplace_back("Mountain Echo", 79100);
    Prosthetics.emplace_back("Malcontent", 79200);

    printf("%llx\n", &CombatArts);
#endif
#if 0
    if (true)
    {
       CombatArts[0].Name = "Whirlwind Slash"; //GUI order, not ID order
       CombatArts[0].realID = 5100;
       CombatArts[1].Name = "NightJar Slash";
       CombatArts[1].realID = 5200;
       CombatArts[2].Name = "Nightjar Slash Reversal";
       CombatArts[2].realID = 7000;
       CombatArts[3].Name = "Ichimonji";
       CombatArts[3].realID = 5300;
       CombatArts[4].Name = "Ichimonji Double";
       CombatArts[4].realID = 7100;
       CombatArts[5].Name = "Ashina Cross";
       CombatArts[5].realID = 5500;
       CombatArts[6].Name = "Praying Strikes";
       CombatArts[6].realID = 5900;
       CombatArts[7].Name = "Praying Strikes - Exorcism";
       CombatArts[7].realID = 7500;
       CombatArts[8].Name = "Senpou Leaping Kicks";
       CombatArts[8].realID = 5800;
       CombatArts[9].Name = "High Monk";
       CombatArts[9].realID = 7400;
       CombatArts[10].Name = "Shadow Rush";
       CombatArts[10].realID = 6000;
       CombatArts[11].Name = "Shadow Fall";
       CombatArts[11].realID = 7600;
       CombatArts[12].Name = "Dragon Flash";
       CombatArts[12].realID = 5400;
       CombatArts[13].Name = "One Mind";
       CombatArts[13].realID = 6100;
       CombatArts[14].Name = "Floating Passage";
       CombatArts[14].realID = 5600;
       CombatArts[15].Name = "Spiral Cloud Passage";
       CombatArts[15].realID = 7200;
       CombatArts[16].Name = "Sakura Dance";
       CombatArts[16].realID = 7700;
       CombatArts[17].Name = "Mortal Draw";
       CombatArts[17].realID = 5700;
       CombatArts[18].Name = "Empowered Mortal Draw";
       CombatArts[18].realID = 7300;
       CombatArts[19].Name = "None"; //should so some testing. Guessing might be hardcoded but ya never know. 
       CombatArts[19].realID = 0;

       Prosthetics[0].Name = "Loaded Shuriken"; //GUI order, not ID order
       Prosthetics[0].realID = 70000;
       Prosthetics[1].Name = "NightJar Slash";
       Prosthetics[1].realID = 5200;
       Prosthetics[2].Name = "Nightjar Slash Reversal";
       Prosthetics[2].realID = 7000;
       Prosthetics[3].Name = "Ichimonji";
       Prosthetics[3].realID = 5300;
       Prosthetics[4].Name = "Ichimonji Double";
       Prosthetics[4].realID = 7100;
       Prosthetics[5].Name = "Ashina Cross";
       Prosthetics[5].realID = 5500;
       Prosthetics[6].Name = "Praying Strikes";
       Prosthetics[6].realID = 5900;
       Prosthetics[7].Name = "Praying Strikes - Exorcism";
       Prosthetics[7].realID = 7500;
       Prosthetics[8].Name = "Senpou Leaping Kicks";
       Prosthetics[8].realID = 5800;
       Prosthetics[9].Name = "High Monk";
       Prosthetics[9].realID = 7400;
       Prosthetics[10].Name = "Shadow Rush";
       Prosthetics[10].realID = 6000;
       Prosthetics[11].Name = "Shadow Fall";
       Prosthetics[11].realID = 7600;
       Prosthetics[12].Name = "Dragon Flash";
       Prosthetics[12].realID = 5400;
       Prosthetics[13].Name = "One Mind";
       Prosthetics[13].realID = 6100;
       Prosthetics[14].Name = "Floating Passage";
       Prosthetics[14].realID = 5600;
       Prosthetics[15].Name = "Spiral Cloud Passage";
       Prosthetics[15].realID = 7200;
       Prosthetics[16].Name = "Sakura Dance";
       Prosthetics[16].realID = 7700;
       Prosthetics[17].Name = "Mortal Draw";
       Prosthetics[17].realID = 5700;
       Prosthetics[18].Name = "Empowered Mortal Draw";
       Prosthetics[18].realID = 7300;
       Prosthetics[19].Name = "None"; //should so some testing. Guessing might be hardcoded but ya never know. 
       Prosthetics[19].realID = 0;
    }
#endif


}



bool pleaseSwitch = false;
int originalActiveSlot = -1;
bool restored = true;
int tempID = 0;
int counter;
bool attemptEquip(DWORD realID) {
    printf("attemptEquip, Beginning: Attempting (%d)\n", realID);
    int iconID = iconIDCalculator(realID);
    int icon[15] = { 0,0,0,0,0,0,0,0,0,0,0,0,0, 0,  iconID };
    uint64_t actionRequestModule = GetActionRequestModule();
    if (!actionRequestModule)
        return 0;

    if (iconID == getCurrentEquippedIconID(COMBAT_ART_SLOT))
    {
        printf("\tCombatArt Already Equipped\n");
        return true;
    }
    int force = (realID != 5000);
    bool success = EquipItem(COMBAT_ART_SLOT, (__int64)&icon, force);
    return success;
}

int getCurrentEquippedIconID(int slot) {

    int offset;
    switch (slot)
    {
    case PROSTHETIC_SLOT_1:
        offset = 0x29C;
        break;
    case COMBAT_ART_SLOT:
        offset = 0x2A0;
        break;
    case PROSTHETIC_SLOT_2:
        offset = 0x2A4;
        break;
    case PROSTHETIC_SLOT_3:
        offset = 0x2AC;
        break;
    default:
        return 0;
    }
    uint64_t PlayerGameData = GetPlayerGameDataPtr();
    if (!PlayerGameData)
        return 0;

    return *(int*)(PlayerGameData + offset);
}


int getActiveProstheticSlot() {
    uint64_t PlayerGameData = GetPlayerGameDataPtr();
    if (!PlayerGameData)
        return 0;

    int selection = *(int*)(PlayerGameData + 0x31C);

    if (selection == 0)
        return PROSTHETIC_SLOT_1;
    if (selection == 1)
        return PROSTHETIC_SLOT_2;
    if (selection == 2)
        return PROSTHETIC_SLOT_3;

    return 0;
}

int prostheticSlotConvert(int slot ) {
    if (slot == PROSTHETIC_SLOT_1)
        return 0;
    if (slot == PROSTHETIC_SLOT_2)
        return 1;
    if (slot == PROSTHETIC_SLOT_3)
        return 2;
}

int prostheticSlotConvert2(int selection) {
    if (selection == 0)
        return PROSTHETIC_SLOT_1;
    if (selection == 1)
        return PROSTHETIC_SLOT_2;
    if (selection == 2)
        return PROSTHETIC_SLOT_3;

    return 0;
}

// Simple helper function to load an image into a DX11 texture with common settings
bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data;

    image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
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
    extern ID3D11Device* pDevice;
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