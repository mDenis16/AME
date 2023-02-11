#pragma once
class CLauncher
{
public:
	void Launch();
	DWORD GetSteam();
	void StubSteam();
	void SetLaunchPath(std::string path);
	std::string fileLocation;
};

