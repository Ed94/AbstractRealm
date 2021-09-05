// Parent Header
#include "MasterExecution.hpp"

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
#include "TPAL_ImGui.hpp"


namespace Execution
{
	using namespace Concurrency;

	void MainCycle();

#pragma region StaticData
	PrimitiveExecuter<void()> MasterExecuter;

	Cycler MasterCycler;
#pragma endregion StaticData


	const Cycler& Get_MasterCycler()
	{
		return MasterCycler;
	}

	void Initialize_MasterCycler()
	{
		MasterExecuter.Bind(MainCycle);

		MasterCycler.BindExecuter(MasterExecuter);

		Dev::CLog(String("Interval: ") + ToString(Duration<f64, Ratio<1>>(1.0 / 144.0).count()));

		MasterCycler.Initiate();

		Dev::Console_DisableAutoUpdate();
	}

	void MainCycle()
	{
		using namespace TPAL;

		OSAL::PollEvents();

		static Duration64 _ConsoleUpdateDelta(0), _ConsoleUpdateInterval(1.0 / 30.0);
		static Duration64 _RenderPresentDelta(0);

		if (_ConsoleUpdateDelta >= _ConsoleUpdateInterval)
		{
			Dev::CLog_Status("Master    Delta: " + ToString(MasterCycler.GetDeltaTime().count()), 0, 0);
			Dev::CLog_Status("Render    Delta: " + ToString(_RenderPresentDelta.count()), 1, 0);

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

			Dev::CLog_Status("Console   Delta: " + ToString(_ConsoleUpdateDelta.count()), 2, 0);

			_ConsoleUpdateDelta = Duration64(0);
		}
		else
		{
			_ConsoleUpdateDelta += MasterCycler.GetDeltaTime();
		}

		if (_RenderPresentDelta >= Renderer::Get_PresentInterval())
		{
			switch (Meta::GPU_API())
			{
				case Meta::EGPUPlatformAPI:: Methane :
					Exception::Fatal::NotImplemented("Software rendering not supported.");
					[[fallthrough]];

				case Meta::EGPUPlatformAPI:: Vulkan :
				{
								Imgui ::MakeFrame();
								Imgui ::Render();
					HAL::GPU::	Vulkan::Render();
					HAL::GPU::	Vulkan::Present();
								Imgui ::Dirty_DoSurfaceStuff(Renderer::EngineWindow());

					break;
				} 

				default: [[fallthrough]];
				case Meta::EGPUPlatformAPI::No_API :
					Exception::Fatal::NotImplemented("Software rendering not supported.");
			}

			_RenderPresentDelta = Duration64(0);
		}
		else
		{
			_RenderPresentDelta += MasterCycler.GetDeltaTime();
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
