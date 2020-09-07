// Parent Header
#include "Basic_FileIO.hpp"



namespace IO
{
	// Public

	FileBuffer BufferFile(const Path& _fileToBuffer)
	{
		OpenFlags flags;

		flags.Set(EOpenFlag::SeekToEOF, EOpenFlag::BinaryMode);

		Heap(File_InputStream fileStream(_fileToBuffer, flags));

		if (! fileStream.is_open())
		{
			throw RuntimeError("BufferFile: Failed to open file at: " + _fileToBuffer.generic_string());
		}

		// TellG: Get position in input sequence 
		// Returns the position of the current character in the input stream.

		// The advantage of starting to read at the end of the file is that we can use the read position to determine the size of the file and allocate a buffer:
		WordSize streamSize = WordSize(fileStream.tellg());

		FileBuffer bufferToReturn(streamSize);

		fileStream.seekg(FileStream_Beginning);

		fileStream.read(bufferToReturn.data(), streamSize);

		Heap(fileStream.close());

		return bufferToReturn;
	}

	bool OpenFile(File_OutputStream& _fileStream, OpenFlags _flags, const Path& _file)
	{
		Heap(_fileStream.open(_file), _flags);

		if (! _fileStream.is_open()) 
			return false;
		else
			return true;
	}

	bool OpenFile(File_InputStream& _fileStream, OpenFlags _flags, const Path& _file)
	{
		Heap(_fileStream.open(_file), _flags);

		if (!_fileStream.is_open())
			return false;
		else
			return true;
	}
}