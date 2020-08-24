#pragma once


#include "GPUVK_Memory.hpp"



namespace HAL::GPU::Vulkan
{
	// Classes

	class Image : public V4::Image
	{
	public:

		using Parent = V4::Image;

		bool HasStencilComponent();

		EResult TransitionLayout(EImageLayout _old, EImageLayout _new);

		operator Parent&();

	protected:
	};


	class ImagePackage
	{
	public:
		Image     image;
		Memory    memory;
		ImageView view;
	};
}
