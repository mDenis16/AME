#pragma once
#include "CFactory.h"
#include <iostream>


CFactory(CNetworkLibrary)
{
public:

	CNetworkLibrary() : CBaseFactory() {

		std::cout << "CNetworkLibrary constructor" << std::endl;

	};
	void Hook() override;
	void Unhook() override;
};
CEnd(CNetworkLibrary)