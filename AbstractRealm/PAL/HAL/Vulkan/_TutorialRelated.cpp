// Parent Header
#include "_TutorialRelated.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb/stb_image.h"



namespace HAL::GPU::Vulkan
{
	StaticData()

		// Public

		DynamicArray<Vertex_WTexture> ModelVerticies;
		DynamicArray<u32> ModelIndicies ;

		// TODO: Make the GPU hal agnostic to this.

		Buffer VertexBuffer_Old      ;
		Memory VertexBufferMemory;

		Buffer IndexBuffer_Old      ;
		Memory IndexBufferMemory;

		DynamicArray<Buffer> UniformBuffers      ;
		DynamicArray<Memory> UniformBuffersMemory;

		Image     TextureImage_Old      ;
		Memory    TextureImageMemory;
		ImageView TextureImageView  ;
		Sampler   TextureSampler    ;



	void LoadModel(String _modelPath)
	{
		tinyobj::attrib_t attrib;

		DynamicArray<tinyobj::shape_t> shapes;

		DynamicArray<tinyobj::material_t> materials;

		String warning, error;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, _modelPath.c_str()))
			throw RuntimeError(warning + error);

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex_WTexture vertex{};

				vertex.Position =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.TextureCoordinates =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.Color = { 1.0f, 1.0f, 1.0f };

				ModelVerticies.push_back(vertex);
				ModelIndicies.push_back(SCast<u32>(ModelIndicies.size()));
			}
		}
	}
}