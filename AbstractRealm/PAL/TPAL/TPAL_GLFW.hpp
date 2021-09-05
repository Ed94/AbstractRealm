/*
GFLW Software Abstraction Layer
*/

#pragma once

#include "Meta/Meta.hpp"


// Vulkan must be included before GLFW
#include "HAL/Vulkan/Vulkan_API.hpp"

#include "GLFW/glfw3.h"         // Main glfw header, must come first.

#ifdef _WIN32 

	#ifndef GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
		#include "GLFW/glfw3native.h"   // Provides platform specific functionality.
	#endif

	#undef UNICODE

#endif


namespace TPAL::GLFW
{
	// Usings

	using namespace LAL;
	using namespace OSAL;
	using namespace VV;


	// Enums

	enum class EWindowCreationParameter
	{
		Focused   = GLFW_FOCUSED   ,
		Iconified = GLFW_ICONIFIED ,
		Resizable = GLFW_RESIZABLE ,
		Visible   = GLFW_VISIBLE   ,
		ClientAPI = GLFW_CLIENT_API,
	};

	enum class EClient_API
	{
		OpenGL    = GLFW_OPENGL_API   ,
		OpenGL_ES = GLFW_OPENGL_ES_API,
		No_API    = GLFW_NO_API
	};

	enum class EBool
	{
		True  = GLFW_TRUE ,
		False = GLFW_FALSE
	};


	// Types

	using Monitor = GLFWmonitor;
	using Window  = GLFWwindow ;

	using WindowSize_Callback = GLFWwindowsizefun;


	// Compile-Time

	constexpr ptr<Window > NotShared    = NULL; 
	constexpr ptr<Monitor> WindowedMode = NULL; 


	// Functions

	bool CanClose(const ptr<Window> WindowToCheck);

	template
	<
		typename AppInstanceHandle,
		typename SurfaceHandle    ,

		class EResult = VkResult
	>
	Where<sizeof(AppInstanceHandle) == sizeof(VkInstance  ) && 
		  sizeof(SurfaceHandle    ) == sizeof(VkSurfaceKHR)   , 
	EResult> CreateWindowSurface
	(
		AppInstanceHandle                _appHandle, 
		ptr<Window>                      _window   , 
		ptr<const VkAllocationCallbacks> _allocator, 
		SurfaceHandle&                   _surface
	)
	{
		return glfwCreateWindowSurface(_appHandle, _window, _allocator, &_surface);
	}

	void DestroyWindow(ptr<Window> WindowToDestroy_in);

	bool Initalize();

	ptr<Window> MakeWindow
	(
				  int      Width                  , 
				  int      Height                 , 
		ptr<const char   > Title                  ,
		ptr<      Monitor> MonitorToFullScreen    ,
		ptr<      Window > MonitorToShareResources
	);

	void PollEvents();

	template<typename ParameterType>
	void SetWindowCreationParameter(EWindowCreationParameter ParameterToSet, ParameterType ParameterValue);

	void WaitForEvents();

	void Terminate();


	// Vulkan Related

	using CStrArray = ptr<const ptr<const char>>;

	CStrArray GetRequiredVulkanAppExtensions(u32& NumberOfExensions_Container_out);


	// Platform

	Where<OSAL::IsWindows, OSAL::PlatformTypes::
	OS_WindowHandle> GetOS_WindowHandle(const ptr<Window> _window);

	void GetFramebufferSize(const ptr<Window> _window, int& _width_out, int& _height_out);

	void SetWidnowSizeCallback(const ptr<Window> _window, WindowSize_Callback _callback_out);
}


// Implementation
#include "TPAL_GLFW.tpp"
