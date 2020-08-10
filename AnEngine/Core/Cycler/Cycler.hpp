#pragma once



#include "LAL/LAL.hpp"



namespace Core::Execution
{
	enum class CyclerType
	{
		Core       ,
		Debugger   ,
		Editor     ,
		IO         ,  
		Profile    ,
		Render     ,
		Simulation ,
		UI         ,
		UserManager,
		Custom
	};

	enum class EReturnCode
	{
		CriticalFailure,
		Completed
	};

	class ACycler
	{
	public:
		

	public:
		~ACycler() {};

		virtual void        Initiate          () = NULL;
		virtual EReturnCode Initiate_withRCode() = NULL;
	};

	class Cycler : public ACycler
	{
	public:

		Cycler();

		implem void        Initiate          ();
		implem EReturnCode Initiate_withRCode();

	private:

		bool exist;
	};
}
