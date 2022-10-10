#pragma once
#include <cstdint>
#include <string>
#include <d3d11.h>
#include <list>
#include <iostream>
#include <iterator>
#include <vector>
#include <codecvt>
#include <filesystem>
#include <unordered_map>
#include "../Graphics/imgui/imgui.h"
#include <minwinbase.h>
#define PROSTHETIC_SLOT_1 0
#define COMBAT_ART_SLOT   1
#define PROSTHETIC_SLOT_2 2
#define PROSTHETIC_SLOT_3 4

#define COMBAT_ART 0
#define PROSTHETIC 1

std::string getNameFromRealID(int realID);
int GetRealIconIDFromRealID(int realID);
bool validateItem(int realID);


enum ImageSheet{
    SB_Boss = 0,
    SB_Icon,
    SB_Icon_02,
    SB_Icon_03,
    SB_Icon_04
};

struct ImageInfoStruct {
    int sheet;
    int realIconID; //too many ID's :(
    ImVec2 uv0;
    ImVec2 uv1;
    float width;
    float height;
    void* imageptr;
    ImageInfoStruct() {
        sheet = 0;
        realIconID = 0;
        imageptr = 0;
    }
    ImageInfoStruct(int sheet2, int id2, float x2, float y2, float width2, float height2) {
        sheet = sheet2;
        realIconID = id2;
        width = width2;
        height = height2;
        
        int maxWidth = 0;
        int maxHeight = 0;

        if (sheet == SB_Boss)
        {
            maxWidth = 2048;
            maxHeight = 2048;
        }
        else if (sheet == SB_Icon || sheet == SB_Icon_03)
        {
            maxWidth = 4096;
            maxHeight = 2048;
        }
        else
        {
            maxWidth = 1024;
            maxHeight = 2048;
        }

        uv0.x = x2 / maxWidth;
        uv0.y = y2 / maxHeight;

        uv1.x = (x2 + width) / maxWidth;
        uv1.y = (y2 + height) / maxHeight;

    }

    bool operator==(const int a) {
        return (a == realIconID);
    }

    void SetImagePtr() {
        uint64_t ptr = *(uint64_t*)(0x143D98F10);

        __try
        {
        ptr = *(uint64_t*)(ptr + 0x58);
        ptr = *(uint64_t*)(ptr + 0xC30);
        ptr = *(uint64_t*)(ptr + 0x80);


        switch (sheet)
        {
        case SB_Boss:
             ptr = *(uint64_t*)(ptr + 0x108);
            break;
        case SB_Icon:
             ptr = *(uint64_t*)(ptr + 0x138);
            break;
        case SB_Icon_02:
             ptr = *(uint64_t*)(ptr + 0x140);
            break;
        case SB_Icon_03:
             ptr = *(uint64_t*)(ptr + 0x148);
            break;
        case SB_Icon_04:
             ptr = *(uint64_t*)(ptr + 0x150);
            break;
        default:
            ptr = *(uint64_t*)(ptr + 0x150);
            break;

        }

        if (ptr > 0x00007FF400000000 && ptr < 0x00007FF4ffffffff)
        {                                       
            ptr = *(uint64_t*)(ptr + 0x68);
            ptr = *(uint64_t*)(ptr + 0x10);
            ptr = *(uint64_t*)(ptr + 0x48);
            ptr = *(uint64_t*)(ptr + 0x10);

            imageptr = (void*)ptr;
        }
        else
        {
            imageptr = 0;
        }

        }
        __except (EXCEPTION_ACCESS_VIOLATION) { imageptr = 0;  }



    }
};
ImageInfoStruct* getImageFromRealID(int realID);

/// <summary>
/// A struct to marry all the information for prosthetics and
/// combat arts for easy access. 
/// </summary>
struct EquipmentStruct {
    std::string Name; // Ex: "Whirlwind Slash, Lazulite Axe"
    int realID;     // The ID of the combat art that is consistent between games/saves. 
    int type;   //
    ImageInfoStruct image;

    EquipmentStruct(std::string iname, int iid, ImageInfoStruct iimage) {
        Name = iname;
        realID = iid;
        image = iimage;
    }

    bool operator==(const int a) {
        return (a == realID);
    }

    bool operator==(EquipmentStruct const& a) const {
        return a.realID == realID;
    }

    std::string getName() {

        return getNameFromRealID(this->realID);

    }
};
extern bool operator!=(const EquipmentStruct& a, const EquipmentStruct& b);

extern std::vector<EquipmentStruct> CombatArts;
extern std::vector<EquipmentStruct> Prosthetics;
extern std::vector<ImageInfoStruct> imageInfos;

extern bool usingGamepad2;
extern bool analogueTriggerState;
extern bool triggerSetting;
extern bool popupOpen;
extern bool configPopupOpen;
extern bool pleaseSwitch;
extern bool equipSuccess;
extern int operatingMode;
//OPTIONS
extern bool wantTimeSlow;

/// <summary>
/// Can recreate any sound that's used by the in-game gui.
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
extern _GUISoundMaker9000 GUISoundMaker9000;

typedef __int64(__fastcall* __ActionRequest10)(uint64_t a1, char a2, char a3);
extern __ActionRequest10 requestAction10;

typedef __int64 (__fastcall* __getIDinfo)(__int64 weirdStackShit, int realID);
extern __getIDinfo getIDinfo;

typedef __int64(__fastcall* __getIconID)(__int64 EquipInventoryDataPtrPlus0x10, unsigned int* realIDptr);
extern __getIconID getIconID;
/// <summary>
/// Equips prosthetics and combat arts.
/// 
/// slotNumArg: 4 slot numbers
///             0: 1st prosthetic
///             1: combat art
///             2: 2nd prosthetic
///             3: 3rd prosthetic
/// </summary>
typedef __int64(__fastcall* _EquipArt2)(unsigned int slotNumArg, __int64 paddedArray, bool ForceEquipArg);
extern _EquipArt2 EquipItem;


typedef bool(_fastcall* _GamePadInputParser)(uint64_t PadDevicePtr);
extern _GamePadInputParser GamePadInputParser;

typedef __int64(__fastcall* __namePtrRetriever)(__int64 msgRepositoryImp, unsigned int realID);
extern __namePtrRetriever namePtrRetriever;

/// <summary>
/// Calculates the mutable iconID from the consistent one
/// so that the weapon equip function can be called.
/// 
/// </summary>
/// <param name="ID"></param>
/// <returns></returns>
unsigned int iconIDCalculator(u_int realID);

bool attemptEquip(DWORD realID);

int getCurrentEquippedIconID(int slot);

bool loadedIn();

int getGameMenuState();

int getOtherMenuStates();

int getActiveProstheticSlot();

bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

void structInitializer();

int prostheticSlotConvert(int slot);

int prostheticSlotConvert2(int selection);

