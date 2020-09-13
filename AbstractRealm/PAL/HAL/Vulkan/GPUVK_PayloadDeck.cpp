// Parent Header
#include "GPUVK_PayloadDeck.hpp"



namespace HAL::GPU::Vulkan
{
	const CommandBuffer& CommandPool::RequestBuffer()
	{
		AllocateInfo info;

		info.BufferCount = 1;
		info.Pool        = handle;
		info.Level       = ECommandBufferLevel::Primary;

		CommandBuffer::Handle handle;
		
		Allocate(info, &handle);

		commandBuffers.push_back(CommandBuffer(GetEngagedDevice(), handle));

		return commandBuffers.back();
	}

	const CommandBuffer& CommandPool::BeginSingleTimeCommands()
	{
		EResult result;

		commandBuffers.resize(commandBuffers.size() + 1);

		CommandBuffer& buffer = commandBuffers.back();

		result = Heap(Allocate(buffer));

		if (result != EResult::Success) throw RuntimeError("Failed to allocate...");

		CommandBuffer::BeginInfo beginInfo;

		beginInfo.Flags = ECommandBufferUsageFlag::OneTimeSubmit;

		result = buffer.BeginRecord(beginInfo);

		if (result != EResult::Success) throw RuntimeError("Failed to begin recording...");

		return commandBuffers.back();
	}

	void CommandPool::EndSingleTimeCommands(const CommandBuffer& _buffer)
	{
		auto result = std::find(commandBuffers.begin(), commandBuffers.end(), _buffer);

		if (result != commandBuffers.end())
		{
			auto pos = std::distance(commandBuffers.begin(), result); 

			CommandBuffer& buffer = commandBuffers.at(pos);

			auto vResult = buffer.EndRecord();

			if (vResult != EResult::Success) throw RuntimeError("Failed to end buffer recording");

			CommandBuffer::SubmitInfo submitInfo;

			submitInfo.CommandBufferCount = 1     ;
			submitInfo.CommandBuffers     = buffer;

			vResult = GetGraphicsQueue().SubmitToQueue(1, submitInfo, Null<Fence::Handle>);

			if (vResult != EResult::Success) throw RuntimeError("Failed to submit to queue.");

			vResult = GetGraphicsQueue().WaitUntilIdle();

			if (vResult != EResult::Success) throw RuntimeError("Failed to wait for queue to idle.");

			Free(buffer);

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

		CommandPools.resize(CommandPools.size() + 1);

		GeneralPool = &CommandPools.back();
		
		Heap(GeneralPool->Create(GetEngagedDevice(), info));


		CommandPools.resize(CommandPools.size() + 1);

		TransientPool = &CommandPools.back();

		info.Flags.Set(ECommandPoolCreateFlag::Transient);

		Heap(TransientPool->Create(GetEngagedDevice(), info));
	}

	const CommandBuffer& RecordOnGraphics()
	{
		return GeneralPool->RequestBuffer();
	}

	const CommandBuffer& RecordOnTransient()
	{
		return TransientPool->BeginSingleTimeCommands(); 
	}

	const ptr<CommandPool> RequestCommandPools(WordSize _numDesired)
	{
		WordSize firstOfNewPools = CommandPools.size();

		CommandPools.resize(firstOfNewPools + _numDesired);

		CommandPool::CreateInfo info;

		info.QueueFamilyIndex = GetGraphicsQueue().GetFamilyIndex();

		EResult result = CommandPools.back().Create(GetEngagedDevice(), info);

		if (result != EResult::Success)
		{
			throw RuntimeError("Failed to create requested command pool.");
		}

		return &CommandPools[firstOfNewPools];
	}

	void EndRecordOnTransient(const CommandBuffer& _buffer)
	{
		TransientPool->EndSingleTimeCommands(_buffer);
	}

	void WipeDecks()
	{
		for (auto& pool : CommandPools)
		{
			pool.Destroy();
		}
	}
}