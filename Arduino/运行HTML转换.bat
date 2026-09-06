@echo off
"D:\conda_envs\claude_env\python.exe" "%~dp0html_to_esp32.py"
set "exit_code=%errorlevel%"
echo.
pause
exit /b %exit_code%
