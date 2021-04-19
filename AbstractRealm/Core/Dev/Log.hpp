#pragma once


#include "LAL/LAL.hpp"
//#include "OSAL/Timing.hpp"


namespace Dev
{
	using namespace LAL;


	enum class Severity
	{
		Verbose,
		Info,
		Warning,
		Error
	};


	// Add Editor Log Output

	class Log
	{
	public:

		struct RecordEntry
		{
			RecordEntry(Severity _severity, String _category, String _message);

			static uDM indexCounter;

			uDM          index;
			CalendarDate date;
			Severity     severity;
			String       category;
			String       message;
		};

		Log();

		Log(String _name);

		void Init(String _name);

		void Record(Severity _severity, String _message) const;

		static void Queue_DebugUI();

		static void GlobalRecord(Severity _severity, String message);

		static void Record_EditorDevDebugUI();

	protected:

		using SubRecords = DynamicArray< ptr<RecordEntry>>;

		String name;

		ptr<SubRecords> subRecordsRef;

		static UnorderedMap<String, SubRecords> subLogs; 

		static DynamicArray< UPtr<RecordEntry> > records;
	};
}
