#include "stdafx.h"

void InitGame() {
	Sleep(25000);
	ULONG64 Addr = (ULONG64)GetModuleHandle(xorstr(L"crossfire_x64.exe"));

	while (true)
	{
		if (!Addr)
			break;
	}
	Sleep(5000);
	MH_Initialize();
    SettingsHelper::Initialize();
    if(!Offsets::GetAddres()) MessageBox(0, L"基址初始化失败!", L"", 0);
	if(!Render::Initialize()) MessageBox(0, L"D3D初始化失败!", L"", 0);
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		/*AllocConsole();
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		SetConsoleTitleA("xkhack Debug");*/
		Utils::InstallSafeThread(InitGame);
		break;
	}
	}
	return TRUE;
}

