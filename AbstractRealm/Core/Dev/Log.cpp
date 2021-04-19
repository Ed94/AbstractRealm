// Parent
#include "Log.hpp"


#include "Console.hpp"
#include "EngineInfo.hpp"
#include "ImGui_SAL.hpp"
#include "OSAL/OSAL_Timing.hpp"



namespace Dev
{
	uDM Log::RecordEntry::indexCounter = 0;

	UnorderedMap<String, Log::SubRecords> Log::subLogs;

	DynamicArray< UPtr<Log::RecordEntry>> Log::records;


	Log::RecordEntry::RecordEntry(Severity _severity, String _category, String _message) :
		index(indexCounter++), 
		date(OSAL::GetTime_Local() ), 
		severity(_severity), 
		category(_category), 
		message(_message)
	{}


	Log::Log()
	{
	}

	Log::Log(String _name) :
		name(_name)
	{
		SubRecords newSubRecord;

		subLogs.insert( { name, move(newSubRecord) } );

		subRecordsRef = getPtr(subLogs.at(name));

		Record(Severity::Info, String("Created Subrecords Log: ") + name);
	}

	void Log::Init(String _name)
	{
		name = _name;

		SubRecords newSubRecord;

		subLogs.insert({ name, move(newSubRecord) });

		subRecordsRef = getPtr(subLogs.at(name));

		Record(Severity::Info, String("Created Subrecords Log: ") + name);
	}

	void Log::Record(Severity _severity, String _message) const
	{
		records.push_back(MakeUPtr<RecordEntry>(_severity, name, _message));

		subRecordsRef->push_back(records.back().get());

		switch (_severity)
		{
			case Severity::Info:
			{
				CLog(name + ": " + _message);

			} break;

			case Severity::Error:
			{
				CLog_Error(name + ": " + _message);

			} break;
		}
	}

	void Log::GlobalRecord(Severity _severity, String _message)
	{
		records.push_back(MakeUPtr<RecordEntry>(_severity, "Global", _message));

		switch (_severity)
		{
			case Severity::Info:
			{
				CLog(_message);

			} break;

			case Severity::Error:
			{
				CLog_Error(_message);

			} break;
		}
	}

	void Log::Queue_DebugUI()
	{
		SAL::Imgui::Queue("Dev Log", Log::Record_EditorDevDebugUI);
	}


	void Log::Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;
		using namespace LAL;

		#define Args(_Entry) NameOf(_Entry).str(), _Entry


		if (ImGui::BeginTabBar("Log Tabs"))
		{
			ImVec2 scrolling_Size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 10);

			static StringStream dateSig; 
			
			if (ImGui::BeginTabItem("Global"))
			{
				BeginChild("Global Log", ImVec2(), true, ImGuiWindowFlags_HorizontalScrollbar);
				
				for (auto& record : records)
				{
					dateSig.str(String());

					dateSig << "[" << put_time(&record->date, "%F %I:%M:%S %p") << "] ";

					Text(String
					(
						ToString(record->index) + " " +
						dateSig.str() + " " +
						nameOf(record->severity).data()) + ": " + 
						record->category + ": " + 
						record->message
					);
				}

				EndChild();

				ImGui::EndTabItem();
			}

			for (auto& sublog : subLogs)
			{
				if (ImGui::BeginTabItem(sublog.first.c_str()))
				{
					BeginChild("Global Log", ImVec2(), true, ImGuiWindowFlags_HorizontalScrollbar);

					for (auto& record : sublog.second)
					{
						dateSig.str(String());

						dateSig << "[" << put_time(&record->date, "%F %I:%M:%S %p") << "] ";

						Text(String
						(
							 ToString(record->index) + " " +
							 dateSig.str() + " " +
							 nameOf(record->severity).data()) + ": " + 
							 record->message
						);
					}

					EndChild();

					ImGui::EndTabItem();
				}
			}

			ImGui::EndTabBar();
		}
	}
}