@echo off
SET dict=%~dp0
if not exist "%dict%\..\..\source\bin" mkdir  "%dict%\..\..\source\bin"

if "%VS170COMNTOOLS%" == "" (
  msg "%username%" "Visual Studio 17 not detected"
  exit 1
)
if not exist "RD.sln" (
  call make-solutions.bat
)
goto start
if exist "RD.sln" (
  call "%VS170COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsall.bat"
  MSBuild /property:Configuration="Release" RD.sln
  MSBuild /property:Configuration="Debug" RD.sln
)
:start