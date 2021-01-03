// Parent Header
#include "GPUVK_Shaders.hpp"



#include "Core/IO/Basic_FileIO.hpp"
#include "HAL_Backend.hpp"



namespace HAL::GPU::Vulkan
{
	namespace SPIR_V
	{
		BuiltInResource Hardcoded_Resource;

		void InitializeResource()
		{
			Hardcoded_Resource.maxLights = 32;
			Hardcoded_Resource.maxClipPlanes = 6;
			Hardcoded_Resource.maxTextureUnits = 32;
			Hardcoded_Resource.maxTextureCoords = 32;
			Hardcoded_Resource.maxVertexAttribs = 64;
			Hardcoded_Resource.maxVertexUniformComponents = 4096;
			Hardcoded_Resource.maxVaryingFloats = 64;
			Hardcoded_Resource.maxVertexTextureImageUnits = 32;
			Hardcoded_Resource.maxCombinedTextureImageUnits = 80;
			Hardcoded_Resource.maxTextureImageUnits = 32;
			Hardcoded_Resource.maxFragmentUniformComponents = 4096;
			Hardcoded_Resource.maxDrawBuffers = 32;
			Hardcoded_Resource.maxVertexUniformVectors = 128;
			Hardcoded_Resource.maxVaryingVectors = 8;
			Hardcoded_Resource.maxFragmentUniformVectors = 16;
			Hardcoded_Resource.maxVertexOutputVectors = 16;
			Hardcoded_Resource.maxFragmentInputVectors = 15;
			Hardcoded_Resource.minProgramTexelOffset = -8;
			Hardcoded_Resource.maxProgramTexelOffset = 7;
			Hardcoded_Resource.maxClipDistances = 8;
			Hardcoded_Resource.maxComputeWorkGroupCountX = 65535;
			Hardcoded_Resource.maxComputeWorkGroupCountY = 65535;
			Hardcoded_Resource.maxComputeWorkGroupCountZ = 65535;
			Hardcoded_Resource.maxComputeWorkGroupSizeX = 1024;
			Hardcoded_Resource.maxComputeWorkGroupSizeY = 1024;
			Hardcoded_Resource.maxComputeWorkGroupSizeZ = 64;
			Hardcoded_Resource.maxComputeUniformComponents = 1024;
			Hardcoded_Resource.maxComputeTextureImageUnits = 16;
			Hardcoded_Resource.maxComputeImageUniforms = 8;
			Hardcoded_Resource.maxComputeAtomicCounters = 8;
			Hardcoded_Resource.maxComputeAtomicCounterBuffers = 1;
			Hardcoded_Resource.maxVaryingComponents = 60;
			Hardcoded_Resource.maxVertexOutputComponents = 64;
			Hardcoded_Resource.maxGeometryInputComponents = 64;
			Hardcoded_Resource.maxGeometryOutputComponents = 128;
			Hardcoded_Resource.maxFragmentInputComponents = 128;
			Hardcoded_Resource.maxImageUnits = 8;
			Hardcoded_Resource.maxCombinedImageUnitsAndFragmentOutputs = 8;
			Hardcoded_Resource.maxCombinedShaderOutputResources = 8;
			Hardcoded_Resource.maxImageSamples = 0;
			Hardcoded_Resource.maxVertexImageUniforms = 0;
			Hardcoded_Resource.maxTessControlImageUniforms = 0;
			Hardcoded_Resource.maxTessEvaluationImageUniforms = 0;
			Hardcoded_Resource.maxGeometryImageUniforms = 0;
			Hardcoded_Resource.maxFragmentImageUniforms = 8;
			Hardcoded_Resource.maxCombinedImageUniforms = 8;
			Hardcoded_Resource.maxGeometryTextureImageUnits = 16;
			Hardcoded_Resource.maxGeometryOutputVertices = 256;
			Hardcoded_Resource.maxGeometryTotalOutputComponents = 1024;
			Hardcoded_Resource.maxGeometryUniformComponents = 1024;
			Hardcoded_Resource.maxGeometryVaryingComponents = 64;
			Hardcoded_Resource.maxTessControlInputComponents = 128;
			Hardcoded_Resource.maxTessControlOutputComponents = 128;
			Hardcoded_Resource.maxTessControlTextureImageUnits = 16;
			Hardcoded_Resource.maxTessControlUniformComponents = 1024;
			Hardcoded_Resource.maxTessControlTotalOutputComponents = 4096;
			Hardcoded_Resource.maxTessEvaluationInputComponents = 128;
			Hardcoded_Resource.maxTessEvaluationOutputComponents = 128;
			Hardcoded_Resource.maxTessEvaluationTextureImageUnits = 16;
			Hardcoded_Resource.maxTessEvaluationUniformComponents = 1024;
			Hardcoded_Resource.maxTessPatchComponents = 120;
			Hardcoded_Resource.maxPatchVertices = 32;
			Hardcoded_Resource.maxTessGenLevel = 64;
			Hardcoded_Resource.maxViewports = 16;
			Hardcoded_Resource.maxVertexAtomicCounters = 0;
			Hardcoded_Resource.maxTessControlAtomicCounters = 0;
			Hardcoded_Resource.maxTessEvaluationAtomicCounters = 0;
			Hardcoded_Resource.maxGeometryAtomicCounters = 0;
			Hardcoded_Resource.maxFragmentAtomicCounters = 8;
			Hardcoded_Resource.maxCombinedAtomicCounters = 8;
			Hardcoded_Resource.maxAtomicCounterBindings = 1;
			Hardcoded_Resource.maxVertexAtomicCounterBuffers = 0;
			Hardcoded_Resource.maxTessControlAtomicCounterBuffers = 0;
			Hardcoded_Resource.maxTessEvaluationAtomicCounterBuffers = 0;
			Hardcoded_Resource.maxGeometryAtomicCounterBuffers = 0;
			Hardcoded_Resource.maxFragmentAtomicCounterBuffers = 1;
			Hardcoded_Resource.maxCombinedAtomicCounterBuffers = 1;
			Hardcoded_Resource.maxAtomicCounterBufferSize = 16384;
			Hardcoded_Resource.maxTransformFeedbackBuffers = 4;
			Hardcoded_Resource.maxTransformFeedbackInterleavedComponents = 64;
			Hardcoded_Resource.maxCullDistances = 8;
			Hardcoded_Resource.maxCombinedClipAndCullDistances = 8;
			Hardcoded_Resource.maxSamples = 4;
			Hardcoded_Resource.limits.nonInductiveForLoops = 1;
			Hardcoded_Resource.limits.whileLoops = 1;
			Hardcoded_Resource.limits.doWhileLoops = 1;
			Hardcoded_Resource.limits.generalUniformIndexing = 1;
			Hardcoded_Resource.limits.generalAttributeMatrixVectorIndexing = 1;
			Hardcoded_Resource.limits.generalVaryingIndexing = 1;
			Hardcoded_Resource.limits.generalSamplerIndexing = 1;
			Hardcoded_Resource.limits.generalVariableIndexing = 1;
			Hardcoded_Resource.limits.generalConstantMatrixVectorIndexing = 1;
		}

