#pragma once



// Includes

#include "Meta/Config/HAL_Config.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "OSAL/Platform.hpp"
#include "OSAL/Windowing.hpp"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_vulkan.h"

#include "LAL/LAL.hpp"



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
		eGlobal data< bool> GLFW_InstallCallbacks;
	}

	void BindToPlatformAndRenderer(ptr<OSAL::Window> _window);

	void Deinitialize();

	void Initialize(ptr<OSAL::Window> _window);

	void MakeFrame();

	void SetupFonts();

	void SetupGPU_Interface();

	void VerifyVersion();




	// Not Used
	//void MakeWindow(int _width, int _height);
}


