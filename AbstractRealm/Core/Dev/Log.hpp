#pragma once


#include "LAL/LAL.hpp"


namespace Dev
{
	using namespace LAL;


	enum class ESeverity
	{
		Verbose,
		Info,
		Warning,
		Error
	};

	//TODO: If deemed viable... (Could be if using multiple logger types in the future, or want to have ptr refs to them).
	class ALogger
	{};

	class Logger
	{
	public:

		struct RecordEntry
		{
			RecordEntry(ESeverity _severity, String _category, String _message);

			static uDM indexCounter;

			uDM          index;
			CalendarDate date;
			ESeverity    severity;
			String       category;
			String       message;
		};

		Logger(String _name);

		void Init();

		void Record(ESeverity _severity, String _message) const;

		static void Queue_DebugUI();

		static void GlobalRecord(ESeverity _severity, String message);

		static void Record_EditorDevDebugUI();

	protected:

		using SubRecords = DynamicArray< ptr<RecordEntry>>;

		String name;

		ptr<SubRecords> subRecordsRef;

		static UnorderedMap<String, SubRecords> subLogs; 

		static DynamicArray< UPtr<RecordEntry> > records;
	};

	#define Dev_Declare_LogAPI()\
	void Log        (String _message, Dev::ESeverity _severity = Dev::ESeverity::Info);\
	void Log_Verbose(String _message);\
	void Log_Warning(String _message);\
	void Log_Error  (String _message);

	#define Dev_Declare_Log(_Name)\
	Dev::Logger SubLogger(#_Name);\
	\
	void Log(String _message, Dev::ESeverity _severity)\
	{\
		SubLogger.Record(_severity, _message);\
	}\
	\
	void Log_Verbose(String _message)\
	{\
		SubLogger.Record(Dev::ESeverity::Verbose, _message);\
	}\
	\
	void Log_Warning(String _message)\
	{\
		SubLogger.Record(Dev::ESeverity::Warning, _message);\
	}\
	\
	void Log_Error(String _message)\
	{\
		SubLogger.Record(Dev::ESeverity::Error, _message);\
	} 

	#define Dev_UseLog(_Module)\
	using _Module::Log;\
	using _Module::Log_Verbose;\
	using _Module::Log_Warning;\
	using _Module::Log_Error;
}
