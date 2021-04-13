// Parent Header
#include "PAL.hpp"


#include "ImGui_SAL.hpp"

#include "OSAL.hpp"
#include "HAL.hpp"


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


	void Load()
	{
		OSAL::Load();

		HAL::LoadModule();

		SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);
	}
}