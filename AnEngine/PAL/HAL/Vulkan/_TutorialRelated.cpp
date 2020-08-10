// Parent Header
#include "_TutorialRelated.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"



namespace HAL::GPU::Vulkan
{
	DynamicArray<Vertex> ModelVerticies;
	DynamicArray<uint32> ModelIndicies ;
}