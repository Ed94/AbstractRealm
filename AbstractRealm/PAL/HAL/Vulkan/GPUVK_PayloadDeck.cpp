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

		CommandBuffer::Handle newBuffer;

		
		
		Allocate(info, &newBuffer);

		// Allocations of buffers here are not tracked. They are freed automatically by the command pool on its destruction.

		commandBuffers.push_back(CommandBuffer(GPU_Comms::GetEngagedDevice(), newBuffer));

		return commandBuffers.back();
	}

	const CommandBuffer& CommandPool::BeginSingleTimeCommands()
	{
		EResult result;

		commandBuffers.resize(commandBuffers.size() + 1);

		CommandBuffer& buffer = commandBuffers.back();

		result = Allocate(buffer);

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

			vResult = GPU_Comms::GetGraphicsQueue().SubmitToQueue(1, submitInfo, Null<Fence::Handle>);

			if (vResult != EResult::Success) throw RuntimeError("Failed to submit to queue.");

			vResult = GPU_Comms::GetGraphicsQueue().WaitUntilIdle();

			if (vResult != EResult::Success) throw RuntimeError("Failed to wait for queue to idle.");

			Free(buffer);

			commandBuffers.erase(result);
		}
		else
		{
			throw RuntimeError("Cannot find command buffer in tracked pool.");
		}
	}



	StaticData()

		Deque<CommandPool> CommandPools;

		ptr<CommandPool> GeneralPool ;
		ptr<CommandPool> TransientPool;



	void Deck_Maker<Meta::EGPU_Engage::Single>::Prepare()
	{
		CommandPool::CreateInfo info {};

		info.QueueFamilyIndex = GPU_Comms::GetGraphicsQueue().GetFamilyIndex();   // Hard coding pool to graphics queue

		// Only one set of pools for now since its single threaded

		CommandPools.resize(CommandPools.size() + 1);

		GeneralPool = &CommandPools.back();
		
		GeneralPool->Create(GPU_Comms::GetEngagedDevice(), info); 

		CommandPools.resize(CommandPools.size() + 1);

		TransientPool = &CommandPools.back();

		info.Flags.Set(ECommandPoolCreateFlag::Transient);

		TransientPool->Create(GPU_Comms::GetEngagedDevice(), info);
	}

	const CommandBuffer& Deck_Maker<Meta::EGPU_Engage::Single>::RecordOnGraphics()
	{
		return GeneralPool->RequestBuffer();
	}

	const CommandBuffer& Deck_Maker<Meta::EGPU_Engage::Single>::RecordOnTransient()
	{
		return TransientPool->BeginSingleTimeCommands(); 
	}

	ptr<CommandPool> Deck_Maker<Meta::EGPU_Engage::Single>::RequestCommandPools(uDM _numDesired)
	{
		uDM firstOfNewPools = CommandPools.size();

		CommandPools.resize(firstOfNewPools + _numDesired);

		CommandPool::CreateInfo info;

		info.QueueFamilyIndex = GPU_Comms::GetGraphicsQueue().GetFamilyIndex();

		EResult result = CommandPools.back().Create(GPU_Comms::GetEngagedDevice(), info);

		if (result != EResult::Success)
		{
			throw RuntimeError("Failed to create requested command pool.");
		}

		return &CommandPools[firstOfNewPools];
	}

	void Deck_Maker<Meta::EGPU_Engage::Single>::EndRecordOnTransient(const CommandBuffer& _buffer)
	{
		TransientPool->EndSingleTimeCommands(_buffer);
	}

	void Deck_Maker<Meta::EGPU_Engage::Single>::Wipe()
	{
		for (auto& pool : CommandPools)
		{
			pool.Destroy();
		}
	}
}