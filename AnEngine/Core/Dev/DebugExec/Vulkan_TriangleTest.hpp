/*
Triangle Test

Last Modified: 5/19/2020

https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code
*/



#pragma once



// Includes

#include "Core/IO/Basic_FileIO.hpp"
#include "Core/Memory/MemTypes.hpp"

#include "Meta/EngineInfo.hpp"
#include "Meta/Config/HAL_Config.hpp"

#include "Renderer/Shader/TriangleShader/TriangleShader.hpp"

#include "PAL/HAL/Vulkan/Vulkan_API.hpp"
#include "PAL/SAL/GLFW_SAL.hpp"
#include "HAL/Vulkan/GPU_Vulkan.hpp"

#include "LAL.hpp"



namespace Debug
{
	namespace Vulkan_TriangleTest
	{
		// Namespaces

		using namespace LAL;
		using namespace Meta;
		using namespace Vulkan;


		// Typedefs

		// GLFW

		using SAL::GLFW::Window;

		// Vulkan

		using AppInfo                     = AppInstance::AppInfo           ;
		using AppCreateInfo               = AppInstance::CreateInfo        ;
		using AppHandle                   = AppInstance::Handle            ;
		using Bytecode_Buffer             = DynamicArray< SPIR_V::Bytecode>;
		using CommandBufferList           = std::vector< VkCommandBuffer  >;   // TODO: Wrap
		using ExtensionIdentifierList     = std::vector< RoCStr           >;
		using FenceList                   = std::vector< VkFence          >;
		using FrameBufferList             = std::vector< VkFramebuffer    >;   // TODO: Wrap
		using ImageList                   = std::vector< Image::Handle    >;
		using ImageViewList               = std::vector< ImageView::Handle>;
		using SemaphoreList               = std::vector< VkSemaphore      >;   // TODO: Wrap
		using SurfaceFormatList           = std::vector< Surface::Format  >;
		using SurfacePresentationModeList = std::vector< EPresentationMode>;

		using namespace HAL::GPU::Platform_Vulkan;


		// Static Data

		// GLFW

		data< ptr<Window> > TriangleWindow = nullptr;

		data< int> Width = 1280, Height = 720;

		// Vulkan

		data< DataSize> CurrentFrame = 0;

		data< sInt32> MaxFramesInFlight = 2;

		bss< AppHandle> App;

		data< AppVersion> AppVer = { 1, 0, 0 };

		bss< CommandBufferList> CommandBuffers;

		bss< VkCommandPool> CommandPool;   // TODO: Wrap

		bss< Messenger::Handle    > DebugMessenger_Handle      ;
		bss< Messenger::CreateInfo> DebugMessenger_CreationSpec;

		bss< LogicalDevice::Handle       > LogicalDevice    ;
		bss< LogicalDevice::Queue::Handle> GraphicsQueue    ;
		bss< LogicalDevice::Queue::Handle> PresentationQueue;

		data< PhysicalDevice::Handle> PhysicalDevice = PhysicalDevice::NullHandle();

		bss< Pipeline::Layout::Handle> PipelineLayout;

		data< ExtensionIdentifierList> DeviceExtensions =
		{
			Swapchain_ExtensionName
		};

		bss< VkPipeline> GraphicsPipeline;

		bss< SemaphoreList> ImageAvailable_Semaphores;
		bss< SemaphoreList> RenderFinished_Semaphores;

		bss< FenceList> InFlightFences;
		bss< FenceList> ImagesInFlight;

		bss< Surface::Handle> SurfaceHandle;

		bss< SwapChain::Handle> SwapChain;

		bss< Extent2D     > SwapChain_Extent     ;
		bss< ImageList    > SwapChain_Images     ;
		bss< EFormat > SwapChain_ImageFormat;
		bss< ImageViewList> SwapChain_ImageViews ;

		data< bool> FramebufferResized = false;

		bss< FrameBufferList> SwapChain_Framebuffers;

		data< ValidationLayerList> ValidationLayerIdentifiers =
		{
			ValidationLayer_Khronos
		};

		bss< VkRenderPass> RenderPass;   // TODO: Wrap.


