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
	using STL::cin;
	using STL::cout;
	using STL::endl;
	using STL::wcerr;
	using STL::wcin;
	using STL::wcout;

	inline 
	bool SyncWith_STDIO(bool _newSync)
	{
		return STL::ios::sync_with_stdio(_newSync);
	}

	template <class ElemType>
	_NODISCARD 
	STL::_Fillobj<ElemType> SetFill(ElemType _Ch)
	{
		return STL::_Fillobj<ElemType>(_Ch);
	}

	constexpr auto SetWidth = STL::setw;

	bool CheckPathExists(const Path& _path);

	bool Create_Directory  (const Path& _path);
	bool Create_Directories(const Path& _path);
}
