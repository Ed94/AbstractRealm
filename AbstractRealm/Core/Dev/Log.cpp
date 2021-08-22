// Parent
#include "Log.hpp"


// PAL
#include "OSAL/OSAL_Timing.hpp"
#include "SAL_ImGui.hpp"
// Core
#include "IO/Basic_FileIO.hpp"
#include "Console.hpp"


namespace Dev
{
	File_OutputStream Logger::globalFileOut;

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

	Logger::~Logger()
	{
		FileOut.close();
	}

	void Logger::Init()
	{
		SubRecords newSubRecord;

		if (Meta::ShouldLogToFile() && Meta::LogToFile_Mode() == Meta::ELogToFileMode::Full)
		{
			StringStream dateStream, dateFull; 

			auto dateSnapshot = OSAL::GetExecutionStartDate();
			
			dateStream << PutTime(dateSnapshot, "%F");

			dateFull << dateStream.str() << PutTime(dateSnapshot, "_%I-%M-%S_%p");

			IO::OpenFile
			(
				FileOut,
				IO::OpenFlags(IO::EOpenFlag::ForOutput),
				Path(String(DevLogPath) + String("/") + dateStream.str() + "/" + name + "__" + dateFull.str() + ".txt")
			);

			if (FileOut.is_open())
			{
				using namespace Meta;

				FileOut << setfill('-') << setw(140) << '-' << endl;

				FileOut
					<< "Abstract Realm: MVP Build - "
					<< EEngineVersion::Major << "."
					<< EEngineVersion::Minor << "."
					<< EEngineVersion::Patch << "    "

					<< "Dev Log - " + name + ": " << PutTime(OSAL::GetExecutionStartDate(), "%F %I:%M:%S %p") << endl;

				FileOut << setfill('-') << setw(140) << '-' << endl;
			}
		}

		subLogs.insert({ name, move(newSubRecord) });

		subRecordsRef = getPtr(subLogs.at(name));

		Record(ESeverity::Info, String("Created Subrecords Log: ") + name);

		static StringStream dateSig;

		dateSig << PutTime(records.back()->date, "%F %I:%M:%S %p");
	}

	void Logger::Record(ESeverity _severity, String _message)
	{
		records.push_back(MakeUnique<RecordEntry>(_severity, name, _message));

		subRecordsRef->push_back(records.back().get());

		if (FileOut.is_open())
		{
			static StringStream dateSig; dateSig.str(String());

			dateSig << PutTime(records.back().get()->date, "%F %I:%M:%S %p");

			FileOut
				<< ToString(records.back()->index) << " | "
				<< dateSig.str() << " | "
				<< String(nameOf(_severity).data())
				<< String(": ")
				<< _message
				<< endl;

			globalFileOut
				<< ToString(records.back()->index) << " | "
				<< std::string(nameOf(_severity).data()) << " | "
				<< records.back()->category
				<< String(": ")
				<< _message
				<< endl;
		}

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

	void Logger::GlobalInit()
	{
		if (Meta::ShouldLogToFile())
		{
			StringStream dateStream, dateFull;

			auto dateSnapshot = OSAL::GetExecutionStartDate();

			dateStream << PutTime(dateSnapshot, "%F");

			dateFull << dateStream.str() << PutTime(dateSnapshot, "_%I-%M-%S_%p");

			switch (Meta::LogToFile_Mode())
			{
				case Meta::ELogToFileMode::FailureOnly:
				{
					break;
				} 

				case Meta::ELogToFileMode::Default:
				{
					break;
				} 

				case Meta::ELogToFileMode::GlobalOnly:
				{
					IO::OpenFile
					(
						globalFileOut,
						IO::OpenFlags(IO::EOpenFlag::ForOutput),
						Path(String(DevLogPath) + String("/") + String(DevLogName) + String("__") + dateStream.str() + String(".txt"))
					);

					break;
				}

				case Meta::ELogToFileMode::Full:
				{
					Create_Directory(String(DevLogPath) + String("/") + dateStream.str());

					IO::OpenFile
					(
						globalFileOut,
						IO::OpenFlags(IO::EOpenFlag::ForOutput),
						Path(String(DevLogPath) + "/" + dateStream.str() + "/" + "Global__" + dateFull.str() + ".txt")
					);

					break;
				}
			}

			if (globalFileOut.is_open())
			{
				using namespace Meta;

				globalFileOut << setfill('-') << setw(140) << '-' << endl;

				globalFileOut
					<< "Abstract Realm: MVP Build - "
					<< EEngineVersion::Major << "."
					<< EEngineVersion::Minor << "."
					<< EEngineVersion::Patch << "    "

					<< "Dev Log: " << PutTime(OSAL::GetExecutionStartDate(), "%F %I:%M:%S %p") << endl;

				globalFileOut << setfill('-') << setw(140) << '-' << endl;
			}
		}
	}

	void Logger::GlobalRecord(ESeverity _severity, String _message)
	{
		records.push_back(MakeUnique<RecordEntry>(_severity, "Global", _message));

		if (globalFileOut.is_open())
		{
			static StringStream dateSig;

			dateSig << PutTime(records.back()->date, "%F %I:%M:%S %p");

			globalFileOut
				<< ToString(records.back()->index) << " | "
				<< std::string(nameOf(_severity).data()) << " | "
				<< records.back()->category
				<< String(": ")
				<< _message
				<< endl;
		}

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
				
				if (Table5C::Record("Index", "Time", "Severity", "Category", "Message"))
				{
					for (auto& record : records)
					{
						dateSig.str(String());

						dateSig << PutTime(record->date, "%F %I:%M:%S %p");

						Table5C::Entry
						(
							record->index,
							dateSig.str(),
							record->severity,
							record->category,
							record->message
						);
					}

					Table5C::EndRecord();
				}
				
				EndChild();

				ImGui::EndTabItem();
			}

			for (auto& sublog : subLogs)
			{
				if (ImGui::BeginTabItem(sublog.first.c_str()))
				{
					BeginChild("Global Log", ImVec2(), true, ImGuiWindowFlags_HorizontalScrollbar);

					if (Table4C::Record("Index", "Time", "Severity", "Message"))
					{
						for (auto& record : sublog.second)
						{
							dateSig.str(String());

							dateSig << PutTime(record->date, "%F %I:%M:%S %p");

							Table4C::Entry
							(
								record->index,
								dateSig.str(),
								record->severity,
								record->message
							);
						}

						Table4C::EndRecord();
					}

					EndChild();

					ImGui::EndTabItem();
				}
			}

			ImGui::EndTabBar();
		}
	}
}