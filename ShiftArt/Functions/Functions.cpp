#pragma once
#include "Functions.h"
#include <stdio.h>
#include <wtypes.h>
#include <d3d11.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION


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

    // int animation5 = *(int *)(actionRequestModule + 0xe4); // idk?
    int animation4 = *(int *)(actionRequestModule + 0x1B4); // idk?
    int force = ((realID != 5000) || (animation4 == 1));
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