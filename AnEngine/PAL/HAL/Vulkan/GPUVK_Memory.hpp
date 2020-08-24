#pragma once



#include "GPUVK_Comms.hpp"




namespace HAL::GPU::Vulkan
{
	class Memory : public V4::Memory
	{
	public:

		using Parent = V4::Memory;

		EResult Allocate(const Requirements& _requirements, PropertyFlags _propertyFlags);
	};
}
