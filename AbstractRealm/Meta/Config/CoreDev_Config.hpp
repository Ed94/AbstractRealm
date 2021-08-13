#pragma once







namespace Meta
{
	//#define Meta_UseCpp_Exceptions 

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

	#define Meta_ForceInlineMode_EngineDiscretion


	// Logging

	enum class ELogToFileMode
	{
		FailureOnly,   // Intended for a release build, will log only the failure that occurred.
		Default	   ,   // Use default behavior specified by the log to file attributes in the Log class.
		GlobalOnly ,   // Only output the DevLog_#Data file.
		Full           // Create a directory named by date, have a file for global and sub-logs.
	};



	// TODO: Make these runtime settable. (However whether or not they can be used or exist depends on build type.

	// Will not build any log to file functionality. (Could be used to optimize build size).
	constexpr bool LogToFile = true;

	// Can be loaded from config file, change affects next engine load.
	constexpr ELogToFileMode LogToFile_Mode = ELogToFileMode::GlobalOnly;

	// Should the engine dump its state to json if it crashes?
	constexpr bool Dump_EngineStateJson_OnCrash = false;
}
