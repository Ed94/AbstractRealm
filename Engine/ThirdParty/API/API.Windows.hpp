#ifndef HPP_API_Windows

#ifdef	_WIN64
#		define M_OS		M_OS_Windows
#		include <vcruntime_string.h>
		// Required for compiler intrinsics (memcpy)

	using uw	=	unsigned	__int64;
	using sw 	= 				__int64;
	using siPtr	=				__int64;
	using uiPtr	=	unsigned	__int64;

	static_assert(sizeof(uw) 	== sizeof(sw),		"sizeof(uw) != sizeof(sw)");
	static_assert(sizeof(uiPtr) == sizeof(siPtr),	"sizeof(uiPtr) != sizeof(siPtr)");

	namespace OSAL
	{
		constexpr sw 
		DefaultAlignment = (2 * sizeof(void*));
	}
#else
	static_assert(false, "OSAL does not support non-64-bit platforms.")
#endif

#define HPP_API_Windows
#endif