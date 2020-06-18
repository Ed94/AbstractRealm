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

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> HookToWindow(ptr<OSAL::Window> _window);

	void VerifyVersion();
}


