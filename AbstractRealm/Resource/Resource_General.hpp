#pragma once

#include "Resource_Backend.hpp"


namespace Resource
{
#pragma region Color

	using namespace LAL;

	using GreyscaleValue = Meta::Choose_FloatAccuracy<Meta::Greyscale_LinearAccuracy >;
	using ColorValue     = Meta::Choose_FloatAccuracy<Meta::ColorValue_LinearAccuracy>;


	template<typename Value>
	struct TGreyscale
	{
		TGreyscale() : value(0.0) {}

		TGreyscale(Value _value) : 
			value(_value) 
		{}

		Value 
			V,   // Value 
			A;   // Alpha
	};

	template<typename Value>
	struct TGreyscale_NoAlpha
	{
		TGreyscale_NoAlpha() {}

		Value V;   // Value
	};

	template<typename Value>
	struct TColor
	{
		TColor() : R(0.0), G(0.0), B(0.0), A(0.0) {}

		TColor(Value _red, Value _green, Value _blue) : 
			R(_red), G(_green), B(_blue), A(_alpha)
		{}

		Value 
			R,   // Red
			G,   // Green
			B,   // Blue
			A;   // Alpha
	};

	template<typename Value>
	struct TColor_NoAlpha
	{
		TColor_NoAlpha() : R(0.0), G(0.0), B(0.0)
		{}

		TColor_NoAlpha(Value _red, Value _green, Value _blue) : 
			R(_red), G(_green), B(_blue)
		{}

		Value 
			R,   // Red
			G,   // Green 
			B;   // Blue
	};


	using LinearGrey_NoA = TGreyscale_NoAlpha<GreyscaleValue>;
	using LinearGrey     = TGreyscale        <GreyscaleValue>;

	using Greyscale_8  = TGreyscale<u8 >;
	using Greyscale_16 = TGreyscale<u16>;
	using Greyscale_32 = TGreyscale<u32>;
	using Greyscale_64 = TGreyscale<u64>;

	using Greyscale_NoA_8  = TGreyscale_NoAlpha<u8 >;
	using Greyscale_NoA_16 = TGreyscale_NoAlpha<u16>;
	using Greyscale_NoA_32 = TGreyscale_NoAlpha<u32>;
	using Greyscale_NoA_64 = TGreyscale_NoAlpha<u64>;

	using Greyscale     = Greyscale_8    ;
	using Greyscale_NoA = Greyscale_NoA_8;

	using LinearColor     = TColor        <ColorValue>;
	using LinearColor_NoA = TColor_NoAlpha<ColorValue>;

	using Color_8  = TColor<u8 >;
	using Color_16 = TColor<u16>;
	using Color_32 = TColor<u32>;
	using Color_64 = TColor<u64>;

	using Color_NoA_8  = TColor<u8 >;
	using Color_NoA_16 = TColor<u16>;
	using Color_NoA_32 = TColor<u32>;
	using Color_NoA_64 = TColor<u64>;

	using Color = Color_8;

#pragma endregion Color

#pragma region Vertex
	
	// Vertex Value should be inherited from Sci's DecN...

	using VertexValue = Meta::Choose_FloatAccuracy<Meta::VertexValue_Accuracy>;


	namespace _2D
	{
		template<typename Value>
		struct TVertex
		{
			TVertex() : X(0.0), Y(0.0) {}

			TVertex(Value _x, Value _y) :
				X(_x), Y(_y)
			{}

			Value X, Y;
		};

		using Vertex = TVertex<VertexValue>;
	}

	namespace _3D
	{
		template<typename Value>
		struct TVertex
		{
			TVertex(Value _x, Value _y, Value _z) :
				X(_x), Y(_y), Z(_z)
			{}

			Value X, Y, Z;
		};

		using Vertex = TVertex<VertexValue>;
	}

	/*template<typename VertexType, typename ColorType>
	struct TVertexColor
	{
		VertexType Vertex;
	};*/

#pragma endregion Vertex
}
