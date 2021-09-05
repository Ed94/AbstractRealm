// Parent Header
#include "OSAL_Hardware.hpp"



#include "OSAL_Backend.hpp"

#include "infoware/cpu.hpp"
#include "infoware/system.hpp"



namespace OSAL
{
#pragma region StaticData
	String CPU_Vendor   ;
	String CPU_Model    ;
	u64    CPU_Frequency;

	MemoryInfo Memory;

	DynamicArray<DisplayInfo> Displays;

	ptr<DisplayInfo> MainDisplay;
#pragma endregion StaticData


	const String& Get_CPUVendor()
	{
		return CPU_Vendor;
	}

	const String& Get_CPUModel()
	{
		return CPU_Model;
	}

	u64 Get_CPUFrequency()
	{
		return CPU_Frequency;
	}

	const MemoryInfo& Get_Memory()
	{
		return Memory;
	}

	const DynamicArray<DisplayInfo>& Get_Displays()
	{
		return Displays;
	}

	const DisplayInfo& Get_MainDisplay()
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

		Memory = RCast<MemoryInfo>(memInfo);

		auto displays = iware::system::displays();
		auto configs = iware::system::available_display_configurations();

		uDM displayIndex = 0;

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

		MainDisplay = getPtr(Displays[0]);

		Log("CPU Information:"); 
		Log(String("Vendor: ") + CPU_Vendor);
		Log("Model: " + CPU_Model);
		Log("Frequency: " + ToString(CPU_Frequency) + " Hz");

		Log("Memory Information: ");

		Log("Physical:");
		Log(String("Total    : ") + ToString(Memory.PhysicalTotal) + " Bytes");
		Log(String("Available: ") + ToString(Memory.PhysicalAvail) + " Bytes");

		Log("Virtual:");
		Log(String("Total    : ") + ToString(Memory.VirtualTotal) + " Bytes");
		Log(String("Available: ") + ToString(Memory.VirtualAvail) + " Bytes");

		Log("Display Information");

		for (auto& display : Displays)
		{
			Log
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