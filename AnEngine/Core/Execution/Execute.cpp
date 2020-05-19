/*
Execute

Last Modified: 5/18/2020

Currently the only defined main in the engine. You can manually define what procedure to execute here...
*/



#include "LAL/LAL.hpp"



#include "Debug/Exec_GLFW_GLM_Test.hpp"
#include "Debug/Vulkan_TriangleTest.hpp"



int main()
{
	// Debug Suite

	Debug::Exec_GLFW_GLM_Test();

	if (!Debug::Vulkan_TriangleTest::Run()) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}