// Parent Header
#include "GPUVK_PayloadDeck.hpp"



namespace HAL::GPU::Vulkan
{
	/*const CommandBuffer& CommandPool::RequestBuffer()
	{


	}*/

	const CommandBuffer& CommandPool::RecordSingleTime()
	{
		Heap(commandBuffers.push_back(BeginSingleTimeCommands()));

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
		DynamicArray<CommandPool> CommandPools;

		ptr<CommandPool> GraphicsPool ;
		ptr<CommandPool> TransientPool;
	)


	void PrepareDecks()
	{
		CommandPool::CreateInfo info {};

		info.QueueFamilyIndex = GetGraphicsQueue().GetFamilyIndex();   // Hardcoding pool to graphics queue

		
		CommandPool generalPool;

		Heap(generalPool.Create(GetEngagedDevice().GetHandle(), info));

		CommandPools.push_back(generalPool);

		GraphicsPool = &CommandPools.back();


		CommandPool transientPool;

		info.Flags.Set(ECommandPoolCreateFlag::Transient);

		Heap(transientPool.Create(GetEngagedDevice().GetHandle(), info));

		CommandPools.push_back(transientPool);

		TransientPool = &CommandPools.back();
	}

	const CommandBuffer& RecordOnTransient()
	{
		return TransientPool->RecordSingleTime(); 
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