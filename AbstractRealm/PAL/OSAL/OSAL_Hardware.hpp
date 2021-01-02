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
		ui32 Width, Height;

		DynamicArray<f64> RefreshRates;
	};

	struct DisplayInfo
	{
		WordSize ID;

		ui32 Width, Height;

		ui32 DPI, ColorDepth;

		f64 RefreshRate;

		DynamicArray<DisplayConfig> Configurations;
	};

	struct MemoryInfo
	{
		uI64 PhysicalTotal;
		uI64 PhysicalAvail;
		uI64 VirtualTotal;
		uI64 VirtualAvail;
	};

	const DisplayInfo& GetMainDisplay();

	void Load_Hardware();
}
