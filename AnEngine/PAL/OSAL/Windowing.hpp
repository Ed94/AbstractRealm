/*
Currently windowing provider is determined at compile-time. This leads to a not so nice
way of doing generics for different providers not so nice.

Currently only GLFW is supported so if I ever decide to support SDL or the raw OS API instead this may turn
into a binding interface instead (Or at least most likely refactor how I do the generalization of function specification)
*/

#pragma once


#include "LAL/LAL.hpp"

#include "Meta/Config/OSAL_Config.hpp"

#include "OSAL/Platform.hpp"

#include "SAL/GLFW_SAL.hpp"



namespace OSAL
{
	// Usings

	using namespace LAL;

	using namespace Meta;
	using namespace SAL;


	struct FrameBufferDimensions
	{
		FrameBufferDimensions() { Width = 0; Height = 0; }

		FrameBufferDimensions(int _width, int _height) : Width(_width), Height(_height)
		{}

		int Width, Height;
	};


	namespace PlatformBackend
	{
		// Usings

		using namespace Meta;


		// Compile-time

		namespace WIP
		{
			/*
			This might be a better way of generalizing window types between platforms.


			*/
			template<EWindowingPlatform>
			struct WindowType_Maker;

			struct WindowType_PlatformAgnostic
			{};

			template<>
			struct WindowType_Maker<EWindowingPlatform::GLFW> : WindowType_PlatformAgnostic
			{

				ptr<GLFW::Window> window; 
			};

			using Window = WindowType_Maker<WindowingPlatform>;
		}

		using Window = 
			Choose<WindowingPlatform == EWindowingPlatform::GLFW,

				SAL::GLFW::Window,
				ptr<void>
			>;

		using WindowSize_CallbackFunc = 
			Choose<WindowingPlatform == EWindowingPlatform::GLFW,

				SAL::GLFW::WindowSize_CallbackFunc,
				ptr<void>
			>;


		// Figure out how to do this properly later...
		using namespace SAL;

		using GLFW::WindowedMode;
		using GLFW::NotShared;

		template<EWindowingPlatform>
		struct WindowCreationSpec_Maker;

		struct WindowCreationSpec_PlatformAgnostic
		{
			RoCStr                WindowTitle;
			FrameBufferDimensions WindowSize;
		};

		template<>
		struct WindowCreationSpec_Maker<EWindowingPlatform::GLFW> : WindowCreationSpec_PlatformAgnostic
		{
			GLFW::EClient_API       ClientAPI;
			GLFW::EBool             Resizable;
			ptr<SAL::GLFW::Monitor> Windowed;
			ptr<Window>             WindowToShareWith;
		};

		using WindowCreationSpec = WindowCreationSpec_Maker<WindowingPlatform>;
	}


	using PlatformBackend::PlatformTypes;
	using PlatformBackend::Window;
	using PlatformBackend::WindowSize_CallbackFunc;
	using PlatformBackend::WindowedMode;
	using PlatformBackend::NotShared;
	using PlatformBackend::WindowCreationSpec;


	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	ptr< Window>> Create_Window(WindowCreationSpec _windowCreationSpec);

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> Destroy_Window(const ptr<Window> _window);

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	bool> CanClose(const ptr<Window> _window);

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> SetWindow_SizeChangeCallback(const ptr<Window> _window, WindowSize_CallbackFunc _callback);

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	PlatformTypes::OS_WindowHandle> GetOS_WindowHandle(const ptr<Window> _window);

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	FrameBufferDimensions> GetFramebufferDimensions(const ptr<Window> _window);
}
