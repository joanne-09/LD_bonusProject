@echo off
REM run.bat
set "par1=%~1"
if "%par1%" == ""( "par1=0" )
set executable=main.exe
set input_path=../testcases/case02.in
set output_path=../output/case02.out


REM Run the main executable with the provided arguments
@echo on
main.exe ../testcases/case0%par1%.in ../output/case0%par1%.out
pause
