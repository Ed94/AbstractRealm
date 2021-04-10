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
		u32 Width, Height;

		DynamicArray<f64> RefreshRates;
	};

	struct DisplayInfo
	{
		uDM ID;

		u32 Width, Height;

		u32 DPI, ColorDepth;

		f64 RefreshRate;

		DynamicArray<DisplayConfig> Configurations;
	};

	struct MemoryInfo
	{
		u64 PhysicalTotal;
		u64 PhysicalAvail;
		u64 VirtualTotal;
		u64 VirtualAvail;
	};

	const DisplayInfo& GetMainDisplay();

	void Load_Hardware();
}
