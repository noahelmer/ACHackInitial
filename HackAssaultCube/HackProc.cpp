#include "HackProc.h"
#include "stdafx.h"
#include "proc.h"
#include "mem.h"

int healthOverride(int newHealth);
int recoilOverride();


int main() {
	//healthOverride(200);
	recoilOverride();
}

int healthOverride(int newHealth) {
	// First grabbing the process ID of the assault cube executable
	DWORD procId = GetProcId(L"ac_client.exe");

	// Get the module base address
	uintptr_t moduleBaseAddr = GetModuleBaseAddress(procId, L"ac_client.exe");

	// Get handle to process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	// Resolve base address of the pointer chain
	uintptr_t dynamicPtrBaseAddr = moduleBaseAddr + 0x17E0A8;

	std::cout << "DynamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr;

	// Resolve our health pointer chain
	std::vector<unsigned int> healthOffsets = { 0xEC };
	uintptr_t healthAddr = FindDMAAddress(hProcess, dynamicPtrBaseAddr, healthOffsets);

	std::cout << "healthAddr = " << "0x" << std::hex << healthAddr;

	// Read health value
	int healthValue = 0;

	ReadProcessMemory(hProcess, (BYTE*)healthAddr, &healthValue, sizeof(healthValue), nullptr);
	std::cout << "Current health = " << healthValue << std::endl;

	// Write to it
	WriteProcessMemory(hProcess, (BYTE*)healthAddr, &newHealth, sizeof(newHealth), nullptr);

	// Read out again
	ReadProcessMemory(hProcess, (BYTE*)healthAddr, &healthValue, sizeof(healthValue), nullptr);

	std::cout << "New health = " << healthValue << std::endl;

	getchar();
	return 0;
}

int recoilOverride() {
	HANDLE hProcess = 0;
	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false;

	const int newValue = 200;

	DWORD procId = GetProcId(L"ac_client.exe");

	if (procId) {
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

		localPlayerPtr = moduleBase + 0x17E0A8;

		healthAddr = FindDMAAddress(hProcess, localPlayerPtr, { 0xEC });
	}
	else {
		std::cout << "Process not found";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;

	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			bHealth = !bHealth;
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			bAmmo = !bAmmo;

			if (bAmmo) {
				mem::PatchEx((BYTE*)(moduleBase + 0x17E0A8), (BYTE*)"\xFF\x06", 2, hProcess);
			}
			else {
				mem::PatchEx((BYTE*)(moduleBase + 0x17E0A8), (BYTE*)"\xFF\x0E", 2, hProcess);
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			bRecoil = !bRecoil;

			if (bRecoil) {

			}
		}

		if (GetAsyncKeyState(VK_INSERT) & 1) {
			return 0;
		}
	}
}