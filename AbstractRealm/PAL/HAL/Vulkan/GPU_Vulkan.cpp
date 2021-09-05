// Parent Header
#include "GPU_Vulkan.hpp"

// TUtorial: This needs to be removed from GPU_Vulkan
// This can be done by making the design of the GPU HAL agnostic to what the tutorial requires.
#include "_TutorialRelated.hpp"

// Engine
#include "HAL_Backend.hpp"
#include "GPUVK_Comms.hpp"
#include "GPUVK_PayloadDeck.hpp"
#include "GPUVK_Pipeline.hpp"
#include "GPUVK_Rendering.hpp"

#include "Dev/Console.hpp"

#if VulkanAPI_Interface == VaultedVulkan_Interface

namespace HAL::GPU::Vulkan
{
#pragma region StaticData

		RawRenderContext RenderContext_Default;   // Should this still be used?

		ptr<Surface>       GPUVKDemo_Surface;
		ptr<Swapchain>     GPUVKDemo_Swap;
		ptr<RenderContext> GPUVKDemo_Context;

		BasicShader ModelWTxtur_Shader;

		BasicShader TriangleDemo_Shader;

		ptr<ARenderable> TriangleDemo_Renderable;
		ptr<ARenderable> ModelWTexur_Renderable;

		using ImageBytes = stbi_uc;

		ptr<ImageBytes> ModelWTxtur_TxtImage;

		int Model_TxtWidth, Model_TxtHeight, Model_TxtChannels;

#pragma endregion


	void SetRenderContext();

	void Start_GPUVK_Demo(ptr<OSAL::Window> _window_in)
	{
		GPUVKDemo_Surface = getPtr(Rendering::Request_Surface(_window_in));

		Surface::Format format;

		format.Format     = EFormat::B8_G8_R8_A8_UNormalized;
		format.ColorSpace = EColorSpace::SRGB_NonLinear;

		GPUVKDemo_Swap    = getPtr(Rendering::Request_SwapChain    (dref(GPUVKDemo_Surface), format));
		GPUVKDemo_Context = getPtr(Rendering::Request_RenderContext(dref(GPUVKDemo_Swap)));

		ModelWTxtur_Shader.Create
		(
			String(Renderer::Shader::Paths::VKTut) + "VertexShaderV5.vert",
			String(Renderer::Shader::Paths::VKTut) + "FragmentShaderV5.frag",
			sizeof(UniformBufferObject)
		);

		LoadModel(VikingRoom_ModelPath);

		ModelWTxtur_TxtImage = stbi_load(VikingRoom_TexturePath.c_str(), &Model_TxtWidth, &Model_TxtHeight, &Model_TxtChannels, STBI_rgb_alpha);

		ModelWTexur_Renderable = Resources::Request_Renderable
		(
			ModelVerticies,
			ModelIndicies,
			ModelWTxtur_TxtImage, Model_TxtWidth, Model_TxtHeight,
			getPtr(ModelWTxtur_Shader)
		);

		GPUVKDemo_Context->AddRenderable(ModelWTexur_Renderable);

		//AddTestCallback();

		SetRenderContext();
	}

	//DynamicArray<RenderCallback> RenderCallbacks;

	void AddRenderCallback(RenderCallback _renderCallback_in)
	{
		GPUVKDemo_Context->AddRenderCallback(_renderCallback_in);
	}

	void UpdateUniformBuffers();

	void Render()
	{
		UpdateUniformBuffers();

		Rendering::Update();
	}

	void Present()
	{
		Rendering::Present();
	}

	void Stop_GPUVK_Demo()
	{
		Log("Stopping Clear Color Demo...");

		stbi_image_free(ModelWTxtur_TxtImage);

		ModelWTxtur_Shader.Destroy();

		Rendering::Retire_RenderContext(GPUVKDemo_Context);
		Rendering::Retire_SwapChain    (GPUVKDemo_Swap   );
		Rendering::Retire_Surface      (GPUVKDemo_Surface);
	}

#pragma region Staying

