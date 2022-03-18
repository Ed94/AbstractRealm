#ifndef HPP_API_Windows

#ifdef _WIN32
#		define M_OS		M_OS_Windows
#endif

#ifdef	_WIN64
	using uw	=	unsigned	__int64;
	using sw 	= 				__int64;
	using siPtr	=				__int64;
	using uiPtr	=	unsigned	__int64;

	static_assert(sizeof(uw) 	== sizeof(sw),		"sizeof(uw) != sizeof(sw)");
	static_assert(sizeof(uiPtr) == sizeof(siPtr),	"sizeof(uiPtr) != sizeof(siPtr)");
#else
	static_assert(false, "OSAL does not support non-64-bit platforms.")
#endif

#define HPP_API_Windows
#endif