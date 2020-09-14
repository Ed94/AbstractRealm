// Parent Header
#include "GPUVK_Resources.hpp"



#include "GPUVK_PayloadDeck.hpp"
#include "GPUVK_Memory.hpp"




namespace HAL::GPU::Vulkan
{
#pragma region Image

	EResult Image::Create(const LogicalDevice& _device, const CreateInfo& _info)
	{
		info = _info;

		return Parent::Create(_device, info);
	}

	EResult Image::Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator)
	{
		info = _info;

		return Parent::Create(_device, _info);
	}

	EFormat Image::GetFormat() const
	{
		return info.Format;
	}

	bool Image::HasStencilComponent() const
	{
		return info.Format == EFormat::D32_SFloat_S8_UInt ||  info.Format  == EFormat::D24_UNormalized_S8_UInt;
	}

	void Image::TransitionLayout(EImageLayout _old, EImageLayout _new)
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

		Heap() EndRecordOnTransient(commandBuffer);
	}


	Image::operator Parent& ()
	{
		return *static_cast<Parent*>(this);
	}

#pragma endregion Image

#pragma region ImageView

	EResult ImageView::Create(const LogicalDevice& _device, const CreateInfo& _info)
	{
		info = _info;

		return Parent::Create(_device, _info);
	}

	EResult ImageView::Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator)
	{
		info = _info;

		return Parent::Create(_device, _info, _allocator);
	}

#pragma endregion ImageView

#pragma region BufferPackage

	BufferPackage::BufferPackage() : buffer(), memory(), memoryOffset(0), view()
	{}

	BufferPackage::BufferPackage(const LogicalDevice& _device) : buffer(_device), memory(nullptr), memoryOffset(0), view(_device)
	{}

	BufferPackage::BufferPackage(const LogicalDevice& _device, const Memory::AllocationCallbacks& _memory) : 
		buffer(_device), memory(nullptr), memoryOffset(0), view(_device)
	{}

	BufferPackage::~BufferPackage()
	{
		// Individual objects handle their destruction.
	}

	void BufferPackage::Create(const Buffer::CreateInfo& _bufferInfo, Memory::PropertyFlags _memoryFlags)
	{
		EResult returnCode = buffer.Create(_bufferInfo);

		Memory::AllocateInfo allocInfo;

		auto& memReq = buffer.GetMemoryRequirements();

		allocInfo.AllocationSize = memReq.Size;

		allocInfo.MemoryTypeIndex = 
			buffer.GetDevice().GetPhysicalDevice().FindMemoryType(memReq.MemoryTypeBits, _memoryFlags);

		memory = &RequestMemory(allocInfo);

		buffer.BindMemory(*memory, memoryOffset);
	}

	void BufferPackage::Destroy()
	{
		buffer.Destroy();
		view  .Destroy();

		memory = nullptr;

		memoryOffset = 0;
	}

	const Buffer& BufferPackage::GetBuffer()
	{
		return buffer;
	}

	const Memory& BufferPackage::GetMemory()
	{
		return *memory;
	}

	const DeviceSize& BufferPackage::GetMemoryOffset()
	{
		return memoryOffset;
	}

	const BufferView& BufferPackage::GetView()
	{
		return view;
	}

#pragma endregion BufferPackage

#pragma region ImagePackage

	EFormat ImagePackage::GetFormat()
	{
		return image.GetFormat();
	}

#pragma endregion ImagePackage

#pragma region VertexBuffer

	EResult VertexBuffer::Create(ptr<void> data, uint32 _dataSize, uint32 _stride, VertexInputState& _stateInfo)
	{
		EResult result = EResult::Incomplete;

		Buffer stagingBuffer;

		Memory stagingBufferMemory;

		Buffer::CreateInfo stagingBufferInfo;

		stagingBufferInfo.SharingMode = ESharingMode::Exclusive;
		stagingBufferInfo.Size        = _dataSize;

		stagingBufferInfo.Usage.Set(EBufferUsage::TransferSource);

		Heap()
		/*result = stagingBuffer.CreateAndBind
		(
			GetEngagedDevice(),
			stagingBufferInfo,
			Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent), 
			stagingBufferMemory
		);*/

		if (result != EResult::Success) return result;

		// Hard coded vertex data...

		Memory::MapFlags mapflags;

		stagingBufferMemory.WriteToGPU(0, _dataSize, mapflags, data);

		Buffer::CreateInfo vertexBufferInfo {};

		vertexBufferInfo.Size        = _dataSize;
		vertexBufferInfo.SharingMode = ESharingMode::Exclusive;

		vertexBufferInfo.Usage.Set(EBufferUsage::TransferDestination, EBufferUsage::VertexBuffer);

		Heap()
		/*result = buffer.CreateAndBind
		(
			GetEngagedDevice(),
			vertexBufferInfo,
			Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal),
			memory
		);*/

		if (result != EResult::Success) return result;

		Buffer::CopyInfo copyInfo {}; copyInfo.DestinationOffset = 0; copyInfo.SourceOffset = 0; copyInfo.Size = _dataSize;

		auto commandBuffer = RecordOnTransient();

		commandBuffer.CopyBuffer(stagingBuffer, buffer, 1, &copyInfo);

		EndRecordOnTransient(commandBuffer);

		Heap()
		stagingBuffer.Destroy();
		stagingBufferMemory.Free();

		return result;
	}

	void VertexBuffer::Destroy()
	{
		buffer.Destroy();

		//memory.Free();
	}

#pragma endregion VertexBuffer

	
}