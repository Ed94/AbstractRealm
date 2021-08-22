// Parent Header
#include "GPUVK_Memory.hpp"




namespace HAL::GPU::Vulkan
{
#pragma region StaticData

		DynamicArray<Memory> Memories;

#pragma endregion StaticData



	// Forwards

	const Memory& RequestMemory(const Memory::AllocateInfo& _info);


#pragma region Memory

	EResult Memory::Allocate(const AllocateInfo& _info)
	{
		info = _info;

		return Parent::Allocate(_info);
	}

	EResult Memory::Allocate(const LogicalDevice& _device, const AllocateInfo& _info)
	{
		device = &_device;
		info = _info;

		return Parent::Allocate(_info);
	}

	EResult Memory::Allocate(const Requirements& _requirements, PropertyFlags _propertyFlags)
	{
		info.AllocationSize  = _requirements.Size;

		info.MemoryTypeIndex = GPU_Comms::GetEngagedPhysicalGPU().FindMemoryType
		(
			_requirements.MemoryTypeBits,
			_propertyFlags
		);

		return Parent::Allocate(GPU_Comms::GetEngagedDevice(), info);
	}

	void Memory::Free()
	{
		return Parent::Free();
	}

#pragma endregion Memory

	/**
	Right now a new memory object is provided per allocation request.
	*/
	const Memory& RequestMemory(const Memory::AllocateInfo& _info)
	{
		Memories.resize(Memories.size() + 1);

		auto& memory = Memories.back();

		memory.Allocate(_info);

		return memory;
	}

	void WipeMemory()
	{
		for (auto& memory : Memories)
		{
			memory.Free();
		}
	}
}