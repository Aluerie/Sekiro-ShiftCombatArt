#include "Hooks.h"
#include "stdio.h"
#include "../Graphics/imgui/imgui.h"
#include <cstdint>
#include "../Functions/Functions.h"

namespace Hooks
{

	bool initialized = false;

	typedef __int64(__fastcall *__timeStepSetter)(__int64 trash, __int64 FD4Time);
	__timeStepSetter timeStepSetter = (__timeStepSetter)(0x1411DF900);
	__timeStepSetter timeStepSetterOriginal = NULL;
	__int64 setTimeStep(__int64 trash, __int64 FD4Time)
	{
		// this was meant for time slow
		return timeStepSetterOriginal(trash, FD4Time);
	}

	/// <summary>
	/// It doesn't actually check duplicates, but returns some numbers that can be harnessed to
	/// skip prosthetic duplicate removal.
	/// </summary>
	typedef __int64(__fastcall *__duplicateChecker)(unsigned int a1);
	__duplicateChecker checkDuplicate = (__duplicateChecker)(0x1408AFFC0);
	__duplicateChecker checkDuplicateOriginal = NULL;
	__int64 forceNonDuplicate(unsigned int a1)
	{
		return 0x9FFFFFFF;
	}

	/// <summary>
	/// Hijacks action requests so that I can request prosthetic cycle.
	/// </summary>
	typedef __int64(__fastcall *__ActionRequest)(uint64_t a1, __int64 a2);
	__ActionRequest requestAction = (__ActionRequest)(0x140B2C190);
	__ActionRequest requestActionOriginal = NULL;
	bool skipRound = false; // allows the game to reset itself so it doesn't think I was just holding it down.
	int hijackDelay = 3;
	__int64 hijackRequest(uint64_t actionRequestModule, __int64 a2)
	{
		if (pleaseSwitch)
		{
			if (skipRound)
			{
				*(int *)(actionRequestModule + 0x10) = 0x0;
				skipRound = false;
			}
			else
			{
				*(int *)(actionRequestModule + 0x10) |= 0x400;
				skipRound = true;
			}
			pleaseSwitch = false;
		}
		if (!equipSuccess)
		{
			if (*(int *)(actionRequestModule + 0x10) && 0x5)
			{
				*(int *)(actionRequestModule + 0x10) &= ~0x1;
			}
			hijackDelay = 3;
		}
		else if (equipSuccess && hijackDelay && false)
		{
			//*(int*)(actionRequestModule + 0x10) &= ~0x5;
			hijackDelay--;
		}
		__int64 result = requestActionOriginal(actionRequestModule, a2);

		return result;
	}

	/// <summary>
	/// The original function always returns 0, so I'm not clear on the actual purpose,
	/// but forcing it to return 1 trips a check that releases input and mouse capture.
	/// </summary>
	bool firstFree = false;
	typedef bool(__fastcall *__MenuChecker)();
	__MenuChecker menCheck = (__MenuChecker)(0x1408BC400);
	__MenuChecker menCheckOriginal = NULL;
	bool setInputCapture()
	{
		ImGuiIO &io = ImGui::GetIO();

		if (io.WantCaptureMouse)
		{
			if (popupOpen)
			{
				firstFree = true;
			}
			return 1;
		}

		if (firstFree)
		{
			firstFree = false;
		}
		return menCheckOriginal();
	}

	typedef __int64(__fastcall *__WeirdMenuManager)(__int64 a1, float a2);
	__WeirdMenuManager menInputDisable = (__WeirdMenuManager)(0x140D884F0);
	__WeirdMenuManager menInputDisableOriginal = NULL;
	__int64 setMenuCapture(__int64 a1, float a2)
	{
		ImGuiIO &io = ImGui::GetIO();

		if (io.WantCaptureMouse && configPopupOpen)
		{
			return 1;
		}
		return menInputDisableOriginal(a1, a2);
	}

	/// <summary>
	/// Returns the gamepad without parsing it when the imgui menu is open.
	/// </summary>
	typedef bool(_fastcall *_GamePadInputParser)(uint64_t PadDevicePtr);
	_GamePadInputParser GamePadInputParser = (_GamePadInputParser)(0x141A8D3E0);
	_GamePadInputParser GamePadInputParserOriginal = NULL;

	bool setGamePadInputCapture(uint64_t PadDevicePtr)
	{

		if (popupOpen)
		{

			uint64_t a = *(uint64_t *)0x143F42850;
			if (!a)
				return 0;

			uint64_t b = *(uint64_t *)(a + 0x18);
			if (!b)
				return 0;

			uint64_t c = *(uint64_t *)(b + 0x10); // gamepad0
			if (!c)
				return 0;

			*(float *)(c + 0x24C) = 0; // analog sticks, so that they don't freeze at a non-zero number, turning the camera around endlessly.
			*(float *)(c + 0x250) = 0;
			*(float *)(c + 0x258) = 0;
			*(float *)(c + 0x25C) = 0;

			return true;
		}
		return GamePadInputParserOriginal(PadDevicePtr);
	}

	/// <summary>
	/// Sets the value that changes what menu state you're in.
	/// </summary>
	typedef void(_fastcall *_UIMenuSetter)(int a, int b);
	_UIMenuSetter UIMenuSetter = (_UIMenuSetter)(0x1408C68B0);
	_UIMenuSetter UIMenuSetterOriginal = NULL;

	void setGameMenu(int a, int b)
	{

		if (a != 8)
			return UIMenuSetterOriginal(a, b);

		if (popupOpen)
			return UIMenuSetterOriginal(a, 0);

		return UIMenuSetterOriginal(a, b);
	}

	/// <summary>
	/// Create and enable all the hooks.
	/// </summary>
	void createHooks()
	{
		// MH_CreateHook((LPVOID)menCheck, (LPVOID)&setInputCapture, reinterpret_cast<LPVOID *>(&menCheckOriginal));
		// MH_EnableHook((LPVOID)menCheck);

		// MH_CreateHook((LPVOID)timeStepSetter, (LPVOID)&setTimeStep, reinterpret_cast<LPVOID *>(&timeStepSetterOriginal));
		// MH_EnableHook((LPVOID)timeStepSetter);

		MH_CreateHook((LPVOID)checkDuplicate, (LPVOID)&forceNonDuplicate, reinterpret_cast<LPVOID *>(&checkDuplicateOriginal));
		MH_EnableHook((LPVOID)checkDuplicate);

		MH_CreateHook((LPVOID)requestAction, (LPVOID)&hijackRequest, reinterpret_cast<LPVOID *>(&requestActionOriginal));
		MH_EnableHook((LPVOID)requestAction);

		// MH_CreateHook((LPVOID)menInputDisable, (LPVOID)&setMenuCapture, reinterpret_cast<LPVOID *>(&menInputDisableOriginal));
		// MH_EnableHook((LPVOID)menInputDisable);

		MH_CreateHook((LPVOID)GamePadInputParser, (LPVOID)&setGamePadInputCapture, reinterpret_cast<LPVOID *>(&GamePadInputParserOriginal));
		MH_EnableHook((LPVOID)GamePadInputParser);

		// MH_CreateHook((LPVOID)UIMenuSetter, (LPVOID)&setGameMenu, reinterpret_cast<LPVOID *>(&UIMenuSetterOriginal));
		// MH_EnableHook((LPVOID)UIMenuSetter);

		initialized = true;
	}

	bool hooksInitialized()
	{
		return initialized;
	}

}
