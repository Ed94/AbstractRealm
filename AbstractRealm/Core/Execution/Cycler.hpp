#pragma once


// Engine
#include "Core_Backend.hpp"
#include "Executer.hpp"


namespace Execution
{
	/**
	 * .
	 */
	/*enum class CyclerType
	{
		Master ,
		Generic
	};*/


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
		~Cycler();

		void        Initiate          () override;
		EReturnCode Initiate_withRCode() override;

		void AssignInterval(Duration64 _interval);

		void BindExecuter(ptr<AExecuter> _executerToBind);

		ForceInline
		Duration64 GetAverageDelta() const 
		{
			return averageDelta; 
		}

		ForceInline
		f64 GetCycle() const
		{
			return cycles;
		}

		ForceInline
		Duration64 GetDeltaTime() const
		{
			return deltaTime; 
		}

		ForceInline
		void Lapse ()       
		{
			exist = false; 
		}

		ForceInline
		bool Lapsed() const 
		{ 
			return lapsed; 
		}

		ForceInline
		void Toggle()
		{ 
			pause ^= true; 
		}

		operator ptr<ACycler>() 
		{ 
			return RCast<ACycler>(this); 
		}

	protected:

		void CalculateStats();

		bool CanExecute();

		ptr<AExecuter> executer;

		f64 cycles;

		Duration64 deltaTime, averageDelta, interval, deltaInterval;

		SteadyTimePoint cycleStart, cycleEnd;

		bool exist, lapsed, pause;
	};
}
