#pragma once


#include "LAL/LAL.hpp"

#include "OSAL/OSAL_Windowing.hpp"



namespace Renderer
{
	using namespace LAL;

	const Duration64& Get_PresentInterval();	

	void Load();

	void Unload();


	ptr<OSAL::Window> EngineWindow();
}
