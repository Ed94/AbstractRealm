#pragma once


#include "LAL/LAL.hpp"
#include "Meta/AppInfo.hpp"






namespace HAL::GPU::BGFX
{
	using namespace LAL; using namespace Meta;

	void Start_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion);

	void Setup_Window();

	void Cease_GPUComms();
}
