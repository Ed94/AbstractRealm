#pragma once



#include "GPUVK_Comms.hpp"




namespace HAL::GPU::Vulkan
{
	class Memory : public V3::Memory
	{
	public:

		using Parent = V3::Memory;

		Memory() : Parent::Memory()
		{}

		EResult Allocate(const AllocateInfo& _info);

		EResult Allocate(const LogicalDevice& _device, const AllocateInfo& _info);

		EResult Allocate(const Requirements& _requirements, PropertyFlags _propertyFlags);

		void Free();

	protected:

		// KeyValue Pair<void*, DeviceSize> size.

		AllocateInfo info;
	};

	const Memory& RequestMemory(const Memory::AllocateInfo& _info);

	void WipeMemory();
}
