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
	"/I", $Path_API,
	"/I", $Path_Core,
	"/I", $PATH_PAL,
	"/I", $Path_PAL_LAL,
	"/I", $Path_PAL_OSAL,
	
	"/I", ($Path_ThirdParty + "infoware\include\")
)

$ModuleSearchDirs = $(
	"/ifcSearchDir", $Path_API,
	"/ifcSearchDir", $Path_Core,c
	"/ifcSearchDir", $PATH_PAL,
	"/ifcSearchDir", $Path_PAL_LAL,
	"/ifcSearchDir", $Path_PAL_OSAL
)

$Modules = @(
	$Path_PAL_OSAL + "OSAL.Entrypoint.ixx"
)

$TranslationUnits = @( 
#	Cold Units : bundle
	$Path_Engine + "Build\Build.Cold.cpp"

# 	Hot Units : individual
	$Path_API	+ "API.Windows.Backend.cpp"
	$Path_Core	+ "Entrypoint.cpp"
)

# Write-Output "Include Paths:"
# foreach($Path in $Includes)
# {
# 	if ($Path -ne "/I")
# 	{
# 		Write-Output $Path
# 	}
# }

$CompilerFlags = @(
	"/nologo",						#	
	"/EHa",							#
	"/Fe: AbstractRealm.exe",		# Exectuable Name
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

$Libraries_ThirdParty = @(
	"kernel32.lib",
	"user32.lib",
	"gdi32.lib",
	"winspool.lib",
	"comdlg32.lib",
	"advapi32.lib",
	"shell32.lib",
	"ole32.lib",
	"oleaut32.lib",
	"uuid.lib",
	"odbc32.lib",
	"odbccp32.lib",
	"Wbemuuid.lib",
	"version.lib",

	($Path_ThirdParty + "infoware\out\build\x64-debug\lib\infowared.lib")
)

#endregion Context Vars

#region Compiler

cmd.exe /c cl $CompilerFlags $Includes $Modules $ModuleSearchDirs $TranslationUnits -link $LinkerFlags $Libraries_ThirdParty

#endregion Compiler

Pop-Location
