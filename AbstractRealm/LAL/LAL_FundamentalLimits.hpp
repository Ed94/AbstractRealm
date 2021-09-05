/*
STL Fundamental Limits

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	using IntMaxType = STL::intmax_t;

	template<typename Type>
	using NumLimits = STL::numeric_limits<Type>;

	constexpr s8 SInt8Min  = NumLimits<s8>::min();
	constexpr s8 SInt8Max  = NumLimits<s8>::max();

	constexpr s16 SInt16Max = NumLimits<s16>::min();
	constexpr s16 SInt16Min = NumLimits<s16>::max();

	constexpr s32 SInt32Min = NumLimits<s32>::min();
	constexpr s32 SInt32Max = NumLimits<s32>::max();

	constexpr s64 SInt64Min = NumLimits<s64>::min();
	constexpr s64 SInt64Max = NumLimits<s64>::max();

	constexpr u8  UInt8Max  = NumLimits<u8 >::max();
	constexpr u16 UInt16Max = NumLimits<u16>::max();
	constexpr u32 UInt32Max = NumLimits<u32>::max();
	constexpr u64 UInt64Max = NumLimits<u64>::max();

	constexpr f32 Float32_Min      = NumLimits<f32>::min          ()             ;
	constexpr f32 Float32_MinNeg   = NumLimits<f32>::min          () * f32(-1.0f);
	constexpr f32 Float32_Max      = NumLimits<f32>::max          ()             ;
	constexpr f32 Float32_MaxNeg   = NumLimits<f32>::max          () * f32(-1.0f);
	constexpr f32 Float32_Epsilon  = NumLimits<f32>::epsilon      ()             ;
	constexpr f32 Float32_Infinity = NumLimits<f32>::infinity     ()             ;
	constexpr f32 Float32_QNaN     = NumLimits<f32>::quiet_NaN    ()             ;
	constexpr f32 Float32_SNaN     = NumLimits<f32>::signaling_NaN()             ;

	constexpr f32 Float32_HighAccuracy = 0.00001F;
	constexpr f32 Float32_LowAccuracy  = 0.001F  ;

	constexpr f64 Float64_Min      = NumLimits<f64>::min          ()            ;
	constexpr f64 Float64_MinNeg   = NumLimits<f64>::min          () * f64(-1.0);
	constexpr f64 Float64_Max      = NumLimits<f64>::max          ()            ;
	constexpr f64 Float64_MaxNeg   = NumLimits<f64>::max          () * f64(-1.0);
	constexpr f64 Float64_Epsilon  = NumLimits<f64>::epsilon      ()            ;
	constexpr f64 Float64_Infinity = NumLimits<f64>::infinity     ()            ;
	constexpr f64 Float64_QNaN     = NumLimits<f64>::quiet_NaN    ()            ;
	constexpr f64 Float64_SNaN     = NumLimits<f64>::signaling_NaN()            ;

	constexpr f64 Float64_NanoAccuracy = 0.000000001F;
	constexpr f64 Float64_HighAccuracy = 0.00001F    ;
	constexpr f64 Float64_LowAccuracy  = 0.001F      ;
}
