#pragma once



#include "GPUVK_Comms.hpp"



namespace HAL::GPU::Vulkan
{
	class CommandBuffer : public V3::CommandBuffer
	{
	public:
		using Parent = V3::CommandBuffer;

		CommandBuffer() : Parent::CommandBuffer() {};

		CommandBuffer(const LogicalDevice& _device, Handle& _handle) : Parent::CommandBuffer(_device, _handle) {}

		void Assign(const LogicalDevice& _device, AllocateInfo& _info, Handle& _handle);
		 
		void Clear();

		const AllocateInfo& GetAllocateInfo() const { return info; }

		using Parent::BindVertexBuffers;

	protected:
		AllocateInfo info;
	};

	class CommandPool : public V3::CommandPool
	{
	public:
		using Parent = V3::CommandPool;

		const CommandBuffer& RequestBuffer();

		const CommandBuffer& BeginSingleTimeCommands();

		void EndSingleTimeCommands(const CommandBuffer& _buffer);


	protected:
		DynamicArray<CommandBuffer> commandBuffers;
	};


	template<Meta::EGPU_Engage>
	class Deck_Maker;

	template<>
	class Deck_Maker<Meta::EGPU_Engage::Single>
	{
	public:

		unbound void Prepare();

		unbound void Wipe();


		unbound ptr<CommandPool> RequestCommandPools(WordSize _numDesired);

		unbound const CommandBuffer& RecordOnGraphics();

		unbound const CommandBuffer& RecordOnTransient();

		unbound void EndRecordOnTransient(const CommandBuffer& _buffer);
	};

	using Deck = Deck_Maker<Meta::GPU_Engagement>;
}
