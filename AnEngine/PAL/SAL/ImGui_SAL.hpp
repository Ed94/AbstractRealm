#pragma once


#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_vulkan.h"





namespace SAL
{
	namespace Imgui
	{
		void VerifyVersion()
		{
			IMGUI_CHECKVERSION();
		}
	}
}


