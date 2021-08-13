// Parent
#include "Core.hpp"

#include "Core_Backend.hpp"
#include "PAL.hpp"


namespace Core
{
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
				Dev::Record_EditorDevDebugUI();

				TreePop();
			}

			if (TreeNode("Execution"))
			{
				using namespace Execution;

				if (CollapsingHeader("Master Cycler"))
				{
					if (Table2C::Record())
					{
						Table2C::Entry("Cycle"        , ToString(Get_MasterCycler().GetCycle()));
						Table2C::Entry("Delta Time"   , ToString(Get_MasterCycler().GetDeltaTime().count()));
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

	
	namespace Module
	{
		void Load()
		{
			LoadBackend();

			Log("Loading module.");

			SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);

			Concurrency::Load();
		}

		void Unload()
		{
			Log("Loading module");

			Concurrency::Unload();
		}
	}
}