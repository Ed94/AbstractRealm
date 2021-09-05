#pragma once

#include "Resource/Resource_General.hpp"


namespace Resource::Mesh
{
	using namespace LAL;
	using namespace Meta;

	namespace _2D
	{
		using namespace Resource::_2D;

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
			Square(VertexValue _x, VertexValue _y) :
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
		using namespace Resource::_3D;

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
			Square(VertexValue _x, VertexValue _y) :
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
