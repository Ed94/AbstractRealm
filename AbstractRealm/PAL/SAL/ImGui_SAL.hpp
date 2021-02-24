#pragma once



// Includes

#include "LAL/LAL.hpp"
#include "Meta/Config/HAL_Config.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "OSAL/OSAL_Platform.hpp"
#include "OSAL/OSAL_Windowing.hpp"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/imgui.h"



namespace SAL::Imgui
{
	using namespace LAL;

	using IO = ImGuiIO;

	using ImGui::CreateContext  ;
	using ImGui::GetIO          ;
	using ImGui::Render         ;
	using ImGui::StyleColorsDark;

	namespace PlatformBackend
	{
		//eGlobal bool GLFW_InstallCallbacks;
	}

	void Dirty_DoSurfaceStuff(ptr<OSAL::Window> _window);

	void BindToPlatformAndRenderer(ptr<OSAL::Window> _window);

	void Deinitialize();

	void Initialize(ptr<OSAL::Window> _window);

	void MakeFrame();

	void SetupFonts();

	void SetupGPU_Interface();

	void VerifyVersion();
}


