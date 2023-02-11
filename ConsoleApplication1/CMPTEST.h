#pragma once
#include "CFactory.h"
#include <iostream>



CFactory(CMPTEST)
{
public:

	CMPTEST() {
		std::cout << "Called CMPTEST" << std::endl;
	}
	void Hook() override;
	void Unhook() override;
};
//CEnd(CMPTEST)
