# WIP build script for Abstract Realm
# Can be used as a template for creating builds without a full build system

function Gen-ArgStr($Arguments)
{
	$result = ""
	for ( $index = 0; $index -lt $Arguments.count; $index++)
	{
		if ($index -eq ($Arguments.count - 1))
		{
			$result += $Arguments[$index]
			break
		}

		$result += $Arguments[$index] + " "
	}

	return $result;
}

# Setup the compiler
cmd.exe /c "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
# cmd.exe /c "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

# Setup build output path.
if (-not (Test-Path "Windows"))
{
	New-Item . -Name "Windows" -ItemType "directory"
	Write-Output "Created Windows directory"
}

Push-Location -Path "Windows"

#region Context Vars

$Debug = $true

$Path_Engine_Dir = (get-item (Get-Location)).parent.parent
$Path_Engine = $Path_Engine_Dir.FullName + "\"

$Path_Core			= $Path_Engine		+ "Core\"
$Path_PAL			= $Path_Engine 		+ "PAL\"
$Path_PAL_LAL		= $Path_PAL 		+ "LAL\"
$Path_PAL_OSAL		= $Path_PAL 		+ "OSAL\"
$Path_ThirdParty	= $Path_Engine 		+ "ThirdParty\"
$Path_API			= $Path_ThirdParty	+ "API\"

$Includes = $(
	"/I", "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\include",
	"/I", "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared",
	"/I", "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt",
	"/I", "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um",

	"/I", $Path_API,
	"/I", $Path_Core,
	"/I", $PATH_PAL,
	"/I", $Path_PAL_LAL,
	"/I", $Path_PAL_OSAL,
	
	"/I", ($Path_ThirdParty + "infoware\include\")
)

$ModuleSearchDirs = $(
	"/ifcSearchDir", $Path_API,
	"/ifcSearchDir", $Path_Core,
	"/ifcSearchDir", $PATH_PAL,
	"/ifcSearchDir", $Path_PAL_LAL,
	"/ifcSearchDir", $Path_PAL_OSAL
)

$Modules = @(
	($Path_PAL_OSAL + "OSAL.Entrypoint.ixx"),
	($Path_PAL_OSAL + "OSAL.Platform.ixx")
)

$TranslationUnits = @( 
#	Cold Units : bundle
	$Path_Engine + "Build\Build.Cold.cpp"

# 	Hot Units : individual
	$Path_API	+ "API.Windows.Backend.cpp"
	$Path_Core	+ "Entrypoint.cpp"
)

$CompilerFlags = @(
	"/nologo",						#	
	"/EHa",							#
	"/Fe:AbstractRealm.exe",		# Exectuable Name
	# "/GR",						# Deprecated
	"/GS-",							#
	"/Gs9999999",					#
	"/MDd"							#
	# "/MP"							# Multi-Processor Build
	"/Oi",						#
	"/std:c++20"					# Language Version
	"/W4",							# Warning Level
	"/Zi"							# Debug
)

$LinkerFlags = @(
	# "/CGTHREADS:8"					# Max Threads for /MP
	"/subsystem:windows"
	# "-nodefaultlib"
	"/stack:0x100000,0x100000"
)

# $LibraryIncludes = @(
# 	# "/LIBPATH",  "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64"
# )

$Path_WinKit_um64 = "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\"
$Path_WinKit_ucrt = "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\ucrt\x64\"
$Path_MSVC_Lib64 = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\lib\x64\"

$Libraries_ThirdParty = @(
	($Path_WinKit_um64 + "kernel32.Lib"),
	($Path_WinKit_um64 + "user32.Lib"),
	($Path_WinKit_um64 + "gdi32.Lib"),
	($Path_WinKit_um64 + "winspool.lib"),
	($Path_WinKit_um64 + "comdlg32.lib"),
	($Path_WinKit_um64 + "advapi32.lib"),
	($Path_WinKit_um64 + "shell32.lib"),
	($Path_WinKit_um64 + "ole32.lib"),
	($Path_WinKit_um64 + "oleaut32.lib"),
	($Path_WinKit_um64 + "uuid.lib"),
	($Path_WinKit_um64 + "odbc32.lib"),
	($Path_WinKit_um64 + "odbccp32.lib"),
	($Path_WinKit_um64 + "Wbemuuid.lib"),
	($Path_WinKit_um64 + "version.lib"),

	($Path_MSVC_Lib64 + "msvcrtd.lib"),
	($Path_MSVC_Lib64 + "oldnames.lib"),
	($Path_MSVC_Lib64 + "msvcprtd.lib"),
	($Path_MSVC_Lib64 + "vcruntimed.lib"),

	($Path_WinKit_ucrt + "ucrtd.lib"),

	($Path_ThirdParty + "infoware\out\build\x64-debug\lib\infowared.lib")
)

#endregion Context Vars

#region Compiler

cmd.exe /c cl $CompilerFlags $Includes $Modules $ModuleSearchDirs $TranslationUnits -link $LinkerFlags $Libraries_ThirdParty

#endregion Compiler

Pop-Location

#Read-Host -Prompt "Press Enter to exit"