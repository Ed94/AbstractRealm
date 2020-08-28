// Parent Header
#include "Cycler.hpp"



namespace Core::Execution
{
	// Public

	Cycler::Cycler() : 
		executer     (nullptr), 
		cycles       (1),
		deltaTime    (), 
		averageDelta (1),
		interval     (Duration64::zero()),
		deltaInterval(Duration64::zero()),
		cycleStart   (), 
		cycleEnd     (), 
		exist        (false), 
		lapsed       (false),
		pause        (false)
	{}

	Cycler::~Cycler() {};

	void Cycler::AssignInterval(Duration64 _interval)
	{
		interval = _interval;
	}

	void Cycler::BindExecuter(ptr<AExecuter> _executerToBind)
	{
		executer = _executerToBind;
	}

	Duration64 Cycler::GetAverageDelta() const 
	{ 
		return averageDelta; 
	}

	float64 Cycler::GetCycle() const 
	{ 
		return cycles; 
	}
	
	Duration64 Cycler::GetDeltaTime() const
	{ 
		return deltaTime; 
	}

	void Cycler::Initiate()
	{
		exist = true;

		while (exist)
		{
			cycleStart = SteadyClock::now();

			if (CanExecute()) 
			{
				executer->Execute();

				cycleEnd = SteadyClock::now();
				
				CalculateStats();

				deltaInterval = Duration64::zero();
			}
			else
			{
				cycleEnd = SteadyClock::now();

				CalculateStats();

				deltaInterval += deltaTime;

				//std::this_thread::yield(); 
			}
		}

		lapsed = true;
	}

	EReturnCode Cycler::Initiate_withRCode()
	{
		return EReturnCode::Completed;
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

		cycles++;
	}

	bool Cycler::CanExecute()
	{
		return
			// Fail cases
			executer == nullptr       ? false :
			pause                     ? false :
			deltaInterval < interval  ? false :

			// Otherwise:
			true; 
	}
}