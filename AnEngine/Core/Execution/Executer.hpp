/*
Executer

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL/LAL.hpp"


#include "Meta/EngineInfo.hpp"
#include "Dev/Dev.hpp"
#include "OSAL/OSAL.hpp"
#include "HAL/GPU_HAL.hpp"


namespace Core::Execution
{
	using namespace LAL;
	using namespace Meta;


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
		using FN_Procedure = Function<FN_Type>;

		unbound constexpr EExecutionType Type = EExecutionType::Primitive;

		implem void Bind(Function<FN_Type> _procedure);

		implem void Execute();
		//implem void Execute(Duration64 _deltaTime);

		operator ptr<AExecuter>()
		{
			return RCast<AExecuter>(this);
		}

	protected:
	private:
		FN_Procedure procedure;
	};

	class EngineExecuter : AExecuter
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

	
	class EditorExecuter : AExecuter
	{
	public:
	protected:
	private:
		Queue<ptr<AExecuter>> queue;
	};


	/**
	 * Engine application entry point.
	 */
	OSAL::ExitValT EntryPoint();
}


namespace Core::Execution
{
	template<typename FN_Type>
	void PrimitiveExecuter<FN_Type>::Bind(Function<FN_Type> _procedure)
	{
		procedure = _procedure;
	}

	template<typename FN_Type>
	void PrimitiveExecuter<FN_Type>::Execute()
	{
		procedure();
	}
}


