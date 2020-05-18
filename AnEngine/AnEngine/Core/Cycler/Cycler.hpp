#pragma once

#include "Meta/Explicit_Specifiers.hpp"

namespace Core::Cycler
{
	enum class CyclerType
	{
		Core      ,
		Debugger  ,
		Editor    ,
		IO        ,
		Profile   ,
		Render    ,
		Simulation,
		UI        ,
		UserManage,
		Custom
	};

	class ACycler
	{
	public:
		enum class ReturnCode
		{
			CriticalFailure,
			Completed
		};

	public:
		~ACycler() {};

		virtual void       Initiate          ();
		virtual ReturnCode Initiate_withRCode();
	};

	class Cycler : public ACycler
	{
	public:
		implem void       Initiate          ();
		implem ReturnCode Initiate_withRCode();

	private:

		bool exist;
	};
}
