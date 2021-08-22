/*


*/


#pragma once


#include "LAL_Cpp_STL.hpp"


namespace LAL
{
	using File_InputStream  = STL::ifstream;
	using File_OutputStream = STL::ofstream;

	using OStream       = STL::ostream      ;
	using OStringStream = STL::ostringstream;

	using Path = STL::filesystem::path;



	using STL::cerr;
	using STL::cout;
	using STL::endl;

	using STL::setfill;
	using STL::setw   ;



	bool CheckPathExists(const Path& _path);

	bool Create_Directory  (const Path& _path);
	bool Create_Directories(const Path& _path);
}
