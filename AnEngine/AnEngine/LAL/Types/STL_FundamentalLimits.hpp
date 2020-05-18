/*
STL Fundamental Limits

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL/Cpp_STL.hpp"



namespace LAL
{
	template<typename Type>
	using NumLimits = std::numeric_limits<Type>;

	constexpr sInt8 SInt8Min  = NumLimits<sInt8>::min();
	constexpr sInt8 SInt8Max  = NumLimits<sInt8>::max();

	constexpr sInt16 SInt16Max = NumLimits<sInt16>::min();
	constexpr sInt16 SInt16Min = NumLimits<sInt16>::max();

	constexpr sInt32 SInt32Min = NumLimits<sInt32>::min();
	constexpr sInt32 SInt32Max = NumLimits<sInt32>::max();

	constexpr sInt64 SInt64Min = NumLimits<sInt64>::min();
	constexpr sInt64 SInt64Max = NumLimits<sInt64>::max();

	constexpr uInt8  UInt8Max  = NumLimits<uInt8 >::max();
	constexpr uInt16 UInt16Max = NumLimits<uInt16>::max();
	constexpr uInt32 UInt32Max = NumLimits<uInt32>::max();
	constexpr uInt64 UInt64Max = NumLimits<uInt64>::max();

	constexpr float32 Float32_Min      = NumLimits<float32>::min          ()                 ;
	constexpr float32 Float32_MinNeg   = NumLimits<float32>::min          () * float32(-1.0f);
	constexpr float32 Float32_Max      = NumLimits<float32>::max          ()                 ;
	constexpr float32 Float32_MaxNeg   = NumLimits<float32>::max          () * float32(-1.0f);
	constexpr float32 Float32_Epsilon  = NumLimits<float32>::epsilon      ()                 ;
	constexpr float32 Float32_Infinity = NumLimits<float32>::infinity     ()                 ;
	constexpr float32 Float32_QNaN     = NumLimits<float32>::quiet_NaN    ()                 ;
	constexpr float32 Float32_SNaN     = NumLimits<float32>::signaling_NaN()                 ;

	constexpr float32 Float32_HighAccuracy = 0.00001F;
	constexpr float32 Float32_LowAccuracy  = 0.001F  ;

	constexpr float64 Float64_Min      = NumLimits<float64>::min          ()                ;
	constexpr float64 Float64_MinNeg   = NumLimits<float64>::min          () * float64(-1.0);
	constexpr float64 Float64_Max      = NumLimits<float64>::max          ()                ;
	constexpr float64 Float64_MaxNeg   = NumLimits<float64>::max          () * float64(-1.0);
	constexpr float64 Float64_Epsilon  = NumLimits<float64>::epsilon      ()                ;
	constexpr float64 Float64_Infinity = NumLimits<float64>::infinity     ()                ;
	constexpr float64 Float64_QNaN     = NumLimits<float64>::quiet_NaN    ()                ;
	constexpr float64 Float64_SNaN     = NumLimits<float64>::signaling_NaN()                ;

	constexpr float32 Float64_NanoAccuracy = 0.000000001F;
	constexpr float32 Float64_HighAccuracy = 0.00001F    ;
	constexpr float32 Float64_LowAccuracy  = 0.001F      ;
}
