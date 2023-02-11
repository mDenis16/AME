#pragma once
#include "CFactory.h"
#include <iostream>



CFactory(CMain)
{
public:

	CMain() {
		std::cout << "Called CMain" << std::endl;
	}
	void Hook() override;
	void Unhook() override;

	static void TriggerHook();
};
CEnd(CMain)
