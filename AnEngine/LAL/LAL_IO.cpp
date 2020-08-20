// Parent Header
#include "LAL_IO.hpp"



namespace LAL
{
	bool CheckPathExists(const Path& _path)
	{
		return std::filesystem::exists(_path);
	}

	bool Create_Directory(const Path& _path)
	{
		return std::filesystem::create_directory(_path);
	}

	bool Create_Directories(const Path& _path)
	{
		return std::filesystem::create_directories(_path);
	}
}