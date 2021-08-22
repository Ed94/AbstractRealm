// Parent Header
#include "PAL.hpp"


#include "PAL_Backend.hpp"

#include "SAL_ImGui.hpp"

#include "OSAL.hpp"
#include "HAL.hpp"
#include "Dev/Log.hpp"
#include "LAL/LAL.hpp"



namespace PAL
{
	void Record_EditorDevDebugUI()
	{
		if (ImGui::CollapsingHeader("PAL"))
		{
			if (ImGui::TreeNode("HAL"))
			{

				ImGui::TreePop();
			}

			OSAL::Record_EditorDevDebugUI();

			if (ImGui::TreeNode("SAL"))
			{

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("ThirdParty"))
			{

				ImGui::TreePop();
			}
		}
	}

	namespace Module
	{
		void Load()
		{
			Load_Backend();

			Log("Loading module.");

			SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);

			OSAL::Load();

			HAL::Load();
		}

		void Unload()
		{
			Log("Unloading module.");

			OSAL::Unload();

			HAL::Unload();
		}
	}
}