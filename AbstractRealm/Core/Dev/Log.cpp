// Parent
#include "Log.hpp"



namespace Dev
{
	Log::Log(String _name) :
		name(_name)
	{
		SubRecords newSubRecord;

		subLogs.insert( { name, move(newSubRecord) } );

		subRecordsRef = getPtr(subLogs.at(name));

		Record(Severity::Info, String("Created Subrecords Log: ") + name);
	}

	void Log::Record(Severity _severity, String& _message) const
	{
		records.push_back(RecordEntry(_severity, _message));

		subRecordsRef->push_back(getPtr(records.back()));
	}

	void Log::GlobalRecord(Severity _severity, String& _message)
	{
		records.push_back(RecordEntry(_severity, _message));
	}

	UnorderedMap<String, Log::SubRecords> Log::subLogs;

	DynamicArray< Log::RecordEntry> Log::records;
}