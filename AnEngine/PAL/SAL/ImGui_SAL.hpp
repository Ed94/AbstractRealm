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
	using ImGui::StyleColorsDark;

	namespace PlatformBackend
	{
		eGlobal data< bool> GLFW_InstallCallbacks;
	}

	void DrawFrame();

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> BindToPlatformAndRenderer(ptr<OSAL::Window> _window);

	void MakeFrame();

	void Render();

	void MakeWindow(int _width, int _height);

	void SetupFonts();

	void SetupGPU_Resources();

	void VerifyVersion();
}


