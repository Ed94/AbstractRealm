/*
Executer

Last Modified: 5/18/2020
*/


#pragma once

#include "Execution_Backend.hpp"

#include "OSAL/OSAL_Entrypoint.hpp"


namespace Execution
{
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
		static constexpr EExecutionType Type = EExecutionType::Primitive;

		virtual void Bind(Function<FN_Type> _task) ;

		void Execute() override;
		//void Execute(Duration64 _deltaTime) override;

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
		static constexpr EExecutionType Type = EExecutionType::Engine;

		template<typename FN_Type>
		void Add(Function<FN_Type> _routine);

		void Execute() override;
		//void Execute(Duration64 _deltaTime) override;

	protected:
		void ToggleIdle();

	private:	
		Queue<ptr<AExecuter>> queue;
	};


	// Functions
	
	/**
	 * Engine application entry point.
	 */
	OSAL::ExitValT EntryPoint();

	//ptr<OSAL::Window> EngineWindow();
}



// Template Implementation
// #include "PrimitiveExecuter_Implem.hpp"
// TODO ^^ Why doesn't it work? (SEE GLFW FOR WORKING VER)

namespace Execution
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


