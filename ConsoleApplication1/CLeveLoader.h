#pragma once
#include "CFactory.h"
#include <iostream>



CFactory(CLevelLoader)
{
public:
	
	CLevelLoader() : CBaseFactory() {
	
		std::cout << "CLevelLoader constructor" << std::endl;
	
	};
	void Hook() override;
	void Unhook() override;
};
CEnd(CLevelLoader)
