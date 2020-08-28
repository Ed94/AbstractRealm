/*
Currently windowing provider is determined at compile-time. This leads to a not so nice
way of doing generics for different providers not so nice.

Currently only GLFW is supported so if I ever decide to support SDL or the raw OS API instead this may turn
into a binding interface instead (Or at least most likely refactor how I do the generalization of function specification)
*/

#pragma once


#include "LAL/LAL.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "OSAL_Platform.hpp"
#include "SAL/GLFW_SAL.hpp"
#include "OSAL_Backend.hpp"


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
		using namespace Meta;

		template<EWindowingPlatform>
		struct WindowInfo_Maker;

		struct WindowInfo_PlatformAgnostic
		{
			RoCStr                WindowTitle;
			FrameBufferDimensions WindowSize ;
		};

		template<>
		struct WindowInfo_Maker<EWindowingPlatform::GLFW> : WindowInfo_PlatformAgnostic
		{
			using EBool       = GLFW::EBool      ;
			using EClient_API = GLFW::EClient_API;
			using Monitor     = GLFW::Monitor    ;
			using Window      = GLFW::Window     ;

			static constexpr auto WindowedMode = GLFW::WindowedMode;

			EBool                   Resizable        ;
			ptr<SAL::GLFW::Monitor> Windowed         ;
			ptr<Window>             WindowToShareWith;
		};

		using WindowInfo = WindowInfo_Maker<WindowingPlatform>;

		template<EWindowingPlatform>
		struct WindowingAPI;

		template<>
		struct WindowingAPI<EWindowingPlatform::GLFW>
		{
			using Window              = GLFW::Window;
			using WindowSize_Callback = GLFW::WindowSize_Callback;

			static ptr<Window> Create_Window(WindowInfo _windowCreationSpec)
			{
				using namespace SAL::GLFW;

				SetWindowCreationParameter(EWindowCreationParameter::ClientAPI, EClient_API::No_API          );
				SetWindowCreationParameter(EWindowCreationParameter::Resizable, _windowCreationSpec.Resizable);

				ptr<Window> window = 

					MakeWindow
					(
						_windowCreationSpec.WindowSize.Width,
						_windowCreationSpec.WindowSize.Height,
						_windowCreationSpec.WindowTitle,
						_windowCreationSpec.Windowed,
						_windowCreationSpec.WindowToShareWith
					);

				StringStream ptrString; ptrString << window;

				CLog("Window created with handle: " + ptrString.str() + "  Title: " + _windowCreationSpec.WindowTitle);

				return window;
			}

			static bool CanClose(const ptr<Window> _window)
			{
				return SAL::GLFW::CanClose(_window);
			}

			static void Destroy_Window(const ptr<Window> _window)
			{
				SAL::GLFW::DestroyWindow(_window);

				StringStream ptrString; ptrString << _window;

				CLog("Window: " + ptrString.str() + " destroyed");
			}

			static void SetWindow_SizeChangeCallback(const ptr<Window> _window, WindowSize_Callback _callback)
			{
				SAL::GLFW::SetWidnowSizeCallback(_window, _callback);
			}

			static OS_WindowHandle GetOS_WindowHandle(const ptr<Window> _window)
			{
				return SAL::GLFW::GetOS_WindowHandle(_window);
			}

			static FrameBufferDimensions GetFramebufferDimensions(const ptr<Window> _window)
			{
				int width, height;

				SAL::GLFW::GetFramebufferSize(_window, width, height);

				return FrameBufferDimensions(width, height);
			}
		};
	}

	using WindowInfo   = PlatformBackend::WindowInfo_Maker<WindowingPlatform>;
	using WindowingAPI = PlatformBackend::WindowingAPI    <WindowingPlatform>;

	using WinBool             = WindowInfo  ::EBool              ; 
	using Window              = WindowingAPI::Window             ;
	using WindowSize_Callback = WindowingAPI::WindowSize_Callback;

	constexpr auto Create_Window                = WindowingAPI::Create_Window               ;
	constexpr auto Destroy_Window               = WindowingAPI::Destroy_Window              ;
	constexpr auto CanClose                     = WindowingAPI::CanClose                    ;
	constexpr auto SetWindow_SizeChangeCallback = WindowingAPI::SetWindow_SizeChangeCallback;
	constexpr auto GetOS_WindowHandle           = WindowingAPI::GetOS_WindowHandle          ;
	constexpr auto GetFramebufferDimensions     = WindowingAPI::GetFramebufferDimensions    ;
}
