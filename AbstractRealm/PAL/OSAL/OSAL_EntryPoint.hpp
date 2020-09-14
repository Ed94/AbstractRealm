/*****************************************************************//**
 * \file   OSAL_EntryPoint.hpp
 * \brief  This header must be included after the entry point is defined.
 * OSAL_SpeicifyEntryPoint must be used after including header to specify entry point.
 * 
 * \author Edward
 * \date   August 2020
 *********************************************************************/



#pragma once



#include "LAL.hpp"
#include "OSAL_Platform.hpp"



namespace OSAL
{
	/**
	* Engine application entry point.
	*/
	ExitValT EntryPoint();
}
