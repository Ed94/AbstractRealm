// Parent Header
#include "OSAL_Hardware.hpp"



#include "OSAL_Backend.hpp"

#include "infoware/cpu.hpp"
#include "infoware/system.hpp"



namespace OSAL
{
	



	StaticData
	(
		String CPU_Vendor   ;
		String CPU_Model    ;
		uInt64 CPU_Frequency;

		MemoryInfo Memory;

		DynamicArray<DisplayInfo> Displays;

		DisplayInfo* MainDisplay;
	)


	const DisplayInfo& GetMainDisplay()
	{
		return dref(MainDisplay);
	}

	void Load_Hardware()
	{
		CPU_Model     = iware::cpu::model_name();
		CPU_Vendor    = iware::cpu::vendor    ();
		CPU_Frequency = iware::cpu::frequency ();

		//auto instructionSupp = iware::cpu::instruction_set_supported()

		auto memInfo = iware::system::memory();

		Memory = *reinterpret_cast<MemoryInfo*>(&memInfo);

		auto displays = iware::system::displays();
		auto configs = iware::system::available_display_configurations();

		DataSize displayIndex = 0;

		for (auto& display : displays)
		{
			DisplayInfo info {};

			info.ID = displayIndex + 1;

			info.Width = display.width; 
			info.Height = display.height;

			info.DPI         = display.dpi         ;
			info.ColorDepth  = display.bpp         ;
			info.RefreshRate = display.refresh_rate;

			for (auto& configRaw : configs[displayIndex])
			{
				DisplayConfig config {};

				config.Width  = configRaw.width;
				config.Height = configRaw.height;

				config.RefreshRates = configRaw.refresh_rates;

				info.Configurations.push_back(config);
			}

			Displays.push_back(info);

			displayIndex++;
		}

		MainDisplay = getAddress(Displays[0]);

		CLog("CPU Information:"); 
		CLog(String("Vendor: ") + CPU_Vendor);
		CLog("Model: " + CPU_Model);
		CLog("Frequency: " + ToString(CPU_Frequency) + " Hz");

		CLog("Memory Information: ");

		CLog("Physical:");
		CLog(String("Total    : ") + ToString(Memory.PhysicalTotal) + " Bytes");
		CLog(String("Available: ") + ToString(Memory.PhysicalAvail) + " Bytes");

		CLog("Virtual:");
		CLog(String("Total    : ") + ToString(Memory.VirtualTotal) + " Bytes");
		CLog(String("Available: ") + ToString(Memory.VirtualAvail) + " Bytes");

		CLog("Display Information");

		for (auto& display : Displays)
		{
			CLog
			(
				String("ID: ") + ToString(display.ID) + 
				String(" Native Resolution: ") + ToString(display.Width) + String("x") + ToString(display.Height) +

				String(" DPI: "         ) + ToString(display.DPI        ) + 
				String(" Refresh Rate: ") + ToString(display.RefreshRate) + 
				String(" Color Depth: " ) + ToString(display.ColorDepth )
			);
		}
		
	}
}