	u32 GetMinimumFramebufferCount()
	{
		return 2;
	}

	u32 GetNumberOfFramebuffers()
	{
		return SCast<u32>(GPUVKDemo_Swap->GetImages().size());
	}

	const CommandBuffer& RequestSingleTimeBuffer()
	{
		return Deck::RecordOnTransient();
	}

	void EndSingleTimeBuffer(const CommandBuffer& _buffer_in)
	{
		Deck::EndRecordOnTransient(_buffer_in);
	}

	EResult RequestDescriptorPool(V3::DescriptorPool& _pool_out, V3::DescriptorPool::CreateInfo _info)
	{
		EResult returnCode = _pool_out.Create(Comms::GetEngagedDevice(), _info);

		return returnCode;
	}

#pragma endregion Staying

	void UpdateUniformBuffers()
	{
		using namespace TP_API;

		static auto _StartTime = HighResClock::now();

		auto currentTime = HighResClock::now();

		float time = Duration32(currentTime - _StartTime).count();

		UniformBufferObject ubo {};

		ubo.ModelSpace = glm::rotate(glm::mat4(1.0f), time * glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		ubo.Viewport = glm::lookAt(glm::vec3(1.7f, 1.7f, 1.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));   // The default
		//ubo.Viewport = glm::lookAt(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); crying cat

		ubo.Projection = glm::perspective(glm::radians(45.0f), GPUVKDemo_Swap->GetExtent().Width / (float) GPUVKDemo_Swap->GetExtent().Height, 0.1f, 10.0f);

		ubo.Projection[1][1] *= -1;

		ptr<const void> address = getPtr(ubo);

		ModelWTexur_Renderable->UpdateUniforms(address, sizeof(ubo));
	}

	// GPU_HAL

	void Start_GPUComms(RoCStr _applicationName, Meta::AppVersion _applicationVersion)
	{
		Comms::Initialize(_applicationName, _applicationVersion);

		Comms::AcquirePhysicalDevices();

		Comms::GenerateLogicalDevices();

		Comms::EngageMostSuitableDevice();

		Deck::Prepare();
	}

	void CeaseComms()
	{
		Deck::Wipe();

		Resources::Clear();

		Comms::Cease();	
	}

	void WaitForIdle()
	{
		Comms::GetEngagedDevice().WaitUntilIdle();
	}
	
	ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> /* _window */)
	{
		return &RenderContext_Default;
	}

	void SetRenderContext()
	{
		RenderContext_Default.ApplicationInstance = Comms::GetAppInstance();
		RenderContext_Default.PhysicalDevice      = Comms::GetEngagedDevice().GetPhysicalDevice();
		RenderContext_Default.LogicalDevice       = Comms::GetEngagedDevice();
		RenderContext_Default.Queue               = Comms::GetEngagedDevice().GetGraphicsQueue();
		RenderContext_Default.PipelineCache       = PipelineManager::Request_Cache();
		RenderContext_Default.ImageFormat         = GPUVKDemo_Swap->GetFormat();
		RenderContext_Default.FrameSize           = GPUVKDemo_Swap->GetExtent();
		RenderContext_Default.Allocator           = Memory::DefaultAllocator;
		RenderContext_Default.RenderPass          = GPUVKDemo_Context->GetRenderPass();
		RenderContext_Default.MinimumFrameBuffers = GPUVKDemo_Swap->GetMinimumImageCount();
		RenderContext_Default.FrameBufferCount    = SCast<u32>(GPUVKDemo_Swap->GetImages().size());
		RenderContext_Default.MSAA_Samples        = ESampleCount::_1;
	}

	void Default_InitializeRenderer(ptr<Window> _window_in)
	{
	}

	void Default_ReinitializeRenderer(ptr<Window> _window_in)
	{
	}

	DynamicArray<CommandBuffer::Handle> CommandBuffersToSubmit;

	void Default_DrawFrame(ptr<Window> _window_in)
	{
	}

	void Default_DeinitializeRenderer(ptr<OSAL::Window> /* _window_in */)
	{
	}
}

#endif