/*
	Basic File Input/Output


	This is a temporary implementation of file I/O for early file handling by the engine.
*/


#pragma once


#include "LAL/LAL.hpp"
#include "Exceptions/Exceptions.hpp"


namespace IO
{
	using namespace LAL;

	using FileBuffer = DynamicArray<char>;


	// Enums and Masks

	enum class EOpenFlag : STL::ios::openmode
	{
		SeekToEOF_BeforeWrite = STL::ios::app   ,
		BinaryMode            = STL::ios::binary,
		ForInput              = STL::ios::in    ,
		ForOutput             = STL::ios::out   ,
		DiscardStreamContents = STL::ios::trunc ,
		SeekToEOF             = STL::ios::ate   ,

		SpecifyBitmaskable = sizeof(STL::ios::openmode)
	};

	using OpenFlags = 
		Bitfield<EOpenFlag, STL::ios::openmode>;


	// Compile-Time
		
	constexpr int FileStream_Beginning = 0;


	// Functions

	/*
	Buffers the file to a FileBuffer type.
	*/
	FileBuffer BufferFile(const Path& _fileToBuffer);

	bool OpenFile(File_OutputStream& _fileStream_in, OpenFlags _flags, const Path& _file);
	bool OpenFile(File_InputStream&  _fileStream_in, OpenFlags _flags, const Path& _file);
}
