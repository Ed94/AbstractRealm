// Parent Header
#include "Meta.hpp"

#include "Config/CoreDev_Config.hpp"
#include "Config/HAL_Config.hpp"
#include "Config/OSAL_Config.hpp"
#include "Config/Resource_Config.hpp"
#include "Config/Simulation_Config.hpp"
#include "EngineInfo.hpp"

#include "ImGui_SAL.hpp"
#include "Specification.hpp"



namespace Meta
{
	using namespace SAL;

	// Table Objects referencing Meta constants and variables.

	void Record_EditorDevDebugUI()
	{
		using namespace Imgui;

		#define Args(_Entry) NameOf(_Entry).str(), _Entry

		if (CollapsingHeader("Meta"))
		{
			if (TreeNode("Config"))
			{
				if (CollapsingHeader("Core Dev"))
				{
					if (Table2C::Record())
					{
						Table2C::Entry(Args(UseCpp_Exceptions));
						Table2C::Entry(Args(Enable_HeapTracking));

						Table2C::EndRecord();
					}
				}

				if (CollapsingHeader("HAL"))
				{
					if (Table2C::Record())
					{
						Table2C::Entry(Args(Default_GPU_API));
						Table2C::Entry(Args(GPU_Engagement));
						Table2C::Entry(Args(GPU_API()));
						Table2C::Entry(Args(GPU_PresentationPref()));
						Table2C::Entry(Args(GPU_FrameBufferingPref()));

						Table2C::EndRecord();
					}

					switch (GPU_API())
					{
						case EGPUPlatformAPI::BGFX:
						{
						} break;

						case Meta::EGPUPlatformAPI::Vulkan:
						{
							if (ImGui::TreeNode("Vulkan"))
							{
								if (Table2C::Record())
								{
									Table2C::Entry(Args(Vulkan::EnableLayers()));
									Table2C::Entry(Args(Vulkan::Enable_API_Dump()));
									Table2C::Entry(Args(Vulkan::Enable_FPSMonitor()));
									Table2C::Entry(Args(Vulkan::Enable_Validation()));
									Table2C::Entry(Args(Vulkan::Enable_LogVerbose()));
									Table2C::Entry(Args(Vulkan::Enable_LogInfo()));
									Table2C::Entry(Args(Vulkan::Enable_LogWarning()));
									Table2C::Entry(Args(Vulkan::Enable_LogError()));

									Table2C::EndRecord();
								}

								ImGui::TreePop();
							}

						} break;
					}
				}

				if (CollapsingHeader("OSAL"))
				{
					if (Table2C::Record())
					{
						Table2C::Entry(Args(WindowingPlatform));

						Table2C::EndRecord();
					}
				}

				if (CollapsingHeader("Resource"))
				{
					if (Table2C::Record())
					{
						Table2C::Entry(Args(Greyscale_LinearAccuracy));
						Table2C::Entry(Args(ColorValue_LinearAccuracy));
						Table2C::Entry(Args(VertexValue_Accuracy));

						Table2C::EndRecord();
					}
				}

				if (CollapsingHeader("Simulation"))
				{
					if (Table2C::Record())
					{
						Table2C::Entry(Args(DecN_UseHighAccuracy_Compare));
						Table2C::Entry(Args(DecN_UseNanoAccuracy_Compare));
						Table2C::Entry(Args(DecN_UseHighAccuracy_Unit));
						Table2C::Entry(Args(DecN_UseNanoAccuracy_Unit));
						Table2C::Entry(Args(IntN_Accuracy));
						Table2C::Entry(Args(DecN_Accuracy));

						Table2C::EndRecord();
					}
				}

				TreePop();
			}

			if (ImGui::TreeNode("Engine Info"))
			{
				Table2C::Record();

				Table2C::Entry(Args(EngineName));
				Table2C::Entry("EngineVersion", 
							   ToString(EEngineVersion::Major) + "." +
							   ToString(EEngineVersion::Minor) + "." +
							   ToString(EEngineVersion::Patch));
				Table2C::Entry(Args(OperationalMode()));
				Table2C::Entry(Args(UseEditor()));
				Table2C::Entry(Args(UseDebug()));
				Table2C::Entry(Args(UseProfiling()));
				Table2C::Entry(Args(UseConcurrency()));
				Table2C::Entry(Args(UseConcurrency()));
				Table2C::Entry(Args(FixRenderRateToRefreshRate()));

				Table2C::EndRecord();

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Specification"))
			{
				if (Table2C::Record())
				{
					Table2C::Entry(Args(BuildType));

					Table2C::EndRecord();
				}

				ImGui::TreePop();
			}
		}
	}

	void LoadModule()
	{
		Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);
	}
}