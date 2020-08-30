// Parent Header
#include "GPUVK_PayloadDeck.hpp"



namespace HAL::GPU::Vulkan
{
	const CommandBuffer& CommandPool::RequestBuffer()
	{
		AllocateInfo info;

		info.BufferCount = 1;
		info.Pool = handle;
		info.Level = ECommandBufferLevel::Primary;

		CommandBuffer commandBuffer;

		CommandBuffer::Handle handle;
		
		Allocate(info, &handle);

		commandBuffer.Assign(GetEngagedDevice(), info, handle);

		commandBuffers.push_back(commandBuffer);

		return commandBuffers.back();
	}

	const CommandBuffer& CommandPool::RecordSingleTime()
	{
		EResult result;

		Heap(commandBuffers.push_back(BeginSingleTimeCommands(result)));

		return commandBuffers.back();
	}

	void CommandPool::EndSingleTimeRecord(const CommandBuffer& _buffer)
	{
		auto result = std::find(commandBuffers.begin(), commandBuffers.end(), _buffer);

		if (result != commandBuffers.end())
		{
			auto pos = std::distance(commandBuffers.begin(), result); 

			Heap(EndSingleTimeCommands(commandBuffers.at(pos), GetGraphicsQueue()));

			commandBuffers.erase(result);
		}
		else
		{
			throw RuntimeError("Cannot find command buffer in tracked pool.");
		}
	}



	StaticData
	(
		Deque<CommandPool> CommandPools;

		ptr<CommandPool> GeneralPool ;
		ptr<CommandPool> TransientPool;
	)



	void PrepareDecks()
	{
		CommandPool::CreateInfo info {};

		info.QueueFamilyIndex = GetGraphicsQueue().GetFamilyIndex();   // Hardcoding pool to graphics queue


		// Only one set of pools for now since its single threaded

		CommandPool generalPool;
		
		Heap(generalPool.Create(GetEngagedDevice(), info));

		CommandPools.push_back(generalPool);

		GeneralPool = &CommandPools.back();


		CommandPool transientPool;

		info.Flags.Set(ECommandPoolCreateFlag::Transient);

		Heap(transientPool.Create(GetEngagedDevice(), info));

		CommandPools.push_back(transientPool);

		TransientPool = &CommandPools.back();
	}

	const CommandBuffer& RecordOnGraphics()
	{
		return GeneralPool->RequestBuffer();
	}

	const CommandBuffer& RecordOnTransient()
	{
		return TransientPool->RecordSingleTime(); 
	}

	const ptr<CommandPool> RequestCommandPools(DataSize _numDesired)
	{
		DataSize firstOfNewPools = CommandPools.size();

		CommandPools.resize(firstOfNewPools + _numDesired);

		CommandPool::CreateInfo info;

		info.QueueFamilyIndex =  GetGraphicsQueue().GetFamilyIndex();

		EResult result = CommandPools.back().Create(GetEngagedDevice(), info);

		if (result != EResult::Success)
		{
			throw RuntimeError("Failed to create requested command pool.");
		}

		return &CommandPools[firstOfNewPools];
	}

	void EndRecordOnTransient(const CommandBuffer& _buffer)
	{
		TransientPool->EndSingleTimeRecord(_buffer);
	}

	void WipeDeck()
	{
		for (auto& pool : CommandPools)
		{
			pool.Destroy();
		}
	}
}