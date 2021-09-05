#pragma once

#include "Renderer_Backend.hpp"


namespace Renderer::Shader
{
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

		DynamicArray<Path> source;
	};
}
