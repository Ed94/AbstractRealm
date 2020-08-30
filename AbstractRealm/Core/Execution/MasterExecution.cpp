// Parent Header
#include "MasterExecution.hpp"


#include "Cycler.hpp"
#include "Concurrency/CyclerPool.hpp"

// When you have a proper UI module setup, setup it to bind to imgui instead.
#include "ImGui_SAL.hpp"



namespace Core::Execution
{
	using namespace Concurrency;

	PrimitiveExecuter<void()> MasterExecuter;

	Cycler MasterCycler;


	void MainCycle();


	void Initialize_MasterCycler()
	{
		MasterExecuter.Bind(MainCycle);

		MasterCycler.BindExecuter(MasterExecuter);

		//MasterCycler.AssignInterval(Duration64(1.0 / 1036.0));

		std::chrono::seconds sec(1);

		//Dev::CLog(String("Interval: ") + ToString(std::chrono::duration<float64, std::ratio<1>>(1.0 / 1036.0).count()));

		MasterCycler.Initiate();
	}

	void MainCycle()
	{
		using namespace SAL;

		OSAL::PollEvents();

		Imgui::MakeFrame();

		Imgui::Render();

		HAL::GPU::Default_DrawFrame(EngineWindow);

		HAL::GPU::Vulkan::Update();

		Imgui::Dirty_DoSurfaceStuff(EngineWindow);	

		Dev::CLog_Status("Master    Delta: " + ToString(MasterCycler.GetDeltaTime().count()), 0, 0);

		if (Concurrency::CyclerPool::GetNumUnits() > 0)
		{
			for (uInt16 row = 1, col = 0, cycleIndex = 0; cycleIndex < CyclerPool::GetNumUnits(); cycleIndex++)
			{
				Dev::CLog_Status("Thread 1  Delta: " + ToString(CyclerPool::GetCycler(cycleIndex).GetDeltaTime().count()), row++, col);

				if (row == 4)
				{
					row = 0; col++;
				}

				if (col == 4) break;
			}
		}

		static Duration64 consoleUpdateDelta(0), consoleUpdateInterval(1.0 / 24.0);
		
		if (consoleUpdateDelta >= consoleUpdateInterval)
		{
			//Dev::Console_UpdateInput();

			Dev::Console_UpdateBuffer();

			Dev::CLog_Status("Console   Delta: " + ToString(consoleUpdateDelta.count()), 1, 0);

			consoleUpdateDelta = Duration64(0);
		}
		else
		{
			consoleUpdateDelta += MasterCycler.GetDeltaTime();
		}

		if (OSAL::CanClose(EngineWindow))
		{
			HAL::GPU::WaitFor_GPUIdle();

			if (Meta::UseConcurrency)
			{
				Concurrency::CyclerPool::RequestShutdown();
			}
			else
			{
				MasterCycler.Lapse();
			}
			
		}

		if (Meta::UseConcurrency)
		{
			// Query module states to see if its safe for the master cycler to shutdown.
			if (Concurrency::CyclerPool::IsShutdown())
			{
				MasterCycler.Lapse();
			}
		}
	}
}
