#pragma once
#include <cstddef>



#define XBR_VIRTUAL_METHOD(Return, Name, Args) \
	virtual Return Name Args = 0;

#define XBR_VIRTUAL_DTOR(Name) \
	virtual ~Name() = 0;
