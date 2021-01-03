#pragma once




#include "LAL.hpp"





namespace Renderer::Shader
{
	using namespace LAL;


	enum class ELanguage
	{
		GLSL,
		HLSL
	};



	class Shader
	{
	public:

	protected:

		String name;

		ELanguage language;

		Path shaderPath;
	};
}
