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
__getIDinfo getIDinfo = (__getIDinfo)(0x1410D3120);

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

int GetRealIconIDFromRealID(int realID) {
    uint64_t SoloParamRepository = *(uint64_t*)0x143D978B0;

    if (!SoloParamRepository)
        return 0;

    uint64_t EquipParamWeaponPtr = *(uint64_t*)(SoloParamRepository + 0x78);
    if (!EquipParamWeaponPtr)
    {
        return 0;
    }

    EquipParamWeaponPtr = *(uint64_t*)(EquipParamWeaponPtr + 0x70);
    short tableCount = *(short*)(EquipParamWeaponPtr + 0xFA);
    EquipParamWeaponPtr = (EquipParamWeaponPtr + 0xD2);
    for (size_t i = 0; i < tableCount; i++)
    {
        int rowID = *(int*)(EquipParamWeaponPtr + 0x5e + (i * 0x18));
        printf("%d\n", rowID);
        if (rowID == realID)
        {
            int offset = *(int*)(EquipParamWeaponPtr + 0x5e + 0x8 + (i * 0x18));
            printf("    %d\n", offset);
            printf("%llx\n", EquipParamWeaponPtr);
            EquipParamWeaponPtr = EquipParamWeaponPtr + offset + 0x1e;
            break;
        }
    }
    printf("%llx\n", EquipParamWeaponPtr);
    int realIconID = *(short*)(EquipParamWeaponPtr + 0xBA);
    return realIconID;

}

