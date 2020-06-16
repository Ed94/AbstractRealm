#pragma once

// Parent Header
//#include "GLFW_SAL.hpp"



namespace SAL
{
	namespace GLFW
	{
		bool CanClose(const ptr<Window> _window)
		{
			return glfwWindowShouldClose(_window);
		}

		ptr<Window> MakeWindow(int _width, int _height, ptr<const char> _title, ptr<Monitor> _fullscreenCast, ptr<Window> _windowToShareWith)
		{
			return glfwCreateWindow(_width, _height, _title, _fullscreenCast, _windowToShareWith);
		}

		void DestroyWindow(ptr<Window> _window)
		{
			glfwDestroyWindow(_window);
		}

		bool Initalize()
		{
			return glfwInit() ? true : false;
		}

		void PollEvents()
		{
			glfwPollEvents();

			return;
		}

		void Terminate()
		{
			glfwTerminate();
		}

		template<typename ParameterType>
		void SetWindowCreationParameter(EWindowCreationParameter _param, ParameterType _value)
		{
			glfwWindowHint(int(_param), int(_value));

			return;
		}


		// Vulkan Related

		CStrArray GetRequiredVulkanAppExtensions(uint32& _extensionCount)
		{
			return glfwGetRequiredInstanceExtensions( getAddress(_extensionCount) );
		}
	}
}