// Parent Header
#include "_TutorialRelated.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"



namespace HAL::GPU::Vulkan
{
	StaticData	
	(
		// Public

		DynamicArray<Vertex> ModelVerticies;
		DynamicArray<uint32> ModelIndicies ;

		// TODO: Make the GPU hal agnostic to this.

		Buffer VertexBuffer      ;
		Memory VertexBufferMemory;

		Buffer IndexBuffer      ;
		Memory IndexBufferMemory;

		DynamicArray<Buffer> UniformBuffers      ;
		DynamicArray<Memory> UniformBuffersMemory;

		Image     TextureImage      ;
		Memory    TextureImageMemory;
		ImageView TextureImageView  ;
		Sampler   TextureSampler    ;
	)
}