		// Functions

		void FramebufferSizeCallback(Window* _window, int width, int height)
		{
			//auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
			// Not necessary, this isn't object oriented.

			FramebufferResized = true;
		}

		void InitWindow()
		{
			using namespace SAL::GLFW;

			Initalize();

			using SAL::GLFW::EBool;

			SetWindowCreationParameter(EWindowCreationParameter::ClientAPI, EClient_API::No_API);
			SetWindowCreationParameter(EWindowCreationParameter::Resizable, EBool      ::True  );

			TriangleWindow =
				MakeWindow(Width, Height, "AnEngine: Triangle Test", WindowedMode, NotShared);

			//glfwSetWindowUserPointer(window, this);
			// Not necessary see FramebufferSizeCallback procedure.

			glfwSetWindowSizeCallback(TriangleWindow, FramebufferSizeCallback);   // TODO: Wrap
		}

		void InitVulkan()
		{
			CreateApplicationInstance("Triangle Test", AppVer, &ValidationLayerIdentifiers, &DebugMessenger_CreationSpec, App);

			SetupDebugMessenger(App, DebugMessenger_Handle);

			CreateSurface(App, TriangleWindow, SurfaceHandle);

			PickPhysicalDevice(App, PhysicalDevice, SurfaceHandle, DeviceExtensions);

			CreateLogicalDevice(PhysicalDevice, SurfaceHandle, DeviceExtensions, &ValidationLayerIdentifiers, LogicalDevice, GraphicsQueue, PresentationQueue);

			CreateSwapChain(TriangleWindow, PhysicalDevice, LogicalDevice, SurfaceHandle, SwapChain, SwapChain_ImageFormat, SwapChain_Extent, SwapChain_Images);

			CreateImageViews(LogicalDevice, SwapChain_Images, SwapChain_ImageFormat, SwapChain_ImageViews);

			CreateRenderPass(LogicalDevice, SwapChain_ImageFormat, RenderPass);

			CreateGraphicsPipeline(LogicalDevice, SwapChain_Extent, RenderPass, PipelineLayout, GraphicsPipeline);

			CreateFrameBuffers(LogicalDevice, RenderPass, SwapChain_Extent, SwapChain_ImageViews, SwapChain_Framebuffers);

			CreateCommandPool(PhysicalDevice, LogicalDevice, SurfaceHandle, CommandPool);

			CreateCommandBuffers(LogicalDevice, GraphicsPipeline, SwapChain_Framebuffers, SwapChain_Extent, RenderPass, CommandPool, CommandBuffers);;

			CreateSyncObjects(LogicalDevice, MaxFramesInFlight, SwapChain_Images, ImageAvailable_Semaphores, RenderFinished_Semaphores, InFlightFences, ImagesInFlight);
		}

		void RecreateSwapChain()
		{
			// TODO: Wrap

			int width = 0, height = 0;

			glfwGetFramebufferSize(TriangleWindow, &width, &height);

			while (width == 0 || height == 0) 
			{
				glfwGetFramebufferSize(TriangleWindow, &width, &height);

				glfwWaitEvents();
			}

			vkDeviceWaitIdle(LogicalDevice);

			CleanupSwapChain(LogicalDevice, PipelineLayout, GraphicsPipeline, SwapChain, SwapChain_ImageViews, SwapChain_Framebuffers, RenderPass, CommandPool, CommandBuffers);

			CreateSwapChain(TriangleWindow, PhysicalDevice, LogicalDevice, SurfaceHandle, SwapChain, SwapChain_ImageFormat, SwapChain_Extent, SwapChain_Images);

			CreateImageViews(LogicalDevice, SwapChain_Images, SwapChain_ImageFormat, SwapChain_ImageViews);
			CreateRenderPass(LogicalDevice, SwapChain_ImageFormat, RenderPass);
			CreateGraphicsPipeline(LogicalDevice, SwapChain_Extent, RenderPass, PipelineLayout, GraphicsPipeline);
			CreateFrameBuffers(LogicalDevice, RenderPass, SwapChain_Extent, SwapChain_ImageViews, SwapChain_Framebuffers);
			CreateCommandBuffers(LogicalDevice, GraphicsPipeline, SwapChain_Framebuffers, SwapChain_Extent, RenderPass, CommandPool, CommandBuffers);
			CreateCommandBuffers(LogicalDevice, GraphicsPipeline, SwapChain_Framebuffers, SwapChain_Extent, RenderPass, CommandPool, CommandBuffers);;
		}

