#pragma once

#include "GPUVK_Comms.hpp"

#include "Renderer/Shader/Shader.hpp"


namespace HAL::GPU::Vulkan
{
	class ShaderModule : public V3::ShaderModule
	{
		using Parent = V3::ShaderModule;

	public:

		 ShaderModule();
		~ShaderModule();

	protected:

		CreateInfo info;
	};

	class AShader
	{
	public:

		using ShaderStageInfo = Pipeline::ShaderStage::CreateInfo;

		~AShader() {};



		virtual const DynamicArray<ShaderStageInfo>& GetShaderStageInfos() const = NULL;

		virtual DeviceSize GetUniformSize() const = NULL;
	};

	// Supports only vertex and fragment shader pair.
	class BasicShader : public AShader
	{
	public:

		 BasicShader();
		 BasicShader(const Path& _vertShader, const Path& _fragShader, DeviceSize _uniformSize);
		~BasicShader();

		 // Mitigating stuff...
		 

		void Create(const Path& _vertShader, const Path& _fragShader);

		void Create(const Path& _vertShader, const Path& _fragShader, DeviceSize _uniformSize);

		void Destroy();

		//void Destroy();

		const DynamicArray<ShaderStageInfo>& GetShaderStageInfos() const override;

		virtual DeviceSize GetUniformSize() const override { return uboSize; }


	protected:

		ShaderModule    shaderModules[2];
		ShaderStageInfo shaderStageInfos[2];   // One for vertex and for fragment.

		DeviceSize uboSize = 0;
	};


	namespace SPIR_V
	{
		enum class EStage
		{
			Vertex                = EShLangVertex,
			TesselationControl    = EShLangTessControl,
			TesselationEvaluation = EShLangTessEvaluation,
			Geometry              = EShLangGeometry,
			Fragment              = EShLangFragment,
			Compute               = EShLangCompute,
			RayGen                = EShLangRayGen,
			RayGenNV              = EShLangRayGenNV,
			Intersect             = EShLangIntersect,
			IntersectNV           = EShLangIntersectNV,
			AnyHit                = EShLangAnyHit,
			AnyHitNV              = EShLangAnyHitNV,
			ClosestHit            = EShLangClosestHit,
			ClosestHitNV          = EShLangClosestHitNV,
			Miss                  = EShLangMiss,
			MissNV                = EShLangMissNV,
			Callable              = EShLangCallable,
			CallableNV            = EShLangCallableNV,
			TaskNV                = EShLangTaskNV,
			MeshNV                = EShLangMeshNV,
		};

		enum class EMessageFlag : ui32
		{
			Default               = EShMsgDefault             ,  // default is to give all required errors and extra warnings
			RelaxedErrors         = EShMsgRelaxedErrors       ,  // be liberal in accepting input
			SuppressWarnings      = EShMsgSuppressWarnings    ,  // suppress all warnings, except those required by the specification
			AST_IR                = EShMsgAST                 ,  // print the AST intermediate representation
			SPIRV_Rules           = EShMsgSpvRules            ,  // issue messages for SPIR-V generation
			VulkanRules           = EShMsgVulkanRules         ,  // issue messages for Vulkan-requirements of GLSL for SPIR-V
			OnlyPreprocessor      = EShMsgOnlyPreprocessor    ,  // only print out errors produced by the preprocessor
			Read_HLSL             = EShMsgReadHlsl            ,  // use HLSL parsing rules and semantics
			CascadingErrors       = EShMsgCascadingErrors     ,  // get cascading errors; risks error-recovery issues, instead of an early exit
			KeepUncalled          = EShMsgKeepUncalled        ,  // for testing, don't eliminate uncalled functions
			HLSL_Offsets          = EShMsgHlslOffsets         ,  // allow block offsets to follow HLSL rules instead of GLSL rules
			DebugInfo             = EShMsgDebugInfo           ,  // save debug information
			HSLS_Enable16BitTypes = EShMsgHlslEnable16BitTypes,  // enable use of 16-bit types in SPIR-V for HLSL
			HLSL_Legalization     = EShMsgHlslLegalization    ,  // enable HLSL Legalization messages
			HLSL_DX9_Compatible   = EShMsgHlslDX9Compatible   ,  // enable HLSL DX9 compatible mode (for samplers and semantics)
			BuiltinSymbolTable    = EShMsgBuiltinSymbolTable  ,  // print the builtin symbol table

			VV_SpecifyBitmaskable
		};

		using ShaderUnit      = glslang::TShader;
		using LinkerUnit      = glslang::TProgram;
		using BuiltInResource = TBuiltInResource;

		using VV::SPIR_V::Bytecode_Buffer;

		using MessageFlags = VV::V0::Bitfield<EMessageFlag, u32>;

		

		/*class Compiler
		{

		public:

			unbound bool CompileGLSL(Path& _path, const EShaderStageFlag _type, );


		protected:

			RoCStr strings[1];

			ptr<ShaderUnit> shader;
			ptr<LinkerUnit> linker;
		};*/

		// Compiles GLSL to SPIR-V Bytecode.
		// Note this is hardcoded to only link one shader for now.
		bool CompileGLSL(const Path& _path, const EShaderStageFlag _type, Bytecode_Buffer& _bytecode);
	}
}
