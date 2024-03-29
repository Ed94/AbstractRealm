#pragma once


#include "Dev_Backend.hpp"


namespace Dev
{
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
		~Logger();

		static void GlobalInit();

		static void Queue_DebugUI();

		static void GlobalRecord(ESeverity _severity, String message);

		static void Record_EditorDevDebugUI();

		void Init();

		void Record(ESeverity _severity, String _message);

	protected:

		using SubRecords = DynamicArray< ptr<RecordEntry>>;

		File_OutputStream fileOut;

		String name;

		ptr<SubRecords> subRecordsRef;

		static File_OutputStream globalFileOut;

		static UnorderedMap<String, SubRecords> subLogs; 

		static DynamicArray< UPtr<RecordEntry> > records;
	};

	#define Dev_Declare_LogAPI()\
	void Log        (String _message, Dev::ESeverity _severity =  Dev::ESeverity::Info);\
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
