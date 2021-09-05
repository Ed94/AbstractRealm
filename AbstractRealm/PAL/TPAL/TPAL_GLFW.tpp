#pragma once

// Parent Header
//#include "TPAL_GLFW.hpp"


namespace TPAL::GLFW
{
	template<typename ParameterType>
	void SetWindowCreationParameter(EWindowCreationParameter _param, ParameterType _value)
	{
		using namespace TP_API;

		glfwWindowHint(int(_param), int(_value));

		return;
	}
}