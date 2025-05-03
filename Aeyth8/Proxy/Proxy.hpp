#pragma once
#include "../../pch.h"

#define EXPORT extern "C" inline __declspec(dllexport) 


class Proxy
{
private:

	inline static HMODULE RealDLL{0};
	inline static int AttachCounter{0};

public:


	static bool Attach(HMODULE CurrentModule);
	static void Detach();



};