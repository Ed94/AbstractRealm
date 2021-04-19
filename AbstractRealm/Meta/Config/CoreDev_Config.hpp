#pragma once







namespace Meta
{
	/**
	 * See: https://twitter.com/TimSweeneyEpic/status/1223077404660371456?s=20.
	 * 
	 * For later use, it seems that with the way debugging works, there is a significant overhead when it comes to
	 * runtime debug and the exception scope handling.
	 * 
	 * This would make using try-catch exceptions optional and there would be an alternative implementation...
	 */
	constexpr bool UseCpp_Exceptions = true;

	constexpr bool Enable_HeapTracking = true;

	// Logging

	enum class ELogToFileMode
	{
		Default	  ,   // Use default behavior specified by the log to file attributes in the Log class.
		GlobalOnly,   // Only output the DevLog_#Data file.
		Full          // Create a directory named by date, have a file for global and sub-logs.
	};

	constexpr bool LogToFile = true;

	constexpr ELogToFileMode LogToFile_Mode = ELogToFileMode::GlobalOnly;

	constexpr bool Dump_EngineStateJson_OnCrash = true;
}
