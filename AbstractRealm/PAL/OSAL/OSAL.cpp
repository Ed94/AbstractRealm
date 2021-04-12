// Parent Header
#include "OSAL.hpp"


#include "ImGui_SAL.hpp"


#include "OSAL_Backend.hpp"
#include "OSAL_Hardware.hpp"



namespace OSAL
{
	using namespace Meta;


	void Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;

		#define Args(_Entry) NameOf(_Entry).str(), _Entry

		if (ImGui::TreeNode("OSAL"))
		{
			if (ImGui::CollapsingHeader("Hardware"))
			{
				if (Table2C::Record())
				{
					if (Table2C::NestedTree("CPU"))
					{
						Table2C::Entry(Args(OSAL::Get_CPUVendor()));
						Table2C::Entry(Args(OSAL::Get_CPUModel()));
						Table2C::Entry(Args(OSAL::Get_CPUFrequency()));

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

			ImGui::TreePop();
		}
	}



	// Public

	void Load()
	{
		CLog("Loading Module...");

		Load_Hardware();

		Load_Platform();

		Load_Timing();	

		QueryThreadInfo();	

		GenerateThreads();

		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:
			{
				SAL::GLFW::Initalize();

				CLog("Initialized windowing platform: GLFW");
				
				break;
			}
		}
	}

	void Unload()
	{
		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:
			{
				SAL::GLFW::Terminate();

				CLog("Terminated windowing platform: GLFW");
			}
		}

		CLog("Unloaded module");
	}

	void PollEvents()
	{
		if constexpr (WindowingPlatform == Meta::EWindowingPlatform::GLFW)
		{
			SAL::GLFW::PollEvents();
		}
	}


	// Private


	void GetProessInformation()
	{

	}
}