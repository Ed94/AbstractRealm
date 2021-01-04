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

	void GraphicsPipeline::Create(const RenderContext& _renderContext, const BasicShader& _shader, bool UseVertexInput, Bool Enable_Depth)
	{
		// Need a renderable object first...
		VertexInputState::CreateInfo vertexInputStateInfo;

		vertexInputStateInfo.BindingDescriptionCount = 0;   
		vertexInputStateInfo.BindingDescriptions     = nullptr;   

		vertexInputStateInfo.AttributeDescriptionCount = 0; 
		vertexInputStateInfo.AttributeDescription      = nullptr;

		InputAssemblyState::CreateInfo inputAssemblyStateInfo;

		inputAssemblyStateInfo.Topology               = EPrimitiveTopology::TriangleList;
		inputAssemblyStateInfo.PrimitiveRestartEnable = EBool::False;

		ViewportState::CreateInfo viewportStateInfo;

		// Hardcoded for now to 1...

		viewportStateInfo.ViewportCount = 1;
		viewportStateInfo.Viewports     = nullptr;
		viewportStateInfo.ScissorCount  = 1;
		viewportStateInfo.Scissors      = nullptr;

		MultiSampleState::CreateInfo multisampleStateInfo;

		multisampleStateInfo.RasterizationSamples  = ESampleCount::_1;
		multisampleStateInfo.EnableSampleShading   = EBool::False;
		multisampleStateInfo.MinSampleShading      = 1.0f;
		multisampleStateInfo.EnableAlphaToCoverage = EBool::False;
		multisampleStateInfo.EnableAlphaToOne      = EBool::False;

		DepthStencilState::CreateInfo depthStencilStateInfo;

		depthStencilStateInfo.DepthTestEnable       = Enable_Depth;
		depthStencilStateInfo.DepthWriteEnable      = Enable_Depth;
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

		ColorBlendState::AttachmentState colorBlendAttachmentState;

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

		ColorBlendState::CreateInfo colorBlendStateInfo;

		colorBlendStateInfo.EnableLogicOperations = EBool::False;
		colorBlendStateInfo.LogicOperation        = ELogicOperation::Copy;
		colorBlendStateInfo.AttachmentCount       = 1;
		colorBlendStateInfo.Attachments           = getAddress(colorBlendAttachmentState);
		colorBlendStateInfo.BlendConstants[0]     = 0.0f;
		colorBlendStateInfo.BlendConstants[1]     = 0.0f;
		colorBlendStateInfo.BlendConstants[2]     = 0.0f;
		colorBlendStateInfo.BlendConstants[3]     = 0.0f;

		RasterizationState::CreateInfo rasterizationStateInfo;

		rasterizationStateInfo.EnableDepthClamp = Enable_Depth;
		rasterizationStateInfo.PolygonMode      = EPolygonMode::Fill;

		rasterizationStateInfo.CullMode.Set(ECullModeFlag::Back);

		rasterizationStateInfo.FrontFace       = EFrontFace::Clockwise;
		rasterizationStateInfo.EnableDepthBias = EBool::False;

		rasterizationStateInfo.DepthBiasConstantFactor = 0;
		rasterizationStateInfo.DepthBiasClamp          = 0;
		rasterizationStateInfo.DepthBiasSlopeFactor    = 0;
		rasterizationStateInfo.LineWidth               = 1.0f;

		// Hardcoded to viewport and scissor for now (although almost if not all 3d rendered objects will need these.
		StaticArray<EDynamicState, 2> enabledDynamicStates = 
		{ 
			EDynamicState::Viewport, 
			EDynamicState::Scissor 
		};

		DynamicState::CreateInfo dynamicStateInfo;

		dynamicStateInfo.StateCount = enabledDynamicStates.size();
		dynamicStateInfo.States     = enabledDynamicStates.data();


		info.StageCount         = 2;   // BasicSahder only has 2 shader stages.
		info.Stages             = _shader.GetShaderStages();
		info.VertexInputState   = getAddress(vertexInputStateInfo);
		info.InputAssemblyState = getAddress(inputAssemblyStateInfo);
		info.TessellationState  = nullptr;   // None for now...
		info.ViewportState      = getAddress(viewportStateInfo);
		info.MultisampleState   = getAddress(multisampleStateInfo);
		info.DepthStencilState  = getAddress(depthStencilStateInfo);
		info.ColorBlendState    = getAddress(colorBlendStateInfo);
		info.RasterizationState = getAddress(rasterizationStateInfo);
		info.DynamicState       = getAddress(dynamicStateInfo);
		info.Layout             = Null<Layout::Handle>;
		info.RenderPass         = _renderContext.GetRenderPass();
		info.Subpass            = 0;   // Why?
		info.BasePipelineHandle = Null<Handle>;
		info.BasePipelineIndex  = 0;

		if (Parent::Create(GPU_Comms::GetEngagedDevice(), cache, info) != EResult::Success)
		{
			throw RuntimeError("Could not create graphics pipeline.");
		}
	}

#pragma endregion GraphicsPipeline
}