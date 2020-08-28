/*


*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	using File_InputStream  = std::ifstream;
	using File_OutputStream = std::ofstream;

	using OStream       = std::ostream      ;
	using OStringStream = std::ostringstream;

	using Path = std::filesystem::path;



	using std::cerr;
	using std::cout;
	using std::endl;

	using std::setfill;
	using std::setw   ;



	bool CheckPathExists(const Path& _path);

	bool Create_Directory  (const Path& _path);
	bool Create_Directories(const Path& _path);
}
