@echo off
cls
echo vvvvvvvvvv-BUILD-LOG-vvvvvvvvvv
c++ cdl.cpp -o cdl.exe -std=c++17
echo ^^^^^^^^^^^^^^^^^^^^-BUILD-LOG-^^^^^^^^^^^^^^^^^^^^
echo COMPILATION ERRORLEVEL: %errorlevel%
if %errorlevel% EQU 0 (
echo COMPILATION COMPLETE: RUNNING cdl.exe
REM start main.exe
start cmd /k "cdl.exe"
)
if %errorlevel% NEQ 0 (
echo ERROR DURING COMPILATION
)