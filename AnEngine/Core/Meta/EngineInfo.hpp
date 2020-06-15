/*
Engine Information

Last Modified: 5/19/2020
*/



#pragma once



#include "LAL/LAL.hpp"



namespace Core::Meta
{
	using namespace LAL;

	constexpr char EngineName[] = "Abstract Realm";

	// 0.1: MVP Version
	class EEngineVersion
	{
	public:

		enum
		{
			Major = 0,
			Minor = 1,
			Patch = 0
		};
	};
}
