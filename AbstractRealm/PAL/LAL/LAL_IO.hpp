/*


*/

#pragma once


#include "LAL_Cpp_STL.hpp"
#include "LAL_Bits.hpp"
#include "LAL_Containers.hpp"


namespace LAL
{
	template<typename Type>
	using FileBuffer = DynamicArray<Type>;

	// Enums and Masks

	enum class EOpenFlag : STL::ios::openmode
	{
		SeekToEOF_BeforeWrite = STL::ios::app   ,
		BinaryMode            = STL::ios::binary,
		ForInput              = STL::ios::in    ,
		ForOutput             = STL::ios::out   ,
		DiscardStreamContents = STL::ios::trunc ,
		SeekToEOF             = STL::ios::ate   ,

		SpecifyBitmaskable = sizeof(STL::ios::openmode)
	};

	using OpenFlags = 
		Bitfield<EOpenFlag, STL::ios::openmode>;

	// Compile-Time

	constexpr int FileStream_Beginning = 0;

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

	bool Check_PathExists(const Path& _path);

	bool Create_Directory  (const Path& _path);
	bool Create_Directories(const Path& _path);
}
