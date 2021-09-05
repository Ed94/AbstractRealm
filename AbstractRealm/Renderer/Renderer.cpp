// Parent Header
#include "Renderer.hpp"


#include "Meta/EngineInfo.hpp"

#include "HAL.hpp"
#include "HAL/HAL_GPU.hpp"
#include "HAL/Vulkan/GPU_Vulkan.hpp"

#include "OSAL/OSAL_Hardware.hpp"

#include "TPAL_ImGui.hpp"


namespace Renderer
{
	Duration64 PresentInterval;

	namespace StaticData
	{
		ptr<OSAL::Window> EngineWindow;
	}

	OSAL::WindowInfo Engine_WidnowSpec;

	const Duration64& Get_PresentInterval()
	{
		return PresentInterval;
	}

	ptr<OSAL::Window> EngineWindow() { return StaticData::EngineWindow; }

	Meta::AppVersion AppVer =
	{
		Meta::EEngineVersion::Major,
		Meta::EEngineVersion::Minor,
		Meta::EEngineVersion::Patch
	};

	OSAL::FrameBufferDimensions WindowSize = { 1280, 720 };

	void Record_EditorDevDebugUI()
	{
		using namespace TPAL::Imgui;

		if (CollapsingHeader("Renderer"))
		{
			if (Table2C::Record())
			{
				Table2C::Entry("Present Interval", ToString(PresentInterval.count()));

				StringStream toString;

				if (Table2C::NestedTree("Engine Window"))
				{
					toString << StaticData::EngineWindow;

					Table2C::Entry("Handle", toString.str());

					#define Args(_Entry) NameOf(_Entry).str(), _Entry

					Table2C::Entry("Window Title", Engine_WidnowSpec.WindowTitle);

					Table2C::Entry("Window Size", ToString(WindowSize.Width) + String("x") + ToString(WindowSize.Height));

					Table2C::Entry("Windowed", (Engine_WidnowSpec.Windowed == nullptr) ? "True" : "False");

					Table2C::Entry(Args(Engine_WidnowSpec.Resizable));
					Table2C::Entry("Resisable2", nameOf(Engine_WidnowSpec.Resizable).data());

					#undef Args

					TreePop();
				}

				Table2C::EndRecord();
			}
		}
	}

	

	bool WindowResized = false;

	void WindowSizeChanged(ptr<OSAL::Window> /*_window*/, int _width, int _height)
	{
		WindowResized = true;

		WindowSize.Width = _width; WindowSize.Height = _height;
	}


	void Load()
	{
		Load_Backend();

		PresentInterval = Meta::FixRenderRateToRefreshRate() ? Duration64(1.0 / OSAL::Get_MainDisplay().RefreshRate) : Duration64(0.0);

		TPAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);

		// Temp situation due to not having a well fleshed out interface to the gpu hal backend.
		switch (Meta::GPU_API())
		{
			using namespace Meta;

			case Meta::EGPUPlatformAPI::Methane:
			{
				HAL::GPU::StartComms
				(
					String("Abstract Realm: MVP " +
					ToString(EEngineVersion::Major) + "." +
					ToString(EEngineVersion::Minor) + "." +
					ToString(EEngineVersion::Patch)).c_str(),
					AppVer
				);

				HAL::GPU::CeaseComms();

				break;
			} 

			case Meta::EGPUPlatformAPI::Vulkan:
			{
				HAL::GPU::StartComms
				(
					String("Abstract Realm: MVP " +
					ToString(EEngineVersion::Major) + "." +
					ToString(EEngineVersion::Minor) + "." +
					ToString(EEngineVersion::Patch)).c_str(),
					AppVer
				);

				// Window

				Engine_WidnowSpec = {};

				//windowSpec.WindowTitle = "Abstract Realm"               ;
				Engine_WidnowSpec.WindowTitle = "Modular Renderable Demo";
				Engine_WidnowSpec.WindowSize  = WindowSize;
				Engine_WidnowSpec.Windowed    = OSAL::WindowInfo::WindowedMode;
				Engine_WidnowSpec.Resizable   = OSAL::WinBool::True;

				StaticData::EngineWindow = OSAL::Create_Window(Engine_WidnowSpec);

				OSAL::SetWindow_SizeChangeCallback(StaticData::EngineWindow, WindowSizeChanged);

				HAL::GPU::Vulkan::Start_GPUVK_Demo(StaticData::EngineWindow);

				break;
			}

			case Meta::EGPUPlatformAPI::No_API:
			{
				Exception::Fatal::NotImplemented("Software rendering is not supported yet.");

				break;
			}
		}
	}

	void Unload()
	{
		switch (Meta::GPU_API())
		{
			case Meta::EGPUPlatformAPI::Methane:
				Exception::Fatal::NotImplemented("Software rendering is not supported yet.");
				[[fallthrough]];

			case Meta::EGPUPlatformAPI::Vulkan:
			{
				HAL::GPU::Vulkan::Stop_GPUVK_Demo();

				OSAL::Destroy_Window(StaticData::EngineWindow);

				HAL::GPU::CeaseComms();

				break;
			}

			case Meta::EGPUPlatformAPI::No_API:
			{
				Exception::Fatal::NotImplemented("Software rendering is not supported yet.");

				break;
			}
		}
	}
}