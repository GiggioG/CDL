@echo off
cls
echo vvvvvvvvvv-BUILD-LOG-vvvvvvvvvv
g++ msdn.cpp -o msdn.exe -std=c++11
echo ^^^^^^^^^^^^^^^^^^^^-BUILD-LOG-^^^^^^^^^^^^^^^^^^^^
echo COMPILATION ERRORLEVEL: %errorlevel%
if %errorlevel% EQU 0 (
echo COMPILATION COMPLETE: RUNNING msdn.exe
REM start main.exe
start cmd /k "msdn.exe"
)
if %errorlevel% NEQ 0 (
echo ERROR DURING COMPILATION
)