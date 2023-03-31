#pragma once
class CSocialClub {
public:
	HMODULE hModule;
	CSocialClub(HMODULE l) { hModule = l; }
	void Hook();
};