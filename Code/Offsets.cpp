#include <stdafx.h>
PDWORD64 pBaseAddress = 0;
PDWORD64 RoomManagera = 0;
PDWORD64 pWeaponMgr = 0;
PDWORD64 pWorldAddress = 0;
DWORD64 AIObject = 0;
namespace Offsets {
	bool GetAddres() {
		auto Cshell = GetModuleHandle(xorstr(L"CShell_x64.dll"));
		auto CrossFire = GetModuleHandle(xorstr(L"crossfire_x64.exe"));


		auto addr = Utils::FindPattern(xorstr("\x8B\x05\x00\x00\x00\x00\x85\xC0\x0F\x84\x00\x00\x00\x00\x83\x3D"), xorstr("xx????xxxx????xx"), CrossFire) - 1;
		if (!addr) {
			MessageBox(0, xorstr(L"pWorldAddress Error!"), xorstr(L"Error"), MB_OK);
			return false;
		}
		pWorldAddress = reinterpret_cast<decltype(pWorldAddress)>(RELATIVE_ADDR(addr, 7) - 8);


		addr = Utils::FindPattern(xorstr("\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x88\x00\x00\x00\x00\x48\x8B\x41\x08\x48\x85\xC0"), xorstr("xxx????xxx????xxxxxxx"), Cshell);
		if (!addr) {
			MessageBox(0, xorstr(L"pBaseAddress Error!"), xorstr(L"Error"), MB_OK);
			return false;
		}
		pBaseAddress = reinterpret_cast<decltype(pBaseAddress)>(RELATIVE_ADDR(addr, 7));

		addr = Utils::FindPattern(xorstr("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x75\x1F"), xorstr("xxx????xxxxx"), Cshell);
		if (!addr) {
			MessageBox(0, xorstr(L"RoomManager Error!"), xorstr(L"Error"), MB_OK);
			return false;
		}
		RoomManagera = reinterpret_cast<decltype(RoomManagera)>(RELATIVE_ADDR(addr, 7));

		addr = Utils::FindPattern(xorstr("\x48\x8B\x0D\x00\x00\x00\x00\x48\x2B\xC1\x48\xC1\xF8\x03\x48\x3B\xD0"), xorstr("xxx????xxxxxxxxxx"), Cshell);
		if (!addr) {
			MessageBox(0, xorstr(L"pWeaponMgr Error!"), xorstr(L"Error"), MB_OK);
			return false;
		}
		pWeaponMgr = reinterpret_cast<decltype(pWeaponMgr)>(RELATIVE_ADDR(addr, 7));

		addr = Utils::FindPattern(xorstr("\x48\x89\x5C\x24\x18\x55\x48\x83\xEC\x20\x48\x8B\x01\x48\x8B\xD9"), xorstr("xxxxxxxxxxxxxxxx"), Cshell);
		if (!addr) {
			MessageBox(0, xorstr(L"AIObject Error!"), xorstr(L"Error"), MB_OK);
			return false;
		}
		AIObject = reinterpret_cast<decltype(AIObject)>(addr);

		//vertex lock failed
		/*addr = Utils::FindPattern(xorstr("\xE8\x00\x00\x00\x00\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48"), xorstr("x????xxxxxxxxxxxx????x"), Cshell);
		if (!addr) {
			MessageBox(0, xorstr(L"GameMessagesBox Error!"), xorstr(L"Error"), MB_OK);
			return false;
		}
		g_GameMessagesBox = reinterpret_cast<decltype(g_GameMessagesBox)>(addr);*/
		g_CLTClient = (CLTClient*)(*(DWORD64*)((DWORD64)(pBaseAddress) - CLTClientoffset));
		g_IntersectSegment = (mIntersectSegment)(g_CLTClient->IntersectSegment);
		Core::Hook_Remotekill();
		//g_GameMessagesBox(0x37, 0x77, 0, xorstr("Welcome to SuJiu CrossFire 体验服一区 \n\n\n Version:2021.04.28"), 0, 0, 0, 0);

		return true;
	}
}

