// Parent Header
#include "SAL_ImGui.hpp"


// Engine
#include "Memory/MemTracking.hpp"
#include "HAL_GPU.hpp"
#include "Console.hpp"


#include "Meta/Config/CoreDev_Config.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "Meta/Config/Resource_Config.hpp"
#include "Meta/Config/Simulation_Config.hpp"

#include "PAL/OSAL//OSAL_Hardware.hpp"
#include "cereal/archives/json.hpp"
#include "ctti/nameof.hpp"
#include "ctti/name.hpp"
#include "ctti/detailed_nameof.hpp"
#include "nameof.hpp"

#include "imgui/backends/imgui_impl_vulkan.h"	
#include "imgui/backends/imgui_impl_glfw.h"


namespace SAL::Imgui
{
	using namespace C_API;

	using namespace Memory;

	UnorderedMap<String, DynamicArray<WindowCallback>> WindowsQueued;

	void CLog(String _info)
	{
		Dev::CLog("Imgui: " + _info);
	}

	namespace PlatformBackend
	{
		StaticData()

			// GLFW

			bool GLFW_InstallCallbacks = true;

			// Vulkan

			HAL::GPU::Vulkan::RawRenderContext* RenderContext;

			VV::V3::GraphicsPipeline GraphicsPipeline;

			VV::V3::DescriptorPool DescriptorPool;

			DynamicArray<VV::V3::Framebuffer> SwapChain_Framebuffers;


		// Functions

		void Vulkan_CreateDescriptorObjects()
		{
			using namespace VV::V3;

			DescriptorPool::Size pool_sizes[11];

			pool_sizes[0 ].Type = EDescriptorType::Sampler             ; pool_sizes[0 ].Count = 1000;
			pool_sizes[1 ].Type = EDescriptorType::CombinedImageSampler; pool_sizes[1 ].Count = 1000;
			pool_sizes[2 ].Type = EDescriptorType::SampledImage        ; pool_sizes[2 ].Count = 1000;
			pool_sizes[3 ].Type = EDescriptorType::StorageImage        ; pool_sizes[3 ].Count = 1000;
			pool_sizes[4 ].Type = EDescriptorType::UniformTexelBuffer  ; pool_sizes[4 ].Count = 1000;
			pool_sizes[5 ].Type = EDescriptorType::StorageTexelBuffer  ; pool_sizes[5 ].Count = 1000;
			pool_sizes[6 ].Type = EDescriptorType::UniformBuffer       ; pool_sizes[6 ].Count = 1000;
			pool_sizes[7 ].Type = EDescriptorType::StorageBuffer       ; pool_sizes[7 ].Count = 1000;
			pool_sizes[8 ].Type = EDescriptorType::UniformBufferDynamic; pool_sizes[8 ].Count = 1000;
			pool_sizes[9 ].Type = EDescriptorType::StorageBufferDynamic; pool_sizes[9 ].Count = 1000;
			pool_sizes[10].Type = EDescriptorType::InputAttachment     ; pool_sizes[10].Count = 1000;

			DescriptorPool::CreateInfo info;

			info.Flags.Set(EDescriptorPoolCreateFlag::FreeDescriptorSet);

			info.MaxSets       = 1000 *  ((int)(sizeof(pool_sizes)) / sizeof(*(pool_sizes)));   // TODO: Define this better
			info.PoolSizeCount = (u32)  ((int)(sizeof(pool_sizes)) / sizeof(*(pool_sizes)));
			info.PoolSizes     = pool_sizes;

			HAL::GPU::Vulkan::RequestDescriptorPool(DescriptorPool, info);
		}

		void Vulkan_DebugCallback(VkResult _returnCode)
		{
			if (_returnCode != VkResult(VV::V3::EResult::Success))
			{
				std::cerr << "Imgui Callback Error Code: " << int(_returnCode) << std::endl;

				CLog("Imgui Callback Error Code: " + ToString(int(_returnCode)));
			}
		}

