#pragma once



#include "GPUVK_Comms.hpp"



namespace HAL::GPU::Vulkan
{
	class CommandPool : public V4::CommandPool
	{
	public:
		using Parent = V4::CommandPool;

		const CommandBuffer& RequestBuffer();

		const CommandBuffer& RecordSingleTime();

		void EndSingleTimeRecord(const CommandBuffer& _buffer);




	private:
		DynamicArray<CommandBuffer> commandBuffers;
	};



	StaticData
	(
		eGlobal Deque<CommandPool> CommandPools;


		// Singled threaded stuff...

		eGlobal ptr<CommandPool> GeneralPool;

		eGlobal ptr<CommandPool> TransientPool;
	)



	void WipeDeck();

	void PrepareDecks();	

	const ptr<CommandPool> RequestCommandPools(DataSize _numDesired);

	const CommandBuffer& RecordOnTransient();

	void EndRecordOnTransient(const CommandBuffer& _buffer);
}
