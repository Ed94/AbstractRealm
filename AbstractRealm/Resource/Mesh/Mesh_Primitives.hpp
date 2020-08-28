#pragma once



#include "LAL.hpp"

#include "Meta/Config/Resource_Config.hpp"



namespace Resource::Mesh
{
	using namespace LAL;
	using namespace Meta;


	template<BitAccuracy _accuracy> 
	using Choose_Accuracy = 
		ChooseEither<Meta::Is<BitAccuracy::_32_Bit>(_accuracy),
			float32, float64>;

	using DefaultValue = Choose_Accuracy<Meta::Default_VertexAccuracy>;

	namespace _2D
	{
		template<typename Value>
		struct TVertex
		{
			TVertex() : X(0.0f), Y(0.0f) {}

			TVertex(Value _x, Value _y) :
				X(_x), Y(_y)
			{}

			Value X, Y;
		};

		using Vertex = TVertex<DefaultValue>;

		struct Triangle
		{
			Triangle(Vertex _v1, Vertex _v2, Vertex _v3) :
				V1(_v1), V2(_v2), V3(_v3) 
			{}

			static Triangle Identity(); 

			Vertex V1, V2, V3;
		};

		struct Square
		{
			//Square() : 
			//	V1( -1.0f,  1.0f ),
			//	V2(  1.0f,  1.0f ),
			//	V3( -1.0f, -1.0f ),
			//	V4(  1.0f, -1.0f )
			//{}

			Square(DefaultValue _x, DefaultValue _y) :
				V1(-_x,  _y),
				V2( _x,  _y),
				V3(-_x, -_y),
				V4( _x, -_y)
			{}

			static Square Identity();

			Vertex V1, V2, V3, V4;
		};
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

		using Vertex = TVertex<DefaultValue>;

		struct Triangle
		{
			Triangle(Vertex _v1, Vertex _v2, Vertex _v3) :
				V1(_v1), V2(_v2), V3(_v3)
			{}

			static Triangle Identity();

			Vertex V1, V2, V3;
		};

		struct Square
		{
			Square(DefaultValue _x, DefaultValue _y) :
				V1(-_x,  _y, 0.0f),
				V2( _x,  _y, 0.0f),
				V3(-_x, -_y, 0.0f),
				V4( _x, -_y, 0.0f)
			{}

			static Square Identity();

			Vertex V1, V2, V3, V4;
		};
	}
}	
