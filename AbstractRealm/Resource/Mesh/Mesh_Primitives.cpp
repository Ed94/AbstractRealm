// Parent Header
#include "Mesh_Primitives.hpp"


namespace Resource::Mesh
{
	namespace _2D
	{
		Triangle Triangle::Identity()
		{
			return 
			
			Triangle
			(
				Vertex( 0.0f,  1.0f),
				Vertex(-1.0f, -1.0f),
				Vertex( 1.0f, -1.0f)
			);
		}

		Square Square::Identity()
		{
			return Square(1.0f, 1.0f);
		}
	}

	namespace _3D
	{
		Triangle Triangle::Identity()
		{
			return

			Triangle
			(
				Vertex( 0.0f,  1.0f, 0.0f),
				Vertex(-1.0f, -1.0f, 0.0f),
				Vertex( 1.0f, -1.0f, 0.0f)
			);
		}

		Square Square::Identity()
		{
			return Square(1.0f, 1.0f);
		}
	}
}