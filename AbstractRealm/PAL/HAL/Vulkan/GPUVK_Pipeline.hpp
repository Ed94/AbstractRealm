#pragma once


#include "GPUVK_Comms.hpp"

#include "GPUVK_Rendering.hpp"
#include "GPUVK_Shaders.hpp"


namespace HAL::GPU::Vulkan
{
	class PipelineCache : public V3::Pipeline::Cache
	{
		using Parent = V3::Pipeline::Cache;

	public:
		 //PipelineCache();
		//~PipelineCache();

		void Create();

	private:

		CreateInfo info;
	};


	class GraphicsPipeline : public V3::GraphicsPipeline
	{
		using Parent = V3::GraphicsPipeline;

	public:
		 //GraphicsPipeline();
		//~GraphicsPipeline();

		void Create(const RenderContext& _renderContext, const BasicShader& _shader, bool UseVertexInput, Bool Enable_DepthClamp);

	private:

		CreateInfo info;   // Should this be kept?

		PipelineCache cache;

		DescriptorSetLayout descriptorSetLayout;

		Layout layout;
	};
	
}
