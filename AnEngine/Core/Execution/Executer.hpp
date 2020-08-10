/*
Executer

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL/LAL.hpp"



namespace Core::Execution
{
	using namespace LAL;


	enum class EExecutionType
	{
		Primitive,   // A rudimentary execution system. (Used by cycle for example)
		Engine   ,   // The executer for the engine level routines.
		Editor       // The executer for the editor level routines.
	};

	template<EExecutionType>
	class Executer
	{
	public:
		 Executer();
		~Executer();

	protected:

	private:
	};


	template<>
	class Executer<EExecutionType::Primitive>
	{
	public:
	protected:
	private:
	};

	template<>
	class Executer<EExecutionType::Engine>
	{
	public:
	protected:
	private:
	};

	template<>
	class Executer<EExecutionType::Editor>
	{
	public:
	protected:
	private:
	};



	/**
	 * Loads the Core's execution module and starts up the .
	 */
	int InitiateExecution();
}
