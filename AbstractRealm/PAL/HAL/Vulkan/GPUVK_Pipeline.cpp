// Parent Header
#include "GPUVK_Pipeline.hpp"






namespace HAL::GPU::Vulkan
{
#pragma region PipelineCache

	/*PipelineCache::PipelineCache()
	{
	}*/

	/*PipelineCache::~PipelineCache()
	{
	}*/

	void PipelineCache::Create()
	{
		if (Parent::Create(GPU_Comms::GetEngagedDevice(), info) != EResult::Success)
		{
			throw RuntimeError("Failed to create pipeline cache.");
		}
	}

#pragma endregion PipelineCache

#pragma region GraphicsPipeline

	void GraphicsPipeline::Create
	(
		const RenderPass&                         _renderPass       , 
		      ptr<const AShader>                  _shader              , 
		      DynamicArray<AttributeDescription>& _attributeDescriptions,
		      DynamicArray<BindingDescription  >& _bindingDescriptions  ,
			  ptr<const DescriptorSetLayout>  _descriptorSetLayout,   // one for now.
		      Bool                                _enableDepthClamp
	)
	{
		// Vertex Input State

		vertexInputStateInfo.BindingDescriptionCount = _bindingDescriptions.size();   
		vertexInputStateInfo.BindingDescriptions     = _bindingDescriptions.data();   

		vertexInputStateInfo.AttributeDescriptionCount = _attributeDescriptions.size(); 
		vertexInputStateInfo.AttributeDescriptions     = _attributeDescriptions.data();

		// Input Assembly State

		inputAssemblyStateInfo.Topology               = EPrimitiveTopology::TriangleList;
		inputAssemblyStateInfo.PrimitiveRestartEnable = EBool::False;

		// Viewport State

		// Hardcoded for now to 1...

		viewportStateInfo.ViewportCount = 1;
		viewportStateInfo.Viewports     = nullptr;
		viewportStateInfo.ScissorCount  = 1;
		viewportStateInfo.Scissors      = nullptr;

		// Multisample State

		multisampleStateInfo.RasterizationSamples  = ESampleCount::_1;
		multisampleStateInfo.EnableSampleShading   = EBool::False;
		multisampleStateInfo.MinSampleShading      = 1.0f;
		multisampleStateInfo.EnableAlphaToCoverage = EBool::False;
		multisampleStateInfo.EnableAlphaToOne      = EBool::False;

		// Depth Stencil State

		depthStencilStateInfo.DepthTestEnable       = _enableDepthClamp;
		depthStencilStateInfo.DepthWriteEnable      = _enableDepthClamp;
		depthStencilStateInfo.DepthCompareOp        = ECompareOperation::Less;
		depthStencilStateInfo.DepthBoundsTestEnable = EBool::False;

		depthStencilStateInfo.StencilTestEnable  = EBool::False;
		depthStencilStateInfo.Front.FailOp       = EStencilOperation::Keep;
		depthStencilStateInfo.Front.PassOp       = EStencilOperation::Keep;
		depthStencilStateInfo.Front.DepthFailOp  = EStencilOperation::Keep;
		depthStencilStateInfo.Front.CompareOp    = ECompareOperation::Always;
		depthStencilStateInfo.Front.CompareMask  = 0;
		depthStencilStateInfo.Front.WriteMask    = 0;
		depthStencilStateInfo.Front.Reference    = 0;
		depthStencilStateInfo.Back               = depthStencilStateInfo.Front;

		depthStencilStateInfo.MinDepthBounds = 0;
		depthStencilStateInfo.MaxDepthBounds = 0;

		// Color Blend Attachment State

		colorBlendAttachmentState.EnableBlend                  = EBool::False;
		colorBlendAttachmentState.Source_ColorBlendFactor      = EBlendFactor::Zero;
		colorBlendAttachmentState.Destination_ColorBlendFactor = EBlendFactor::Zero;
		colorBlendAttachmentState.ColorOperation               = EBlendOperation::Add;
		colorBlendAttachmentState.Source_AlphaBlendFactor      = EBlendFactor::Zero;
		colorBlendAttachmentState.Destination_AlphaBlendFactor = EBlendFactor::Zero;
		colorBlendAttachmentState.AlphaOperation               = EBlendOperation::Add;

		colorBlendAttachmentState.ColorWriteMask.Set
		(
			EColorComponentFlag::R,
			EColorComponentFlag::G,
			EColorComponentFlag::B,
			EColorComponentFlag::A
		);

		// Color Blend State

		colorBlendStateInfo.EnableLogicOperations = EBool::False;
		colorBlendStateInfo.LogicOperation        = ELogicOperation::Copy;
		colorBlendStateInfo.AttachmentCount       = 1;
		colorBlendStateInfo.Attachments           = getPtr(colorBlendAttachmentState);
		colorBlendStateInfo.BlendConstants[0]     = 0.0f;
		colorBlendStateInfo.BlendConstants[1]     = 0.0f;
		colorBlendStateInfo.BlendConstants[2]     = 0.0f;
		colorBlendStateInfo.BlendConstants[3]     = 0.0f;

		// Rasterization State

		rasterizationStateInfo.EnableDepthClamp = _enableDepthClamp;
		rasterizationStateInfo.PolygonMode      = EPolygonMode::Line;

		rasterizationStateInfo.CullMode.Set(ECullModeFlag::Back);

		rasterizationStateInfo.FrontFace       = EFrontFace::Clockwise;
		rasterizationStateInfo.EnableDepthBias = EBool::False;

		rasterizationStateInfo.DepthBiasConstantFactor = 0;
		rasterizationStateInfo.DepthBiasClamp          = 0;
		rasterizationStateInfo.DepthBiasSlopeFactor    = 0;
		rasterizationStateInfo.LineWidth               = 1.0f;

		// Dynamic States

		// Hardcoded to viewport and scissor for now (although almost if not all 3d rendered objects will need these.
		enabledDynamicStates = 
		{ 
			EDynamicState::Viewport, 
			EDynamicState::Scissor 
		};

		dynamicStateInfo.StateCount = enabledDynamicStates.size();
		dynamicStateInfo.States     = enabledDynamicStates.data();

		// Layout

		//Layout::CreateInfo layoutInfo;

		//layoutInfo.SetLayoutCount = RCast<ui32>(_descriptorSetLayouts.size());  // hardcoded for now.
		layoutInfo.SetLayoutCount = 1;  
		layoutInfo.SetLayouts     = *_descriptorSetLayout;
		layoutInfo.PushConstantRangeCount = 0;
		layoutInfo.PushConstantRanges = nullptr;


		if (layout.Create(GPU_Comms::GetEngagedDevice(), layoutInfo) != EResult::Success)
		{
			throw RuntimeError("Could not create pipeline layout.");
		}



		info.StageCount         = _shader->GetShaderStageInfos().size();   // BasicSahder only has 2 shader stages.
		info.Stages             = _shader->GetShaderStageInfos().data();
		info.VertexInputState   = getPtr(vertexInputStateInfo);
		info.InputAssemblyState = getPtr(inputAssemblyStateInfo);
		info.TessellationState  = nullptr;   // None for now...
		info.ViewportState      = getPtr(viewportStateInfo);
		info.MultisampleState   = getPtr(multisampleStateInfo);
		info.DepthStencilState  = getPtr(depthStencilStateInfo);
		info.ColorBlendState    = getPtr(colorBlendStateInfo);
		info.RasterizationState = getPtr(rasterizationStateInfo);
		info.DynamicState       = getPtr(dynamicStateInfo);
		info.Layout             = layout;
		info.RenderPass         = _renderPass;
		info.Subpass            = 0;   // Why?
		info.BasePipelineHandle = Null<Handle>;
		info.BasePipelineIndex  = 0;

		if (Parent::Create(GPU_Comms::GetEngagedDevice(), GPU_Pipeline::Request_Cache(), info) != EResult::Success)
		{
			throw RuntimeError("Could not create graphics pipeline.");
		}
	}

	ptr<const GraphicsPipeline::ShaderStage::CreateInfo> GraphicsPipeline::GetShaderStages() const
	{
		return info.Stages;
	}

	const GraphicsPipeline::VertexInputState::CreateInfo& GraphicsPipeline::GetVertexInputState() const
	{
		return vertexInputStateInfo;
	}

	const GraphicsPipeline::Layout& GraphicsPipeline::GetLayout() const
	{
		return layout;
	}

	/*const DescriptorSetLayout& GraphicsPipeline::GetDescriptorSetLayout() const
	{
		return layoutInfo.SetLayouts;
	}*/

#pragma endregion GraphicsPipeline

#pragma region GPU_Pipeline

const PipelineCache& GPU_Pipeline::Request_Cache()
{
	return cache;
}

// Protected

PipelineCache GPU_Pipeline::cache;

#pragma endregion GPU_Pipeline
}