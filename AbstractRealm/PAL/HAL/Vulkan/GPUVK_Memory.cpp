// Parent Header
#include "GPUVK_Memory.hpp"




namespace HAL::GPU::Vulkan
{
	EResult Memory::Allocate(const Requirements& _requirements, PropertyFlags _propertyFlags)
	{
		AllocateInfo allocInfo {};

		allocInfo.AllocationSize  = _requirements.Size;

		allocInfo.MemoryTypeIndex = GetEngagedPhysicalGPU().FindMemoryType
		(
			_requirements.MemoryTypeBits,
			_propertyFlags
		);

		return Parent::Allocate(GetEngagedDevice().GetHandle(), allocInfo);
	}
}