/*
	Basic File Input/Output


	This is a temporary implementation of file I/O for early file handling by the engine.
*/



#pragma once



// Engine
#include "LAL/LAL.hpp"
#include "Memory/MemTracking.hpp"



namespace Core::IO
{
	using namespace Core::Memory;

	

	// Usings

	using namespace LAL;

	using FileBuffer = DynamicArray<char>;
	//using Path       = String            ;



	// Enums and Masks

	enum class EOpenFlag : std::ios::openmode
	{
		SeekToEOF_BeforeWrite = std::ios::app   ,
		BinaryMode            = std::ios::binary,
		ForInput              = std::ios::in    ,
		ForOutput             = std::ios::out   ,
		DiscardStreamContents = std::ios::trunc ,
		SeekToEOF             = std::ios::ate   ,

		SpecifyBitmaskable = sizeof(std::ios::openmode)
	};

	

	using OpenFlags = 
		Bitmask<EOpenFlag, std::ios::openmode>;



	// Compile-Time
		
	constexpr int FileStream_Beginning = 0;



	// Functions

	/*
	Buffers the file to a FileBuffer type.
	*/
	FileBuffer BufferFile(const Path& _fileToBuffer);

	bool OpenFile(File_OutputStream& _fileStream, OpenFlags _flags, const Path& _file);

	bool OpenFile(File_InputStream&  _fileStream, OpenFlags _flags, const Path& _file);
}
