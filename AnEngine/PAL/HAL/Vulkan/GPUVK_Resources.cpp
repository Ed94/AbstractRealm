// Parent Header
#include "GPUVK_Resources.hpp"



#include "GPUVK_PayloadDeck.hpp"




namespace HAL::GPU::Vulkan
{
	bool Image::HasStencilComponent()
	{
		return info.Format == EFormat::D32_SFloat_S8_UInt ||  info.Format  == EFormat::D24_UNormalized_S8_UInt;
	}

	EResult Image::TransitionLayout(EImageLayout _old, EImageLayout _new)
	{
		auto commandBuffer = RecordOnTransient();

		Image::Memory_Barrier barrier {};

		barrier.OldLayout = _old;
		barrier.NewLayout = _new;

		barrier.SrcQueueFamilyIndex = QueueFamily_Ignored;
		barrier.DstQueueFamilyIndex = QueueFamily_Ignored;

		barrier.Image = handle;

		barrier.SubresourceRange.AspectMask.Set(EImageAspect::Color);

		if (_new == EImageLayout::DepthStencil_AttachmentOptimal)
		{
			barrier.SubresourceRange.AspectMask.Set(EImageAspect::Depth);

			if (HasStencilComponent())
			{
				barrier.SubresourceRange.AspectMask.Add(EImageAspect::Stencil);
			}
		}
		else
		{
			barrier.SubresourceRange.AspectMask.Set(EImageAspect::Color);
		}

		barrier.SubresourceRange.BaseMipLevel   = 0                ;
		barrier.SubresourceRange.LevelCount     = info.MipmapLevels;
		barrier.SubresourceRange.BaseArrayLayer = 0                ;
		barrier.SubresourceRange.LayerCount     = 1                ;

		Pipeline::StageFlags sourceStage     ;
		Pipeline::StageFlags destinationStage;

		if (_old == EImageLayout::Undefined && _new == EImageLayout::TransferDestination_Optimal)
		{
			barrier.SrcAccessMask = 0;

			barrier.DstAccessMask.Set(EAccessFlag::TransferWrite);

			sourceStage     .Set(EPipelineStageFlag::TopOfPipe);
			destinationStage.Set(EPipelineStageFlag::Transfer );
		}
		else if (_old == EImageLayout::TransferDestination_Optimal && _new == EImageLayout::Shader_ReadonlyOptimal)
		{
			barrier.SrcAccessMask.Set(EAccessFlag::TransferWrite);
			barrier.DstAccessMask.Set(EAccessFlag::ShaderRead   );

			sourceStage     .Set(EPipelineStageFlag::Transfer       );
			destinationStage.Set(EPipelineStageFlag::FragementShader);
		}
		else if (_old == EImageLayout::Undefined && _new == EImageLayout::DepthStencil_AttachmentOptimal)
		{
			barrier.SrcAccessMask = 0;

			barrier.DstAccessMask.Set(EAccessFlag::DepthStencilAttachmentRead, EAccessFlag::DepthStencilAttachmentWrite);

			sourceStage     .Set(EPipelineStageFlag::TopOfPipe         );
			destinationStage.Set(EPipelineStageFlag::EarlyFragmentTests);
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		commandBuffer.SubmitPipelineBarrier
		(
			sourceStage, destinationStage,   // TODO
			0, 
			1, &barrier
		);

		Heap(EndRecordOnTransient(commandBuffer));
	}


	Image::operator Parent& ()
	{
		return *static_cast<Parent*>(this);
	}
}