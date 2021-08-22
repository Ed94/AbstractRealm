/*
Explicit Specifiers


*/



#pragma once


#include "Meta/Config/CoreDev_Config.hpp"


// Statics:

// Creates a static duration variable only accessible to the file. (Global scope/File scope only)
#define sInternal \
static

// Creates a static duration variable accessible to any file or linkage. (Global/File scope Only)
#define eGlobal \
extern

//// Creates a static duration stack variable.
//#define sGlobal \
//static

// Defines a static member function or variable that is not bound to an instance of a class, or a static duration variable.
//#define unbound \
//static


// Member Specialization

// Alias for virtual in the context of a derived class implementing a virtual member.
#define implem \
virtual


// Inlines

// Alias for an inline variable that is supposed to have a constant value, but exist in multiple definitions.
#define multiDefs \
inline

// Alias for a constexpr intended to be used as an inline expansion.
#define constInline 

#ifdef						Meta_ForceInlineMode_EngineDiscretion

	// Standard force inline define. See: https://en.wikipedia.org/wiki/Inline_function
	#ifdef _MSC_VER
							#define ForceInline __forceinline
	#elif defined(__GNUC__)
							#define ForceInline inline __attribute__((__always_inline__))
	#elif defined(__CLANG__)
	#if __has_attribute(__always_inline__)
							#define ForceInline inline __attribute__((__always_inline__))
	#else
							#define ForceInline inline
	#endif
	#else
							#define ForceInline inline
	#endif
#else
							// Using compiler discretion.
							#define ForceInline inline
#endif


// Namespaces

// Used in context with a namespace declaration to signify type_as the latest. (If using a versioning scheme for namespacing.
#define latest \
inline


// Structs

//Plain Old Data type
//#define POD \
//struct
template<typename Type> struct POD;

#define EnforceConstraint(__CONSTRAINT, __REASON_MSG) \
static_assert(__CONSTRAINT, __REASON_MSG)

