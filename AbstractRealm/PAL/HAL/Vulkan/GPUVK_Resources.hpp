#pragma once


#include "GPUVK_Memory.hpp"
//#include "GPUVK_Renderer.hpp"



namespace HAL::GPU::Vulkan
{
	// Classes

	class Buffer : public V3::Buffer
	{
	public:
		using Parent = V3::Buffer;

		Buffer() : Parent::Buffer()
		{}

		Buffer(const LogicalDevice& _device) : Parent::Buffer(_device)
		{}

		Buffer(const LogicalDevice& _device, const Memory::AllocationCallbacks& _allocator) :
			Parent::Buffer(_device, _allocator)
		{}

	protected:

		CreateInfo info;
	};

	class Image : public V3::Image
	{
	public:

		using Parent = V3::Image;

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

		EFormat GetFormat() const ;

		bool HasStencilComponent() const;

		void TransitionLayout(EImageLayout _old, EImageLayout _new);

		operator Parent&();

	protected:

		CreateInfo info;
	};

	class ImageView : public V3::ImageView
	{
	public:

		using Parent = V3::ImageView;

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

	protected:

		CreateInfo info;
	};

	class BufferPackage
	{
	public:

		BufferPackage();

		BufferPackage(const LogicalDevice& _device);

		BufferPackage(const LogicalDevice& _device, const Memory::AllocationCallbacks& _allocator);

		~BufferPackage();

		void Create(const Buffer::CreateInfo& _bufferInfo, Memory::PropertyFlags _memoryFlags);

		void Destroy();

		const Buffer&     GetBuffer();
		const Memory&     GetMemory();
		const DeviceSize& GetMemoryOffset();
		const BufferView& GetView();

		void WriteToGPU();

	protected:

		      Buffer     buffer      ;
		const Memory*    memory      ;
		      DeviceSize memoryOffset;
		      BufferView view        ;
	};

	class ImagePackage
	{
	public:
		EFormat GetFormat();

		Image      image ;
		Memory     memory;
		//Memory*    memory;
		DeviceSize memoryOffset;
		ImageView  view  ;
	};

	struct VertexInputState : Pipeline::VertexInputState
	{
		DynamicArray<AttributeDescription> Attributes;
		BindingDescription                 Bind      ;
	};

	class VertexBuffer
	{
	public:

		EResult Create(ptr<void> data, ui32 _dataSize, ui32 _stride, VertexInputState& _stateInfo);

		void Destroy();

	protected:

		Buffer     buffer      ;
		Memory*    memory      ;
		DeviceSize memoryOffset;

		VertexInputState state;
	};

	class IndexBuffer
	{
	public:

		EResult Create();

		void Destroy();

	protected:

		Buffer     buffer      ;
		Memory*    memory      ;
		DeviceSize memoryOffset;

		DynamicArray<ui32> indices;
	};
}
