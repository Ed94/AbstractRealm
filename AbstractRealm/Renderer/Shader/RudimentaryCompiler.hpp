/*
Rudimentary Shader Compiler


TODO: Work on this when there is a bunch of shaders...
*/


#pragma once

#include "Renderer_Backend.hpp"
#include "shaderc/shaderc.hpp"


namespace Renderer::Shader
{
	// Not using this after finding out about shaderc...
	//constexpr RoCStr GLSLC_Path = "./Engine/ThirdParty/Tools/glslc.exe";

	// ShaderC Use

	using shaderc::Compiler      ;
	using shaderc::CompileOptions;

	using ShaderKind = shaderc_shader_kind;

	shaderc::Compiler       ShaderC_Compiler      ;
	shaderc::CompileOptions ShaderC_CompileOptions;

	//ShaderC_Instance.CompileGlslToSpv(const String& source_text, shaderc_shader_kind shader_kind, const char* input_file_name)
}
