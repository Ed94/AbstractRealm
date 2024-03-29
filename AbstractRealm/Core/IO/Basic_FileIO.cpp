// Parent Header
#include "Basic_FileIO.hpp"


namespace IO
{
	// Public

	FileBuffer BufferFile(const Path& _fileToBuffer)
	{
		OpenFlags flags;

		flags.Set(EOpenFlag::SeekToEOF, EOpenFlag::BinaryMode);

		File_InputStream fileStream(_fileToBuffer, flags); 

		if (! fileStream.is_open())
		{
			Exception::Fatal::Throw("BufferFile: Failed to open file at: " + _fileToBuffer.generic_string());
		}

		// TellG: Get position in input sequence 
		// Returns the position of the current character in the input stream.

		// The advantage of starting to read at the end of the file is that we can use the read position to determine the size of the file and allocate a buffer:
		uDM streamSize = uDM(fileStream.tellg());

		FileBuffer bufferToReturn(streamSize);

		fileStream.seekg(FileStream_Beginning);

		fileStream.read(bufferToReturn.data(), streamSize);

		fileStream.close(); 

		return bufferToReturn;
	}

	bool OpenFile(File_OutputStream& _fileStream_in, OpenFlags _flags, const Path& _file)
	{
		_fileStream_in.open(_file, _flags);

		if (! _fileStream_in.is_open()) 
			return false;
		else
			return true;
	}

	bool OpenFile(File_InputStream& _fileStream_in, OpenFlags _flags, const Path& _file)
	{
		_fileStream_in.open(_file, _flags);

		if (!_fileStream_in.is_open())
			return false;
		else
			return true;
	}
}