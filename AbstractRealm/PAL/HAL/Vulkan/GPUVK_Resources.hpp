#pragma once


#include "GPUVK_Memory.hpp"
//#include "GPUVK_Renderer.hpp"



namespace HAL::GPU::Vulkan
{
	// Classes

	class Image : public V4::Image
	{
	public:

		using Parent = V4::Image;

		EFormat GetFormat() const ;

		bool HasStencilComponent() const;

		EResult TransitionLayout(EImageLayout _old, EImageLayout _new);

	

		operator Parent&();

	protected:
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
