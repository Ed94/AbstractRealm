/*
Explicit Specifiers


*/



#pragma once



// Statics:

// Creates a static duration variable only accessible to the file. (Global scope/File scope only)
#define sInternal \
static

// Creates a static duration variable accessible to any file or linkage. (Global/File scope Only)
#define eGlobal \
extern

// Defines a static member function or variable that is not bound to an instance of a class.
#define unbound \
static


// Memeber Specialization

// Alias for virtual in the context of a derived class implementing a virtual member.
#define implem \
virtual


// Inlines

// Alias for an inline variable that is supposed to have a constant value, but exist in multiple definitions.
#define multiDefs \
inline


// Namespaces

// Used in context with a namespace declaration to signify type_as the latest. (If using a versioning scheme for namespacing.
#define latest \
inline


// Structs

//Plain Old Data type
#define POD \
struct

// Bit-field type
#define BitField \
struct

