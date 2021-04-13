// Parent
#include "Core.hpp"



#include "ImGui_SAL.hpp"
#include "MasterExecution.hpp"
#include "LAL/LAL.hpp"




namespace Core
{
	using namespace LAL;

	void Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;

		if (CollapsingHeader("Core"))
		{
			if (TreeNode("Concurrency"))
			{
				TreePop();
			}

			if (TreeNode("Dev"))
			{
				TreePop();
			}

			if (TreeNode("Execution"))
			{
				using namespace Execution;

				if (CollapsingHeader("Master Cycler"))
				{
					if (Table2C::Record())
					{
						Table2C::Entry("Cycle", ToString(Get_MasterCycler().GetCycle()));
						Table2C::Entry("Delta Time", ToString(Get_MasterCycler().GetDeltaTime().count()));
						Table2C::Entry("Average Delta", ToString(Get_MasterCycler().GetAverageDelta().count()));

						Table2C::EndRecord();
					}
				}

				TreePop();
			}

			if (TreeNode("IO"))
			{
				TreePop();
			}

			if (TreeNode("Memory"))
			{
				TreePop();
			}

			if (TreeNode("Objects"))
			{
				TreePop();
			}
		}
	}


	void Load()
	{
		SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);
	}
}