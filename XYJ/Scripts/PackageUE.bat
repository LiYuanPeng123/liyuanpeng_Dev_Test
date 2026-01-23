@echo off
setlocal
set SCRIPT_DIR=%~dp0
set PROJECT=%SCRIPT_DIR%..\XYJ.uproject
set PLATFORM=%1
if "%PLATFORM%"=="" set PLATFORM=Win64
set CONFIG=%2
if "%CONFIG%"=="" set CONFIG=DebugGame

REM Try to find UE_ROOT
if not "%UE_ROOT%"=="" goto CheckUAT
if not "%UE5_ROOT%"=="" set UE_ROOT=%UE5_ROOT%
if not "%UE_ROOT%"=="" goto CheckUAT
set UE_ROOT=C:\Program Files\Epic Games\UE_5.6

:CheckUAT
set UAT="%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat"
if not exist %UAT% (
  echo Error: RunUAT.bat not found at %UE_ROOT%
  echo Please set UE_ROOT or UE5_ROOT environment variable to your Unreal Engine installation.
  exit /b 1
)

for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set datetime=%%I
set TIMESTAMP=%datetime:~0,8%_%datetime:~8,6%

set OUTPUT_DIR=%SCRIPT_DIR%..\..\Builds\%PLATFORM%\%CONFIG%\%TIMESTAMP%
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

echo Packaging %PROJECT% for %PLATFORM% %CONFIG%...
echo Output: %OUTPUT_DIR%
echo UE Root: %UE_ROOT%

%UAT% BuildCookRun -project="%PROJECT%" -noP4 -platform=%PLATFORM% -clientconfig=%CONFIG% -cook -build -stage -pak -archive -archivedirectory="%OUTPUT_DIR%" -prereqs -unattended -utf8output -targetplatform=%PLATFORM%

exit /b %errorlevel%
