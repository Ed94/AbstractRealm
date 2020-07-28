/*
	Basic File Input/Output


	This is a temporary implementation of file I/O for early file handling by the engine.


*/



#pragma once



#include "LAL/LAL.hpp"



namespace IO
{
	using namespace LAL;

	using Path = String;

	using FileBuffer = DynamicArray<char>;

	enum class EOpenFlag : std::ios::openmode
	{
		SeekToEOF_BeforeWrite = std::ios::app,
		BinaryMode            = std::ios::binary,
		ForInput              = std::ios::in,
		ForOutput             = std::ios::out,
		DiscardStreamContents = std::ios::trunc,
		SeekToEOF             = std::ios::ate
	};

	SpecifyBitmaskable(EOpenFlag);

	using OpenFlags = 
		Bitmask<EOpenFlag, std::ios::openmode>;
		
		//LAL::Bitmask<EOpenFlag, std::ios::openmode>;


	constexpr int FileStream_Beginning = 0;


	/*
	Buffers the file to a FileBuffer type.
	*/
	FileBuffer BufferFile(const Path& _fileToBuffer);
}
