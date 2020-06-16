#pragma once

// Parent Header
//#include "GLFW_SAL.hpp"



namespace SAL
{
	namespace GLFW
	{
		template<typename ParameterType>
		void SetWindowCreationParameter(EWindowCreationParameter _param, ParameterType _value)
		{
			glfwWindowHint(int(_param), int(_value));

			return;
		}
	}
}