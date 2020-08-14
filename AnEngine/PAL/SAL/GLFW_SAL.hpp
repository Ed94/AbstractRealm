/*
GFLW Software Abstraction Layer
*/



#pragma once



// Includes

// Vulkan must be included before GLFW
#include "HAL/Vulkan/Vulkan_API.hpp"

// GLFW
#ifdef _WIN32 
	#ifndef GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#endif
#endif

#include "GLFW/glfw3.h"         // Main glfw header, must come first.
#include "GLFW/glfw3native.h"   // Provides platform specific functionality.

// Engine
#include "LAL/LAL.hpp"
#include "OSAL/OSAL_Platform.hpp"



namespace SAL::GLFW
{
	using namespace LAL;
	using namespace OSAL;
	using namespace VT;

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


	// Constants

	constexpr ptr<Window > NotShared    = NULL; 
	constexpr ptr<Monitor> WindowedMode = NULL; 


	// Functions

	bool CanClose(const ptr<Window> WindowToCheck);

	ptr<Window> MakeWindow
	(
				  int      Width                  , 
				  int      Height                 , 
		ptr<const char   > Title                  ,
		ptr<      Monitor> MonitorToFullScreen    ,
		ptr<      Window > MonitorToShareResources
	);

	template
	<
		typename AppInstanceHandle  ,
		typename SurfaceHandle,
		class EResult = VkResult
	>
	typename std::enable_if< 
		sizeof(AppInstanceHandle) == sizeof(VkInstance  ) && 
		sizeof(SurfaceHandle    ) == sizeof(VkSurfaceKHR), 
	EResult>::type CreateWindowSurface
	(
			  AppInstanceHandle      _appHandle, 
			  Window*                _window   , 
		const VkAllocationCallbacks* _allocator, 
			  SurfaceHandle&         _surface
	)
	{
		return glfwCreateWindowSurface(_appHandle, _window, _allocator, &_surface);
	}

	void DestroyWindow(ptr<Window> WindowToDestroy);

	bool Initalize();

	void PollEvents();

	template<typename ParameterType>
	void SetWindowCreationParameter(EWindowCreationParameter ParameterToSet, ParameterType ParameterValue);

	void WaitForEvents();

	void Terminate();


	// Vulkan Related

	using CStrArray = ptr<const ptr<const char>>;

	CStrArray GetRequiredVulkanAppExtensions(uint32& NumberOfExensions_Container);


	// Platform

	Where<OSAL::IsWindows, OSAL::PlatformTypes::
	OS_WindowHandle> GetOS_WindowHandle(const ptr<Window> _window);

	void GetFramebufferSize(const ptr<Window> _window, int& _width, int& _height);

	void SetWidnowSizeCallback(const ptr<Window> _window, WindowSize_Callback _callback);
}



// Implementation
#include "GLFW_SAL_Implem.hpp"
