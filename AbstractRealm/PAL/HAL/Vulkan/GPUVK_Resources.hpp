#pragma once


#include "GPUVK_Memory.hpp"
//#include "GPUVK_Renderer.hpp"



namespace HAL::GPU::Vulkan
{
	// Classes

	class Image : public V3::Image
	{
	public:

		using Parent = V3::Image;

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

		EResult CreateAndBind
		(
			const LogicalDevice&        _device     ,  
			const CreateInfo&           _info       ,  
				  Memory::PropertyFlags _memoryFlags,
				  Memory&               _memory
		);

		EResult CreateAndBind
		(
			const LogicalDevice&               _device        ,  
			const CreateInfo&                  _info          , 
				  Memory::PropertyFlags        _memoryFlags   ,
				  Memory&                      _memory        ,
			const Memory::AllocationCallbacks* _allocator
		);

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


	class ImagePackage
	{
	public:
		EFormat GetFormat();

		Image     image;
		Memory    memory;
		ImageView view;
	};

	struct VertexInputState : Pipeline::VertexInputState
	{
		DynamicArray<AttributeDescription> Attributes;
		BindingDescription                 Bind      ;
	};

	class VertexBuffer
	{
	public:

		EResult Create(ptr<void> data, uint32 _dataSize, uint32 _stride, VertexInputState& _stateInfo);

		void Destroy();

	protected:

		Buffer buffer;
		Memory memory;

		VertexInputState state;
	};
}
