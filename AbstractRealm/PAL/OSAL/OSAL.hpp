/*
Operating System Abstraction Layer
*/



#pragma once



#include "OSAL_Platform.hpp"
#include "OSAL_Console.hpp"
#include "OSAL_Hardware.hpp"
#include "OSAL_Timing.hpp"
#include "OSAL_Threading.hpp"
#include "OSAL_Windowing.hpp"



namespace OSAL
{
	/*
	Loads the OSAL submodule.

	Note: This prepares the submodule for use of its related functionality supported.
	*/
	void Load();


	void Unload();

	/*
	Currently based of GLFW events (Since there are no other OS related events yet).
	*/
	void PollEvents();

	void Record_EditorDevDebugUI();
}
