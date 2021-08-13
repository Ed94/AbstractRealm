// Parent
#include "Log.hpp"


#include "Console.hpp"
#include "EngineInfo.hpp"
#include "SAL_ImGui.hpp"
#include "OSAL/OSAL_Timing.hpp"



namespace Dev
{
	uDM Logger::RecordEntry::indexCounter = 0;

	UnorderedMap<String, Logger::SubRecords> Logger::subLogs;

	DynamicArray< UPtr<Logger::RecordEntry>> Logger::records;


	Logger::RecordEntry::RecordEntry(ESeverity _severity, String _category, String _message) :
		index(indexCounter++), 
		date(OSAL::GetTime_Local() ), 
		severity(_severity), 
		category(_category), 
		message(_message)
	{}


	Logger::Logger(String _name) :
		name(_name)
	{}

	void Logger::Init()
	{
		SubRecords newSubRecord;

		subLogs.insert({ name, move(newSubRecord) });

		subRecordsRef = getPtr(subLogs.at(name));

		Record(ESeverity::Info, String("Created Subrecords Log: ") + name);
	}

	void Logger::Record(ESeverity _severity, String _message) const
	{
		records.push_back(MakeUPtr<RecordEntry>(_severity, name, _message));

		subRecordsRef->push_back(records.back().get());

		switch (_severity)
		{
			case ESeverity::Info:
			{
				CLog(name + ": " + _message);

			} break;

			case ESeverity::Error:
			{
				CLog_Error(name + ": " + _message);

			} break;
		}
	}

	void Logger::GlobalRecord(ESeverity _severity, String _message)
	{
		records.push_back(MakeUPtr<RecordEntry>(_severity, "Global", _message));

		switch (_severity)
		{
			case ESeverity::Info:
			{
				CLog(_message);

			} break;

			case ESeverity::Error:
			{
				CLog_Error(_message);

			} break;
		}
	}

	void Logger::Queue_DebugUI()
	{
		SAL::Imgui::Queue("Dev Log", Logger::Record_EditorDevDebugUI);
	}


	void Logger::Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;

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

					dateSig << "[" << PutTime(&record->date, "%F %I:%M:%S %p") << "] ";

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

						dateSig << "[" << PutTime(&record->date, "%F %I:%M:%S %p") << "] ";

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