		void Vulkan_GPU_DrawRender(const HAL::GPU::Vulkan::CommandBuffer& _buffer, int /* _index */)
		{
			ImDrawData* draw_data = ImGui::GetDrawData();

			// Record dear Imgui primitives into command buffer
			ImGui_ImplVulkan_RenderDrawData(draw_data, _buffer);
		}
	}

	// Static Data

	Imgui::IO* IO_Config;


	void Dirty_DoSurfaceStuff(ptr<OSAL::Window> _window)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		glfwMakeContextCurrent(_window);
	}


	// Functions

	void Queue(RoCStr _windowName, WindowCallback _callback)
	{
		auto possibleWindow = WindowsQueued.find(String(_windowName));

		if (possibleWindow != WindowsQueued.end())
		{
			possibleWindow->second.push_back(_callback);
		}
		else
		{
			auto callbacks = DynamicArray<WindowCallback>();

			callbacks.push_back(_callback);

			WindowsQueued.insert({_windowName, move(callbacks) });
		}
	}

	void Dequeue(RoCStr _windowName, WindowCallback _callback)
	{
		auto possibleWindow = WindowsQueued.find(String(_windowName));

		if (possibleWindow != WindowsQueued.end())
		{
			WindowsQueued.erase(possibleWindow);
		}
	}

	void BindToPlatformAndRenderer(ptr<OSAL::Window> _window)
	{
		switch (Meta::WindowingPlatform)
		{
			case Meta::EWindowingPlatform::GLFW:
			{
				ImGui_ImplGlfw_InitForVulkan(_window, PlatformBackend::GLFW_InstallCallbacks);

				CLog("GLFW callbacks setup");

				break;
			}
			case Meta::EWindowingPlatform::OSAL:
			{
				throw NotImplementedException();

				break;
			}
		}

		switch (Meta::GPU_API())
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				using namespace HAL::GPU;

				using HAL::GPU::Vulkan::RawRenderContext;

				using namespace PlatformBackend;

				RenderContext = RCast<RawRenderContext>(GetRenderContext(_window));

				ImGui_ImplVulkan_InitInfo initSpec{};

				initSpec.Instance        = RenderContext->ApplicationInstance   ;
				initSpec.PhysicalDevice  = RenderContext->PhysicalDevice        ;
				initSpec.Device          = RenderContext->LogicalDevice         ;
				initSpec.QueueFamily     = RenderContext->Queue.GetFamilyIndex();
				initSpec.Queue           = RenderContext->Queue                 ;
				initSpec.PipelineCache   = RenderContext->PipelineCache         ;
				initSpec.Allocator       = RenderContext->Allocator->operator const C_API::VkAllocationCallbacks*();
				initSpec.MinImageCount   = RenderContext->MinimumFrameBuffers   ;
				initSpec.ImageCount      = RenderContext->FrameBufferCount      ;
				initSpec.MSAASamples     = C_API::VkSampleCountFlagBits(RenderContext->MSAA_Samples);
				initSpec.CheckVkResultFn = PlatformBackend::Vulkan_DebugCallback;

				SetupGPU_Interface();

				initSpec.DescriptorPool = DescriptorPool;

				ImGui_ImplVulkan_Init(&initSpec, RenderContext->RenderPass);

				CLog("Hooked onto Vulkan backend");

				//ImGui_ImplVulkan_SetMinImageCount(HAL::GPU::Vulkan::GetMinimumFramebufferCount());

				break;
			}
			case Meta::EGPUPlatformAPI::No_API:
			{
				throw RuntimeError("No API Selected");

				break;
			}
		}
	}

	void Deinitialize()
	{
		ImGui_ImplVulkan_Shutdown();

		PlatformBackend::DescriptorPool.Destroy();

		CLog("Deinitialized");
	}

	void Initialize(ptr<OSAL::Window> _window)
	{
		CLog("Initializing...");

		using namespace SAL;

		Imgui::VerifyVersion();

		Imgui::CreateContext();

		IO_Config = &Imgui::GetIO(); (void)IO_Config;   // TODO: Find out what that (void) is for...

		IO_Config->ConfigFlags |= ImGuiConfigFlags_DockingEnable  ;
		IO_Config->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		Imgui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		if (IO_Config->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 0.0f;
		}

		Imgui::BindToPlatformAndRenderer(_window);

		Imgui::SetupFonts();
	}

	void Text(RoCStr _cStr)
	{
		ImGui::Text(_cStr);
	}

	void Text(const StringView& _view)
	{
		ImGui::Text(_view.data());
	}

	void Text(const String& _string)
	{
		ImGui::Text(_string.c_str());
	}

	bool TreeNode(RoCStr _cStr)
	{
		return ImGui::TreeNode(_cStr);
	}

	bool TreeNode(const StringView& _view)
	{
		return ImGui::TreeNode(_view.data());
	}

	bool TreeNode(const String& _string)
	{
		return ImGui::TreeNode(_string.c_str());
	}

	bool CollapsingHeader(RoCStr _cStr)
	{
		return ImGui::CollapsingHeader(_cStr);
	}

	bool CollapsingHeader(const StringView& _view)
	{
		return ImGui::CollapsingHeader(_view.data());
	}

	bool CollapsingHeader(const String& _string)
	{
		return ImGui::CollapsingHeader(_string.c_str());
	}

	Table2C::Table2C() : 
		name("Data Table"),
		flags(ImGuiTableFlags_SizingFixedFit |ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)
	{}

	Table2C::Table2C(RoCStr _name) : 
		name(_name)
	{}

	Table2C::Table2C(const String& _name) : 
		name(_name)
	{}

	Table2C::Table2C(Flags _flags) : 
		flags(_flags)
	{}

	Table2C::Table2C(RoCStr _name, Flags _flags) :
		name(_name), flags(_flags)
	{}

	Table2C::Table2C(const String& _name, Flags _flags) :
		name(_name), flags(_flags)
	{}

	bool Table2C::Record()
	{
		bool result = ImGui::BeginTable("Data Table", 2, ImGuiTableFlags_SizingFixedFit |ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders);

		if (result)
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableHeadersRow();
		}

		return result;	
	}

	void Table2C::EndRecord()
	{
		ImGui::EndTable();
	}

	void MakeFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame  ();

		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport();

		for (auto& entry : WindowsQueued)
		{
			ImGui::Begin(entry.first.c_str());

			for (auto& callback : entry.second)
			{
				callback();
			}

			ImGui::End();
		}

	#define Args(_Entry) NameOf(_Entry).str(), _Entry

		ImGui::ShowDemoWindow();

	#undef Args
	}

	void SetupFonts()
	{
		switch (Meta::GPU_API())
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				using namespace HAL::GPU::Vulkan;

				const HAL::GPU::Vulkan::CommandBuffer& cmdBuffer = RequestSingleTimeBuffer();

				ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);

				EndSingleTimeBuffer(cmdBuffer);

				break;
			}
			case Meta::EGPUPlatformAPI::No_API:
			{
				throw RuntimeError("No API Selected.");
			}
		}
	}

	void SetupGPU_Interface()
	{
		switch (Meta::GPU_API())
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				using namespace HAL::GPU;

				PlatformBackend::Vulkan_CreateDescriptorObjects();

				HAL::GPU::Vulkan::AddRenderCallback(PlatformBackend::Vulkan_GPU_DrawRender);

				break;
			}
			case Meta::EGPUPlatformAPI::No_API:
			{
				throw RuntimeError("No API Selected.");
			}
		}
	}	
	
	void VerifyVersion()
	{
		IMGUI_CHECKVERSION();
	}
}