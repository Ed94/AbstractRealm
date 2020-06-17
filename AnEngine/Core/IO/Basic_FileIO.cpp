// Parent Header
#include "Basic_FileIO.hpp"



namespace IO
{
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