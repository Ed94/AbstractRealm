#pragma once


#include "GPUVK_Comms.hpp"


namespace HAL::GPU::Vulkan
{
	class CommandBuffer : public V3::CommandBuffer
	{
	public:
		using Parent = V3::CommandBuffer;

		CommandBuffer() : Parent::CommandBuffer() {};

		CommandBuffer(const LogicalDevice& _device, Handle& _handle_in) : Parent::CommandBuffer(_device, _handle_in) {}

		void Assign(const LogicalDevice& _device, AllocateInfo& _info_in, Handle& _handle_in);
		 
		void Clear();

		ForceInline
		const AllocateInfo& GetAllocateInfo() const 
		{
			return info; 
		}

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

		static void Prepare();

		static void Wipe();


		static ptr<CommandPool> RequestCommandPools(uDM _numDesired);

		static const CommandBuffer& RecordOnGraphics();

		static const CommandBuffer& RecordOnTransient();

		static void EndRecordOnTransient(const CommandBuffer& _buffer);
	};

	using Deck = Deck_Maker<Meta::GPU_Engagement>;
}
