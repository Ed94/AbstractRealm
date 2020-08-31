#pragma once



#include "GPUVK_Comms.hpp"




namespace HAL::GPU::Vulkan
{
	class Memory : public V3::Memory
	{
	public:

		using Parent = V3::Memory;

		EResult Allocate(const Requirements& _requirements, PropertyFlags _propertyFlags);
	};
}
