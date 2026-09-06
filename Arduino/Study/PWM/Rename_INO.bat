@echo off
setlocal EnableExtensions DisableDelayedExpansion

set "scriptDir=%~dp0"
for %%D in ("%scriptDir%.") do set "sketchName=%%~nxD"
set "targetName=%sketchName%.ino"

if exist "%scriptDir%%targetName%" (
    echo The sketch name is already correct: "%targetName%"
    goto success
)

set "inoCount=0"
set "sourceName="
for /f "delims=" %%F in ('dir /b /a-d "%scriptDir%*.ino" 2^>nul') do (
    set /a inoCount+=1
    set "sourceName=%%F"
)

if "%inoCount%"=="0" (
    echo ERROR: No .ino file was found in:
    echo "%scriptDir%"
    goto failure
)

if not "%inoCount%"=="1" (
    echo ERROR: Found %inoCount% .ino files. No files were renamed.
    echo Keep exactly one .ino file in this folder, then run this script again.
    goto failure
)

ren "%scriptDir%%sourceName%" "%targetName%"
if errorlevel 1 (
    echo ERROR: Could not rename "%sourceName%" to "%targetName%".
    goto failure
)

echo Renamed "%sourceName%" to "%targetName%" successfully.

:success
echo.
echo Press any key to close this window...
pause >nul
exit /b 0

:failure
echo.
echo Press any key to close this window...
pause >nul
exit /b 1
