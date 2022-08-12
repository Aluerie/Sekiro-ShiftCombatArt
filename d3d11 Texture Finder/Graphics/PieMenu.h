#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "../Functions/Functions.h"
#include "../Hooks/Hooks.h"

int PiePopupSelectMenu(const ImVec2& center, const char* popup_id, std::vector<EquipmentStruct> items, int* p_selected, int* p_selected2);
void playHoverSound(int index);
void resetSound();
void playOpenCloseSound(bool close);