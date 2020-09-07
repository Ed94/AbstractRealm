#pragma once



#include "GPUVK_Comms.hpp"



namespace HAL::GPU::Vulkan
{
	class CommandPool : public V3::CommandPool
	{
	public:
		using Parent = V3::CommandPool;

		const CommandBuffer& RequestBuffer();

		const CommandBuffer& RecordSingleTime();

		void EndSingleTimeRecord(const CommandBuffer& _buffer);




	private:
		DynamicArray<CommandBuffer> commandBuffers;
	};



	void WipeDecks();

	void PrepareDecks();	

	const ptr<CommandPool> RequestCommandPools(WordSize _numDesired);

	const CommandBuffer& RecordOnTransient();

	void EndRecordOnTransient(const CommandBuffer& _buffer);
}
