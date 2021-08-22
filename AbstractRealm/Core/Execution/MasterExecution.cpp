// Parent Header
#include "MasterExecution.hpp"

#define Meta_EngineModule
#include "Meta/Meta.hpp"

#include "HAL.hpp"
#include "HAL/HAL_GPU.hpp"
#include "HAL/Vulkan/GPU_Vulkan.hpp"
#include "OSAL.hpp"

#include "Concurrency/CyclerPool.hpp"
#include "Cycler.hpp"
#include "Dev/Console.hpp"

#include "Renderer/Renderer.hpp"

// When you have a proper UI module setup, setup it to bind to imgui instead.
#include "SAL_ImGui.hpp"


namespace Execution
{
	using namespace Concurrency;
	//using namespace Meta;

	PrimitiveExecuter<void()> MasterExecuter;

	Cycler MasterCycler;


	void MainCycle();

	const Cycler& Get_MasterCycler()
	{
		return MasterCycler;
	}


	void Initialize_MasterCycler()
	{
		MasterExecuter.Bind(MainCycle);

		MasterCycler.BindExecuter(MasterExecuter);

		//MasterCycler.AssignInterval(Duration64(1.0 / 1036.0));

		//std::chrono::seconds sec(1);

		Dev::CLog(String("Interval: ") + ToString(std::chrono::duration<f64, std::ratio<1>>(1.0 / 144.0).count()));

		MasterCycler.Initiate();

		Dev::Console_DisableAutoUpdate();
	}

	void MainCycle()
	{
		using namespace SAL;

		OSAL::PollEvents();

		static Duration64 consoleUpdateDelta(0), consoleUpdateInterval(1.0 / 30.0);
		static Duration64 renderPresentDelta(0);

		if (consoleUpdateDelta >= consoleUpdateInterval)
		{

			Dev::CLog_Status("Master    Delta: " + ToString(MasterCycler.GetDeltaTime().count()), 0, 0);
			Dev::CLog_Status("Render    Delta: " + ToString(renderPresentDelta.count()), 1, 0);

			if (Concurrency::CyclerPool::GetNumUnits() > 0)
			{
				for (u16 row = 1, col = 0, cycleIndex = 0; cycleIndex < CyclerPool::GetNumUnits(); cycleIndex++)
				{
					Dev::CLog_Status("Thread 1  Delta: " + ToString(CyclerPool::GetCycler(cycleIndex).GetDeltaTime().count()), row++, col);

					if (row == 4)
					{
						row = 0; col++;
					}

					if (col == 4) break;
				}
			}

			Dev::Console_UpdateBuffer();

			Dev::CLog_Status("Console   Delta: " + ToString(consoleUpdateDelta.count()), 2, 0);

			consoleUpdateDelta = Duration64(0);
		}
		else
		{
			consoleUpdateDelta += MasterCycler.GetDeltaTime();
		}

		if (renderPresentDelta >= Renderer::Get_PresentInterval())
		{
			switch (GPU_API())
			{
				case Meta::EGPUPlatformAPI:: Methane :
				{
					

				} break;
				
				case Meta::EGPUPlatformAPI:: Vulkan :
				{
								Imgui ::MakeFrame();
								Imgui ::Render();
					HAL::GPU::	Vulkan::Render();
					HAL::GPU::	Vulkan::Present();
								Imgui ::Dirty_DoSurfaceStuff(Renderer::EngineWindow());

				} break;

				case Meta::EGPUPlatformAPI::No_API :
				{


				} break;
			}

			renderPresentDelta = Duration64(0);
		}
		else
		{
			renderPresentDelta += MasterCycler.GetDeltaTime();
		}

		if (OSAL::CanClose(Renderer::EngineWindow()))
		{
			HAL::GPU::WaitForIdle();

			if (Meta::UseConcurrency())
			{
				Concurrency::CyclerPool::RequestShutdown();
			}
			else
			{
				MasterCycler.Lapse();
			}
		}

		if (Meta::UseConcurrency())
		{
			// Query module states to see if its safe for the master cycler to shutdown.
			if (Concurrency::CyclerPool::IsShutdown())
			{
				MasterCycler.Lapse();
			}
		}
	}
}
