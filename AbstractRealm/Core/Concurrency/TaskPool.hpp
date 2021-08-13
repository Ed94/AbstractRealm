#pragma once


namespace Concurrency
{
	class ATaskPool
	{
	public:
		~ATaskPool() {};

	protected:
	private:
	};

	class EngineTaskPool : ATaskPool
	{

	};

	class EditorTaskPool : ATaskPool
	{

	};
}
