/*
Executer

Last Modified: 5/18/2020
*/



#pragma once



// Engine
#include "LAL/LAL.hpp"
#include "Meta/EngineInfo.hpp"
#include "OSAL/OSAL.hpp"
#include "HAL/GPU_HAL.hpp"
#include "Dev/Dev.hpp"



namespace Core::Execution
{
	// Namespaces

	using namespace LAL ;
	using namespace Meta;



	// Enums

	enum class EExecutionType
	{
		Primitive,   // A rudimentary execution system. 
		Engine   ,   // The executer for the engine level routines.
		Editor       // The executer for the editor level routines.
	};

	// Possibly be used later to optimize execution calls...
	enum class EParameterFlag
	{
		DeltaTime,

	};

	enum class EReturnCode
	{
		CriticalFailure,
		Completed
	};

	enum class EModule
	{
		Core       ,
		Dev        ,
		Editor     ,
		IO         ,
		Manifest   ,
		Profile    ,
		Renderer   ,
		Simulation ,
		User       ,
		Custom
	};



	// Classes

	class AExecuter
	{
	public:
		~AExecuter() {};

		virtual void Execute() = NULL;
		//virtual void Execute(Duration64 _deltaTime) = NULL;
	};

	template<typename FN_Type>
	class PrimitiveExecuter : AExecuter
	{
	public:
		unbound constexpr EExecutionType Type = EExecutionType::Primitive;

		implem void Bind(Function<FN_Type> _task);

		implem void Execute();
		//implem void Execute(Duration64 _deltaTime);

		operator ptr<AExecuter>()
		{
			return RCast<AExecuter>(this);
		}

	protected:
	private:
		Function<FN_Type> task;
	};

	class QueuedExecuter : AExecuter
	{
	public:
		unbound constexpr EExecutionType Type = EExecutionType::Engine;

		template<typename FN_Type>
		void Add(Function<FN_Type> _routine);

		implem void Execute();
		//implem void Execute(Duration64 _deltaTime);

	protected:
		void ToggleIdle();

	private:	
		Queue<ptr<AExecuter>> queue;
	};
	
	/**
	 * Engine application entry point.
	 */
	OSAL::ExitValT EntryPoint();


	eGlobal ptr<OSAL::Window> EngineWindow;
}



// Template Implementation
// #include "PrimitiveExecuter_Implem.hpp"
// TODO ^^ Why doesn't it work? (SEE GLFW FOR WORKING VER)

namespace Core::Execution
{
	template<typename FN_Type>
	void PrimitiveExecuter<FN_Type>::Bind(Function<FN_Type> _task)
	{
		task = _task;
	}

	template<typename FN_Type>
	void PrimitiveExecuter<FN_Type>::Execute()
	{
		task();
	}
}


