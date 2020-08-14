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

		MasterCycler.AssignInterval(Duration64(1.0 / 144.0));

		MasterCycler.Initiate();
	}

	void MainCycle()
	{
		using namespace SAL;

		OSAL::PollEvents();

		Imgui::MakeFrame();

		Imgui::Render();

		HAL::GPU::Default_DrawFrame(EngineWindow);

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
		
		//Dev::Console_UpdateBuffer();

		if (OSAL::CanClose(EngineWindow))
		{
			HAL::GPU::WaitFor_GPUIdle();

			Concurrency::CyclerPool::RequestShutdown();
		}

		// Query module states to see if its safe for the master cycler to shutdown.
		if (Concurrency::CyclerPool::IsShutdown())
		{
			MasterCycler.Lapse();
		}
	}
}
