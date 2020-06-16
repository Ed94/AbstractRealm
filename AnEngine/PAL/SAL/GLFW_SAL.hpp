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
#include "OSAL/Platform.hpp"



namespace SAL::GLFW
{
	using namespace LAL;

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


	// Constants

	constexpr ptr<Window > NotShared   () { return NULL; }
	constexpr ptr<Monitor> WindowedMode() { return NULL; }


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

	void Terminate();


	// Vulkan Related

	using CStrArray = ptr<const ptr<const char>>;

	CStrArray GetRequiredVulkanAppExtensions(ptr<uint32> NumberOfExensions_Container);


	// Platform

	namespace PlatformBackend
	{
		template<OSAL::EOS>
		struct PlatformTypes_Maker;

		template<>
		struct PlatformTypes_Maker<OSAL::EOS::Windows>
		{
			using OS_AppHandle    = HINSTANCE;
			using OS_WindowHandle = HWND     ;
		};

		using PlatformTypes = PlatformTypes_Maker<OSAL::OS>;
	}

	using PlatformTypes = PlatformBackend::PlatformTypes;

	Where<OSAL::IsWindows, PlatformTypes::
	OS_WindowHandle> GetOS_WindowHandle(Window* _window)
	{
		return glfwGetWin32Window(_window);
	}
}



// Implementation
#include "GLFW_SAL_Implem.hpp"
