#pragma once


#include "GPUVK_Comms.hpp"

//#include "GPUVK_Rendering.hpp"
#include "GPUVK_Shaders.hpp"


namespace HAL::GPU::Vulkan
{
	class PipelineCache : public V3::Pipeline::Cache
	{
		using Parent = V3::Pipeline::Cache;

	public:

		void Create();

	protected:

		CreateInfo info;
	};

	class GraphicsPipeline : public V3::GraphicsPipeline
	{
		using Parent = V3::GraphicsPipeline;

	public:
		
		using AttributeDescription = Pipeline::VertexInputState::AttributeDescription;
		using BindingDescription   = Pipeline::VertexInputState::BindingDescription;

		void Create
		(
			const RenderPass&                         _renderContext        , 
			      ptr<const AShader>                  _shader               , 
			      DynamicArray<AttributeDescription>& _attributeDescriptions, 
				  DynamicArray<BindingDescription  >& _bindingDescriptions  ,
			      ptr<const DescriptorSetLayout>      _descriptorSetLayout,
			      Bool                                _enableDepthClamp
		);

		ptr<const ShaderStage::CreateInfo> GetShaderStages() const;

		const VertexInputState::CreateInfo& GetVertexInputState() const;

		const Layout& GetLayout() const;

		//const DescriptorSetLayout& GetDescriptorSetLayout() const;

	protected:

		VertexInputState  ::CreateInfo      vertexInputStateInfo;
		InputAssemblyState::CreateInfo      inputAssemblyStateInfo;
		ViewportState     ::CreateInfo      viewportStateInfo;
		MultiSampleState  ::CreateInfo      multisampleStateInfo;
		DepthStencilState ::CreateInfo      depthStencilStateInfo;
		ColorBlendState   ::AttachmentState colorBlendAttachmentState;
		ColorBlendState   ::CreateInfo      colorBlendStateInfo;
		RasterizationState::CreateInfo      rasterizationStateInfo;
		DynamicArray<EDynamicState>         enabledDynamicStates;
		DynamicState      ::CreateInfo      dynamicStateInfo;
		Layout            ::CreateInfo      layoutInfo;

		CreateInfo info;   // Should this be kept?

		DescriptorSetLayout descriptorSetLayout;

		Layout layout;
	};


	class PipelineManager
	{

	public:

		static const PipelineCache& Request_Cache();

	protected:

		static DynamicArray<Pipeline::Layout> layouts;

		static PipelineCache cache;
	};
}
