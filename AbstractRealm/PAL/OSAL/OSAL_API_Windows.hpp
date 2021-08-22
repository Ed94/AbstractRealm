/*
OSAL: Windows API
*/


#pragma once


// Windows
#ifdef _WIN32

#ifndef WIN_HEADER_GUARD
#define WIN_HEADER_GUARD


		#define NOMINMAX              // Prevents the numeric limits error.
		#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers

		#include <Windows.h>

// Networking
		#include <WinSock2.h>
		#include <WS2tcpip.h>

// IO
		#include <corecrt_io.h>
		#include <fcntl.h>

#endif

#endif
