#pragma once


#include "LAL/LAL.hpp"


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


	class Log
	{
	public:

		struct RecordEntry
		{
			RecordEntry(Severity _severity, String _message) :
				severity(_severity), message(_message)
			{}

			Severity severity;
			String    message;
		};

		Log(String _name);

		void Record(Severity _severity, String& _message) const;

		static void GlobalRecord(Severity _severity, String& message);

	protected:

		using SubRecords = DynamicArray< ptr<RecordEntry>>;

		String name;

		ptr<SubRecords> subRecordsRef;

		static UnorderedMap<String, SubRecords> subLogs; 

		static DynamicArray< RecordEntry > records;
	};
}
