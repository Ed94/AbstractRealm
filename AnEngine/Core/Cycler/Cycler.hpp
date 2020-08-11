#pragma once



#include "LAL/LAL.hpp"

#include "Execution/Executer.hpp"
#include <atomic>



namespace Core::Execution
{
	enum class CyclerType
	{
		Core       ,
		Debugger   ,
		Editor     ,
		IO         ,  
		Profile    ,
		Render     ,
		Simulation ,
		UI         ,
		UserManager,
		Custom
	};

	class ACycler
	{
	public:
		~ACycler() {};

		virtual void        Initiate          () = NULL;
		virtual EReturnCode Initiate_withRCode() = NULL;
	};

	class Cycler : public ACycler
	{
	public:

		Cycler();

		implem void        Initiate          ();
		implem EReturnCode Initiate_withRCode();

		void BindExecuter(ptr<AExecuter> _executerToBind);

		void Lapse();

		void Toggle();

		bool Lapsed() { return lapsed; };

		Duration64 GetDeltaTime() { return deltaTime; }

		Duration64 GetAverageDelta() { return averageDelta; }

		float64 GetCycle() { return cycles; }

		operator ptr<ACycler>();

		//operator ptr<ACycler>();

	protected:

		void CalculateStats();

		ptr<AExecuter> executer;

		float64 cycles;

		Duration64 deltaTime, averageDelta;

		SteadyTimePoint cycleStart, cycleEnd;

		bool exist, lapsed, pause;
	};
}
