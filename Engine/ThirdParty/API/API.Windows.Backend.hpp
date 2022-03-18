#ifndef BACKEND_UNIT
static_assert(false, 
	"This file CANNOT be used outside of backend units. "
	"Backend units CAN POLLUTE translation units of C/C++ heavily."
);
#endif

#ifndef HPP_OSAL_API_Windows

// Windows
#ifdef _WIN32

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Weverything"
#endif
	#define NOMINMAX              // Prevents the numeric limits error.
	#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers

	#include <Windows.h>

		// Networking
	#include <WinSock2.h>
	#include <WS2tcpip.h>

		// IO
	#include <corecrt_io.h>
	#include <fcntl.h>
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif

#define HPP_OSAL_API_Windows
#endif

