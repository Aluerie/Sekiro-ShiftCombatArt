#pragma once
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include "Graphics/kiero/minhook/include/MinHook.h"
#include <d3d11.h>
#include <dxgi.h>
#include <ctime>
#include "Hooks/Hooks.h"

extern bool graphicsInit();

namespace ApplicationLoader {
	void init();
}

class CApplicationCore {
public:

};