		EStage GetStageType(const EShaderStageFlag _shaderType)
		{
			switch (_shaderType)
			{
				case EShaderStageFlag::Vertex :
					return EStage::Vertex;

				case EShaderStageFlag::TessellationControl :
					return EStage::TesselationControl;

				case EShaderStageFlag::TessellationEvaluation :
					return EStage::TesselationEvaluation;

				case EShaderStageFlag::Geometry :
					return EStage::Geometry;

				case EShaderStageFlag::Fragment :
					return EStage::Fragment;

				case EShaderStageFlag::Compute :
					return EStage::Compute;

				default:
					throw RuntimeError("Unknown shader type specified. Exiting!");
			}
		}

		bool CompileGLSL(const Path& _path, const EShaderStageFlag _type, Bytecode_Buffer& _bytecode)
		{
			unbound constexpr auto numShaders = 1;

			unbound bool doneOnce = false;

			if (!doneOnce) 
			{
				InitializeResource();

				doneOnce = true;
			}

			auto shaderCode = Core::IO::BufferFile(_path);

			MessageFlags messageOptions;

			messageOptions.Set(EMessageFlag::SPIRV_Rules, EMessageFlag::VulkanRules);

			EStage stage = GetStageType(_type);

			UPtr<LinkerUnit> linker = MakeUPtr<LinkerUnit>();

			UPtr<ShaderUnit> shader = MakeUPtr<ShaderUnit>( EShLanguage(stage));

			RoCStr strings[numShaders];
			
			strings[0] = shaderCode.data();

			shader->setStrings(strings, numShaders);

			if (! shader->parse(getAddress(Hardcoded_Resource), 100, false, (EShMessages)ui32(messageOptions)))
			{
				CLog_Error(String(shader->getInfoLog()));
				CLog_Error(String(shader->getInfoDebugLog()));

				return false;
			}

			linker->addShader(shader.get());

			if (! linker->link( (EShMessages)ui32(messageOptions)))
			{
				CLog_Error(String(shader->getInfoLog()));
				CLog_Error(String(shader->getInfoDebugLog()));

				return false;
			}

			glslang::GlslangToSpv(dref(linker->getIntermediate( EShLanguage(stage))), _bytecode);

			return true;
		}
	}
}