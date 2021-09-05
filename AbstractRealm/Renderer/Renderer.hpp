#pragma once


#include "Renderer_Backend.hpp"
#include "OSAL/OSAL_Windowing.hpp"


namespace Renderer
{
	const Duration64& Get_PresentInterval();	

	void Load();

	void Unload();

	ptr<OSAL::Window> EngineWindow();
}
