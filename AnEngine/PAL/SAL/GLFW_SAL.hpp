/*
GFLW Software Abstraction Layer

Last Modified: 5/19/2020
*/



#pragma once


// GLFW
#include "GLFW/glfw3.h"

// LAL
#include "LAL/LAL.hpp"
#include "Core/Memory/MemTypes.hpp"



namespace SAL
{
	using namespace LAL;

	namespace GLFW
	{
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

		ptr<Window> CreateWindow
		(
			          int      Width                  , 
			          int      Height                 , 
			ptr<const char   > Title                  ,
			ptr<      Monitor> MonitorToFullScreen    ,
			ptr<      Window > MonitorToShareResources
		);

		void DestroyWindow(ptr<Window> WindowToDestroy);

		bool Initalize();

		void PollEvents();

		template<typename ParameterType>
		void SetWindowCreationParameter(EWindowCreationParameter ParameterToSet, ParameterType ParameterValue);

		void Terminate();


		// Vulkan Related

		using CStrArray = ptr<const ptr<const char>>;

		CStrArray GetRequiredVulkanAppExtensions(ptr<uInt32> NumberOfExensions_Container);
	}
}



// Implementation
#include "GLFW_SAL.hppi"
