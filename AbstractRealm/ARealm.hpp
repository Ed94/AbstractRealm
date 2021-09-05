/*
ARealm

File: ARealm.hpp

The main include file that games should use.
*/


#pragma once


#include "Meta/Meta.hpp"
#include "PAL.hpp"
#include "Networking/Networking.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Resource.hpp"
#include "Simulation/Simulation.hpp"
#include "User/User.hpp"
#include "Editor/Editor.hpp"
#include "Core/Core.hpp"
#include "Manifest/Manifest.hpp"


namespace ARealm
{
	namespace LAL  = LAL;
	namespace Meta = Meta;
	namespace HAL
	{
		using namespace HAL;

		namespace GPU
		{
			using namespace HAL::GPU;
		}
	}
	namespace OSAL = OSAL;
	namespace PAL  = PAL;
}
