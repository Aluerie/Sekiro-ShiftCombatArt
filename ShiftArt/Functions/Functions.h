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

#define PROSTHETIC_SLOT_1 0
#define COMBAT_ART_SLOT 1
#define PROSTHETIC_SLOT_2 2
#define PROSTHETIC_SLOT_3 4

#define COMBAT_ART 0
#define PROSTHETIC 1

std::string getNameFromRealID(int realID);
bool validateItem(int realID);
/// <summary>
/// A struct to marry all the information for prosthetics and
/// combat arts for easy access.
/// </summary>
struct EquipmentStruct
{
    std::string Name; // Ex: "Whirlwind Slash, Lazulite Axe"
    int realID;       // The ID of the combat art that is consistent between games/saves.
    int type;         //
    ID3D11ShaderResourceView *texture = NULL;

    EquipmentStruct(std::string iname, int iid)
    {
        Name = iname;
        realID = iid;
    }

    bool operator==(const int a)
    {
        return (a == realID);
    }

    bool operator==(EquipmentStruct const &a) const
    {
        return a.realID == realID;
    }

    std::string getName()
    {

        return getNameFromRealID(this->realID);
    }
};

extern bool operator!=(const EquipmentStruct &a, const EquipmentStruct &b);

extern std::vector<EquipmentStruct> CombatArts;
extern std::vector<EquipmentStruct> Prosthetics;
extern std::unordered_map<int, ID3D11ShaderResourceView *> imageMap;

extern bool popupOpen;
extern bool configPopupOpen;
extern bool pleaseSwitch;
extern bool equipSuccess;
extern int operatingMode;
// OPTIONS
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
typedef void(_fastcall *_GUISoundMaker9000)(signed int a1, int a2);
extern _GUISoundMaker9000 GUISoundMaker9000;

typedef __int64(__fastcall *__ActionRequest10)(uint64_t a1, char a2, char a3);
extern __ActionRequest10 requestAction10;

typedef __int64(__fastcall *__getIconID)(__int64 EquipInventoryDataPtrPlus0x10, unsigned int *realIDptr);
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
typedef __int64(__fastcall *_EquipArt2)(unsigned int slotNumArg, __int64 paddedArray, bool ForceEquipArg);
extern _EquipArt2 EquipItem;

typedef bool(_fastcall *_GamePadInputParser)(uint64_t PadDevicePtr);
extern _GamePadInputParser GamePadInputParser;

typedef __int64(__fastcall *__namePtrRetriever)(__int64 msgRepositoryImp, unsigned int realID);
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

int getCurrentEquippedIconID();

bool loadedIn();

int getGameMenuState();

int getOtherMenuStates();
