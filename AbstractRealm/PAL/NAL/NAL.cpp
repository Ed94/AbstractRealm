// Parent
#include "NAL.hpp"

#include "NAL_Backend.hpp"


namespace NAL
{
	void Load()
	{
		LoadBackend();

		Log("Loading module.");
	}

	void Unload()
	{

	}
}