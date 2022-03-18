#ifndef BACKEND_UNIT
static_assert(false, 
	"This file CANNOT be used outside of backend units. "
	"Backend units CAN POLLUTE translation units of C/C++ heavily."
);
#endif

#ifndef HPP_API_Infoware

#include "infoware/system.hpp"

#define HPP_API_Infoware
#endif