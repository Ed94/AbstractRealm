/*
STL Fundamental Limits

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	template<typename Type>
	using NumLimits = std::numeric_limits<Type>;

	constexpr sI8 SInt8Min  = NumLimits<sI8>::min();
	constexpr sI8 SInt8Max  = NumLimits<sI8>::max();

	constexpr sI16 SInt16Max = NumLimits<sI16>::min();
	constexpr sI16 SInt16Min = NumLimits<sI16>::max();

	constexpr sI32 SInt32Min = NumLimits<sI32>::min();
	constexpr sI32 SInt32Max = NumLimits<sI32>::max();

	constexpr sI64 SInt64Min = NumLimits<sI64>::min();
	constexpr sI64 SInt64Max = NumLimits<sI64>::max();

	constexpr uI8  UInt8Max  = NumLimits<uI8 >::max();
	constexpr uI16 UInt16Max = NumLimits<uI16>::max();
	constexpr uI32 UInt32Max = NumLimits<uI32>::max();
	constexpr uI64 UInt64Max = NumLimits<uI64>::max();

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

	constexpr f32 Float64_NanoAccuracy = 0.000000001F;
	constexpr f32 Float64_HighAccuracy = 0.00001F    ;
	constexpr f32 Float64_LowAccuracy  = 0.001F      ;
}
