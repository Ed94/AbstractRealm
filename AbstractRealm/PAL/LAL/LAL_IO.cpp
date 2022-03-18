// Parent Header
#include "LAL_IO.hpp"


namespace LAL
{
	bool Check_PathExists(const Path& _path)
	{
		return STL::filesystem::exists(_path);
	}

	bool Create_Directory(const Path& _path)
	{
		return STL::filesystem::create_directory(_path);
	}

	bool Create_Directories(const Path& _path)
	{
		return STL::filesystem::create_directories(_path);
	}
}