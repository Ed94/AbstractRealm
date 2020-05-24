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

	using OpenFlags = LAL::Bitmask<EOpenFlag, std::ios::openmode>;


	constexpr int FileStream_Beginning = 0;


	/*
	Buffers the file to a FileBuffer type.
	*/
	FileBuffer BufferFile(const Path& _fileToBuffer)
	{
		OpenFlags flags;

		flags.Set(EOpenFlag::SeekToEOF, EOpenFlag::BinaryMode);

		File_InputStream fileStream(_fileToBuffer, flags);

		if (! fileStream.is_open())
		{
			throw RuntimeError("BufferFile: Failed to open file at: " + _fileToBuffer);
		}

		// TellG: Get position in input sequence 
		// Returns the position of the current character in the input stream.

		// The advantage of starting to read at the end of the file is that we can use the read position to determine the size of the file and allocate a buffer:
		DataSize streamSize = DataSize(fileStream.tellg());

		FileBuffer bufferToReturn(streamSize);

		fileStream.seekg(FileStream_Beginning);

		fileStream.read(bufferToReturn.data(), streamSize);

		fileStream.close();

		return bufferToReturn;
	}
}
