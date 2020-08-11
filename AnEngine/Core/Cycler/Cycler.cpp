// Parent Header
#include "Cycler.hpp"





namespace Core::Execution
{
	//ACycler::~ACycler() {}

	Cycler::Cycler() : 
		executer(nullptr), 
		deltaTime(), 
		averageDelta(1),
		cycleStart(), 
		cycleEnd(), 
		exist(false), 
		lapsed(false),
		pause(false)
	{
		cycles = 1;
	}

	void Cycler::BindExecuter(ptr<AExecuter> _executerToBind)
	{
		executer = _executerToBind;
	}

	void Cycler::Initiate()
	{
		exist = true;

		while (exist)
		{
			if (pause) continue;

			cycleStart = SteadyClock::now();

			executer->Execute();

			cycleEnd = SteadyClock::now();

			CalculateStats();
		}

		lapsed = true;
	}

	EReturnCode Cycler::Initiate_withRCode()
	{
		return EReturnCode::Completed;
	}



	void Cycler::Lapse() { exist = false; }

	void Cycler::Toggle() { pause ^= true; }

	Cycler::operator ptr<ACycler>()
	{
		return RCast<ACycler>(this);
	}

	// Protected

	void Cycler::CalculateStats()
	{
		deltaTime = std::chrono::duration_cast<Duration64>(cycleEnd - cycleStart);

		float64 alpha = 0.5;

		averageDelta = 	
			// Exponential Moving Average method
			// https://android.developreference.com/article/24557849/Calculate+rolling+++moving+average+in+C%2B%2B
			(averageDelta * alpha) + (deltaTime * (1.0L - alpha));

		//auto temp = cycles.load(std::memory_order_relaxed) + 1;

		cycles++;
	}

	/*Cycler::operator ptr<ACycler>()
	{
		
	}*/
}