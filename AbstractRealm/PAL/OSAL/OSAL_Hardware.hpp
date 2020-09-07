#pragma once



#include "LAL.hpp"





namespace OSAL
{
	using namespace LAL;



	struct CPU_InstructionSupportInfo
	{

	};


	struct DisplayConfig
	{
		uint32 Width, Height;

		DynamicArray<float64> RefreshRates;
	};

	struct DisplayInfo
	{
		WordSize ID;

		uint32 Width, Height;

		uint32 DPI, ColorDepth;

		float64 RefreshRate;

		DynamicArray<DisplayConfig> Configurations;
	};

	struct MemoryInfo
	{
		uInt64 PhysicalTotal;
		uInt64 PhysicalAvail;
		uInt64 VirtualTotal;
		uInt64 VirtualAvail;
	};

	const DisplayInfo& GetMainDisplay();

	void Load_Hardware();
}
