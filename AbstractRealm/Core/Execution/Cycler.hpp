#pragma once



// Engine
#include "LAL/LAL.hpp"
#include "Execution/Executer.hpp"



namespace Core::Execution
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

		implem void        Initiate          ();
		implem EReturnCode Initiate_withRCode();

		void AssignInterval(Duration64 _interval);

		void BindExecuter(ptr<AExecuter> _executerToBind);

		Duration64 GetAverageDelta() const;   // { return averageDelta; }
		f64        GetCycle       () const;   // { return cycles      ; }
		Duration64 GetDeltaTime   () const;   // { return deltaTime   ; }

		void Lapse ()       { exist = false; }
		bool Lapsed() const { return lapsed; }
		void Toggle()       { pause ^= true; }

		operator ptr<ACycler>() { return RCast<ACycler>(this); }

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