ImageInfoStruct* getImageFromRealID(int realID) {

    int realIconId = GetRealIconIDFromRealID(realID);
    auto it = std::find(imageInfos.begin(), imageInfos.end(), realIconId);
    if (it != imageInfos.end())
        return &(*it);
    else
        return &(* imageInfos.begin());
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
std::vector<ImageInfoStruct> imageInfos;

void structInitializer() {
        imageInfos.emplace_back(SB_Boss, 750, 1750, 1280, 252, 252);
        imageInfos.emplace_back(SB_Boss, 752, 1750, 1024, 252, 252);
        imageInfos.emplace_back(SB_Boss, 754, 1644, 768, 252, 252);
        imageInfos.emplace_back(SB_Boss, 756, 1594, 512, 252, 252);
        imageInfos.emplace_back(SB_Boss, 970, 1594, 256, 252, 252);
        imageInfos.emplace_back(SB_Boss, 972, 1594, 0, 252, 252);
        imageInfos.emplace_back(SB_Boss, 973, 1750, 1536, 252, 252);
        imageInfos.emplace_back(SB_Boss, 974, 1494, 1472, 252, 252);
        imageInfos.emplace_back(SB_Boss, 975, 1494, 1216, 252, 252);
        imageInfos.emplace_back(SB_Boss, 976, 1338, 512, 252, 252);
        imageInfos.emplace_back(SB_Boss, 977, 1338, 256, 252, 252);
        imageInfos.emplace_back(SB_Boss, 978, 1338, 0, 252, 252);
        imageInfos.emplace_back(SB_Boss, 990, 1494, 1728, 252, 252);
        imageInfos.emplace_back(SB_Boss, 1650, 676, 876, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1, 3840, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 10, 3840, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 11, 3840, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 12, 3840, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 20, 3840, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 21, 3840, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 22, 3840, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 23, 3584, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 30, 3584, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 31, 3584, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 40, 3584, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 43, 3584, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 44, 3584, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 45, 3584, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 50, 3584, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 51, 3328, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 52, 3328, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 60, 3328, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 61, 3328, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 62, 3328, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 63, 3328, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 70, 3328, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 71, 3328, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 72, 3072, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 80, 3072, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 90, 3072, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 150, 3072, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 151, 3072, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 152, 3072, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 153, 3072, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 160, 3072, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 161, 2816, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 162, 2816, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 163, 2816, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 170, 2816, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 171, 2816, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 172, 2816, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 173, 2816, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 180, 2816, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 181, 2560, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 182, 2560, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 183, 2560, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 190, 2560, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 191, 2560, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 192, 2560, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 193, 2560, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 250, 2560, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 251, 2304, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 260, 2304, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 261, 2304, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 262, 2304, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 400, 2304, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 401, 2304, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 450, 2304, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 510, 2304, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 520, 2048, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 530, 2048, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 531, 2048, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 532, 2048, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 533, 2048, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 534, 2048, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 535, 2048, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 536, 2048, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 537, 1792, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 538, 1792, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 539, 1792, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 540, 1792, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 542, 1792, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 543, 1792, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 544, 1792, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 550, 1792, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 551, 1536, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 552, 1536, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 561, 1536, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 562, 1536, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 563, 1536, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 565, 1536, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 566, 1536, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 571, 1536, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 572, 1280, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 573, 1280, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 574, 1280, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 588, 1280, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 589, 1280, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 650, 1280, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 651, 1280, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 652, 1280, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 720, 1024, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 800, 1024, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 801, 1024, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 802, 1024, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 810, 1024, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 811, 1024, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 818, 1024, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 819, 1024, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 820, 768, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1000, 768, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1100, 768, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1101, 768, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1102, 768, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1103, 768, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1104, 768, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1105, 768, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1106, 512, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1108, 512, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1109, 512, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1150, 512, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1151, 512, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1152, 512, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1153, 512, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1154, 512, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1155, 256, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1156, 256, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1158, 256, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1159, 256, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1200, 256, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1201, 256, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1202, 256, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1203, 256, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1204, 0, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1205, 0, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1206, 0, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1207, 0, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1208, 0, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1209, 0, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1210, 0, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon, 1212, 0, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1300, 256, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1310, 768, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1320, 768, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1330, 768, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1340, 768, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1350, 768, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1360, 768, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1370, 768, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1380, 512, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1400, 512, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1410, 512, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1420, 512, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1430, 512, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1440, 512, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1450, 512, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1510, 512, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1520, 768, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1530, 256, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1600, 256, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1610, 256, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1620, 256, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1630, 256, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1700, 256, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1710, 256, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1720, 0, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1750, 0, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1760, 0, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1770, 0, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1780, 0, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1800, 0, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1810, 0, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_02, 1830, 0, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 13, 1792, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 14, 3840, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 41, 3840, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 42, 3840, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 65, 3840, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 81, 3840, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 100, 3840, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 110, 3840, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 111, 3584, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 112, 3584, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 113, 3584, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 270, 3584, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 271, 3584, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 272, 3584, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 273, 3584, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 274, 3584, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 280, 3328, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 410, 3328, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 411, 3328, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 412, 3328, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 413, 3328, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 414, 3328, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 415, 3328, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 416, 3328, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 417, 3072, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 418, 3072, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 419, 3072, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 420, 3072, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 545, 3072, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 546, 3072, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 564, 3072, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 567, 3072, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 568, 2816, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 569, 2816, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 576, 2816, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 578, 2816, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 579, 2816, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 580, 2816, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 581, 2816, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 582, 2816, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 583, 2560, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 584, 2560, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 585, 2560, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 586, 2560, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 590, 2560, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 591, 2560, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 592, 2560, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 593, 2560, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 594, 2304, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 595, 2304, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 596, 2304, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 600, 2304, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 601, 2304, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 602, 2304, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 610, 2304, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 620, 2304, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 621, 2048, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 630, 2048, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 631, 2048, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 640, 2048, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 700, 2048, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 710, 2048, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 711, 2048, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 712, 2048, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 713, 3840, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 714, 1792, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 715, 1792, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 716, 1792, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 717, 1792, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 718, 1792, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 719, 1792, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 803, 1792, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 804, 1536, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 805, 1536, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 806, 1536, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 807, 1536, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 808, 1536, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 809, 1536, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 812, 1536, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 813, 1536, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 814, 1280, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 815, 1280, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 816, 1280, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 817, 1280, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 900, 1280, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 901, 1280, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 910, 1280, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 920, 1280, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 930, 1024, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 940, 1024, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 950, 1024, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 951, 1024, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 960, 1024, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 961, 1024, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 962, 1024, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1107, 1024, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1157, 768, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1160, 768, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1211, 768, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1213, 768, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1214, 768, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1215, 768, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1216, 768, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1217, 768, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1218, 512, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1219, 512, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1220, 512, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1221, 512, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1222, 512, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1223, 512, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1224, 512, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1225, 512, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1226, 256, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1227, 256, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1228, 256, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1229, 256, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1790, 256, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1820, 256, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1840, 256, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1850, 256, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1860, 0, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1870, 0, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1880, 0, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1890, 0, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1900, 0, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1910, 0, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 1920, 0, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_03, 2010, 0, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 252, 256, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 255, 768, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 421, 768, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 422, 768, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 423, 768, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 424, 768, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 425, 768, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 426, 768, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 427, 512, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 428, 512, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 429, 512, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 430, 512, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 431, 512, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 432, 512, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 433, 512, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 434, 512, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 435, 768, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 436, 256, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 437, 256, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 440, 256, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 441, 256, 768, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 442, 256, 512, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 443, 256, 256, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 1940, 256, 0, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 1997, 0, 1792, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 1998, 0, 1536, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 1999, 0, 1280, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 2720, 0, 1024, 252, 252);
        imageInfos.emplace_back(SB_Icon_04, 2740, 0, 768, 252, 252);
        

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

    
    if (realID < 10000)
    {
        if (iconID == getCurrentEquippedIconID(COMBAT_ART_SLOT)) {
            printf("\tCombatArt Already Equipped\n");
            return true;
        }
        if (operatingMode == 1) //instant switch
        {
            EquipItem(COMBAT_ART_SLOT, (__int64)&icon, 1);
            return true;
        }
        char animation1 = *(int*)(actionRequestModule + 0x248); //is performing air art like sakura dance or shadow fall.
        int animation2 = *(int*)(actionRequestModule + 0xD8);
        char animation3 = *(int*)(actionRequestModule + 0x249);
        int animation4 = *(int*)(actionRequestModule + 0x1B4); 
        int animation5 = *(int*)(actionRequestModule + 0xe4); 
        int animation6 = *(int*)(actionRequestModule + 0xb8); 


        bool success = EquipItem(COMBAT_ART_SLOT, (__int64)&icon, 0);

        if (success)
        {
            printf("\tSUCCESSFULLY EQUIPPED COMBAT ART\n");
        }

        if (success)
            return success;

        printf("\tART EQUIP UNSUCCESSFUL, NOW FORCING\n");
        if (animation5 == 1)
        {
            EquipItem(COMBAT_ART_SLOT, (__int64)&icon, 1);
            printf("\tFORCE SUCCESSFUL\n");
            return true;
        }
        if (animation4 == 1) {
            counter--;
            if (!counter)
            {
                EquipItem(COMBAT_ART_SLOT, (__int64)&icon, 1);
                printf("\tFORCE SUCCESSFUL\n");
                return true;
            }
            else
            {
                printf("\tDELAY COUNTER\n");
            }
        }
        else
        {
            counter = 3;
        }

        printf("\tFORCE FAILED: BUSY\n");
        return false;
    }
    else if (realID > 10000)
    {
        if (operatingMode == 1) //instant switch
        {
            EquipItem(getActiveProstheticSlot(), (__int64)&icon, 1);
            return true;
        }
        int activeSlot = getActiveProstheticSlot();
        int activeSlotRaw = prostheticSlotConvert(activeSlot);
        int nextSlotRaw = (activeSlotRaw + 1) % 3;
        int nextSlot = prostheticSlotConvert2(nextSlotRaw);

        if (!pleaseSwitch) //if  first time or switch attempted
        {
            printf("\t !pleaseSwitch\n");
            if (originalActiveSlot == -1) //if first time
            {
                printf("\t originalActiveSlot = -1\n");
                if (icon[14] == getCurrentEquippedIconID(getActiveProstheticSlot())) //if the CURRENT slot already has the item, return true
                {
                    printf("\t 3\n");

                    return true;
                }
                else
                {
                    printf("\t 4\n");

                    originalActiveSlot = activeSlot;
                    tempID = getCurrentEquippedIconID(nextSlot);
                    if (tempID != icon[14]) //if the NEXT slot already has said item, only pretend to equip
                    {
                        printf("\t 5\n");

                        EquipItem(nextSlot, (__int64)&icon, 1);
                        printf("Active Slot: %d   Equipped %d into next slot: %d\n", activeSlot, icon[14], nextSlot);
                        if (nextSlot == activeSlot)
                        {
                            printf("\tThat shouldn't be possible\n");
                        }
                    }
                    pleaseSwitch = true;
                    return false;
                }

            }
            else if (activeSlot == originalActiveSlot) { //if the slot stayed the same, despite attempting to switch
                printf("\t 6\n");


                pleaseSwitch = true;
                return false;
            }
            else if (activeSlot != originalActiveSlot) //if the slots actually changed.
            {
                printf("\t 7\n");

                uint64_t activeSlotAddress = GetPlayerGameDataPtr() + 0x31C;

                EquipItem(originalActiveSlot, (__int64)&icon, 1); //equip what I wanted this entire time
                *(int*)activeSlotAddress = originalActiveSlot;

                *(int*)activeSlotAddress = prostheticSlotConvert(originalActiveSlot);

                if (icon[14] != tempID) //only re-equip burned prosthetic if they're not the same (so it doesn't accidentally get removed.
                {
                    printf("\t 8\n");

                    icon[14] = tempID;
                    EquipItem(activeSlot, (__int64)&icon, 1);  //equip the tempID back into the burned slot.
                }

                originalActiveSlot = -1;
                return true;
            }
            else {
                printf("Execution should not have gotten here? Returning success so no infinite equip..\n");
                return true;
            }
        }
        return false;

    }
    return false;
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

    return 0;
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