// Parent Header
#include "OSAL.hpp"


#include "TPAL_ImGui.hpp"


#include "OSAL_Backend.hpp"
#include "OSAL_Hardware.hpp"
#include "OSAL_Networking.hpp"
#include "OSAL_Timing.hpp"


namespace OSAL
{
	using namespace Meta;


	void Record_EditorDevDebugUI()
	{
		using namespace TPAL::Imgui;

		#define Args(_Entry) NameOf(_Entry).str(), _Entry

		if (TreeNode("OSAL"))
		{
			if (CollapsingHeader("Hardware"))
			{
				if (Table2C::Record())
				{
					if (Table2C::NestedTree("CPU"))
					{
						Table2C::Entry("Vendor", OSAL::Get_CPUVendor());
						Table2C::Entry("Model" , OSAL::Get_CPUModel());
						Table2C::Entry("Frequency", OSAL::Get_CPUFrequency());

						ImGui::TreePop();
					}

					if (Table2C::NestedTree("Memory"))
					{
						if (Table2C::NestedCollapsingHeader("Physical"))
						{
							Table2C::Entry(Args(OSAL::Get_Memory().PhysicalTotal));
							Table2C::Entry(Args(OSAL::Get_Memory().PhysicalAvail));
						}

						if (Table2C::NestedCollapsingHeader("Virtual"))
						{
							Table2C::Entry(Args(OSAL::Get_Memory().VirtualTotal));
							Table2C::Entry(Args(OSAL::Get_Memory().VirtualAvail));
						}

						ImGui::TreePop();
					}

					if (Table2C::NestedTree("Display Information"))
					{
						for (auto& display : OSAL::Get_Displays())
						{
							auto idNode = String("ID: ") + ToString(display.ID);

							if (Table2C::NestedCollapsingHeader(idNode))
							{
								Table2C::Entry("Native Resolution", ToString(display.Width) + String("x") + ToString(display.Height));
								Table2C::Entry(Args(display.DPI));
								Table2C::Entry(Args(display.RefreshRate));
								Table2C::Entry(Args(display.ColorDepth));
							}
						}

						ImGui::TreePop();
					}

					Table2C::EndRecord();
				}
			}

			if (CollapsingHeader("Platform"))
			{
				if (Table2C::Record())
				{
					Table2C::Entry(Args(OSAL::OS));

					Table2C::Entry("Full Name", OSAL::Get_OSName());

					Table2C::Entry("Version", OSAL::Get_OSVersion().Str());

					Table2C::EndRecord();
				}
			}

			if (CollapsingHeader("Timing"))
			{
				if (Table2C::Record())
				{
					StringStream toString;
					
					toString << PutTime(GetExecutionStartDate(), "%F %I:%M:%S %p");

					Table2C::Entry("Execution Start", toString.str());

					toString.str(String());;

					toString << PutTime(GetTime_Local(), "%F %I:%M:%S %p");

					Table2C::Entry("Current Time (Local)", toString.str());

					toString.str(String());;

					toString << PutTime(GetTime_UTC(), "%F %I:%M:%S %p");

					Table2C::Entry("Current Time (UTC)", toString.str());

					auto sinceStart = duration_cast<Nanoseconds>(SystemClock::now().time_since_epoch()).count();

					toString.str(String());;

					toString << sinceStart;

					Table2C::Entry("System Clock: Now", toString.str());

					sinceStart = duration_cast<Nanoseconds>(SteadyClock::now().time_since_epoch()).count();

					toString.str(String());;

					toString << sinceStart;

					Table2C::Entry("Steady Clock: Now", toString.str());

					sinceStart = duration_cast<Nanoseconds>(HighResClock::now().time_since_epoch()).count();

					toString.str(String());;

					toString << sinceStart;

					Table2C::Entry("High Resolution Clock: Now", toString.str());

					Table2C::Entry("System Time Accuracy", ToString(Get_SystemTimeInfo().Precison) + " milliseconds");
					Table2C::Entry("Steady Time Accuracy", ToString(Get_SteadyTimeInfo().Precison) + " milliseconds");
					Table2C::Entry("High Resolution Time Accuracy", ToString(Get_HighResTimeInfo().Precison) + " milliseconds");

					Table2C::EndRecord();
				}
			}

			ImGui::TreePop();
		}
	}



	// Public

	void Load()
	{
		Load_Backend();

		Log("Loading Module...");

		Load_Hardware();

		Load_Platform();

		Load_Timing();	

		QueryThreadInfo();	

		GenerateThreads();

		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:
			{
				TPAL::GLFW::Initalize();

				Log("Initialized windowing platform: GLFW");
				
				break;
			}
		}

		Load_Networking();
	}

	void Unload()
	{
		Unload_Networking();

		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:

			{
				TPAL::GLFW::Terminate();

				Log("Terminated windowing platform: GLFW");
			}
		}

		Log("Unloaded module");
	}

	void PollEvents()
	{
		if constexpr (WindowingPlatform == Meta::EWindowingPlatform::GLFW)
		{
			TPAL::GLFW::PollEvents();
		}
	}



	// Private

	void GetProessInformation()
	{

	}
}