		// TODO: Wrap
		void DrawFrame()
		{
			vkWaitForFences(LogicalDevice, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

			uint32 imageIndex;

			VkResult&& result = vkAcquireNextImageKHR(LogicalDevice, SwapChain, UINT64_MAX, ImageAvailable_Semaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);
			
			if (result == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				RecreateSwapChain();

				return;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
			{
				throw std::runtime_error("Failed to acquire swap chain image!");
			}

			if (ImagesInFlight[imageIndex] != VK_NULL_HANDLE) 
				vkWaitForFences(LogicalDevice, 1, &ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);

			ImagesInFlight[imageIndex] = InFlightFences[CurrentFrame];


			VkSubmitInfo submitInfo {};

			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


			VkSemaphore waitSemaphores[] = { ImageAvailable_Semaphores[CurrentFrame] };

			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores    = waitSemaphores;
			submitInfo.pWaitDstStageMask  = waitStages;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers    = &CommandBuffers[imageIndex];


			VkSemaphore signalSemaphores[] = { RenderFinished_Semaphores[CurrentFrame] };

			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores    = signalSemaphores;


			vkResetFences(LogicalDevice, 1, &InFlightFences[CurrentFrame]);

			if (vkQueueSubmit(GraphicsQueue, 1, &submitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS) 
				throw std::runtime_error("Failed to submit draw command buffer!");


			VkPresentInfoKHR presentInfo{};

			presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores    = signalSemaphores;


			VkSwapchainKHR swapChains[] = { SwapChain };

			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains    = swapChains;
			presentInfo.pImageIndices  = &imageIndex;

			presentInfo.pResults = nullptr; // Optional


			result = vkQueuePresentKHR(PresentationQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || FramebufferResized) 
			{
				FramebufferResized = false;

				RecreateSwapChain();
			}
			else if (result != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to present swap chain image!");
			}

			CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
		}

		void MainLoop()
		{
			using namespace SAL::GLFW;

			while (! SAL::GLFW::CanClose(TriangleWindow))
			{
				PollEvents();

				DrawFrame();
			}

			vkDeviceWaitIdle(LogicalDevice);   // TODO: Wrap 
		}

		void Cleanup()
		{
			using namespace SAL::GLFW;

			CleanupSwapChain(LogicalDevice, PipelineLayout, GraphicsPipeline, SwapChain, SwapChain_ImageViews, SwapChain_Framebuffers, RenderPass, CommandPool, CommandBuffers);

			for (size_t index = 0; index < MaxFramesInFlight; index++) 
			{
				vkDestroySemaphore(LogicalDevice, RenderFinished_Semaphores[index], nullptr);   // TODO: Wrap
				vkDestroySemaphore(LogicalDevice, ImageAvailable_Semaphores[index], nullptr);   // TODO: Wrap
				vkDestroyFence    (LogicalDevice, InFlightFences           [index], nullptr);   // TODO: Wrap
			}

			vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);   // TODO: Wrap

			LogicalDevice_Destory(LogicalDevice, nullptr);

			if (Vulkan_EnableValidationLayers) DestroyMessenger(App, DebugMessenger_Handle, nullptr);

			DestroySurface(App, SurfaceHandle, nullptr);

			AppInstance_Destory(App, nullptr);

			DestroyWindow(TriangleWindow);

			Terminate();
		}

		int Execute_VulkanTriTest()
		{
			try
			{
				InitWindow();

				InitVulkan();

				MainLoop();

				Cleanup();
			}
			catch (const std::exception& error)
			{
				std::cerr << error.what() << std::endl;

				getchar();

				return EXIT_FAILURE;
			}

			return EXIT_SUCCESS;
		}
	}
}
