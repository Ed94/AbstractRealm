/*
GLFW and GLM test execution

A very rudimentary test of the GLFW and GLM libraries. 
Reference: https://vulkan-tutorial.com/en/Development_environment
*/



#pragma once



#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


#include "LAL.hpp"
#include "Core/Memory/MemTypes.hpp"


namespace Debug
{
	/* 
	Raw test copy and paste from:
	https://vulkan-tutorial.com/Development_environment
	*/
	int Exec_GLFW_GLM_Test()
	{
		using namespace Core::Memory;

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		GLFWwindow* window = glfwCreateWindow(1280, 720, "AnEngine: GLFW, GLM Test", nullptr, nullptr);

		uint32_t extensionCount = 0;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::cout << extensionCount << " extensions supported\n";

		glm::mat4 matrix;
		glm::vec4 vec;

		Stack(auto) test = matrix * vec;

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}

		glfwDestroyWindow(window);

		glfwTerminate();

		return 0;
	}

	
}
