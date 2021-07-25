#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <algorithm>

const wchar_t* title = L"this took me 5 minutes to make";
bool attached = false;

// length of max message logged in the text input framwork
static wchar_t buffer[128];

void main_thread(void* instance) {
	
	// setup console
	attached = AllocConsole() && SetConsoleTitleW(title);
	freopen_s(reinterpret_cast<FILE**>stdin, "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>stdout, "CONOUT$", "w", stdout);

	// find "textinputframework.dll"
	uint32_t textinput_mod = (uint32_t)GetModuleHandleA("textinputframework.dll");

	if (!textinput_mod)
		return;

	while (!GetAsyncKeyState(VK_END)) {
		
		memcpy_s(buffer, 128, (void*)(textinput_mod + 0xAB730), 128);

		// current issue is that the previous bytes dont get removed after removing them from the message box in discord.
		// so you are left with strange results. still very readable.

		wprintf(L"%s\n", buffer);
		Sleep(100);
	}

	auto console_window = GetConsoleWindow();
	FreeConsole();
	PostMessageA(console_window, WM_QUIT, 0, 0);
	FreeLibraryAndExitThread((HMODULE)instance, 0);
}

int __stdcall DllMain(void* instance, unsigned long reason_to_call, void* reserved) {
	if (reason_to_call != DLL_PROCESS_ATTACH)
		return TRUE;

	CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_thread, instance, 0, nullptr));

	return TRUE;
}