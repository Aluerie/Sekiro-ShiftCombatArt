#pragma once
#include "../Graphics/kiero/minhook/include/MinHook.h"

namespace Hooks {
	void createHooks();
	bool hooksInitialized();

	bool setInputCapture();
	void setGameMenu(int a, int b);
}